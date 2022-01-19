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

int main( int argc, char **argv ) {
  char *env;
  int pipid, ntasks, ntenv;

  /* before calling pip_init(), this must fail */
  pipid = PIP_PIPID_ANY;
  CHECK( pip_spawn( argv[0], argv, NULL, PIP_CPUCORE_ASIS, &pipid,
		    NULL, NULL, NULL ),
	 RV!=EPERM,
	 return(EXIT_FAIL) );

  ntasks = NTASKS;
  if( ( env = getenv( "NTASKS" ) ) != NULL ) {
    ntenv  = strtol( env, NULL, 10 );
    ntasks = ( ntasks > ntenv ) ? ntenv : ntasks;
  }
  CHECK( pip_init( &pipid, &ntasks, NULL, 0 ), RV, return(EXIT_FAIL) );

  /* after calling pip_init() */
  if(
#if PIP_VERSION_MAJOR > 1
     pip_isa_task()
#else
     pipid != PIP_PIPID_ROOT
#endif
     ) {
    pipid = PIP_PIPID_ANY;
    CHECK( pip_spawn( argv[0], argv, NULL, PIP_CPUCORE_ASIS, &pipid,
		      NULL, NULL, NULL ),
	   RV!=EPERM,
	   return(EXIT_FAIL) );

  } else {			/* root */
    int coreno, status = 0, extval = 0;

    pipid = PIP_PIPID_ANY;
    CHECK( pip_spawn( NULL, NULL, NULL, PIP_CPUCORE_ASIS, &pipid,
		      NULL, NULL, NULL ),
	   RV!=EINVAL,
	   return(EXIT_FAIL) );

    pipid = PIP_PIPID_ANY;
    coreno = -123;
    CHECK( pip_spawn( argv[0], argv, NULL, coreno, &pipid,
			 NULL, NULL, NULL ),
	   RV!=EINVAL,
	   return(EXIT_FAIL) );

#if PIP_VERSION_MAJOR > 1
    pipid = PIP_PIPID_ANY;
    coreno = PIP_CPUCORE_CORENO_MAX + 1;
    CHECK( pip_spawn( argv[0], argv, NULL, coreno, &pipid,
			 NULL, NULL, NULL ),
	   RV!=EINVAL,
	   return(EXIT_FAIL) );
#endif

    pipid = 100000;
    CHECK( pip_spawn( argv[0], argv, NULL, PIP_CPUCORE_ASIS, &pipid,
			 NULL, NULL, NULL ),
	   RV!=EINVAL,
	   return(EXIT_FAIL) );

    pipid = -123;
    CHECK( pip_spawn( argv[0], argv, NULL, PIP_CPUCORE_ASIS, &pipid,
			 NULL, NULL, NULL ),
	   RV!=EINVAL,
	   return(EXIT_FAIL) );

    pipid = PIP_PIPID_MYSELF;
    CHECK( pip_spawn( argv[0], argv, NULL, PIP_CPUCORE_ASIS, &pipid,
			 NULL, NULL, NULL ),
	   RV!=EINVAL,
	   return(EXIT_FAIL) );

    pipid = PIP_PIPID_ANY;
    CHECK( pip_spawn( argv[0], argv, NULL, PIP_CPUCORE_ASIS, &pipid,
		      NULL, NULL, NULL ),
	   RV,
	   return(EXIT_FAIL) );

    CHECK( pip_wait( -123, &status ),  RV!=ERANGE, return(EXIT_FAIL) );
    CHECK( pip_wait( pipid, &status ), RV,         return(EXIT_FAIL) );

    if( WIFEXITED( status ) ) {
      extval = WEXITSTATUS( status );
    } else {
      extval = EXIT_UNRESOLVED;
    }
    return extval;
  }
  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return EXIT_PASS;
}
