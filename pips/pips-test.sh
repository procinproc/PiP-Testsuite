#!/bin/sh

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

. ../config.sh

pips_log="pips-test"
flag_gen=false
flag_error=false

echo -n > $pips_log

check() {
    flag=$1
    retv=$2
    file=$3
    if [ $retv == 0 ]; then
	if $flag; then
	    if [ x"$file" != x ]; then
		if ! diff $file.check $file.output >> $pips_log 2>&1; then
		    flag_error=true
		    echo " >>> DIFF ($file)"
		    echo "<<<< DIFF ($file)" >> $pips_log
		    return
		fi
	    fi
	    echo " >>> OK"
	    echo "<<<< OK" >> $pips_log
	else
	    flag_error=true
	    echo " >>> NG"
	    echo "<<<< NG" >> $pips_log
	fi
    else
	if $flag; then
	    flag_error=true
	    echo " >>> NG"
	    echo "<<<< NG" >> $pips_log
	else
	    if [ x"$file" != x ]; then
		if ! diff $file.check $file.output >> $pips_log 2>&1; then
		    flag_error=true
		    echo " >>> DIFF ($file)"
		    echo "<<<< DIFF ($file)" >> $pips_log
		    return
		fi
	    fi
	    echo " >>> OK"
	    echo "<<<< OK" >> $pips_log
	fi
    fi
}

do_test() {
    flag=$1
    shift
    pips_file=$1
    shift
    unset PIP_CHECK_GEN
    unset PIP_CHECK_OUT
    if $flag_gen; then
	echo -n "pips $@"
	echo "pips $@" >> $pips_log
	PIPS_CHECK_GEN=${pips_file} ${PIPS} $@ >> $pips_log 2>&1
	retval=$?
	check $flag $retval
	if ! [ -f ${pips_file}.input ]; then
	    flag_error=true
	    echo "${pips_file}.input is missing"
	    echo "${pips_file}.input is missing" >> $pips_log
	fi
	if ! [ -f ${pips_file}.output ]; then
	    flag_error=true
	    echo "${pips_file}.output is missing"
	    echo "${pips_file}.output is missing" >> $pips_log
	fi
    else
	echo -n "pips $@"
	echo ">>>> pips $@" >> $pips_log
	nofile=false
	if ! [ -f ${pips_file}.input ]; then
	    flag_error=true
	    nofile=true
	    echo "${pips_file}.input is missing"
	    echo "${pips_file}.input is missing" >> $pips_log
	fi
	if ! [ -f ${pips_file}.output ]; then
	    flag_error=true
	    nofile=true
	    echo "${pips_file}.output is missing"
	    echo "${pips_file}.output is missing" >> $pips_log
	fi
	if ! $nofile; then
	    PIPS_CHECK_OUT=${pips_file} ${PIPS} $@ >> $pips_log 2>&1
	    extval=$?
	    if ! [ -f ${pips_file}.check ]; then
		flag_error=true
		echo "${pips_file}.check is missing"
		echo "${pips_file}.check is missing" >> $pips_log
	    else
		check $flag $extval ${pips_file}
	    fi
	fi
    fi
}

if [ x"$1" == x ]; then
    pips_log=${pips_log}-chk.log
    echo "CHECKING ..."
else
    flag_gen=true
    pips_log=${pips_log}-gen.log
    echo "GENERATING ..."
fi

do_test true  pips-noop
do_test true  pips-noop-  -
do_test true  pips-noop-a - ~a~
do_test true  pips-noop--  --
do_test true  pips-noop--b - ~b~

do_test true  pips-a    a ~a~
do_test true  pips-u    u
do_test true  pips-x    x
do_test false pips-x-nosuch x nosuchpip
do_test true  pips-au   au ~a~ ~b~
do_test true  pips-ax   ax
do_test true  pips-ua   ua
do_test true  pips-ux   ux
do_test true  pips-xa   xa
do_test true  pips-xu   xu
do_test true  pips-aux  aux ~a~ ~b~ ~c~ ~d~

do_test true  pips-root au --root
do_test true  pips-r       -r
do_test true  pips-r-exec  -r pip-exec
do_test false pips-r-a     -r ~a~
do_test true  pips-task    --task
do_test true  pips-au-t    au -t
do_test false pips-t-exec  -t pip-exec

do_test true  pips-family   --family
do_test true  pips-f        -f
do_test true  pips-family-a --family ~a~
do_test true  pips-f-a-c    -f ~a~ ~c~
do_test false pips-f-nosuch -f nosuchpip

do_test true  pips-test-kill      --kill
do_test true  pips-task-k        --task -k
do_test true  pips-task-k-a      --task -k ~a~
do_test false pips-signal        --signal
do_test false pips-r-s           -r -s
do_test true  pips-s-hup         -s hup
do_test true  pips-signal-sighup --signal SIGhup
do_test true  pips-s-2-verbose   -s 2 --verbose
do_test true  pips-t-s-2-v       -t -s 2 -v
do_test false pips-signal-nosuch -signal nosuchsig
do_test false pips-s-nosuch      -s nosuchsig

do_test true  pips-ps        --ps
do_test false pips-ps-nosuch --ps - nosuchpip
do_test true  pips-P         -P
do_test true  pips-ps-a      --ps -- ~a~
do_test true  pips-P-a-c     -P - ~a~ ~c~
do_test true  pips-ps-l      --ps l
do_test true  pips-P-l       -P l
do_test true  pips-ps-l-a    --ps l - ~a~
do_test true  pips-P-l-b-c   -P l -- ~b~ ~c~

if $flag_error; then
    echo "FAILED"
    exit 1
fi
echo "succeeded"
exit 0
