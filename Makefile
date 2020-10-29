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

top_builddir = .
top_srcdir = $(top_builddir)
srcdir = .

include $(top_srcdir)/build/var.mk

SUBDIRS_COMMON = bin pipcc util-common basics-common pthread openmp cxx fortran issues

SUBDIRS_V1 = $(SUBDIRS_COMMON) \
	prog-v1 \
	basics-v1 \
	compat-v1
SUBDIRS_V2 = $(SUBDIRS_COMMON) \
	prog-v2 \
	basics-v1 basics-v2 \
	compat-v1 compat-v2
SUBDIRS_V3 = $(SUBDIRS_COMMON) \
	prog-v2 \
	util-v3 \
	basics-v1 basics-v2 \
	compat-v1 compat-v2 \
	blt-v3

SUBDIRS_CLEAN = $(SIBDIRS_COMMON) \
	basics-v1 basics-v2 blt-v3 build \
	compat-v1 compat-v2 cxx fortran include  \
	prog-v1 prog-v2 pthread release util-v3 \
	pipgdb porting

include $(top_srcdir)/build/rule.mk

subdir-all subdir-debug subdir-testclean subdir-veryclean:
	@target=`expr $@ : 'subdir-\(.*\)'`; \
	. $(top_srcdir)/config.sh; \
	echo "version ${PIP_VERSION_MAJOR}"; \
	case ${PIP_VERSION_MAJOR} in \
		1) SUBDIRS="$(SUBDIRS_V1)" ;; \
		2) SUBDIRS="$(SUBDIRS_V2)" ;; \
		3) SUBDIRS="$(SUBDIRS_V3)" ;; \
	esac; \
	for dir in -- $${SUBDIRS}; do \
		case $${dir} in --) continue;; esac; \
		echo '[' making $${dir} ']'; \
		make -C $${dir} $${target}; \
	done

subdir-clean:
	@for dir in $(SUBDIRS_CLEAN); do \
		echo '[' making $${dir} ']'; \
		make -C $${dir} clean; \
	done

all: subdir-all
	echo ${PIP_VERSION} > .build_version

clean: subdir-clean
	$(RM) .build_version

testclean: subdir-testclean

post-veryclean-hook:
	$(RM) config.log config.status .build_version
	$(RM) -r autom4te.cache

debug::
	$(MAKE) subdir-debug;

cdebug:: clean debug

TEST_LIST = test-v$(PIP_VERSION_MAJOR).list

test: all
	./test.sh $(TEST_LIST)
