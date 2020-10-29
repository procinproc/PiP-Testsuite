#!/bin/sh

natasks=$1
nptasks=$2

PROG_RPATH=../bin/abc
PROG_APATH=`realpath ../bin/abc`

echo "relative path:" $PROG_RPATH
echo "absolute path:" $PROG_APATH

pip_blt $natasks $nptasks $PROG_RPATH
ev=$?
if [ $ev != 0 ]; then exit $ev; fi

pip_blt $natasks $nptasks $PROG_APATH
ev=$?
if [ $ev != 0 ]; then exit $ev; fi

exit 0;
