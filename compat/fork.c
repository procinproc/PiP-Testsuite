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

#include <sys/wait.h>
#include <test.h>

#define NFORK 	(10)

#define IDX(P,I)	(((P)*nfork)+(I))

int main( int argc, char **argv ) {
  pid_t ppid, pid, tid, *forked;
  int nfork, pipid, ntasks, thrd;
  int sz, i, j, k, n, done, err = 0;

  nfork = 0;
  if( argc > 1 ) {
    nfork = strtol( argv[1], NULL, 10 );
  }
  nfork = ( nfork == 0 ) ? NFORK : nfork;

  CHECK( pip_is_threaded(&thrd),  RV, return(EXIT_FAIL) );
  CHECK( pip_get_pipid(&pipid),   RV, return(EXIT_FAIL) );
  CHECK( pip_get_ntasks(&ntasks), RV, return(EXIT_FAIL) );

  if( pipid == 0 ) {
    sz = nfork * ntasks;
    CHECK( ( forked = (pid_t*) malloc(sizeof(pid_t)*sz) ),
	   !RV, 
	   return(EXIT_UNTESTED) );
    for( i=0; i<sz; i++ ) forked[i] = 0;
    CHECK( pip_named_export(forked,"FORKED"), 
	   RV, 
	   return(EXIT_FAIL) );
  } else {
    CHECK( pip_named_import(0,(void**)&forked,"FORKED"), 
	   RV, 
	   return(EXIT_FAIL) );
  }

  ppid = getpid();
  for( i=0; i<nfork; i++ ) {
    CHECK( ( pid = fork() ), RV<0, return(EXIT_UNTESTED) );
    if( pid == 0 ) {
      return 0;
    } else {
      fprintf( stderr, "[%d] %d forks %d (%d/%d)\n", 
	       pipid, ppid, pid, i, nfork );
      forked[IDX(pipid,i)] = pid;
    }
  }

  if( pipid == 0 ) {
    for( i=1; i<ntasks; i++ ) {
      CHECK( pip_named_import(i,(void**)&done,"forked"), 
	     RV, 
	     return(EXIT_FAIL) );
    }
    CHECK( pip_named_export((void*)&done,"CHECK"), 
	   RV, 
	   return(EXIT_FAIL) );
  } else {
    CHECK( pip_named_export((void*)&done,"forked"), 
	   RV, 
	   return(EXIT_FAIL) );
    CHECK( pip_named_import(0,(void**)&done,"CHECK"), 
	   RV, 
	   return(EXIT_FAIL) );
  }

  n = nfork;
  do {
    while( 1 ) {
      pid = wait( NULL );
      if( pid < 0 ) {
	if( errno == EINTR  ) continue;
	if( errno == ECHILD ) return EXIT_FAIL;
      }
      CHECK( pid<0, errno, return(EXIT_FAIL) );
      break;
    }
    for( j=0; j<ntasks; j++ ) {
      if( !thrd && j != pipid ) continue;
      for( k=0; k<nfork; k++ ) {
	if( forked[IDX(j,k)] == pid ) {
	  if( j == pipid ) {
	    fprintf( stderr, "[%d] %d forks %d -- terminated\n", 
		     j, ppid, pid );
	  } else {
	    /* this may happen with the thread mode */
	    fprintf( stderr, "[%d] %d forks %d -- TERMINATED (%d)\n", 
		     j, ppid, pid, pipid );
	  }
	  forked[IDX(j,k)] = 0;
	  n --;
	  break;
	}
      }
    }
  } while( n > 0 );

  if( pipid == 0 ) {
    for( i=1; i<ntasks; i++ ) {
      CHECK( pip_named_import(i,(void**)&done,"DONE"), 
	     RV, 
	     return(EXIT_FAIL) );
    }
  } else {
    CHECK( pip_named_export((void*)&done,"DONE"), 
	   RV, 
	   return(EXIT_FAIL) );
  }
  return err;
}
