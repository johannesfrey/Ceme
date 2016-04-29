#ifndef READER_H
#define READER_H

#include <stdio.h>

#include "memory.h"

object* read_object(FILE *in_stream);

#endif
