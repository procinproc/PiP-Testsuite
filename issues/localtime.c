/* this program was based on the code written by Florian Weimer */
/* https://sourceware.org/bugzilla/show_bug.cgi?id=11754#c15    */

#ifndef NO_PIP
#include <test.h>
#else
#include <stdio.h>
#endif
#include <time.h>

static int mylocaltime (char *tz, time_t t, int dl)
{
  struct tm *tm;

  setenv( "TZ", tz, 1 );
  tzset();
  tm = localtime (&t);

  printf( "%s\t%d/%d/%d %d:%d:%d\n", tz,
	  tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
	  tm->tm_hour,      tm->tm_min,   tm->tm_sec );
  printf( "\ttimezone: %s",   tzname[0] );
  printf( "\tdaylight: %d\n", daylight  );

#ifdef NO_PIP
  if( daylight != dl ) {
    fprintf( stderr, "daylight(%d) != dl(%d)\n", daylight, dl );
    return 1;
  }
#else
  CHECK( daylight!=dl, RV, return(EXIT_FAIL) );
#endif
}

int main() {
  time_t t = 1555332781;	/* 4/15 12:53:1 in Tokyo */
  int err = 0;

  err += mylocaltime ( "Asis/Tokyo",      t, 0 );
  err += mylocaltime ( "Europe/Berlin",   t, 1 );
  err += mylocaltime ( "America/Chicago", t, 1 );

  if( err ) {
#ifdef NO_PIP
    return 1;
#else
    return EXIT_FAIL;
#endif
  }
  return 0;
}
