/*
 * $RIKEN_copyright: Riken Center for Computational Sceience,
 * System Software Development Team, 2016, 2017, 2018, 2019, 2020$
 * $PIP_TEST_VERSION: Version 1.0.0$
 * $PIP_license: <Simplified BSD License>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the PiP project.$
 */
/*
 * Written by Atsushi HORI <ahori@riken.jp>
 */

#include <test.h>

#define NITERS		(5)
#define MALLOC_MAX	(1024*1024*32)

#define FREEMEM_THRESHOLD	(512*1024*1024) /* 1 GB */
#define VARIATION	(0.33)

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
}

int malloc_loop( int niters, int pipid ) {
  ssize_t fm, ave, min, max;
  char *prev, *p;
  int i;

  fm = freemem();
  if( fm > 0 ) {
    fm = FREEMEM_THRESHOLD;
  } else if( fm < FREEMEM_THRESHOLD ) {
    fprintf( stderr, "Not enough memory for malloc test (%d MB)\n", fm/1024 );
    pip_exit( EXIT_UNTESTED );
  }
  fm  = ( fm > FREEMEM_THRESHOLD )? FREEMEM_THRESHOLD : fm;
  ave = ( fm / 4 ) >> pipid;
  ave = ( ave < 256 )? 256: ave;
  min = (ssize_t) ( ((float)ave) - ( ((float)ave) * VARIATION ) );
  max = (ssize_t) ( ((float)ave) + ( ((float)ave) * VARIATION ) );

  niters <<= pipid;;

  fprintf( stderr, "[%d] FREE:%7ld  AVE:%7ld  MIN:%7ld  MAX:%7ld  iters:%d\n",
	   pipid, fm, ave/1024, min/1024, max/1024, niters );
  CHECK( ( prev = my_malloc(min,max,pipid) ), RV==0, return(ENOMEM) );
  for( i=0; i<niters; i++ ) {
#if PIP_VERSION > 1
    CHECK( pip_yield(PIP_YIELD_USER), RV!=0&&RV!=EINTR, return(EXIT_FAIL) );
#endif
    CHECK( ( p = my_malloc(min,max,pipid) ),  RV==0, return(ENOMEM) );
    my_free( prev, min, pipid );
    prev = p;
  }
  if( fp != NULL ) fclose( fp );
  my_free( prev, min, pipid );
  return 0;
}

int main( int argc, char **argv ) {
  int niters = 0;
  int pipid  = 0;

  if( argc > 1 ) {
    niters = strtol( argv[1], NULL, 10 );
  }
  niters = ( niters <= 0 ) ? NITERS : niters;

  CHECK( pip_get_pipid( &pipid ),      RV, return(EXIT_FAIL) );
  CHECK( malloc_loop( niters, pipid ), RV, return(EXIT_FAIL) );
  return 0;
}
