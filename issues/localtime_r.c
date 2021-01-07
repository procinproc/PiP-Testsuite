/*
  https://sourceware.org/bugzilla/show_bug.cgi?id=11754#c15
*/

#include <test.h>
#include <sys/time.h>
#include <time.h>

char *task_prog = "./localtime_t";
char *time_str  = "1555332781";

void mylocaltime( int pipid, time_t t ) {
  extern char *tzname[2];
  struct tm *tm;
  struct timeval  tv;
  struct timezone tz;
  
  tm = localtime( &t );
  printf( "ROOT:    tm: %p\n", tm );
  printf( "ROOT:    tm_isdst: %d\n", tm->tm_isdst );
  printf( "ROOT:    daylight: %d\n", daylight );
  printf( "ROOT:    %d/%d %2d:%2d\n", tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min );
  printf( "ROOT:    %s / %s\n", tzname[0], tzname[1] );
  gettimeofday( &tv, &tz );
  printf( "ROOT:    %d  %d\n", tz.tz_minuteswest, tz.tz_dsttime );
}

int main( int argc, char **argv ) {
  char *env;
  int pipid, ntasks, ntenv;
  int extval = EXIT_PASS;
  time_t ltm = (time_t) strtoll( time_str, NULL, 10 );
  char *newargv[] = { task_prog, time_str, NULL };
  int i, coreno, status = 0;

  ntasks = 0;
  if( argc > 1 ) {
    ntasks = strtol( argv[1], NULL, 10 );
  }
  ntasks = ( ntasks <= 0 || ntasks > 10 ) ? 10 : ntasks;

  CHECK( pip_init( &pipid, &ntasks, NULL, 0 ), RV, return(EXIT_FAIL) );

  setenv ("TZ", "Europe/Berlin", 1);
  mylocaltime( pipid, ltm );

  for( i=0; i<ntasks; i++ ) {
    pipid = PIP_PIPID_ANY;
    CHECK( pip_spawn( newargv[0], newargv, NULL, PIP_CPUCORE_ASIS, &pipid,
		      NULL, NULL, NULL ),
	   RV,
	     return(EXIT_FAIL) );
  }
  for( i=0; i<ntasks; i++ ) {
    CHECK( pip_wait( i, &status ), RV, return(EXIT_FAIL) );
    if( WIFEXITED( status ) ) {
      extval = WEXITSTATUS( status );
    } else {
      extval = EXIT_UNRESOLVED;
    }
  }
  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return extval;
}
