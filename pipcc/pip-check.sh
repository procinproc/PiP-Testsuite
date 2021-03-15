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

tstp=$0
dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

rm -f nopip piproot piptask pipboth

${PIPCC} --nopip a.c -o nopip
if ${PIP_CHECK} nopip; then
    echo "${PIP_CHECK} nopip -- FAILS"
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} nopip -- SUCCESS"
fi

echo ${PIPCC} --piproot a.c -o piproot
${PIPCC} --piproot a.c -o piproot
if ! ${PIP_CHECK} --piproot piproot; then
    echo "! ${PIP_CHECK} --piproot piproot -- FAILS"
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piproot piproot -- SUCCESS"
fi
if ${PIP_CHECK} --piptask piproot; then
    echo "${PIP_CHECK} --piptask piproot -- FAILS"
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} --piptask piproot -- SUCCESS"
fi

${PIPCC} --piptask a.c -o piptask
if ! ${PIP_CHECK} --piptask piptask; then
    echo "! ${PIP_CHECK} --piptask piptask -- FAILS"
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piptask piptask -- SUCCESS"
fi

${PIPCC} a.c -o pipboth
if ! ${PIP_CHECK} --both pipboth; then
    echo "! ${PIP_CHECK} --both pipboth -- FAILS"
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --both pipboth -- SUCCESS"
fi
if ! ${PIP_CHECK} --piproot pipboth; then
    echo "! ${PIP_CHECK} --piproot pipboth -- FAILS"
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piproot pipboth -- SUCCESS"
fi
if ! ${PIP_CHECK} --piptask pipboth; then
    echo "! ${PIP_CHECK} --piptask pipboth -- FAILS"
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piptask pipboth -- SUCCESS"
fi

echo $tstp ": PASS";
exit $EXIT_PASS;
