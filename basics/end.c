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

#define NITERS		(1000)

struct args {
  int	pipid;
  int 	before;
  int 	after;
};

int before_hook( void *args ) {
  struct args *argp = args;
  int pipid;
  printf( "[%d/%d] Before called\n", getpid(), argp->pipid );
  (argp->before) ++;
  return 0;
}

int after_hook( void *args ) {
  struct args *argp = args;
  int pipid;
  printf( "[%d/%d] After called\n", getpid(), argp->pipid );
  (argp->after) ++;
  return 0;
}

int check( struct args *argp, int nt, int test ) {
  int i, ret = 0;
  for( i=0; i<nt; i++ ) {
    switch( test ) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
      if( argp[i].before != 1 || argp[i].after != 1 ) {
	if( argp[i].before == 0 ) {
	  fprintf( stderr, "[%d] before was not called\n", i );
	  ret = 1;
	} else if( argp[i].before > 1 ) {
	  fprintf( stderr, "[%d] before was called more than once\n", i );
	  ret = 1;
	}
	if( argp[i].after == 0 ) {
	  fprintf( stderr, "[%d] after was not called\n", i );
	  ret = 1;
	} else if( argp[i].after > 1 ) {
	  fprintf( stderr, "[%d] after was called more than once\n", i );
	  ret = 1;
	}
      }
      break;
    case 3:			/* _exit */
    case 7:			/* fin-_exit */
      if( argp[i].before == 0 ) {
	fprintf( stderr, "[%d] before was not called\n", i );
	ret = 1;
      } else if( argp[i].before > 1 ) {
	fprintf( stderr, "[%d] before was called more than once\n", i );
	ret = 1;
      }
      if( argp[i].after != 0 ) {
	fprintf( stderr, "[%d] after ???\n", i );
	ret = 1;
      }
      break;
    default:
      fprintf( stderr, "[%d] ???\n", i );
      ret = 1;
      break;
    }
  }
  return ret;
}

static int choice( char *opt ) {
  if( opt == NULL || *opt == '\0' ||
      strcasecmp( "return", opt ) == 0 ) {
    return 0;
  } else if( strcasecmp( "exit", opt ) == 0 ) {
    return 1;
  } else if( strcasecmp( "pip_exit", opt ) == 0 ) {
    return 2;
  } else if( strcasecmp( "_exit", opt ) == 0 ) {
    return 3;
  } else if( strcasecmp( "fin-return", opt ) == 0 ) {
    return 4;
  } else if( strcasecmp( "fin-exit", opt ) == 0 ) {
    return 5;
  } else if( strcasecmp( "fin-pip_exit", opt ) == 0 ) {
    return 6;
  } else if( strcasecmp( "fin-_exit", opt ) == 0 ) {
    return 7;
  } else {
    fprintf( stderr, "Unknown choice: %s\n", opt );
    exit( 1 );
  }
}


int main( int argc, char **argv ) {
  pip_barrier_t	barr, *barrp = &barr;
  struct args *argp;
  int 	ntasks, ntenv, pipid;
  int	end_rt = 0, end_tk = 0;
  int	nt, i, extval = 0;

  ntasks = 0;
  if( argc > 1 ) {
    ntasks = strtol( argv[1], NULL, 10 );
  }
  ntasks = ( ntasks < 0 ) ? 8 : ntasks;
  if( ntasks > NTASKS ) return EXIT_UNTESTED;

  if( argc > 2 ) {
    end_rt = choice( argv[2] );
  }
  if( argc > 3 ) {
    end_tk = choice( argv[3] );
  }

  nt = ( ntasks < 1 ) ? 1 : ntasks;
  CHECK( pip_init( &pipid, &nt, (void**)&barrp, 0 ), 
	 RV, 
	 return(EXIT_FAIL) );
  if( pipid == PIP_PIPID_ROOT ) {
    if( ntasks > 0 ) {
      argp = (struct args*) malloc( sizeof(struct args) * ntasks );
      CHECK( pip_barrier_init(barrp,ntasks), RV, return(EXIT_FAIL) );
    }
    for( i=0; i<ntasks; i++ ) {
      pipid = i;
      argp[i].pipid  = i;
      argp[i].before = 0;
      argp[i].after  = 0;
      CHECK( pip_spawn(argv[0],argv,NULL,PIP_CPUCORE_ASIS,&pipid,
		       before_hook,after_hook,&argp[i]),
	     RV,
	     return(EXIT_FAIL) );
    }
    for( i=0; i<ntasks; i++ ) {
      int status;
      CHECK( pip_wait_any( NULL, &status ), RV, return(EXIT_FAIL) );
      if( WIFEXITED( status ) ) {
	CHECK( ( extval = WEXITSTATUS( status ) ),
	       RV,
	       return(EXIT_FAIL) );
      } else {
	CHECK( "Task is signaled", RV, return(EXIT_UNRESOLVED) );
      }
    }
    if( check( argp, ntasks, end_tk ) != 0 ) {
      return EXIT_FAIL;
    }
    switch( end_rt ) {
    case 0:
      break;
    case 1: 
      exit( extval );
    case 2: 
      pip_exit( extval );
    case 3: 
      _exit( extval );
    case 4: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      return extval;
    case 5: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      exit( extval );
    case 6: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      pip_exit( extval );
    case 7: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      _exit( extval );
    }

  } else {
    CHECK( pip_barrier_wait( barrp ), RV, return(EXIT_FAIL) );
    switch( end_tk ) {
    case 0:
      break;
    case 1: 
      exit( extval );
    case 2: 
      pip_exit( extval );
    case 3: 
      _exit( extval );
    case 4: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      return extval;
    case 5: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      exit( extval );
    case 6: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      pip_exit( extval );
    case 7: 
      CHECK( pip_fin(), RV, return(EXIT_FAIL) );
      _exit( extval );
    }
  }
  /* never reach here */
  return extval;
}
