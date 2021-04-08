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

#set -x

tst=$1

cmd=`basename $0`
dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

pipmode=`${PIP_MODE_CMD}`
case ${pipmode} in
    pthread) exit ${EXIT_UNSUPPORTED};;
esac

SIGHUP=1
SIGSEGV=11
SIGPIPE=13

if [ x"$tst" = x ]; then
    echo "$cmd: No test number"
    exit ${EXIT_UNTESTED}
fi

pipmode=`${PIP_MODE_CMD}`
case ${pipmode} in
    pthread) exit ${EXIT_SUPPORETD};;
esac

. ./check_gdb.sh.inc

testprog=./xy

unset PIP_GDB_COMMAND PIP_GDB_SIGNALS PIP_SHOW_MAPS PIP_SHOW_PIPS

echo_and_do () {
    echo $@
    $@
}

case $tst in
    0) echo_and_do ${PIP_EXEC} -n 4 $testprog;
	extval=$?;;
    1) echo_and_do env PIP_GDB_COMMAND=./pip-gdb-sig.xcmd ${PIP_EXEC} -n 4 $testprog 0;
	extval=$?;;
    2) echo_and_do env PIP_GDB_SIGNALS=HUP ${PIP_EXEC} -n 4 $testprog 1 $SIGHUP;
	extval=$?;;
    3) echo_and_do env PIP_GDB_SIGNALS=HUP+PIPE ${PIP_EXEC} -n 4 $testprog 2 $SIGPIPE;
	extval=$?;;
    4) echo_and_do env PIP_GDB_SIGNALS=ALL-PIPE ${PIP_EXEC} -n 4 $testprog 3 $SIGPIPE;
	extval=$?;;
    5) echo_and_do env PIP_SHOW_PIPS=on ${PIP_EXEC} -n 4 $testprog 0;
	extval=$?;;
    6) echo_and_do env PIP_SHOW_MAPS=on ${PIP_EXEC} -n 4 $testprog 1;
	extval=$?;;
    7) echo_and_do env PIP_SHOW_PIPS=on PIP_SHOW_MAPS=on ${PIP_EXEC} -n 4 $testprog 2;
	extval=$?;;
    *) echo "$cmd: Unknown test No. $tst"; exit ${EXIT_FAIL};;
esac

if [ $extval -eq 0 ]; then
    exit ${EXIT_FAIL}
fi
exit ${EXIT_PASS}
