#include "test-lib/portable-symbols/printf.h"
#include "test-lib/portable-symbols/tmpnam.h"
#include "test-lib/compiler-features.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>

YALIBCT_DIAGNOSTIC_IGNORE("-Wformat-extra-args")

int main() {
  char *filename = tmpnam(NULL);
  assert(filename != NULL);
  int fd=open(filename,O_CREAT|O_WRONLY|O_EXCL,0666);
  struct timeval tv[2];
  struct stat s;
  if (fd==-1) {
    perror("open");
    return -1;
  }
  lseek(fd,0x100000000,SEEK_SET);
  write(fd,"x",1);
  close(fd);
  tv[0].tv_sec=123456789;
  tv[0].tv_usec=0;
  tv[1].tv_sec=123456789;
  tv[1].tv_usec=0;
  if (utimes(filename,tv)) {
    perror("utimes");
    unlink(filename);
    return -1;
  }
  if (stat(filename,&s)) {
    perror("stat");
    unlink(filename);
    return -1;
  }
  printf("size %lld, atime %ld, mtime %ld"/*, inode %ld*/"\n",(long long)s.st_size,(long)s.st_atime,(long)s.st_mtime/*,(long)s.st_ino*/);
  unlink(filename);
}
