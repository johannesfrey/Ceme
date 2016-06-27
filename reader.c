#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dbg.h"
#include "reader.h"

void skip_whitespace(FILE *in_stream)
{
    int ch;

    while ((ch = getc(in_stream)) != EOF) {
        if (isspace(ch)) {
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
    while (isdigit(ch = getc(in_stream))) {
        val = val * 10 + (ch - '0');
    }
    ungetc(ch, in_stream);

    return alloc_number(val);
}

object* read_string(FILE *in_stream)
{
    int ch;
    char *chars = NULL;
    int current_buffer_size = 64;
    int current_used_size = 0;

    chars = malloc(current_buffer_size);
    ch = getc(in_stream);

    while ((ch != '"') && (ch != EOF)) {
        // Check for special characters
        if (ch == '\\') {
            ch = getc(in_stream);
            switch (ch) {
                case EOF:
                    log_err("Unterminated string");
                    break;
                case 'r':
                    ch = '\r';
                    break;
                case 't':
                    ch = '\t';
                    break;
                case 'n':
                    ch = '\n';
                    break;
                default:
                    break;
            }
        }

        chars[current_used_size++] = ch;
        if (current_used_size == current_buffer_size) {
            int new_buffer_size = current_buffer_size * 2;

            chars = realloc(chars, new_buffer_size);
            current_buffer_size = new_buffer_size;
        }

        ch = getc(in_stream);
    }
    // Trim to actual size and force append NUL character
    chars = realloc(chars, current_used_size+1);
    chars[current_used_size] = '\0';

    return alloc_string(chars);
}

object* read_symbol(FILE *in_stream)
{
    int ch;
    char *chars = NULL;
    int current_buffer_size = 64;
    int current_used_size = 0;

    chars = malloc(current_buffer_size);
    ch = getc(in_stream);

    while (!(isspace(ch)) && (ch != '(') && (ch != ')') && (ch != EOF)) {
        chars[current_used_size++] = ch;
        if (current_used_size == current_buffer_size) {
            int new_buffer_size = current_buffer_size * 2;

            chars = realloc(chars, new_buffer_size);
            current_buffer_size = new_buffer_size;
        }

        ch = getc(in_stream);
    }
    // Trim to actual size and force append NUL character
    chars = realloc(chars, current_used_size+1);
    chars[current_used_size] = '\0';

    if (strcmp(chars, "nil") == 0) {
        return nil_object;
    }
    if (strcmp(chars, "#t") == 0) {
        return true_object;
    }
    if (strcmp(chars, "#f") == 0) {
        return false_object;
    }

    return alloc_string(chars);
}


object* read_object(FILE *in_stream)
{
    int ch;

    skip_whitespace(in_stream);

    ch = getc(in_stream);

    if (isdigit(ch)) {
        return read_number(in_stream, ch);
    }
    if (ch == '"') {
        return read_string(in_stream);
    }
    else {
        ungetc(ch, in_stream);
        return read_symbol(in_stream);
    }
}

