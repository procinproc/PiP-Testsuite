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
 * System Software Development Team, 2016-2020
 * $
 * $PIP_TESTSUITE: Version 1.0.0$
 *
 * $Author: Atsushi Hori (R-CCS) mailto: ahori@riken.jp or ahori@me.com
 * $
 */

#include <netdb.h>

#ifndef PTHREADS
#include <test.h>

#else

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define CHECK(F,C,A) \
  do { errno=0; long int RV=(intptr_t)(F);	\
    if(C) { printf("%s()=%ld",#F,RV); A; } } while(0)

#define EXIT_PASS	0
#define EXIT_FAIL	1
#define EXIT_XPASS	2  /* passed, but it's unexpected. fixed recently?   */
#define EXIT_XFAIL	3  /* failed, but it's expected. i.e. known bug      */
#define EXIT_UNRESOLVED	4  /* cannot determine whether (X)?PASS or (X)?FAIL  */
#define EXIT_UNTESTED	5  /* not tested, this test haven't written yet      */
#define EXIT_UNSUPPORTED 6 /* not tested, this environment can't test this   */
#define EXIT_KILLED	7  /* killed by Control-C or something               */

#define NT_MAX		(10)
#endif

#define NITERS		(100)

int my_getaddrinfo( char *hostname ) {
  struct addrinfo hints, *res = NULL;
  int rv;

  memset( &hints, 0, sizeof(hints) );
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;

#ifndef PTHREADS
  pip_glibc_lock();
#endif
  rv = getaddrinfo( hostname, NULL, &hints, &res );
  if( rv == 0 && res != NULL ) freeaddrinfo( res );
#ifndef PTHREADS
  pip_glibc_unlock();
#endif
  return rv;
}

#define HOSTNAMELEN	(256)

int test( void ) {
  char hostname[HOSTNAMELEN];
  {
    CHECK( gethostname( hostname, HOSTNAMELEN ), RV, exit(EXIT_FAIL) );
    CHECK( my_getaddrinfo( hostname ),           RV, exit(EXIT_FAIL) );
    CHECK( my_getaddrinfo( "127.0.0.1" ),        RV, exit(EXIT_FAIL) );
  }
  return 0;
}

void *test_loop( void *niters_p ) {
  int niters = *((int*)niters_p);
  int i;

#ifndef PTHREADS
#if PIP_VERSION > 1
  int pipid, ntasks, sync = 0, *syncp;
  CHECK( pip_get_pipid(  &pipid ),  RV, exit(EXIT_FAIL) );
  CHECK( pip_get_ntasks( &ntasks ), RV,	exit(EXIT_FAIL) );
  if( pipid == 0 ) {
    for( i=1; i<ntasks; i++ ) {
      CHECK( pip_named_import( i, (void**)&syncp, "sync" ),
	     RV, exit(EXIT_FAIL) );
    }
    CHECK( pip_named_export( &sync, "done" ), RV, exit(EXIT_FAIL) );
  } else {
    CHECK( pip_named_export( &sync, "sync" ), RV, exit(EXIT_FAIL) );
    CHECK( pip_named_import( 0, (void**)&syncp, "done" ),
	   RV, exit(EXIT_FAIL) );
  }
#endif
#endif
  for( i=0; i<niters; i++ ) {
    test();
  }
#ifdef PTHREADS
  pthread_exit( NULL );
#endif
}

int main( int argc, char **argv ) {
  int niters = 0;

  if( argc > 1 ) {
    niters = strtol( argv[1], NULL, 10 );
  }
  niters = ( niters == 0 ) ? NITERS : niters;

#ifndef PTHREADS
  test_loop( (void*)&niters );
#else
  pthread_t	th[NT_MAX];
  int 		i, nt = 0;

  if( argc > 2 ) {
    nt = strtol( argv[2], NULL, 10 );
  }
  nt = ( nt == 0      ) ? NT_MAX : nt;
  nt = ( nt >= NT_MAX ) ? NT_MAX : nt;
  for( i=0; i<nt; i++ ) {
    CHECK( pthread_create( &th[i], NULL, test_loop, (void*)&niters ),
	   RV,
	   exit(EXIT_FAIL) );
  }
  for( i=0; i<nt; i++ ) {
    CHECK( pthread_join( th[i], NULL ), RV, exit(EXIT_FAIL) );
  }
#endif
  return 0;
}
