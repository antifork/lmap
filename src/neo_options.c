/*
 * neo_options
 * 
 * Copyright (c) 2002 Bonelli Nicola <bonelli@blackhats.it>
 *
 * All rights reserved.
 * 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define  CVSID          "$Id$"
#define  VERSION        "1.1"
#define  AUTHORS        "Copyright (c) 2002 Bonelli Nicola <bonelli@blackhats.it>"

#define   NEO_LIBRARY
#include "neo_options.h"

#define RSIZE	(80 bit)

static int
check_neo_options (struct neo_options *n)
{
    REG reg[RSIZE];

    REG_CLR (reg, RSIZE);

    for (; n->opt != 0; n++)
	switch (n->opt) {

	 case '-':
	 case '+':
	     continue;
	 default:

	     if (REG_BT (reg, (n->opt - '0')))
		 FATAL ("-%c opt error. option already in use", n->opt);
	     REG_BS (reg, (n->opt - '0'));
	}


    return 0;
}


static int
compile_optmask (char c, char *s, REG r[], REG m[])
{

    int ret = 0,
        slash = 0,
        i;

    REG_CLR (r, RSIZE);
    REG_CLR (m, RSIZE);

    if (s == NULL)
	return 0;

    for (i = 0; s[i] != 0; i++) {

	switch (s[i]) {

	 case '/':
	     if (slash != 0)
		 FATAL ("-%c \"%s\" error bad optmask", c, s);	/* error */
	     slash++;
	     continue;
	 default:
	     REG_BS ((slash == 0 ? r : m), (s[i] - '0'));
	     ret |= (slash == 0 ? 2 : 1);
	}

    }

    return (ret);
}


static char *
compile_optstring (struct neo_options *n)
{

    struct neo_options *m;
    char *op;
    int l = 0;

    for (m = n; m->opt != 0; m++)
	l += (m->has_arg > 1 ? 2 : 1);
    op = (char *) malloc (l + 1);
    op[l] = 0;
    for (m = n, l = 0; m->opt != 0; m++)

	switch (m->has_arg) {
	 case 0:
	     continue;
	 case 1:
	     op[l++] = m->opt;
	     continue;
	 default:
	     op[l++] = m->opt;
	     op[l++] = ':';
	}


    return op;
}


static char spaces[32] = {[0 ... 31] = ' ' };

/*** 
public functions 
***/

int
neo_usage (FILE * f, char *h, struct neo_options *n)
{
    int br = 1;

    if (n == NULL)
	FATAL ("NULL pointer?");
/* print header */
    fprintf (f, "%s\n", h);

    for (; n->opt != 0; n++)

	switch (n->opt) {

	 case '-':
	     fprintf (f, " %s\n", n->usage);
	     continue;
	 case '+':
	     continue;
	 default:
	     if (n->usage != NULL) {
		 if (n->arg == NULL) {
		     fprintf (f, "   -%c                           %s\n", n->opt, n->usage);
		     break;
		 }
		 spaces[ (sizeof (spaces) - br) & 0x1f] = ' ';
		 fprintf (f, "   -%c %s%n", n->opt, n->arg, &br);
		 spaces[ (sizeof (spaces) - br) & 0x1f] = '\0';
		 fprintf (f, "%s%s\n", spaces, n->usage);
	     }
	}


    return 0;
}


int
neo_showdepend (FILE * f, struct neo_options *n)
{

    if (n == NULL)
	FATAL ("NULL pointer?");
/* print header */

    fprintf (f, "option dependencies:\n");

    for (; n->opt != 0; n++)

	switch (n->opt) {

	 case '-':
	 case '@':
	     continue;
	 case '+':
	     if (n->depend != NULL)
		 fprintf (f, "      (or)                      \"%s\"\n", n->depend);
	     continue;
	 default:
	     if (n->depend != NULL)
		 fprintf (f, "   -%c (and)                     \"%s\"\n", n->opt, n->depend);
	}


    return 0;
}


int
neo_getopt (int argc, char *const argv[], struct neo_options *n)
{

    static REG master[RSIZE];
    static char *optstring;
    REG opts[RSIZE],
        mask[RSIZE],
        temp[RSIZE];
    int c;
    int d;

    if (optstring == NULL) {

/* init */
	check_neo_options (n);
	optstring = compile_optstring (n);
    }

    c = getopt (argc, argv, optstring);
    REG_BS (master, (c - '0'));
    if (c == EOF) {

	for (; n->opt != 0; n++) {
	    switch (n->opt) {

/* comment line */
	     case '-':
		 continue;
/* obligation mask */
	     case '+':
		 REG_CPY (master, temp, RSIZE);
		 d = compile_optmask (n->opt, n->depend, opts, mask);

		 switch (d) {
		  /* NULL/NULL */ case 0:
		      /* empty oblimask */
		      continue;
		  /* NULL/OK */ case 1:
		      /* absolute obligation */
		      break;
		  /* OK/NULL */ case 2:
		      /* oblimask error. ignoring it... */
		      continue;
		  /* OK/OK */ case 3:
		      /* control whether to check the obligation or not */

		      REG_AND (temp, opts, RSIZE);

		      if (REG_ISNULL (opts, RSIZE))
			  continue;
		 }

/* real oblimask checking */

		 REG_AND (mask, temp, RSIZE);
		 if (REG_ISNULL (temp, RSIZE))
		     FATAL ("obligation mask \"%s\" error. (missing options?)", n->depend);
		 continue;
/* classic optmask */
	     default:
		 if (REG_BT (master, (n->opt - '0')) == 0)
		     continue;	/* the current options wasn't set, its optmask is ignored */
		 REG_CPY (master, temp, RSIZE);
		 /* be sure that the current options is set */
		 REG_BS (temp, (n->opt - '0'));
		 compile_optmask (n->opt, n->depend, opts, mask);
		 REG_AND (mask, temp, RSIZE);
		 if (!REG_CMP (temp, opts, RSIZE))
		     FATAL ("-%c optmask \"%s\" error. (mismatch dependency?)", n->opt, n->depend);	/* error */
		 continue;
	    }

	}

    }

    return (c);
}


#ifdef TEST_NEOOPT

struct neo_options opt[] = {
    {'-', 0, 0, NULL, "first section"},
    {'a', no_argument, "a/a", NULL, "test"},
    {'b', no_argument, "bc/bc", NULL, "test.."},
    {'-', 0, 0, NULL, "second section"},
    {'c', required_argument, "cd/cd", "int", "test..."},
    {'d', required_argument, "d/d", "u_int", "test...."},
    {'e', no_argument, "", NULL, "---"},
    {'h', no_argument, "", NULL, "print help"},
    {'@', no_argument, NULL, NULL, "show dependencies"},
    {'+', 0, "a/de", 0},
    {'+', 0, "/dz", 0},
    {0, 0, 0, 0}
};

int
main (int argc, char **argv)
{

    int i;

    if (argc < 2) {
	neo_usage (stderr, "usage: neo_getopt model usage", opt);
	exit (1);
    }

    while ((i = neo_getopt (argc, argv, opt)) != EOF)
	switch (i) {
	 case '?':
	     exit (-1);
	     break;

	 case 'h':
	     neo_usage (stderr, "usage: neo_getopt model usage", opt);
	     exit (1);
	     break;
	 case '@':
	     neo_showdepend (stderr, opt);
	     exit (1);
	     break;
	 default:
	     printf ("opt: %-c given\n", i);
	     break;
	}

    return 0;
}

#endif
