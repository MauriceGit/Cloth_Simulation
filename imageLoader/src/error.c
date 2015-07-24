#include <cgimage/error.h>
#include <cgimage/compat.h>

#include <config.h>
#ifdef HAVE_EXECINFO_H
# include <execinfo.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static void CGError_defaultErrorHandler(const CGError* error);

/* error handler */
static CGErrorHandler CGError_handleError = CGError_defaultErrorHandler;

void CGError_registerHandler(CGErrorHandler handler) {
    if (handler)
        CGError_handleError = handler;
    else
        CGError_handleError = CGError_defaultErrorHandler;
}

void CGError_report(
    const char* file, const char* method, unsigned int line,
    const char* message
) {
    CGError error;

    error.file    = file;
    error.method  = method;
    error.line    = line;
    error.message = message;

    CGError_handleError(&error);
}

void CGError_reportFormat(
    const char* file, const char* method, unsigned int line,
    const char* format, ...
) {
    char buffer[1024];
    va_list list;

    va_start(list, format);
    vsnprintf(buffer, 1024, format, list);
    va_end(list);

    CGError_report(file, method, line, buffer);
}

void CGError_printStackTrace(void) {
#if HAVE_EXECINFO_H
    char    **bt_syms;
    void    *bt[128];
    int     bt_size;
    int     i;
       
    bt_size = backtrace(bt, sizeof(bt) / sizeof(*bt));
    bt_syms = backtrace_symbols(bt, bt_size);

    fprintf(stderr, "Current call stack:\n");
    for(i = 0; i < bt_size; i++) {
        fprintf(stderr, "  #%-2d  %s\n", i, bt_syms[i]);
    }
    free(bt_syms);
#endif
}

static void CGError_defaultErrorHandler(const CGError* error) {
    if (error->method) {
        fprintf(
            stderr,
            "%s:%d (%s): %s\n",
            error->file,
            error->line,
            error->method,
            error->message
        );

    } else {
        fprintf(
            stderr,
            "%s:%d: %s\n",
            error->file,
            error->line,
            error->message
        );
    }
}

void CGError_abort(
    const char* file, const char* method, unsigned int line,
    const char* message
) {
    fprintf(stderr, "PANIC: %s,\n", message);

      if (method)
        fprintf(stderr, "at %s:%u (%s).\n\n", file, line, method);
      else
        fprintf(stderr, "at %s:%u.\n\n", file, line);

    CGError_printStackTrace();

    abort();
}

void CGError_abortFormat(
    const char* file, const char* method, unsigned int line,
    const char* format, ...
) {
    va_list vargs;

    fprintf(stderr, "PANIC: ");

    va_start(vargs, format);
    vfprintf(stderr, format, vargs);
    va_end(vargs);

    fprintf(stderr, "\n");

    if (method)
        fprintf(stderr, "at %s:%u (%s).\n\n", file, line, method);
    else
        fprintf(stderr, "at %s:%u.\n\n", file, line);
  
    CGError_printStackTrace();

    abort();
}
