CC      ?= gcc
CFLAGS  ?= -std=c17 -g -v\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble_util/*.c assemble.o
emulate: emulate_util/*.c emulate.o

clean:
	$(RM) *.o assemble emulate
	
