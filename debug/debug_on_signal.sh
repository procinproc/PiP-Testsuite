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

tst=$1

dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

SIGHUP=1
SIGSEGV=11
SIGPIPE=13

if [ x"$tst" = x ]; then
    echo "No test number"
    exit ${EXIT_UNTESTED}
fi

pipmode=`${PIP_MODE_CMD}`
case ${pipmode} in
    pthread) exit ${EXIT_SUPPORETD};;
esac

if [ -x ${PIP_GDB_PATH} ]; then
    PIPGDB=${PIP_GDB_PATH}
elif [ -x ${PIP_BINDIR}/pip-gdb ]; then
    PIPGDB=${PIP_BINDIR}/bin/pip-gdb
fi

if ! [ -x ${PIPGDB} ]; then
    echo "Unable to find PiP-gdb"
    exit ${EXIT_UNTESTED}
fi
export PIP_GDB_PATH=${PIPGDB} 

testprog=./xy

case $tst in
    0) {PIP_EXEC} -n 4 $testprog;
	extval=$?;;
    1) env PIP_GDB_COMMAND=./gdb_cmd ${PIP_EXEC} -n 4 $testprog;
	extval=$?;;
    2) env PIP_GDB_SIGNALS=HUP ${PIP_EXEC} -n 4 $testprog -1 $SIGHUP;
	extval=$?;;
    3) env PIP_GDB_SIGNALS=HUP+PIPE ${PIP_EXEC} -n 4 $testprog -1 $SIGPIPE;
	extval=$?;;
    4) env PIP_GDB_SIGNALS=ALL-PIPE ${PIP_EXEC} -n 4 $testprog -1 $SIGPIPE;
	extval=$?;;
    5) env PIP_SHOW_PIPS=on ${PIP_EXEC} -n 4 $testprog;
	extval=$?;;
    6) env PIP_SHOW_MAPS=on ${PIP_EXEC} -n 4 $testprog;
	extval=$?;;
    7) env PIP_SHOW_PIPS=on PIP_SHOW_MAPS=on ${PIP_EXEC} -n 4 $testprog;
	extval=$?;;
    *) echo "Unknown test No."; exit ${EXIT_FAIL};;
esac

if [ $extval == 0 ]; then
    exit ${EXIT_FAIL}
fi
exit ${EXIT_PASS}
