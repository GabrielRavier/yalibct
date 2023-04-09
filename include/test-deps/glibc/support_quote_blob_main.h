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

char *
SUPPORT_QUOTE_BLOB (const void *blob, size_t length)
{
  struct xmemstream_or_string_buffer out;
  xmemstream_or_string_buffer_init (&out);

  const YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_BLOB_CHAR *p = blob;
  for (size_t i = 0; i < length; ++i)
    {
      YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_BLOB_CHAR ch = p[i];

      /* Use C backslash escapes for those control characters for
	 which they are defined.  */
      switch (ch)
	{
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\a'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked ('a', &out);
	  break;
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\b'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked ('b', &out);
	  break;
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\f'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked ('f', &out);
	  break;
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\n'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked ('n', &out);
	  break;
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\r'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked ('r', &out);
	  break;
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\t'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked ('t', &out);
	  break;
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\v'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked ('v', &out);
	  break;
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\\'):
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\''):
	case YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('\"'):
	  xmemstream_or_string_buffer_putc_unlocked ('\\', &out);
	  xmemstream_or_string_buffer_putc_unlocked (ch, &out);
	  break;
	default:
	  if (ch < YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_(' ') || ch > YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_('~'))
	    /* For narrow characters, use octal sequences because they
	       are fixed width, unlike hexadecimal sequences.  For
	       wide characters, use N2785 delimited escape
	       sequences.  */
	    if (WIDE)
	      xmemstream_or_string_buffer_fprintf (&out, "\\x{%x}", (unsigned int) ch);
	    else
	      xmemstream_or_string_buffer_fprintf (&out, "\\%03o", (unsigned int) ch);
	  else
	    xmemstream_or_string_buffer_putc_unlocked (ch, &out);
	}
    }

  xmemstream_or_string_buffer_xfclose(&out);
  return out.buffer;
}

#undef YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_BLOB_CHAR
#undef YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_
#undef SUPPORT_QUOTE_BLOB
#undef WIDE
