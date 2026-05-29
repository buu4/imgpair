#ifndef _ERROR_H
#define _ERROR_H

#define PNGPAIL_SUCCESS 0
#define PNGPAIL_FAILURE -1

typedef int    PNGPAILSTATUS;

extern PNGPAILSTATUS pngpail_errpuc (const char *reason);
extern const char *pngpail_failure_reason (void);
extern void error (const char *fmt, ...);

#endif /*_ERROR_H */
