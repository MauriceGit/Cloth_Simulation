AC_INIT(lescegra--image-loader, 0.0.20051023, uebergeek@web.de)

# define output files
AC_CONFIG_COMMANDS(
    [build_tree],
    [
        mkdir -p src lib doc
    ]
)

AC_CONFIG_HEADERS([
    config.h
])

AC_CONFIG_FILES([
    config.mk
    Makefile
])

# check source dir
AC_CONFIG_SRCDIR([Makefile.in])
AC_CONFIG_AUX_DIR([autotools])

# include autoconf macros
m4_include([/usr/share/autoconf-archive/ax_cflags_gcc_option.m4])

# get real package info
[
    PKG_REVISION="unknown"
    PKG_MAINTAINER="Enno Cramer <uebergeek@web.de>"

    if test -f "$srcdir/VERSION"; then
        PKG_REVISION=`head -n1 < "$srcdir/REVISION"`
        PKG_MAINTAINER=`grep -e "^Maintainer:" < "$srcdir/REVISION"`

    elif test -d "$srcdir/{arch}" && tla --version >/dev/null 2>&1; then
        PKG_REVISION=`cd "$srcdir" && tla tree-version`--`tla logs -d "$srcdir" | tail -n1`
        PKG_MAINTAINER=`tla my-id`

        if tla changes -d "$srcdir" >/dev/null 2>&1; then
            :
        else
            PKG_REVISION="$PKG_REVISION + modifications"
        fi
    fi
]

AC_DEFINE_UNQUOTED(PKG_MAINTAINER, "$PKG_MAINTAINER")
AC_DEFINE_UNQUOTED(PKG_REVISION,   "$PKG_REVISION")

# configure options
AC_ARG_VAR(SHELL,       [posix shell])
AC_ARG_VAR(CC,          [c compiler])
AC_ARG_VAR(AR,          [static library generator])
AC_ARG_VAR(MAKEDEPEND,  [automatic dependency generator])
AC_ARG_VAR(NATURALDOCS, [natrual docs api doc generator])

AC_CANONICAL_HOST()

# programs
AC_PROG_CC()
AC_PROG_CPP()
AC_CHECK_PROG(AR, ar, ar)

AC_CHECK_PROG(MAKEDEPEND, makedepend, makedepend, :)
AC_CHECK_PROG(NATURALDOCS, NaturalDocs, NaturalDocs)

# architecture
AC_C_CONST()
AC_C_INLINE()
AC_C_BIGENDIAN()

AC_CHECK_SIZEOF(short int)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long int)

AC_CHECK_FUNCS(snprintf vsnprintf fconvert fcvt)

AC_CHECK_HEADER([execinfo.h], [AC_DEFINE(HAVE_EXECINFO_H, 1, [call stack backtrace])], [])

AC_LANG_PUSH([C])

AC_CHECK_LIB(z, inflate)
AC_CHECK_HEADER([zlib.h], [], [ac_cv_lib_zlib_inflate=no])

AC_LANG_POP([C])

AC_SUBST(HAVE_LIBZ,   [$ac_cv_lib_z_inflate])

# test dependency availability

if test "$ac_cv_lib_z_inflate" != yes; then
    AC_MSG_WARN([libz not availble, not building png loader])
fi

if test "$MAKEDEPEND" = ":"; then
    AC_MSG_WARN([makedepend not available, rebuilds may be incomplete])
fi

AC_OUTPUT()
