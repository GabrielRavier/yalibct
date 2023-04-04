// Derived from code with this license:
/* hash - hashing table processing.
   Copyright (C) 1998-1999, 2001, 2003, 2009-2023 Free Software Foundation,
   Inc.
   Written by Jim Meyering <meyering@ascend.com>, 1998.

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

/* A generic hash table package.  */

#pragma once

#include <stddef.h>

typedef size_t (*Hash_hasher) (const void *entry, size_t table_size);
