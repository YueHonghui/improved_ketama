#ifndef KETAMA_H_
#define KETAMA_H_
#include <stdint.h>

typedef struct ipv4_addr
{
    char hostname[128];
    uint16_t port;
}ipv4_addr;

typedef struct item_info
{
    uint32_t weight;
    ipv4_addr addr;
}item_info;

typedef struct ketama_ctx ketama_ctx;

uint64_t ketama_hashi(const char* str, int len);
ketama_ctx* ketama_create();
int ketama_append_item(ketama_ctx*, const item_info* item);
int ketama_distribute_item(ketama_ctx*);
void ketama_destroy(ketama_ctx*);
int ketama_get(ketama_ctx*, ipv4_addr* result, int max, uint64_t key, int hop_idx, int cnt);

static inline int ketama_get_simple(ketama_ctx* ctx, ipv4_addr* result, uint64_t key)
{
    return ketama_get(ctx, result, 1, key, 0, 1);
}

#endif
