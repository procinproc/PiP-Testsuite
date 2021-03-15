#!/bin/sh

PID=$1
RECFILE="perf-$3-$2.data"
perf record -o $RECFILE -p $PID &
sleep 1
kill -CONT $PID
wait
perf report -i $RECFILE --stdio
ls -l perf-*-*.data*
rm perf-*-*.data*
