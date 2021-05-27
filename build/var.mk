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

PROGRAMS = $(PROGRAM)

PIP_INCDIR = $(PIP_DIR)/include
PIP_BINDIR = $(PIP_DIR)/bin

PIPLIB       = $(PIP_LIBDIR)/libpip.so
PIPCC        = $(PIP_BINDIR)/pipcc
PIPFC        = $(PIP_BINDIR)/pipfc
PIP_CHECK    = $(PIP_BINDIR)/pip-check
PIP_EXEC     = $(PIP_BINDIR)/pip-exec
PIP_MODE     = $(PIP_BINDIR)/pip-mode
PRINTPIPMODE = $(PIP_BINDIR)/printpipmode
PIPS         = $(PIP_BINDIR)/pips
PIP_UNPIE    = $(PIP_BINDIR)/pip-unpie

CFLAGS += -I$(top_srcdir)/include
DEPINCS = $(top_srcdir)/include/test.h
DEPLIBS = $(PIPLIB)
DEPS    = $(top_srcdir)/config.status $(DEPINCS) $(DEPLIBS)

CPPFLAGS += $(DBGFLAG)
