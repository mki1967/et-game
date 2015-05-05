#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([et-game], [1.04-alpha], [mki1967@gmail.com], [etgame], ['https://github.com/mki1967/et-game'])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src/et-game.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([GL], [glPushMatrix])
AC_CHECK_LIB([X11], [XRaiseWindow])
AC_CHECK_LIB([m], [sqrt])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 data/Makefile])
AC_OUTPUT
