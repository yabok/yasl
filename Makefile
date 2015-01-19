CFLAGS += -O2 -std=c99 -ggdb -fPIE -pie -fstack-protector-all -Wl,-z,relro -Wl,-z,now
CFLAGS += -Weverything -Wno-disabled-macro-expansion
CC = clang

SOURCES = test/twbctf.c src/sds.c
HEADERS = test/twbctf.h src/sds.h
MANPAGES = docs/sds.3 docs/sdsnew.3 docs/sdsfree.3

all: sds-test $(MANPAGES)


sds-test: $(SOURCES) $(HEADERS) test/test.c
	@echo "==> Building sds-test"
	$(CC) $(CFLAGS) -Isrc -o $@ $(SOURCES)

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
