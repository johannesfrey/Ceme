CC		= clang
CFLAGS		= -Wall -std=gnu99 -g
TEST_PROGS 	= reader_test symbol_table_test

tests: $(TEST_PROGS)
	$(foreach prog,$(TEST_PROGS),./$(prog);)

reader_test: reader_test.c ../reader.h ../reader.c ../dbg.h
	cd ..; make reader.o
	$(CC) $(CFLAGS) reader_test.c ../reader.o ../memory.o -o reader_test

symbol_table_test: symbol_table_test.c ../symbol_table.h ../symbol_table.c ../third_party/superfasthash.c ../dbg.h
	cd ..; make symbol_table.o
	$(CC) $(CFLAGS) symbol_table_test.c ../third_party/superfasthash.c ../symbol_table.o ../memory.o -o symbol_table_test

clean:
	rm -rf $(TEST_PROGS) *.dSYM
