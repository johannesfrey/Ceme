OBJS = memory.o reader.o eval.o printer.o 
CFLAGS = -g -Wall -std=gnu99

run: tests/*.c scheme
	cd tests; make tests

scheme: scheme.c $(OBJS)
	gcc scheme.c $(OBJS) -o scheme

memory.o: memory.h memory.c
	gcc -c memory.c

reader.o: reader.h reader.c memory.o
	gcc -c reader.c

eval.o: eval.h eval.c memory.o
	gcc -c eval.c

printer.o: printer.h printer.c memory.o
	gcc -c printer.c
