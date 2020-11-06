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

#define SZ	(10)

int array[SZ] = { 0 };
int pipid;

void init_func( void ) __attribute__ ((constructor));
void init_func( void ) {
  char *env;
  int i;

  CHECK( (env=getenv(PIP_TEST_PIPID_ENV)), RV==0, _exit(EXIT_FAIL) );
  pipid = strtol( env, NULL, 10 );
  fprintf( stderr, "[%d] constructor is called\n", pipid );
  for( i=0; i<SZ; i++ ) {
    array[i] = i + pipid;
  }
}

void dest_func( void ) __attribute__ ((destructor));
void dest_func( void ) {
  fprintf( stderr, "[%d] destructor is called\n", pipid );
}

int main( int argc, char **argv ) {
  int i;

  for( i=0; i<SZ; i++ ) {
    CHECK( array[i]!=i+pipid, RV, return(EXIT_FAIL) );
  }
  return 0;
}
