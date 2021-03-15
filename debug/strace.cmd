#!/bin/sh

PID=$1
STRACE=strace

if type -P $STRACE > /dev/null 2>&1 ; then
    RECFILE="strace-$3-$2.data"
    $STRACE -o $RECFILE -p $PID &
    sleep 1
    kill -CONT $PID
    wait
    cat $RECFILE
fi
