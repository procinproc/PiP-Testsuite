
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

SRCS_V1  = malloc.c ctype.c gettimeofday.c \
	   gethostname.c gethostbyname.c \
	   const_destruct.c system.c get_nprocs.c environ.c \
	   atexit.c on_exit.c
PROG_V1  = malloc ctype gettimeofday \
	   gethostname gethostbyname \
	   const_destruct system get_nprocs environ \
	   atexit on_exit

SRCS_V2  = $(SRCS_V1) fork.c sigchld.c pagefault.c pip_dlopen.c libnull.c \
	getaddrinfo.c malloc-x.c realloc-x.c memalign-x.c getpwnam.c
PROG_V2  = $(PROG_V1) fork sigchld pagefault pip_dlopen libnull.so \
	getaddrinfo malloc-x realloc-x memalign-x getpwnam

SRCS_V3 = $(SRCS_V2)
PROG_V3 = $(PROG_V2)

PROG_ALL = $(PROG_V3)

SRCS     = $(SRCS_V$(PIP_VERSION_MAJOR))
PROGRAMS = $(PROG_V$(PIP_VERSION_MAJOR))

include $(top_srcdir)/build/rule.mk

all: $(PROGRAMS)

libnull.so: libnull.c Makefile
	$(PIPCC) --nopip -g -O3 -fPIC -shared libnull.c -o libnull.so

TEST_LIST = test-v$(PIP_VERSION_MAJOR).list

test: all
	../test.sh $(TEST_LIST)

post-clean-hook:
	$(RM) $(PROG_ALL)
