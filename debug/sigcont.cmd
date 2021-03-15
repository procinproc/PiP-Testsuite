#!/bin/sh

echo PID=$1 PIPID=$2 PROG=$3
PID=$1
kill -CONT $PID
