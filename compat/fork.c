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

int main( int argc, char **argv ) {
  pid_t pid;
  int nfork, i;

  nfork = 0;
  if( argc > 1 ) {
    nfork = strtol( argv[1], NULL, 10 );
  }
  nfork = ( nfork == 0 ) ? NFORK : nfork;

  for( i=0; i<nfork; i++ ) {
    CHECK( ( pid = fork() ),  RV<0, return(EXIT_FAIL) );
    if( pid == 0 ) {
      if( i & 1 ) {
	CHECK( 0, 0, return(EXIT_FAIL) );
	return 0;
      } else {
	CHECK( 1, 0, return(EXIT_FAIL) );
	exit( 0 );
      }
    }
#if PIP_VERSION_MAJOR > 1
    CHECK( pip_yield(0), RV!=0&&RV!=EINTR, return(EXIT_FAIL) );
#endif
  }
  for( i=0; i<nfork; i++ ) {
    while( 1 ) {
      errno = 0;
      pid = wait( NULL );
      if( errno == EINTR ) continue;
      CHECK( pid, RV<0, return(EXIT_FAIL) );
      break;
    }
  }
  return 0;
}
