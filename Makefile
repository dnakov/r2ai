.PHONY: all all.old deps clean deps-global pub lint cilint
.PHONY: install uninstall user-install user-uninstall

CC=gcc
CFLAGS=-g -Wall -I.. -I. -std=c99 -DTEST_MODE=1 -D_GNU_SOURCE
LDFLAGS=-lcmocka
R2_CFLAGS=$(shell pkg-config --cflags r_core)
R2_LDFLAGS=$(shell pkg-config --libs r_core)

all:
	@echo "Usage: Run 'make' in the following subdirectories instead"
	@echo "src/    - Modern C rewrite in form of a native r2 plugin"
	@echo "py/     - The old Python cli and r2 plugin"
	@echo "decai/  - r2js plugin with focus on decompiling"
	@echo "server/ - shellscript to easily run llamacpp and other"
	@false

clean:
	@echo We are clean already

mrproper:
	$(MAKE) clean

