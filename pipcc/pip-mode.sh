#!/bin/sh

tstp=$0
dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

out=`${PIP_MODE_CMD} -p ${PRINTPIPMODE}`;
if [ "$out" != "process:preload" ]; then
    echo "${PIP_MODE_CMD} -p ${PRINTPIPMODE}" "FAILS"
    echo "Output: $out"
    exit $EXIT_FAIL;
fi

##out=`${PIP_MODE_CMD} -c ../../bin/printpipmode`;
##if [ "$out" != "process:pipclone" ]; then
##    echo "${PIP_MODE_CMD} -c ../../bin/printpipmode" "FAILS"
##    echo "Output: $out"
##    exit $EXIT_FAIL;
##fi

out=`${PIP_MODE_CMD} -g ${PRINTPIPMODE}`;
if [ "$out" != "process:got" ]; then
    echo "${PIP_MODE_CMD} -c ${PRINTPIPMODE}" "FAILS"
    echo "Output: $out"
    exit $EXIT_FAIL;
fi

out=`${PIP_MODE_CMD} -t ${PRINTPIPMODE}`;
if [ "$out" != "pthread" ]; then
    echo "${PIP_MODE_CMD} -t ${PRINTPIPMODE}" "FAILS"
    echo "Output: $out"
    exit $EXIT_FAIL;
fi

echo $tstp ": PASS"
exit $EXIT_PASSD;
