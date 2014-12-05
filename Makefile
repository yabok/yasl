CFLAGS = -Weverything -std=c11
CC = clang

SOURCES = src/test.c src/sds.c
HEADERS = src/test.h src/sds.c
all: sds-test

sds-test: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)
	@echo ">>> Type ./sds-test to run the sds.c unit tests."

clean:
	rm -f sds-test
