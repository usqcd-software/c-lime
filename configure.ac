AC_PREREQ(2.53)
AC_INIT(lime, 1.1, detar@physics.utah.edu)
AC_CONFIG_AUX_DIR(config)

AC_CONFIG_SRCDIR([include/lime_header.h])
AC_CONFIG_HEADER([include/lime_config_internal.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
## AC_C_CONST
## AC_TYPE_SIZE_T

# Checks for library functions.
## AC_FUNC_MALLOC

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(include/Makefile)
AC_CONFIG_FILES(lib/Makefile)
AC_CONFIG_FILES(examples/Makefile)
AC_CONFIG_FILES(doc/Makefile)

AC_OUTPUT
