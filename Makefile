CFLAGS += -Weverything -std=c11 -g
CC = clang

SOURCES = src/twbctf.c src/sds.c
HEADERS = src/twbctf.h src/sds.c

all: sds-test

sds-test: $(SOURCES) $(HEADERS) src/test.c
	$(CC) $(CFLAGS) -o $@ $(SOURCES)
	@echo ">>> Type ./sds-test to run the sds.c unit tests."

clean:
	rm -f sds-test
