#!/bin/sh

. ../config.sh

cmd=$0
PID=$1

${PIP_GDB} -p $PID
