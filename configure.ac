AC_INIT(gpup, 0.3.0, g_85_g@yahoo.com)

AM_INIT_AUTOMAKE([1.10.0 foreign subdir-objects dist-bzip2])
AM_CONFIG_HEADER(src/config.h)
AC_CONFIG_MACRO_DIR([m4])

# Add configure switches.
AC_ARG_ENABLE(debug, [  --enable-debug          Enable debugging [default=no]], gpup_debug=1, gpup_debug=0 )
if test $gpup_debug -eq 1
then
    CXXFLAGS="-Wall -g -O0 $CXXFLAGS"
else
    CXXFLAGS="-Wall -O3 $CXXFLAGS"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries
PKG_CHECK_MODULES([LIBXMLPP], [libxml++-2.6 >= 2.14.0])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_BIGENDIAN
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([memset strtol strtoul])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/common/Makefile
                 src/core/Makefile])
AC_OUTPUT
