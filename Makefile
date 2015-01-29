CC = clang
SOVER = 0.0.0

CFLAGS += -Isrc -O2 -std=c99 -ggdb -fstack-protector-all -Wl,-z,relro -Wl,-z,now -Weverything
SOCFLAGS += $(CFLAGS) -fPIC -shared -Wl,-soname,libyasl.so.0
TESTCFLAGS += $(CFLAGS) -fPIE -pie -Wno-disabled-macro-expansion -Wno-vla -Wno-cast-align

MANPAGES = docs/yasl.3 docs/yaslnew.3 docs/yaslfree.3

DESTDIR ?= /
PREFIX ?= /usr/local

all: yasl-test libyasl.so.$(SOVER) $(MANPAGES)

libyasl.so.$(SOVER): src/yasl.c src/yasl.h
	@echo "==> Building libyasl.so.$(SOVER)"
	$(CC) $(SOCFLAGS) -o $@ src/yasl.c

yasl-test: src/yasl.c src/yasl.h test/twbctf.c test/twbctf.h test/tests.c
	@echo "==> Building yasl-test"
	$(CC) $(TESTCFLAGS) -o $@ test/twbctf.c src/yasl.c

docs/%.3: docs/%.rst
	@echo "==> Building manpages"
	sphinx-build -b man -E docs/ docs/

test: yasl-test
	@echo "==> Running test suite"
	./yasl-test

clean:
	rm libyasl.so.*
	rm -f yasl-test
	rm docs/*.3


.PHONY: all test
