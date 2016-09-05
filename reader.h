#ifndef READER_H
#define READER_H

#include <stdio.h>

#include "scanner.h"
#include "memory.h"

object* read_object(scanner_t *scanner);

#endif
