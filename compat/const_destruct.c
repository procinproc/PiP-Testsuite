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

#define SZ	(10)

int array[SZ] = { 0 };
int pipid;

static char *is_initialized( void ) {
  char *inited;
  if( pip_is_initialized() ) {
    inited = "initialized";
  } else {
    inited = "not initilized";
  }
  return inited;
}

void init_func( void ) __attribute__ ((constructor));
void init_func( void ) {
  char *env;
  int i;

  CHECK( (env=getenv(PIP_TEST_PIPID_ENV)), RV==0, _exit(EXIT_FAIL) );
  pipid = strtol( env, NULL, 10 );

  fprintf( stderr, "[%d:%d] constructor is called (%s)\n", 
	   pip_gettid(), pipid, is_initialized() );
  for( i=0; i<SZ; i++ ) {
    array[i] = i + pipid;
  }
}

void dest_func( void ) __attribute__ ((destructor));
void dest_func( void ) {
  fprintf( stderr, "[%d:%d] destructor is called (%s)\n", 
	   pip_gettid(), pipid, is_initialized() );
}

int main( int argc, char **argv ) {
  int i, id;

  CHECK( pip_get_pipid( &id ), RV, return(EXIT_FAIL) );
  fprintf( stderr, "[%d:%d] PiP task main (%s)\n", 
	   pip_gettid(), id, is_initialized() );
  for( i=0; i<SZ; i++ ) {
    CHECK( array[i]!=i+pipid, RV, return(EXIT_FAIL) );
  }
  pip_exit( 0 );
}
