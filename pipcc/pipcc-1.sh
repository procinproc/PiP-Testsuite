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

tstp=$0;
dir=`dirname $0`;
. ${dir}/../config.sh
. $dir/../exit_code.sh.inc;

rm -f root task

${PIPCC} ${CPPFLAGS} --piproot root.c -o root;
if [ $? -ne 0 ]; then
    echo "${pipcc} --piproot root.c -o root -- FAIL";
    exit $EXIT_FAIL;
else
    echo "${PIPCC}pipcc --piproot root.c -o root -- SUCCESS";
fi

${PIP_CHECK} -r root;
if [ $? -ne 0 ]; then
    echo "${PIP_CHECK} -r root -- FAIL"
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} -r root -- SUCCESS"
fi

${PIP_CHECK} -t root
if [ $? -eq 0 ]; then
    echo "${PIP_CHECK} -t root -- FAIL"
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} -t root -- SUCCESS"
fi

${PIPCC} ${CPPFLAGS} --piptask task.c -o task;
if [ $? -ne 0 ]; then
    echo '${PIPCC} --piptask task.c -o task -- FAIL';
    exit $EXIT_FAIL;
else
    echo '${PIPCC} --piptask task.c -o task -- SUCCESS';
fi

${PIP_CHECK} -t task
if [ $? -ne 0 ]; then
    echo "${PIP_CHECK} -t task -- FAIL";
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} -t task -- SUCCESS";
fi

../utils/timer 1 ./root
if [ $? -ne 0 ]; then
    echo 'timer 1 ./root -- FAIL';
    exit $EXIT_FAIL;
else
    echo 'timer 1 ./root -- SUCCESS';
fi

echo $tstp ": PASS";
exit $EXIT_PASS;
