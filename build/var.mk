VPATH=$(srcdir)

include $(top_srcdir)/build/config.mk

MKDIR_P = mkdir -p
RM = rm -f
CP = cp -f
MV = mv

PTHREADFLAG = -pthread

CFLAGS   = $(DEFAULT_CFLAGS)
CXXFLAGS = $(DEFAULT_CFLAGS)
FFLAGS   = $(DEFAULT_FFLAGS)
LDFLAGS  = $(DEFAULT_LDFLAGS)

PIPCC     = $(PIP_DIR)/bin/pipcc
PIP_CHECK = $(PIP_DIR)/bin/pip_check

PROGRAMS = $(PROGRAM)

PIP_INCDIR = $(PIP_DIR)/include
PIP_LIBDIR = $(PIP_DIR)/lib
PIP_BINDIR = $(PIP_DIR)/bin

PIPLIB       = $(PIP_LIBDIR)/libpip.so
PIPCC        = $(PIP_BINDIR)/pipcc
PIP_CHECK    = $(PIP_BINDIR)/pip-check
PIP_EXEC     = $(PIP_BINDIR)/pip-exec
PRINTPIPMODE = $(PIP_BINDIR)/printpipmode
PIPS         = $(PIP_BINDIR)/pips

CFLAGS += -I$(top_srcdir)/include -DPIP_VERSION=$(PIP_VERSION_MAJOR)
DEPINCS = $(top_srcdir)/include/test.h
DEPLIBS = $(PIPLIB)
DEPS    = $(top_srcdir)/config.status $(DEPINCS) $(DEPLIBS)

CPPFLAGS += $(DBGFLAG)
