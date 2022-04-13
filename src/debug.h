#pragma once

#ifdef DEBUG

#include <stdio.h>
#include "chunk.h"
#include "value.h"

int disassembleInstruction(Chunk *chunk, int offset);
void disassembleChunk(Chunk *chunk, const char *name);

#endif
