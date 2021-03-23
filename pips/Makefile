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

PROGRAMS = ~a~ ~b~ ~c~ ~d~
SRCS = a.c

include $(top_srcdir)/build/rule.mk

all: $(PROGRAMS)

gen: $(PROGRAMS)
	$(RM) *.input *.output
	./pips-test.sh gen

~a~: a.c $(DEPS) Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) a.c -o $@

~b~: a.c $(DEPS) Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) a.c -o $@

~c~: a.c $(DEPS) Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) a.c -o $@

~d~: a.c $(DEPS) Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) a.c -o $@

TEST_LIST = test-v$(PIP_VERSION_MAJOR).list

test: all
	../test.sh $(TEST_LIST)

post-clean-hook:
	$(RM) $(PROGRAMS)
	$(RM) -f *.check *.log

post-distclean-hook: post-clean-hook
	$(RM) *.input *.output
