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
#include <pip/pip_machdep.h>
#include <malloc.h>
#include <mcheck.h>

#define NITERS		(10*1000)

#define MAX 	(1024*1024)

struct slot { 
  void *region; 
  int who;
  pip_spinlock_t lock;
};

pip_barrier_t barrier;
pip_atomic_t  count;

int main( int argc, char **argv ) {
  struct slot	*slots;
  int niters, i, n, nt;
  int pipid, who, c;
  size_t sz;
  pip_barrier_t *barrp;
  pip_atomic_t  *countp;

  if( argc > 1 ) {
    niters = strtol( argv[1], NULL, 10 );
  }
  niters = ( niters <= 0 ) ? NITERS : niters;

  mcheck( NULL );
  mallopt( M_CHECK_ACTION, 3 );

  pip_get_ntasks( &nt );
  pip_get_pipid( &pipid );
  if( pipid == 0 ) {
    slots = (struct slot*) malloc( sizeof(struct slot) * nt );
    for( i=0; i<nt; i++ ) {
      slots[i].region = NULL;
      slots[i].who    = -1;
      pip_spin_init( &slots[i].lock );
    }
    count = nt - 1;
    countp = &count;
    CHECK( pip_named_export( (void*) countp, "COUNT" ),       RV, return(EXIT_FAIL) );
    CHECK( pip_named_export( (void*) slots, "SLOTS" ),        RV, return(EXIT_FAIL) );
    barrp = &barrier;
    CHECK( pip_barrier_init( barrp, nt ), RV, return(EXIT_FAIL) );
    CHECK( pip_named_export( (void*) barrp, "BARRIER" ),      RV, return(EXIT_FAIL) );
  } else {
    CHECK( pip_named_import( 0, (void**) &countp, "COUNT" ),  RV, return(EXIT_FAIL) );
    CHECK( pip_named_import( 0, (void**) &slots, "SLOTS" ),   RV, return(EXIT_FAIL) );
    CHECK( pip_named_import( 0, (void**) &barrp, "BARRIER" ), RV, return(EXIT_FAIL) );
  }
  slots[pipid].region = malloc( 1024 );
#ifdef DEBUG
  printf( "[%d] malloc: %p (%d/%d)\n", pipid, &slots[pipid], (int)count, nt );
#endif
  slots[pipid].who = pipid;

  CHECK( pip_barrier_wait( barrp ), RV, return(EXIT_FAIL) );

  srandom( pipid + 101 );
  for( i=0; i<niters; i++ ) {
    do {
      if( *countp == 0 ) goto done;
      n = random() % nt;
#if PIP_VERSION_MAJOR > 1
      pip_yield( 0 );
#endif
    } while( slots[n].who == pipid ||
	     !pip_spin_trylock( &slots[n].lock ) );
    {
      sz = random() % MAX;
      who = slots[n].who;
#ifdef DEBUG
      printf( "[%d:%d] free[%d] and malloc(%lu): %p\n", pipid, n, who, sz, &slots[n] );
#endif
      switch( mprobe( slots[n].region ) ) {
      case MCHECK_HEAD:
	printf( "MCHECK_HEAD: %p\n", slots[n].region );
	return EXIT_FAIL;
	break;
      case MCHECK_TAIL:
	printf( "MCHECK_TAIL: %p\n", slots[n].region );
	return EXIT_FAIL;
	break;
      case MCHECK_FREE:
	printf( "MCHECK_FREE: %p\n", slots[n].region );
	return EXIT_FAIL;
	break;
      }
      free( slots[n].region );
      slots[n].region = malloc( sz );
      slots[n].who = pipid;
    }
    pip_spin_unlock( &slots[n].lock );
  }
 done:
  c = pip_atomic_sub_and_fetch( countp, 1 );
#ifdef DEBUG
  printf( "[%d] c:%d\n", pipid, c );
#endif
  mcheck_check_all();

#ifdef DEBUG
  for( i=0; i<nt; i++ ) {
    CHECK( pip_barrier_wait( barrp ), RV, return(EXIT_FAIL) );
    if( i == pipid ) {
      printf( "[%d]\n", pipid );
      malloc_info( 0, stdout );
      fflush( NULL );
    }
  }
#endif
  return 0;
}
