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

#include <test.h>

static struct my_exp	*expp;
static int 		ntasks, pipid;
static sigset_t 	signull, sigusrs;

static struct my_exp {
  pthread_barrier_t	barr;
  int			pipid_sender;
} exp;

static int block_sigusrs( void ) {
  return pip_sigmask( SIG_BLOCK, &sigusrs, NULL );
}

static int wait_signal_root( int signo ) {
  //(void) sigsuspend( &signull );
  int sig;
  expp->pipid_sender = PIP_PIPID_ROOT;
  CHECK( pip_kill( 0, signo ),            RV, exit(EXIT_FAIL) );
  //pause();
  (void) sigwait( &sigusrs, &sig );
  CHECK( expp->pipid_sender != ntasks -1, RV, exit(EXIT_FAIL) );
  CHECK( sig!=signo,            RV, exit(EXIT_FAIL) );
  return 0;
}

static int wait_signal_task( int pipid, int signo ) {
  int	next = pipid + 1;
  int	prev = pipid - 1;
  next = ( next >= ntasks ) ? PIP_PIPID_ROOT : next;
  prev = ( prev < 0       ) ? PIP_PIPID_ROOT : prev;
  fprintf( stderr, "(PIPID:%d,PID:%d) prev:%d next:%d\n", pipid, pip_gettid(), prev, next );
  fflush(stderr);
  //(void) sigsuspend( &signull );
  //pause();
  int sig = -1;
  (void) sigwait( &sigusrs, &sig );
  fprintf( stderr, "(PIPID:%d,PID:%d) sig:%d signo:%d from:%d\n", 
	   pipid, pip_gettid(), sig, signo, expp->pipid_sender );
  CHECK( sig!=signo,              RV, exit(EXIT_FAIL) );
  CHECK( expp->pipid_sender != prev, RV, exit(EXIT_FAIL) );
  fflush(stderr);
  //(void) pause();
  expp->pipid_sender = pipid;
  CHECK( pip_kill( next, signo ), RV, exit(EXIT_FAIL) );
  return 0;
}

static void sigusr_handler( int signo ) {
  int pipid;
  fprintf( stderr, "PID:%d sigusr_handler\n", pip_gettid() );
  fflush(stderr);
  CHECK( pip_get_pipid(&pipid), RV, RV=0 );
  fprintf( stderr, "PIPID:%d signal:%d\n", pipid, signo );
  fflush(stderr);
  return;
}

int main( int argc, char **argv ) {
  struct sigaction sigact;
  char	*env;
  int	ntenv, niters;
  int	i, j, extval = 0;

  ntasks = 0;
  if( argc > 1 ) {
    ntasks = strtol( argv[1], NULL, 10 );
  }
  ntasks = ( ntasks <= 0 ) ? NTASKS : ntasks;
  if( ( env = getenv( "NTASKS" ) ) != NULL ) {
    ntenv = strtol( env, NULL, 10 );
    if( ntasks > ntenv )  return(EXIT_UNTESTED);
  } else {
    if( ntasks > NTASKS ) return(EXIT_UNTESTED);
  }

  niters = 100;
  if( argc > 2 ) {
    niters = strtol( argv[1], NULL, 10 );
  }
  niters = ( niters <= 0 ) ? 100 : niters;

  CHECK( sigemptyset( &signull ),        RV, exit(EXIT_FAIL) );
  CHECK( sigemptyset( &sigusrs ),        RV, exit(EXIT_FAIL) );
  CHECK( sigaddset( &sigusrs, SIGUSR1 ), RV, exit(EXIT_FAIL) );
  CHECK( sigaddset( &sigusrs, SIGUSR2 ), RV, exit(EXIT_FAIL) );

  memset( &sigact, 0, sizeof(sigact) );
  sigact.sa_handler = sigusr_handler;
  //CHECK( sigaddset( &sigact.sa_mask, SIGUSR1 ), RV, exit(EXIT_FAIL) );
  //CHECK( sigaddset( &sigact.sa_mask, SIGUSR2 ), RV, exit(EXIT_FAIL) );
  CHECK( sigaction( SIGUSR1, &sigact, NULL ),   RV, exit(EXIT_FAIL) );
  CHECK( sigaction( SIGUSR2, &sigact, NULL ),   RV, exit(EXIT_FAIL) );

  expp = &exp;
  CHECK( pip_init(&pipid,&ntasks,(void**)&expp,0), RV, return(EXIT_FAIL) );
  /* the following function must be called after calling pip_init() */
  CHECK( block_sigusrs(), RV, exit(EXIT_FAIL) );
  if( pipid == PIP_PIPID_ROOT ) {
    /* illegal cases */
    CHECK( pip_kill( -1, SIGUSR1 ),     RV!=ERANGE&&RV!=EINVAL, exit(EXIT_FAIL) );
    CHECK( pip_kill( ntasks, SIGUSR1 ), RV!=ERANGE&&RV!=EINVAL, exit(EXIT_FAIL) );
    CHECK( pip_kill( 0,  SIGUSR1 ),     RV!=ESRCH,  exit(EXIT_FAIL) );
    /* legal case */
    memset( expp, 0, sizeof(exp) );
    CHECK( pthread_barrier_init(&expp->barr,NULL,ntasks+1),
	   RV, return(EXIT_FAIL) );

    for( i=0; i<ntasks; i++ ) {
      pipid = i;
      CHECK( pip_spawn(argv[0],argv,NULL,PIP_CPUCORE_ASIS,&pipid,
		       NULL,NULL,NULL),
	     RV, return(EXIT_FAIL) );
    }

    CHECK( pthread_barrier_wait( &expp->barr ),
	   (RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD), exit(EXIT_FAIL) );
    for( j=0; j<niters; j++ ) {
      wait_signal_root( SIGUSR1 );
      wait_signal_root( SIGUSR2 );
    }
    CHECK( pthread_barrier_wait( &expp->barr ),
	   (RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD), exit(EXIT_FAIL) );

    for( i=0; i<ntasks; i++ ) {
      int status;
      CHECK( pip_wait( i, &status ),        RV, exit(EXIT_FAIL) );
      if( WIFEXITED( status ) ) {
	CHECK( ( extval = WEXITSTATUS( status ) ), RV, exit(EXIT_FAIL) );
      } else {
	CHECK( "Task is signaled", RV, exit(EXIT_UNRESOLVED) );
      }
    }
    /* illegal case */
    //CHECK( pip_kill( 0, SIGUSR2 ), RV!=ESRCH, return(EXIT_FAIL) );

  } else {
    /* illegal cases */
    CHECK( pip_kill( -1,     SIGUSR1 ), RV!=ERANGE, exit(EXIT_FAIL) );
    CHECK( pip_kill( ntasks, SIGUSR1 ), RV!=ERANGE, exit(EXIT_FAIL) );
    /* legal case */
    CHECK( pthread_barrier_wait( &expp->barr ),
	   (RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD), 
	   exit(EXIT_FAIL) );
    for( j=0; j<niters; j++ ) {
      wait_signal_task( pipid, SIGUSR1 );
      wait_signal_task( pipid, SIGUSR2 );
    }
    CHECK( pthread_barrier_wait( &expp->barr ),
	   (RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD), 
	   return(EXIT_FAIL) );
  }
  CHECK( pip_fin(), RV, exit(EXIT_FAIL) );
  return extval;
}
