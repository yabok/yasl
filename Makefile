CC = clang

CFLAGS += -Isrc -O2 -std=c99 -ggdb -fstack-protector-all -Wl,-z,relro -Wl,-z,now -Weverything
TESTCFLAGS += $(CFLAGS) -fPIE -pie -Wno-disabled-macro-expansion -Wno-vla -Wno-cast-align

MANPAGES = docs/yasl.3 docs/yaslnew.3 docs/yaslfree.3

all: yasl-test $(MANPAGES)


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
	rm -f yasl-test
	rm docs/*.3


.PHONY: all test
