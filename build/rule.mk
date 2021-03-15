
all: $(PROGRAMS)
.PHONY: all

check: test
.PHONE: test check

clean: prog-clean obj-clean \
	misc-clean post-clean-hook
.PHONY: clean

testclean: \
	testclean-here post-testclean-hook
.PHONY: testclean

veryclean: clean testclean prog-clean post-veryclean-hook
.PHONY: veryclean

distclean: testclean veryclean \
	prog-distclean \
	distclean-here post-distclean-hook
.PHONY: distclean

misc-clean:
	@echo \
	$(RM) *.E *.S
	$(RM) \#*\# .\#* *~
	$(RM) core.*
	-$(RM) .nfs*
.PHONY: misc-clean

distclean-here:
.PHONY: distclean-here

testclean-here:
	$(RM) *.log.xml
	$(RM) test.log test.log.* test.out.* .test-sum-*.sh
	$(RM) loop-*.log loop-*.log~ .loop-*.log \#loop-*.log\# .\#loop-*.log
	$(RM) core.*
	$(RM) seek-file.text
.PHONY: testclean-here

### prog rules

prog-all: $(PROGRAMS)
.PHONY: prog-all

prog-clean:
	@case "$(PROGRAMS)" in \
	'')	;; \
	*)	echo \
		$(RM) $(PROGRAMS); \
		$(RM) $(PROGRAMS);; \
	esac
.PHONY: prog-clean

prog-distclean:
.PHONY: prog-distclean

prog-testclean:
	$(RM) *.log *.log.xml
	$(RM) test.log test.log.* test.out.*
	$(RM) .test-sum-*.sh
	$(RM) loop-*.log .loop-*.log
	$(RM) seek-file.text
.PHONY: prog-testclean

### common rules

obj-clean:
	@case "$(OBJS)" in \
	'')	;; \
	*)	echo \
		$(RM) $(OBJS); \
		$(RM) $(OBJS);; \
	esac

debug::
	DBGFLAG=-DDEBUG $(MAKE) all

cdebug:: clean debug

$(OBJS):

### hooks

post-all-hook:
post-clean-hook:
post-veryclean-hook:
post-distclean-hook:
post-testclean-hook:
.PHONY: post-all-hook
.PHONY: post-clean-hook post-veryclean-hook post-distclean-hook
.PHONY: post-testclean-hook
.PHONY: debug cdebug
.PHONY: test

all : $(PROGRAMS)

%: %.c $(SRCS) $(DEPS) Makefile
	$(PIPCC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@
