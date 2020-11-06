#include <unistd.h>
#include <stdlib.h>

int main( int argc, char **argv ) {
  if( argc > 1 ) {
    int t = strtol( argv[1], NULL, 10 );
    if( t > 0 ) sleep( t );
  }
  return 0;
}
