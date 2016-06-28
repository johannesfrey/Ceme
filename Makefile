CC	= clang
CFLAGS	= -Wall -std=gnu99 -g
OBJS	= memory.o reader.o eval.o printer.o symbol_table.o

run: tests/*.c scheme
	cd tests; make tests

scheme: scheme.c ./third_party/*.c $(OBJS)
	$(CC) $(CFLAGS) scheme.c ./third_party/*.c $(OBJS) -o scheme

memory.o: memory.h memory.c
	$(CC) $(CFLAGS) -c memory.c

reader.o: reader.h reader.c memory.o
	$(CC) $(CFLAGS) -c reader.c

eval.o: eval.h eval.c memory.o
	$(CC) $(CFLAGS) -c eval.c

printer.o: printer.h printer.c memory.o
	$(CC) $(CFLAGS) -c printer.c

symbol_table.o: symbol_table.h symbol_table.c 
	$(CC) $(CFLAGS) -c symbol_table.c 

clean:
	rm -rf $(OBJS) scheme *.dSYM
	cd tests; make clean
