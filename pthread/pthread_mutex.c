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

#define DEBUG

#include <test.h>

#define NTIMES		(1000)

struct task_comm {
  pthread_barrier_t	barr;
  pthread_mutex_t	mutex;
  int			count;
};

int main( int argc, char **argv ) {
  struct task_comm	tc;
  struct task_comm 	*tcp = &tc;
  void 		*exp;
  int 		pipid  = 999;
  int 		ntasks, i;

  if( argc > 1 ) {
    ntasks = strtol( argv[1], NULL, 10 );
    ntasks = ( ntasks == 0 ) ? 1 : ntasks;
  } else {
    ntasks = 1;
  }
  exp = (void*) tcp;
  CHECK( pip_init( &pipid, &ntasks, &exp, 0 ), RV, return(EXIT_FAIL) );
  if( pipid == PIP_PIPID_ROOT ) {
    printf( "sizeof(pthread_mutex_t) = %d\n", sizeof(pthread_mutex_t) );
    CHECK( pthread_barrier_init( &tcp->barr, NULL, ntasks+1 ), RV, return(EXIT_FAIL) );
    CHECK( pthread_mutex_init( &tcp->mutex, NULL ), RV, return(EXIT_FAIL) );
    tcp->count = 0;

    for( i=0; i<ntasks; i++ ) {
      pipid = i;
      CHECK( pip_spawn( argv[0], argv, NULL, PIP_CPUCORE_ASIS, &pipid,
			NULL, NULL, NULL ),
	     RV,
	     return(EXIT_FAIL) );
    }
    CHECK( pthread_barrier_wait( &tcp->barr ),   
	   RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD,  return(EXIT_FAIL) );

    for( i=0; i<NTIMES; i++ ) {
      CHECK( pthread_mutex_lock(   &tcp->mutex ), RV, return(EXIT_FAIL) );
      tcp->count ++;
      CHECK( pthread_mutex_unlock( &tcp->mutex ), RV, return(EXIT_FAIL) );
    }

    for( i=0; i<ntasks; i++ ) {
      CHECK( pip_wait( i, NULL ), RV, return(EXIT_FAIL) );
    }

    printf( "count:%d (%d) ntasks:%d\n", tcp->count, NTIMES*(ntasks+1), ntasks );
    CHECK( tcp->count != NTIMES * ( ntasks + 1 ), RV, return(EXIT_FAIL) );

  } else {
    tcp = (struct task_comm*) exp;

    CHECK( pthread_barrier_wait( &tcp->barr ),   
	   RV!=0&&RV!=PTHREAD_BARRIER_SERIAL_THREAD,  return(EXIT_FAIL) );

    for( i=0; i<NTIMES; i++ ) {
      CHECK( pthread_mutex_lock( &tcp->mutex ),   RV, exit(EXIT_FAIL) );
      tcp->count ++;
      CHECK( pthread_mutex_unlock( &tcp->mutex ), RV, exit(EXIT_FAIL) );
    }
  }
  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return 0;
}
