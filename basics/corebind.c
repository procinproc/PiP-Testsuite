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

static void fill_cpuset( cpu_set_t *fullset ) {
  cpu_set_t zero, save;
  int i, n;

  CHECK( sched_getaffinity( 0, sizeof(zero), &save ), RV, exit(errno) );
  CPU_ZERO( fullset );
  n = sizeof(cpu_set_t) * 8;
  for( i=0; i<n; i++ ) {
    CPU_ZERO( &zero );
    CPU_SET( i, &zero );
    if( sched_setaffinity( 0, sizeof(zero), &zero ) == 0 ) {
      CPU_SET( i, fullset );
    }
  }
  n = sizeof(cpu_set_t) / sizeof(int);
  for( i=0; i<n; i++ ) {
    unsigned int cpu = ((unsigned int*)(fullset))[i];
    if( cpu != 0 ) fprintf( stderr, "CPUSET[%d] 0x%x\n", i, cpu );
  }
  fprintf( stderr, "CPU count: %d\n", CPU_COUNT( fullset ) );
  CHECK( sched_setaffinity( 0, sizeof(zero), &save ), RV, exit(errno) );
}

static int nth_core( int nth, cpu_set_t *cpuset ) {
  int i, j, ncores;
  ncores = CPU_COUNT( cpuset );
  nth %= ncores;
  for( i=0, j=0; i<CPU_SETSIZE; i++ ) {
    if( CPU_ISSET( i, cpuset ) ) {
      if( j++ == nth ) return i;
    }
  }
  return -1;
}

int main( int argc, char **argv ) {
  cpu_set_t	init_set, *init_setp, cpuset, cpufull;
  void		*exp;
  char		*env;
  int 		ntasks, hntasks, ntenv, pipid;
  int		core, i, n, extval = 0;

  ntasks = 0;
  if( argc > 1 ) {
    ntasks = strtol( argv[1], NULL, 10 );
  }
  ntasks = ( ntasks <= 0 ) ? NTASKS : ntasks;
  if( ( env = getenv( "NTASKS" ) ) != NULL ) {
    ntenv = strtol( env, NULL, 10 );
    if( ntasks > ntenv ) return(EXIT_UNTESTED);
  } else {
    if( ntasks > NTASKS ) return(EXIT_UNTESTED);
  }
  hntasks = ntasks / 2;
  hntasks = ( hntasks < 1 ) ? 1 : hntasks;

  fill_cpuset( &cpufull );

  init_setp = &init_set;
  exp = init_setp;
  CHECK( pip_init(&pipid,&ntasks,(void**)&exp,0), RV, return(EXIT_FAIL) );
  if( pipid == PIP_PIPID_ROOT ) {
    for( i=0; i<hntasks; i++ ) {
      core = nth_core( i, &cpufull );
      pipid = i;
      printf( ">> PIPID:%d core:%d\n", pipid, core );
      CHECK( pip_spawn(argv[0],argv,NULL,core,&pipid,NULL,NULL,NULL),
	     RV,
	     return(EXIT_FAIL) );
    }
    for( i=hntasks; i<ntasks; i++ ) {
      core = nth_core( i, &cpufull );
      CPU_ZERO( &cpuset );
      CPU_SET( core, &cpuset );
      CHECK( sched_setaffinity( 0, sizeof(cpuset), &cpuset ),
	     RV,
	     return(EXIT_UNRESOLVED) );
      pipid = i;
      printf( ">> PIPID:%d core:%d\n", pipid, core );
      CHECK( pip_spawn(argv[0],argv,NULL,PIP_CPUCORE_ASIS,&pipid,NULL,NULL,NULL),
	     RV,
	     return(EXIT_FAIL) );
    }
    for( i=0; i<ntasks; i++ ) {
      int status;
      CHECK( pip_wait_any( NULL, &status ), RV, return(EXIT_FAIL) );
      if( WIFEXITED( status ) ) {
	extval = WEXITSTATUS( status );
	CHECK( ( extval != 0 ), RV, return(EXIT_FAIL) );
      } else {
	CHECK( 1, RV, RV=0 );
	extval = EXIT_UNRESOLVED;
	break;
      }
    }

  } else {
    init_setp = (cpu_set_t*) exp;
    core = nth_core( pipid, &cpufull );
    CHECK( sched_getaffinity( 0, sizeof(cpuset), &cpuset ),
	   RV,
	   return(EXIT_FAIL) );
    if( !CPU_ISSET( core, &cpuset ) ) {
      n = sizeof( cpuset ) / sizeof( int );
      for( i=0; i<n; i++ ) {
	unsigned int cpu = ((unsigned int*)(&cpuset))[i];
	if( cpu != 0 ) fprintf( stderr, "CPUSET[%d] 0x%x (core:%d)\n", i, cpu, core );
      }
    }
    CHECK( CPU_ISSET( core, &cpuset ),
	   !RV,
	   return(EXIT_FAIL) );
    printf( "<< PIPID:%d core:%d\n", pipid, core );
  }
  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return extval;
}
