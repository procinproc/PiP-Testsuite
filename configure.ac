#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([PiP-Testsuite], 
	[1.1.0], 
	[procinproc-info@googlegroups.com], 
	[pip-testsuite],
	[https://github.com/procinproc/PiP-Testsuite])
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
  [AS_HELP_STRING([--with-pip=PIPDIR], [PiP installed directory])])

case "${with_pip}" in
""|yes|no)   AC_MSG_ERROR([--with-pip is a mandatory option]);;
esac
if test -d "${with_pip}/include" \
   -a '(' -d "${with_pip}/lib64" -a -x "${with_pip}/lib64/libpip.so" -o \
          -d "${with_pip}/lib"   -a -x "${with_pip}/lib/libpip.so" ')' \
   -a -d "${with_pip}/bin" -a -x "${with_pip}/bin/pipcc"; then
   if test -f "${with_pip}/include/pip.h"; then
       pipincdir="${with_pip}/include"
   elif test -f "${with_pip}/include/pip/pip.h"; then
       pipincdir="${with_pip}/include/pip"
   else
       AC_MSG_ERROR([--with-pip doesn't seem to be a PiP directory])
   fi
   pver_str=`grep PACKAGE_VERSION ${pipincdir}/pip_config.h 2> /dev/null`
   if test -z "${pver_str}"; then
       AC_MSG_ERROR([pip_config.h ???])
   fi
   pip_version=`echo ${pver_str} | cut -d ' ' -f 3 | cut -d '"' -f 2`
   pip_version_major=`echo ${pip_version} | cut -d '.' -f 1`
   pip_version_minor=`echo ${pip_version} | cut -d '.' -f 2`
   pip_version_subminor=`echo ${pip_version} | cut -d '.' -f 3`
   if test -d "${with_pip}/lib64" -a -x "${with_pip}/lib64/libpip.so"; then
     PIP_LIBDIR="${with_pip}/lib64"
   elif test -d "${with_pip}/lib" -a -x "${with_pip}/lib/libpip.so"; then
     PIP_LIBDIR="${with_pip}/lib"
   else
     AC_MSG_ERROR([cannot determine PIP_LIBDIR])
   fi
else
   AC_MSG_ERROR([--with-pip doesn't seem to be a PiP directory])
fi
AC_MSG_RESULT([${with_pip}])
AC_MSG_RESULT([PIP_VERSION_MAJOR=${pip_version_major}])

AC_ARG_WITH([pipgdb],
  [AS_HELP_STRING([--with-pipgdb=PIPGDB_PATH], [Path to pip-gdb])])
case "${with_pipgdb}" in
""|yes|no)   
	if test -x "${with_pip}/bin/pip-gdb"; then
	   with_pipgdb=${with_pip}/bin/pip-gdb
        fi;;
*)	if ! test -x "${with_pipgdb}"; then
  	   AC_MSG_ERROR([--with-pipgdb doesn't seem to be a path to pip-gdb]);
	fi;;
esac
AC_MSG_RESULT([${with_pipgdb}])

AC_SUBST(with_pip)
AC_SUBST(pip_version)
AC_SUBST(pip_version_major)
AC_SUBST(pip_version_minor)
AC_SUBST(pip_version_subminor)
AC_SUBST(PIP_LIBDIR)
AC_SUBST(with_pipgdb)

# Checks for programs.
AC_PROG_CC()
AC_PROG_CXX()

if test x"$GCC" = x"yes"; then
  CFLAGS="$CFLAGS -Wall"
fi

AC_CHECK_PROGS([FC],[gfortran g95 g77])

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
