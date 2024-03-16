CC ?= gcc
CFLAGS ?= -g -Wall -O2

SOURCES := $(wildcard ../*.c)

# this target should build all executables for all tests
all:
	$(CC) $(CFLAGS) -o filesystem $(SOURCES) `pkg-config fuse --cflags --libs`


