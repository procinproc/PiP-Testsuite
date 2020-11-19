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
#include <test.h>

int my_getaddrinfo( char *hostname ) {
  struct addrinfo hints, *res = NULL;
  int rv;

  memset( &hints, 0, sizeof(hints) );
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;
  //pip_glibc_lock();
  rv = getaddrinfo( hostname, NULL, &hints, &res );
  if( rv == 0 && res != NULL ) freeaddrinfo( res );
  //pip_glibc_unlock();
  return rv;
}

#define HOSTNAMELEN	(256)

int main( int argc, char **argv ) {
  char hostname[HOSTNAMELEN];

  CHECK( gethostname( hostname, HOSTNAMELEN ), RV, return(EXIT_FAIL) );
  CHECK( my_getaddrinfo( hostname ),           RV, return(EXIT_FAIL) );
  CHECK( my_getaddrinfo( "127.0.0.1" ),        RV, return(EXIT_FAIL) );
  return 0;
}
