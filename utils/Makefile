
# $PIP_license: <Simplified BSD License>
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#     Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
# $
# $RIKEN_copyright: Riken Center for Computational Sceience (R-CCS),
# System Software Development Team, 2016-2021
# $
# $PIP_TESTSUITE: Version 1.1.0$
#
# $Author: Atsushi Hori (R-CCS) mailto: ahori@riken.jp or ahori@me.com
# $

top_builddir = ..
top_srcdir = $(top_builddir)
srcdir = .

include $(top_srcdir)/build/var.mk
include $(top_srcdir)/build/config.mk

CWD = $(shell pwd)
CPPFLAGS += -I../include -DCWD=\"$(CWD)\" -DPIPS=\"$(PIPS)\"

SRCS_V1 = dlmopen_count.c ompnumthread.c timer.c inf.c \
	pip_task.c check_ptrace.c clone_check.c
SRCS_V2 = $(SRCS_V1)
SRCS_V3 = $(SRCS_V2) pip_blt.c

PROG_V1 = dlmopen_count ompnumthread timer inf pip_mode_check \
	pip_task check_ptrace clone_check

PROG_V2 = $(PROG_V1)
PROG_V3 = $(PROG_V2) pip_blt

PROG_ALL = $(PROG_V3)

SRCS = $(SRCS_V$(PIP_VERSION_MAJOR))
PROGRAMS = $(PROG_V$(PIP_VERSION_MAJOR))

OPTS = -g -O2

include $(top_srcdir)/build/rule.mk

all: $(PROGRAMS)

clone_check: clone_check.c $(DEPS) Makefile
	$(CC) $(CPPFLAGS) -I$(PIP_DIR)/include $< -o $@

dlmopen_count: dlmopen_count.c $(DEPS) Makefile
	$(PIPCC) $(OPTS) $(CPPFLAGS) $< -o $@

ompnumthread: ompnumthread.c $(DEPS) Makefile
	$(CC) $(OPTS) -fopenmp -openmp $< -o $@

timer: timer.c $(DEPS) Makefile
	$(CC) $(OPTS) $(CPPFLAGS) -DPIPLIB=\"$(PIPLIB)\" $< -o $@

pip_mode_check : pip_mode_check.sh
	cp $< $@
	chmod +x $@

pip_task : pip_task.c $(DEPS) Makefile
	$(PIPCC) $(OPTS) $(CPPFLAGS) --piproot $< -o $@

inf : inf.c $(DEPS) Makefile
	$(PIPCC) $(OPTS) $(CPPFLAGS) $< -o $@

TEST_LIST = test-v$(PIP_VERSION_MAJOR).list

test: all
	../test.sh $(TEST_LIST)

post-clean-hook:
	$(RM) $(PROG_ALL)
