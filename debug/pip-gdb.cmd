#!/bin/sh

cmd=$0
PID=$1

pip_gdb="${PIP_GDB_PATH}"
bindir=`dirname $pip_gdb`
pips=${bindir}/pips

if [ -x ${pip_gdb} ]; then
    ${pip_gdb} -p $PID -x ./pip-gdb.xcmd -q -n &
    sleep 1
    kill -CONT $PID
    sleep 1
    ${pips} x -s CONT -f $PPID
    wait
else
    echo "$cmd: pip-gdb not found"
fi
