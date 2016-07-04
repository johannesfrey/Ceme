#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../dbg.h"
#include "../reader.h"
#include "../memory.h"
#include "../symbol_table.h"

static char *test_file_name = "reader_test_file";

FILE* 
write_to_testfile(const char *string)
{
    FILE *test_file = NULL;

    check(test_file = fopen(test_file_name, "w+"), "Could not open %s", test_file_name);
    check(fputs(string, test_file), "Could not write to %s", test_file_name);
    fflush(test_file);
    fclose(test_file);

    // Prepare for reading
    test_file = fopen(test_file_name, "r");

    return test_file;

error:
    if (test_file) fclose(test_file);
    return NULL;
}

int 
test_reader()
{
    FILE *input = NULL;
    object *obj = NULL;

    // Check for T_NUMBER types and skipping of whitespace
    check(input = write_to_testfile("123"), "write_to_testfile failed");
    obj = read_object(input);
    check(obj->any.tag == T_NUMBER, "Not of type T_NUMBER");
    check(obj->number.value == 123, "Bad value for Number");

    check(input = write_to_testfile("    123"), "write_to_testfile failed");
    obj = read_object(input);
    check(obj->any.tag == T_NUMBER, "Not of type T_NUMBER");
    check(obj->number.value == 123, "Bad value for Number");

    check(input = write_to_testfile("   123     "), "write_to_testfile failed");
    obj = read_object(input);
    check(obj->any.tag == T_NUMBER, "Not of type T_NUMBER");
    check(obj->number.value == 123, "Bad value for Number");

    // Check for T_STRING types
    check(input = write_to_testfile("\"abc\""), "write_to_testfile failed");
    obj = read_object(input);
    check(obj->any.tag == T_STRING, "Not of type T_STRING");
    check((strcmp(obj->string.value, "abc") == 0), "Bad value for String");

    check(input = write_to_testfile("\"abc\\td\""), "write_to_testfile failed");
    obj = read_object(input);
    check(obj->any.tag == T_STRING, "Not of type T_STRING");
    check((strcmp(obj->string.value, "abc\td") == 0), "Bad value for String");

    check(input = write_to_testfile("\"abc\\nd\""), "write_to_testfile failed");
    obj = read_object(input);
    check(obj->any.tag == T_STRING, "Not of type T_STRING");
    check((strcmp(obj->string.value, "abc\nd") == 0), "Bad value for String");

    // Check for well-known symbols (T_NIL, T_TRUE, T_FALSE)
    check(input = write_to_testfile("#t"), "write_to_testfile failed");
    obj = read_object(input);
    check((obj->any.tag == T_TRUE && obj == true_object), "Not of type T_TRUE");

    check(input = write_to_testfile("#f"), "write_to_testfile failed");
    obj = read_object(input);
    check((obj->any.tag == T_FALSE && obj == false_object), "Not of type T_FALSE");

    check(input = write_to_testfile("nil"), "write_to_testfile failed");
    obj = read_object(input);
    check((obj->any.tag == T_NIL && obj == nil_object), "Not of type T_NIL");

    // Check for custom symbols
    check(input = write_to_testfile("mysymbol"), "write_to_testfile failed");
    obj = read_object(input);
    check(obj->any.tag == T_SYMBOL, "Not of type T_SYMBOL");
    check((strcmp(obj->symbol.value, "mysymbol") == 0), "Bad value for String");

    // Check for lists (T_CONS)
    check(input = write_to_testfile("()"), "write_to_testfile failed");
    obj = read_object(input);
    check(TAG(obj) == T_NIL, "Empty list should return nil");

    check(input = write_to_testfile("(()())"), "write_to_testfile failed");
    obj = read_object(input);
    check(IS_CONS(obj) && IS_NIL(CAR(obj)) && IS_CONS(CDR(obj)) 
            && IS_NIL(CAR(CDR(obj))) && IS_NIL(CDR(CDR(obj))), "Problems reading two empty lists inside list");

    check(input = write_to_testfile("(1)"), "write_to_testfile failed");
    obj = read_object(input);
    check(IS_CONS(obj) && IS_NUMBER(CAR(obj)) && NUMBER_VAL(CAR(obj)) == 1
            && IS_NIL(CDR(obj)), "Problems reading list with single number entry");

    check(input = write_to_testfile("(1 2)"), "write_to_testfile failed");
    obj = read_object(input);
    check(IS_CONS(obj) && IS_NUMBER(CAR(obj)) && NUMBER_VAL(CAR(obj)) == 1, "Problems reading first number element in list with two number elements");
    obj = CDR(obj);
    check(IS_CONS(obj) && IS_NUMBER(CAR(obj)) && NUMBER_VAL(CAR(obj)) == 2, "Problems reading first number element in list with two number elements");
    obj = CDR(obj);
    check(IS_NIL(obj), "Problems reading nil terminator element in list with two number elements");

    fclose(input);

    return 0;

error:
    if (input) fclose(input);
    return -1;
}

int 
main()
{
    init_wellknown_objects();
    init_symbol_table();
    check(test_reader() == 0, "test_reader failed");

    return 0;

error:
    return -1;
}
