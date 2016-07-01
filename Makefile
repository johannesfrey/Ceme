CC			= clang
CFLAGS			= -Wall -std=gnu99
DEBUG_BUILD		= -g
THIRD_PARTY_DIR		= ./third_party
THIRD_PARTY_SRCS	:= $(wildcard $(THIRD_PARTY_DIR)/*.c)
OBJS			= memory.o reader.o eval.o printer.o symbol_table.o environment.o

.PHONY: all ndebug run clean

all: scheme

ndebug: DEBUG_BUILD = -DNDEBUG
ndebug: scheme

scheme: scheme.c $(THIRD_PARTY_SRCS) $(OBJS)
	$(CC) $(CFLAGS) $(DEBUG_BUILD) scheme.c $(THIRD_PARTY_SRCS) $(OBJS) -o scheme

memory.o: memory.h memory.c
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c memory.c

reader.o: reader.h reader.c memory.o symbol_table.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c reader.c

eval.o: eval.h eval.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c eval.c

printer.o: printer.h printer.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c printer.c

symbol_table.o: symbol_table.h symbol_table.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c symbol_table.c

environment.o: environment.h environment.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c environment.c

run: tests/*.c scheme
	cd tests; make tests

clean:
	rm -rf $(OBJS) scheme *.dSYM
	cd tests; make clean
