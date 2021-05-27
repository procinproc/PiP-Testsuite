#define _GNU_SOURCE                                                                    
#include <sched.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int foo( void *arg ) { return 0; }

char stack[8192];

main() {
  int flags = CLONE_VM;
  pid_t pid;
  if( ( pid = clone( foo, stack, flags, NULL ) ) < 0 ) {
    return 1;
  }
  while( 1 ) {
    if( wait( NULL ) == pid || errno != EINTR ) break;
  }
  return 0;
}
