CFLAGS			= -Wall -std=c99
DEBUG_BUILD		=
OBJS			= memory.o builtins.o continuation_passing.o reader.o eval.o printer.o symbol_table.o environment.o scanner.o bstrlib.o logger.o output.o

.PHONY: all debug run clean

all: scheme

debug: DEBUG_BUILD = -g -DDEBUG
debug: scheme

scheme: scheme.c $(OBJS)
	$(CC) $(CFLAGS) $(DEBUG_BUILD) scheme.c $(OBJS) -o scheme

logger.o: logger.h logger.c output.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c logger.c

memory.o: memory.h memory.c bstrlib.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c memory.c

builtins.o: builtins.h builtins.c memory.o continuation_passing.o eval.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c builtins.c

continuation_passing.o: continuation_passing.h continuation_passing.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c continuation_passing.c

scanner.o: scanner.h scanner.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c scanner.c

reader.o: reader.h reader.c memory.o scanner.o symbol_table.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c reader.c

eval.o: eval.h eval.c memory.o continuation_passing.o logger.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c eval.c

printer.o: printer.h printer.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c printer.c

output.o: output.h output.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c output.c

symbol_table.o: symbol_table.h symbol_table.c memory.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c symbol_table.c

environment.o: environment.h environment.c memory.o logger.o
	$(CC) $(CFLAGS) $(DEBUG_BUILD) -c environment.c

run: tests/*.c scheme
	cd tests; make tests

clean:
	rm -rf $(OBJS) scheme *.dSYM
	cd tests; make clean
