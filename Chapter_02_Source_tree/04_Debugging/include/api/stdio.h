/*! Printing on stdout/stderr */
#pragma once

int printf ( char *format, ... );
void warn ( char *format, ... );
int puts ( char *text );

int stdio_init ();
