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

CWD = $(shell pwd)
CPPFLAGS += -I../include -DCWD=\"$(CWD)\" -DPIPS=\"$(PIPS)\"

SRCS_V1 = dlmopen_count.c ompnumthread.c timer.c inf.c \
	pip_mode_check.c pip_task.c
SRCS_V2 = $(SRCS_V1)
SRCS_V3 = $(SRCS_V2) pip_blt.c

PROG_V1 = dlmopen_count ompnumthread timer inf pip_mode_check pip_task
PROG_V2 = $(PROG_V1)
PROG_V3 = $(PROG_V2) pip_blt

PROG_ALL = ($PROG_V3)

SRCS = $(SRCS_V$(PIP_VERSION_MAJOR))
PROGRAMS = $(PROG_V$(PIP_VERSION_MAJOR))

include $(top_srcdir)/build/rule.mk

all: $(PROGRAMS)

dlmopen_count: dlmopen_count.c $(DEPS)
	$(PIPCC) -g -O2 $(CPPFLAGS)  $< -o $@

ompnumthread: ompnumthread.c $(DEPS)
	$(CC) -fopenmp -openmp $< -o $@

timer: timer.c $(DEPS)
	$(PIPCC) -g -O2 $(CPPFLAGS) $< -o $@

pip_mode_check : pip_mode_check.c $(DEPS)
	$(PIPCC) -g -O2 $(CPPFLAGS) --piptask $< -o $@

pip_task : pip_task.c $(DEPS)
	$(PIPCC) -g -O2 $(CPPFLAGS) --piproot $< -o $@

inf : inf.c $(DEPS)
	$(CC) -g -O2 $(CPPFLAGS) $< -o $@

TEST_LIST = test-v$(PIP_VERSION_MAJOR).list

test :
	../test.sh $(TEST_LIST)

post-clean-hook:
	$(RM) $(PROG_ALL)
