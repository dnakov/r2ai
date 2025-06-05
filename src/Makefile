EXT=$(shell r2 -H R2_LIBEXT)
CFLAGS+=$(shell pkg-config --cflags r_core)
LDFLAGS+=$(shell pkg-config --libs r_core)
R2SYSPLUGDIR=$(shell r2 -H R2_LIBR_PLUGINS)
R2USRPLUGDIR=$(shell r2 -H R2_USER_PLUGINS)
CFLAGS+=-I. -fPIC
CFLAGS+=-Wall
CFLAGS+=-g

USE_R2_CURL=1
USE_LIBCURL=0

HAVE_LIBCURL=$(shell pkg-config --exists libcurl && echo 1 || echo 0)

CFLAGS+=-DUSE_R2_CURL=$(USE_R2_CURL)
CFLAGS+=-DUSE_LIBCURL=$(USE_LIBCURL)

ifeq ($(USE_LIBCURL),1)
  ifeq ($(HAVE_LIBCURL),1)
    CFLAGS+=$(shell pkg-config --cflags libcurl)
    LDFLAGS+=$(shell pkg-config --libs libcurl)
    CFLAGS+=-DHAVE_LIBCURL=1
  else
    CFLAGS+=-DHAVE_LIBCURL=0
  endif
endif

OBJS=r2ai.o
OBJS+=auto.o
OBJS+=vdb.o
OBJS+=tools.o
OBJS+=messages.o
OBJS+=anthropic.o
OBJS+=r2ai_http.o
OBJS+=openai.o
OBJS+=markdown.o

all: $(OBJS) r2check
	$(CC) -fPIC -shared -o r2ai.$(EXT) $(OBJS) $(CFLAGS) $(LDFLAGS)

try: all
	$(MAKE) user-install
	$(MAKE) doc-install

doc-install:
	mkdir -p /tmp/embeds
	cp ../doc/data/quotes.txt /tmp/embeds

vdb.o: vdb_embed.inc.c

indent:
	for a in *.c ; do python indent.py $$a ; done

user-install: user-uninstall
	mkdir -p $(R2USRPLUGDIR)
	cp -f r2ai.$(EXT) $(R2USRPLUGDIR)

user-uninstall:
	rm -f $(R2USRPLUGDIR)/r2ai.$(EXT)

install:
	cp -f r2ai.$(EXT) $(R2SYSPLUGDIR)

uninstall:
	rm -f $(R2SYSPLUGDIR)/r2ai.$(EXT)

r2check:
	@r2 -qcq --

# Testing targets
test: all
	@echo "Running unit tests..."
	@if [ -d test ]; then \
		$(MAKE) -C test test; \
	else \
		echo "Test directory not found. Run 'make setup-tests' first."; \
	fi

test-coverage: all
	@echo "Generating coverage report..."
	@if [ -d test ]; then \
		$(MAKE) -C test coverage; \
	else \
		echo "Test directory not found. Run 'make setup-tests' first."; \
	fi

test-install-deps:
	@echo "Installing test dependencies..."
	@if [ -d test ]; then \
		$(MAKE) -C test install-deps; \
	else \
		echo "Test directory not found. Please create test/ directory first."; \
	fi

test-clean:
	@if [ -d test ]; then \
		$(MAKE) -C test clean; \
	fi

setup-tests:
	@echo "Setting up test environment..."
	@echo "Test framework should be available in test/ directory"
	@echo "Run the following commands to get started:"
	@echo "  cd test"
	@echo "  make install-deps"
	@echo "  make test"

clean:
	rm -f *.o *.d
	@if [ -d test ]; then \
		$(MAKE) -C test clean; \
	fi

.PHONY: all try user-install user-uninstall install uninstall clean test test-coverage test-install-deps test-clean setup-tests

help:
	@echo "Available targets:"
	@echo "  all              - Build the r2ai plugin"
	@echo "  try              - Build and install for testing"
	@echo "  user-install     - Install plugin for current user"
	@echo "  user-uninstall   - Uninstall user plugin"
	@echo "  install          - Install plugin system-wide"
	@echo "  uninstall        - Uninstall system plugin"
	@echo "  clean            - Clean build artifacts"
	@echo ""
	@echo "Testing targets:"
	@echo "  test             - Run unit tests"
	@echo "  test-coverage    - Generate coverage report"
	@echo "  test-install-deps - Install testing dependencies"
	@echo "  test-clean       - Clean test artifacts"
	@echo "  setup-tests      - Show setup instructions"
