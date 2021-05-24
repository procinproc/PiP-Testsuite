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

# XXX TO-DO: LC_ALL=en_US.UTF-8 doesn't work if custom-built libc is used
unset LANG LC_ALL

TEST_TRAP_SIGS='1 2 14 15';

check_command() {
    prog=$@;
    if ! $@ > /dev/null 2>&1; then
	echo "[$prog] does not work properly";
	exit 2;
    fi
}

check_command "dirname ."
check_command "basename ."

dir=`dirname $0`;
dir_real=`cd $dir && pwd`
base=`basename $0`;
myself=$dir_real/$base;

longestmsg=" T -- UNRESOLVED :-O";
width=60;

print_usage()
{
    echo >&2 "Usage: $base [-APLGCT] [-thread] [-process[:preload|:got|:pipclone]] [<test_list_file>]";
    exit 2;
}

set_term_width() {
    if [ "x$COLUMNS" != "x" ]; then
	termwidth=${COLUMNS};
    elif [ "x$TERM" != "xdumb" ]; then
	termwidth=`tput cols`;
    else
	termwidth=80;
    fi
    lmsglen=${#longestmsg};
    width=$((termwidth-lmsglen));
    if [ $width -lt 40 ]; then
	width=60;
    fi
}

# not to print any debug messages
export PIP_NODEBUG=1;

rprocs=`ps rux | wc -l`;
if [ $rprocs -gt 3 ]; then
    echo >&2 "WARNING: some other processes seem to be running ... ($rprocs)"
fi

if ! [ -f ${dir_real}/config.sh ]; then
    ${dir_real}/configure --help
    exit 1
fi
. $dir_real/config.sh

BUILD_VERSION_FILE="${dir_real}/.pip_version"
if ! [ -f ${BUILD_VERSION_FILE} ]; then
    echo "$base: Not yet built"
    exit 1
fi
BUILD_VERSION=`cat ${BUILD_VERSION_FILE} | cut -d '.' -f 1`
if [ ${PIP_VERSION_MAJOR} -gt 1 ]; then
    if ! [ -x ${LIBPIPSO} ]; then
	echo "$base: Unable to find ${LIBPIPSO}"
	exit 1
    fi
    CONFIG_VERSION=`${LIBPIPSO} --version | cut -d '.' -f 1`
else
    PIP_CONFIG_HEADER=${PIP_DIR}/include/pip_config.h
    if ! [ -f ${PIP_CONFIG_HEADER} ]; then
	echo "$base: Unable to find ${PIP_CONFIG_HEADER}"
	exit 1
    fi
    CONFIG_VERSION=`grep PACKAGE_VERSION ${PIP_CONFIG_HEADER} 2> /dev/null | \
	 	    cut -d ' ' -f 3 | cut -d '"' -f 2 | cut -d '.' -f 1`
fi
if [ "${BUILD_VERSION}" != "${CONFIG_VERSION}" ]; then
    echo "$base: Version miss-match: build=${BUILD_VERSION} configure=${CONFIG_VERSION}"
    exit 1
fi

path=.:${dir_real}/utils:${dir_real}/basics:${PIP_DIR}/bin
if [ x"$SUMMARY_FILE" = x ]; then
    export PATH=${path}:$PATH
    sum_file=$dir_real/.test-sum-$$.sh
else
    sum_file=$SUMMARY_FILE
fi

unset PIP_MODE
check_command "pip_mode_check"
##check_command "dlmopen_count"
##check_command "ompnumthread"

cleanup() {
    echo;
    echo "cleaning up ..."
    rm -f $sum_file;
    exit 1;
}

trap cleanup 2;

if [ x"$INCLUDE_FILE" != x ]; then
    inc_fn=$INCLUDE_FILE': ';
    case $inc_fn in
	./*) inc_fn=`expr "$inc_fn" : '..\(.*\)'`;;
    esac
fi

. $dir/exit_code.sh.inc

export NTASKS=`$MCEXEC dlmopen_count -p`
##if [ $NTASKS -lt 8 ]; then
##    echo "dlmopen_count:$NTASKS is not enough"
##    exit 1;
##fi

export OMP_NUM_THREADS=`$MCEXEC ompnumthread`;
##if [ $OMP_NUM_THREADS -lt 4 ]; then
##    echo "ompnumthread:$OMP_NUM_THREADS is not enough"
##    exit 1;
##fi

debug=`${LIBPIPSO} --debug`

if ! [ -f ${PIP_PRELOAD} ]; then
    echo "$base: Unable to find pip_preload.so";
    exit 1;
fi

file_summary() {
    rm -f $sum_file;
    echo TEST_LOG_FILE=$TEST_LOG_FILE		>> $sum_file;
    echo TEST_LOG_XML=$TEST_LOG_XML		>> $sum_file;
    echo TEST_OUT_STDOUT=$TEST_OUT_STDOUT	>> $sum_file;
    echo TEST_OUT_STDERR=$TEST_OUT_STDERR	>> $sum_file;
    echo TEST_OUT_TIME=$TEST_OUT_TIME		>> $sum_file;
    echo n_PASS=$n_PASS              		>> $sum_file;
    echo n_FAIL=$n_FAIL              		>> $sum_file;
    echo n_XPASS=$n_XPASS             		>> $sum_file;
    echo n_XFAIL=$n_XFAIL             		>> $sum_file;
    echo n_UNRESOLVED=$n_UNRESOLVED   		>> $sum_file;
    echo n_UNTESTED=$n_UNTESTED       		>> $sum_file;
    echo n_UNSUPPORTED=$n_UNSUPPORTED 		>> $sum_file;
    echo n_KILLED=$n_KILLED           		>> $sum_file;
    echo TOTAL_TIME=$TOTAL_TIME       		>> $sum_file;
    chmod +x $sum_file;
}

print_summary()
{
    if [ x"$SUMMARY_FILE" = x ]; then
	echo ""
	echo     "Total test: $(expr $n_PASS + $n_FAIL + $n_XPASS + $n_XFAIL \
		+ $n_UNRESOLVED + $n_UNTESTED + $n_UNSUPPORTED + $n_KILLED)"
	echo     "  success            : $n_PASS"
	echo     "  failure            : $n_FAIL"

	if [ $n_XPASS -gt 0 ]; then
	    echo "  unexpected success : $n_XPASS";
	fi
	if [ $n_XFAIL -gt 0 ]; then
	    echo "  expected failure   : $n_XFAIL";
	fi
	if [ $n_UNRESOLVED -gt 0 ]; then
	    echo "  unresolved         : $n_UNRESOLVED";
	fi
	if [ $n_UNTESTED -gt 0 ]; then
	    echo "  untested           : $n_UNTESTED";
	fi
	if [ $n_UNSUPPORTED -gt 0 ]; then
	    echo "  unsupported        : $n_UNSUPPORTED";
	fi
	if [ $n_KILLED -gt 0 ]; then
	    echo "  killed             : $n_KILLED";
	fi
    else
	file_summary;
    fi
}

reset_summary()
{
    TEST_TOP_DIR=$dir_real;
    TEST_LOG_FILE=test.log;
    : ${TEST_LOG_XML:=$TEST_TOP_DIR/test.log.xml};
##    TEST_OUT_STDOUT=$TEST_TOP_DIR/test.out.stdout;
##    TEST_OUT_STDERR=$TEST_TOP_DIR/test.out.err;
##    TEST_OUT_TIME=$TEST_TOP_DIR/test.out.time;
    TEST_OUT_STDOUT=test.out.stdout;
    TEST_OUT_STDERR=test.out.err;
    TEST_OUT_TIME=test.out.time;

    n_PASS=0;
    n_FAIL=0;
    n_XPASS=0;
    n_XFAIL=0;
    n_UNRESOLVED=0;
    n_UNTESTED=0;
    n_UNSUPPORTED=0;
    n_KILLED=0;
    TOTAL_TIME=0;

    file_summary;
}

pip_mode_name_P=process
pip_mode_name_L=$pip_mode_name_P:preload
pip_mode_name_G=$pip_mode_name_P:got
pip_mode_name_C=$pip_mode_name_P:pipclone
pip_mode_name_T=pthread

print_mode_list()
{
    echo "List of PiP execution modes:"
    echo "  " $pip_mode_name_T "(T)";
    echo "  " $pip_mode_name_G "(G)";
    echo "  " $pip_mode_name_P "(P)";
    echo "  " $pip_mode_name_L "(L)";
    echo "  " $pip_mode_name_C "(C)";
    exit 1;
}

# parse command line option
cmd=$0
case $# in
0)	print_usage;;
*)	run_test_L=''
	run_test_G=''
	run_test_C=''
	run_test_T=''
	while	case $1 in
		-*) true;;
		*) false;;
		esac
	do
		case $1 in *A*)	run_test_L=L; run_test_G=G; run_test_C=C; run_test_T=T;; esac
		case $1 in *P*)	run_test_L=L; run_test_G=G; run_test_C=C;; esac
		case $1 in *C*)	run_test_C=C;; esac
		case $1 in *L*)	run_test_L=L;; esac
		case $1 in *G*)	run_test_G=G;; esac
		case $1 in *T*)	run_test_T=T;; esac
		case $1 in *list*) print_mode_list;; esac
		case $1 in *thread)    run_test_T=T;; esac
		case $1 in *process)   run_test_L=L; run_test_G=G; run_test_C=C;; esac
		case $1 in *$pip_mode_name_L) run_test_L=L;; esac
		case $1 in *$pip_mode_name_G) run_test_G=G;; esac
		case $1 in *$pip_mode_name_C) run_test_C=C;; esac
		shift
	done
	if [ X"${run_test_L}${run_test_G}${run_test_C}${run_test_T}" = X ]; then
	    pip_mode_list="L G C T";
	else
	    pip_mode_list="$run_test_L $run_test_G $run_test_C $run_test_T"
	fi
	;;
esac

case $# in
0)	;;
1)	TEST_LIST=$1;;
*)	echo >&2 "$base: unknown argument '$*'"
	print_usage
	exit 2;;
esac

if [ x"$TEST_LIST" = x ]; then
    print_usage;
fi
if [ ! -e "$TEST_LIST" ]; then
    print_usage;
fi

if [ -z "$pip_mode_list" ]; then
    print_usage;
fi

if [ x"$SUMMARY_FILE" = x ]; then
    reset_summary;
else
    . $SUMMARY_FILE;
fi

[ -f ${TEST_LOG_FILE} ] && mv -f ${TEST_LOG_FILE} ${TEST_LOG_FILE}.bak

if [ -n "$MCEXEC" ]; then
    pip_mode_list_all='T';
else
    pip_mode_list_all='L G C T';
fi

if [ x"$SUMMARY_FILE" = x ]; then
    echo "--with-pip=$PIP_DIR"
    echo "libpip.so -- DEBUG:" $debug
    echo "LD_PRELOAD=$PIP_PRELOAD"
    echo "NTASKS:  " ${NTASKS}
    echo "NTHREADS:" ${OMP_NUM_THREADS}
    if [ x"$MCEXEC" != x ]; then
	echo "MCEXEC: " $MCEXEC
    fi
    if [ x"${PIP_TEST_THRESHOLD}" != x ]; then
	echo "PIP_TEST_THRESHOLD: $PIP_TEST_THRESHOLD"
    fi
fi

# check whether each $PIP_MODE is testable or not
run_test_L=''
run_test_G=''
run_test_C=''
run_test_T=''
if [ x"${SUMMARY_FILE}" = x ]; then
    for pip_mode in $pip_mode_list; do
	eval 'pip_mode_name=$pip_mode_name_'${pip_mode}
	mode_actual=`${PIP_MODE_CMD} -${pip_mode} pip_mode_check 2>/dev/null`
	case $pip_mode in
	    L)
		case $mode_actual in
		    process:preload) 
			run_test_L='L';
			echo "testing ${pip_mode} - ${pip_mode_name}";;
		    *)  echo >&2 "WARNING: $pip_mode_name is not testable";;
		esac;;
	    G)
		case $mode_actual in
		    process:got) 
			run_test_G='G';
			echo "testing ${pip_mode} - ${pip_mode_name}";;
		    *)  echo >&2 "WARNING: $pip_mode_name is not testable";;
		esac;;
	    C)
		case $mode_actual in
		    process:pipclone) 
			run_test_C='C';
			echo "testing ${pip_mode} - ${pip_mode_name}";;
		    *)  echo >&2 "WARNING: $pip_mode_name is not testable";;
		esac;;
	    T)
		case $mode_actual in
		    pthread) 
			run_test_T='T';
			echo "testing ${pip_mode} - ${pip_mode_name}";;
		    *)  echo >&2 "WARNING: $pip_mode_name ($pip_mode) is not testable";;
		esac;;
	esac
    done
else
    for pip_mode in $pip_mode_list; do
	eval 'pip_mode_name=$pip_mode_name_'${pip_mode}
	mode_actual=`${PIP_MODE_CMD} -${pip_mode} pip_mode_check 2>/dev/null`
	case $pip_mode in
	    L)
		case $mode_actual in
		    process:preload) run_test_L='L';;
		    *)  :;;
		esac;;
	    G)
		case $mode_actual in
		    process:got) run_test_G='G';;
		    *)  :;;
		esac;;
	    C)
		case $mode_actual in
		    process:pipclone) run_test_C='C';;
		    *)  :;;
		esac;;
	    T)
		case $mode_actual in
		    pthread) run_test_T='T';;
		    *)  :;;
		esac;;
	esac
    done
fi

pip_mode_list="$run_test_L $run_test_G $run_test_C $run_test_T"

pml=`echo ${pip_mode_list} | tr -d ' '`
if [ -z "$pml" ]; then
    echo "$base: No PIP_MODE to test"
    exit 1
fi

if [ x"$SUMMARY_FILE" = x ]; then
    echo;
fi

options=
if [ x"$run_test_L" = x"L" ]; then
    options="-L $options";
fi
if [ x"$run_test_G" = x"G" ]; then
    options="-G $options";
fi
if [ x"$run_test_C" = x"C" ]; then
    options="-C $options";
fi
if [ x"$run_test_T" = x"T" ]; then
    options="-T $options";
fi

LOG_BEG="=== ============================================================ ===="
LOG_SEP="--- ------------------------------------------------------------ ----"

if [ x"$SUMMARY_FILE" = x ]; then
    echo "<testsuite>" >$TEST_LOG_XML
fi

cd  `dirname $TEST_LIST`

while read line; do
    set_term_width;
	set x $line
	shift
	case $# in 0) continue;; esac
	case $1 in '%include')
		shift;
		ifile=$1
		if [ -f $ifile ]; then
		    file_summary;
		    echo ">>> $ifile"
		    SUMMARY_FILE=$sum_file \
			INCLUDE_FILE=$ifile \
			$myself $options $ifile;
		    echo "<<< $ifile"
		    . $sum_file;
		else
		    echo >&2 $base: "### inlcude file ($ifile) not found ###";
		fi
		continue;;
	esac
	case $1 in '#'*) continue;; esac

	cmd="$@";
	CMD=$inc_fn$cmd;

        if [ ${#CMD} -ge $width ]; then
	    dash="... "
	    w=$((${width}-${#dash}))
	    short="`printf "%-${w}.${w}s" "${inc_fn}${cmd}" "${dash}"`"
	else
	    short=$CMD;
	fi

	for pip_mode in $pip_mode_list
	do
		eval 'pip_mode_name=$pip_mode_name_'${pip_mode}

		printf "%-${width}.${width}s ${pip_mode} --" "$short"
		(
		  echo "$LOG_BEG"
		  echo "--- $CMD PIP_MODE=${pip_mode_name}"
		  echo "$LOG_SEP"
		  date +'@@_ start at %s - %Y-%m-%d %H:%M:%S'
		) >>$TEST_LOG_FILE
		rm -f $TEST_OUT_STDOUT $TEST_OUT_STDERR $TEST_OUT_TIME

		SECONDS=0
		${PIP_MODE_CMD} -${pip_mode} ${MCEXEC} ${cmd} \
		    > ${TEST_OUT_STDOUT} 2> ${TEST_OUT_STDERR} < /dev/null;
		test_exit_status=$?
		t=$SECONDS

		msg=
		case $test_exit_status in
		$EXIT_PASS)		status=PASS;;
		$EXIT_FAIL)		status=FAIL;;
		$EXIT_XPASS)		status=XPASS;;
		$EXIT_XFAIL)		status=XFAIL;;
		$EXIT_UNRESOLVED)	status=UNRESOLVED;;
		$EXIT_UNTESTED)		status=UNTESTED;;
		$EXIT_UNSUPPORTED)	status=UNSUPPORTED;;
		$EXIT_KILLED)		status=KILLED;;
		*)			status=UNRESOLVED;
					msg="exit status $test_exit_status";;
		esac
		eval "((n_$status=n_$status + 1))"

		# floating point expression
		TOTAL_TIME=$(awk 'BEGIN {print '"$TOTAL_TIMME"'+'"$t"'; exit}')
		(
			echo '  <testcase classname="'PIP'"' \
				'name="'"${CMD} - ${pip_mode_name}"'"' \
				'time="'"${t}"'">'
			case $status in
			PASS|XPASS)
				;;
			FAIL)
				echo '    <failure type="'$status'"/>';;
			XFAIL|UNTESTED|UNSUPPORTED)
				echo '    <skipped/>';;
			UNRESOLVED|KILLED)
				printf '    <error type="'$status'"';
				if [ -n "$msg" ]; then
					printf ' message="'"$msg"'"'
				fi
				echo '/>'
				;;
			esac
			if [ -s $TEST_OUT_STDOUT ]; then
				printf '    <system-out><![CDATA['
				iconv -fISO-8859-1 -tUTF-8 <$TEST_OUT_STDOUT
				echo ']]></system-out>'
			fi
			if [ -s $TEST_OUT_STDERR ]; then
				printf '    <system-err><![CDATA['
				iconv -fISO-8859-1 -tUTF-8 <$TEST_OUT_STDERR
				echo ']]></system-err>'
			fi
			echo '  </testcase>'
		) >>$TEST_LOG_XML

		: ${msg:=$status}
		(
			if [ -s $TEST_OUT_STDOUT ]; then
				echo "@@:stdout"
				cat $TEST_OUT_STDOUT
			fi
			if [ -s $TEST_OUT_STDERR ]; then
				echo "@@:stderr"
				cat $TEST_OUT_STDERR
			fi
			if [ -s $TEST_OUT_TIME ]; then
				echo "@@:time"
				cat $TEST_OUT_TIME
			fi

			date +'@@~  end  at %s - %Y-%m-%d %H:%M:%S'
			echo "$LOG_SEP"
			printf "@:= %s %s\n" $CMD "$msg"
		) >>$TEST_LOG_FILE

		rm -f $TEST_OUT_STDOUT $TEST_OUT_STDERR $TEST_OUT_TIME

		case $status in
		FAIL)		msg="$msg :-O";;
		XPASS)		msg="$msg :-D";;
		XFAIL)		msg="$msg :-?";;
		UNRESOLVED)	msg="$msg :-O";;
		UNTESTED)	msg="$msg :-|";;
		UNSUPPORTED)	msg="$msg";;
		KILLED)		msg="$msg ^C?";;
		esac

		echo " $msg"

		# stopped by Control-C or something like that
		[ $test_exit_status -eq $EXIT_KILLED ] && break 2
	done

done < `basename $TEST_LIST`

if [ x"$SUMMARY_FILE" = x ]; then
    echo "</testsuite>" >>$TEST_LOG_XML
else
    file_summary;
fi

(
	echo $LOG_BEG
	print_summary
) >>$TEST_LOG_FILE

print_summary

if [ x"$SUMMARY_FILE" = x ]; then
    rm -f $sum_file
fi

case $n_KILLED in 0) :;; *) exit $EXIT_KILLED;; esac
case $n_PASS   in 0)        exit $EXIT_FAIL;;   esac

if [ $n_FAIL -eq 0 -a $n_UNRESOLVED -eq 0 -a $n_PASS -gt 0 ]; then
    exit $EXIT_PASS
fi

if [ x"${SUMMARY_FILE}" = x ]; then
    if [ x"${PIP_TEST_THRESHOLD}" != x ]; then
	nerr=`expr $n_FAIL + $n_UNRESOLVED`
	if [ $nerr -lt ${PIP_TEST_THRESHOLD} ]; then
	    echo "Some test fails ($nerr) but less than PIP_TEST_THRESHOLD ($PIP_TEST_THRESHOLD)"
	    exit $EXIT_PASS
	fi
    fi
fi

exit $EXIT_FAIL
