#!/bin/sh

PID=$1
LTRACE=ltrace

cat /proc/$PID/maps > log

if type -P $LTRACE > /dev/null 2>&1 ; then
    RECFILE="ltrace-$3-$2.data"
    strace $LTRACE -D 77 -o $RECFILE -p $PID >> log 2>&1
    echo $RECFILE
    cat "$RECFILE"
fi
