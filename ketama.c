#include "ketama.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "algorithm.h"
#include "list.h"
#include "md5.h"

#define RANDOM_STR "78430)()843&&^e.,^ksd-142687fFYTghjwlefj"
const int realloc_step = 50;

typedef struct item_info_list_node
{
    item_info item;
    struct list_head link;
}item_info_list_node;

typedef struct point
{
    uint64_t idx;
    item_info* item;
}point;

struct ketama_ctx
{
    uint32_t item_cnt;
    struct list_head items;

    uint64_t point_cnt;
    point* circle;
};

uint64_t 
ketama_hashi(const char* str, int len)
{
    ASSERT(len > 0);
    char md5[16];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, len);
    MD5_Final(md5, &ctx);
    uint64_t* tmp = (uint64_t*)md5;
    return tmp[0];
}

static int
compare_point(const void* p1, const void* p2)
{
    if(((point*)p1)->idx > ((point*)p2)->idx)
        return 1;
    else if(((point*)p1)->idx < ((point*)p2)->idx)
        return -1;
    else
        return 0;
}

static inline void
init_ketama(ketama_ctx* ctx)
{
    ctx->item_cnt = 0;
    INIT_LIST_HEAD(&ctx->items);
    ctx->point_cnt = 0;
    ctx->circle = NULL;
}

int
ketama_append_item(ketama_ctx* ctx, const item_info* item)
{
    ASSERT(ctx->item_cnt < (uint32_t)-1);
    item_info_list_node* item_node = (item_info_list_node*)malloc(sizeof(item_info_list_node));
    if(NULL == item_node){
        return -2;
    }
    memcpy(&item_node->item, item, sizeof(item_info));
    list_add_tail(&(item_node->link), &ctx->items);
    ctx->item_cnt++;
    return 0;
}

int
ketama_distribute_item(ketama_ctx* ctx)
{
    uint32_t total_weight = 0;
    struct list_head* pos;
    item_info_list_node* item_node = NULL;
    list_for_each(pos, &ctx->items){
        item_node = container_of(pos, item_info_list_node, link);
        total_weight += item_node->item.weight;
    }
    ctx->point_cnt = total_weight * 2;
    ctx->circle = (point*)malloc(sizeof(point) * ctx->point_cnt);
    if(NULL == ctx->circle){
        return -2;
    }
    uint64_t point_idx = 0;
    int i, len;
    char name[1024];
    char md5[16];
    uint64_t* tmp_point = NULL;
    MD5_CTX md5_ctx;
    item_info* item = NULL;
    list_for_each(pos, &ctx->items){
        item_node = container_of(pos, item_info_list_node, link);
        item = &item_node->item;
        for(i = 0; i < item->weight; i++){
            len = snprintf(name, sizeof(name), "%s:%hu-%d-%s", item->addr.hostname, item->addr.port, i, RANDOM_STR);
            if(len >= sizeof(name) || len < 0){
                free(ctx->circle);
                ctx->circle = NULL;
                return -3;
            }
            MD5_Init(&md5_ctx);
            MD5_Update(&md5_ctx, name, len);
            MD5_Final(md5, &md5_ctx);
            tmp_point = (uint64_t*)md5;
            ctx->circle[point_idx].idx = tmp_point[0];
            ctx->circle[point_idx].item = item;
            point_idx++;
            ctx->circle[point_idx].idx = tmp_point[1];
            ctx->circle[point_idx].item = item;
            point_idx++;
        }
    }
    ASSERT(point_idx == ctx->point_cnt);
    qsort(ctx->circle, ctx->point_cnt, sizeof(point), compare_point);
    return 0;
}

ketama_ctx* 
ketama_create()
{
    ketama_ctx* ctx = (ketama_ctx*)malloc(sizeof(ketama_ctx));
    if(unlikely(NULL == ctx)){
        return NULL;
    }
    init_ketama(ctx);
    return ctx;
}

void 
ketama_destroy(ketama_ctx* ctx)
{
    struct list_head* pos = NULL;
    struct list_head* tmp = NULL;
    list_for_each_safe(pos, tmp, &ctx->items){
        list_del(pos);
        free(container_of(pos, item_info_list_node, link));
    }
    free(ctx->circle);
    free(ctx);
}

int 
ketama_get(ketama_ctx* ctx, ipv4_addr* result, int max, uint64_t key, int hop_idx, int cnt)
{
    point tmp_point;
    tmp_point.idx = key;
    tmp_point.item = NULL;
    size_t pos = algo_circle_search(&tmp_point, ctx->circle, ctx->point_cnt, sizeof(point), compare_point);
    pos = (pos + hop_idx) % ctx->point_cnt;
    int i = 0;
    for(i = 0; i < cnt && i < max; i++){
        memcpy(result + i, &(ctx->circle[pos].item->addr), sizeof(ipv4_addr));
        pos = (pos + 1) % ctx->point_cnt;
    }
    return i;
}
