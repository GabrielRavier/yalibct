/*
 *
 *   Copyright (c) International Business Machines  Corp., 2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * Test Name: stat02
 *
 * Test Description:
 *  Verify that, stat(2) succeeds to get the status of a file and fills the
 *  stat structure elements though process doesn't have read access to the
 *  file.
 *
 * Expected Result:
 *  stat() should return value 0 on success and the stat structure elements
 *  should be filled with specified 'file' information.
 *
 * Algorithm:
 *  Setup:
 *   Setup signal handling.
 *   Create temporary directory.
 *   Pause for SIGUSR1 if option specified.
 *
 *  Test:
 *   Loop if the proper options are given.
 *   Execute system call
 *   Check return code, if system call failed (return=-1)
 *   	Log the errno and Issue a FAIL message.
 *   Otherwise,
 *   	Verify the Functionality of system call
 *      if successful,
 *      	Issue Functionality-Pass message.
 *      Otherwise,
 *		Issue Functionality-Fail message.
 *  Cleanup:
 *   Print errno log and/or timing stats if options given
 *   Delete the temporary directory created.
 *
 * Usage:  <for command-line>
 *  stat02 [-c n] [-e] [-f] [-i n] [-I x] [-p x] [-t]
 *	where,  -c n : Run n copies concurrently.
 *		-e   : Turn on errno logging.
 *		-f   : Turn off functionality Testing.
 *		-i n : Execute test n times.
 *		-I x : Execute test for x seconds.
 *		-P x : Pause for x seconds between iterations.
 *		-t   : Turn on syscall timing.
 *
 * History
 *	07/2001 John George
 *		-Ported
 *
 * Restrictions:
 *
 */
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>

#include "test-deps/cygwin.h"

#define FILE_MODE	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define TESTFILE	"testfile"
enum {
    FILE_SIZE = 1024,
    BUF_SIZE = 256,
    NEW_MODE = 0222,
    MASK = 0777
};

const char *TCID="stat02"; 		/* Test program identifier.    */
int TST_TOTAL=1;			/* Total number of test cases. */
extern int Tst_count;		/* Test Case counter for tst_* routines */
int exp_enos[]={0};
uid_t User_id;			/* eff. user id/group id of test process */
gid_t Group_id;
char nobody_uid[] = "nobody";
struct passwd *ltpuser;

void setup();			/* Setup function for the test */
void cleanup(void) __attribute__((noreturn));			/* Cleanup function for the test */

int
main(int ac, char **av)
{
	struct stat stat_buf;	/* stat structure buffer */
	int lc;			/* loop counter */
	const char *msg;	/* message returned from parse_opts */

	/* Parse standard options given to run the test. */
	msg = parse_opts(ac, av, (option_t *) NULL, NULL);
	if (msg != (char *) NULL) {
		tst_brkm(TBROK, NULL, "OPTION PARSING ERROR - %s", msg);
		tst_exit();
	}

	/* Perform global setup for test */
	setup();

	/* set the expected errnos... */
	TEST_EXP_ENOS(exp_enos);

	/* Check looping state if -i option given */
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		/* Reset Tst_count in case we are looping. */
		Tst_count = 0;

		/*
		 * Call stat(2) to get the status of
		 * specified 'file' into stat structure.
		 */
		TEST(stat(TESTFILE, &stat_buf));

		/* check return code of stat(2) */
		if (TEST_RETURN == -1) {
			TEST_ERROR_LOG(TEST_ERRNO);
			tst_resm(TFAIL,
				 "stat(%s, &stat_buf) Failed, errno=%d : %s",
				 TESTFILE, TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			/*
			 * Perform functional verification if test
			 * executed without (-f) option.
			 */
			if (STD_FUNCTIONAL_TEST) {
				stat_buf.st_mode &= ~S_IFREG;
				/*
				 * Verify the data returned by stat(2)
				 * aganist the expected data.
				 */
				if ((stat_buf.st_uid != User_id) || \
				    (stat_buf.st_gid != Group_id) || \
				    (stat_buf.st_size != FILE_SIZE) || \
				    ((stat_buf.st_mode & MASK) != NEW_MODE)) {
					tst_resm(TFAIL, "Functionality of "
						"stat(2) on '%s' Failed",
						TESTFILE);
				} else {
					tst_resm(TPASS, "Functionality of "
						"stat(2) on '%s' Succcessful",
						TESTFILE);
				}
			} else {
				tst_resm(TPASS, "Call succeeded");
			}
		}
		Tst_count++;			/* incr TEST_LOOP counter */
	}	/* End for TEST_LOOPING */

	/* Call cleanup() to undo setup done for the test. */
	cleanup();
	/*NOTREACHED*/

}	/* End main */

/*
 * void
 * setup() -  Performs setup function for the test.
 *  Creat a temporary directory and change directory to it.
 *  Creat a testfile and write some data into it.
 *  Modify the mode permissions of testfile such that test process
 *  has read-only access to testfile.
 */
void
setup()
{
	int i, fd;			/* counter, file handle for file */
	char tst_buff[BUF_SIZE];	/* data buffer for file */
	int wbytes;			/* no. of bytes written to file */
	int write_len = 0;

	/* capture signals */
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
#if 0
	/* Switch to nobody user for correct error code collection */
        if (geteuid() != 0) {
                tst_brkm(TBROK, tst_exit, "Test must be run as root");
        }
         ltpuser = getpwnam(nobody_uid);
         if (setuid(ltpuser->pw_uid) == -1) {
                tst_resm(TINFO, "setuid failed to "
                         "to set the effective uid to %d",
                         ltpuser->pw_uid);
                perror("setuid");
         }
#endif
	/* Pause if that option was specified
	 * TEST_PAUSE contains the code to fork the test with the -i option.
	 * You want to make sure you do this before you create your temporary
	 * directory.
	 */
	TEST_PAUSE;

	/* make a temp directory and cd to it */
	tst_tmpdir();

        fd = open(TESTFILE, O_RDWR|O_CREAT, FILE_MODE);
	if (fd == -1) {
		tst_brkm(TBROK, cleanup,
			 "open(%s, O_RDWR|O_CREAT, %#o) Failed, errno=%d : %s",
			 TESTFILE, FILE_MODE, errno, strerror(errno));
		/*NOTREACHED*/
	}

	/* Fill the test buffer with the known data */
	for (i = 0; i < BUF_SIZE; i++) {
		tst_buff[i] = 'a';
	}

	/* Write to the file 1k data from the buffer */
	while (write_len < FILE_SIZE) {
		wbytes = write(fd, tst_buff, sizeof(tst_buff));
		if (wbytes <= 0) {
			tst_brkm(TBROK, cleanup,
				 "write(2) on %s Failed, errno=%d : %s",
				 TESTFILE, errno, strerror(errno));
			/*NOTREACHED*/
		} else {
			write_len += wbytes;
		}
	}

	if (close(fd) == -1) {
		tst_resm(TWARN, "close(%s) Failed, errno=%d : %s",
			 TESTFILE, errno, strerror(errno));
	}

	/* Modify mode permissions on the testfile */
	if (chmod(TESTFILE, NEW_MODE) < 0) {
		tst_brkm(TBROK, cleanup,
			 "chmod(2) on %s Failed, errno=%d : %s",
			 TESTFILE, errno, strerror(errno));
		/*NOTREACHED*/
	}

	/* Get the uid/gid of the process */
	User_id = getuid();
	Group_id = getgid();

}	/* End setup() */

/*
 * cleanup() - performs all ONE TIME cleanup for this test at
 *	       completion or premature exit.
 *  Remove the temporary directory and file created.
 */
void
cleanup()
{
	/*
	 * print timing stats if that option was specified.
	 * print errno log if that option was specified.
	 */
	TEST_CLEANUP;

	/* Remove tmp dir and all files in it */
        assert(remove(TESTFILE) == 0);
	tst_rmdir();

	/* exit with return code appropriate for results */
	tst_exit();
}	/* End cleanup() */
