#!/bin/sh

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
