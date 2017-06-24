/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1985-2012 AT&T Intellectual Property          *
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
*               Glenn Fowler <glenn.s.fowler@gmail.com>                *
*                    David Korn <dgkorn@gmail.com>                     *
*                     Phong Vo <phongvo@gmail.com>                     *
*                                                                      *
***********************************************************************/
#include	"sfhdr.h"

/*
 * _sfopen() wrapper to allow user sfopen() intercept
 */

extern Sfio_t*		_sfopenat _ARG_((int, Sfio_t*, const char*, const char*));
extern Sfio_t*		_sfopenat _ARG_((int, Sfio_t*, const char*, const char*));

#if __STD_C
Sfio_t* sfopen(Sfio_t* f, const char* file, const char* mode)
#else
Sfio_t* sfopen(f,file,mode)
Sfio_t*		f;		/* old stream structure */
char*		file;		/* file/string to be opened */
reg char*	mode;		/* mode of the stream */
#endif
{
#ifdef AT_FDCWD
	return _sfopenat(AT_FDCWD, f, file, mode);
#else
/*
 * If |AT_FDCWD| is not defined then the |openat()| API is not
 * available. In that case |sfopenat()| will ignore the dirfd argument.
 */
	return _sfopenat(-1, f, file, mode);
#endif
}

/* #if __STD_C
 * Sfio_t* sfopenat(int dirfd, Sfio_t* f, const char* file, const char* mode)
 * #else
 * Sfio_t* sfopenat(dirfd, f,file,mode)
 * int		dirfd;		/* directory fd *\/
 * Sfio_t*		f;		/* old stream structure *\/
 * char*		file;		/* file/string to be opened *\/
 * reg char*	mode;		/* mode of the stream *\/
 * #endif
 * {
 * 	return _sfopenat(dirfd, f, file, mode);
 * }
 */

#if __STD_C
Sfio_t* sfopenat(int cwd, Sfio_t* f, const char* file, const char* mode)
#else
Sfio_t* sfopenat(cwd,f,file,mode)
int		cwd;		/* openat() cwd */
Sfio_t*		f;		/* old stream structure */
char*		file;		/* file/string to be opened */
reg char*	mode;		/* mode of the stream */
#endif
{
	return _sfopenat(cwd, f, file, mode);
}
