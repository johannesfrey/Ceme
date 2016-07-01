CC			= clang
CFLAGS			= -Wall -std=gnu99
DEBUG_BUILD		= -g
OBJS			= memory.o reader.o eval.o printer.o symbol_table.o environment.o

.PHONY: all ndebug run clean

all: scheme

ndebug: DEBUG_BUILD = -DNDEBUG
ndebug: scheme

scheme: scheme.c $(OBJS)
	$(CC) $(CFLAGS) $(DEBUG_BUILD) scheme.c $(OBJS) -o scheme

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
