/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1989-2013 AT&T Intellectual Property          *
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
*                                                                      *
***********************************************************************/
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Bell Laboratories
 *
 * convert a make abstract machine stream on stdin
 * to an nmake makefile on stdout
 */

static const char usage[] =
"[-?\n@(#)$Id: mamnew (AT&T Research) 1990-06-11 $\n]"
USAGE_LICENSE
"[+NAME?mamnew - make abstract machine to nmake makefile conversion filter]"
"[+DESCRIPTION?\bmamnew\b reads MAM (Make Abstract Machine) target and"
"	prerequisite file descriptions from the standard input and writes"
"	an equivalent \bnmake\b(1) makefile on the standard output. Mamfiles"
"	are generated by the \b--mam\b option of \bnmake\b(1) and"
"	\bgmake\b(1).]"
"[+?Symbolic information is preserved where possible in the output makefile."
"	Comments, however, are lost in the conversion. Header file"
"	dependencies are omitted since these are implicitly determined by"
"	\bnmake\b(1). Recursive makefiles are not converted; each makefile"
"	level must be converted separately. Some targets are omitted; most"
"	of these are provided as predefined \bnmake\b(1) targets.]"
"[+?\bmamnew\b is only an \aaid\a in the makefile conversion process. It is"
"	a rare old-make makefile that requires no manual intervention after"
"	\bmamnew\b conversion.]"

"[d:debug?]#[level]"
"[x:omit?Omit pathnames with directory \aprefix\a.]#[prefix]"

"[+SEE ALSO?\bmamold\b(1), \bgmake\b(1), \bnmake\b(1)]"
"[+REFERENCES]{"
"	[+A Make abstract machine, Glenn Fowler, 1994.?"
"		http://www.research.att.com/~gsf/mam/]"
"}"
;

#include <ast.h>
#include <mam.h>
#include <ctype.h>
#include <error.h>

#define LONGLINE	72		/* too long output line length	*/
#define MAXNAME		1024		/* max rule name		*/

#define message		if(error_info.trace<0)error

#define A_ancestor	(A_LAST<<1)	/* ancestor directory		*/
#define A_command	(A_LAST<<2)	/* command target		*/
#define A_directory	(A_LAST<<3)	/* source directory		*/
#define A_implict	(A_LAST<<4)	/* metarules can handle it	*/
#define A_listprereq	(A_LAST<<5)	/* prereqs listed		*/
#define A_listtarg	(A_LAST<<6)	/* listed as target		*/
#define A_omit		(A_LAST<<7)	/* omit prereq			*/

#define V_state		(V_LAST<<1)	/* state var			*/

struct local				/* local rule info		*/
{
	char*		base;		/* base name			*/
	struct rule*	directory;	/* directory prefix		*/
	char*		suffix;		/* suffix			*/
	char*		target;		/* target name if != base	*/
};

static struct				/* program state		*/
{
	int		ancestor;	/* ancestor dir .. count	*/
	struct mam*	mam;		/* make abstract machine info	*/
	struct block*	omit;		/* dir prefixes to omit		*/

	struct
	{
	struct rule*	source;		/* .SOURCE			*/
	struct rule*	source_a;	/* .SOURCE.a			*/
	struct rule*	source_h;	/* .SOURCE.h			*/
	struct rule*	sources;	/* .MAM.SOURCES.		*/
	}		atom;		/* special atoms		*/
} state;

/*
 * check if name is on omit list
 */

static int
omitted(char* name)
{
	char*		s;
	char*		t;
	struct block*	d;

	for (d = state.omit; d; d = d->next)
	{
		s = name;
		t = d->data;
		do if (!*t)
		{
			if (!*s || *s == '/')
				return 1;
			break;
		} while (*s++ == *t++);
	}
	return 0;
}

/*
 * initialize source dir
 */

static struct rule*
initdir(char* s, char* suf)
{
	struct rule*	d;
	int		n;
	struct rule*		z;
	char			buf[MAXNAME];

	d = mamrule(state.mam->main, s);
	if (!(d->attributes & A_directory))
	{
		d->attributes |= A_directory;
		n = 0;
		while (*s == '.' && *(s + 1) == '.' && *(s + 2) == '/')
		{
			s += 3;
			n++;
		}
		if (!strmatch(s, "include|lib"))
		{
			sfsprintf(buf, sizeof(buf), "%s%s", state.atom.source->name, suf);
			z = mamrule(state.mam->main, buf);
			z->attributes |= A_directory;
			mamprereq(state.mam->main, z, d, NiL);
			mamprereq(state.mam->main, state.atom.sources, z, NiL);
		}
		else if (n > state.ancestor)
			state.ancestor = n;
	}
	return d;
}

/*
 * dump a value that may be expanded by oldmake
 */

static int
dumpvalue(int col, char* s, int sep)
{
	int	c;
	char*	v;

	if (sep && sep != '\\')
	{
		sfputc(sfstdout, sep);
		col++;
	}
	for (;;)
		switch (c = *s++)
		{
		case 0:
			if (sep && sep != '\t')
			{
				col = 1;
				sfputc(sfstdout, '\n');
			}
			return col;
		case '\t':
			col += 7 - (col % 8);
			/*FALLTHROUGH*/
		case ' ':
			if (col < LONGLINE - 8)
				goto emit;
			while (isspace(*s))
				s++;
			if (*s)
			{
				sfputr(sfstdout, " \\\n\t", -1);
				col = 8;
			}
			break;
		case '\\':
			if (sep == '\\')
				c = *s++;
			goto emit;
		case '$':
			if (isalpha(*s) || *s == '_')
			{
				for (v = s; isalnum(*v) || *v == '_'; v++);
				c = *v;
				*v = 0;
				if (getvar(state.mam->main, s))
				{
					sfprintf(sfstdout, "$(%s)", s);
					col += (v - s) + 3;
					*(s = v) = c;
					break;
				}
				*v = c;
				c = '$';
			}
			/*FALLTHROUGH*/
		default:
		emit:
			sfputc(sfstdout, c);
			col++;
			break;
		}
}

/*
 * dump a name keeping track of the right margin
 */

static int
dumpname(int col, char* s)
{

	int	n;

	n = strlen(s);
	if (col + n >= LONGLINE)
	{
		sfputr(sfstdout, " \\\n\t\t", -1);
		col = 16;
	}
	else if (col <= 1)
		col = 1;
	else
	{
		sfputc(sfstdout, ' ');
		col++;
	}
	col += n;
	dumpvalue(0, s, 0);
	return col;
}

/*
 * check s for interesting cc flags
 * if v!=0 then it is var for s
 */

static void
ccflags(char* s, struct var* v)
{
	char*		t;
	char*			b;
	char*			u;
	int			n;
	char			buf[MAXNAME];

	static int		init;

	b = buf;
	for (;;)
	{
		while (*s && *s != '-')
			s++;
		if (!*s)
			break;
		for (t = s; *t; t++)
		{
			if (isspace(*t))
				break;
			if (*t == '\\' && !*++t)
				break;
		}
		*t++ = 0;
		switch (*(s + 1))
		{
		case 'c':
		case 'U':
			break;
		case 'D':
			for (u = s + 2; isalnum(*u); u++);
			if (n = *u)
				*u = 0;
			mamvar(state.mam->main, s + 2, s);
			if (n)
				*u = n;
			break;
		case 'I':
			pathcanon(s += 2, 0, 0);
			if (s[0] && (s[0] != '.' || s[1]) && !omitted(s))
				initdir(s, ".h");
			break;
		case 'O':
			if (!*(s + 2))
				break;
			/*FALLTHROUGH*/
		default:
			if (b != buf)
				*b++ = ' ';
			b = stpcpy(b, s);
			break;
		}
		s = t;
	}
	if (v && b > buf)
	{
		if (!init)
		{
			init = 1;
			sfputc(sfstdout, '\n');
		}
		*b = 0;
		dumpvalue(dumpname(0, v->name), buf, '=');
	}
}

/*
 * initialize local rule info
 */

static int
initrule(const char* as, char* ar, void* handle)
{
	char*		s = (char*)as;
	struct rule*	r = (struct rule*)ar;
	struct local*	x;
	struct block*	p;
	char*			t;

	NoP(handle);
	if (!(x = newof(0, struct local, 1, 0)))
		error(3, "out of space");
	r->local.pointer = (char*)x;
	pathcanon(r->name, 0, 0);
	s = r->name + strlen(r->name);
	if (s >= r->name + 2 && *(s -= 2) == '.')
		x->suffix = s + 1;
	else
		s = "";
	if (r->action || !(t = strrchr(r->name, '/')))
		x->base = r->name;
	else if (omitted(r->name))
	{
		r->attributes |= A_omit;
		x->base = t + 1;
	}
	else
	{
		*t = 0;
		x->directory = initdir(r->name, s);
		*t++ = '/';
		x->base = t;
	}
	if (strmatch(x->base, "lib*.a"))
	{
		x->target = x->base;
		s = x->base = strdup(x->base + 1);
		s[0] = '-';
		s[1] = 'l';
		s[strlen(s) - 2] = 0;
	}
	else if ((p = r->action) && s[1] == 'o' && !s[2])
	{
		r->action = 0;
		r->prereqs = 0;
		while (p)
		{
			ccflags(p->data, NiL);
			p = p->next;
		}
	}
	return 0;
}

/*
 * clear listprereq for all prerequisites
 */

static void
clrprereqs(struct rule* r)
{
	struct list*	p;

	r->attributes &= ~A_listprereq;
	for (p = r->prereqs; p; p = p->next)
		if (p->rule->attributes & A_listprereq)
			clrprereqs(p->rule);
}

/*
 * dump the (implicit) prerequisites for r
 */

static int
dumpprereqs(int col, struct rule* r)
{
	struct list*	p;

	if (!(r->attributes & (A_listprereq|A_omit)))
	{
		r->attributes |= A_listprereq;
		col = dumpname(col, r->local.pointer ? ((struct local*)r->local.pointer)->base : r->name);
		if (r->attributes & (A_directory))
			for (p = r->prereqs; p; p = p->next)
				if (p->rule->attributes & (A_directory))
					col = dumpprereqs(col, p->rule);
	}
	return col;
}

/*
 * dump an action
 */

static void
dumpaction(struct block* p)
{
	if (p) do
	{
		dumpvalue(0, p->data, '\t');
		sfputc(sfstdout, '\n');
	} while (p = p->next);
}

/*
 * dump the rules
 */

static void
dump(struct rule* r)
{
	int		col;
	struct list*	p;

	if (!(r->attributes & (A_listtarg|A_metarule)))
	{
		r->attributes |= A_listtarg;
		if (r->action || r->prereqs)
		{
			clrprereqs(r);
			r->attributes |= A_listprereq;
			sfputc(sfstdout, '\n');
			message(-1, "%s:%s", r->name, r->action ? " action" : "");
			col = dumpname(1, r->local.pointer ? (((struct local*)r->local.pointer)->target ? ((struct local*)r->local.pointer)->target : ((struct local*)r->local.pointer)->base) : r->name);
			col = dumpname(col, ":");
			for (p = r->prereqs; p; p = p->next)
				if (!(p->rule->attributes & A_listprereq))
				{
					clrprereqs(p->rule);
					col = dumpprereqs(col, p->rule);
				}
			sfputc(sfstdout, '\n');
			dumpaction(r->action);
		}
		for (p = r->prereqs; p; p = p->next)
			if (p->rule != r)
				dump(p->rule);
	}
}

/*
 * dump state var definition
 */

static int
dumpstate(const char* an, char* av, void* handle)
{
	char*		name = (char*)an;
	struct var*	v = (struct var*)av;
	char*	s = v->value;
	int	col;

	static int	init;

	NoP(handle);
	if (*s++ == '-')
	{
		if (*s == 'D')
		{
			while (isalnum(*++s));
			if (*s++ == '=')
			{
				if (!init)
				{
					init = 1;
					sfputc(sfstdout, '\n');
				}
				v->attributes |= V_state;
				col = dumpname(0, name);
				sfputr(sfstdout, " == ", -1);
				col += 2;
				if (*s == '"')
				{
					s[strlen(s) - 1] = 0;
					dumpvalue(col, s + 1, '\\');
				}
				else
					dumpvalue(col, s, 0);
			}
			else
			{
				if (!init)
				{
					init = 1;
					sfputc(sfstdout, '\n');
				}
				v->attributes |= V_state;
				dumpname(0, name);
				sfputr(sfstdout, " == 1\n", -1);
			}
		}
		else if (*s == 'U')
		{
			if (!init)
			{
				init = 1;
				sfputc(sfstdout, '\n');
			}
			v->attributes |= V_state;
			dumpname(0, name);
			sfputr(sfstdout, " ==\n", -1);
		}
	}
	return 0;
}

/*
 * dump interesting var definitions
 */

static void
dumpvar(void)
{
	struct var*	v;

	if (v = getvar(state.mam->main, "CCFLAGS"))
		ccflags(v->value, v);
}

/*
 * add prefix to list of dir prefixes to omit
 */

static void
omit(char* prefix)
{
	struct block*	p;

	if (!(p = newof(0, struct block, 1, 0)))
		error(3, "out of space");
	p->data = prefix;
	p->next = state.omit;
	state.omit = p;
}

int
main(int argc, char** argv)
{
	struct list*	p;

	NoP(argc);
	error_info.id = "mamnew";
	for (;;)
	{
		switch (optget(argv, usage))
		{
		case 'd':
			error_info.trace = -opt_info.num;
			continue;
		case 'x':
			omit(opt_info.arg);
			continue;
		case '?':
			error(ERROR_USAGE|4, opt_info.arg);
			break;
		case ':':
			error(2, opt_info.arg);
			break;
		}
		break;
	}
	if (error_info.errors)
		error(ERROR_USAGE|4, "%s", optusage(NiL));

	/*
	 * preventive maintenance
	 */

	error(1, "still under development -- send positive contributions to gsf@research.att.com");

	/*
	 * initialize
	 */

	omit("/usr/include");
	omit("/usr/lib");

	/*
	 * scan, collect and dump
	 */

	sfprintf(sfstdout, "/* # # nmake makefile generated by mamnew # # */\n");
	if (!(state.mam = mamalloc()))
		error(3, "cannot initialize");
	state.atom.source = mamrule(state.mam->main, ".SOURCE");
	state.atom.source_a = mamrule(state.mam->main, ".SOURCE.a");
	state.atom.source_h = mamrule(state.mam->main, ".SOURCE.h");
	state.atom.sources = mamrule(state.mam->main, ".MAM.SOURCES.");
	if (mamscan(state.mam, NiL) < 0)
		error(3, "invalid input");
	hashwalk(state.mam->main->rules, 0, initrule, NiL);
	if (state.ancestor)
		sfprintf(sfstdout, "\nancestor = %d\n", state.ancestor);
	dumpvar();
	hashwalk(state.mam->main->vars, 0, dumpstate, NiL);
	for (p = state.atom.sources->prereqs; p; p = p->next)
		if (p->rule->prereqs)
			dump(p->rule);
	for (p = state.mam->main->root->prereqs; p; p = p->next)
		dump(p->rule);
	exit(error_info.errors != 0);
}
