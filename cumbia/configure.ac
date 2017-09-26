#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([cumbia], [0.1.0], [giacomo.strangolino@elettra.trieste.it])
AC_CONFIG_SRCDIR([src/lib/cumbia.h])


# If you want to use libtool’s versioning system, then you must specify
# the version information to libtool using the -version-info flag during link mode
# This flag accepts an argument of the form ‘current[:revision[:age]]’. So,
# passing -version-info 3:12:1 sets current to 3, revision to 12, and age to 1.
# If either revision or age are omitted, they default to 0. Also note that age must
# be less than or equal to the current interface number.
# If the library source code has changed at all since the last update, then increment
# revision (‘c:r:a’ becomes ‘c:r+1:a’).

# Here are a set of rules to help you update your library version information:

AC_SUBST([CUMBIA_VERSION], [0:1:0])

##
## Actually, we use revision in src/Makefile.am
##
AC_SUBST([CUMBIA_VERSION], [0.1.0])

# AC_CONFIG_HEADERS([config.h])

AC_ENABLE_SHARED

LT_INIT

AM_INIT_AUTOMAKE(foreign subdir-objects)

# AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_MACRO_DIR([m4])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB # rendered obsolete by LT_INIT


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [main])

AC_CHECK_LIB([pthread], [pthread_create])

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stdlib.h string.h strings.h sys/time.h unistd.h math.h])

# Checks for typedefs, structures, and compiler characteristics.
# AC_CHECK_HEADER_STDBOOL

# AX_CXX_COMPILE_STDCXX_11([],[mandatory])

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday localtime_r memset select strcasecmp strchr strerror strstr strtol strtoul])


DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)

## Note:
## doc is the output that has been setup in Doxyfile.in ("OUTPUT_DIRECTORY")
##
DX_INIT_DOXYGEN([$PACKAGE_NAME],[Doxyfile.in],[doc])


AC_DISABLE_STATIC

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/cumbia.pc])

AC_DEFINE(CUMBIA_PRINTINFO, 1)

AX_CXX_COMPILE_STDCXX_11([ext], [mandatory])

AC_OUTPUT




