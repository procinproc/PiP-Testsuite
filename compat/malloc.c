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

#include <test.h>

#define NITERS		(10)

#define FREEMEM_THRESHOLD	(512*1024*1024) /* 0.5 GiB */
#define VARIATION	(0.33)
#define MALLOC_AVEMIN	(1024)

#define MEMINFO_PATH	"/proc/meminfo"

FILE *fp = NULL;

ssize_t freemem( void ) {
  char name[32] = { 0 };
  char unit[4]  = { 0 };
  ssize_t  fm       = 0;

  if( fp == NULL ) {
    if( ( fp = fopen( MEMINFO_PATH, "r" ) ) == NULL ) return -1;
  }
  while( fscanf( fp, "%30s %ld %2s\n", name, &fm, unit ) == 3 ) {
    if( strcmp( "MemFree:", name ) == 0 ) {
      rewind( fp );
      switch( unit[0]  ) {
      case 'g':
      case 'G':
	fm *= 1024;
	/* fallthrough */
      case 'm':
      case 'M':
	fm *= 1024;
	/* fallthrough */
      case 'k':
      case 'K':
	fm *= 1024;
	break;
      default:
	break;
      }
      break;
    }
  }
  return fm;
}

char *my_malloc( size_t min, size_t max, int pipid ) {
  long int rd, sz;
  char *p;
  int i;

  rd = random() % ( max - min );
  sz = rd + min;

  if( ( p = (char*) malloc( (size_t) sz ) ) != NULL ) {
    for( i=0; i<sz; i++ ) p[i] = (char) pipid;
  }
  return p;
}

void my_free( char *alloc, size_t min, int pipid ) {
  int i;
  for( i=0; i<min; i++ ) {
    if( alloc[i] != (char) pipid ) pip_exit( EXIT_FAIL );
  }
  free( alloc );
}

int malloc_loop( int niters, int pipid, int ntasks ) {
  size_t fmall, fm, ave0, ave1, min, max;
  char *prev, *p;
  int i;

  fmall = freemem();
  if( fmall > 0 ) {
    fm = FREEMEM_THRESHOLD;
  } else if( fmall < FREEMEM_THRESHOLD ) {
    fprintf( stderr, "Not enough memory for malloc test (%d MB)\n", fm/1024 );
    pip_exit( EXIT_UNTESTED );
  }
  fm  = ( fm > FREEMEM_THRESHOLD )? FREEMEM_THRESHOLD : fm;
  fm  = ((float)fm) * ( 1.0 - VARIATION );
  ave0 = fm >> pipid;
  ave1 = ( ave0 < MALLOC_AVEMIN ) ? MALLOC_AVEMIN: ave0;
  min = (size_t) ( ((float)ave1) - ( ((float)ave1) * VARIATION ) );
  max = (size_t) ( ((float)ave1) + ( ((float)ave1) * VARIATION ) );

  niters <<= pipid;

  fprintf( stderr, "[%d] FREE:%7lu  AVE:%7lu  MIN:%7lu  MAX:%7lu [KB] iters:%d\n",
	   pipid, fmall/1024, ave1/1024, min/1024, max/1024, niters );
  CHECK( ( prev = my_malloc(min,max,pipid) ), RV==0, return(ENOMEM) );
  for( i=0; i<niters; i++ ) {
#if PIP_VERSION_MAJOR > 1
    CHECK( pip_yield(PIP_YIELD_USER), RV!=0&&RV!=EINTR, return(EXIT_FAIL) );
#endif
    CHECK( ( p = my_malloc(min,max,pipid) ), RV==0, return(ENOMEM) );
    my_free( prev, min, pipid );
    prev = p;
  }
  if( fp != NULL ) fclose( fp );
  my_free( prev, min, pipid );
  return 0;
}

int main( int argc, char **argv ) {
  int niters = 0;
  int pipid, ntasks;

  if( argc > 1 ) {
    niters = strtol( argv[1], NULL, 10 );
  }
  niters = ( niters <= 0 ) ? NITERS : niters;

  CHECK( pip_init( &pipid, &ntasks, NULL, 0 ), RV, return(EXIT_FAIL) );
  CHECK( malloc_loop( niters, pipid, ntasks ), RV, return(EXIT_FAIL) );
  return 0;
}
