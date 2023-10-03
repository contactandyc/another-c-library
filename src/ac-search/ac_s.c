#include "another-c-library/ac_s.h"
#include "another-c-library/ac_allocator.h"
#include "another-c-library/ac_pool.h"
#include "another-c-library/ac_map.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#pragma pack(push)
#pragma pack(2)
typedef struct ac_block_s
{
    uint16_t type : 1;
    uint16_t group : 15;
    uint8_t *block;
} ac_block_t;
#pragma pack(pop)

static inline uint8_t *advance_id(uint8_t *p, uint32_t *id)
{
    uint16_t v = (*(uint16_t *)(p));
    *id = (v >> 4);

    p += sizeof(uint16_t);
    if (v & 0x8) // payload allows any number of bytes
        p += (*p) + 1;
    v &= 0x7;
    if (v < 7)
        p += v;
    else
    {
        if (*p > 1)
            p += (*p) + 1;
        else
        {
            if (*p == 0)
            {
                p++;
                p += (*(uint16_t *)(p)) + 2;
            }
            else
            {
                p++;
                p += (*(uint32_t *)(p)) + 4;
            }
        }
    }
    return p;
}

static inline uint8_t *advance_pos_id(uint8_t *p, uint8_t **sp, uint32_t *id)
{
    uint16_t v = (*(uint16_t *)(p));
    *id = (v >> 4);

    p += sizeof(uint16_t);
    if (v & 0x8) // payload allows any number of bytes
        p += (*p) + 1;
    v &= 0x7;
    if (v < 7) {
        *sp = p;
        p += v;
    }
    else
    {
        if (*p > 1) {
            *sp = p + 1;
            p += (*p) + 1;
        }
        else
        {
            if (*p == 0)
            {
                p++;
                *sp = p + 2;
                p += (*(uint16_t *)(p)) + 2;
            }
            else
            {
                p++;
                *sp = p + 4;
                p += (*(uint32_t *)(p)) + 4;
            }
        }
    }
    return p;
}

static inline uint32_t peek_id(uint8_t *p)
{
    uint16_t v = (*(uint16_t *)(p));
    return (v >> 4);
}

static inline uint32_t ac_s_get_group_id(uint32_t id)
{
    return (id & 0x7fff000) >> 12;
}

static inline uint32_t ac_s_get_sub_id(uint32_t id)
{
    return id & 0xfff;
}

static const uint32_t size_tbl[] = {0 * sizeof(ac_block_t), 1 * sizeof(ac_block_t), 2 * sizeof(ac_block_t), 3 * sizeof(ac_block_t), 4 * sizeof(ac_block_t),
                                    5 * sizeof(ac_block_t), 6 * sizeof(ac_block_t), 7 * sizeof(ac_block_t), 12 * sizeof(ac_block_t), 24 * sizeof(ac_block_t),
                                    32 * sizeof(ac_block_t), 48 * sizeof(ac_block_t), 64 * sizeof(ac_block_t), 96 * sizeof(ac_block_t), 128 * sizeof(ac_block_t),
                                    196 * sizeof(ac_block_t), 256 * sizeof(ac_block_t), 512 * sizeof(ac_block_t), 512 * 2 * sizeof(ac_block_t),
                                    512 * 4 * sizeof(ac_block_t), 512 * 6 * sizeof(ac_block_t), 512 * 8 * sizeof(ac_block_t), 2048 * 6 * sizeof(ac_block_t),
                                    2048 * 8 * sizeof(ac_block_t), 2048 * 12 * sizeof(ac_block_t), 2048 * 16 * sizeof(ac_block_t), 2048 * 24 * sizeof(ac_block_t),
                                    2048 * 32 * sizeof(ac_block_t), 2048 * 48 * sizeof(ac_block_t), 65536 * 2 * sizeof(ac_block_t), 65536 * 3 * sizeof(ac_block_t),
                                    65536 * 4 * sizeof(ac_block_t), 65536 * 8 * sizeof(ac_block_t), 65536 * 12 * sizeof(ac_block_t), 65536 * 16 * sizeof(ac_block_t),
                                    65536 * 24 * sizeof(ac_block_t), 65536 * 32 * sizeof(ac_block_t), 65536 * 48 * sizeof(ac_block_t), 65536 * 64 * sizeof(ac_block_t),
                                    65536 * 96 * sizeof(ac_block_t), 65536 * 128 * sizeof(ac_block_t), 65536 * 196 * sizeof(ac_block_t), 65536 * 256 * sizeof(ac_block_t)};

static inline uint32_t get_block_id(uint32_t size)
{
    if (size <= size_tbl[16])
    {
        if (size <= size_tbl[8])
        {
            if (size <= size_tbl[4])
            {
                if (size <= size_tbl[2])
                {
                    if (size <= size_tbl[0])
                        return 0;
                    else if (size <= size_tbl[1])
                        return 1;
                    else
                        return 2;
                }
                else
                {
                    if (size <= size_tbl[3])
                        return 3;
                    else
                        return 4;
                }
            }
            else
            {
                if (size <= size_tbl[6])
                {
                    if (size <= size_tbl[5])
                        return 5;
                    else
                        return 6;
                }
                else
                {
                    if (size <= size_tbl[7])
                        return 7;
                    else
                        return 8;
                }
            }
        }
        else if (size <= size_tbl[12])
        {
            if (size <= size_tbl[10])
            {
                if (size <= size_tbl[9])
                    return 9;
                else
                    return 10;
            }
            else
            {
                if (size <= size_tbl[11])
                    return 11;
                else
                    return 12;
            }
        }
        else
        {
            if (size <= size_tbl[14])
            {
                if (size <= size_tbl[13])
                    return 13;
                else
                    return 14;
            }
            else
            {
                if (size <= size_tbl[15])
                    return 15;
                else
                    return 16;
            }
        }
    }
    else if (size <= size_tbl[32])
    {
        if (size <= size_tbl[24])
        {
            if (size <= size_tbl[20])
            {
                if (size <= size_tbl[18])
                {
                    if (size <= size_tbl[17])
                        return 17;
                    else
                        return 18;
                }
                else
                {
                    if (size <= size_tbl[19])
                        return 19;
                    else
                        return 20;
                }
            }
            else
            {
                if (size <= size_tbl[22])
                {
                    if (size <= size_tbl[21])
                        return 21;
                    else
                        return 22;
                }
                else
                {
                    if (size <= size_tbl[23])
                        return 23;
                    else
                        return 24;
                }
            }
        }
        else if (size <= size_tbl[28])
        {
            if (size <= size_tbl[26])
            {
                if (size <= size_tbl[25])
                    return 25;
                else
                    return 26;
            }
            else
            {
                if (size <= size_tbl[27])
                    return 27;
                else
                    return 28;
            }
        }
        else
        {
            if (size <= size_tbl[30])
            {
                if (size <= size_tbl[29])
                    return 29;
                else
                    return 30;
            }
            else
            {
                if (size <= size_tbl[31])
                    return 31;
                else
                    return 32;
            }
        }
    }
    else if (size <= size_tbl[33]) // all of these are increasingly unlikely, so scanning is probably most efficient
        return 33;
    else if (size <= size_tbl[34])
        return 34;
    else if (size <= size_tbl[35])
        return 35;
    else if (size <= size_tbl[36])
        return 36;
    else if (size <= size_tbl[37])
        return 37;
    else if (size <= size_tbl[38])
        return 38;
    else if (size <= size_tbl[39])
        return 39;
    else if (size <= size_tbl[40])
        return 40;
    else if (size <= size_tbl[41])
        return 41;
    else if (size <= size_tbl[42])
        return 42;
    else
        abort();
}

struct free_node_t;
struct free_node_head_t;
typedef struct free_node_t free_node_t;
typedef struct free_node_head_t free_node_head_t;

struct free_node_t
{
    free_node_t *next;
};

struct free_node_head_t
{
    free_node_t *next;
};


struct ac_s_idx_s;
typedef struct ac_s_idx_s ac_s_idx_t;
struct ac_s_term_s;
typedef struct ac_s_term_s ac_s_term_t;

struct ac_s_term_s {
    ac_map_t node;
    ac_block_t *groups;
    uint32_t num_groups;
    uint32_t num_ids;
    uint32_t bytes;
    uint32_t max_bytes;
};

uint32_t total_bytes = 0;

static inline
int compare_term_for_find(const char *key, const ac_s_term_t *node) {
    return strcmp(key, (const char *)(node+1));
}

static inline
int compare_term_for_insert(const ac_s_term_t *a, const ac_s_term_t *b) {
    return strcmp((const char *)(a+1), (const char *)(b+1));
}

static ac_map_lower_bound_m(_term_upper, char, ac_s_term_t, compare_term_for_find);
static ac_map_find_m(_term_find, char, ac_s_term_t, compare_term_for_find);
static ac_map_insert_m(_term_insert, ac_s_term_t, compare_term_for_insert);

struct ac_s_idx_s {
    ac_pool_t *pool;
    ac_map_t *root;    
};

struct ac_s_s
{
    free_node_head_t *free_tbl;
    ac_pool_t *pool;
    ac_s_idx_t *idx;
    ac_s_custom_cursor_cb cb;
};

static uint32_t letter_frequencies[26] = { /* A-Z first letter frequencies */
    2435, 2220, 3772, 2501, 1674,
    1715, 1220, 1440, 1911, 319,
    233, 1206, 2048, 850, 992,
    3258, 197, 2091, 4635, 2031,
    1554, 642, 1005, 11, 109, 58
};

ac_s_t *ac_s_init(ac_s_custom_cursor_cb cb)
{
    ac_s_t *h = (ac_s_t *)ac_calloc(sizeof(ac_s_t) + (sizeof(free_node_head_t) * 44) + (sizeof(ac_s_idx_t) * 256));
    h->free_tbl = (free_node_head_t *)(h + 1);
    h->pool = ac_pool_init(65536);
    h->idx = (ac_s_idx_t *)(h->free_tbl + 44);
    h->cb = cb;
    for( uint32_t i=0; i<256; i++ ) {
        if(i >= 'A' && i <= 'Z')
            continue;
        if(i >= 'a' && i <= 'z')
            h->idx[i].pool = ac_pool_init(1024 + (letter_frequencies[i-'a'] * 32));        
        else if(i >= '0' && i <= '9')
            h->idx[i].pool = ac_pool_init(32768);
        else
            h->idx[i].pool = ac_pool_init(1024);
    }
    return h;
}

void ac_s_destroy(ac_s_t *h)
{
    if (!h)
        return;

    size_t size = ac_pool_used(h->pool);
    for( uint32_t i=0; i<256; i++ ) {
        if(h->idx[i].pool) {
            size += ac_pool_used(h->idx[i].pool);
            ac_pool_destroy(h->idx[i].pool);
        }
    }
    ac_pool_destroy(h->pool);
    ac_free(h);
    // printf( "%zu byte(s) allocated\n", size );
}

static void *ac_s_alloc(ac_s_t *h, uint32_t block_id)
{
    if (!h->free_tbl[block_id].next)
        return (void *)ac_pool_alloc(h->pool, size_tbl[block_id]);
    else
    {
        free_node_t *fn = h->free_tbl[block_id].next;
        void *res = (void *)fn;
        h->free_tbl[block_id].next = fn->next;
        return res;
    }
}

static void ac_s_release(ac_s_t *h, void *data, size_t size)
{
    uint32_t block_id = get_block_id(size);
    free_node_t *fn = (free_node_t *)data;
    fn->next = h->free_tbl[block_id].next;
    h->free_tbl[block_id].next = fn;
}

static void ac_s_release_block(ac_s_t *h, void *data)
{
    uint32_t *d = (uint32_t *)data;
    uint32_t size = *d + sizeof(uint32_t);
    ac_s_release(h, data, size);
}


static void ac_s_release_group(ac_s_t *h, ac_block_t *data, size_t num)
{
    ac_s_release(h, data, num * sizeof(*data));
}

static uint32_t ac_s_get_block_id(uint32_t size)
{
    return get_block_id(size);
}

static uint32_t ac_s_get_block_id_size(uint32_t block_id)
{
    return size_tbl[block_id];
}

static ac_block_t *get_add_block(ac_s_t *h, size_t old_size)
{
    old_size *= sizeof(ac_block_t);
    uint32_t block_id = get_block_id(old_size);
    if (old_size + sizeof(ac_block_t) <= size_tbl[block_id])
        return NULL;
    return (ac_block_t *)ac_s_alloc(h, block_id + 1);
}

static ac_block_t *get_remove_block(ac_s_t *h, size_t old_size)
{
    old_size *= sizeof(ac_block_t);
    uint32_t block_id = get_block_id(old_size);
    if (old_size - sizeof(ac_block_t) > size_tbl[block_id-1])
        return NULL;

    return (ac_block_t *)ac_s_alloc(h, block_id - 1);
}


static ac_block_t *remove_group(ac_s_t *h, ac_block_t *match, ac_block_t *groups, uint32_t *num_groups)
{
    if (*num_groups == 1)
    {
        ac_s_release_group(h, groups, 1);
        *num_groups = 0;
        return NULL;
    }
    else
    {
        ac_block_t *new_block = get_remove_block(h, *num_groups);
        *num_groups = (*num_groups) - 1;
        ac_block_t *ep = groups + (*num_groups);
        if (new_block)
        {
            ac_block_t *dp = new_block;
            ac_block_t *sp = groups;
            while (sp < match)
                *dp++ = *sp++;
            sp++;
            while (sp < ep)
                *dp++ = *sp++;

            ac_s_release_group(h, groups, (*num_groups) + 1);
            return new_block;
        }
        else
        {
            if (match + 1 < ep)
                memmove(match, match + 1, sizeof(ac_block_t) * (ep - (match + 1)));
            return groups;
        }
    }
}



static uint8_t *find_id_in_block(uint8_t **nextp, uint8_t *block, uint8_t *ep, uint32_t id) {
    uint32_t _id = 0;
    uint8_t *bp = block;
    while(bp < ep) {
        uint8_t *next = advance_id(bp, &_id);
        if(id > _id) {
            bp = next;
            continue;
        }
        else if(id == _id)
            *nextp = next;
        else
            *nextp = NULL;
        return bp;
    }
    *nextp = NULL;
    return ep;
}

static bool remove_id_from_group(ac_s_t *h, ac_s_term_t *term, ac_block_t *match, uint32_t sub_id) {
    uint8_t *block = match->block;
    uint8_t *ep = block + (*(uint32_t *)(block)) + sizeof(uint32_t);
    uint32_t block_size = ep - block;    
    block += sizeof(uint32_t);

    uint8_t *next = NULL;
    uint8_t *mp = find_id_in_block(&next, block, ep, sub_id);
    if(!next)
        return false;

    term->num_ids--;
    term->bytes -= (next-mp);
    total_bytes -= (next-mp);

    if((ep-block) - (next-mp) < 9 && (block == mp || ep == next)) {
        if(block == mp && ep == next) {
            ac_s_release_block(h, match->block);
            return true;
        }
        else {
            uint8_t *tmp = match->block;
            uint8_t *p = (uint8_t *)match;
            p += 2;
            while(block < mp)
                *p++ = *block++;
            block = next;
            while(block < ep)
                *p++ = *block++;
            match->type = 0;
            ac_s_release_block(h, tmp);
            return false;
        }
    }

    uint32_t old_block_id = get_block_id(block_size);
    uint32_t new_size = block_size - (next - mp);
    uint32_t new_block_id = get_block_id(new_size);
    if (old_block_id == new_block_id) {
        memmove(mp, next, ep-next);
        (*(uint32_t *)(match->block)) = new_size - sizeof(uint32_t);
    }
    else {
        uint8_t *new_block = (uint8_t *)ac_s_alloc(h, new_block_id);
        (*(uint32_t *)(new_block)) = new_size - sizeof(uint32_t);
        new_block += sizeof(uint32_t);
        memcpy(new_block, block, mp - block);
        memcpy(new_block + (mp - block), next, ep - next);
        ac_s_release_block(h, match->block);
        match->block = new_block - sizeof(uint32_t);
    }
    return false;
}

static inline
ac_block_t *find_block(ac_block_t *groups, uint32_t num_groups, uint32_t group_id) {
    ac_block_t *low = groups;
    ac_block_t *ep = groups + num_groups;
    ac_block_t *high = ep;
    ac_block_t *mid;
    while (low < high) {
        mid = low + ((high-low) >> 1);
        if(group_id > mid->group)
            low = mid + 1;
        else
            high = mid;
    }
    if(low == ep)
        return NULL;
    return low;
}

static
ac_block_t *_ac_s_remove(ac_s_t *h, ac_s_term_t *term, ac_block_t *groups, uint32_t *num_groups, uint32_t id)
{
    uint32_t group_id = ac_s_get_group_id(id);
    ac_block_t *match = find_block(groups, *num_groups, group_id);
    if (!match || match->group != group_id)
        return groups;

    uint32_t sub_id = ac_s_get_sub_id(id);
    if (match->type != 0)
    {
        if (remove_id_from_group(h, term, match, sub_id))
            return remove_group(h, match, groups, num_groups);
        else
            return groups;
    }
    else
    {
        uint16_t *ptr = (uint16_t *)(match);
        ptr++;
        uint32_t v = *ptr;
        uint32_t bytes = v & 0x7;
        v >>= 4;
        if (v != sub_id)
            return groups;
        term->num_ids--;
        term->bytes -= (bytes+1);
        total_bytes -= (bytes+1);
        return remove_group(h, match, groups, num_groups);
    }
}

static uint8_t *_ac_s_find(ac_s_t *h, uint8_t **nextp, ac_block_t *groups, uint32_t num_groups, uint32_t id) {
    *nextp = NULL;
    uint32_t group_id = ac_s_get_group_id(id);
    ac_block_t *match = find_block(groups, num_groups, group_id);
    if (!match || match->group != group_id)
        return NULL;

    uint32_t sub_id = ac_s_get_sub_id(id);
    if(match->type != 0) {
        uint8_t *block = match->block;
        uint8_t *ep = block + (*(uint32_t *)(block)) + sizeof(uint32_t);
        block += sizeof(uint32_t);

        uint8_t *next = NULL;
        uint8_t *mp = find_id_in_block(nextp, block, ep, sub_id);
        if(!(*nextp))
            return NULL;
        return mp;
    }
    else {
        uint8_t *mp = (uint8_t *)(match);
        mp += 2;
        uint32_t _id = 0;
        *nextp = advance_id(mp, &_id);
        if(_id != sub_id) {
            *nextp = NULL;
            return NULL;
        }
        return mp;
    }
}


static inline uint8_t *copy_data(uint8_t *ptr, uint32_t sub_id, const uint8_t *data, uint32_t len)
{
    uint16_t *idp = (uint16_t *)ptr;
    ptr += 2;
    uint16_t v = sub_id;
    v <<= 4;
    v |= data[0];
    data++;
    len--;
    *idp = v;
    while (len)
    {
        *ptr++ = *data++;
        len--;
    }

    return ptr;
}

static ac_block_t new_group_with_data(ac_s_t *h, uint32_t id, const uint8_t *data, uint32_t len ) {
    uint32_t group_id = ac_s_get_group_id(id);
    ac_block_t block;
    block.type = 0;
    block.group = group_id;
    block.block = NULL;

    uint32_t sub_id = ac_s_get_sub_id(id);
    uint8_t *bp;
    if(len < 7) {
        bp = (uint8_t *)&block;
        bp += 2;
    }
    else {
        block.type = 1;
        bp = (uint8_t *)ac_s_alloc(h, ac_s_get_block_id(len + 1 + 4));
        (*(uint32_t *)(bp)) = len + 1;
        block.block = bp;
        bp += sizeof(uint32_t);
    }
    copy_data(bp, sub_id, data, len);
    return block;
}

static ac_block_t *insert_new_group_with_data(ac_s_t *h, 
                                       ac_s_term_t *term,
                                       ac_block_t *match, 
                                       ac_block_t *groups,
                                       uint32_t *num_groups,
                                       uint32_t id, const uint8_t *data, uint32_t len ) {
    ac_block_t *ep = groups + (*num_groups);
    ac_block_t nb = new_group_with_data(h, id, data, len);
    if(!match)
        match = ep;

    term->num_ids++;
    term->bytes += (len+1);
    total_bytes += (len+1);
    ac_block_t *new_block = get_add_block(h, *num_groups);
    *num_groups = (*num_groups) + 1;
    if (new_block)
    {
        ac_block_t *dp = new_block;
        ac_block_t *sp = groups;
        while (sp < match)
            *dp++ = *sp++;
        *dp++ = nb;
        while (sp < ep)
            *dp++ = *sp++;

        if(*num_groups > 1)
            ac_s_release_group(h, groups, (*num_groups) - 1);
        return new_block;
    }
    else
    {
        if (match < ep)
            memmove(match+1, match, sizeof(ac_block_t) * (ep - match));
        *match = nb;
        return groups;
    }
}

static
void update_group_with_data(ac_s_t *h, ac_s_term_t *term, ac_block_t *match, uint32_t id, const uint8_t *data, uint32_t len) {
    uint32_t sub_id = ac_s_get_sub_id(id);
    if(match->type != 0) {
        uint8_t *block = match->block;
        uint32_t block_size = (*(uint32_t *)(block)) + sizeof(uint32_t);
        //if(block_size > 1000)
        //    abort();
        uint8_t *ep = block + block_size;      
        block += sizeof(uint32_t);

        uint8_t *next = NULL;
        uint8_t *mp = find_id_in_block(&next, block, ep, sub_id);
        // mp points to where the id should go and next will point to the length if it is a match
        if(len < 7 && block == mp && ep == next) {
            block -= sizeof(uint32_t);
            term->bytes -= (block_size-sizeof(uint32_t));
            term->bytes += len + 1;

            total_bytes -= (block_size-sizeof(uint32_t));
            total_bytes += len + 1;
            
            ac_s_release_block(h, block);
            uint8_t *p = (uint8_t *)match;
            p += 2;
            copy_data(p, sub_id, data, len);
            return;
        }

        if(next) {
            if(next-mp == len+1) {
                copy_data(mp, sub_id, data, len);
                return;
            }
            term->bytes -= (next-mp);
            term->bytes += len + 1;

            total_bytes -= (next-mp);
            total_bytes += len + 1;
        }
        else {
            term->num_ids++;
            term->bytes += len + 1;

            total_bytes += len + 1;
            next = mp;
        }
        uint32_t len2 = next-mp;
        uint32_t old_block_id = get_block_id(block_size);
        uint32_t new_size = block_size + len + 1 - len2;
        uint32_t new_block_id = get_block_id(new_size);
        if (old_block_id == new_block_id) {
            memmove(next, mp+len+1, ep-next);
            copy_data(mp, sub_id, data, len);
            (*(uint32_t *)(match->block)) = new_size - sizeof(uint32_t);
        }
        else {
            uint8_t *new_block = (uint8_t *)ac_s_alloc(h, new_block_id);
            (*(uint32_t *)(new_block)) = new_size - sizeof(uint32_t);
            new_block += sizeof(uint32_t);
            memcpy(new_block, block, mp - block);
            uint8_t *np = new_block;
            np += (mp - block);
            copy_data(np, sub_id, data, len);
            np += len+1;            
            memcpy(np, next, ep-next);
            ac_s_release_block(h, match->block);
            match->block = new_block - sizeof(uint32_t);
        }
    }
    else {
        uint8_t *mp = (uint8_t *)(match);
        mp += 2;
        uint32_t _id = 0;
        uint8_t *next = advance_id(mp, &_id);
        if(_id == sub_id) {
            uint8_t *bp;
            if(len < 7) {
                bp = (uint8_t *)(match);
                bp += 2;
            }
            else {
                match->type = 1;
                uint8_t *bp = (uint8_t *)ac_s_alloc(h, ac_s_get_block_id(len + 1 + 4));
                (*(uint32_t *)(bp)) = len + 1;
                match->block = bp;
                bp += sizeof(uint32_t);
            }
            term->bytes -= next-mp;
            total_bytes -= next-mp;
            term->bytes += len + 1;
            total_bytes += len + 1;
            copy_data(bp, sub_id, data, len);
        }
        else {
            // new block, adding current id
            match->type = 1;
            uint32_t len2 = next - mp;
            uint8_t *bp = (uint8_t *)ac_s_alloc(h, ac_s_get_block_id(len2 + len + 1 + 4));
            (*(uint32_t *)(bp)) = len2 + len + 1;
            bp += sizeof(uint32_t);
            if(_id < sub_id) {
                memcpy(bp, mp, len2);
                copy_data(bp+len2, sub_id, data, len);
            }
            else {
                copy_data(bp, sub_id, data, len);
                bp += len + 1;
                memcpy(bp, mp, len2);
            }
            term->num_ids++;
            term->bytes += len + 1;
            total_bytes += len + 1;
            match->block = bp - sizeof(uint32_t);
        }
    }
}

static ac_block_t *_ac_s_add(ac_s_t *h, ac_s_term_t *term, ac_block_t *groups, uint32_t *num_groups, uint32_t id, const uint8_t *data, uint32_t len) {
    if(term->max_bytes < len-1)
        term->max_bytes = len-1;

    uint32_t group_id = ac_s_get_group_id(id);
    ac_block_t *match = find_block(groups, *num_groups, group_id);
    if (!match || match->group != group_id) {
        // insert new group with data/len
        return insert_new_group_with_data(h, term, match, groups, num_groups, id, data, len);
    }
    else {
        // update current group
        update_group_with_data(h, term, match, id, data, len);
        return groups;
    }
}

void ac_s_erase(ac_s_t *h, const char *term, uint32_t id) {
    uint8_t key = term[0];
    if(key >= 'A' && key <= 'Z')
        key = key - 'A' + 'a';
    ac_s_term_t *t = _term_find(term, h->idx[key].root);
    if(!t)
        return;
    t->groups = _ac_s_remove(h, t, t->groups, &t->num_groups, id);
}

uint8_t *ac_s_find(ac_s_t *h, uint32_t *len, const char *term, uint32_t id) {
    *len = 0;
    uint8_t key = term[0];
    if(key >= 'A' && key <= 'Z')
        key = key - 'A' + 'a';
    ac_s_term_t *t = _term_find(term, h->idx[key].root);
    if(!t)
        return NULL;
    uint8_t *next = NULL;
    uint8_t *res = _ac_s_find(h, &next, t->groups, t->num_groups, id);
    if(next && res) {
        *len = next-res;
        return res;
    }
    return NULL;
}

void ac_s_insert(ac_s_t *h, const char *term, uint32_t id, const void *data, uint32_t len) {
    uint8_t key = term[0];
    if(key >= 'A' && key <= 'Z')
        key = key - 'A' + 'a';
    ac_s_term_t *t = _term_find(term, h->idx[key].root);
    if(!t) {
        t = ac_pool_calloc(h->idx[key].pool, sizeof(*t) + strlen(term) + 1);
        strcpy((char *)(t+1), term);
        _term_insert(t, &h->idx[key].root);    
    }
    t->groups = _ac_s_add(h, t, t->groups, &t->num_groups, id, data, len);
}



uint32_t advance_empty_cursor_to(void *c, uint32_t id)
{
    return 0;
}

uint32_t advance_empty_cursor(void *c)
{
    return 0;
}

ac_s_cursor_t *ac_s_init_empty_cursor(ac_pool_t *pool) {
    ac_s_cursor_t *r = (ac_s_cursor_t *)ac_pool_calloc(pool, sizeof(ac_s_cursor_t));
    r->advance = advance_empty_cursor;
    r->advance_to = advance_empty_cursor_to;
    r->type = 0; 
    return r;
}

struct or_cursor_s;
typedef struct or_cursor_s or_cursor_t;

struct or_cursor_s {
    ac_s_cursor_t cursor;
    ac_s_cursor_t **cursors;
    uint32_t num_cursors;
    uint32_t cursor_size;

    ac_s_cursor_t **active;
    ac_s_cursor_t **heap;
    uint32_t num_active;
    uint32_t num_heap;
};

void or_add_to_cursor( or_cursor_t *dest, ac_s_cursor_t *src ) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    if(dest->num_cursors >= dest->cursor_size) {
        uint32_t num_cursors = (dest->cursor_size+1)*2;
        ac_s_cursor_t **cursors = 
            (ac_s_cursor_t **)ac_pool_alloc(dest->cursor.pool, sizeof(ac_s_cursor_t *) * num_cursors);
        if(dest->num_cursors)
            memcpy(cursors, dest->cursors, dest->num_cursors * sizeof(ac_s_cursor_t *));
        dest->cursor_size = num_cursors;
        dest->cursors = cursors;
    }
    dest->cursors[dest->num_cursors] = src;
    dest->num_cursors++;
}

void or_cursor_push(or_cursor_t *c, ac_s_cursor_t *src) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );
    // ac_s_reset(src);
    ac_s_cursor_t *tmp, **heap = c->heap;

    c->num_heap++;
    uint32_t i=c->num_heap;
    heap[i] = src;
    uint32_t j=i>>1;

    while (j > 0 && heap[i]->current < heap[j]->current) {
        tmp = heap[i];
        heap[i] = heap[j];
        heap[j] = tmp;
        i = j;
        j = j >> 1;
    }
}

ac_s_cursor_t *or_cursor_pop(or_cursor_t *c) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    c->num_heap--;
    ssize_t num = c->num_heap;
    ac_s_cursor_t **heap = c->heap;
    ac_s_cursor_t *r = heap[1];
    heap[1] = heap[num + 1];

    ssize_t i = 1;
    ssize_t j = i << 1;
    ssize_t k = j + 1;

    if (k <= num && heap[k]->current < heap[j]->current)
        j = k;

    while (j <= num && heap[j]->current < heap[i]->current) {
        ac_s_cursor_t *tmp = heap[i];
        heap[i] = heap[j];
        heap[j] = tmp;

        i = j;
        j = i << 1;
        k = j + 1;
        if (k <= num && heap[k]->current < heap[j]->current)
            j = k;
    }
    return r;
}

uint32_t advance_or_cursor(or_cursor_t *c) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    for( uint32_t i=0; i<c->num_active; i++ ) {
        if(c->active[i]->advance(c->active[i]))
            or_cursor_push(c, c->active[i]);
    }
    if(!c->num_heap)
        return ac_s_empty_cursor(&c->cursor);


    c->active[0] = or_cursor_pop(c);
    c->num_active = 1;

    ac_s_cursor_t **heap = c->heap;
    uint32_t current = c->active[0]->current;
    while(c->num_heap && heap[1]->current == current) {
        c->active[c->num_active] = or_cursor_pop(c);
        c->num_active++;
    }

    c->cursor.current = current;
    return current;
}

uint32_t advance_or_cursor_to(or_cursor_t *c, uint32_t id) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    if (id <= c->cursor.current)
        return c->cursor.current;

    for( uint32_t i=0; i<c->num_active; i++ ) {
        if(c->active[i]->advance_to(c->active[i], id))
            or_cursor_push(c, c->active[i]);
    }
    if(!c->num_heap)
        return ac_s_empty_cursor(&c->cursor);


    c->active[0] = or_cursor_pop(c);
    c->num_active = 1;

    ac_s_cursor_t **heap = c->heap;
    uint32_t current = c->active[0]->current;
    while(c->num_heap && heap[1]->current == current) {
        c->active[c->num_active] = or_cursor_pop(c);
        c->num_active++;
    }

    c->cursor.current = current;
    return current;
}


uint32_t advance_or_cursor_init(or_cursor_t *c) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    uint32_t num = c->num_cursors;

    c->heap = (ac_s_cursor_t **)ac_pool_alloc(c->cursor.pool, sizeof(ac_s_cursor_t *) * (num+1) * 2);
    c->active = c->heap + (num+1);
    c->num_heap = 0;
    c->num_active = 0;

    for( uint32_t k=0; k<c->num_cursors; k++ ) {
        if(c->cursors[k]->advance(c->cursors[k])) {
            // ac_s_reset(c->cursors[k]);
            or_cursor_push(c, c->cursors[k]);
        }
    }
    
    if(!c->num_heap)
        return ac_s_empty_cursor(&c->cursor);

    c->cursor.advance = (ac_s_advance_cb)advance_or_cursor;
    c->cursor.advance_to = (ac_s_advance_to_cb)advance_or_cursor_to;

    return advance_or_cursor(c);
}

uint32_t advance_or_cursor_to_init(or_cursor_t *c, uint32_t id) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    uint32_t r=advance_or_cursor_init(c);
    if(r >= id)
        return r;
    if(!r)
         return ac_s_empty_cursor(&c->cursor);


    return c->cursor.advance_to(c, id);
}    

static
void init_or_cursor(ac_pool_t *pool, or_cursor_t *r) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    uint32_t num_cursors = 2;
    r->cursor.pool = pool;
    r->cursor.type = 11; 
    r->cursor.advance = (ac_s_advance_cb)advance_or_cursor_init;
    r->cursor.advance_to = (ac_s_advance_to_cb)advance_or_cursor_to_init;
    r->cursor.add_to_cursor = (ac_s_add_to_cursor_cb)or_add_to_cursor;
    r->cursors = (ac_s_cursor_t **)ac_pool_alloc(pool, sizeof(ac_s_cursor_t *) * num_cursors);
    r->num_cursors = 0;
    r->cursor_size = num_cursors;
}

ac_s_cursor_t *ac_s_init_or_cursor(ac_pool_t *pool) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    or_cursor_t *r = (or_cursor_t *)ac_pool_calloc(pool, sizeof(or_cursor_t));
    init_or_cursor(pool, r);
    return (ac_s_cursor_t *)r;
}

struct or_term_cursor_s;
typedef struct or_term_cursor_s or_term_cursor_t;

struct or_term_cursor_s {
    or_cursor_t oc;

    uint16_t *term_positions;
};


struct block_cursor_s;
typedef struct block_cursor_s block_cursor_t;

struct block_cursor_s
{
    ac_s_cursor_t cursor;
    ac_s_term_t *term;
    char *key;
    ac_block_t *groups;
    ac_block_t *eg;
    ac_block_t *cur;
    uint32_t gid;
    uint8_t *p;
    uint8_t *ep;
};

void fill_term_positions(or_term_cursor_t *c) {
    ac_s_cursor_t **active = (ac_s_cursor_t **)(c->oc.active);
    uint32_t num_active = c->oc.num_active;
    if(num_active == 1) {        
        c->oc.cursor.wp = active[0]->wp;
        c->oc.cursor.ewp = active[0]->ewp;
    }
    else if(num_active >= 2) {
        uint16_t *p1 = active[0]->wp;
        uint16_t *ep1 = active[0]->ewp;
        uint16_t *p2 = active[1]->wp;
        uint16_t *ep2 = active[1]->ewp;
        uint16_t *wp = c->term_positions;
        c->oc.cursor.wp = wp;
        if(p1 < ep1 && p2 < ep2) {
            while(true) {
                if(*p1 != *p2) {
                    if(*p1 < *p2) {
                        *wp++ = *p1++;
                        if(p1 >= ep1)
                            break;
                    }
                    else {
                        *wp++ = *p2++;
                        if(p2 >= ep2)
                            break;
                    }
                }
                else {
                    *wp++ = *p1++;
                    p2++;
                    if(p1 >= ep1 || p2 >= ep2)
                        break;
                }
            }
        }
        while(p1 < ep1)
            *wp++ = *p1++;            
        while(p2 < ep2)
            *wp++ = *p2++;            
        c->oc.cursor.ewp = wp;
    }
    /*
    implement 3 or more word heap/sort
    else {

    }*/
}


uint32_t advance_or_term_cursor(or_term_cursor_t *oc) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    or_cursor_t *c = (or_cursor_t *)oc;

    for( uint32_t i=0; i<c->num_active; i++ ) {
        if(c->active[i]->advance(c->active[i]))
            or_cursor_push(c, c->active[i]);
    }
    if(!c->num_heap)
        return ac_s_empty_cursor(&c->cursor);


    c->active[0] = or_cursor_pop(c);
    c->num_active = 1;

    ac_s_cursor_t **heap = c->heap;
    uint32_t current = c->active[0]->current;
    while(c->num_heap && heap[1]->current == current) {
        c->active[c->num_active] = or_cursor_pop(c);
        c->num_active++;
    }

    c->cursor.current = current;
    fill_term_positions(oc);
    return current;
}

uint32_t advance_or_term_cursor_to(or_term_cursor_t *oc, uint32_t id) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    or_cursor_t *c = (or_cursor_t *)oc;
    if (id <= c->cursor.current)
        return c->cursor.current;

    for( uint32_t i=0; i<c->num_active; i++ ) {
        if(c->active[i]->advance_to(c->active[i], id))
            or_cursor_push(c, c->active[i]);
    }
    if(!c->num_heap)
        return ac_s_empty_cursor(&c->cursor);


    c->active[0] = or_cursor_pop(c);
    c->num_active = 1;

    ac_s_cursor_t **heap = c->heap;
    uint32_t current = c->active[0]->current;
    while(c->num_heap && heap[1]->current == current) {
        c->active[c->num_active] = or_cursor_pop(c);
        c->num_active++;
    }

    c->cursor.current = current;
    fill_term_positions(oc);
    return current;
}


uint32_t advance_or_term_cursor_init(or_term_cursor_t *oc) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    or_cursor_t *c = (or_cursor_t *)oc;

    uint32_t num = c->num_cursors;

    c->heap = (ac_s_cursor_t **)ac_pool_alloc(c->cursor.pool, sizeof(ac_s_cursor_t *) * (num+1) * 2);
    c->active = c->heap + (num+1);
    c->num_heap = 0;
    c->num_active = 0;

    for( uint32_t k=0; k<c->num_cursors; k++ ) {
        if(c->cursors[k]->advance(c->cursors[k])) {
            // ac_s_reset(c->cursors[k]);
            or_cursor_push(c, c->cursors[k]);
        }
    }
    
    if(!c->num_heap)
        return ac_s_empty_cursor(&c->cursor);

    c->cursor.advance = (ac_s_advance_cb)advance_or_term_cursor;
    c->cursor.advance_to = (ac_s_advance_to_cb)advance_or_term_cursor_to;

    return advance_or_term_cursor(oc);
}

uint32_t advance_or_term_cursor_to_init(or_term_cursor_t *oc, uint32_t id) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    or_cursor_t *c = (or_cursor_t *)oc;

    uint32_t r=advance_or_term_cursor_init(oc);
    if(r >= id) {
        return r;
    }
    if(!r)
        return ac_s_empty_cursor(&c->cursor);


    return c->cursor.advance_to(c, id);
}    

ac_s_cursor_t *ac_s_init_or_term_cursor(ac_pool_t *pool) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    or_term_cursor_t *r = (or_term_cursor_t *)ac_pool_calloc(pool, sizeof(or_term_cursor_t));
    init_or_cursor(pool, &r->oc);
    r->oc.cursor.advance = (ac_s_advance_cb)advance_or_term_cursor_init;
    r->oc.cursor.advance_to = (ac_s_advance_to_cb)advance_or_term_cursor_to_init;
    r->oc.cursor.type = 12;
    r->term_positions = ac_pool_calloc(pool, sizeof(uint32_t) * 65536);
    return (ac_s_cursor_t *)r;
}

struct not_cursor_s;
typedef struct not_cursor_s not_cursor_t;

struct not_cursor_s {
    ac_s_cursor_t cursor;
    ac_s_cursor_t *pos;
    ac_s_cursor_t *neg;
};

uint32_t advance_pos_cursor(not_cursor_t *c) {
    uint32_t id=c->pos->advance(c->pos);
    c->cursor.current=id;
    if(id)
        return id;
    c->cursor.advance_to = advance_empty_cursor_to;
    c->cursor.advance = advance_empty_cursor;
    return 0;
}


uint32_t advance_pos_cursor_to(not_cursor_t *c, uint32_t _id) {
    if (_id <= c->cursor.current)
        return c->cursor.current;

    uint32_t id=c->pos->advance_to(c->pos, _id);
    c->cursor.current=id;
    if(id)
        return id;
    return ac_s_empty_cursor(&c->cursor);
}


uint32_t advance_not_cursor(not_cursor_t *c)
{
    while(true) {
        uint32_t id=c->pos->advance(c->pos);
        if(!id)
            return ac_s_empty_cursor(&c->cursor);
        uint32_t id2=c->neg->advance_to(c->neg, id);
        if(id==id2)
            continue;
        c->cursor.current = id;
        if(id2)
            return id;
        c->cursor.advance_to = (ac_s_advance_to_cb)advance_pos_cursor_to;
        c->cursor.advance = (ac_s_advance_cb)advance_pos_cursor;
        return id;        
    }
}

uint32_t advance_not_cursor_to(not_cursor_t *c, uint32_t _id)
{
    if (_id <= c->cursor.current)
        return c->cursor.current;

    uint32_t id=c->pos->advance_to(c->pos, _id);
    if(!id)
        return ac_s_empty_cursor(&c->cursor);
    uint32_t id2=c->neg->advance_to(c->neg, id);
    if(id==id2)
        return advance_not_cursor(c);

    c->cursor.current = id;
    if(id2)
        return id;
    c->cursor.advance_to = (ac_s_advance_to_cb)advance_pos_cursor_to;
    c->cursor.advance = (ac_s_advance_cb)advance_pos_cursor;
    return id;        
}


ac_s_cursor_t *ac_s_init_not_cursor(ac_pool_t *pool, 
                                    ac_s_cursor_t *pos, 
                                    ac_s_cursor_t *neg ) {
    if(!pos)
        return ac_s_init_empty_cursor(pool);
    else if(!neg)
        return pos;
    not_cursor_t *r = (not_cursor_t *)ac_pool_calloc(pool, sizeof(not_cursor_t));
    r->cursor.pool = pool;
    r->cursor.type = 13; 
    r->cursor.advance = (ac_s_advance_cb)advance_not_cursor;
    r->cursor.advance_to = (ac_s_advance_to_cb)advance_not_cursor_to;
    r->pos = pos;
    r->neg = neg;
    return (ac_s_cursor_t *)r;
}

struct and_cursor_s;
typedef struct and_cursor_s and_cursor_t;

struct and_cursor_s {
    ac_s_cursor_t cursor;
    ac_s_cursor_t **cursors;
    uint32_t num_cursors;
    uint32_t cursor_size;
};

void and_add_to_cursor( and_cursor_t *dest, ac_s_cursor_t *src ) {
    if(dest->num_cursors >= dest->cursor_size) {
        uint32_t num_cursors = (dest->cursor_size+1)*2;
        ac_s_cursor_t **cursors = 
            (ac_s_cursor_t **)ac_pool_alloc(dest->cursor.pool, sizeof(ac_s_cursor_t *) * num_cursors);
        if(dest->num_cursors)
            memcpy(cursors, dest->cursors, dest->num_cursors * sizeof(ac_s_cursor_t *));
        dest->cursor_size = num_cursors;
        dest->cursors = cursors;
    }
    dest->cursors[dest->num_cursors] = src;
    dest->num_cursors++;
}


uint32_t advance_and_cursor(and_cursor_t *c)
{
    uint32_t i=1;
    uint32_t id = c->cursors[0]->advance(c->cursors[0]);
    while(id && i < c->num_cursors) {
        uint32_t id2 = c->cursors[i]->advance_to(c->cursors[i], id);
        if(id==id2)
            i++;
        else {
            i=0;
            id=id2;
        }
    }
    if(!id)
        return ac_s_empty_cursor(&c->cursor);
    c->cursor.current = id;
    return id;
}

uint32_t advance_and_cursor_to(and_cursor_t *c, uint32_t id)
{
    if (id <= c->cursor.current)
        return c->cursor.current;

    uint32_t i=0;
    while(id && i < c->num_cursors) {
        uint32_t id2 = c->cursors[i]->advance_to(c->cursors[i], id);
        if(id==id2)
            i++;
        else {
            i=0;
            id=id2;
        }
    }
    if(!id)
        return ac_s_empty_cursor(&c->cursor);

    c->cursor.current = id;
    return id;
}

ac_s_cursor_t *ac_s_init_and_cursor(ac_pool_t *pool) {
    uint32_t num_cursors = 2;
    and_cursor_t *r = (and_cursor_t *)ac_pool_calloc(pool, sizeof(and_cursor_t));
    r->cursor.pool = pool;
    r->cursor.type = 10; 
    r->cursor.advance = (ac_s_advance_cb)advance_and_cursor;
    r->cursor.advance_to = (ac_s_advance_to_cb)advance_and_cursor_to;
    r->cursor.add_to_cursor = (ac_s_add_to_cursor_cb)and_add_to_cursor;
    r->cursors = (ac_s_cursor_t **)ac_pool_alloc(pool, sizeof(ac_s_cursor_t *) * num_cursors);
    r->num_cursors = 0;
    r->cursor_size = num_cursors;
    return (ac_s_cursor_t *)r;
}

struct filter_cursor_s;
typedef struct filter_cursor_s filter_cursor_t;

typedef bool (*filter_cursor_cb)(filter_cursor_t *f);

struct filter_cursor_s
{
    ac_s_cursor_t cursor;
    ac_s_cursor_t *c;

    // min_position_filter
    uint16_t min_position;

    // max_position_filter
    uint16_t max_position;

    // equal_position_filter
    uint32_t position;

    // min_frequency_filter
    uint32_t min_frequency;

    // max_frequency_filter
    uint32_t max_frequency;

    // equal_frequency_filter
    uint32_t frequency;
    
    ac_buffer_t *filter_cbs_bh;

    filter_cursor_cb *filters;
    filter_cursor_cb *ef;
};


uint32_t filter_single_advance(filter_cursor_t *c) {
    while(true) {
        if(!c->c->advance(c->c))
            return ac_s_empty_cursor(&c->cursor);

        filter_cursor_cb *f = c->filters;
        if(f[0](c)) {
            c->cursor.current = c->c->current;
            c->cursor.wp = c->c->wp;
            c->cursor.ewp = c->c->ewp;
            return c->cursor.current;
        }
    }    
}

uint32_t filter_single_advance_to(filter_cursor_t *c, uint32_t id) {
    uint32_t id2=c->c->advance_to(c->c, id);
    if(!id2)
        return ac_s_empty_cursor(&c->cursor);
    filter_cursor_cb *f = c->filters;
    if(f[0](c)) {
        c->cursor.current = id2;
        c->cursor.wp = c->c->wp;
        c->cursor.ewp = c->c->ewp;
        return id2;
    }
    return filter_single_advance(c);
}

uint32_t filter_multi_advance(filter_cursor_t *c) {
    filter_cursor_cb *f;
    while(true) {
loop_start:
        if(!c->c->advance(c->c))
            return ac_s_empty_cursor(&c->cursor);

        f = c->filters;
        while(f < c->ef) {
            if(!f[0](c))
                goto loop_start;
            f++;
        }
        c->cursor.current = c->c->current;
        c->cursor.wp = c->c->wp;
        c->cursor.ewp = c->c->ewp;
        return c->cursor.current;
    }
}

uint32_t filter_multi_advance_to(filter_cursor_t *c, uint32_t id) {
    filter_cursor_cb *f;
    uint32_t id2=c->c->advance_to(c->c, id);
    if(!id2)
        return ac_s_empty_cursor(&c->cursor);

    f = c->filters;
    while(f < c->ef) {
        if(!f[0](c))
            goto do_advance;
        f++;
    }
    c->cursor.current = id2;
    c->cursor.wp = c->c->wp;
    c->cursor.ewp = c->c->ewp;
    return id2;
do_advance:
    return filter_multi_advance(c);
}

ac_s_cursor_t *ac_s_filter_cursor(ac_s_cursor_t *c) {
    if(c->type == 2) {
        // filtering a filter cursor is unsupported (maybe)???
        return c;
    }
    ac_pool_t *pool = c->pool;
    filter_cursor_t *r = (filter_cursor_t *)ac_pool_calloc(pool, sizeof(*r));
    r->c = c;

    r->cursor.pool = pool;
    r->cursor.current = 0;
    r->cursor.type = 2;
   
    r->filter_cbs_bh = ac_buffer_pool_init(pool, 64);
    return (ac_s_cursor_t *)r;
}

bool filter_min_position(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    uint16_t *wp = b->wp;
    while(wp < b->ewp && *wp < f->min_position)
        wp++;
    return wp < b->ewp ? true : false;
}

bool filter_max_position(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    uint16_t *ewp = b->ewp;
    while(ewp > b->wp && ewp[-1] > f->max_position)
        ewp--;
    return b->wp < ewp ? true : false;
}

bool filter_position(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    uint16_t *wp = b->wp;
    while(wp < b->ewp && *wp < f->position)
        wp++;
    if(wp < b->ewp && *wp == f->position)
        return true;
    return false;
}

bool filter_range_position(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    uint16_t *wp = b->wp;
    uint16_t *ewp = b->ewp;
    while(wp < ewp && *wp < f->min_position)
        wp++;
    while(ewp > wp && ewp[-1] > f->max_position)
        ewp--;
    return wp < ewp ? true : false;
}

bool filter_min_frequency(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    return b->ewp-b->wp >= f->min_frequency ? true : false;
}

bool filter_max_frequency(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    return b->ewp - b->wp <= f->max_frequency ? true : false;
}

bool filter_frequency(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    return b->ewp-b->wp == f->frequency ? true : false;
}

bool filter_range_frequency(filter_cursor_t *f) {
    ac_s_cursor_t *b = f->c;
    size_t len = b->ewp-b->wp;
    return len >= f->min_frequency && len <= f->max_frequency ? true : false;
}

void filter_add(filter_cursor_t *f, filter_cursor_cb cb) {
    ac_buffer_append(f->filter_cbs_bh, &cb, sizeof(cb));
    f->filters = (filter_cursor_cb *)ac_buffer_data(f->filter_cbs_bh);
    f->ef = (filter_cursor_cb *)ac_buffer_end(f->filter_cbs_bh);
    if(f->ef - f->filters == 1) {
        f->cursor.advance_to = (ac_s_advance_to_cb)filter_single_advance_to;
        f->cursor.advance = (ac_s_advance_cb)filter_single_advance;
    }
    else {
        f->cursor.advance_to = (ac_s_advance_to_cb)filter_multi_advance_to;
        f->cursor.advance = (ac_s_advance_cb)filter_multi_advance;
    }
}

ac_s_cursor_t *ac_s_filter_min_position(ac_s_cursor_t *c, uint16_t min_position) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->min_position = min_position;
    filter_add(f, &filter_min_position);
    return r;
}

ac_s_cursor_t *ac_s_filter_max_position(ac_s_cursor_t *c, uint16_t max_position) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->max_position = max_position;
    filter_add(f, filter_max_position);
    return r;
}

ac_s_cursor_t *ac_s_filter_position(ac_s_cursor_t *c, uint16_t position) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->position = position;
    filter_add(f, filter_position);
    return r;
}

ac_s_cursor_t *ac_s_filter_range_position(ac_s_cursor_t *c, 
                                          uint16_t min_position,
                                          uint16_t max_position) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->min_position = min_position;
    f->max_position = max_position;
    filter_add(f, filter_range_position);
    return r;
}

ac_s_cursor_t *ac_s_filter_min_frequency(ac_s_cursor_t *c, uint32_t min_frequency) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->min_frequency = min_frequency;
    filter_add(f, &filter_min_frequency);
    return r;
}

ac_s_cursor_t *ac_s_filter_max_frequency(ac_s_cursor_t *c, uint32_t max_frequency) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->max_frequency = max_frequency;
    filter_add(f, filter_max_frequency);
    return r;
}

ac_s_cursor_t *ac_s_filter_frequency(ac_s_cursor_t *c, uint32_t frequency) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->frequency = frequency;
    filter_add(f, filter_frequency);
    return r;
}

ac_s_cursor_t *ac_s_filter_range_frequency(ac_s_cursor_t *c, 
                                          uint32_t min_frequency,
                                          uint32_t max_frequency) {
    if(c->type != 1 && c->type != 12) return c; // only supported type at the moment

    ac_s_cursor_t *r = ac_s_filter_cursor(c);
    filter_cursor_t *f = (filter_cursor_t *)r;
    f->min_frequency = min_frequency;
    f->max_frequency = max_frequency;
    filter_add(f, filter_range_frequency);
    return r;
}

static inline void advance_group(block_cursor_t *sc)
{
    sc->gid = sc->cur->group;
    sc->gid <<= 12;
    if (sc->cur->type != 0)
    {
        uint8_t *block = sc->cur->block;
        uint32_t len = *(uint32_t *)(block);
        block += sizeof(uint32_t);
        sc->p = block;
        sc->ep = block + len;
    }
    else
    {
        uint8_t *block = (uint8_t *)(sc->cur);
        block += 2;
        sc->p = block;
        uint32_t tmp = 0;
        sc->ep = advance_id(block, &tmp);
    }
    sc->cur++;
}

uint32_t ac_s_empty_cursor(ac_s_cursor_t *c) {
    c->advance_to = advance_empty_cursor_to;
    c->advance = advance_empty_cursor;
    c->current = 0;
    return 0;
}


uint32_t advance_block_cursor(block_cursor_t *sc)
{
    if (sc->p >= sc->ep)
    {
        if (sc->cur >= sc->eg)
            return ac_s_empty_cursor(&sc->cursor);
        advance_group(sc);
    }
    uint32_t res = 0;
    uint8_t *wp;
    sc->p = advance_pos_id(sc->p, &wp, &res);
    sc->cursor.wp = (uint16_t *)wp;
    sc->cursor.ewp = (uint16_t *)sc->p;
    sc->cursor.current = res + sc->gid;
    return sc->cursor.current;
}

uint32_t advance_block_cursor_to(ac_s_cursor_t *c, uint32_t id)
{
    if (id <= c->current)
        return c->current;
    block_cursor_t *sc = (block_cursor_t *)c;
    uint32_t gid = id & 0x7fff000;
    while(sc->gid < gid) {
        if (sc->cur >= sc->eg)
            return ac_s_empty_cursor(c);
        advance_group(sc);
    }

    if (sc->p >= sc->ep)
    {
        if (sc->cur >= sc->eg)
            return ac_s_empty_cursor(c);
        advance_group(sc);
    }
    else if (sc->gid == gid) {
        id = id & 0xfff;
        uint32_t cid = peek_id(sc->p);
        while (cid < id)
        {
            sc->p = advance_id(sc->p, &cid);
            if (sc->p >= sc->ep)
                break;
            cid = peek_id(sc->p);
        }
        if (sc->p >= sc->ep)
        {
            if (sc->cur >= sc->eg)
                return ac_s_empty_cursor(c);
            advance_group(sc);
        }
    }
    uint32_t res = 0;
    uint8_t *wp;
    sc->p = advance_pos_id(sc->p, &wp, &res);
    c->wp = (uint16_t *)wp;
    c->ewp = (uint16_t *)sc->p;
    c->current = res + sc->gid;
    return c->current;
}

uint32_t post_reset_advance(ac_s_cursor_t *c) {
    c->advance = c->_advance;
    return c->current;
}

void ac_s_reset(ac_s_cursor_t *c) {
    c->_advance = c->advance;
    c->advance = (ac_s_advance_cb)post_reset_advance;
}

ac_s_cursor_t *_init_term_cursor(ac_pool_t *pool, ac_s_term_t *t) {
    block_cursor_t *r = (block_cursor_t *)ac_pool_calloc(pool, sizeof(block_cursor_t));
    r->cursor.pool = pool;
    r->cursor.current = 0;
    r->cursor.type = 1;

    r->groups = t->groups;
    r->eg = t->groups + t->num_groups;
    r->cur = t->groups;
    r->gid = 0;
    r->p = NULL;
    r->ep = NULL;

    r->term = t;
    r->cursor.advance = (ac_s_advance_cb)advance_block_cursor;
    r->cursor.advance_to = (ac_s_advance_to_cb)advance_block_cursor_to;

    r->cursor.advance((ac_s_cursor_t *)r);
    ac_s_reset((ac_s_cursor_t *)r);
    
    return (ac_s_cursor_t *)r;
}

ac_s_cursor_t *ac_s_init_wild_cursor(ac_pool_t *pool, ac_s_t *h, const char *term) {
    uint8_t key = term[0];
    if(key >= 'A' && key <= 'Z')
        key = key - 'A' + 'a';
    ac_s_term_t *t = _term_upper(term, h->idx[key].root);
    if(!t || strncmp((char *)(t+1), term, strlen(term))) {
        return ac_s_init_empty_cursor(pool);
    }

    ac_s_cursor_t *r = ac_s_init_or_term_cursor(pool);
    ac_map_t *iter = &t->node;
    do {
        t = (ac_s_term_t *)iter;
        if(strncmp((char *)(t+1), term, strlen(term)))
            break;
        r->add_to_cursor(r, _init_term_cursor(pool, t));
    }
    while((iter = ac_map_next(iter)));

    or_cursor_t *oc = (or_cursor_t *)r;
    if(oc->num_cursors == 1)
        return oc->cursors[0];
    
    return r;
}

ac_s_cursor_t *ac_s_init_term_cursor(ac_pool_t *pool, ac_s_t *h, ac_token_t *token, void *arg)
{
    if(h->cb) {
        ac_s_cursor_t *res = h->cb(pool, h, token, arg);
        if(res)
            return res;
    }
    const char *term = token->token;
    if(term[strlen(term)-1] == '*') {
        char *s = ac_pool_strdup(pool, term);
        s[strlen(s)-1] = 0;
        return ac_s_init_wild_cursor(pool, h, s);
    }
    uint8_t key = term[0];
    if(key >= 'A' && key <= 'Z')
        key = key - 'A' + 'a';
    ac_s_term_t *t = _term_find(term, h->idx[key].root);
    if(!t) {
        // empty cursor or NULL?
        return ac_s_init_empty_cursor(pool);
    }

    return _init_term_cursor(pool, t);
}


ac_s_cursor_t ** ac_s_sub_cursors(ac_s_cursor_t *c, uint32_t *num_sub ) {
    if(c->type == 11 || c->type == 12) {
        or_cursor_t *r = (or_cursor_t *)c;
        *num_sub = r->num_active;
        return r->active;
    }
    else if(c->type == 10) {
        and_cursor_t *r = (and_cursor_t *)c;
        *num_sub = r->num_cursors;
        return r->cursors;
    }
    else if(c->type == 2) {
        filter_cursor_t *f = (filter_cursor_t *)c;
        return ac_s_sub_cursors(f->c, num_sub);
    }
    *num_sub = 0;
    return NULL;
}


struct phrase_cursor_s;
typedef struct phrase_cursor_s phrase_cursor_t;

struct phrase_cursor_s {
    ac_s_cursor_t cursor;
    ac_s_cursor_t **cursors;
    uint32_t num_cursors;
    uint32_t cursor_size;
    uint16_t *positions;
    uint16_t ***wp;
    uint16_t ***ewp;
    uint16_t **tmp_wp;
};

void phrase_add_to_cursor( phrase_cursor_t *dest, ac_s_cursor_t *src ) {
    if(dest->num_cursors >= dest->cursor_size) {
        uint32_t num_cursors = (dest->cursor_size+1)*2;
        ac_s_cursor_t **cursors = 
            (ac_s_cursor_t **)ac_pool_alloc(dest->cursor.pool, 
                (sizeof(ac_s_cursor_t *) * num_cursors) +
                (sizeof(uint16_t **) * num_cursors * 4) );
        if(dest->num_cursors) {
            memcpy(cursors, dest->cursors, dest->num_cursors * sizeof(ac_s_cursor_t *));
            memcpy((uint16_t ***)(cursors + num_cursors), dest->wp, dest->num_cursors * (sizeof(uint16_t **)));
            memcpy((uint16_t ***)(cursors + (num_cursors+num_cursors)), dest->ewp, dest->num_cursors * (sizeof(uint16_t **)));            
        }
        dest->cursor_size = num_cursors;
        dest->cursors = cursors;
        dest->wp = (uint16_t ***)cursors + num_cursors;
        dest->ewp = dest->wp + num_cursors;
        dest->tmp_wp = (uint16_t **)dest->ewp + num_cursors;
    }
    dest->cursors[dest->num_cursors] = src;
    dest->wp[dest->num_cursors] = &src->wp;
    dest->ewp[dest->num_cursors] = &src->ewp;
    dest->num_cursors++;
}

static inline
void copy_phrase_pointers(phrase_cursor_t *c) {
    uint16_t ***wp = c->wp;
    uint16_t ***ep = wp + c->num_cursors;
    uint16_t **tmp_wp = c->tmp_wp;
    while(wp < ep) {
        *tmp_wp = **wp;
        tmp_wp++;
        wp++;
    }
    wp = c->ewp;
    ep = wp + c->num_cursors;
    while(wp < ep) {
        *tmp_wp = **wp;
        tmp_wp++;
        wp++;
    }
}

bool find_phrases(phrase_cursor_t *c) {
    uint16_t *dp = c->positions;
    copy_phrase_pointers(c);
    uint16_t **wp = c->tmp_wp;
    size_t num_c = c->num_cursors;
    uint16_t **ewp = wp+num_c;
    size_t i;
    uint16_t min_pos;
    // printf( "%s\n", ((block_cursor_t *)(c->cursors[0]))->key );
    while(wp[0] < ewp[0]) {
        // printf( "%s %u\n", ((block_cursor_t *)(c->cursors[0]))->key, **wp );
        min_pos = **wp;
        i = 1;
        while(i<num_c) {            
            while(wp[i] < ewp[i] && *wp[i] < min_pos+i)
                wp[i]++;
            if(wp[i] >= ewp[i])
                return dp > c->positions ? true : false;            
            // printf( "B> %s %u\n", ((block_cursor_t *)(c->cursors[i]))->key, *wp[i] );
            if(*wp[i] > min_pos+i) {
                min_pos = *wp[i] - i;
                i=0;
                continue;
            }
            // printf( "C> %s %u\n", ((block_cursor_t *)(c->cursors[i]))->key, *wp[i] );
            i++;
        }
        *dp++ = min_pos;
        wp[0]++;
    }
    return dp > c->positions ? true : false;            
}

uint32_t advance_phrase_cursor(phrase_cursor_t *c)
{
    while(true) {
        uint32_t i=1;
        uint32_t id = c->cursors[0]->advance(c->cursors[0]);
        while(id && i < c->num_cursors) {
            uint32_t id2 = c->cursors[i]->advance_to(c->cursors[i], id);
            if(id==id2)
                i++;
            else {
                i=0;
                id=id2;
            }
        }
        if(!id)
            return ac_s_empty_cursor(&c->cursor);

        if(!find_phrases(c))
            continue;
        c->cursor.current = id;
        return id;
    }
}

uint32_t advance_phrase_cursor_to(phrase_cursor_t *c, uint32_t id)
{
    if (id <= c->cursor.current)
        return c->cursor.current;

    uint32_t i, id2;
    i=0;
    while(id && i < c->num_cursors) {
        id2 = c->cursors[i]->advance_to(c->cursors[i], id);
        if(id==id2)
            i++;
        else {
            i=0;
            id=id2;
        }
    }
    if(!id)
        return ac_s_empty_cursor(&c->cursor);
    if(!find_phrases(c))
        return advance_phrase_cursor(c);
    c->cursor.current = id;
    return id;
}

ac_s_cursor_t *ac_s_init_phrase_cursor(ac_pool_t *pool) {
    uint32_t num_cursors = 0;
    phrase_cursor_t *r = (phrase_cursor_t *)ac_pool_calloc(pool, sizeof(phrase_cursor_t));
    r->cursor.pool = pool;
    r->cursor.type = 10; 
    r->cursor.advance = (ac_s_advance_cb)advance_phrase_cursor;
    r->cursor.advance_to = (ac_s_advance_to_cb)advance_phrase_cursor_to;
    r->cursor.add_to_cursor = (ac_s_add_to_cursor_cb)phrase_add_to_cursor;
    r->cursors = NULL;
    r->num_cursors = 0;
    r->cursor_size = num_cursors;
    r->positions = (uint16_t *)ac_pool_alloc(pool, sizeof(uint16_t) * 65536);
    return (ac_s_cursor_t *)r;
}

ac_s_cursor_t *_ac_s_open_cursor(ac_pool_t *pool, ac_s_t *search, ac_token_t *t, void *arg) {
    if(t->child) {
        if(t->type == AC_TOKEN_OPEN_PAREN) {
            ac_s_cursor_t *resp = ac_s_init_and_cursor(pool);
            ac_token_t *n = t->child;
            while(n) {
                ac_s_cursor_t *c = _ac_s_open_cursor(pool, search, n, arg);
                if(c && c->type != 0)
                    resp->add_to_cursor(resp, c);
                else
                    return NULL;
                n = n->next;
            }
            return resp;
        }
        else if(t->type == AC_TOKEN_OR) {
            ac_s_cursor_t *resp = ac_s_init_or_cursor(pool);
            ac_token_t *n = t->child;
            while(n) {
                ac_s_cursor_t *c = _ac_s_open_cursor(pool, search, n, arg);
                if(c)
                    resp->add_to_cursor(resp, c);
                n = n->next;
            }
            return resp;
        }
        else if(t->type == AC_TOKEN_QUOTE || t->type == AC_TOKEN_DQUOTE) {
            ac_s_cursor_t *resp = ac_s_init_phrase_cursor(pool);
            ac_token_t *n = t->child;
            while(n) {
                ac_s_cursor_t *c = _ac_s_open_cursor(pool, search, n, arg);
                if(c && c->type != 0)
                    resp->add_to_cursor(resp, c);
                else
                    return NULL;                
                n = n->next;
            }
            return resp;
        }
        else if(t->type == AC_TOKEN_NOT) {
            if(t->child && t->child->next) {
                return ac_s_init_not_cursor(pool,
                                            _ac_s_open_cursor(pool, search, t->child->next, arg),
                                            _ac_s_open_cursor(pool, search, t->child, arg));
            }
        }
        return NULL;
    }
    else
        return ac_s_init_term_cursor(pool, search, t, arg);
}

ac_s_cursor_t *ac_s_open_cursor(ac_pool_t *pool, ac_s_t *search, ac_token_t *t, void *arg) {
    if(!t)
        return ac_s_init_empty_cursor(pool);
        
    ac_s_cursor_t *c = _ac_s_open_cursor(pool, search, t, arg);
    if(!c)
        return ac_s_init_empty_cursor(pool);
    return c;
}