#!/bin/sh

ntasks=$1

PROG_RPATH=../bin/abc
PROG_APATH=`realpath ../bin/abc`

echo "relative path:" $PROG_RPATH
echo "absolute path:" $PROG_APATH

pip_task $ntasks $PROG_RPATH
ev=$?
if [ $ev != 0 ]; then exit $ev; fi

pip_task $ntasks $PROG_APATH
ev=$?
if [ $ev != 0 ]; then exit $ev; fi

exit 0;
