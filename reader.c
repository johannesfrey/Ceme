#include <stdlib.h>
#include <ctype.h>

#include "reader.h"

void skip_whitespace(FILE *in_stream)
{
  int ch;

  while ((ch = getc(in_stream)) != EOF)
  {
    if (isspace(ch))
    {
      continue;
    }
    ungetc(ch, in_stream);
    break;
  }
}

object* read_number(FILE *in_stream, int first)
{
  int ch;
  int val = 0;

  // ASCII digits are encoded ascending so we can
  // subtract by char 0 for the real value
  val = first - '0';
  while (isdigit(ch = getc(in_stream)))
  {
    val = val * 10 + (ch - '0');
  }
  ungetc(ch, in_stream);

  return alloc_number(val);
}

object* read_object(FILE *in_stream)
{
  int ch;

  skip_whitespace(in_stream);

  ch = getc(in_stream);

  if (isdigit(ch))
  {
    return read_number(in_stream, ch);
  } 
  else
  {
    printf("Only fixed numbers for now\n");
    exit(0);
  }
}

