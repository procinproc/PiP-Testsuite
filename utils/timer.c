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

#include <sys/time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEBUG_SCALE	(3)

static pid_t pid        = 0;
static char *prog       = NULL;
static char *target     = NULL;
static int timer_count  = 0;
static int timedout     = 0;
static int timer_period = 0;
static int timer_actual = 0;

static void cleanup( void ) {
  if( pid > 0 ) {
    (void) kill( pid, SIGHUP );
    system( PIPS );
    sleep( 1 );
    system( PIPS " -s KILL > /dev/null 2>&1" );
    (void) kill( pid, SIGKILL );
  }
}

static int get_load() {
  FILE *fp;
  int ncores = 0;
  float load;

  if( ( fp = popen( "getconf _NPROCESSORS_ONLN", "r" ) ) == NULL ) {
    return 0;
  }
  fscanf( fp, "%d", &ncores );
  fclose( fp );

  if( ( fp = popen( "cat /proc/loadavg", "r" ) ) == NULL ) {
    return 0;
  }
  fscanf( fp, "%f", &load );
  fclose( fp );

  load /= (float) ncores;
  printf( "load: %g\n", load );
  return (int)load;
}

static bool ignore_alarm( void ) {
  struct sigaction sigact;
  memset( (void*) &sigact, 0, sizeof( sigact ) );
  sigact.sa_handler = SIG_IGN;
  return( sigaction( SIGALRM, &sigact, NULL ) == 0 );
}

static void timer_watcher( int sig, siginfo_t *siginfo, void *dummy ) {
  timer_actual ++;
  printf( "%d %d\n", timer_count, timer_actual );
  if( --timer_count == 0 ) {
    timedout = 1;
    (void) ignore_alarm();
    cleanup();
  } else if( get_load() > 1 ) {
    timer_count += 1;
  }
}

static void set_timer( int timer ) {
  struct sigaction sigact;
  struct itimerval tv;
  int load = get_load();

  timer *= ( load + 1 );
  timer_period = timer;
  timer_count  = timer;

  memset( (void*) &sigact, 0, sizeof( sigact ) );
  sigact.sa_sigaction = timer_watcher;
  sigact.sa_flags     = SA_RESTART;
  if( sigaction( SIGALRM, &sigact, NULL ) != 0 ) {
    fprintf( stderr, "[%s] sigaction(): %d\n", prog, errno );
    cleanup();
    exit( EXIT_UNTESTED );
  }
  memset( &tv, 0, sizeof(tv) );
  if( timer > 0 ) {
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

static void unset_timer( void ) {
  if( !ignore_alarm() ) {
    fprintf( stderr, "[%s] sigaction(): %d\n", prog, errno );
    cleanup();
  }
}

static void usage( void ) {
  fprintf( stderr, "Usage: %s <time> <prog> [<args>]\n", prog );
  exit( EXIT_UNTESTED );
}

int main( int argc, char **argv ) {
  int 	time, status, flag_debug = 0;
  char	*pip_test_timer_scale = getenv("PIP_TEST_TIMER_SCALE");

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
#if PIP_VERSION > 1
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
      if (rv != -1 || errno != EINTR) break;
    }
    if (rv == -1) {
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