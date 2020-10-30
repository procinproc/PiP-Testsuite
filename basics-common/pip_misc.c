/*
 * $RIKEN_copyright: Riken Center for Computational Sceience,
 * System Software Development Team, 2016, 2017, 2018, 2019, 2020$
 * $PIP_TEST_VERSION: Version 3.0.0$
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

#include <test.h>

int main( int argc, char **argv ) {
  int mode, ntasks, nt, pipid, id;
  const char *mode_str = NULL;

  /* newer PiP lib. allows to call PiP functions without calling pip_init() */
#ifdef NO_IMPLICIT_INIT
  /*** before calling pip_init(), this must fail ***/

#if PIP_VERSION > 1
  CHECK( pip_is_initialized(), 	    RV, 	return(EXIT_FAIL) );
  CHECK( pip_isa_root(), 	    RV, 	return(EXIT_FAIL) );
  CHECK( pip_isa_task(),	    RV, 	return(EXIT_FAIL) );
#endif
  CHECK( pip_get_pipid( NULL ),	    RV!=EPERM, 	return(EXIT_FAIL) );
  CHECK( pip_get_pipid( &pipid ),   RV!=EPERM, 	return(EXIT_FAIL) );
  CHECK( pip_get_mode( NULL ),	    RV!=EPERM, 	return(EXIT_FAIL) );
  CHECK( pip_get_mode( &mode ),	    RV!=EPERM, 	return(EXIT_FAIL) );
  CHECK( pip_get_ntasks( &ntasks ), RV!=EPERM,	return(EXIT_FAIL) );

  CHECK( ( ( mode_str = pip_get_mode_str() ) != NULL ),
	 RV,
	 return(EXIT_FAIL) );
#else
#if PIP_VERSION > 1
  CHECK( pip_is_initialized(), 	    !RV, 	return(EXIT_FAIL) );
#endif
  //CHECK( pip_isa_root(), 	    !RV, 	return(EXIT_FAIL) );
  //CHECK( pip_isa_task(),	    !RV, 	return(EXIT_FAIL) );
#if PIP_VERSION > 1
  CHECK( pip_get_pipid( NULL ),	    RV, 	return(EXIT_FAIL) );
#else
  CHECK( pip_get_pipid( NULL ),	    RV!=EINVAL, return(EXIT_FAIL) );
#endif
  CHECK( pip_get_pipid( &pipid ),   RV, 	return(EXIT_FAIL) );
#if PIP_VERSION > 1
  CHECK( pip_get_mode( NULL ),	    RV, 	return(EXIT_FAIL) );
  CHECK( pip_get_mode( &mode ),	    RV, 	return(EXIT_FAIL) );
  CHECK( pip_get_ntasks( &ntasks ), RV,	return(EXIT_FAIL) );
  CHECK( ( ( mode_str = pip_get_mode_str() ) == NULL ),
	 RV,
	 return(EXIT_FAIL) );
#else
  CHECK( pip_get_mode( NULL ),	    RV!=EINVAL, return(EXIT_FAIL) );
#endif
#endif
  ntasks = NTASKS;
  CHECK( pip_init( &pipid, &ntasks, NULL, 0 ), RV, return(EXIT_FAIL) );

  /*** after calling pip_init(), this must succeed ***/
#if PIP_VERSION > 1
  CHECK( pip_is_initialized(), 	!RV, 	return(EXIT_FAIL) );
  CHECK( pip_get_ntasks( &nt ),	RV,	return(EXIT_FAIL) );
  CHECK( nt!=ntasks, 		RV,	return(EXIT_FAIL) );
#endif
  CHECK( pip_get_mode( &mode ),	RV,	return(EXIT_FAIL) );

  CHECK( ( ( mode_str = pip_get_mode_str() ) != NULL ),
	 !RV,
	 return(EXIT_FAIL) );
  mode_str = pip_get_mode_str();
  if( strcmp( mode_str, PIP_ENV_MODE_PTHREAD          ) != 0 &&
      strcmp( mode_str, PIP_ENV_MODE_PROCESS          ) != 0 &&
      strcmp( mode_str, PIP_ENV_MODE_PROCESS_PRELOAD  ) != 0 &&
#if PIP_VERSION > 1
      strcmp( mode_str, PIP_ENV_MODE_PROCESS_GOT      ) != 0 &&
#endif
      strcmp( mode_str, PIP_ENV_MODE_PROCESS_PIPCLONE ) != 0 ) {
    CHECK( 1, RV, return(EXIT_FAIL) );
  }

  /* pip_isa_root() */
  if(
#if PIP_VERSION > 1
     pip_isa_root()
#else
     pipid == PIP_PIPID_ROOT
#endif
 ) {
    CHECK( pip_get_pipid( NULL ),     RV,   	return(EXIT_FAIL) );
    CHECK( pip_get_pipid( &id ),      RV, 	return(EXIT_FAIL) );
    CHECK( id!=pipid, 		      RV,	return(EXIT_FAIL) );
    CHECK( id!=PIP_PIPID_ROOT, 	      RV,	return(EXIT_FAIL) );

  } else if(
#if PIP_VERSION > 1
	    pip_isa_task()
#else
	    pipid != PIP_PIPID_ROOT
#endif
	    ) {
    char *env;
    int nte;

#if PIP_VERSION > 1
    CHECK( pip_get_pipid( NULL ),     RV,	  return(EXIT_FAIL) );
#else
    CHECK( pip_get_pipid( NULL ),     RV!=EINVAL, return(EXIT_FAIL) );
#endif
    CHECK( pip_get_pipid( &id ),      RV,	  return(EXIT_FAIL) );
    CHECK( id!=pipid, 		      RV,	  return(EXIT_FAIL) );

    env = getenv( PIP_TEST_NTASKS_ENV );
    CHECK( env==NULL, 		      RV,	  return(EXIT_FAIL) );

    nte = strtol( env, NULL, 10 );
    CHECK( ntasks!=nte,  	      RV,	  return(EXIT_FAIL) );

  } else {
    CHECK( 1, RV, return(EXIT_FAIL) );
  }
  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return EXIT_PASS;
}