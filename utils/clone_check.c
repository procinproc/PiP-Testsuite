#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pip/pip_clone.h>

int foo( void *arg ) { return 0; }

char stack[8192];

main() {
  int flags = pip_clone_flags( 0 );
  pid_t pid;

  flags &= ~(CLONE_SETTLS); 	/* 0x80000 */
  if( ( pid = clone( foo, stack, flags, NULL ) ) < 0 ) {
    return 1;
  }
  while( 1 ) {
    errno = 0;
    if( wait( NULL ) == pid || errno != EINTR ) break;
  }
  return errno;
}
