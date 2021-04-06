
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

top_builddir = .
top_srcdir = $(top_builddir)
srcdir = .

include $(top_srcdir)/build/var.mk

SUBDIRS_COMMON = bin pipcc utils basics pthread openmp cxx issues pips debug

SUBDIRS_V1 = $(SUBDIRS_COMMON) prog compat
SUBDIRS_V2 = $(SUBDIRS_COMMON) fortran prog compat libpip
SUBDIRS_V3 = $(SUBDIRS_COMMON) fortran prog compat blt libpip

SUBDIRS_CLEAN = $(SUBDIRS_COMMON) \
	fortran prog compat blt libpip \
	build include release porting

include $(top_srcdir)/build/rule.mk

subdir-all subdir-debug :
	@target=`expr $@ : 'subdir-\(.*\)'`; \
	. $(top_srcdir)/config.sh; \
	echo "version ${PIP_VERSION_MAJOR}"; \
	case ${PIP_VERSION_MAJOR} in \
		1) SUBDIRS="$(SUBDIRS_V1)" ;; \
		2) SUBDIRS="$(SUBDIRS_V2)" ;; \
		3) SUBDIRS="$(SUBDIRS_V3)" ;; \
	        *) echo "Unsupported PiP version"; exit 1;; \
	esac; \
	for dir in -- $${SUBDIRS}; do \
		case $${dir} in --) continue;; esac; \
		echo '[' making $${dir} ']'; \
		make -C $${dir} $${target}; \
	done

subdir-clean subdir-testclean subdir-veryclean :
	@target=`expr $@ : 'subdir-\(.*\)'`; \
	for dir in $(SUBDIRS_CLEAN); do \
		echo '[' making $${dir} ']'; \
		make -C $${dir} $${target}; \
	done

all: subdir-all
	echo ${PIP_VERSION} > .pip_version

clean: subdir-clean
	$(RM) .pip_version

testclean: subdir-testclean

post-veryclean-hook:
	$(RM) config.log config.status
	$(RM) -r autom4te.cache
	$(RM) config.sh build/config.mk

debug::
	$(MAKE) subdir-debug;

TEST_LIST = test-v$(PIP_VERSION_MAJOR).list

test: all
	./test.sh $(TEST_LIST)

test1: all
	env PIP_TEST_THRESHOLD=1 ./test.sh $(TEST_LIST)

test2: all
	env PIP_TEST_THRESHOLD=2 ./test.sh $(TEST_LIST)

test3: all
	env PIP_TEST_THRESHOLD=3 ./test.sh $(TEST_LIST)

test4: all
	env PIP_TEST_THRESHOLD=4 ./test.sh $(TEST_LIST)

test5: all
	env PIP_TEST_THRESHOLD=5 ./test.sh $(TEST_LIST)

test6: all
	env PIP_TEST_THRESHOLD=6 ./test.sh $(TEST_LIST)

test7: all
	env PIP_TEST_THRESHOLD=7 ./test.sh $(TEST_LIST)

test8: all
	env PIP_TEST_THRESHOLD=8 ./test.sh $(TEST_LIST)

test9: all
	env PIP_TEST_THRESHOLD=9 ./test.sh $(TEST_LIST)

test10: all
	env PIP_TEST_THRESHOLD=10 ./test.sh $(TEST_LIST)
