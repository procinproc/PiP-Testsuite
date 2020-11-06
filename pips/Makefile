# $PIP_TEST_VERSION: Version 3.0.0$
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

PROGRAMS = a b c

SRCS = a.c

include $(top_srcdir)/build/rule.mk

all: test

check: test

a: a.c Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) a.c -o a

b: a.c Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) a.c -o b

c: a.c Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) a.c -o c

test: clean
	./pips-test.sh

gen: $(PROGRAMS)
	. ../config.sh; \
	PIP_MODE=thread      $(PIP_EXEC) -n 4 ./a 30 & \
	PIP_MODE=process:got $(PIP_EXEC) -n 3 ./b 30 & \
	LD_PRELOAD=$${PIP_PRELOAD} PIP_MODE=process:preload $(PIP_EXEC) -n 2 ./c 30 &
	$(RM) *.input *.output
	./pips-test.sh gen
	${PIPS} -k a b c pip-exec

post-clean-hook:
	$(RM) $(PROGRAMS)
	$(RM) -f *.check *.log

post-distclean-hook: post-clean-hook
	$(RM) *.input *.output
