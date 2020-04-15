/*! Printing on stdout, reading from stdin */

#include <api/stdio.h>

#include <lib/string.h>
#include <types/io.h>
#include <api/errno.h>

int _errno;	/* Error number that represent last syscall error */

console_t *u_stdout, *u_stderr;

/*! Initialize standard descriptors (input, output, error) */
int stdio_init ()
{
	extern console_t U_STDOUT, U_STDERR;

	u_stdout = &U_STDOUT;
	u_stdout->init (0);

	u_stderr = &U_STDERR;
	u_stderr->init (0);

	return EXIT_SUCCESS;
}

/*! Writes a string to stdout up to but not including the null character.
 *  A newline character is appended to the output.
 */
int puts( char *text )
{
  char text_to_print[CONSOLE_MAXLEN];
  strcpy(text_to_print, text);

  int len = strlen(text_to_print);
  text_to_print[len] = '\n';        //.print in vga_text
                                    //will process \n
  text_to_print[len + 1] = 0;

  return u_stdout->print( text_to_print );
}

/*! Formated output to console (lightweight version of 'printf') */
int printf ( char *format, ... )
{
	char text[CONSOLE_MAXLEN];

	vssprintf ( text, CONSOLE_MAXLEN, &format );

	return u_stdout->print ( text );
}

/*! Formated output to error console */
void warn ( char *format, ... )
{
	char text[CONSOLE_MAXLEN];

	vssprintf ( text, CONSOLE_MAXLEN, &format );

	u_stderr->print ( text );
}
