#!/bin/sh

cmd=$0
PID=$1

pip_gdb=
if [ -x ${PIP_GDB_PATH} ]; then
    pip_gdb="${PIP_GDB_PATH}"
elif [ -x ${PIP_BINDIR}/pip-gdb ]; then
    pip_gdb="${PIP_BINDIR}/pip-gdb"
fi

if [ x"${pip_gdb}" != x ]; then
    ${pip_gdb} -p $PID -x ./pip-gdb.xcmd -q -batch &
    sleep 1
    kill -CONT $PID
    wait
else
    echo "$cmd: pip-gdb not found"
fi
