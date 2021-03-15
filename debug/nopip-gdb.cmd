#!/bin/sh

cmd=$0
PID=$1

gdb -p $PID -x ./nopip-gdb.xcmd -q -batch &
sleep 1
kill -CONT $PID
wait

