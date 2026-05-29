#include "error.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

static const char *_pngpail_error_reason = NULL;
extern char *progname;

PNGPAILSTATUS pngpail_errpuc (const char *reason)
{
  _pngpail_error_reason = reason;
  return PNGPAIL_FAILURE;
}

const char *pngpail_failure_reason (void)
{
  return _pngpail_error_reason;
}

void error (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  fprintf (stderr, "%s: error: ", progname);
  vfprintf (stderr, fmt, ap);
  putc ('\n', stderr);
  fflush (stderr);
  va_end (ap);
}
