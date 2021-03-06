AC_INIT([libac], [0.1.0], [contactandyc@gmail.com], [anotherclibrary])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX

AC_LANG([C])
AC_LANG([C++])

LT_INIT([disable-shared])
AC_CONFIG_MACRO_DIR([m4])

test -z "$SED" && SED=sed

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
                  [whether to include debug symbols (default is no)])],
  [enable_debug=$enableval],
  [enable_debug=no]
)

if test "x$enable_debug" = xyes; then
  dnl Remove all optimization flags from CFLAGS
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9s]*//g'`

  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-g[0-9]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-g[0-9]*//g'`
  changequote([,])

  CFLAGS="$CFLAGS -g -O0"
  CXXFLAGS="$CXXFLAGS -g -O0"
fi

if test "x$enable_debug" = xno; then
  dnl Remove all optimization flags from CFLAGS
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9s]*//g'`

  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-g[0-9]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-g[0-9]*//g'`
  changequote([,])

  CFLAGS="$CFLAGS -O3"
  CXXFLAGS="$CXXFLAGS -O3"
fi

AC_SEARCH_LIBS([uv_loop_init], [uv], [], [AC_MSG_ERROR([unable to find libuv])])
AC_CHECK_HEADER(uv.h, , [AC_MSG_ERROR([Unable to find uv.h; Install libuv dev package])])
CFLAGS="$CFLAGS -std=gnu99 -Wall"
AM_PATH_CHECK

AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([demo/Makefile])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
