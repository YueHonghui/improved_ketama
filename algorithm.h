#ifndef ALGORITHM_H_
#define ALGORITHM_H_
#include <unistd.h>
#include <stdint.h>

uint64_t algo_circle_search(void* key, const void* base, size_t n, size_t size, int (*compar)(const void *, const void *));

#endif
