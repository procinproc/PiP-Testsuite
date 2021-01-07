#include <dlfcn.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void
mylocaltime2 (time_t t)
{
  struct tm *tm = localtime (&t);
  printf ("tm_isdst (from main program): %d\n", tm->tm_isdst);
  printf ("daylight (from main program): %d\n", daylight);
  printf ("timezone (from main program): %s\n", tzname[0]);
}

int
main (void)
{
  mylocaltime2 (1555332781);
}
