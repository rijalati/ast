/***********************************************************************
 *                                                                      *
 *               This software is part of the ast package               *
 *          Copyright (c) 1985-2011 AT&T Intellectual Property          *
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
#include <ast_std.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#if 0
#    include <arpa/inet.h>
#    include <arpa/nameser.h>
#    include <linux/sockios.h>
#    include <net/if.h>
#    include <resolv.h>
#endif

#ifndef sigmask
#    define sigmask(n) (( unsigned long ) 1 << (( n ) -1))
#endif

extern void
_sethtent(int f);
extern void
_endhtent(void);
extern struct hostent *
_gethtent(void);
extern struct hostent *
_gethtbyname(const char *name);
extern struct hostent *
_gethtbyaddr(const char *addr, int len, int type);
extern int
_validuser(FILE *hostf,
           const char *rhost,
           const char *luser,
           const char *ruser,
           int baselen);
extern int
_checkhost(const char *rhost, const char *lhost, int len);

#if 0
extern void putlong(u_long l, u_char *msgp);
extern void putshort(u_short l, u_char *msgp);
extern u_int32_t _getlong(const u_char *msgp);
extern u_int16_t _getshort(const u_char *msgp);
extern void p_query(char *msg);
extern void fp_query(char *msg, FILE *file);
extern char *p_cdname(char *cp, char *msg, FILE *file);
extern char *p_rr(char *cp, char *msg, FILE *file);
extern char *p_type(int type);
extern char * p_class(int class);
extern char *p_time(u_long value);
#endif

extern char *
hostalias(const char *name);
extern void
sethostfile(char *name);
extern void
_res_close(void);
extern void
ruserpass(const char *host, char **aname, char **apass);
extern char *
index(const char *, int);
extern int
strcasecmp(const char *, const char *);
extern void
bcopy(const void *, void *, size_t);
