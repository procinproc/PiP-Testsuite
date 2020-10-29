#!/bin/sh

tstp=$0
dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

${PIPCC} --nopip a.c -o nopip
if ${PIP_CHECK} nopip; then
    echo "${PIP_CHECK} nopip -- FAILS"
    rm -f nopip
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} nopip -- SUCCESS"
fi
rm -f nopip

echo ${PIPCC} --piproot a.c -o piproot
${PIPCC} --piproot a.c -o piproot
if ! ${PIP_CHECK} --piproot piproot; then
    echo "! ${PIP_CHECK} --piproot piproot -- FAILS"
    rm -f piproot
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piproot piproot -- SUCCESS"
fi
if ${PIP_CHECK} --piptask piproot; then
    echo "${PIP_CHECK} --piptask piproot -- FAILS"
    rm -f piproot
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} --piptask piproot -- SUCCESS"
fi
rm -f piproot

${PIPCC} --piptask a.c -o piptask
if ! ${PIP_CHECK} --piptask piptask; then
    echo "! ${PIP_CHECK} --piptask piptask -- FAILS"
    rm -f piptask
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piptask piptask -- SUCCESS"
fi
##if ${PIP_CHECK} --piproot piptask; then
##    echo "${PIP_CHECK} --piproot piptask -- FAILS"
##    rm -f piptask
##    exit $EXIT_FAIL;
##else
##    echo "${PIP_CHECK} --piproot piptask -- SUCCESS"
##fi
rm -f piptask

${PIPCC} a.c -o pipboth
if ! ${PIP_CHECK} --both pipboth; then
    echo "! ${PIP_CHECK} --both pipboth -- FAILS"
    rm -f pipboth
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --both pipboth -- SUCCESS"
fi
if ! ${PIP_CHECK} --piproot pipboth; then
    echo "! ${PIP_CHECK} --piproot pipboth -- FAILS"
    rm -f pipboth
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piproot pipboth -- SUCCESS"
fi
if ! ${PIP_CHECK} --piptask pipboth; then
    echo "! ${PIP_CHECK} --piptask pipboth -- FAILS"
    rm -f pipboth
    exit $EXIT_FAIL;
else
    echo "! ${PIP_CHECK} --piptask pipboth -- SUCCESS"
fi
rm -f pipboth

echo $tstp ": PASS";
exit $EXIT_PASS;
