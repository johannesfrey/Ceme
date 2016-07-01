CC		= clang
CFLAGS		= -Wall -std=gnu99 -g
TEST_PROGS 	= reader_test symbol_table_test environment_test

tests: $(TEST_PROGS)
	$(foreach prog,$(TEST_PROGS),./$(prog);)

reader_test: reader_test.c ../reader.h ../reader.c  ../dbg.h
	cd ..; make reader.o
	$(CC) $(CFLAGS) reader_test.c  ../reader.o ../memory.o ../symbol_table.o -o reader_test

symbol_table_test: symbol_table_test.c ../symbol_table.h ../symbol_table.c ../memory.h ../memory.c  ../dbg.h
	cd ..; make symbol_table.o
	$(CC) $(CFLAGS) symbol_table_test.c  ../symbol_table.o ../memory.o -o symbol_table_test

environment_test: environment_test.c ../symbol_table.h ../symbol_table.c ../environment.h ../environment.c ../memory.h ../memory.c  ../dbg.h
	cd ..; make environment.o symbol_table.o
	$(CC) $(CFLAGS) environment_test.c  ../environment.o ../symbol_table.o ../memory.o -o environment_test

clean:
	rm -rf $(TEST_PROGS) *.dSYM
