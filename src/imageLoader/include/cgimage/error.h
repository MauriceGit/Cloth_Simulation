/*********************************************************************
* lescegra - image loader                                            *
*                                                                    *
* http://geeky.kicks-ass.org/projects/lescegra.html                  *
*                                                                    *
* Copyright 2003-2005 by Enno Cramer <uebergeek@web.de>              *
*                                                                    *
* This library is free software; you can redistribute it and/or      *
* modify it under the terms of the GNU Library General Public        *
* License as published by the Free Software Foundation; either       *
* version 2 of the License, or (at your option) any later version.   *
*                                                                    *
* This library is distributed in the hope that it will be useful,    *
* but WITHOUT ANY WARRANTY; without even the implied warranty of     *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
* Library General Public License for more details.                   *
*                                                                    *
* You should have received a copy of the GNU Library General Public  *
* License along with this library; if not, write to the Free         *
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. *
*********************************************************************/

#ifndef CG_ERROR_H
#define CG_ERROR_H 1

/**
 * Class: CGError
 * Error handling functions
 */

/**
 * Type: CGError
 * Encapsulate an error message and the location where it has been reported.
 */
typedef struct {
    const char* message;
    const char* file;
    const char* method;
    unsigned int line;
} CGError;

/**
 * Prototype: CGErrorHandler
 * Error handler prototype.
 */
typedef void (*CGErrorHandler)(const CGError* error);

/**
 * Function: CGError_registerHandler
 * Register a new error handler.
 *
 * Parameters:
 *   handler - new error handling function
 */
void CGError_registerHandler(CGErrorHandler handler);

/**
 * Function: CGError_report
 * Report an error condition using the currently registered error
 * handler.
 *
 * Parameters:
 *   file    - source file from where the error is reported
 *   method  - method reporting the error
 *   line    - line of the source file where the error is reported
 *   message - error message
 */
void CGError_report(
    const char* file, const char* method, unsigned int line,
    const char* message
);

/**
 * Function: CGError_reportFormat
 * Report an error condition with a formatted message using the
 * currently registered error handler.
 *
 * Parameters:
 *   file   - source file from where the error is reported
 *   method - method reporting the error
 *   line   - line of the source file where the error is reported
 *   format - error message template
 */
void CGError_reportFormat(
    const char* file, const char* method, unsigned int line,
    const char* format, ...
);

/**
 * Function: CGError_printStackTrace
 * Print a backtrace of the current call stack.
 */
void CGError_printStackTrace(void);

/**
 * Function: CGError_abort
 * Report an error condition and abort the program.
 *
 * Parameters:
 *   file    - source file from where the error is reported
 *   method  - method reporting the error
 *   line    - line of the source where the error is reported
 *   message - error message
 */
void CGError_abort(
    const char* file, const char* method, unsigned int line,
    const char* message
);

/**
 * Function: CGError_abortFormat
 * Report an error condition with a formatted message and abort the
 * program.
 *
 * Parameters:
 *   file   - source file from where the error is reported
 *   method - method reporting the error
 *   line   - line of the source where the error is reported
 *   format - error message template
 */
void CGError_abortFormat(
    const char* file, const char* method, unsigned int line,
    const char* format, ...
);

#ifdef __GNUC__
# define CG_ERROR_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif __STDC_VERSION__ >= 199901L
# define CG_ERROR_CURRENT_FUNCTION __func__
#else
# define CG_ERROR_CURRENT_FUNCTION ((const char*)0)
#endif

#define cg_abort(msg) \
    CGError_abort(__FILE__, CG_ERROR_CURRENT_FUNCTION, __LINE__, (msg))

#undef cg_assert
#undef cg_assert_msg

#ifdef NDEBUG
# define cg_assert_msg(expr, msg)
# define cg_assert(expr)
#else
# define cg_assert_msg(expr, msg) \
    ((expr) ? 0 : (cg_abort(msg), 0))
# define cg_assert(expr) \
    cg_assert_msg((expr), "Assertion '" #expr "' failed")
#endif

#endif
