/*! Hello world program */

#include <stdio.h>
#include <api/prog_info.h>
#include <lib/string.h>

int hello_world ()
{
  int text_size = 100;
  
  //"Manually" format with vssprintf to use puts
  char* ptr1 = "Example program: [%s:%s]\n%s\n\n";
  char *format[3];
  format[0] = ptr1;
  format[1] = (char*)__FILE__;
  format[2] = (char*)__FUNCTION__;

  char text_formatted[text_size];
  vssprintf( text_formatted, text_size, format );

	puts ( text_formatted );
	puts ( "Hello World!\n" );

#if 0	/* test escape sequence */
	printf ( "\x1b[20;40H" "Hello World at 40, 20!\n" );
#endif

	return 0;
}
