/*********************************************************************
* lescegra                                                           *
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

#ifndef CG_COMPAT_H
#define CG_COMPAT_H 1

#include <stddef.h>
#include <stdarg.h>

int snprintf(char* buf, size_t size, const char* fmt, ...);
int vsnprintf(char* buf, size_t size, const char* fmt, va_list args);

#endif
