#include <stdio.h>
#include <api/prog_info.h>

int hello_world ()
{
   printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
       hello_world_PROG_HELP );

   printf ( "Hello World!\n" );

   MINUS();

   return 0;
}
