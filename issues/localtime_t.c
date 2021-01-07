/*
  https://sourceware.org/bugzilla/show_bug.cgi?id=11754#c15
*/

#include <test.h>
#include <sys/time.h>
#include <time.h>

void mylocaltime( int pipid, time_t t ) {
  extern char *tzname[2];
  struct tm *tm;
  struct timeval  tv;
  struct timezone tz;

  tm = localtime( &t );
  if( pipid == PIP_PIPID_ROOT ) {
    printf( "ROOT:    tm: %p\n", tm );
    printf( "ROOT:    tm_isdst: %d\n", tm->tm_isdst );
    printf( "ROOT:    daylight: %d\n", daylight );
    printf( "ROOT:    %d/%d %02d:%02d\n", tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min );
    printf( "ROOT:    %s / %s\n", tzname[0], tzname[1] );
    gettimeofday( NULL, &tz );
    printf( "ROOT:    %d  %d\n", tz.tz_minuteswest, tz.tz_dsttime );
  } else {
    printf( "TASK[%d]: tm: %p\n", pipid, tm );
    printf( "TASK[%d]: tm_isdst: %d\n", pipid, tm->tm_isdst );
    printf( "TASK[%d]: daylight: %d\n", pipid, daylight );
    printf( "TASK[%d]: %d/%d %02d:%02d\n", pipid,
	    tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min );
    printf( "TASK[%d]: %s / %s\n", pipid, tzname[0], tzname[1] );
    gettimeofday( NULL, &tz );
    printf( "TASK[%d]: %d  %d\n", pipid, tz.tz_minuteswest, tz.tz_dsttime );
  }
}

int main( int argc, char **argv ) {
  char *env;
  int pipid, ntasks = 1;
  int extval = EXIT_PASS;
  time_t ltm;

  if( argv[1] == NULL ) {
    ltm = 1555332781;
  } else {
    ltm = (time_t) strtoll( argv[1], NULL, 10 );
  }
  //CHECK( pip_init( &pipid, &ntasks, NULL, 0 ), RV, return(EXIT_FAIL) );

  mylocaltime( pipid, ltm );

  CHECK( pip_fin(), RV, return(EXIT_FAIL) );
  return extval;
}
