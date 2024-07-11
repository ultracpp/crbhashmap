#ifndef XXHASH_H
#define XXHASH_H

#include <stdint.h>
#include <stddef.h>

uint64_t XXH64(const void* input, size_t length, uint64_t seed);

#endif
