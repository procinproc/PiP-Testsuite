
CWD=`dirname $0`
CWD=`realpath $CWD`

ntasks=$1
shift

export PIP_GDB_PATH=${PIP_GDB}
export PIP_SHOW_MAPS=on
export PIP_STOP_ON_START="${CWD}/pip-gdb.cmd@-1"

${CWD}/pip_task $ntasks $@
