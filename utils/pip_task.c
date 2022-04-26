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

#define DO_COREBIND

#ifdef DO_COREBIND
static int get_ncpus( void ) {
  cpu_set_t cpuset;
  CHECK( sched_getaffinity( 0, sizeof(cpuset), &cpuset ),
	 RV, exit(EXIT_UNTESTED) );
  return CPU_COUNT( &cpuset );
}
#endif

int spawn_tasks( int ntasks, char **argv ) {
  int pipid, status, extval;
  int i, c, nc, err;
  char env_ntasks[128], env_pipid[128];
  char *env;

  CHECK( pip_init( &pipid, &ntasks, NULL, 0 ), RV, return(EXIT_UNTESTED) );
  sprintf( env_ntasks, "%s=%d", PIP_TEST_NTASKS_ENV, ntasks );
  putenv( env_ntasks );

  nc = 0;
#ifdef DO_COREBIND
  nc = get_ncpus() - 1;
#endif

  for( i=0; i<ntasks; i++ ) {
    pipid = i;
#ifndef DO_COREBIND
    c = PIP_CPUCORE_ASIS;
#else
    if( nc == 0 ) {
      c = PIP_CPUCORE_ASIS;
    } else {
      c = i % nc;
    }
#endif
    sprintf( env_pipid, "%s=%d", PIP_TEST_PIPID_ENV, pipid );
    putenv( env_pipid );
    CHECK( pip_spawn( argv[0], argv, NULL, c, &pipid,
		      NULL, NULL, NULL ),
	   RV,
	   return(EXIT_FAIL) );
    CHECK( pipid, RV!=i, return(EXIT_FAIL) );
  }
  err    = 0;
  extval = 0;
  for( i=0; i<ntasks; i++ ) {
    status = 0;
    CHECK( pip_wait( i, &status ), RV, return(EXIT_FAIL) );
    if( WIFEXITED( status ) ) {
      extval = WEXITSTATUS( status );
      if( extval ) {
	fprintf( stderr, "%s Task[%d] returns %d\n", argv[0], i, extval );
      }
    } else {
      extval = EXIT_UNRESOLVED;
    }
    if( err == 0 && extval != 0 ) err = extval;
  }

  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return err;
}

int main( int argc, char **argv ) {
  int ntasks, ntenv, status;
  int i, c, nc, extval;
  pid_t pid;
  char *prog, *env;

  prog = argv[0];

  set_sigint_watcher();

  if( argc < 3 ) return EXIT_UNTESTED;

  ntasks = strtol( argv[1], NULL, 10 );
  CHECK( ntasks, RV<=0||RV>NTASKS, return(EXIT_UNTESTED) );
  if( ( env = getenv( "NTASKS" ) ) != NULL ) {
    ntenv = strtol( env, NULL, 10 );
    if( ntasks > ntenv ) return(EXIT_UNTESTED);
  }

  if( ( pid = fork() ) == 0 ) {
    setpgid( 0, 0 );
    extval = spawn_tasks( ntasks, &argv[2] );
    switch( extval ) {
    case EXIT_PASS:
    case EXIT_FAIL:
    case EXIT_XPASS:
    case EXIT_XFAIL:
    case EXIT_UNRESOLVED:
    case EXIT_UNTESTED:
    case EXIT_UNSUPPORTED:
    case EXIT_KILLED:
      break;
    default:
      extval = EXIT_FAIL;
    }
    return extval;
  } else if( pid < 0 ) {
    fprintf( stderr, "%s: Unable to fork (%d)\n", prog, errno );
    return EXIT_UNTESTED;
  } else {
    while( wait( &status ) != pid );
    if( WIFEXITED( status ) ) {
      return WEXITSTATUS( status );
    } else if( WIFSIGNALED( status ) ) {
      fprintf( stderr, "%s: signaled (%d)\n", prog, WTERMSIG( status ) );
      return EXIT_FAIL;
    }
  }
  fprintf( stderr, "%s: ???\n", prog );
  return EXIT_UNTESTED;
}
