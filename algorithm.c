#include "algorithm.h"
#include <unistd.h>
#include <stdint.h>
#include "common.h"

uint64_t
algo_circle_search(void* key, const void* base, size_t n, size_t size, int (*compar)(const void *, const void *))
{
#define ITEM_IDX(base, size, i) ((const char*)base + size * i)
    ASSERT(n > 0);
    uint64_t start = 0;
    uint64_t end = (uint64_t)n;
    uint64_t pos = -1;
    int cmp = 0;
    while(1){
        pos = (start + end) / 2;
        cmp = compar(key, ITEM_IDX(base, size, pos));
        if(0 != cmp && (pos == start || pos == end)){
            pos = (pos + 1) % n;
            break;
        }
        if(cmp < 0){
            end = pos;
        }else if(cmp > 0){
            start = pos;
        }else{
            break;
        }
    }
    return pos;
}
