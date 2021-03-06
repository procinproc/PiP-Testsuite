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
#include <sys/time.h>

#define NTHREADS	(100)

#define WARMUP		(100)
#define NITERS		(10*1000*1000)

static pthread_t 		threads[NTHREADS];
static int			ids[NTHREADS];
static pthread_barrier_t	barr;
static int 			nthreads;
static __thread int		tlsvar;

static double tls_gettime( void ) {
  struct timeval tv;
  gettimeofday( &tv, NULL );
  return ((double)tv.tv_sec + (((double)tv.tv_usec) * 1.0e-6));
}

static void *thread_main( void *argp ) {
  pthread_t thr;
  double t0, t1;
  intptr_t tls;
  int id, i;

  id = *((int*)argp);
  //fprintf( stderr, "ID:%d\n", id );
  CHECK( (id<0 || id>nthreads), RV, exit(EXIT_FAIL) );

  tlsvar = id;
  for( i=0; i<WARMUP; i++ ) {
    CHECK( pip_save_tls( &tls ), RV, exit(EXIT_FAIL) );
  }
  t0 = - tls_gettime();
  for( i=0; i<NITERS; i++ ) {
    CHECK( pip_save_tls( &tls ), RV, exit(EXIT_FAIL) );
  }
  t0 += tls_gettime();
  t0 /= (double)NITERS;

  for( i=0; i<WARMUP; i++ ) {
    CHECK( pip_load_tls( tls ), RV, exit(EXIT_FAIL) );
  }
  t1 = - tls_gettime();
  for( i=0; i<NITERS; i++ ) {
    CHECK( pip_load_tls( tls ), RV, exit(EXIT_FAIL) );
  }
  t1 += tls_gettime();
  t1 /= (double)NITERS;

  CHECK( (tlsvar!=id), RV, exit(EXIT_FAIL) );
  for( i=0; i<NITERS; i++ ) thr = pthread_self();

  for( i=0; i<nthreads; i++ ) {
    if( i == 0 && id == 0 ) {
      printf( "No.\tsave_tls()\tload_tls()\tTLS-reg\t\tthread\t\tTLS var\t\tstack\n" );
    }
    if( i == id ) {
      if( tls == thr ) {
	printf( "[%d]\t%g\t%g\t%p\t<<< same <<<\t%p\t%p\n",
		(int) id, t0, t1, (void*) tls, &tlsvar, &i );
      } else {
	printf( "[%d]\t%g\t%g\t%p\t0x%lx\t%p\t%p\n",
		(int) id, t0, t1, (void*) tls, (intptr_t) thr, &tlsvar, &i );
      }
      fflush( NULL );
    }
    CHECK( pthread_barrier_wait( &barr ),
	   ( RV!=PTHREAD_BARRIER_SERIAL_THREAD && RV!=0 ),
	   exit(EXIT_FAIL) );
  }
  return NULL;
}


int main( int argc, char **argv ) {
  int i;

  nthreads = 10;
  if( argc > 1 ) {
    nthreads = strtol( argv[1], NULL, 10 );
  }
  nthreads = ( nthreads == 0       ) ? NTHREADS : nthreads;
  nthreads = ( nthreads > NTHREADS ) ? NTHREADS : nthreads;

  CHECK( pthread_barrier_init( &barr, NULL, nthreads ),
	 RV,
	 return(EXIT_FAIL) );

  for( i=0; i<nthreads; i++ ) {
    ids[i] = i;
    CHECK( pthread_create( &threads[i], NULL, thread_main, &ids[i] ),
	   RV, return(EXIT_FAIL) );
  }
  for( i=0; i<nthreads; i++ ) {
    CHECK( pthread_join( threads[i], NULL ), RV, return(EXIT_FAIL) );
  }
  return 0;
}
