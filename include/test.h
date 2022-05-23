/*
 * $PIP_license: <Simplified BSD License>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *     Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * $
 * $RIKEN_copyright: Riken Center for Computational Sceience (R-CCS),
 * System Software Development Team, 2016-2021
 * $
 * $PIP_TESTSUITE: Version 1.1.0$
 *
 * $Author: Atsushi Hori (R-CCS) mailto: ahori@riken.jp or ahori@me.com
 * $
 */


#ifndef __test_h__
#define __test_h__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef NO_PIP
#if PIP_VERSION_MAJOR==1
#include <pip.h>
#else
#include <pip/pip.h>
#endif
#endif

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <link.h>
#include <dlfcn.h>
#include <elf.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#define NTASKS			PIP_NTASKS_MAX

#define PIP_TEST_NTASKS_ENV 	"PIP_TEST_NTASKS"
#define PIP_TEST_PIPID_ENV 	"PIP_TEST_PIPID"

#define EXIT_PASS	0
#define EXIT_FAIL	1
#define EXIT_XPASS	2  /* passed, but it's unexpected. fixed recently?   */
#define EXIT_XFAIL	3  /* failed, but it's expected. i.e. known bug      */
#define EXIT_UNRESOLVED	4  /* cannot determine whether (X)?PASS or (X)?FAIL  */
#define EXIT_UNTESTED	5  /* not tested, this test haven't written yet      */
#define EXIT_UNSUPPORTED 6 /* not tested, this environment can't test this   */
#define EXIT_KILLED	7  /* killed by Control-C or something               */

#define IMPOSSIBLE_PIPID	(-123)

#ifndef INLINE
#define INLINE	static inline
#endif

extern int pip_id;

typedef struct test_args {
  int 		argc;
  char 		**argv;
  int		ntasks;
  int		nact;
  int		npass;
  int		timer;
  volatile int	niters;
} test_args_t;

INLINE pid_t my_gettid( void ) {
  return (pid_t) syscall( (long int) SYS_gettid );
}

#define TRUE		(1)
#define FALSE		(0)

extern char *__progname;

#define PRINT_FL(FSTR,VAL)	\
  fprintf(stderr,"%s:%d (%s)=%d\n",__FILE__,__LINE__,FSTR,VAL)

#define PRINT_FLE(FSTR,ERR)			\
  if(!errno) {								\
    fprintf(stderr,"\n[%s(%d)] %s:%d (%s): %ld:'%s'\n\n",		\
	    __progname,my_gettid(),__FILE__,__LINE__,FSTR,		\
	    ERR,strerror(ERR));						\
  } else {								\
    fprintf(stderr,"\n[%s(%d)] %s:%d (%s): %ld:'%s' (errno: %d:'%s')\n\n", \
	    __progname, my_gettid(), __FILE__,__LINE__,FSTR,		\
	    (ERR), strerror(ERR), errno,  strerror(errno) ); }

#ifndef DEBUG
#define CHECK(F,C,A) \
  do { errno=0; long int RV=(intptr_t)(F);	\
    if(C) { PRINT_FLE((#F ":" #C),RV); A; } } while(0)
#else
#define CHECK(F,C,A)							\
  do {									\
    fprintf(stderr,"[%s(%d)] %s:%d %s: >> %s\n",__progname,my_gettid(),	\
	    __FILE__,__LINE__,__func__,#F );				\
    errno=0; long int RV=(intptr_t)(F);					\
    fprintf(stderr,"[%s(%d)] %s:%d %s: << %s\n",__progname,my_gettid(),	\
	    __FILE__,__LINE__,__func__,#F );				\
    if(C) { PRINT_FLE(#F,RV); A; }					\
  } while(0)
#endif

INLINE void abend( int extval ) {
#if PIP_VERSION_MAJOR > 1
  (void) pip_kill_all_child_tasks();
#endif
  exit( extval );
}

INLINE void print_maps( void ) {
  int fd = open( "/proc/self/maps", O_RDONLY );
  while( 1 ) {
    char buf[1024];
    int sz;
    if( ( sz = read( fd, buf, 1024 ) ) <= 0 ) break;
    CHECK( write( 1, buf, sz ), RV<0, abend(EXIT_UNTESTED) );
  }
  close( fd );
}

INLINE void print_numa( void ) {
  int fd = open( "/proc/self/numa_maps", O_RDONLY );
  while( 1 ) {
    char buf[1024];
    int sz;
    if( ( sz = read( fd, buf, 1024 ) ) <= 0 ) break;
    CHECK( write( 1, buf, sz ), RV<0, abend(EXIT_UNTESTED) );
  }
  close( fd );
}

#define DUMP_ENV(X,Y)	dump_env( #X, X, Y );
INLINE void dump_env( char *tag, char **envv, int nomore ) {
  int i;
  for( i=0; envv[i]!=NULL; i++ ) {
    if( i >= nomore ) {
      fprintf( stderr, "%s[..] more env-vars may follow...\n", tag );
      break;
    }
    fprintf( stderr, "%s[%d] %s\n", tag, i, envv[i] );
  }
}

const char *signam_tab[] = {
  "(signal0)",		/* 0 */
  "SIGHUP",		/* 1 */
  "SIGINT",		/* 2 */
  "SIGQUIT",		/* 3 */
  "SIGILL",		/* 4 */
  "SIGTRAP",		/* 5 */
  "SIGABRT",		/* 6 */
  "SIGBUS",		/* 7 */
  "SIGFPE",		/* 8 */
  "SIGKILL",		/* 9 */
  "SIGUSR1",		/* 10 */
  "SIGSEGV",		/* 11 */
  "SIGUSR2",		/* 12 */
  "SIGPIPE",		/* 13 */
  "SIGALRM",		/* 14 */
  "SIGTERM",		/* 15 */
  "SIGSTKFLT",		/* 16 */
  "SIGCHLD",		/* 17 */
  "SIGCONT",		/* 18 */
  "SIGSTOP",		/* 19 */
  "SIGTSTP",		/* 20 */
  "SIGTTIN",		/* 21 */
  "SIGTTOU",		/* 22 */
  "SIGURG",		/* 23 */
  "SIGXCPU",		/* 24 */
  "SIGXFSZ",		/* 25 */
  "SIGVTALRM",		/* 26 */
  "SIGPROF",		/* 27 */
  "SIGWINCH",		/* 28 */
  "SIGIO",		/* 29 */
  "SIGPWR",		/* 30 */
  "SIGSYS",		/* 31 */
  NULL
};

static const char *signal_name( int sig ) {
  if( sig < 0 || sig > 31 ) return  "(not a signal)";
  return signam_tab[sig];
}

#define SIGNAM_LEN	(16)
static int signal_number( char *signame ) {
  char name[SIGNAM_LEN], *p;
  int i;

  if( strncasecmp( signame, "SIG", 3 ) == 0 ) {
    strncpy( name, signame, SIGNAM_LEN );
  } else {
    p = stpcpy( name, "SIG" );
    (void) strncpy( p, signame, SIGNAM_LEN-3 );
  }
  for( i=0; signam_tab[i]!=NULL; i++ ) {
    if( strcasecmp( name, signam_tab[i] ) == 0 ) return i;
  }
  return -1;
}

static void signal_watcher( int sig, siginfo_t *siginfo, void *dummy ) {
  fprintf( stderr,
	   "SIGNAL: %s(%d) addr:%p pid=%d !!!!!!\n",
	   signal_name( siginfo->si_signo ),
	   siginfo->si_signo,
	   siginfo->si_addr,
	   my_gettid() );
}

INLINE void set_signal_watcher( int signal ) {
  struct sigaction sigact;
  memset( (void*) &sigact, 0, sizeof( sigact ) );
  sigact.sa_sigaction = signal_watcher;
  sigact.sa_flags     = SA_RESETHAND | SA_SIGINFO;
  CHECK( sigaction( signal, &sigact, NULL ), RV, abend(EXIT_UNTESTED) );
}

INLINE int set_signal_handler( int signal, void(*handler)(int, siginfo_t*, void*) ) {
  struct sigaction sigact;
  memset( (void*) &sigact, 0, sizeof( sigact ) );
  sigact.sa_sigaction = handler;
  sigact.sa_flags     = SA_RESETHAND | SA_SIGINFO;
  errno = 0;
  (void) sigaction( signal, &sigact, NULL );
  return errno;
}

static void sigint_watcher( int sig, siginfo_t *info, void* extra ) {
  fprintf( stderr, "\nInterrupted by user !!!!!!\n" );
  fflush( NULL );
  exit( EXIT_UNRESOLVED );
}

INLINE void set_sigint_watcher( void ) {
  struct sigaction sigact;

  memset( (void*) &sigact, 0, sizeof( sigact ) );
  sigact.sa_sigaction = sigint_watcher;
  sigact.sa_flags     = SA_RESETHAND;
  CHECK( sigaction( SIGINT, &sigact, NULL ), RV, abend(EXIT_UNTESTED) );
}

#endif /* __test_h__ */
