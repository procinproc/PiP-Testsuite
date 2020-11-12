# $PIP_TEST_VERSION: Version 1.0.0$
# $PIP_license: <Simplified BSD License>
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation
# are those of the authors and should not be interpreted as representing
# official policies, either expressed or implied, of the PiP project.$
# $RIKEN_copyright: Riken Center for Computational Sceience,
# System Software Development Team, 2016, 2017, 2018, 2019, 2020$

top_builddir = ..
top_srcdir = $(top_builddir)
srcdir = .

include $(top_srcdir)/build/var.mk

SRCS_V1  = pip_init.c \
	   pip_wait.c \
	   pip_exit.c \
	   exit.c \
	   return.c \
	   pip_spawn.c \
	   pip_misc.c

PROG_V1 = pip_init \
	  pip_wait \
	  pip_exit \
	  exit \
	  return \
	  pip_spawn \
	  pip_misc

SRCS_V2 = $(SRCS_V1) \
	  pip_wait_any.c \
	  pip_trywait.c \
	  pip_trywait_any.c \
	  pip_fin.c \
	  pip_task_spawn.c \
	  corebind.c \
	  spawn_pie.c \
	  hook_and_coe.c \
	  pip_barrier.c \
	  pip_barrier2.c \
	  seek.c \
	  pip_kill.c \
	  pip_namexp.c \
	  pip_aux.c

PROG_V2 = $(PROG_V1) \
	  pip_wait_any \
	  pip_trywait \
	  pip_trywait_any \
	  pip_fin \
	  pip_task_spawn \
	  corebind \
	  spawn_pie \
	  hook_and_coe \
	  pip_barrier \
	  pip_barrier2 \
	  seek \
	  pip_kill \
	  pip_namexp \
	  pip_aux

SRCS_V3 = $(SRCS_V2)
PROG_V3 = $(PROG_V2)

PROG_TEST = prog-noexec \
	    prog-nopie \
	    prog-pie \
	    prog-nordynamic

PROG_ALL = $(PROG_V3) $(PROG_TEST)

SRCS     = $(SRCS_V$(PIP_VERSION_MAJOR))
PROGRAMS = $(PROG_V$(PIP_VERSION_MAJOR)) $(PROG_TEST)

include $(top_srcdir)/build/rule.mk

all: $(PROGRAMS)

prog-noexec : prog.c $(DEPS) Makefile
	$(CC) -g -O2 prog.c -o prog-noexec
	chmod -x prog-noexec

prog-nopie : prog.c $(DEPS) Makefile
	$(CC) -g -O2 prog.c -o prog-nopie

prog-pie : prog.c $(DEPS) Makefile
	$(CC) -g -O2 -fpie -pie -rdynamic prog.c -o prog-pie

prog-nordynamic : prog.c Makefile
	$(CC) -g -O2 -fpie -pie prog.c -o prog-nordynamic

debug :: $(DEPS)
	$(MAKE) -C ../basics-common debug

TEST_LIST = test-v$(PIP_VERSION_MAJOR).list

test :
	../test.sh $(TEST_LIST)

post-clean-hook:
	$(RM) $(PROG_ALL)