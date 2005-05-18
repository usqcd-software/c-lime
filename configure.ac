AC_PREREQ(2.53)
AC_INIT(lime, 1.2.1, detar@physics.utah.edu)
AC_CONFIG_AUX_DIR(config)

AC_CONFIG_SRCDIR([include/lime_header.h])
AC_CONFIG_HEADER([include/lime_config_internal.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for type configuration

AC_CHECK_HEADERS([stdint.h],
[ dnl for inttypes.h and stdint.h for uint_xxx types
  dnl if successful check for the actual types too
  AC_CHECK_TYPES([uint16_t, uint32_t, uint64_t],
                 [],
                 [AC_MSG_ERROR([stdint.h found but either uint16_t, uint32_t or uint64_t not found]) ]
                )
],
[ 
  dnl no inttypes.h or stdint.h found check common unsigned types
  dnl for sizes and make appropriate decisions in the lime_fixed_types.h file
  AC_CHECK_SIZEOF(unsigned char)
  AC_CHECK_SIZEOF(unsigned short)
  AC_CHECK_SIZEOF(unsigned int)
  AC_CHECK_SIZEOF(unsigned long)
  AC_CHECK_SIZEOF(unsigned long long)
]
)

# Check for the fseeko function
# Note that currently this just checks the existence of the function
# rather than dealing with the whole issue of large file support.
AC_FUNC_FSEEKO


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
