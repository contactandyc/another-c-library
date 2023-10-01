AC_INIT([anotherclibrary], [1.0], [contactandyc@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [Enable debugging (default is no)])],
  [case "${enableval}" in
   yes) CFLAGS="-I\${top_srcdir}/include -g -O0" ;;
   no)  CFLAGS="-I\${top_srcdir}/include -O3" ;;
   *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
   esac],
  [CFLAGS="-I\${top_srcdir}/include -O3"])

AM_PROG_AR
AC_PROG_CC
LT_INIT


AC_PATH_PROG(PKG_CONFIG, pkg-config)
PKG_PROG_PKG_CONFIG([0.29])
PKG_CHECK_MODULES(LIBUV, libuv >= 1.35)

# Check for necessary libraries, functions, etc.
AC_CHECK_LIB([z], [gzread], [], [
  AC_MSG_ERROR([zlib library not found])
])

# Check for Address Sanitizer
AC_ARG_ENABLE([address-sanitizer],
              [AS_HELP_STRING([--enable-address-sanitizer], [Enable Address Sanitizer])],
              [if test "x$enable_address_sanitizer" = "xyes"; then
                 CFLAGS="$CFLAGS -fsanitize=address"
               fi])

# Check for Memory Check
AC_ARG_WITH([memory-check],
            [AS_HELP_STRING([--with-memory-check], [Enable Memory Check with given value])],
            [with_memory_check=$withval], [with_memory_check=no])
AM_CONDITIONAL([WITH_MEMORY_CHECK], [test "x$with_memory_check" = "xyes"])

# Check for Memory Check File
AC_ARG_WITH([memory-check-file],
            [AS_HELP_STRING([--with-memory-check-file=filename], [Enable Memory Check with given file])],
            [MEMORY_CHECK_FILE=$withval],[MEMORY_CHECK_FILE=no])
AC_SUBST([MEMORY_CHECK_FILE])
AM_CONDITIONAL([WITH_MEMORY_CHECK_FILE], [test "x$MEMORY_CHECK_FILE" != "xno"])

# Generate the output files
AC_CONFIG_FILES([Makefile
    src/Makefile
    examples/Makefile
    examples/ac-io/Makefile
    examples/ac-io/read-file-into-buffer/Makefile
    examples/ac-io/read-line-by-line/Makefile
])
AC_OUTPUT
