
PIP_DIR=/home/ahori/PiP/x86_64-redhat-7_pip-2/PiP

export PIP_VERSION=2.0.0
export PIP_VERSION_MAJOR=2
export PIP_VERSION_MINOR=0
export PIP_VERSION_SUBMINOR=0

export PIP_BINDIR=${PIP_DIR}/bin
export PIP_LIBDIR=${PIP_DIR}/lib

export PIPCC=${PIP_BINDIR}/pipcc
export PIP_CHECK=${PIP_BINDIR}/pip-check
export PIP_MODE_CMD=${PIP_BINDIR}/pip-mode
export PIPS=${PIP_BINDIR}/pips
export PRINTPIPMODE=${PIP_BINDIR}/printpipmode

export LIBPIPSO=${PIP_LIBDIR}/libpip.so

if test -f ${PIP_DIR}/preload/pip_preload.so; then
    export PIP_PRELOAD=${PIP_DIR}/preload/pip_preload.so
else
    export PIP_PRELOAD=${PIP_DIR}/lib/pip_preload.so
fi
