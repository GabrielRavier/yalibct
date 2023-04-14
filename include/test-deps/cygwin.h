// Derived from code with this license:
/*
 * Copyright (c) 2000 Silicon Graphics, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 * Mountain View, CA  94043, or:
 *
 * http://www.sgi.com
 *
 * For further information regarding this notice, see:
 *
 * http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 */

#pragma once

#include "test-lib/chdir-to-tmpdir.h"
#include "test-lib/hedley.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>

/***********************************************************************
 * Define option_t structure type.
 * Entries in this struct are used by the parse_opts routine
 * to indicate valid options and return option arguments
 ***********************************************************************/
typedef struct {
  const char *option;   /* Valid option string (one option only) like "a:" */
  int  *flag;       /* pointer to location to set true if option given */
  char **arg;       /* pointer to location to place argument, if needed */
} option_t;

char **STD_opt_arr = NULL;	/* array of option strings */

struct std_option_t {
    const char *optstr;
    const char *help;
    char *flag;
    char **arg;
} std_options[] = {
    { "c:", "  -c n    Run n copies concurrently\n", NULL, NULL},
    { "e" , "  -e      Turn on errno logging\n", NULL, NULL},
    { "f" , "  -f      Turn off functional testing\n", NULL, NULL},
    { "h" , "  -h      Show this help screen\n", NULL, NULL},
    { "i:", "  -i n    Execute test n times\n", NULL, NULL},
    { "I:", "  -I x    Execute test for x seconds\n", NULL, NULL},
    { "p" , "  -p      Pause for SIGUSR1 before starting\n", NULL, NULL},
    { "P:", "  -P x    Pause for x seconds between iterations\n", NULL, NULL},
    { "t" , "  -t      Turn on syscall timing\n", NULL, NULL},
    {NULL, NULL, NULL, NULL}};

int    STD_nopts=0,		/* number of elements in STD_opt_arr */
       STD_argind=1;		/* argv index to next argv element */
				/* (first argument) */
				/* To getopt users, it is like optind */

/*
 * Define bits for options that might have env variable default
 */
#define  OPT_iteration 		01
#define  OPT_nofunccheck	02
#define  OPT_duration		04
#define  OPT_delay		010
#define  OPT_copies		020

float STD_LOOP_DURATION=0.0,    /* duration value in fractional seconds */
      STD_LOOP_DELAY=0.0;	/* loop delay value in fractional seconds */

/* Define flags and args for standard options */
int STD_FUNCTIONAL_TEST=1,	/* flag indicating to do functional testing code */
    STD_TIMING_ON=0,		/* flag indicating to print timing stats */
    STD_PAUSE=0,		/* flag indicating to pause before actual start, */
				/* for contention mode */
    STD_INFINITE=0,		/* flag indciating to loop forever */
    STD_LOOP_COUNT=1,		/* number of iterations */
    STD_COPIES=1,		/* number of copies */
    STD_ERRNO_LOG=0;		/* flag indicating to do errno logging */

/*********************************************************************
 * STD_opts_help() - return a help string for the STD_OPTIONS.
 *********************************************************************/
void
STD_opts_help()
{
    int i;

    for(i = 0; std_options[i].optstr; ++i) {
	if (std_options[i].help)
	    assert(fputs(std_options[i].help, stdout) >= 0);
    }
}

/*********************************************************************
 * print_help() - print help message and user help message
 *********************************************************************/
void print_help(void (*user_help)())
{
    STD_opts_help();

    if (user_help) user_help();
}

/* define the string length for Mesg and Mesg2 strings */
#define STRLEN 2048

static char Mesg2[STRLEN];	/* holds possible return string */

static int  Debug=0;

/*
 * The following define contains the name of an environmental variable
 * that can be used to specify the number of iterations.
 * It is supported in parse_opts.c and USC_setup.c.
 */
#define USC_ITERATION_ENV       "USC_ITERATIONS"

/*
 * The following define contains the name of an environmental variable
 * that can be used to specify to iteration until desired time
 * in floating point seconds has gone by.
 * Supported in USC_setup.c.
 */
#define USC_LOOP_WALLTIME   "USC_LOOP_WALLTIME"

/*
 * The following define contains the name of an environmental variable
 * that can be used to specify that no functional checks are wanted.
 * It is supported in parse_opts.c and USC_setup.c.
 */
#define USC_NO_FUNC_CHECK   "USC_NO_FUNC_CHECK"

/*
 * The following define contains the name of an environmental variable
 * that can be used to specify the delay between each loop iteration.
 * The value is in seconds (fractional numbers are allowed).
 * It is supported in parse_opts.c.
 */
#define USC_LOOP_DELAY      "USC_LOOP_DELAY"

#ifndef USC_COPIES
#define USC_COPIES   "USC_COPIES"
#endif

/*
 * The following variables are to support system testing additions.
 */
static int  STD_TP_barrier=0;	/* flag to do barrier in TEST_PAUSE */
				/* 2 - wait_barrier(), 3 - set_barrier(), * - barrier() */

static int  STD_LP_barrier=0;	/* flag to do barrier in TEST_LOOPING */
				/* 2 - wait_barrier(), 3 - set_barrier(), * - barrier() */
static int  STD_TP_shmem_sz=0;	/* shmalloc this many words per pe in TEST_PAUSE */
static int  STD_LD_shmem=0; 	/* flag to do shmem_puts and shmem_gets during delay */
static int  STD_LP_shmem=0; 	/* flag to do shmem_puts and gets during TEST_LOOPING */
static int  STD_LD_recfun=0;	/* do recressive function calls in loop delay */
static int  STD_LP_recfun=0;	/* do recressive function calls in TEST_LOOPING */
static int  STD_TP_sbrk=0;	/* do sbrk in TEST_PAUSE */
static int  STD_LP_sbrk=0;	/* do sbrk in TEST_LOOPING */
static char *STD_start_break=0; /* original sbrk size */

/*
 * Structure for usc_recressive_func argument
 */
struct usc_bigstack_t {
   char space[4096];
};

static struct usc_bigstack_t *STD_bigstack=NULL;

/**********************************************************************
 * parse_opts:
 **********************************************************************/
const char *
parse_opts(int ac, char **av, option_t *user_optarr, void (*uhf)())
{
    int found;		/* flag to indicate that an option specified was */
			/* found in the user's list */
    int k;		/* scratch integer for returns and short time usage */
    float  ftmp;	/* tmp float for parsing env variables */
    char *ptr;		/* used in getting env variables */
    int options=0;	/* no options specified */
    int optstrlen, i;
    char *optionstr;
    int opt;		/* return of getopt */

    /*
     * If not the first time this function is called, release the old STD_opt_arr
     * vector.
     */

    if ( STD_opt_arr != NULL ) {
	free(STD_opt_arr);
	STD_opt_arr=NULL;
    }
    /* Calculate how much space we need for the option string */
    optstrlen = 0;
    for (i = 0; std_options[i].optstr; ++i)
	optstrlen += strlen(std_options[i].optstr);
    if (user_optarr)
	for (i = 0; user_optarr[i].option; ++i) {
	    if (strlen(user_optarr[i].option) > 2)
		return "parse_opts: ERROR - Only short options are allowed";
	    optstrlen += strlen(user_optarr[i].option);
	}
    optstrlen += 1;

    /* Create the option string for getopt */
    optionstr = (char *)malloc(optstrlen);
    if (!optionstr)
	return "parse_opts: ERROR - Could not allocate memory for optionstr";

    optionstr[0] = '\0';

    for (i = 0; std_options[i].optstr; ++i)
	strcat(optionstr, std_options[i].optstr);
    if (user_optarr)
	for (i = 0; user_optarr[i].option; ++i)
	    /* only add the option if it wasn't there already */
	    if (strchr(optionstr, user_optarr[i].option[0]) == NULL)
		strcat(optionstr, user_optarr[i].option);

#if DEBUG > 1
    printf("STD_nopts = %d\n", STD_nopts);
#endif

    /*
     *  Loop through av parsing options.
     */
    while ( (opt = getopt(ac, av, optionstr)) > 0) {

	STD_argind = optind;
#if DEBUG > 0
	printf("parse_opts: getopt returned '%c'\n", opt);
#endif

	switch (opt) {
		case '?': /* Unknown option */
			return "Unknown option";
			break;
		case ':': /* Missing Arg */
			return "Missing argument";
			break;
		case 'i': /* Iterations */
			options |= OPT_iteration;
			STD_LOOP_COUNT = atoi(optarg);
			if (STD_LOOP_COUNT == 0) STD_INFINITE = 1;
			break;
		case 'P': /* Delay between iterations */
			options |= OPT_delay;
			STD_LOOP_DELAY = atof(optarg);
			break;
		case 'I': /* Time duration */
			options |= OPT_duration;
			STD_LOOP_DURATION = atof(optarg);
			if ( STD_LOOP_DURATION == 0.0 ) STD_INFINITE=1;
			break;
		case 'c': /* Copies */
			options |= OPT_copies;
			STD_COPIES = atoi(optarg);
			break;
		case 'f': /* Functional testing */
			STD_FUNCTIONAL_TEST = 0;
			break;
		case 'p': /* Pause for SIGUSR1 */
			STD_PAUSE = 1;
			break;
		case 't': /* syscall timing */
			STD_TIMING_ON = 1;
			break;
		case 'e': /* errno loggin */
			STD_ERRNO_LOG = 1;
			break;
		case 'h': /* Help */
			print_help(uhf);
			exit(0);
			break;
		default:

            /* Check all the user specified options */
            found=0;
	    for(i = 0; user_optarr[i].option; ++i) {

		if (opt == user_optarr[i].option[0]) {
                    /* Yup, This is a user option, set the flag and look for argument */
		    if ( user_optarr[i].flag ) {
                        *user_optarr[i].flag=1;
		    }
                    found++;

		    /* save the argument at the user's location */
                    if ( user_optarr[i].option[strlen(user_optarr[i].option)-1] == ':' ) {
                        *user_optarr[i].arg=optarg;
                    }
                    break;  /* option found - break out of the for loop */
                }
            }
	    /* This condition "should never happen".  SO CHECK FOR IT!!!! */
            if ( ! found ) {
                assert(sprintf(Mesg2,
                               "parse_opts: ERROR - option:\"%c\" NOT FOUND... INTERNAL ERROR", opt) == strlen(Mesg2));
                return(Mesg2);
            }
	}

    }    /* end of while */

    STD_argind = optind;

    /*
     * Turn on debug
     */
    ptr=getenv("USC_DEBUG");
    if ( ptr != NULL ) {
	Debug=1;
        printf("env USC_DEBUG is defined, turning on debug\n");
    }
    ptr=getenv("USC_VERBOSE");
    if ( ptr != NULL ) {
	Debug=1;
        printf("env USC_VERBOSE is defined, turning on debug\n");
    }

    /*
     * If the USC_ITERATION_ENV environmental variable is set to
     * a number, use that number as iteration count (same as -c option).
     * The -c option with arg will be used even if this env var is set.
     */
    ptr=getenv(USC_ITERATION_ENV);
    if ( !(options & OPT_iteration) && ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1) {
            if ( k == 0 ) {   /* if arg is 0, set infinite loop flag */
                STD_INFINITE=1;
		if ( Debug )
		   printf("Using env %s, set STD_INFINITE to 1\n",
			USC_ITERATION_ENV);
            } else {            /* else, set the loop count to the arguement */
                STD_LOOP_COUNT=k;
		if ( Debug )
		   printf("Using env %s, set STD_LOOP_COUNT to %d\n",
			USC_ITERATION_ENV, k);
            }
        }
    }

    /*
     * If the USC_NO_FUNC_CHECK environmental variable is set, we'll
     * unset the STD_FUNCTIONAL_TEST variable.
     */
    ptr=getenv(USC_NO_FUNC_CHECK);
    if ( !(options & OPT_nofunccheck) && ptr != NULL ) {
        STD_FUNCTIONAL_TEST=0; /* Turn off functional testing */
	if ( Debug )
	    printf("Using env %s, set STD_FUNCTIONAL_TEST to 0\n",
		USC_NO_FUNC_CHECK);
    }

    /*
     * If the USC_LOOP_WALLTIME environmental variable is set,
     * use that number as duration (same as -I option).
     * The -I option with arg will be used even if this env var is set.
     */

    ptr=getenv(USC_LOOP_WALLTIME);
    if ( !(options & OPT_duration) && ptr != NULL ) {
        if ( sscanf(ptr, "%f", &ftmp) == 1 && ftmp >= 0.0 ) {
	    STD_LOOP_DURATION=ftmp;
	    if ( Debug )
	        printf("Using env %s, set STD_LOOP_DURATION to %f\n",
		    USC_LOOP_WALLTIME, ftmp);
            if ( STD_LOOP_DURATION == 0.0 ) {   /* if arg is 0, set infinite loop flag */
                STD_INFINITE=1;
	        if ( Debug )
	            printf("Using env %s, set STD_INFINITE to 1\n", USC_LOOP_WALLTIME);
	    }
        }
    }
    ptr=getenv("USC_DURATION");
    if ( !(options & OPT_duration) && ptr != NULL ) {
        if ( sscanf(ptr, "%f", &ftmp) == 1 && ftmp >= 0.0 ) {
	    STD_LOOP_DURATION=ftmp;
	    if ( Debug )
	        printf("Using env USC_DURATION, set STD_LOOP_DURATION to %f\n", ftmp);
            if ( STD_LOOP_DURATION == 0.0 ) {   /* if arg is 0, set infinite loop flag */
                STD_INFINITE=1;
	        if ( Debug )
	            printf("Using env USC_DURATION, set STD_INFINITE to 1\n");
	    }
        }
    }
    /*
     * If the USC_LOOP_DELAY environmental variable is set,
     * use that number as delay in factional seconds (same as -P option).
     * The -P option with arg will be used even if this env var is set.
     */
    ptr=getenv(USC_LOOP_DELAY);
    if ( !(options & OPT_delay) && ptr != NULL ) {
        if ( sscanf(ptr, "%f", &ftmp) == 1 && ftmp >= 0.0 ) {
	    STD_LOOP_DELAY=ftmp;
	    if ( Debug )
		printf("Using env %s, set STD_LOOP_DELAY = %f\n",
		    USC_LOOP_DELAY, ftmp);
        }
    }

    /*
     * If the USC_COPIES environmental variable is set,
     * use that number as copies (same as -c option).
     * The -c option with arg will be used even if this env var is set.
     */
    ptr=getenv(USC_COPIES);
    if ( !(options & OPT_copies) && ptr != NULL ) {
        if ( sscanf(ptr, "%d", &STD_COPIES) == 1 && STD_COPIES >= 0 ) {
	    if ( Debug )
		printf("Using env %s, set STD_COPIES = %d\n",
		    USC_COPIES, STD_COPIES);
        }
    }

    /*
     * The following are special system testing envs to turn on special
     * hooks in the code.
     */
    ptr=getenv("USC_TP_BARRIER");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
	    STD_TP_barrier=k;
	}
        else
	    STD_TP_barrier=1;
	if ( Debug )
	    printf("using env USC_TP_BARRIER, Set STD_TP_barrier to %d\n",
	        STD_TP_barrier);
    }

    ptr=getenv("USC_LP_BARRIER");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
	    STD_LP_barrier=k;
	}
        else
	    STD_LP_barrier=1;
	if ( Debug )
	    printf("using env USC_LP_BARRIER, Set STD_LP_barrier to %d\n",
	        STD_LP_barrier);
    }

    ptr=getenv("USC_TP_SHMEM");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
            STD_TP_shmem_sz=k;
	    if ( Debug )
	        printf("Using env USC_TP_SHMEM, Set STD_TP_shmem_sz to %d\n",
		    STD_TP_shmem_sz);
        }
    }

    ptr=getenv("USC_LP_SHMEM");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
            STD_LP_shmem=k;
	    if ( Debug )
	        printf("Using env USC_LP_SHMEM, Set STD_LP_shmem to %d\n",
		    STD_LP_shmem);
        }
    }

    ptr=getenv("USC_LD_SHMEM");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
            STD_LD_shmem=k;
	    if ( Debug )
	        printf("Using env USC_LD_SHMEM, Set STD_LD_shmem to %d\n",
		    STD_LD_shmem);
        }
    }

    ptr=getenv("USC_TP_SBRK");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
            STD_TP_sbrk=k;
	    if ( Debug )
	        printf("Using env USC_TP_SBRK, Set STD_TP_sbrk to %d\n",
		    STD_TP_sbrk);
        }
    }

    ptr=getenv("USC_LP_SBRK");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
            STD_LP_sbrk=k;
	    if ( Debug )
	        printf("Using env USC_LP_SBRK, Set STD_LP_sbrk to %d\n",
		    STD_LP_sbrk);
        }
    }

    ptr=getenv("USC_LP_RECFUN");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
	    STD_LP_recfun=k;
	    if ( STD_bigstack != (struct usc_bigstack_t *)NULL )
		STD_bigstack=(struct usc_bigstack_t *)
			malloc(sizeof(struct usc_bigstack_t));
	    if ( Debug )
                printf("Using env USC_LP_RECFUN, Set STD_LP_recfun to %d\n",
		    STD_LP_recfun);
        }
    }

    ptr=getenv("USC_LD_RECFUN");
    if ( ptr != NULL ) {
        if ( sscanf(ptr, "%i", &k) == 1 && k >= 0 ) {
	    STD_LD_recfun=k;
	    if ( STD_bigstack != (struct usc_bigstack_t *)NULL )
		STD_bigstack=(struct usc_bigstack_t *)
			malloc(sizeof(struct usc_bigstack_t));
	    if ( Debug )
                printf("Using env USC_LD_RECFUN, Set STD_LD_recfun to %d\n",
		    STD_LD_recfun);
        }
    }

#if UNIT_TEST
    printf("The following variables after option and env parsing:\n");
    printf("STD_FUNCTIONAL_TEST = %d\n", STD_FUNCTIONAL_TEST);
    printf("STD_LOOP_DURATION   = %f\n", STD_LOOP_DURATION);
    printf("STD_LOOP_DELAY      = %f\n", STD_LOOP_DELAY);
    printf("STD_COPIES          = %d\n", STD_COPIES);
    printf("STD_LOOP_COUNT      = %d\n", STD_LOOP_COUNT);
    printf("STD_INFINITE        = %d\n", STD_INFINITE);
    printf("STD_TIMING_ON       = %d\n", STD_TIMING_ON);
    printf("STD_ERRNO_LOG       = %d\n", STD_ERRNO_LOG);
    printf("STD_PAUSE           = %d\n", STD_PAUSE);
#endif

    return((char *) NULL);

}    /* end of parse_opts */

/*
 * Define some useful macros.
 */
#define VERBOSE      1     /* flag values for the T_mode variable */
#define CONDENSE     2
#define NOPASS       3
#define DISCARD      4

#define MAXMESG      80    /* max length of internal messages */
#define USERMESG     2048  /* max length of user message */
#define TRUE         1
#define FALSE        0

/*
 * EXPAND_VAR_ARGS - Expand the variable portion (arg_fmt) of a result
 *                   message into the specified string.
 */
#define EXPAND_VAR_ARGS(arg_fmt, str) {   \
   va_list ap; /* varargs mechanism */    \
                                          \
   if ( arg_fmt != NULL ) {               \
      if ( Expand_varargs == TRUE ) {     \
         va_start(ap, arg_fmt);           \
         assert(vsprintf(str, arg_fmt, ap) == strlen(str));     \
         va_end(ap);                      \
         Expand_varargs = FALSE;          \
      } else {                            \
         strcpy(str, arg_fmt);            \
      }                                   \
   } else {                               \
      str[0] = '\0';                      \
   }                                      \
}  /* EXPAND_VAR_ARGS() */

/*
 * Define some static/global variables.
 */
static FILE *T_out = NULL;    /* tst_res() output file descriptor */
static const char *File;      /* file whose contents is part of result */
static int  T_exitval = 0;    /* exit value used by tst_exit() */
static int  T_mode = VERBOSE; /* flag indicating print mode: VERBOSE, */
                              /* CONDENSE, NOPASS, DISCARD */

static int  Expand_varargs = TRUE;  /* if TRUE, expand varargs stuff */
static char Warn_mesg[MAXMESG];  /* holds warning messages */

/*
 * These are used for condensing output when NOT in verbose mode.
 */
static int  Buffered = FALSE; /* TRUE if condensed output is currently */
                              /* buffered (i.e. not yet printed) */
static char *Last_tcid;       /* previous test case id */
static int  Last_num;         /* previous test case number */
static int  Last_type;        /* previous test result type */
static char *Last_mesg;       /* previous test result message */

#define TPASS    0    /* Test passed flag */
#define TFAIL    1    /* Test failed flag */
#define TBROK    2    /* Test broken flag */
#define TWARN    4    /* Test warning flag */
#define TRETR    8    /* Test retire flag */
#define TINFO    16   /* Test information flag */
#define TCONF    32   /* Test not appropriate for configuration flag */

static void tst_print(const char *tcid, int tnum, int trange, int ttype, const char *tmesg);

/*
 * These globals must be defined in the test.
 */
extern const char *TCID;      /* Test case identifier from the test source */
extern int  TST_TOTAL;  /* Total number of test cases from the test */
                        /* source */

/*
 * cat_file() - Print the contents of a file to standard out.
 */
static void
cat_file(const char *filename)
{
   FILE *fp;                  /* file pointer */
   int  b_read;               /* number of bytes read with read() */
   int  b_written;            /* number of bytes written with write() */
   char buffer[BUFSIZ];       /* read/write buffer; BUFSIZ defined in */
                              /* stdio.h */

#if DEBUG
   printf("IN cat_file\n"); fflush(stdout);
#endif

   /*
    * Open the file for reading.
    */
   fp = fopen(filename, "r"); // NOLINT(cert-sig30-c,bugprone-signal-handler)
   if ( fp == NULL ) {
       assert(sprintf(Warn_mesg,
                      "tst_res(): fopen(%s, \"r\") failed; errno = %d: %s",
                      filename, errno, strerror(errno)) == strlen(Warn_mesg));
      tst_print(TCID, 0, 1, TWARN, Warn_mesg);
      return;
   }  /* if ( fopen(filename, "r") == -1 ) */

   /*
    * While something to read, continue to read blocks.
    * From fread(3) man page:
    *   If an error occurs, or the end-of-file is reached, the return
    *   value is zero.
    */
   errno = 0;
   while ( (b_read = fread((void *)buffer, 1, BUFSIZ, fp)) != (size_t)0 ) { // NOLINT(cert-sig30-c,bugprone-signal-handler)
      /*
       * Write what was read to the result output stream.
       */
       b_written = fwrite((void *)buffer, 1, b_read, T_out); // NOLINT(cert-sig30-c,bugprone-signal-handler)
      if ( b_written !=
           b_read ) {
          assert(sprintf(Warn_mesg,
                         "tst_res(): While trying to cat \"%s\", fwrite() wrote only %d of %d bytes",
                         filename, b_written, b_read) == strlen(Warn_mesg));
         tst_print(TCID, 0, 1, TWARN, Warn_mesg);
         break;
      }  /* if ( b_written != b_read ) */
   }  /* while ( fread() != 0 ) */

   /*
    * Check for an fread() error.
    */
   if ( !feof(fp) ) { // NOLINT(cert-sig30-c,bugprone-signal-handler)
       assert(sprintf(Warn_mesg,
                      "tst_res(): While trying to cat \"%s\", fread() failed, errno = %d: %s",
                      filename, errno, strerror(errno)) == strlen(Warn_mesg));
      tst_print(TCID, 0, 1, TWARN, Warn_mesg);
   }  /* if ( !feof() ) */

   /*
    * Close the file.
    */
   if ( fclose(fp) == EOF ) { // NOLINT(cert-sig30-c,bugprone-signal-handler)
       assert(sprintf(Warn_mesg,
                      "tst_res(): While trying to cat \"%s\", fclose() failed, errno = %d: %s",
                      filename, errno, strerror(errno)) == strlen(Warn_mesg));
      tst_print(TCID, 0, 1, TWARN, Warn_mesg);
   }  /* if ( fclose(fp) == EOF ) */
}  /* cat_file() */

/*
 * tst_print() - Actually print a line or range of lines to the output
 *               stream.
 */
static void
tst_print(const char *tcid, int tnum, int trange, int ttype, const char *tmesg)
{
   char type[5];

#if DEBUG
   printf("IN tst_print: tnum = %d, trange = %d, ttype = %d, tmesg = %s\n",
          tnum, trange, ttype, tmesg);
   fflush(stdout);
#endif

   /*
    * Save the test result type by ORing ttype into the current exit
    * value (used by tst_exit()).  This is already done in tst_res(), but
    * is also done here to catch internal warnings.  For internal warnings,
    * tst_print() is called directly with a case of TWARN.
    */
   T_exitval |= ttype;

   /*
    * If output mode is DISCARD, or if the output mode is NOPASS and
    * this result is not one of FAIL, BROK, or WARN, just return.  This
    * check is necessary even though we check for DISCARD mode inside of
    * tst_res(), since occasionally we get to this point without going
    * through tst_res() (e.g. internal TWARN messages).
    */
   if ( T_mode == DISCARD || (T_mode == NOPASS && ttype != TFAIL &&
                              ttype != TBROK && ttype != TWARN) )
      return;

   /*
    * Fill in the type string according to ttype.
    */
   switch ( ttype ) {
   case TPASS:
      strcpy(type, "PASS");
      break;
   case TFAIL:
      strcpy(type, "FAIL");
      break;
   case TBROK:
      strcpy(type, "BROK");
      break;
   case TRETR:
      strcpy(type, "RETR");
      break;
   case TCONF:
      strcpy(type, "CONF");
      break;
   case TWARN:
      strcpy(type, "WARN");
      break;
   case TINFO:
      strcpy(type, "INFO");
      break;
   default:
      strcpy(type, "????");
      break;
   }  /* switch ( ttype ) */

   /*
    * Build the result line and print it.
    */
   if ( T_mode == VERBOSE ) {
       assert(fprintf(T_out, "%-8s %4d  %s  :  %s\n", tcid, tnum, type, tmesg) >= 0); // NOLINT(cert-sig30-c,bugprone-signal-handler)
   } else {
      /* condense results if a range is specified */
      if ( trange > 1 )
          assert(fprintf(T_out, "%-8s %4d-%-4d  %s  :  %s\n",
                         tcid, tnum, tnum + trange - 1, type, tmesg) >= 0);
      else
          assert(fprintf(T_out, "%-8s %4d       %s  :  %s\n",
                         tcid, tnum, type, tmesg) >= 0);
   }

   /*
    * If tst_res() was called with a file, append file contents to the
    * end of last printed result.
    */
   if ( File != NULL )
      cat_file(File);
   File = NULL;
}  /* tst_print() */

/*
 * These globals may be externed by the test.
 */
int Tst_count = 0;      /* current count of test cases executed; NOTE: */
                        /* Tst_count may be externed by other programs */
int Tst_lptotal = 0;    /* tst_brkloop() external */
int Tst_lpstart = 0;    /* tst_brkloop() external */
int Tst_range = 1;      /* for specifying multiple results */
int Tst_nobuf = 1;      /* this is a no-op; buffering is never done, but */
                        /* this will stay for compatibility reasons */

/* defines for unexpected signal setup routine (set_usig.c) */
#define FORK    1       /* SIGCLD is to be ignored */
#define NOFORK  0       /* SIGCLD is to be caught */
#define DEF_HANDLER 0   /* tells set_usig() to use default signal handler */

/*
 * The following defines are used to control tst_res and t_result reporting.
 */

#define TOUTPUT    "TOUTPUT"        /* The name of the environment variable */
                    /* that can be set to one of the following */
                    /* strings to control tst_res output */
                    /* If not set, TOUT_VERBOSE_S is assumed */

#define TOUT_VERBOSE_S  "VERBOSE"   /* All test cases reported */
#define TOUT_CONDENSE_S "CONDENSE"  /* ranges are used where identical messages*/
                    /* occur for sequential test cases */
#define TOUT_NOPASS_S   "NOPASS"    /* No pass test cases are reported */
#define TOUT_DISCARD_S  "DISCARD"   /* No output is reported */

#define TST_NOBUF   "TST_NOBUF" /* The name of the environment variable */
                    /* that can be set to control whether or not */
                    /* tst_res will buffer output into 4096 byte */
                    /* blocks of output */
                    /* If not set, buffer is done.  If set, no */
                    /* internal buffering will be done in tst_res */
                    /* t_result does not have internal buffering */

/*
 * The following defines are used to control tst_tmpdir, tst_wildcard and t_mkchdir
 */

#define TDIRECTORY  "TDIRECTORY"    /* The name of the environment variable */
                    /* that if is set, the value (directory) */
                    /* is used by all tests as their working */
                    /* directory.  tst_rmdir and t_rmdir will */
                    /* not attempt to clean up. */
                    /* This environment variable should only */
                    /* be set when doing system testing since */
                    /* tests will collide and break and fail */
                    /* because of setting it. */

#define TEMPDIR "/tmp"          /* This is the default temporary directory. */
                    /* The environment variable TMPDIR is */
                    /* used prior to this valid by tempnam(3). */
                    /* To control the base location of the */
                    /* temporary directory, set the TMPDIR */
                    /* environment variable to desired path */

/*
 * The following contains support for error message passing.
 * See test_error.c for details.
 */
#define  TST_ERR_MESG_SIZE      1023    /* max size of error message */
#define  TST_ERR_FILE_SIZE      511     /* max size of module name used by compiler */
#define  TST_ERR_FUNC_SIZE      127     /* max size of func name */

/*
 * check_env() - Check the value of the environment variable TOUTPUT and
 *               set the global variable T_mode.  The TOUTPUT environment
 *               variable should be set to "VERBOSE", "CONDENSE",
 *               "NOPASS", or "DISCARD".  If TOUTPUT does not exist or
 *               is not set to a valid value, the default is "VERBOSE".
 */
static void
check_env()
{
   static int first_time = 1;
   char       *value;      /* value of TOUTPUT environment variable */

#if DEBUG
   printf("IN check_env\n");
   fflush(stdout);
#endif

   if ( !first_time )
      return;

   first_time = 0;

   value = getenv(TOUTPUT); // NOLINT(cert-sig30-c,bugprone-signal-handler)
   if ( value == NULL ) {
      /* TOUTPUT not defined, use default */
      T_mode = VERBOSE;
   } else if ( strcmp(value, TOUT_CONDENSE_S) == 0 ) {
      T_mode = CONDENSE;
   } else if ( strcmp(value, TOUT_NOPASS_S) == 0 ) {
      T_mode = NOPASS;
   } else if ( strcmp(value, TOUT_DISCARD_S) == 0 ) {
      T_mode = DISCARD;
   } else {
      /* default */
      T_mode = VERBOSE;
   }
}  /* check_env() */

/*
 * tst_condense() - Handle test cases in CONDENSE or NOPASS mode (i.e.
 *                  buffer the current result and print the last result
 *                  if different than the current).  If a file was
 *                  specified, print the current result and do not
 *                  buffer it.
 */
static void
tst_condense(int tnum, int ttype, char *tmesg)
{
   const char *file;

#if DEBUG
   printf("IN tst_condense: tcid = %s, tnum = %d, ttype = %d, tmesg = %s\n",
          TCID, tnum, ttype, tmesg);
   fflush(stdout);
#endif

   /*
    * If this result is the same as the previous result, return.
    */
   if ( Buffered == TRUE ) {
      if ( strcmp(Last_tcid, TCID) == 0 && Last_type == ttype &&
           strcmp(Last_mesg, tmesg) == 0 && File == NULL )
         return;

      /*
       * This result is different from the previous result.  First,
       * print the previous result.
       */
      file = File;
      File = NULL;
      tst_print(Last_tcid, Last_num, tnum - Last_num, Last_type,
                Last_mesg);
      free(Last_tcid); // NOLINT(cert-sig30-c,bugprone-signal-handler)
      free(Last_mesg);
      File = file;
   }  /* if ( Buffered == TRUE ) */

   /*
    * If a file was specified, print the current result since we have no
    * way of retaining the file contents for comparing with future
    * results.  Otherwise, buffer the current result info for next time.
    */
   if ( File != NULL ) {
      tst_print(TCID, tnum, Tst_range, ttype, tmesg);
      Buffered = FALSE;
   } else {
      Last_tcid = (char *)malloc(strlen(TCID) + 1); // NOLINT(cert-sig30-c,bugprone-signal-handler)
      strcpy(Last_tcid, TCID);
      Last_num = tnum;
      Last_type = ttype;
      Last_mesg = (char *)malloc(strlen(tmesg) + 1);
      strcpy(Last_mesg, tmesg);
      Buffered = TRUE;
   }
}  /* tst_condense() */

/*
 * tst_res() - Main result reporting function.  Handle test information
 *             appropriately depending on output display mode.  Call
 *             tst_condense() or tst_print() to actually print results.
 *             All result functions (tst_resm(), tst_brk(), etc.)
 *             eventually get here to print the results.
 */
void
tst_res(int ttype, const char *fname, const char *arg_fmt, ...)
{
   int  i;
   char tmesg[USERMESG];     /* expanded message */

#if DEBUG
   printf("IN tst_res; Tst_count = %d; Tst_range = %d\n",
          Tst_count, Tst_range); fflush(stdout);
#endif

   /*
    * Expand the arg_fmt string into tmesg, if necessary.
    */
   EXPAND_VAR_ARGS(arg_fmt, tmesg);

   /*
    * Save the test result type by ORing ttype into the current exit
    * value (used by tst_exit()).
    */
   T_exitval |= ttype;

   /*
    * Unless T_out has already been set by tst_environ(), make tst_res()
    * output go to standard output.
    */
   if ( T_out == NULL )
       T_out = stdout;

   /*
    * Check TOUTPUT environment variable (if first time) and set T_mode
    * flag.
    */
   check_env();

   /*
    * A negative or NULL range is invalid.
    */
   if ( Tst_range <= 0 ) {
      Tst_range = 1;
      tst_print(TCID, 0, 1, TWARN,
                "tst_res(): Tst_range must be positive");
   }

   /*
    * If a filename was specified, set 'File' if it exists.
    */
   if ( fname != NULL && access(fname, F_OK) == 0 )
      File = fname;

   /*
    * Set the test case number and print the results, depending on the
    * display type.
    */
   if ( ttype == TWARN || ttype == TINFO ) {
      /*
       * Handle WARN and INFO results (test case number is 0).
       */
      if ( Tst_range > 1 ) {
         tst_print(TCID, 0, 1, TWARN,
                   "tst_res(): Range not valid for TINFO or TWARN types");
      }
      tst_print(TCID, 0, 1, ttype, tmesg);
   } else {
      /*
       * Handle all other types of results other than WARN and INFO.
       */
      if ( Tst_count < 0 )
         tst_print(TCID, 0, 1, TWARN,
                   "tst_res(): Tst_count < 0 is not valid");

      /*
       * Process each display type.
       */
      switch ( T_mode ) {
      case DISCARD:
         /* do not print any results */
         break;

      case NOPASS:   /* passing result types are filtered by tst_print() */
      case CONDENSE:
         tst_condense(Tst_count + 1, ttype, tmesg);
         break;

      default: /* VERBOSE */
         for ( i = 1 ; i <= Tst_range ; i++ )
             if (ttype != TPASS)
                 tst_print(TCID, Tst_count + i, Tst_range, ttype, tmesg);
         break;
      }  /* end switch() */

      /*
       * Increment Tst_count.
       */
      Tst_count += Tst_range;
   }  /* if ( ttype == TWARN || ttype == TINFO ) */

   /*
    * Reset some values.
    */
   Tst_range = 1;
   Expand_varargs = TRUE;
}  /* tst_res() */

/*
 * tst_flush() - Print any messages pending because of CONDENSE mode,
 *               and flush T_out.
 */
void
tst_flush()
{
#if DEBUG
   printf("IN tst_flush\n");
   fflush(stdout);
#endif

   /*
    * Print out last line if in CONDENSE or NOPASS mode.
    */
   if ( Buffered == TRUE && (T_mode == CONDENSE || T_mode == NOPASS) ) {
      tst_print(Last_tcid, Last_num, Tst_count - Last_num + 1,
                Last_type, Last_mesg);
      Buffered = FALSE;
   }
   assert(fflush(T_out) == 0); // NOLINT(cert-sig30-c,bugprone-signal-handler)
}  /* tst_flush() */

/*
 * tst_exit() - Call exit() with the value T_exitval, set up by
 *              tst_res().  T_exitval has a bit set for most of the
 *              result types that were seen (including TPASS, TFAIL,
 *              TBROK, TWARN, TCONF).  Also, print the last result (if
 *              necessary) before exiting.
 */
HEDLEY_NO_RETURN void
tst_exit()
{
#if DEBUG
   printf("IN tst_exit\n"); fflush(stdout);
   fflush(stdout);
#endif

   /*
    * Call tst_flush() flush any ouput in the buffer or the last
    * result not printed because of CONDENSE mode.
    */
   tst_flush();

   /*
    * Mask out TRETR, TINFO, and TCONF results from the exit status.
    */
   exit(T_exitval & ~(TRETR | TINFO | TCONF)); // NOLINT(cert-sig30-c,bugprone-signal-handler)
}  /* tst_exit() */

/*
 * tst_brk() - Fail or break current test case, and break the remaining
 *             tests cases.
 */
void
tst_brk(int ttype, char *fname, void (*func)(), const char *arg_fmt, ...)
{
   char    tmesg[USERMESG];      /* expanded message */

#if DEBUG
   printf("IN tst_brk\n"); fflush(stdout);
   fflush(stdout);
#endif

   /*
    * Expand the arg_fmt string into tmesg, if necessary.
    */
   EXPAND_VAR_ARGS(arg_fmt, tmesg);

   /*
    * Only FAIL, BROK, CONF, and RETR are supported by tst_brk().
    */
   if ( ttype != TFAIL && ttype != TBROK && ttype != TCONF &&
        ttype != TRETR ) {
       assert(sprintf(Warn_mesg, "tst_brk(): Invalid Type: %d.  Using TBROK",
                      ttype) == strlen(Warn_mesg));
      tst_print(TCID, 0, 1, TWARN, Warn_mesg);
      ttype = TBROK;
   }

   /*
    * Print the first result, if necessary.
    */
   if ( Tst_count < TST_TOTAL )
      tst_res(ttype, fname, tmesg);

   /*
    * Determine the number of results left to report.
    */
   Tst_range = TST_TOTAL - Tst_count;

   /*
    * Print the rest of the results, if necessary.
    */
   if ( Tst_range > 0 ) {
      if ( ttype == TCONF )
         tst_res(ttype, NULL,
                 "Remaining cases not appropriate for configuration");
      else if ( ttype == TRETR )
         tst_res(ttype, NULL, "Remaining cases retired");
      else
         tst_res(TBROK, NULL, "Remaining cases broken");
   } else {
      Tst_range = 1;
      Expand_varargs = TRUE;
   }  /* if ( Tst_range > 0 ) */

   /*
    * If no cleanup function was specified, just return to the caller.
    * Otherwise call the specified function.  If specified function
    * returns, call tst_exit().
    */
   if ( func != NULL ) {
      (*func)();
      tst_exit();
   }
}  /* tst_brk() */

/*
 * tst_brkm() - Interface to tst_brk(), with no filename.
 */
void
tst_brkm(int ttype, void (*func)(), const char *arg_fmt, ...)
{
   char    tmesg[USERMESG];      /* expanded message */

#if DEBUG
   printf("IN tst_brkm\n"); fflush(stdout);
   fflush(stdout);
#endif

   /*
    * Expand the arg_fmt string into tmesg.
    */
   EXPAND_VAR_ARGS(arg_fmt, tmesg);

   /*
    * Call tst_brk with a null filename argument.
    */
   tst_brk(ttype, NULL, func, tmesg);
}  /* tst_brkm() */

/***********************************************************************
 * TEST_EXP_ENOS(array): set the bits associated with the nput errnos
 *  in the TEST_VALID_ENO array.
 *
 * parameters:
 *  int array[]: a zero terminated array of errnos expected.
 *
 ***********************************************************************/
#define TEST_EXP_ENOS(array)                \
    tmptime=0;                      \
    while (array[tmptime] != 0) {           \
    if (array[tmptime] < USC_MAX_ERRNO)     \
        TEST_VALID_ENO[array[tmptime]].flag=1;  \
    tmptime++;                  \
    }

  /***********************************************************************
   * temporary variables for determining max and min times in TEST macro
   ***********************************************************************/
long btime, etime, tmptime;

#define USC_MAX_ERRNO   2000

/*
 * define a structure
 */
struct usc_errno_t {
    int flag;
};

/*
 * Code from usctest.h that not part of this file since we are the library.
 */

struct usc_errno_t TEST_VALID_ENO[USC_MAX_ERRNO];

  /***********************************************************************
   * Globals for returning the return code and errno from the system call
   * test macros.
   ***********************************************************************/
long TEST_RETURN;
int TEST_ERRNO;

  /***********************************************************************
   * temporary variables for determining max and min times in TEST macro
   ***********************************************************************/
long btime, etime, tmptime;

/***********************************************************************
 * TEST_LOOPING now call the usc_test_looping function.
 * The function will return 1 if the test should continue
 * iterating.
 *
 ***********************************************************************/
#define TEST_LOOPING usc_test_looping

#define USECS_PER_SEC   1000000  /* microseconds per second */

/***********************************************************************
 * This function returns the number of get_current_time()'s return
 * per second.
 ***********************************************************************/

static int
get_timepersec()
{
    return  USECS_PER_SEC;   /* microseconds per second */

}

/***********************************************************************
 * this function will get current time in microseconds since 1970.
 ***********************************************************************/
static int
get_current_time()
{
    struct timeval curtime;

    gettimeofday(&curtime, NULL);

    /* microseconds since 1970 */
    return (curtime.tv_sec*USECS_PER_SEC) + curtime.tv_usec;


}

/*
 * This function recressively calls itself max times.
 */
static void
usc_recressive_func(int cnt, int max, struct usc_bigstack_t **bstack)
{
    if ( cnt < max )
    usc_recressive_func(cnt+1, max, bstack);

}

/***********************************************************************
 *
 * This function will determine if test should continue iterating
 * If the STD_INFINITE flag is set, return 1.
 * If the STD_LOOP_COUNT variable is set, compare it against
 * the counter.
 * If the STD_LOOP_DURATION variable is set, compare current time against
 * calculated stop_time.
 * This function will return 1 until all desired looping methods
 * have been met.
 *
 * counter integer is supplied by the user program.
 ***********************************************************************/
int
usc_test_looping(int counter)
{
    static int first_time = 1;
    static int stop_time = 0;   /* stop time in rtc or usecs */
    static int delay;       /* delay in clocks or usecs  */
    int hz=0;           /* clocks per second or usecs per second */
    int ct, end;        /* current time, end delay time */
    int keepgoing=0;        /* used to determine return value */

    /*
     * If this is the first iteration and we are looping for
     * duration of STD_LOOP_DURATION seconds (fractional) or
     * doing loop delays, get the clocks per second.
     */
    if ( first_time ) {

    first_time=0;
    if ( STD_LOOP_DELAY || STD_LOOP_DURATION ) {
        hz = get_timepersec();
    }

    /*
     * If looping for duration, calculate stop time in
     * clocks.
     */

    if ( STD_LOOP_DURATION) {
        ct=get_current_time();
        stop_time=(int)((float)hz * STD_LOOP_DURATION) + ct;
    }

    /*
     * If doing delay each iteration, calcuate the number
     * of clocks for each delay.
     */
    if ( STD_LOOP_DELAY ) {
        delay=(int)((float)hz * STD_LOOP_DELAY);
    }

    }

    /*
     * if delay each iteration, loop for delay clocks.
     * This will not be done on first iteration.
     * The delay will happen before determining if
     * there will be another iteration.
     */
    else if ( STD_LOOP_DELAY ) {
    ct=get_current_time();
        end=ct+delay;
        while ( ct < end ) {
        /*
         * The following are special test hooks in the delay loop.
         */
        if ( STD_LD_recfun ) {
        if ( Debug )
            printf("calling usc_recressive_func(0, %d, &STD_bigstack)\n",
                STD_LD_recfun);
        usc_recressive_func(0, STD_LD_recfun, &STD_bigstack);
        }

        ct=get_current_time();
    }
    }

    if ( STD_INFINITE ) {
    keepgoing++;
    }

    if ( STD_LOOP_COUNT && counter < STD_LOOP_COUNT ) {
    keepgoing++;
    }

    if ( STD_LOOP_DURATION != 0.0 && get_current_time() < stop_time ) {
    keepgoing++;
    }

    if ( keepgoing == 0 )
    return 0;

    /*
     * The following code allows special system testing hooks.
     */

    if ( STD_LP_recfun ) {
    if ( Debug )
        printf("calling usc_recressive_func(0, %d, &STD_bigstack)\n",
            STD_LP_recfun);
    usc_recressive_func(0, STD_LP_recfun, &STD_bigstack);
    }

    if ( STD_LP_sbrk ) {
    if ( Debug )
        printf("about to do sbrk(%d)\n", STD_LP_sbrk);
    assert(sbrk(STD_LP_sbrk) != (void *)-1);
    }


    if ( keepgoing )
    return 1;
    else
        return 0;   /* done - stop iterating */
}

/***********************************************************************
 * TEST: calls a system call
 *
 * parameters:
 *  SCALL = system call and parameters to execute
 *
 ***********************************************************************/
#define TEST(SCALL) TEST_RETURN = SCALL;  TEST_ERRNO=errno;

/***********************************************************************
 * TEST_ERROR_LOG(eno): log this errno if STD_ERRNO_LOG flag set
 *
 * parameters:
 *  int eno: the errno location in STD_ERRNO_LIST to log.
 *
 ***********************************************************************/
#define TEST_ERROR_LOG(eno)     \
    if ( STD_ERRNO_LOG )        \
        if ( eno < USC_MAX_ERRNO )      \
            STD_ERRNO_LIST[eno]++;

/*
 * Counter of errnos returned (-e option).  Indexed by errno.
 * Make the array USC_MAX_ERRNO long.  That is the first Fortran
 * Lib errno.  No syscall should return an errno that high.
 */
int STD_ERRNO_LIST[USC_MAX_ERRNO];

/*
 * tst_resm() - Interface to tst_res(), with no filename.
 */
void
tst_resm(int ttype, const char *arg_fmt, ...)
{
   char    tmesg[USERMESG];      /* expanded message */

#if DEBUG
   printf("IN tst_resm\n"); fflush(stdout);
   fflush(stdout);
#endif

   /*
    * Expand the arg_fmt string into tmesg.
    */
   EXPAND_VAR_ARGS(arg_fmt, tmesg); // NOLINT(cert-sig30-c,bugprone-signal-handler)

   /*
    * Call tst_res with a null filename argument.
    */
   tst_res(ttype, NULL, tmesg);
}  /* tst_resm() */

void (*T_cleanup)();        /* pointer to cleanup function */

/****************************************************************************
 * def_handler() : default signal handler that is invoked when
 *      an unexpected signal is caught.
 ***************************************************************************/

static void
def_handler(int sig)
{
    char mesg[MAXMESG];     /* holds tst_res message */

    /* first reset trap for this signal (except SIGCHLD - its weird) */
    if ((sig != SIGCHLD) && (sig != SIGSTOP) && (sig != SIGCONT)) {
        if (signal(sig, def_handler) == SIG_ERR) {
            (void) sprintf(mesg, "def_handler: signal() failed for signal %d. error:%d %s.", sig, errno, strerror(errno)); // NOLINT(cert-sig30-c,bugprone-signal-handler)
            tst_resm(TWARN, mesg);
        }
    }

    (void) sprintf(mesg, "Unexpected signal %d received.", sig);

    /*
         * Break remaining test cases, do any cleanup, then exit
     */
    tst_brkm(TBROK, 0, mesg);

    /* now cleanup and exit */
    if (T_cleanup) {
        (*T_cleanup)();
    }

    tst_exit();
}

/****************************************************************************
 * tst_sig() : set-up to catch unexpected signals.  fork_flag is set to NOFORK
 *    if SIGCLD is to be an "unexpected signal", otherwise it is set to
 *    FORK.  cleanup points to a cleanup routine to be executed before
 *    tst_exit is called (cleanup is set to NULL if no cleanup is desired).
 *    handler is a pointer to the signal handling routine (if handler is
 *    set to NULL, a default handler is used).
 ***************************************************************************/

void
tst_sig(int fork_flag, void (*handler)(), void (*cleanup)())
{
    (void)fork_flag;
    (void)handler;
    char mesg[MAXMESG];     /* message buffer for tst_res */
    int sig;

    /*
     * save T_cleanup and handler function pointers
     */
    T_cleanup = cleanup;        /* used by default handler */

    // yalibct note: Seems screwed up and getting it actually working seems remarkebly hard - I don't think we need this anyway
#if 0
    if (handler == DEF_HANDLER) {
        /* use default handler */
        handler = def_handler;
    }

    /*
     * now loop through all signals and set the handlers
     */

    for (sig = 1; sig < NSIG; sig++) {
        /*
         * SIGKILL is never unexpected.
         * SIGCLD is only unexpected when
         *    no forking is being done.
         * SIGINFO is used for file quotas and should be expected
         */

        if (sig == SIGKILL ||
            sig == SIGSTOP ||
#ifdef CRAY
            sig == SIGINFO ||
            sig == SIGRECOVERY ||   /* allow chkpnt/restart */
#endif  /* CRAY */

#ifdef SIGSWAP
            sig == SIGSWAP ||
#endif

#ifdef SIGCKPT
            sig == SIGCKPT ||
#endif
#ifdef SIGRESTART
            sig == SIGRESTART ||
#endif
                /*
                 * pthread-private signals SIGPTINTR and SIGPTRESCHED.
                 * Setting a handler for these signals is disallowed when
                 * the binary is linked against libpthread.
                 */
#ifdef SIGPTINTR
            sig == SIGPTINTR ||
#endif /* SIGPTINTR */
#ifdef SIGPTRESCHED
            sig == SIGPTRESCHED ||
#endif /* SIGPTRESCHED */
#ifdef __CYGWIN__
            sig == SIGILL ||
            sig == SIGTRAP ||
            sig == SIGABRT ||
            sig == SIGEMT ||
            sig == SIGFPE ||
            sig == SIGBUS ||
#endif
            false)
            ;
        else if (sig == SIGCHLD) {
            if ( fork_flag == FORK )
                continue;
        } else {
            if (signal(sig, handler) == SIG_ERR) {
                (void) sprintf(mesg,
                "signal() failed for signal %d. error:%d %s.",
                sig, errno, strerror(errno));
                tst_resm(TWARN, mesg);
            }
        }
#ifdef __sgi
        /* On irix  (07/96), signal() fails when signo is 33 or higher */
        if ( sig+1 >= 33 )
        break;
#endif  /*  __sgi */

    } /* endfor */
#endif
}

/***********************************************************************
 * TEST_PAUSE: Pause for SIGUSR1 if the pause flag is set.
 *         Just continue when signal comes in.
 *
 * parameters:
 *  none
 *
 ***********************************************************************/
#define TEST_PAUSE usc_global_setup_hook();

/*
 * routine to goto when we get the SIGUSR1 for STD_PAUSE
 */
void STD_go(int sig)
{
    (void)sig;
}

/***********************************************************************
 * This function will do desired end of global setup test
 * hooks.
 * Currently it will only do a pause waiting for sigusr1 if
 * STD_PAUSE is set.
 *
 ***********************************************************************/
int
usc_global_setup_hook()
{
    int cnt;
    /* temp variable to store old signal action to be restored after pause */
    int (*_TMP_FUNC)(void);

    /*
     * Fork STD_COPIES-1 copies.
     */
    for(cnt=1;cnt<STD_COPIES;cnt++) {
        switch(fork() ) {
        case -1:
            assert(fprintf(stderr, "%s: fork() failed, errno:%d %s\n",
                           __FILE__, errno, strerror(errno)) >= 0);
        break;
        case 0:  /* child */
            cnt=STD_COPIES;   /* to stop the forking */
        break;

        default: /* parent */
        break;
    }
    }

    /*
     * pause waiting for sigusr1.
     */
    if ( STD_PAUSE ) {
        _TMP_FUNC = (int (*)())signal(SIGUSR1, STD_go);
        pause();
        assert(signal(SIGUSR1, (void (*)())_TMP_FUNC) != SIG_ERR);
    }


    if ( STD_TP_sbrk || STD_LP_sbrk) {
    STD_start_break=sbrk(0);    /* get original sbreak size */
    }

    if ( STD_TP_sbrk ) {
    assert(sbrk(STD_TP_sbrk) != (void *)-1);
    if ( Debug )
        printf("after sbrk(%d)\n", STD_TP_sbrk);
    }
    return 0;
}

#define tst_tmpdir yalibct_chdir_to_tmpdir
#define tst_rmdir yalibct_chdir_to_tmpdir_remove_tmpdir

/***********************************************************************
 * TEST_CLEANUP: print system call timing stats and errno log entries
 * to stdout if STD_TIMING_ON and STD_ERRNO_LOG are set, respectively.
 * Do NOT print ANY information if no system calls logged.
 *
 * parameters:
 *  none
 *
 ***********************************************************************/
#define TEST_CLEANUP    \
if ( STD_ERRNO_LOG ) {                      \
    for (tmptime=0; tmptime<USC_MAX_ERRNO; tmptime++) {     \
         if ( STD_ERRNO_LIST[tmptime] ) {           \
             if ( TEST_VALID_ENO[tmptime].flag )            \
             tst_resm(TINFO, "ERRNO %d:\tReceived %d Times",    \
                  tmptime, STD_ERRNO_LIST[tmptime]);    \
             else                           \
             tst_resm(TINFO,                    \
                  "ERRNO %d:\tReceived %d Times ** UNEXPECTED **",  \
                  tmptime, STD_ERRNO_LIST[tmptime]);    \
         }                              \
    }                               \
}

char *
get_high_address(void)
{
#ifdef __CYGWIN__
       return VirtualAlloc (NULL, 4096, MEM_COMMIT, PAGE_NOACCESS) + 2048;
#else
       return (char *)sbrk(0) + 16384;
#endif
}
