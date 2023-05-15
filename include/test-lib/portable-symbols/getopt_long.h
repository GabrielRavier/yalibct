// Derived from code with this license:
/* getopt_long and getopt_long_only entry points for GNU getopt.
   Copyright (C) 1987-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library and is also part of gnulib.
   Patches to this file should be submitted to both projects.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#pragma once

#ifdef YALIBCT_LIBC_HAS_GETOPT_LONG
#include <getopt.h>
#else

#include "test-lib/portable-symbols/struct_option.h"
#include "test-lib/portable-symbols/gettext.h"
#include "test-lib/portable-symbols/opterr.h"
#include "test-lib/portable-symbols/optind.h"
#include "test-lib/portable-symbols/optopt.h"
#include "test-lib/portable-symbols/internal/gnulib/_GL_UNUSED.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The type of the 'argv' argument to getopt_long and getopt_long_only
   is properly 'char **', since both functions may write to the array
   (in order to move all the options to the beginning).  However, for
   compatibility with old versions of LSB, glibc has to use 'char *const *'
   instead.  */
#ifndef __getopt_argv_const
# define __getopt_argv_const const
#endif

/* Reentrant versions which can handle parsing multiple argument
   vectors at the same time.  */

/* Describe how to deal with options that follow non-option ARGV-elements.

   REQUIRE_ORDER means don't recognize them as options; stop option
   processing when the first non-option is seen.  This is what POSIX
   specifies should happen.

   PERMUTE means permute the contents of ARGV as we scan, so that
   eventually all the non-options are at the end.  This allows options
   to be given in any order, even with programs that were not written
   to expect this.

   RETURN_IN_ORDER is an option available to programs that were
   written to expect options and other ARGV-elements in any order
   and that care about the ordering of the two.  We describe each
   non-option ARGV-element as if it were the argument of an option
   with character code 1.

   The special argument '--' forces an end of option-scanning regardless
   of the value of 'ordering'.  In the case of RETURN_IN_ORDER, only
   '--' can cause 'getopt' to return -1 with 'optind' != ARGC.  */

enum __ord
  {
    REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
  };

/* Data type for reentrant functions.  */
struct _getopt_data
{
  /* These have exactly the same meaning as the corresponding global
     variables, except that they are used for the reentrant
     versions of getopt.  */
  int optind;
  int opterr;
  int optopt;
  char *optarg;

  /* Internal members.  */

  /* True if the internal members have been initialized.  */
  int __initialized;

  /* The next char to be scanned in the option-element
     in which the last option character we returned was found.
     This allows us to pick up the scan where we left off.

     If this is zero, or a null string, it means resume the scan
     by advancing to the next ARGV-element.  */
  char *__nextchar;

  /* See __ord above.  */
  enum __ord __ordering;

  /* Handle permutation of arguments.  */

  /* Describe the part of ARGV that contains non-options that have
     been skipped.  'first_nonopt' is the index in ARGV of the first
     of them; 'last_nonopt' is the index after the last of them.  */

  int __first_nonopt;
  int __last_nonopt;
};

/* Keep a global copy of all internal members of getopt_data.  */

static struct _getopt_data getopt_data;

/* For communication from 'getopt' to the caller.
   When 'getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when 'ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

char *optarg;

/* Initialize internal data upon the first call to getopt.  */

static const char *
_getopt_initialize (_GL_UNUSED int argc,
            _GL_UNUSED char **argv, const char *optstring,
            struct _getopt_data *d, int posixly_correct)
{
  /* Start processing options with ARGV-element 1 (since ARGV-element 0
     is the program name); the sequence of previously skipped
     non-option ARGV-elements is empty.  */
  if (d->optind == 0)
    d->optind = 1;

  d->__first_nonopt = d->__last_nonopt = d->optind;
  d->__nextchar = NULL;

  /* Determine how to handle the ordering of options and nonoptions.  */
  if (optstring[0] == '-')
    {
      d->__ordering = RETURN_IN_ORDER;
      ++optstring;
    }
  else if (optstring[0] == '+')
    {
      d->__ordering = REQUIRE_ORDER;
      ++optstring;
    }
  else if (posixly_correct || !!getenv ("POSIXLY_CORRECT"))
    d->__ordering = REQUIRE_ORDER;
  else
    d->__ordering = PERMUTE;

  d->__initialized = 1;
  return optstring;
}

/* Exchange two adjacent subsequences of ARGV.
   One subsequence is elements [first_nonopt,last_nonopt)
   which contains all the non-options that have been skipped so far.
   The other is elements [last_nonopt,optind), which contains all
   the options processed since those non-options were skipped.

   'first_nonopt' and 'last_nonopt' are relocated so that they describe
   the new indices of the non-options in ARGV after they are moved.  */

static void
exchange (char **argv, struct _getopt_data *d)
{
  int bottom = d->__first_nonopt;
  int middle = d->__last_nonopt;
  int top = d->optind;
  char *tem;

  /* Exchange the shorter segment with the far end of the longer segment.
     That puts the shorter segment into the right place.
     It leaves the longer segment in the right place overall,
     but it consists of two parts that need to be swapped next.  */

  while (top > middle && middle > bottom)
    {
      if (top - middle > middle - bottom)
    {
      /* Bottom segment is the short one.  */
      int len = middle - bottom;
      int i;

      /* Swap it with the top part of the top segment.  */
      for (i = 0; i < len; i++)
        {
          tem = argv[bottom + i];
          argv[bottom + i] = argv[top - (middle - bottom) + i];
          argv[top - (middle - bottom) + i] = tem;
        }
      /* Exclude the moved bottom segment from further swapping.  */
      top -= len;
    }
      else
    {
      /* Top segment is the short one.  */
      int len = top - middle;
      int i;

      /* Swap it with the bottom part of the bottom segment.  */
      for (i = 0; i < len; i++)
        {
          tem = argv[bottom + i];
          argv[bottom + i] = argv[middle + i];
          argv[middle + i] = tem;
        }
      /* Exclude the moved top segment from further swapping.  */
      bottom += len;
    }
    }

  /* Update records for the slots the non-options now occupy.  */

  d->__first_nonopt += (d->optind - d->__last_nonopt);
  d->__last_nonopt = d->optind;
}

/* When used standalone, do not attempt to use alloca.  */
# define __libc_use_alloca(size) 0

# define _(msgid) gettext (msgid)

/* Process the argument starting with d->__nextchar as a long option.
   d->optind should *not* have been advanced over this argument.

   If the value returned is -1, it was not actually a long option, the
   state is unchanged, and the argument should be processed as a set
   of short options (this can only happen when long_only is true).
   Otherwise, the option (and its argument, if any) have been consumed
   and the return value is the value to return from _getopt_internal_r.  */
static int
process_long_option (int argc, char **argv, const char *optstring,
             const struct option *longopts, int *longind,
             int long_only, struct _getopt_data *d,
             int print_errors, const char *prefix)
{
  char *nameend;
  size_t namelen;
  const struct option *p;
  const struct option *pfound = NULL;
  int n_options;
  int option_index;

  for (nameend = d->__nextchar; *nameend && *nameend != '='; nameend++)
    /* Do nothing.  */ ;
  namelen = nameend - d->__nextchar;

  /* First look for an exact match, counting the options as a side
     effect.  */
  for (p = longopts, n_options = 0; p->name; p++, n_options++)
    if (!strncmp (p->name, d->__nextchar, namelen)
    && namelen == strlen (p->name))
      {
    /* Exact match found.  */
    pfound = p;
    option_index = n_options;
    break;
      }

  if (pfound == NULL)
    {
      /* Didn't find an exact match, so look for abbreviations.  */
      unsigned char *ambig_set = NULL;
      int ambig_malloced = 0;
      int ambig_fallback = 0;
      int indfound = -1;

      for (p = longopts, option_index = 0; p->name; p++, option_index++)
    if (!strncmp (p->name, d->__nextchar, namelen))
      {
        if (pfound == NULL)
          {
        /* First nonexact match found.  */
        pfound = p;
        indfound = option_index;
          }
        else if (long_only
             || pfound->has_arg != p->has_arg
             || pfound->flag != p->flag
             || pfound->val != p->val)
          {
        /* Second or later nonexact match found.  */
        if (!ambig_fallback)
          {
            if (!print_errors)
              /* Don't waste effort tracking the ambig set if
             we're not going to print it anyway.  */
              ambig_fallback = 1;
            else if (!ambig_set)
              {
                  /*if (__libc_use_alloca (n_options))
              ambig_set = alloca (n_options);
              else */if ((ambig_set = malloc (n_options)) == NULL)
              /* Fall back to simpler error message.  */
              ambig_fallback = 1;
            else
              ambig_malloced = 1;

            if (ambig_set)
              {
                memset (ambig_set, 0, n_options);
                ambig_set[indfound] = 1;
              }
              }
            if (ambig_set)
              ambig_set[option_index] = 1;
          }
          }
      }

      if (ambig_set || ambig_fallback)
    {
      if (print_errors)
        {
          if (ambig_fallback)
        fprintf (stderr, _("%s: option '%s%s' is ambiguous\n"),
             argv[0], prefix, d->__nextchar);
          else
        {
          flockfile (stderr);
          fprintf (stderr,
               _("%s: option '%s%s' is ambiguous; possibilities:"),
               argv[0], prefix, d->__nextchar);

          for (option_index = 0; option_index < n_options; option_index++)
            if (ambig_set[option_index])
              fprintf (stderr, " '%s%s'",
                   prefix, longopts[option_index].name);

          /* This must use 'fprintf' even though it's only
             printing a single character, so that it goes through
             __fxprintf_nocancel when compiled as part of glibc.  */
          fprintf (stderr, "\n");
          funlockfile (stderr);
        }
        }
      if (ambig_malloced)
        free (ambig_set);
      d->__nextchar += strlen (d->__nextchar);
      d->optind++;
      d->optopt = 0;
      return '?';
    }

      option_index = indfound;
    }

  if (pfound == NULL)
    {
      /* Can't find it as a long option.  If this is not getopt_long_only,
     or the option starts with '--' or is not a valid short option,
     then it's an error.  */
      if (!long_only || argv[d->optind][1] == '-'
      || strchr (optstring, *d->__nextchar) == NULL)
    {
      if (print_errors)
        fprintf (stderr, _("%s: unrecognized option '%s%s'\n"),
             argv[0], prefix, d->__nextchar);

      d->__nextchar = NULL;
      d->optind++;
      d->optopt = 0;
      return '?';
    }

      /* Otherwise interpret it as a short option.  */
      return -1;
    }

  /* We have found a matching long option.  Consume it.  */
  d->optind++;
  d->__nextchar = NULL;
  if (*nameend)
    {
      /* Don't test has_arg with >, because some C compilers don't
     allow it to be used on enums.  */
      if (pfound->has_arg)
    d->optarg = nameend + 1;
      else
    {
      if (print_errors)
        fprintf (stderr,
             _("%s: option '%s%s' doesn't allow an argument\n"),
             argv[0], prefix, pfound->name);

      d->optopt = pfound->val;
      return '?';
    }
    }
  else if (pfound->has_arg == 1)
    {
      if (d->optind < argc)
    d->optarg = argv[d->optind++];
      else
    {
      if (print_errors)
        fprintf (stderr,
             _("%s: option '%s%s' requires an argument\n"),
             argv[0], prefix, pfound->name);

      d->optopt = pfound->val;
      return optstring[0] == ':' ? ':' : '?';
    }
    }

  if (longind != NULL)
    *longind = option_index;
  if (pfound->flag)
    {
      *(pfound->flag) = pfound->val;
      return 0;
    }
  return pfound->val;
}

/* Scan elements of ARGV (whose length is ARGC) for option characters
   given in OPTSTRING.

   If an element of ARGV starts with '-', and is not exactly "-" or "--",
   then it is an option element.  The characters of this element
   (aside from the initial '-') are option characters.  If 'getopt'
   is called repeatedly, it returns successively each of the option characters
   from each of the option elements.

   If 'getopt' finds another option character, it returns that character,
   updating 'optind' and 'nextchar' so that the next call to 'getopt' can
   resume the scan with the following option character or ARGV-element.

   If there are no more option characters, 'getopt' returns -1.
   Then 'optind' is the index in ARGV of the first ARGV-element
   that is not an option.  (The ARGV-elements have been permuted
   so that those that are not options now come last.)

   OPTSTRING is a string containing the legitimate option characters.
   If an option character is seen that is not listed in OPTSTRING,
   return '?' after printing an error message.  If you set 'opterr' to
   zero, the error message is suppressed but we still return '?'.

   If a char in OPTSTRING is followed by a colon, that means it wants an arg,
   so the following text in the same ARGV-element, or the text of the following
   ARGV-element, is returned in 'optarg'.  Two colons mean an option that
   wants an optional arg; if there is text in the current ARGV-element,
   it is returned in 'optarg', otherwise 'optarg' is set to zero.

   If OPTSTRING starts with '-' or '+', it requests different methods of
   handling the non-option ARGV-elements.
   See the comments about RETURN_IN_ORDER and REQUIRE_ORDER, above.

   Long-named options begin with '--' instead of '-'.
   Their names may be abbreviated as long as the abbreviation is unique
   or is an exact match for some defined option.  If they have an
   argument, it follows the option name in the same ARGV-element, separated
   from the option name by a '=', or else the in next ARGV-element.
   When 'getopt' finds a long-named option, it returns 0 if that option's
   'flag' field is nonzero, the value of the option's 'val' field
   if the 'flag' field is zero.

   The elements of ARGV aren't really const, because we permute them.
   But we pretend they're const in the prototype to be compatible
   with other systems.

   LONGOPTS is a vector of 'struct option' terminated by an
   element containing a name which is zero.

   LONGIND returns the index in LONGOPT of the long-named option found.
   It is only valid when a long-named option has been found by the most
   recent call.

   If LONG_ONLY is nonzero, '-' as well as '--' can introduce
   long-named options.  */

int
_getopt_internal_r (int argc, char **argv, const char *optstring,
            const struct option *longopts, int *longind,
            int long_only, struct _getopt_data *d, int posixly_correct)
{
  int print_errors = d->opterr;

  if (argc < 1)
    return -1;

  d->optarg = NULL;

  if (d->optind == 0 || !d->__initialized)
    optstring = _getopt_initialize (argc, argv, optstring, d, posixly_correct);
  else if (optstring[0] == '-' || optstring[0] == '+')
    optstring++;

  if (optstring[0] == ':')
    print_errors = 0;

  /* Test whether ARGV[optind] points to a non-option argument.  */
#define NONOPTION_P (argv[d->optind][0] != '-' || argv[d->optind][1] == '\0')

  if (d->__nextchar == NULL || *d->__nextchar == '\0')
    {
      /* Advance to the next ARGV-element.  */

      /* Give FIRST_NONOPT & LAST_NONOPT rational values if OPTIND has been
     moved back by the user (who may also have changed the arguments).  */
      if (d->__last_nonopt > d->optind)
    d->__last_nonopt = d->optind;
      if (d->__first_nonopt > d->optind)
    d->__first_nonopt = d->optind;

      if (d->__ordering == PERMUTE)
    {
      /* If we have just processed some options following some non-options,
         exchange them so that the options come first.  */

      if (d->__first_nonopt != d->__last_nonopt
          && d->__last_nonopt != d->optind)
        exchange (argv, d);
      else if (d->__last_nonopt != d->optind)
        d->__first_nonopt = d->optind;

      /* Skip any additional non-options
         and extend the range of non-options previously skipped.  */

      while (d->optind < argc && NONOPTION_P)
        d->optind++;
      d->__last_nonopt = d->optind;
    }

      /* The special ARGV-element '--' means premature end of options.
     Skip it like a null option,
     then exchange with previous non-options as if it were an option,
     then skip everything else like a non-option.  */

      if (d->optind != argc && !strcmp (argv[d->optind], "--"))
    {
      d->optind++;

      if (d->__first_nonopt != d->__last_nonopt
          && d->__last_nonopt != d->optind)
        exchange (argv, d);
      else if (d->__first_nonopt == d->__last_nonopt)
        d->__first_nonopt = d->optind;
      d->__last_nonopt = argc;

      d->optind = argc;
    }

      /* If we have done all the ARGV-elements, stop the scan
     and back over any non-options that we skipped and permuted.  */

      if (d->optind == argc)
    {
      /* Set the next-arg-index to point at the non-options
         that we previously skipped, so the caller will digest them.  */
      if (d->__first_nonopt != d->__last_nonopt)
        d->optind = d->__first_nonopt;
      return -1;
    }

      /* If we have come to a non-option and did not permute it,
     either stop the scan or describe it to the caller and pass it by.  */

      if (NONOPTION_P)
    {
      if (d->__ordering == REQUIRE_ORDER)
        return -1;
      d->optarg = argv[d->optind++];
      return 1;
    }

      /* We have found another option-ARGV-element.
     Check whether it might be a long option.  */
      if (longopts)
    {
      if (argv[d->optind][1] == '-')
        {
          /* "--foo" is always a long option.  The special option
         "--" was handled above.  */
          d->__nextchar = argv[d->optind] + 2;
          return process_long_option (argc, argv, optstring, longopts,
                      longind, long_only, d,
                      print_errors, "--");
        }

      /* If long_only and the ARGV-element has the form "-f",
         where f is a valid short option, don't consider it an
         abbreviated form of a long option that starts with f.
         Otherwise there would be no way to give the -f short
         option.

         On the other hand, if there's a long option "fubar" and
         the ARGV-element is "-fu", do consider that an
         abbreviation of the long option, just like "--fu", and
         not "-f" with arg "u".

         This distinction seems to be the most useful approach.  */
      if (long_only && (argv[d->optind][2]
                || !strchr (optstring, argv[d->optind][1])))
        {
          int code;
          d->__nextchar = argv[d->optind] + 1;
          code = process_long_option (argc, argv, optstring, longopts,
                      longind, long_only, d,
                      print_errors, "-");
          if (code != -1)
        return code;
        }
    }

      /* It is not a long option.  Skip the initial punctuation.  */
      d->__nextchar = argv[d->optind] + 1;
    }

  /* Look at and handle the next short option-character.  */

  {
    char c = *d->__nextchar++;
    const char *temp = strchr (optstring, c);

    /* Increment 'optind' when we start to process its last character.  */
    if (*d->__nextchar == '\0')
      ++d->optind;

    if (temp == NULL || c == ':' || c == ';')
      {
    if (print_errors)
      fprintf (stderr, _("%s: invalid option -- '%c'\n"), argv[0], c);
    d->optopt = c;
    return '?';
      }

    /* Convenience. Treat POSIX -W foo same as long option --foo */
    if (temp[0] == 'W' && temp[1] == ';' && longopts != NULL)
      {
    /* This is an option that requires an argument.  */
    if (*d->__nextchar != '\0')
      d->optarg = d->__nextchar;
    else if (d->optind == argc)
      {
        if (print_errors)
          fprintf (stderr,
               _("%s: option requires an argument -- '%c'\n"),
               argv[0], c);

        d->optopt = c;
        if (optstring[0] == ':')
          c = ':';
        else
          c = '?';
        return c;
      }
    else
      d->optarg = argv[d->optind];

    d->__nextchar = d->optarg;
    d->optarg = NULL;
    return process_long_option (argc, argv, optstring, longopts, longind,
                    0 /* long_only */, d, print_errors, "-W ");
      }
    if (temp[1] == ':')
      {
    if (temp[2] == ':')
      {
        /* This is an option that accepts an argument optionally.  */
        if (*d->__nextchar != '\0')
          {
        d->optarg = d->__nextchar;
        d->optind++;
          }
        else
          d->optarg = NULL;
        d->__nextchar = NULL;
      }
    else
      {
        /* This is an option that requires an argument.  */
        if (*d->__nextchar != '\0')
          {
        d->optarg = d->__nextchar;
        /* If we end this ARGV-element by taking the rest as an arg,
           we must advance to the next element now.  */
        d->optind++;
          }
        else if (d->optind == argc)
          {
        if (print_errors)
          fprintf (stderr,
               _("%s: option requires an argument -- '%c'\n"),
               argv[0], c);

        d->optopt = c;
        if (optstring[0] == ':')
          c = ':';
        else
          c = '?';
          }
        else
          /* We already incremented 'optind' once;
         increment it again when taking next ARGV-elt as argument.  */
          d->optarg = argv[d->optind++];
        d->__nextchar = NULL;
      }
      }
    return c;
  }
}


int
_getopt_internal (int argc, char **argv, const char *optstring,
          const struct option *longopts, int *longind, int long_only,
          int posixly_correct)
{
  int result;

  getopt_data.optind = optind;
  getopt_data.opterr = opterr;

  result = _getopt_internal_r (argc, argv, optstring, longopts,
                   longind, long_only, &getopt_data,
                   posixly_correct);

  optind = getopt_data.optind;
  optarg = getopt_data.optarg;
  optopt = getopt_data.optopt;

  return result;
}

int
getopt_long (int argc, char *__getopt_argv_const *argv, const char *options,
         const struct option *long_options, int *opt_index)
{
  return _getopt_internal (argc, (char **) argv, options, long_options,
               opt_index, 0, 0);
}

#endif
