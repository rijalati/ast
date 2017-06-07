/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1982-2014 AT&T Intellectual Property          *
*                      and is licensed under the                       *
*                 Eclipse Public License, Version 1.0                  *
*                    by AT&T Intellectual Property                     *
*                                                                      *
*                A copy of the License is available at                 *
*          http://www.eclipse.org/org/documents/epl-v10.html           *
*         (with md5 checksum b35adb5213ca9657e911e9befb180842)         *
*                                                                      *
*              Information and Software Systems Research               *
*                            AT&T Research                             *
*                           Florham Park NJ                            *
*                                                                      *
*                    David Korn <dgkorn@gmail.com>                     *
*                                                                      *
***********************************************************************/
#pragma prototyped
/*
 * cd [-LP@] [-f dirfd] [dirname]
 * cd [-LP] [-f dirfd] [old] [new]
 * pwd [-LP]
 *
 *   David Korn
 *   dgkorn@gmail.com
 *
 */

#include	"defs.h"
#include	<stak.h>
#include	<error.h>
#include	"variables.h"
#include	"path.h"
#include	"name.h"
#include	"builtins.h"
#include	<pwd.h>
#include	<ls.h>

/* Repeat syscall in expr each time it gets hit with EINTR */
#ifndef EINTR_REPEAT
#define EINTR_REPEAT(expr) while((expr) && (errno == EINTR)) errno=0;
#endif

/*
 * Invalidate path name bindings to relative paths
 */
static void invalidate(register Namval_t *np,void *data)
{
	Pathcomp_t *pp = (Pathcomp_t*)np->nvalue.cp;
	NOT_USED(data);
	if(pp && *pp->name!='/')
		_nv_unset(np,0);
}

/*
 * Obtain a file handle to the directory "path" relative to directory "dir"
 */
int sh_diropenat(Shell_t *shp, int dir, const char *path)
{
	int fd,shfd;

	if ((fd = openat(dir, path, O_DIRECTORY|O_NONBLOCK|O_CLOEXEC)) < 0)
#if O_SEARCH
	if (errno != EACCES || (fd = openat(dir, path, O_SEARCH|O_DIRECTORY|O_NONBLOCK|O_CLOEXEC)) < 0)
#endif
		return fd;

	/* Move fd to a number > 10 and register the fd number with the shell */
	shfd = sh_fcntl(fd, F_DUPFD_CLOEXEC, 10);
	close(fd);
	return(shfd);
}

/*
 * Obtain a file handle to the directory "path" relative to directory
 * "dir", or open a NFSv4 xattr directory handle for file dir/path.
 */
int sh_diropenat(Shell_t *shp, int dir, const char *path, int xattr)
{
#ifdef O_DIRECTORY
#define O_directory (O_DIRECTORY)
#else
#define O_directory (0)
#endif

	int fd;
	int shfd;
	int savederrno;
#ifndef O_DIRECTORY
  struct stat fs;
#endif
#ifndef AT_FDCWD
	NOT_USED(dir);
#endif
#ifndef O_XATTR
	NOT_USED(xattr);
#endif

#ifdef O_XATTR
	if(xattr)
	{
		int apfd; /* attribute parent fd */

		/* open parent node... */
		EINTR_REPEAT((apfd = openat(dir, path, O_RDONLY|O_NONBLOCK|O_cloexec)) < 0);
		if(apfd < 0)
			return -1;

		/* ... and then open a fd to the attribute directory */
		EINTR_REPEAT((fd = openat(apfd, e_dot, O_XATTR|O_cloexec)) < 0);

		savederrno = errno;
		EINTR_REPEAT(close(apfd) < 0);
		errno = savederrno;
	}
	else
#endif
	{
#ifdef AT_FDCWD
		/*
		 * Open directory. First we try without |O_SEARCH| and
		 * if this fails with EACCESS we try with |O_SEARCH|
		 * again.
		 * This is required ...
		 * - ... because some platforms may require that it can
		 * only be used for directories while some filesystems
		 * (e.g. Reiser4 or HSM systems) allow a |fchdir()| into
		 * files, too)
		 * - ... to preserve the semantics of "cd", e.g.
		 * otherwise "cd" would return [No access] instead of
		 * [Not a directory] for files on filesystems which do
		 * not allow a "cd" into files.
		 * - ... to allow that a
		 * $ redirect {n}</etc ; cd /dev/fd/$n # works on most
		 * platforms.
		 */
		EINTR_REPEAT((fd = openat(dir, path, O_cloexec)) < 0);
#ifdef O_SEARCH
		if((fd < 0) && (errno == EACCES))
		{
			EINTR_REPEAT((fd = openat(dir, path, O_SEARCH|O_cloexec)) < 0)
		}
#endif
#else
		/*
		 * Version of openat() call above for systems without
		 * openat API. This only works because we basically
		 * gurantee that |dir| is always the same place as
		 * |cwd| on such machines (but this won't be the case
		 * in the future).
		 */
		/*
		 * This |fchdir()| call is not needed (yet) since
		 * all consumers do not use |dir| when |AT_FDCWD|
		 * is not available.
		 *
		 * fchdir(dir);
		 */
		EINTR_REPEAT((fd = open(path, O_cloexec)) < 0);
#endif
	}

	if(fd < 0)
		return fd;

	/* Move fd to a number > 10 and *register* the fd number with the shell */
	shfd = sh_fcntl(fd, F_DUPFD, 10);
	savederrno=errno;

	EINTR_REPEAT(close(fd) < 0);

	/*
	 * FIXME: |sh_fcntl()| should implement F_DUPFD_CLOEXEC
	 * and F_DUP2FD_CLOEXEC to reduce the number of system calls
	 */
	if(shfd >=0)
		sh_fcntl(shfd, F_SETFD, FD_CLOEXEC);
	errno=savederrno;
	
	return shfd;
}

int	b_cd(int argc, char *argv[],Shbltin_t *context)
{
	register char *dir;
	Pathcomp_t *cdpath = 0;
	register const char *dp;
	register Shell_t *shp = context->shp;
	int saverrno=0;
	int rval,i,j;
	bool fflag=false, pflag=false, xattr=false;
	char *oldpwd;
	int dirfd = shp->pwdfd;
	int newdirfd;
	Namval_t *opwdnod, *pwdnod;
	if(sh_isoption(shp,SH_RESTRICTED))
		errormsg(SH_DICT,ERROR_exit(1),e_restricted+4);
	while((rval = optget(argv,sh_optcd))) switch(rval)
	{
		case 'f':
			dirfd = opt_info.num;
			fflag = true;
			break;
		case 'L':
			pflag = false;
			break;
		case 'P':
			pflag = true;
			break;
		case '@':
			xattr = true;
			break;
#ifdef O_XATTR
		case '@':
			xattr = 1;
			break;
#endif
		case ':':
			errormsg(SH_DICT,2, "%s", opt_info.arg);
			break;
		case '?':
			errormsg(SH_DICT,ERROR_usage(2), "%s", opt_info.arg);
			break;
	}
	argv += opt_info.index;
	argc -= opt_info.index;
	dir = argv[0];
	if(error_info.errors>0 || argc >2)
		errormsg(SH_DICT,ERROR_usage(2),"%s",optusage((char*)0));
	shp->pwd = path_pwd(shp,0);
	oldpwd = (char*)shp->pwd;
	opwdnod = (shp->subshell?sh_assignok(OLDPWDNOD,1):OLDPWDNOD); 
	pwdnod = (shp->subshell?sh_assignok(PWDNOD,1):PWDNOD); 
	if(dirfd!=shp->pwdfd && dir==0)
		dir = (char*)e_dot;
	if(argc==2)
		dir = sh_substitute(shp,oldpwd,dir,argv[1]);
	else if(!dir)
	{
		struct passwd *pw;
		if(!(dir = nv_getval(HOME)) && (pw = getpwuid(geteuid())))
			dir = pw->pw_dir;
	}
	else if(*dir == '-' && dir[1]==0)
		dir = nv_getval(opwdnod);
	if(!dir || *dir==0)
		errormsg(SH_DICT,ERROR_exit(1),argc==2?e_subst+4:e_direct);
	if (xattr)
	{
		if(!shp->strbuf2)
			shp->strbuf2 = sfstropen();
		j = sfprintf(shp->strbuf2,"%s",dir);
		dir = sfstruse(shp->strbuf2);
		pathcanon(dir, j + 1, 0);
		sfprintf(shp->strbuf, "/dev/file/xattr@%s//@//", dir);
		dir = sfstruse(shp->strbuf);
	}
#if _WINIX
	if(*dir != '/' && (dir[1]!=':'))
#else
	if(dirfd==shp->pwdfd && *dir != '/')
#endif /* _WINIX */
	{
		if(!(cdpath = (Pathcomp_t*)shp->cdpathlist) && (dp=sh_scoped(shp,CDPNOD)->nvalue.cp))
		{
			if(cdpath=path_addpath(shp,(Pathcomp_t*)0,dp,PATH_CDPATH))
			{
				shp->cdpathlist = (void*)cdpath;
				cdpath->shp = shp;
			}
		}
		if(!oldpwd)
			oldpwd = path_pwd(shp,1);
	}
	if(dirfd==shp->pwdfd && *dir!='/')
	{
		/* check for leading .. */

		char *cp;

		j = sfprintf(shp->strbuf,"%s",dir);
		cp = sfstruse(shp->strbuf);
		pathcanon(cp, j + 1, 0);
		if(cp[0]=='.' && cp[1]=='.' && (cp[2]=='/' || cp[2]==0))
		{
			if(!shp->strbuf2)
				shp->strbuf2 = sfstropen();
			j = sfprintf(shp->strbuf2,"%s/%s",oldpwd,cp);
			dir = sfstruse(shp->strbuf2);
			pathcanon(dir, j + 1, 0);
		}
	}
#ifdef O_XATTR
	if (xattr)
#ifdef PATH_BFPATH
		cdpath = NULL
#else
		cdpath = ""
#endif
	;
#endif
	rval = -1;
	do
	{
		dp = cdpath?cdpath->name:"";
		cdpath = path_nextcomp(shp,cdpath,dir,0);
#if _WINIX
		if(*stakptr(PATH_OFFSET+1)==':' && isalpha(*stakptr(PATH_OFFSET)))
		{
			*stakptr(PATH_OFFSET+1) = *stakptr(PATH_OFFSET);
			*stakptr(PATH_OFFSET)='/';
		}
#endif /* _WINIX */
		if(*stakptr(PATH_OFFSET)!='/' && dirfd==shp->pwdfd)
		{
			char *last=(char*)stakfreeze(1);
			stakseek(PATH_OFFSET);
			stakputs(oldpwd);
			/* don't add '/' of oldpwd is / itself */
			if(*oldpwd!='/' || oldpwd[1])
				stakputc('/');
			stakputs(last+PATH_OFFSET);
			stakputc(0);
		}
		if(!fflag && !pflag)
		{
			register char *cp;
			stakseek(PATH_MAX+PATH_OFFSET);
#if SHOPT_FS_3D
			if(!(cp = pathcanon(stakptr(PATH_OFFSET),PATH_MAX,PATH_ABSOLUTE|PATH_DOTDOT|PATH_DROP_TAIL_SLASH)))
				continue;
#else
			if(*(cp=stakptr(PATH_OFFSET))=='/')
				if(!pathcanon(cp,PATH_MAX,PATH_ABSOLUTE|PATH_DOTDOT))
					continue;
#endif /* SHOPT_FS_3D */
		}
		rval = newdirfd = sh_diropenat(shp, dirfd, path_relative(shp,stakptr(PATH_OFFSET)));
		if(newdirfd >=0)
		{
			/* chdir for directories on HSM/tapeworms may take minutes */
			if((rval=fchdir(newdirfd)) >= 0)
			{
				if(shp->pwdfd >= 0)
					sh_close(shp->pwdfd);
				shp->pwdfd=newdirfd;
				goto success;
			}
			sh_close(newdirfd);
		}
#if !O_SEARCH
		else if((rval=chdir(path_relative(shp,stakptr(PATH_OFFSET)))) >= 0 && shp->pwdfd >= 0)
		{
			sh_close(shp->pwdfd);
			shp->pwdfd = AT_FDCWD;
		}
#endif
		if(saverrno==0)
			saverrno=errno;
	}
	while(cdpath);
	if(rval<0 && *dir=='/' && *(path_relative(shp,stakptr(PATH_OFFSET)))!='/')
	{
		rval = newdirfd = sh_diropenat(shp, shp->pwdfd, dir);
		if(newdirfd >=0)
		{
			/* chdir for directories on HSM/tapeworms may take minutes */
			if((rval=fchdir(newdirfd)) >= 0)
			{
				if(shp->pwdfd >= 0)
					sh_close(shp->pwdfd);
				shp->pwdfd=newdirfd;
				goto success;
			}
			sh_close(newdirfd);
		}
#if !O_SEARCH
		else if(chdir(dir) >=0 && shp->pwdfd >= 0)
		{
			sh_close(shp->pwdfd);
			shp->pwdfd = AT_FDCWD;
		}
#endif
	}
	/* use absolute chdir() if relative chdir() fails */
	if(rval<0)
	{
		if(saverrno)
			errno = saverrno;
		errormsg(SH_DICT,ERROR_system(1),"%s:",dir);
	}
success:
	if(dir == nv_getval(opwdnod) || argc==2)
		dp = dir;	/* print out directory for cd - */
	if(pflag)
	{
		dir = stakptr(PATH_OFFSET);
		if (!(dir=pathcanon(dir,PATH_MAX,PATH_ABSOLUTE|PATH_PHYSICAL)))
		{
			dir = stakptr(PATH_OFFSET);
			errormsg(SH_DICT,ERROR_system(1),"%s:",dir);
		}
		stakseek(dir-stakptr(0));
	}
	dir = (char*)stakfreeze(1)+PATH_OFFSET;
	if(*dp && (*dp!='.'||dp[1]) && strchr(dir,'/'))
		sfputr(sfstdout,dir,'\n');
	if(*dir != '/')
	{
		if(!fflag)
			return(0);
		dir = fgetcwd(newdirfd, 0, 0);
	}
	nv_putval(opwdnod,oldpwd,NV_RDONLY);
	i = j = (int)strlen(dir);
	/* delete trailing '/' */
	while(--i>0 && dir[i]=='/')
	{
		/* //@// exposed here and in pathrelative() -- rats */
		if(i>=3 && (j-i)==2 && dir[i-1]=='@' && dir[i-2]=='/' && dir[i-3]=='/')
		{
			dir[i+1] = '/';
			break;
		}
		dir[i] = 0;
	}
	nv_putval(pwdnod,dir,NV_RDONLY);
	nv_onattr(pwdnod,NV_NOFREE|NV_EXPORT);
	shp->pwd = pwdnod->nvalue.cp;
	nv_scan(shp->track_tree,invalidate,(void*)0,NV_TAGGED,NV_TAGGED);
	path_newdir(shp,shp->pathlist);
	path_newdir(shp,shp->cdpathlist);
	if(oldpwd)
		free(oldpwd);
	return(0);
}

int	b_pwd(int argc, char *argv[],Shbltin_t *context)
{
	register char *cp, *dir;
	register Shell_t *shp = context->shp;
	bool pflag = false;
	int n,fd,ffd=-1;
	NOT_USED(argc);
	while((n=optget(argv,sh_optpwd))) switch(n)
	{
		case 'f':
			ffd = opt_info.num;
			break;
		case 'L':
			pflag = false;
			break;
		case 'P':
			pflag = true;
			break;
		case ':':
			errormsg(SH_DICT,2, "%s", opt_info.arg);
			break;
		case '?':
			errormsg(SH_DICT,ERROR_usage(2), "%s", opt_info.arg);
			break;
	}
	if(error_info.errors)
		errormsg(SH_DICT,ERROR_usage(2),"%s",optusage((char*)0));
	if (ffd != -1)
	{
		if(!(cp = fgetcwd(ffd, 0, 0)))
			errormsg(SH_DICT, ERROR_system(1), e_pwd);
		sfputr(sfstdout, cp, '\n');
		free(cp);
		return(0);

	}
	if(pflag)
	{
#if SHOPT_FS_3D
		int mc;
		if(shp->gd->lim.fs3d && (mc = mount(e_dot,NIL(char*),FS3D_GET|FS3D_VIEW,0))>=0)
		{
			cp = (char*)stakseek(++mc+PATH_MAX);
			mount(e_dot,cp,FS3D_GET|FS3D_VIEW|FS3D_SIZE(mc),0);
		}
		else
#endif /* SHOPT_FS_3D */
		{
			cp = path_pwd(shp,0);
			cp = strcpy(stakseek(strlen(cp)+PATH_MAX),cp);
		}
		pathcanon(cp,PATH_MAX,PATH_ABSOLUTE|PATH_PHYSICAL);
	}
	else
		cp = path_pwd(shp,0);
	if(*cp!='/')
		errormsg(SH_DICT,ERROR_system(1), e_pwd);
	sfputr(sfstdout,cp,'\n');
	return(0);
}
