#!/bin/sh

tstp=$0
dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

hello="hello";

check_rv () {
    if [ $1 -ne 0 ]; then
	if [ -f ${hello} ]; then
	    rm -f ${hello} > /dev/null 2>&1;
	fi
	if [ -f ${hello}.o ]; then
	    rm -f ${hello}.o > /dev/null 2>&1;
	fi
	exit $EXIT_FAIL;
    fi
}

${PIPCC} -g -c ${hello}.c -o ${hello}.o;
check_rv $?;
${PIPCC} -g ${hello}.o -o ${hello};
check_rv $?;
${PIP_CHECK} -b ${hello}
check_rv $?;

timer 1 ./${hello};
check_rv $?;

echo $tstp ": PASS"
exit $EXIT_PASS;
