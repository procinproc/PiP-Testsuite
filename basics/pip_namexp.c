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

#define NITERS		(100)
#define MAGIC		(314)

int main( int argc, char **argv ) {
  pip_barrier_t barrier;
  pip_barrier_t	*barrp;
  int 	pipid, ntasks, prev, next;
  int 	niters = 0, i;
  int 	count, *counts;
  volatile int *countp, *cp;

  if( argc > 1 ) {
    niters = strtol( argv[1], NULL, 10 );
  }
  niters = ( niters <= 0 ) ? NITERS : niters;

  CHECK( pip_init( &pipid, &ntasks, NULL, 0 ), RV, return(EXIT_FAIL)     );
  CHECK( pipid==PIP_PIPID_ROOT,                RV, return(EXIT_UNTESTED) );
  CHECK( ntasks<2,                             RV, return(EXIT_UNTESTED) );

  prev = ( pipid == 0        ) ? ntasks - 1 : pipid - 1;
  next = ( pipid == ntasks-1 ) ? 0          : pipid + 1;

  count  = MAGIC;
  countp = &count;
  CHECK( pip_named_import( PIP_PIPID_SELF, (void**) &countp, "must be failed" ),
	 RV!=EDEADLK, return(EXIT_FAIL) );
  CHECK( pip_named_export( (void*) countp, "must be found" ),
	 RV, return(EXIT_FAIL) );
  CHECK( pip_named_import( PIP_PIPID_SELF, (void**) &countp, "must be found" ),
	 RV, return(EXIT_FAIL) );
  CHECK( *countp!=MAGIC, RV, return(EXIT_FAIL) );

  if( pipid == 0 ) {
    barrp = &barrier;
    CHECK( pip_barrier_init( barrp, ntasks ), RV, return(EXIT_FAIL) );
    CHECK( pip_named_export( (void*) &barrier, "barrier" ),
	   RV, return(EXIT_FAIL) );
  } else {
    CHECK( pip_named_import( 0, (void**) &barrp, "barrier" ),
	   RV, return(EXIT_FAIL) );
  }
  CHECK( pip_barrier_wait( barrp ), RV, return(EXIT_FAIL) );

  for( i=0; i<niters; i++ ) {
    if( pipid == 0 ) {
      count  = 1;
      countp = &count;
      fprintf( stderr, ">> [%d] countp:%p\n", pipid, countp );
      CHECK( pip_named_export( (void*) countp, "forward:%d", i ),
	     RV, return(EXIT_FAIL) );
      CHECK( pip_named_import( prev, (void**) &cp, "forward:%d", i ),
	     RV, return(EXIT_FAIL) );
      fprintf( stderr, ">> [%d] count:%d @ %p\n", pipid, *cp, cp );
      CHECK(  cp!=countp, RV, return(EXIT_FAIL) );
      CHECK( *cp!=ntasks, RV, return(EXIT_FAIL) );

      count  = 1;
      countp = &count;
      fprintf( stderr, "<< [%d] countp:%p\n", pipid, countp );
      CHECK( pip_named_export( (void*) countp, "backward:%d", i ),
	     RV, return(EXIT_FAIL) );
      CHECK( pip_named_import( next, (void**) &cp, "backward:%d", i ),
	     RV, return(EXIT_FAIL) );
      fprintf( stderr, "<< [%d] count:%d @ %p\n", pipid, *cp, cp );
      CHECK(  cp!=countp, RV, return(EXIT_FAIL) );
      CHECK( *cp!=ntasks, RV, return(EXIT_FAIL) );

    } else {
      CHECK( pip_named_import( prev, (void**) &countp, "forward:%d", i ),
	     RV, return(EXIT_FAIL) );
      fprintf( stderr, ">> [%d] count:%d @ %p\n", pipid, *countp, countp );
      (*countp) ++;
      CHECK( pip_named_export( (void*) countp, "forward:%d", i ),
	     RV, return(EXIT_FAIL) );

      CHECK( pip_named_import( next, (void**) &countp, "backward:%d", i ),
	     RV, return(EXIT_FAIL) );
      fprintf( stderr, "<< [%d] count:%d @ %p\n", pipid, *countp, countp );
      (*countp) ++;
      CHECK( pip_named_export( (void*) countp, "backward:%d", i ),
	     RV, return(EXIT_FAIL) );
    }
  }
  CHECK( pip_barrier_wait( barrp ), RV, return(EXIT_FAIL) );
  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return EXIT_PASS;
}
