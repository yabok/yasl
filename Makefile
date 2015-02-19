CC = clang
SOVER = 0.0
SOMAJOR = 0

CFLAGS += -Isrc -O2 -std=c99 -ggdb -fstack-protector-all -Wl,-z,relro -Wl,-z,now
CFLAGS += -Werror -Weverything -Wno-disabled-macro-expansion
SOCFLAGS += $(CFLAGS) -fPIC -shared -Wl,-soname,libyasl.so.$(SOMAJOR)
TESTCFLAGS += $(CFLAGS) -fPIE -pie -Wno-vla -Wno-cast-align

MANPAGES = docs/yasl.3 docs/yaslnew.3 docs/yaslfree.3

DESTDIR ?= /
PREFIX ?= /usr/local

all: yasl-test libyasl.so.$(SOVER) $(MANPAGES)

libyasl.so.$(SOVER): src/yasl.c src/yasl.h
	@echo "==> Building libyasl.so.$(SOVER)"
	$(CC) $(SOCFLAGS) -o $@ src/yasl.c
	ln -s libyasl.so.$(SOVER) libyasl.so
	ln -s libyasl.so.$(SOVER) libyasl.so.$(SOMAJOR)

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
	-rm -f libyasl.so libyasl.so.$(SOMAJOR) libyasl.so.$(SOVER)
	-rm -f yasl-test
	-rm -rf docs/*.3 docs/.doctrees/

install: libyasl.so.$(SOVER)
	@echo "==> Installing libyasl to $(DESTDIR)$(PREFIX)"
	install -Dm644 src/yasl.h "$(DESTDIR)$(PREFIX)/include/yasl.h"
	install -Dm644 libyasl.so.$(SOVER) "$(DESTDIR)$(PREFIX)/lib/libyasl.so.$(SOVER)"
	ln -s libyasl.so.$(SOVER) "$(DESTDIR)$(PREFIX)/lib/libyasl.so.$(SOMAJOR)"
	ln -s libyasl.so.$(SOVER) "$(DESTDIR)$(PREFIX)/lib/libyasl.so"

.PHONY: all test clean install
