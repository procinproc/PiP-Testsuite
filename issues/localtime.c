/* this program was based on the code written by Florian Weimer */
/* https://sourceware.org/bugzilla/show_bug.cgi?id=11754#c15    */

#include <test.h>
#include <time.h>

static void
mylocaltime (char *tz, time_t t, int dl)
{
  struct tm *tm;

  setenv( "TZ", tz, 1 );
  tzset();
  tm = localtime (&t);

  printf ("%s\t%d/%d/%d %d:%d:%d\n", tz,
	  tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
	  tm->tm_hour,      tm->tm_min,   tm->tm_sec );
  printf ("\ttimezone: %s",   tzname[0]);
  printf ("\tdaylight: %d\n", daylight);

  CHECK( daylight!=dl, RV, exit(EXIT_FAIL) );
}

int
main (void)
{
  time_t t = 1555332781;	/* 4/15 12:53 in Tokyo */

  mylocaltime ( "Asis/Tokyo",      t, 0 );
  mylocaltime ( "Europe/Berlin",   t, 1 );
  mylocaltime ( "America/Chicago", t, 1 );
}
