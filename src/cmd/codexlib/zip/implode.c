#pragma prototyped

/*
 * zip implode decoder
 */

#include "zip.h"
#include "huff.h"

#define EXPLODE_BIG	01
#define EXPLODE_LIT	02

#define WSIZE		32768

/* explode.c -- put in the public domain by Mark Adler
   version c15, 6 July 1996 */


/* You can do whatever you like with this source file, though I would
   prefer that if you modify it and redistribute it that you include
   comments to that effect with your name and the date.  Thank you.

   History:
   vers    date          who           what
   ----  ---------  --------------  ------------------------------------
    c1   30 Mar 92  M. Adler        explode that uses huff() from inflate
                                    (this gives over a 70% speed improvement
                                    over the original unimplode.c, which
                                    decoded a bit at a time)
    c2    4 Apr 92  M. Adler        fixed bug for file sizes a multiple of 32k.
    c3   10 Apr 92  M. Adler        added a little memory tracking if DEBUG
    c4   11 Apr 92  M. Adler        added NOMEMCPY do kill use of memcpy()
    c5   21 Apr 92  M. Adler        added the WSIZE #define to allow reducing
                                    the 32K window size for specialized
                                    applications.
    c6   31 May 92  M. Adler        added typecasts to eliminate some warnings
    c7   27 Jun 92  G. Roelofs      added more typecasts.
    c8   17 Oct 92  G. Roelofs      changed ULONG/UWORD/byte to ulg/ush/uch.
    c9   19 Jul 93  J. Bush         added more typecasts (to return values);
                                    made l[256] array static for Amiga.
    c10   8 Oct 93  G. Roelofs      added used_csize for diagnostics; added
                                    buf and unshrink arguments to flush();
                                    undef'd various macros at end for Turbo C;
                                    removed NEXTBYTE macro (now in unzip.h)
                                    and bytebuf variable (not used); changed
                                    memset() to memzero().
    c11   9 Jan 94  M. Adler        fixed incorrect used_csize calculation.
    c12   9 Apr 94  G. Roelofs      fixed split comments on preprocessor lines
                                    to avoid bug in Encore compiler.
    c13  25 Aug 94  M. Adler        fixed distance-length comment (orig c9 fix)
    c14  22 Nov 95  S. Maxwell      removed unnecessary "static" on auto array
    c15   6 Jul 96  W. Haidinger    added ulg typecasts to flush() calls
 */


/*
   Explode imploded (PKZIP method 6 compressed) data.  This compression
   method searches for as much of the current string of bytes (up to a length
   of ~320) in the previous 4K or 8K bytes.  If it doesn't find any matches
   (of at least length 2 or 3), it codes the next byte.  Otherwise, it codes
   the length of the matched string and its distance backwards from the
   current position.  Single bytes ("literals") are preceded by a one (a
   single bit) and are either uncoded (the eight bits go directly into the
   compressed stream for a total of nine bits) or Huffman coded with a
   supplied literal code tree.  If literals are coded, then the minimum match
   length is three, otherwise it is two.

   There are therefore four kinds of imploded streams: 8K search with coded
   literals (min match = 3), 4K search with coded literals (min match = 3),
   8K with uncoded literals (min match = 2), and 4K with uncoded literals
   (min match = 2).  The kind of stream is identified in two bits of a
   general purpose bit flag that is outside of the compressed stream.

   Distance-length pairs for matched strings are preceded by a zero bit (to
   distinguish them from literals) and are always coded.  The distance comes
   first and is either the low six (4K) or low seven (8K) bits of the
   distance (uncoded), followed by the high six bits of the distance coded.
   Then the length is six bits coded (0..63 + min match length), and if the
   maximum such length is coded, then it's followed by another eight bits
   (uncoded) to be added to the coded length.  This gives a match length
   range of 2..320 or 3..321 bytes.

   The literal, length, and distance codes are all represented in a slightly
   compressed form themselves.  What is sent are the lengths of the codes for
   each value, which is sufficient to construct the codes.  Each byte of the
   code representation is the code length (the low four bits representing
   1..16), and the number of values sequentially with that length (the high
   four bits also representing 1..16).  There are 256 literal code values (if
   literals are coded), 64 length code values, and 64 distance code values,
   in that order at the beginning of the compressed stream.  Each set of code
   values is preceded (redundantly) with a byte indicating how many bytes are
   in the code description that follows, in the range 1..256.

   The codes themselves are decoded using tables made by huff() from
   the bit lengths.  That routine and its comments are in the inflate.c
   module.
 */

struct State_s; typedef struct State_s State_t;

struct State_s
{
    Codex_t* codex;
    int method;
    ssize_t (*explodef)(State_t*, char*, size_t);

    uch  buf[SF_BUFSIZE];
    uch* ip;
    uch* ie;

    ulg bit_buf;	/* bit buffer */
    ulg bit_len;	/* bits in bit buffer */

    uch slide[WSIZE];
    Huff_t *tb;		/* literal, length, and distance tables */
    Huff_t *tl;
    Huff_t *td;
    int bb;			/* number of bits decoded by those */
    int bl;
    int bd;

    ulg u, n, d, w;
    size_t s;			/* original size */
    ulg l[256];		/* bit lengths for codes */

    Vmalloc_t* vm;

    int eof;
};

/* The implode algorithm uses a sliding 4K or 8K byte window on the
   uncompressed stream to find repeated byte strings.  This is implemented
   here as a circular buffer.  The index is updated simply by incrementing
   and then and'ing with 0x0fff (4K-1) or 0x1fff (8K-1).  Here, the 32K
   buffer of inflate is used, and it works just as well to always have
   a 32K circular buffer, so the index is anded with 0x7fff.  This is
   done to allow the window to also be used as the output buffer. */
/* This must be supplied in an external module useable like "uch slide[8192];"
   or "uch *slide;", where the latter would be malloc'ed.  In unzip, slide[]
   is actually a 32K area for use by inflate, which uses a 32K sliding window.
 */


/* Tables for length and distance */
static ush cplen2[] =
        {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
        35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65};
static ush cplen3[] =
        {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
        53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66};
static ush extra[] =
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        8};
static ush cpdist4[] =
        {1, 65, 129, 193, 257, 321, 385, 449, 513, 577, 641, 705,
        769, 833, 897, 961, 1025, 1089, 1153, 1217, 1281, 1345, 1409, 1473,
        1537, 1601, 1665, 1729, 1793, 1857, 1921, 1985, 2049, 2113, 2177,
        2241, 2305, 2369, 2433, 2497, 2561, 2625, 2689, 2753, 2817, 2881,
        2945, 3009, 3073, 3137, 3201, 3265, 3329, 3393, 3457, 3521, 3585,
        3649, 3713, 3777, 3841, 3905, 3969, 4033};
static ush cpdist8[] =
        {1, 129, 257, 385, 513, 641, 769, 897, 1025, 1153, 1281,
        1409, 1537, 1665, 1793, 1921, 2049, 2177, 2305, 2433, 2561, 2689,
        2817, 2945, 3073, 3201, 3329, 3457, 3585, 3713, 3841, 3969, 4097,
        4225, 4353, 4481, 4609, 4737, 4865, 4993, 5121, 5249, 5377, 5505,
        5633, 5761, 5889, 6017, 6145, 6273, 6401, 6529, 6657, 6785, 6913,
        7041, 7169, 7297, 7425, 7553, 7681, 7809, 7937, 8065};


/* Macros for inflate() bit peeking and grabbing.
   The usage is:

        NEEDBITS(j)
        x = GETBITS(j)
        DUMPBITS(j)

   where NEEDBITS makes sure that b has at least j bits in it, and
   DUMPBITS removes the bits from b.  The macros use the variable k
   for the number of bits in b.  Normally, b and k are register
   variables for speed.
 */

#define NEXTBYTE(p)	((p)->ip < (p)->ie ? (int)*(p)->ip++ : fill(p))
#define MASK_BITS(n) ((((ulg)1)<<(n))-1)
#define NEEDBITS(p,n) {while((p)->bit_len<(n)){(p)->bit_buf|=((ulg)NEXTBYTE(p))<<(p)->bit_len;(p)->bit_len+=8;}}
#define GETBITS(p,n)  ((ulg)(p)->bit_buf & MASK_BITS(n))
#define IGETBITS(p,n)  ((~((ulg)(p)->bit_buf)) & MASK_BITS(n))
#define DUMPBITS(p,n) {(p)->bit_buf>>=(n);(p)->bit_len-=(n);}

static int
fill(State_t* state)
{
	ssize_t	r;

	if (state->eof)
		return 0;
	if ((r = sfrd(state->codex->sp, state->buf, sizeof(state->buf), &state->codex->sfdisc)) <= 0)
	{
		state->eof = 1;
		return 0;
	}
	state->ie = (state->ip = state->buf) + r;
	return *state->ip++;
}

static int get_tree(
    State_t* state,
    ulg *l,		/* bit lengths */
    ulg n)			/* number expected */
/* Get the bit lengths for a code representation from the compressed
   stream.  If get_tree() returns 4, then there is an error in the data.
   Otherwise zero is returned. */
{
    ulg i;			/* bytes remaining in list */
    ulg k;			/* lengths entered */
    ulg j;			/* number of codes */
    ulg b;			/* bit length for those codes */

    /* get bit lengths */
    i = NEXTBYTE(state) + 1;		/* length/count pairs to read */
    k = 0;			/* next code */
    do {
	b = ((j = NEXTBYTE(state)) & 0xf) + 1;	/* bits in code (1..16) */
	j = ((j & 0xf0) >> 4) + 1; /* codes with those bits (1..16) */
	if (k + j > n)
	    return 4;		/* don't overflow l[] */
	do {
	    l[k++] = b;
	} while (--j);
    } while (--i);

    return k != n ? 4 : 0;	/* should have read n of them */
}

static ssize_t explode_lit8(State_t* state, char *buff, size_t size)
/* Decompress the imploded data using coded literals and an 8K sliding
   window. */
{
    size_t s;			/* bytes to decompress */
    ulg e;	/* table entry flag/number of extra bits */
    ulg n, d;		/* length and index for copy */
    ulg w;			/* current window position */
    Huff_t *t;		/* pointer to table entry */
    ulg u;			/* true if unflushed */
    size_t j;
    Huff_t *tb, *tl, *td;	/* literal, length, and distance tables */
    int bb, bl, bd;		/* number of bits decoded by those */

    tb = state->tb;
    tl = state->tl;
    td = state->td;
    bb = state->bb;
    bl = state->bl;
    bd = state->bd;

    /* explode the coded data */
    s = state->s;
    w = state->w;
    u = state->u;
    j = 0;

    while(s > 0)		/* do until ucsize bytes uncompressed */
    {
	NEEDBITS(state, 1);
	if(state->bit_buf & 1) /* then literal--decode it */
	{
	    DUMPBITS(state, 1);
	    s--;
	    NEEDBITS(state, (ulg)bb); /* get coded literal */
	    t = tb + IGETBITS(state, bb);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    buff[j++] = state->slide[w++] = (uch)t->v.n;
	    if(w == WSIZE)
		w = u = 0;
	    if(j == size)
	    {
		state->u = u;
		state->w = w;
		state->s = s;
		return size;
	    }
	}
	else			/* else distance/length */
	{
	    DUMPBITS(state, 1);
	    NEEDBITS(state, 7);		/* get distance low bits */
	    d = GETBITS(state, 7);
	    DUMPBITS(state, 7);
	    NEEDBITS(state, (ulg)bd);	/* get coded distance high bits */
	    t = td + IGETBITS(state, bd);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    d = w - d - t->v.n;       /* construct offset */
	    NEEDBITS(state, (ulg)bl);    /* get coded length */
	    t = tl + IGETBITS(state, bl);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    n = t->v.n;
	    if(e)                    /* get length extra bits */
	    {
		NEEDBITS(state, 8);
		n += GETBITS(state, 8);
		DUMPBITS(state, 8);
	    }

	    /* do the copy */
	    s -= n;
	    while(n > 0 && j < size)
	    {
		n--;
		d &= WSIZE - 1;
		w &= WSIZE - 1;
		if(u && w <= d)
		{
		    buff[j++] = 0;
		    w++;
		    d++;
		}
		else
		    buff[j++] = state->slide[w++] = state->slide[d++];
		if(w == WSIZE)
		    w = u = 0;
	    }
	    if(j == size)
	    {
		state->u = u;
		state->n = n;
		state->d = d;
		state->w = w;
		state->s = s;
		return size;
	    }
	    state->n = 0;
	}
    }

    state->n = 0;
    state->w = 0;
    state->eof = 1;
    return j;
}



static ssize_t explode_lit4(State_t* state, char *buff, size_t size)
/* Decompress the imploded data using coded literals and a 4K sliding
   window. */
{
    size_t s;               /* bytes to decompress */
    ulg e;  /* table entry flag/number of extra bits */
    ulg n, d;        /* length and index for copy */
    ulg w;           /* current window position */
    Huff_t *t;       /* pointer to table entry */
    ulg u;           /* true if unflushed */
    size_t j;
    Huff_t *tb, *tl, *td;	/* literal, length, and distance tables */
    int bb, bl, bd;		/* number of bits decoded by those */

    tb = state->tb;
    tl = state->tl;
    td = state->td;
    bb = state->bb;
    bl = state->bl;
    bd = state->bd;

  /* explode the coded data */
    s = state->s;
    w = state->w;
    u = state->u;
    j = 0;

    while(s > 0)                 /* do until ucsize bytes uncompressed */
    {
	NEEDBITS(state, 1);
	if(state->bit_buf & 1)                  /* then literal--decode it */
	{
	    DUMPBITS(state, 1);
	    s--;
	    NEEDBITS(state, (ulg)bb);    /* get coded literal */
	    t = tb + IGETBITS(state, bb);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
	    }
	    DUMPBITS(state, t->b);
	    buff[j++] = state->slide[w++] = (uch)t->v.n;
	    if(w == WSIZE)
		w = u = 0;
	    if(j == size)
	    {
		state->u = u;
		state->w = w;
		state->s = s;
		return size;
	    }
	}
	else                        /* else distance/length */
	{
	    DUMPBITS(state, 1);
	    NEEDBITS(state, 6);               /* get distance low bits */
	    d = GETBITS(state, 6);
	    DUMPBITS(state, 6);
	    NEEDBITS(state, (ulg)bd);    /* get coded distance high bits */
	    t = td + IGETBITS(state, bd);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    d = w - d - t->v.n;       /* construct offset */
	    NEEDBITS(state, (ulg)bl);    /* get coded length */
	    t = tl + IGETBITS(state, bl);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    n = t->v.n;
	    if(e)                    /* get length extra bits */
	    {
		NEEDBITS(state, 8);
		n += GETBITS(state, 8);
		DUMPBITS(state, 8);
	    }

	    /* do the copy */
	    s -= n;
	    while(n > 0 && j < size)
	    {
		n--;
		d &= WSIZE - 1;
		w &= WSIZE - 1;
		if(u && w <= d)
		{
		    buff[j++] = 0;
		    w++;
		    d++;
		}
		else
		    buff[j++] = state->slide[w++] = state->slide[d++];
		if(w == WSIZE)
		    w = u = 0;
	    }
	    if(j == size)
	    {
		state->u = u;
		state->n = n;
		state->d = d;
		state->w = w;
		state->s = s;
		return size;
	    }
	    state->n = 0;
	}
    }

    state->n = 0;
    state->w = 0;
    state->eof = 1;
    return j;
}



static ssize_t explode_nolit8(State_t* state, char *buff, size_t size)
/* Decompress the imploded data using uncoded literals and an 8K sliding
   window. */
{
    size_t s;               /* bytes to decompress */
    ulg e;  /* table entry flag/number of extra bits */
    ulg n, d;        /* length and index for copy */
    ulg w;           /* current window position */
    Huff_t *t;       /* pointer to table entry */
    ulg u;           /* true if unflushed */
    size_t j;
    Huff_t *tl, *td;   /* length and distance state tables */
    int bl, bd;             /* number of bits decoded by tl[] and td[] */

    tl = state->tl;
    td = state->td;
    bl = state->bl;
    bd = state->bd;

  /* explode the coded data */

    s = state->s;
    w = state->w;
    u = state->u;
    j = 0;

    while(s > 0)                 /* do until ucsize bytes uncompressed */
    {
	NEEDBITS(state, 1);
	if(state->bit_buf & 1) /* then literal--get eight bits */
	{
	    DUMPBITS(state, 1);
	    s--;
	    NEEDBITS(state, 8);
	    buff[j++] = state->slide[w++] = (uch)state->bit_buf;;
	    DUMPBITS(state, 8);
	    if(w == WSIZE)
		w = u = 0;
	    if(j == size)
	    {
		state->u = u;
		state->w = w;
		state->s = s;
		return size;
	    }
	}
	else                        /* else distance/length */
	{
	    DUMPBITS(state, 1);
	    NEEDBITS(state, 7);               /* get distance low bits */
	    d = GETBITS(state, 7);
	    DUMPBITS(state, 7);
	    NEEDBITS(state, (ulg)bd);    /* get coded distance high bits */
	    t = td + IGETBITS(state, bd);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    d = w - d - t->v.n;       /* construct offset */
	    NEEDBITS(state, (ulg)bl);    /* get coded length */
	    t = tl + IGETBITS(state, bl);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    n = t->v.n;
	    if(e)                    /* get length extra bits */
	    {
		NEEDBITS(state, 8);
		n += GETBITS(state, 8);
		DUMPBITS(state, 8);
	    }

	    /* do the copy */
	    s -= n;
	    while(n > 0 && j < size)
	    {
		n--;
		d &= WSIZE - 1;
		w &= WSIZE - 1;
		if(u && w <= d)
		{
		    buff[j++] = 0;
		    w++;
		    d++;
		}
		else
		    buff[j++] = state->slide[w++] = state->slide[d++];
		if(w == WSIZE)
		    w = u = 0;
	    }
	    if(j == size)
	    {
		state->u = u;
		state->n = n;
		state->d = d;
		state->w = w;
		state->s = s;
		return size;
	    }
	    state->n = 0;
	}
    }

    state->n = 0;
    state->w = 0;
    state->eof = 1;
    return j;
}



static ssize_t explode_nolit4(State_t* state, char *buff, size_t size)
/* Decompress the imploded data using uncoded literals and a 4K sliding
   window. */
{
    size_t s;               /* bytes to decompress */
    ulg e;  /* table entry flag/number of extra bits */
    ulg n, d;        /* length and index for copy */
    ulg w;           /* current window position */
    Huff_t *t;       /* pointer to table entry */
    ulg u;           /* true if unflushed */
    size_t j;
    Huff_t *tl, *td;   /* length and distance state tables */
    int bl, bd;             /* number of bits decoded by tl[] and td[] */

    tl = state->tl;
    td = state->td;
    bl = state->bl;
    bd = state->bd;

  /* explode the coded data */
    s = state->s;
    w = state->w;
    u = state->u;
    j = 0;

    while(s > 0)                 /* do until ucsize bytes uncompressed */
    {
	NEEDBITS(state, 1);
	if(state->bit_buf & 1) /* then literal--get eight bits */
	{
	    DUMPBITS(state, 1);
	    s--;
	    NEEDBITS(state, 8);
	    buff[j++] = state->slide[w++] = (uch)state->bit_buf;
	    DUMPBITS(state, 8);
	    if(w == WSIZE)
		w = u = 0;
	    if(j == size)
	    {
		state->u = u;
		state->w = w;
		state->s = s;
		return size;
	    }
	}
	else                        /* else distance/length */
	{
	    DUMPBITS(state, 1);
	    NEEDBITS(state, 6);               /* get distance low bits */
	    d = GETBITS(state, 6);
	    DUMPBITS(state, 6);
	    NEEDBITS(state, (ulg)bd);    /* get coded distance high bits */
	    t = td + IGETBITS(state, bd);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    d = w - d - t->v.n;       /* construct offset */
	    NEEDBITS(state, (ulg)bl);    /* get coded length */
	    t = tl + IGETBITS(state, bl);
	    e = t->e;
	    while(e > 16)
	    {
		if(e == 99)
		    return -1;
		DUMPBITS(state, t->b);
		e -= 16;
		NEEDBITS(state, e);
		t = t->v.t + IGETBITS(state, e);
		e = t->e;
	    }
	    DUMPBITS(state, t->b);
	    n = t->v.n;
	    if(e)                    /* get length extra bits */
	    {
		NEEDBITS(state, 8);
		n += GETBITS(state, 8);
		DUMPBITS(state, 8);
	    }

	    /* do the copy */
	    s -= n;
	    while(n > 0 && j < size)
	    {
		n--;
		d &= WSIZE - 1;
		w &= WSIZE - 1;
		if(u && w <= d)
		{
		    buff[j++] = 0;
		    w++;
		    d++;
		}
		else
		    buff[j++] = state->slide[w++] = state->slide[d++];
		if(w == WSIZE)
		    w = u = 0;
	    }
	    if(j == size)
	    {
		state->u = u;
		state->n = n;
		state->d = d;
		state->w = w;
		state->s = s;
		return size;
	    }
	    state->n = 0;
	}
    }

    state->n = 0;
    state->w = 0;
    state->eof = 1;
    return j;
}

static int
implode_open(Codex_t* p, char* const args[], Codexnum_t flags)
{
    State_t* state;
    Vmalloc_t* vm;
    char* const* a;

    if (!(vm = vmopen(Vmdcheap, Vmbest, 0)))
	return -1;
    if (!(state = newof(0, State_t, 1, 0)))
    {
	vmclose(vm);
	return -1;
    }
    for (a = args; *a; a++)
	switch (**a)
	{
	case 'l':
		state->method |= EXPLODE_LIT;
		break;
	case '8':
		state->method |= EXPLODE_BIG;
		break;
	}
    switch (state->method)
    {
    case 0:
	state->explodef = explode_nolit4;
	break;
    case EXPLODE_BIG:
	state->explodef = explode_nolit8;
	break;
    case EXPLODE_LIT:
	state->explodef = explode_lit4;
	break;
    case EXPLODE_LIT|EXPLODE_BIG:
	state->explodef = explode_lit8;
	break;
    }
    state->codex = p;
    p->data = state;
    return 0;
}

static int
implode_close(Codex_t* p)
{
    State_t* state = (State_t*)p->data;

    if (!state)
        return -1;
    if (state->vm)
	vmclose(state->vm);
    free(state);
    return 0;
}

static int
implode_init(Codex_t* p)
{
    State_t*	state = (State_t*)p->data;

    vmclear(state->vm);
    state->ip = state->ie = 0;
    state->eof = 0;
    state->u = 1;
    state->s = p->size;

  /* Tune base table sizes.  Note: I thought that to truly optimize speed,
     I would have to select different bl, bd, and bb values for different
     compressed file sizes.  I was suprised to find out the the values of
     7, 7, and 9 worked best over a very wide range of sizes, except that
     bd = 8 worked marginally better for large compressed sizes. */
    state->bl = 7;
    state->bd = (state->s > 200000L ? 8 : 7);

    /* With literal tree--minimum match length is 3 */
    if(state->method & EXPLODE_LIT)
    {
	state->bb = 9;	/* base table size for literals */
	if(get_tree(state, state->l, 256) != 0)
	    return -1;

	if(huff(state->l, 256, 256, NULL, NULL,
		      &state->tb, &state->bb, state->vm) != 0)
	    return -1;

	if(get_tree(state, state->l, 64) != 0)
	    return -1;

	if(huff(state->l, 64, 0, cplen3, extra,
		      &state->tl, &state->bl, state->vm) != 0)
	    return -1;

	if(get_tree(state, state->l, 64) != 0)
	    return -1;

	if(state->method & EXPLODE_BIG)
	{
	    if(huff(state->l, 64, 0, cpdist8, extra,
			  &state->td, &state->bd, state->vm) != 0)
		return -1;
	}
	else
	{
	    if(huff(state->l, 64, 0, cpdist4, extra,
			  &state->td, &state->bd, state->vm) != 0)
		return -1;
	}
    }
    else
    {
	if(get_tree(state, state->l, 64) != 0)
	    return -1;

	if(huff(state->l, 64, 0, cplen2, extra,
		      &state->tl, &state->bl, state->vm) != 0)
	    return -1;

	if(get_tree(state, state->l, 64) != 0)
	    return -1;

	if(state->method & EXPLODE_BIG)
	{
	    if(huff(state->l, 64, 0, cpdist8, extra,
			  &state->td, &state->bd, state->vm) != 0)
		return -1;
	}
	else
	{
	    if(huff(state->l, 64, 0, cpdist4, extra,
			  &state->td, &state->bd, state->vm) != 0)
		return -1;
	}
    }
    return 0;
}

/* Explode an imploded compressed stream.  Based on the general purpose
   bit flag, decide on coded or uncoded literals, and an 8K or 4K sliding
   window.  Construct the literal (if any), length, and distance codes and
   the tables needed to decode them (using huff() from inflate.c),
   and call the appropriate routine for the type of data in the remainder
   of the stream.  The four routines are nearly identical, differing only
   in whether the literal is decoded or simply read in, and in how many
   bits are read in, uncoded, for the low distance bits. */

static ssize_t
implode_read(Sfio_t* sp, void* buff, size_t size, Sfdisc_t* disc)
{
    State_t* state = (State_t*)CODEX(disc)->data;
    size_t j, i;

    if(size <= 0)
	return size;

    j = 0;
    while(j < size)
    {
	if(state->n > 0) /* do the copy */
	{
	    ulg u, n, w, d;

	    u = state->u;
	    n = state->n;
	    d = state->d;
	    w = state->w;
	    while(n > 0 && j < size)
	    {
		n--;
		d &= WSIZE - 1;
		w &= WSIZE - 1;
		if(u && w <= d)
		{
		    ((char*)buff)[j++] = 0;
		    w++;
		    d++;
		}
		else
		    ((char*)buff)[j++] = state->slide[w++] = state->slide[d++];
		if(w == WSIZE)
		    w = u = 0;
	    }

	    state->u = u;
	    state->n = n;
	    state->d = d;
	    state->w = w;
	    if(j == size)
		return size;
	}

	/* state->n == 0 */
	if(state->eof)
	    return j;

	i = (*state->explodef)(state, (char*)buff + j, size - j);
	if(i == -1)
	    return -1;
	j += i;
    }
    return j;
}

Codexmeth_t	codex_zip_implode =
{
	"implode",
	"zip implode compression (PKZIP method 6). Options are window size"
	" { 4K 8K } and \bliteral\b for literal coding.",
	0,
	CODEX_DECODE|CODEX_COMPRESS,
	0,
	0,
	implode_open,
	implode_close,
	implode_init,
	0,
	implode_read,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};
