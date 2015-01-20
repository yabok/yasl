CFLAGS += -O2 -std=c99 -ggdb -fPIE -pie -fstack-protector-all -Wl,-z,relro -Wl,-z,now
CFLAGS += -Weverything -Wno-disabled-macro-expansion -Wno-vla -Wno-cast-align
CC = clang

SOURCES = test/twbctf.c src/yasl.c
HEADERS = test/twbctf.h src/yasl.h
MANPAGES = docs/yasl.3 docs/yaslnew.3 docs/yaslfree.3

all: yasl-test $(MANPAGES)


yasl-test: $(SOURCES) $(HEADERS) test/test.c
	@echo "==> Building yasl-test"
	$(CC) $(CFLAGS) -Isrc -o $@ $(SOURCES)

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
