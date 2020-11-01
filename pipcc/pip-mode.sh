#!/bin/sh

prog=$0
dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

out=`${PIP_MODE_CMD} --${PIP_MODE} ${PRINTPIPMODE}`;
if [ x"$out" != x"${PIP_MODE}" ]; then
    echo "${PIP_MODE_CMD} --${PIP_MODE} ${PRINTPIPMODE}" "FAILS"
    exit $EXIT_FAIL;
fi

echo $prog ": PASS"
exit $EXIT_PASSD;
