CFLAGS = -g -Wall -std=gnu99

tests: reader_test
	./reader_test

reader_test: reader_test.c ../reader.h ../reader.c ../dbg.h
	cd ..; make reader.o
	gcc reader_test.c ../reader.o ../memory.o -o reader_test
