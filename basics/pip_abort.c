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
static int 		ntasks, pipid, target;

static struct my_exp {
  pthread_barrier_t	barr;
} exp;

int main( int argc, char **argv ) {
  char	*env;
  int	ntenv;
  int	i, extval = 0;
  pid_t pid;

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
  target = 0;
  if( argc > 2 ) {
    target = strtol( argv[2], NULL, 10 );
  }
  expp = &exp;

  if( !pip_is_initialized() ) {
    /* PiP task is implicitly initialized in v2 or higher */
    if( ( pid = fork() ) == 0 ) {
      CHECK( pip_init(&pipid,&ntasks,(void**)&expp,0), RV, return(EXIT_FAIL) );
      /* Root */
      memset( &exp, 0, sizeof(exp) );
      CHECK( pthread_barrier_init(&exp.barr,NULL,ntasks+1),
	     RV, return(EXIT_FAIL) );
      
      for( i=0; i<ntasks; i++ ) {
	pipid = i;
	CHECK( pip_spawn(argv[0],argv,NULL,PIP_CPUCORE_ASIS,&pipid,
			 NULL,NULL,NULL),
	       RV, return(EXIT_FAIL) );
      }
      CHECK( pthread_barrier_wait( &expp->barr ),
	     (RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD), return(EXIT_FAIL) );
      
      sleep( 1 );
      if( target < 0 ) {
	fprintf( stderr, "ROOT: pip_abort()\n" );
	pip_abort();
      }
      while( 1 );

    } else if( pid < 0 ) {
      CHECK( pid<0, RV, return(EXIT_FAIL) );
      
    } else {
      int status, rv;
      while( 1 ) {
	if( wait( &status ) == pid ) break;
	CHECK( rv, ( RV!=0 && errno!=EINTR ), return(EXIT_FAIL) );
      }
      CHECK( !WIFSIGNALED(status), RV, return(EXIT_FAIL) );
      return EXIT_PASS;
    }
  } else {
    CHECK( pip_init(&pipid,&ntasks,(void**)&expp,0), RV, return(EXIT_FAIL) );
    CHECK( pthread_barrier_wait( &expp->barr ),
	   (RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD), return(EXIT_FAIL) );
      
    if( pipid == target || target > ntasks) {
      fprintf( stderr, "TAKS[%d]: pip_abort()\n", pipid );
      pip_abort();
    }
    while( 1 );

    CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  }
  return EXIT_FAIL;
}
