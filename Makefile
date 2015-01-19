CFLAGS += -Weverything -O2 -std=c99 -ggdb
CC = clang

SOURCES = src/twbctf.c src/sds.c
HEADERS = src/twbctf.h src/sds.h
MANPAGES = docs/sds.3 docs/sdsnew.3 docs/sdsfree.3

all: sds-test $(MANPAGES)


sds-test: $(SOURCES) $(HEADERS) src/test.c
	@echo "==> Building sds-test"
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

docs/%.3: docs/%.rst
	@echo "==> Building manpages"
	sphinx-build -b man -E docs/ docs/


test: sds-test
	@echo "==> Running test suite"
	./sds-test


clean:
	rm -f sds-test
	rm docs/*.3


.PHONY: all test
