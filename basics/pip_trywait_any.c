/*
 * $RIKEN_copyright: Riken Center for Computational Sceience (R-CCS),
 * System Software Development Team, 2016-2021
 * $
 * $PIP_VERSION: Version 1.2.0$
 * $PIP_license: <Simplified BSD License>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the PiP project.$
 */
/*
 * Written by Atsushi HORI <ahori@riken.jp>
 */

#include <test.h>

int main( int argc, char **argv ) {
  sigset_t 	sigset;
  pip_barrier_t	barr, *barrp = &barr;
  char	*env;
  int 	ntasks, ntenv, pipid, id;
  int	sig, i, core, status, err;

  ntasks = 0;
  if( argc > 1 ) {
    ntasks = strtol( argv[1], NULL, 10 );
  }
  ntasks = ( ntasks <= 0 ) ? NTASKS : ntasks;
  if( ( env = getenv( "NTASKS" ) ) != NULL ) {
    ntenv = strtol( env, NULL, 10 );
    if( ntasks > ntenv ) return(EXIT_UNTESTED);
  } else {
    if( ntasks > NTASKS ) return(EXIT_UNTESTED);
  }

  sig = 0;
  if( argc > 2 ) {
    sig = signal_number( argv[2] );
    if( sig < 0 ) {
      fprintf( stderr, "Unknown signal %d\n", sig );
      return EXIT_UNTESTED;
    }
  }
  sigemptyset( &sigset );
  sigaddset( &sigset, sig );

  CHECK( pip_init(&pipid,&ntasks,(void**)&barrp,0), RV, return(EXIT_FAIL) );

  if( pipid == PIP_PIPID_ROOT ) {
    int core = PIP_CPUCORE_ASIS;

    CHECK( pip_trywait_any(NULL,NULL), RV!=ECHILD, return(EXIT_FAIL) );

    CHECK( pip_barrier_init( barrp, ntasks ),       RV, return(EXIT_FAIL) );
    for( i=0; i<ntasks; i++ ) {
      pipid = i;
      CHECK( pip_spawn(argv[0],argv,NULL,core,&pipid,NULL,NULL,NULL),
	     RV,
	     return(EXIT_FAIL) );
    }
    for( i=0; i<ntasks; i++ ) {
      status = 0;
      pipid  = IMPOSSIBLE_PIPID;
      while(( err = pip_trywait_any( &pipid, &status ) ) == ECHILD );
      CHECK( err, RV, return(EXIT_FAIL) );
      CHECK( pipid==IMPOSSIBLE_PIPID,    RV, return(EXIT_FAIL) );
      if( sig == 0 ) {
	CHECK( WIFSIGNALED(status),      RV, return(EXIT_FAIL) );
	CHECK( WIFEXITED(status),       !RV, return(EXIT_FAIL) );
	CHECK( WEXITSTATUS(status)==0,  !RV, return(EXIT_FAIL) );
      } else {
	CHECK( WIFEXITED(status),        RV, return(EXIT_FAIL) );
	CHECK( WIFSIGNALED(status),     !RV, return(EXIT_FAIL) );
	CHECK( WTERMSIG(status)==sig,   !RV, return(EXIT_FAIL) );
      }
      status = 0;
    }
    CHECK( pip_trywait_any(&pipid,&status), RV!=ECHILD, return(EXIT_FAIL) );
    CHECK( pip_barrier_fin( barrp ),        RV,         return(EXIT_FAIL) );

  } else {
    CHECK( pip_barrier_wait( barrp ),     RV,        return(EXIT_FAIL) );
    CHECK( pip_trywait_any(&pipid,&status), RV!=EPERM, return(EXIT_FAIL) );

    if( sig != SIGSEGV ) {
      CHECK( pip_kill( PIP_PIPID_SELF, sig ), RV, return(EXIT_FAIL) );
    } else {
      intptr_t null = 0;
      printf( "%d", *((int*)null) );
    }
  }
  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return 0;			/* dummy */
}
