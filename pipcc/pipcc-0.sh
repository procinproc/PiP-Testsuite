#!/bin/sh

tstp=$0
dir=`dirname $0`
. ${dir}/../config.sh
. ${dir}/../exit_code.sh.inc

hello="hello";

check_rv () {
    if [ $1 -ne 0 ]; then
	exit $EXIT_FAIL;
    fi
}

rm -f ${hello}.o ${hello}

${PIPCC} ${CPPFLAGS} -g -c ${hello}.c -o ${hello}.o;
check_rv $?;
${PIPCC} ${CPPFLAGS} -g ${hello}.o -o ${hello};
check_rv $?;
${PIP_CHECK} -b ${hello}
check_rv $?;

./${hello};
check_rv $?;

echo $tstp ": PASS"
exit $EXIT_PASS;
