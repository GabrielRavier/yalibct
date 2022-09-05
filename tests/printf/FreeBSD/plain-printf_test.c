/* $FreeBSD$ */

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright 2013 Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the name of Google Inc. nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/*
 * INTRODUCTION
 *
 * This plain test program mimics the structure and contents of its
 * ATF-based counterpart.  It attempts to represent various test cases
 * in different separate functions and just calls them all from main().
 *
 * In reality, plain test programs can be much simpler.  All they have
 * to do is return 0 on success and non-0 otherwise.
 */

#include "test-lib/chdir-to-tmpdir.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
snprintf__two_formatters(void)
{
	char buffer[128];

	if (snprintf(buffer, sizeof(buffer), "%s, %s!", "Hello",
	    "tests") <= 0)
		errx(EXIT_FAILURE, "snprintf with two formatters failed");

	if (strcmp(buffer, "Hello, tests!") != 0)
		errx(EXIT_FAILURE, "Bad formatting: got %s", buffer);
}

static void
snprintf__overflow(void)
{
	char buffer[10];

	if (snprintf(buffer, sizeof(buffer), "0123456789abcdef") != 16)
		errx(EXIT_FAILURE, "snprintf did not return the expected "
		    "number of characters");

	if (strcmp(buffer, "012345678") != 0)
		errx(EXIT_FAILURE, "Bad formatting: got %s", buffer);
}

static void
fprintf__simple_string(void)
{
	FILE *file;
	char buffer[128];
	size_t length;
	const char *contents = "This is a message\n";

	file = fopen("test.txt", "w+");
	if (fprintf(file, "%s", contents) <= 0)
		err(EXIT_FAILURE, "fprintf failed to write to file");
	rewind(file);
	length = fread(buffer, 1, sizeof(buffer) - 1, file);
	if (length != strlen(contents))
		err(EXIT_FAILURE, "fread failed");
	buffer[length] = '\0';
	fclose(file);

	if (strcmp(buffer, contents) != 0)
		errx(EXIT_FAILURE, "Written and read data differ");

	/* Of special note here is that we are NOT deleting the temporary
	 * files we created in this test.  Kyua takes care of this cleanup
	 * automatically and tests can (and should) rely on this behavior. */
}

int
main(void)
{
    yalibct_chdir_to_tmpdir();
	/* If you have read the printf_test.c counterpart in the atf/
	 * directory, you may think that the sequencing of tests below and
	 * the exposed behavior to the user is very similar.  But you'd be
	 * wrong.
	 *
	 * There are two major differences with this and the ATF version.
	 * The first is that the code below has no provisions to detect
	 * failures in one test and continue running the other tests: the
	 * first failure causes the whole test program to exit.  The second
	 * is that this particular main() has no arguments: without ATF,
	 * all test programs may expose a different command-line interface,
	 * and this is an issue for consistency purposes. */
	snprintf__two_formatters();
	snprintf__overflow();
	fprintf__simple_string();

	return EXIT_SUCCESS;
}
