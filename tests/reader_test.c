#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../dbg.h"
#include "../reader.h"
#include "../memory.h"

static char *test_file_name = "reader_test_file";

FILE* write_to_testfile(const char *string)
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

int test_reader()
{
  FILE *input = NULL;
  object *obj = NULL;

  // Check for T_NUMBER types and skipping of whitespace
  check(input = write_to_testfile("123"), "write_to_testfile failed");
  obj = read_object(input);
  check(obj->u.any.type == T_NUMBER, "Not of type T_NUMBER");
  check(obj->u.number.value == 123, "Bad value for Number");

  check(input = write_to_testfile("    123"), "write_to_testfile failed");
  obj = read_object(input);
  check(obj->u.any.type == T_NUMBER, "Not of type T_NUMBER");
  check(obj->u.number.value == 123, "Bad value for Number");

  check(input = write_to_testfile("   123     "), "write_to_testfile failed");
  obj = read_object(input);
  check(obj->u.any.type == T_NUMBER, "Not of type T_NUMBER");
  check(obj->u.number.value == 123, "Bad value for Number");

  fclose(input);

  return 0;

error:
  if (input) fclose(input);
  return -1;
}

int main()
{
  check(test_reader() == 0, "test_reader failed");

  return 0;

error:
  return -1;
}
