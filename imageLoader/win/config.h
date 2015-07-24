/* package definitions */
#define PKG_MAINTAINER "Enno Cramer <uebergeek@web.de>"
#define PKG_REVISION "unknown"

/* architecture */
/* #undef const */
/* #undef inline */
/* #undef WORDS_BIGENDIAN */

/* c type sizes */
#define SIZEOF_SHORT_INT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG_INT 4

/* functions */
#define HAVE_SNPRINTF 1
/* #undef HAVE_VSNPRINTF */
/* #undef HAVE_FCONVERT */
/* #undef HAVE_FCVT */

/* header */
/* #undef HAVE_EXECINFO_H */

/* libraries */
/* #undef HAVE_LIBZ */


typedef unsigned char uint8;
typedef signed char sint8;

#if SIZEOF_SHORT_INT == 2
typedef unsigned short int uint16;
typedef signed short int sint16;
#elif SIZEOF_INT == 2
typedef unsigned int uint16;
typedef signed int sint16;
#else
# error "cannot define int16"
#endif

#if SIZEOF_INT == 4
typedef unsigned int uint32;
typedef signed int sint32;
#elif SIZEOF_LONG_INT == 4
typedef unsigned long int uint32;
typedef signed long int sint32;
#else
# error "cannot define int32"
#endif

