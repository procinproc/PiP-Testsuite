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

#include <sys/time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEBUG_SCALE	(3)

static char *prog       = NULL;
static char *target     = NULL;
static pid_t pid        = 0;
static int timer_count  = 0;
static int timedout     = 0;
static int timer_period = 0;
static int timer_actual = 0;
static int ncores       = 0;
static float fnc        = 1.0;

static void cleanup( void ) {
  char *sysstr;

  if( pid > 0 ) {
    fprintf( stderr, "[%s] killing test program ...\n", prog );
    /* delicer SIGTERM to all PiP tasks and root */
    asprintf( &sysstr, "%s x -k -d %d", PIPS, pid );
    system( sysstr );
    free( sysstr );
    usleep( 100 * 10000 );	/* 10 msec */
    asprintf( &sysstr, "%s x -s KILL -d %d", PIPS, pid );
    system( sysstr );
    free( sysstr );
    if( kill( pid, SIGTERM ) == 0 || errno != ESRCH ) {
      usleep( 100 * 10000 );	/* 10 msec */
      (void) kill( pid, SIGKILL );
    }
  }
}

static int get_load() {
  FILE *fp;
  int	nc;
  float load = 0.0;

  if( ncores == 0 ) {
    if( ( fp = popen( "getconf _NPROCESSORS_ONLN", "r" ) ) != NULL ) {
      fscanf( fp, "%d", &ncores );
      fclose( fp );
      nc = ncores / 2;
      nc = ( nc > 0 ) ? nc : 1;
      fnc = 1.0 / (float) nc;
    }
  }
  if( ncores > 0 ) {
    if( ( fp = fopen( "/proc/loadavg", "r" ) ) != NULL ) {
      fscanf( fp, "%f", &load );
      fclose( fp );
      load *= fnc;
    }
  }
  return (int)load;
}

static void unset_timer( void ) {
  struct sigaction sigact;
  memset( (void*) &sigact, 0, sizeof( sigact ) );
  sigact.sa_handler = SIG_IGN;
  if( sigaction( SIGALRM, &sigact, NULL ) != 0 ) {
    fprintf( stderr, "[%s] sigaction(): %d\n", prog, errno );
  }
}

static void timer_watcher( int sig, siginfo_t *siginfo, void *dummy ) {
  int ld;

  if( timedout ) return;
  ld = get_load();
  timer_actual ++;
  if( ld == 0 ) {
    if( --timer_count <= 0 ) {
      timedout = 1;
      unset_timer();
      cleanup();
    }
  }
}

static void set_timer( int time ) {
  struct sigaction sigact;
  struct itimerval tv;
  int load = get_load();

  timer_period = time;
  timer_count  = time;

  memset( (void*) &sigact, 0, sizeof( sigact ) );
  sigact.sa_sigaction = timer_watcher;
  sigact.sa_flags     = SA_RESTART;
  if( sigaction( SIGALRM, &sigact, NULL ) != 0 ) {
    fprintf( stderr, "[%s] sigaction(): %d\n", prog, errno );
    cleanup();
    exit( EXIT_UNTESTED );
  }
  memset( &tv, 0, sizeof(tv) );
  if( time > 0 ) {
    tv.it_interval.tv_sec = 1;
    tv.it_value.tv_sec    = 1;
  } else {
    tv.it_interval.tv_usec = 10*1000;
    tv.it_value.tv_usec    = 10*1000;
  }
  if( setitimer( ITIMER_REAL, &tv, NULL ) != 0 ) {
    fprintf( stderr, "[%s] setitimer(): %d\n", prog, errno );
    cleanup();
    exit( EXIT_UNTESTED );
  }
}

static void usage( void ) {
  fprintf( stderr, "Usage: %s <time> <prog> [<args>]\n", prog );
  exit( EXIT_UNTESTED );
}

int main( int argc, char **argv ) {
  char	*pip_test_timer_scale = getenv("PIP_TEST_TIMER_SCALE");
  int 	time, status, flag_debug = 0;

  set_sigint_watcher();

  prog = basename( argv[0] );
  if( argc < 3 ) usage();
  target = basename( argv[2] );

  time = strtol( argv[1], NULL, 10 );
  if( time <  0 ) {
    time = -time;
    flag_debug = 1;
  }

  if( pip_test_timer_scale != NULL && isdigit(pip_test_timer_scale[0]) ) {
    time *= strtol( pip_test_timer_scale, NULL, 10 );
  }
#if PIP_VERSION_MAJOR > 1
  extern int pip_is_debug_build( void );
  if( pip_is_debug_build() && !flag_debug ) {
    time *= DEBUG_SCALE;
  }
#endif

  if( ( pid = fork() ) == 0 ) {
    execvp( argv[2], &argv[2] );
    fprintf( stderr, "[%s] execvp(%s): %s (%d)\n", prog, argv[2],
	     strerror(errno), errno );
    exit( EXIT_UNTESTED );

  } else if( pid > 0 ) {
    pid_t rv;

    set_timer( time );
    for( ;; ) {
      rv = wait( &status );
      if( rv != -1 || errno != EINTR ) break;
    }
    unset_timer();

    if( rv == -1 ) {
      fprintf( stderr, "'%s' failed to wait: %s\n", target, strerror(errno) );
    } else if( WIFEXITED( status ) ) {
      exit( WEXITSTATUS( status ) );
    } else if( WIFSIGNALED( status ) ) {
      if( timedout ) {
	fprintf( stderr, "\nTimer expired (%d/%d sec) !!\n\n", timer_period, timer_actual );
	cleanup();
      } else {
	int sig = WTERMSIG( status );
	fprintf( stderr, "'%s' terminated due to signal (%s)\n", target, strsignal(sig) );
      }
    }
    exit( EXIT_UNRESOLVED );

  } else {
    fprintf( stderr, "[%s] fork(): %d\n", prog, errno );
    unset_timer();
    exit( EXIT_UNTESTED );
  }
  exit( EXIT_UNTESTED );
}
