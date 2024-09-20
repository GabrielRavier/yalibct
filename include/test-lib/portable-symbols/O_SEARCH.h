// Derived from code with this license:
/* Like <fcntl.h>, but with non-working flags defined to 0.

   Copyright (C) 2006-2023 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Paul Eggert */

#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_O_SEARCH
#include <fcntl.h>
#else

#include <fcntl.h>

#undef O_SEARCH
#define O_SEARCH O_RDONLY /* This is often close enough in older systems.  */

#endif
