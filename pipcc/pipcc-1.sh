#!/bin/sh

tstp=$0;
dir=`dirname $0`;
. ${dir}/../config.sh
. $dir/../exit_code.sh.inc;

${PIPCC} --piproot root.c -o root;
if [ $? -ne 0 ]; then
    echo "${pipcc} --piproot root.c -o root -- FAIL";
    rm -f root > /dev/null 2>&1;
    exit $EXIT_FAIL;
else
    echo "${PIPCC}pipcc --piproot root.c -o root -- SUCCESS";
fi

${PIP_CHECK} -r root;
if [ $? -ne 0 ]; then
    echo "${PIP_CHECK} -r root -- FAIL"
    rm -f root > /dev/null 2>&1;
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} -r root -- SUCCESS"
fi

${PIP_CHECK} -t root
if [ $? -eq 0 ]; then
    echo "${PIP_CHECK} -t root -- FAIL"
    rm -f root > /dev/null 2>&1;
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} -t root -- SUCCESS"
fi

${PIPCC} --piptask task.c -o task;
if [ $? -ne 0 ]; then
    echo '${PIPCC} --piptask task.c -o task -- FAIL';
    rm -f task > /dev/null 2>&1;
    exit $EXIT_FAIL;
else
    echo '${PIPCC} --piptask task.c -o task -- SUCCESS';
fi

${PIP_CHECK} -t task
if [ $? -ne 0 ]; then
    echo "${PIP_CHECK} -t task -- FAIL";
    rm -f task > /dev/null 2>&1;
    exit $EXIT_FAIL;
else
    echo "${PIP_CHECK} -t task -- SUCCESS";
fi

timer 1 ./root
if [ $? -ne 0 ]; then
    echo 'timer 1 ./root -- FAIL';
    exit $EXIT_FAIL;
else
    echo 'timer 1 ./root -- SUCCESS';
fi

rm -f root > /dev/null 2>&1;
rm -f task > /dev/null 2>&1;

echo $tstp ": PASS";
exit $EXIT_PASS;
