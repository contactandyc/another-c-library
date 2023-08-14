AC_INIT([anotherclibrary], [1.0], [contactandyc@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC
LT_INIT

# Check for necessary libraries, functions, etc.
# AC_CHECK_LIB([m], [sqrt])
# AC_CHECK_FUNCS([memset])

# Optional checks for optional features
AX_CHECK_ENABLE_DEBUG([yes])

# Generate the output files
AC_CONFIG_FILES([Makefile ])
AC_OUTPUT
