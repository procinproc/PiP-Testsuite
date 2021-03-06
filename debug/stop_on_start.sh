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

dir=`dirname $0`
cmd=`basename $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

ONSTART=$1
flag_pipgdb=$2
command=$3

pipmode=`${PIP_MODE_CMD}`
case ${pipmode} in
    pthread) exit ${EXIT_UNSUPPORTED};;
esac

if ! type -P $command; then
    exit ${EXIT_UNTESTED}
fi

flag_ptrace=false
case $command in
    pip-gdb|ltrace|strace|perf) flag_ptrace=true
esac

if $flag_pipgdb || $flag_ptrace; then
    if ! ${dir}/../utils/check_ptrace; then
	exit ${EXIT_UNTESTED}
    fi
fi
if $flag_pipgdb; then
    . ./check_gdb.sh.inc
fi

testprog=./xx

set -x
set -e

echo PIP_STOP_ON_START=$ONSTART ${PIP_EXEC} -n 4 $testprog 
PIP_STOP_ON_START=$ONSTART ${PIP_EXEC} -n 4 $testprog &
wait %1
extval=$?
if [ $extval -gt 0 ]; then
    exit ${EXIT_FAIL}
fi
exit $EXIT_PASS
