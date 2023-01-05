/* Quote a blob so that it can be used in C literals.
   Copyright (C) 2018-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#include "test-deps/gnulib/string-buffer.h"

char *
YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_SUPPORT_QUOTE_BLOB (const void *blob, size_t length)
{
  struct string_buffer out;
  sb_init(&out);

  const YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_CHAR *p = blob;
  for (size_t i = 0; i < length; ++i)
    {
      YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_CHAR ch = p[i];

      /* Use C backslash escapes for those control characters for
	 which they are defined.  */
      switch (ch)
	{
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\a'):
          sb_append (&out, "\\a");
	  break;
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\b'):
          sb_append (&out, "\\b");
	  break;
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\f'):
          sb_append (&out, "\\f");
	  break;
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\n'):
          sb_append (&out, "\\n");
	  break;
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\r'):
          sb_append (&out, "\\r");
	  break;
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\t'):
          sb_append (&out, "\\t");
	  break;
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\v'):
          sb_append (&out, "\\v");
	  break;
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\\'):
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\''):
	case YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('\"'):
          sb_appendf (&out, "\\%c", ch);
	  break;
	default:
	  if (ch < YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_(' ') || ch > YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_('~'))
	    /* For narrow characters, use octal sequences because they
	       are fixed width, unlike hexadecimal sequences.  For
	       wide characters, use N2785 delimited escape
	       sequences.  */
	    if (YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_WIDE)
	      sb_appendf (&out, "\\x{%x}", (unsigned int) ch);
	    else
	      sb_appendf (&out, "\\%03o", (unsigned int) ch);
	  else
            sb_appendf (&out, "%c", ch);
	}
    }

  return sb_dupfree(&out);
}

#undef YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_CHAR
#undef YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_L_
#undef YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_SUPPORT_QUOTE_BLOB
#undef YALIBCT_GLIBC_INTERNAL_SUPPORT_QUOTE_BLOB_WIDE
