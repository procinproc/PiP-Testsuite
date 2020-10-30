#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([PiP-Testsuite], [1.0.0], [pip@ml.riken.jp], [pip-testsuite],
	[https://github.com/RIKEN-SysSoft/PiP-Testsuite])
AC_CONFIG_SRCDIR([RELEASE_NOTE])
AC_CONFIG_AUX_DIR([build])

# Specialized system macros
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

case "$target_os" in
     linux*) true;;
     *) AC_MSG_ERROR([Unsupported OS]);;
esac

case "$target" in
      x86_64-*)  true;;
      aarch64-*) true;;
      *) AC_MSG_ERROR([Unsupported target CPU architecture]);;
esac;

# srcdir_top

cwd=`dirname $0`;
srcdir_top=`realpath $cwd`;
AC_SUBST(srcdir_top)

# Checks for command line options

AC_MSG_CHECKING([for pip installation])
AC_ARG_WITH([pip],
  [AS_HELP_STRING([--with-pip=DIR], [PiP installed directory])])

case "${with_pip}" in
""|yes|no)   AC_MSG_ERROR([--with-pip is a mandatory option]);;
esac
if test -d "${with_pip}/include" \
   -a -f "${with_pip}/include/pip_config.h" -a -f "${with_pip}/include/pip.h" \
   -a -d "${with_pip}/lib" -a -x "${with_pip}/lib/libpip.so" \
   -a -d "${with_pip}/bin" -a -x "${with_pip}/bin/pipcc"; then
   pver_str=`grep PACKAGE_VERSION ${with_pip}/include/pip_config.h 2> /dev/null`
   if test -z "${pver_str}"; then
       AC_MSG_ERROR([pip_config.h ???])
   fi
   pip_version=`echo ${pver_str} | cut -d ' ' -f 3 | cut -d '"' -f 2`
   pip_version_major=`echo ${pip_version} | cut -d '.' -f 1`
   pip_version_minor=`echo ${pip_version} | cut -d '.' -f 2`
   pip_version_subminor=`echo ${pip_version} | cut -d '.' -f 3`
else
  AC_MSG_ERROR([--with-pip does not seem to be a PiP directory])
fi

AC_MSG_RESULT([${with_pip}])
AC_MSG_RESULT([PIP_VERSION_MAJOR=${pip_version_major}])

AC_SUBST(with_pip)
AC_SUBST(pip_version)
AC_SUBST(pip_version_major)
AC_SUBST(pip_version_minor)
AC_SUBST(pip_version_subminor)

# Checks for programs.
AC_PROG_CC()
AC_PROG_CXX()
AC_PROG_F77()

if test x"$GCC" = x"yes"; then
  CFLAGS="$CFLAGS -Wall"
fi

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
	release/version.conf
	build/config.mk
	config.sh
])
AC_OUTPUT