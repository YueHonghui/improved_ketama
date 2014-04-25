#include "ketama.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

static ketama_ctx*
create_ketama(int* weights, char** hosts, uint16_t* ports, int cnt)
{
    item_info ii;
    ketama_ctx* ketama = ketama_create();
    if(NULL == ketama){
        fprintf(stderr, "ketama_create failed.\n");
        return NULL;
    }
    int i = 0;
    for(i = 0; i < cnt; i++){
        ii.weight = weights[i];
        strncpy(ii.addr.hostname, hosts[i], sizeof(ii.addr.hostname));
        ii.addr.hostname[sizeof(ii.addr.hostname) - 1] = 0;
        ii.addr.port = ports[i];
        ketama_append_item(ketama, &ii);
    }
    ketama_distribute_item(ketama);
    return ketama;
}

int
main()
{
    int weights[] = {1000, 1500, 2000, 1300, 4000, 3000};
    uint16_t ports[] = {8080, 80, 80, 81, 80, 8080};
    char* hosts[] = {"testserver1", "testserver2", "testserver3", "testserver4", "testserver5", "testserver6"};
    char* keys[] = {"testkey1", "testkey2", "testkey3"};

    ketama_ctx* ctx = create_ketama(weights, hosts, ports, 6);
    if(NULL == ctx){
        fprintf(stderr, "create ketama failed\n");
        goto failed_finish;
    }

    ipv4_addr addr;
    int i = 0;
    for(i = 0; i < COUNT_OF(keys); i++){
        uint64_t h = ketama_hashi(keys[i], strlen(keys[i]));
        int ret = ketama_get_simple(ctx, &addr, h);
        ASSERT(1 == ret);
        printf("%s\n", addr.hostname);
    }

    if(NULL != ctx) ketama_destroy(ctx);
    return 0;

failed_finish:
    if(NULL != ctx) ketama_destroy(ctx);
    return -1;
}
