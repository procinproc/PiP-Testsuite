#!/bin/sh

# $PIP_license: <Simplified BSD License>
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#     Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
# $
# $RIKEN_copyright: Riken Center for Computational Sceience (R-CCS),
# System Software Development Team, 2016-2021
# $
# $PIP_TESTSUITE: Version 1.1.0$
#
# $Author: Atsushi Hori (R-CCS) mailto: ahori@riken.jp or ahori@me.com
# $

dir=`dirname $0`;
dir_real=`cd $dir && pwd`;

if ! [ -f ${dir_real}/config.sh ]; then
    ${dir_real}/configure --help
    exit 1
fi
. $dir_real/config.sh

cmdline="$0 $@";
cmd=`basename $0`;
ext=0;
TMP='';

PIP_MODE_CHECK=${dir}/utils/pip_mode_check
if ! [ -x ${PIP_MODE_CHECK} ]; then
    echo "Unable to find ${PIP_MODE_CHECK}. Maybe not build yet."
    exit 1
fi

prt_ext() {
    exit=$1
    if [ $quiet -eq 0 ]; then
	echo;
	case $exit in
	    0) echo "EXIT_PASS";;
	    1) echo "EXIT_FAIL";;
	    2) echo "EXIT_XPASS";;
	    3) echo "EXIT_XFAIL";;
	    4) echo "EXIT_UNRESOLVED";;
	    5) echo "EXIT_UNTESTED";;
	    6) echo "EXIT_UNSUPPORTED";;
	    7) echo "EXIT_KILLED";;
	    *) echo "(unknown:" $1 ")";;
	esac
    fi
}

print_usage() {
    echo >&2 "Usage: $cmd [OPTIONS] [<test_prog> ...]";
    echo >&2 "    -A: all PiP execution modes (default)";
    echo >&2 "    -P: process modes";
    echo >&2 "    -L: process:preload modes";
    echo >&2 "    -C: process:pipclone modes";
    echo >&2 "    -G: process:got modes";
    echo >&2 "    -T: pthread modes";
    echo >&2 "    -N: not specifyng execution mode";
    echo >&2 "    -n <NITER>: Number of iterations";
    echo >&2 "    -t <SEC>: time limit of one loop [seconds]";
    echo >&2 "    -s: show stdout/stderr";
    echo >&2 "    -k: clean loop-*.log files before running test proram";
    echo >&2 "    -d: Suppress debug output (may affects timing)";
    echo >&2 "    -i: ignore error";
    echo >&2 "    -S: Silent mode";
    exit 2;
}

finalize() {
    ${PIPS} -s TERM -v;
    ${PIPS} -s KILL > /dev/null 2>&1;
    if [ x"$TMP" != x ]; then
	if [ -f $TMP ]; then
	    echo "Logfile: $FILE";
	    mv $TMP $FILE;
	fi
    fi
}

sigsegv() {
    echo;
    echo "SIGEGV";
    finalize;
    exit 127;
}

control_c() {
    echo;
    finalize;
    exit 4;
}

trap sigsegv 11
trap control_c 2

duration=0;
iteration=0;
quiet=0;
display=0;
klean=0;
nodebug=0;
mode_list='';
nomode=0;
ignore_err=0;

case $# in
    0)	print_usage;;
    *)	while	case $1 in
	-*) true;;
         *) false;;
                esac
        do
	    case $1 in *A) mode_list='-C -L -G -T';; esac
	    case $1 in *P) mode_list="$mode_list -P";; esac
	    case $1 in *L) mode_list="$mode_list -L";; esac
	    case $1 in *C) mode_list="$mode_list -C";; esac
	    case $1 in *G) mode_list="$mode_list -G";; esac
	    case $1 in *T) mode_list="$mode_list -T";; esac
	    case $1 in *n) shift; iteration=$1;; esac
	    case $1 in *t) shift; duration=$1;;  esac
	    case $1 in *N)        nomode=1;;     esac
	    case $1 in *s)        display=1;;    esac
	    case $1 in *k)        klean=1;;      esac
	    case $1 in *d)        nodebug=1;;    esac
	    case $1 in *S)        quiet=1;;      esac
	    case $1 in *i)        ignore_err=1;; esac
	    case $1 in *h | *u)   print_usage;;  esac
	    shift;
        done
	;;
esac

if [ $# -lt 1 ]; then
    print_usage;
fi

type -P $1 > /dev/null 2>&1
if [ $? != 0 ]; then
    echo "$1: Command not found";
    exit 5;
fi

if [ $klean -ne 0 ]; then
    rm -f loop-*.log;
    rm -f loop-*.log~;
fi

if [ $nodebug -ne 0 ]; then
    export PIP_NODEBUG=1;
fi

if [ $nomode -eq 0 ]; then
    if [ "x${mode_list}" = "x" ]; then
	mode_list='-C -L -G -T';
    fi
    for mode in $mode_list
    do
	if ${PIP_MODE_CMD} $mode ${PIP_MODE_CHECK} > /dev/null 2>&1; then
	    mlist="$mlist $mode";
	fi
    done
fi

PROGNAM=`basename $1`;
FILE="loop-$$.log";
TMP=.$FILE;

i=0;
start=`date +%s`;

if [ $display -ne 0 ]; then
    set -o pipefail;
fi

err_count=0

while true; do
    if [ $nomode -eq 0 ]; then
	for mode in $mlist; do
	    echo -n "" > $TMP #rewind
	    if [ $display -eq 0 ]; then
		echo "[[" "$i$mode" "]]" "$cmdline" ":" `date` >> $TMP;
		if [ $quiet -eq 0 ]; then
		    echo -n $i$mode "";
		fi
		${PIP_MODE_CMD} $mode $@ >> $TMP 2>&1;
	    else
		echo "[[" "$i$mode" "]]" "$cmdline" `date` | tee -a $TMP;
		${PIP_MODE_CMD} $mode $@ 2>&1 | tee -a $TMP;
	    fi
	    ext=$?;
	    if [ $ext != 0 ] ; then
		err_count=$((err_count+1));
		if [ $ignore_err -eq 0 ] ; then
		    prt_ext $ext;
		    finalize;
		    exit $ext;
		fi
	    fi
	    rm -f $TMP;
	    touch $TMP;
	done
    else			# nomode
	echo "" > $TMP;
	if [ $display -eq 0 ]; then
	    echo "[[" "$i" "]]" "$cmdline" `date` >> $TMP;
	    if [ $quiet -eq 0 ]; then
		echo -n $i "";
	    fi
	    $@ >> $TMP 2>&1;
	else
	    echo;
	    echo "[[" "$i" "]]" "$cmdline" `date` | tee -a $TMP;
	    echo;
	    $@ 2>&1 | tee -a $TMP;
	fi
	ext=$?;
	if [ $ext != 0 ]; then
	    err_count=$((err_count+1));
	    if [ $ignore_err -eq 0 ] ; then
	        prt_ext $ext;
	        finalize;
	        exit $ext;
	    fi
	fi
	rm -f $TMP;
	touch $TMP;
    fi

    i=$((i+1));

    if [ $err_count -gt 0 ] ; then
	echo;
	echo "*** Error run: $err_count / $i";
    fi

    if [ $iteration -gt 0 -a $i -ge $iteration ]; then
	if [ $quiet -eq 0 ]; then
	    echo;
	    echo Repeated $iteration times;
	fi
	break;
    fi

    now=`date +%s`;
    elaps=$((now-start));
    if [ $duration -gt 0 -a $elaps -gt $duration ]; then
	if [ $quiet -eq 0 ]; then
	    echo;
	    echo Time up "($duration Sec)";
	fi
	break;
    fi
done

rm -f $TMP;

prt_ext 0;
exit 0;
