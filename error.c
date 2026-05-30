#include "error.h"
#include <stdarg.h>
#include <stdio.h>

extern char *progname;

void report_error (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  fprintf (stderr, "%s: error: ", progname);
  vfprintf (stderr, fmt, ap);
  putc ('\n', stderr);
  fflush (stderr);
  va_end (ap);
}
