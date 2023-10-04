#include "another-c-library/ac_search_builder.h"

#include <inttypes.h>

#include "another-c-library/ac_out.h"
#include "another-c-library/ac_buffer.h"
#include "the-macro-library/macro_bsearch.h"

static ac_out_t *open_sorted(char *filename, ac_io_compare_cb compare, size_t buffer_size) {
  ac_out_options_t options;
  ac_out_ext_options_t ext_options;
  ac_out_options_init(&options);
  ac_out_ext_options_init(&ext_options);

  ac_out_options_format(&options, ac_io_prefix());
  ac_out_options_buffer_size(&options, buffer_size);
  ac_out_ext_options_compare(&ext_options, compare, NULL);
  ac_out_ext_options_reducer(&ext_options,
                             ac_io_keep_first, NULL);
  ac_out_ext_options_use_extra_thread(&ext_options);

  return ac_out_ext_init(filename, &options, &ext_options);
}

struct ac_search_builder_s {
    char *filename;
    char *base_filename;
    size_t filename_len;
    size_t buffer_size;
    ac_buffer_t *bh;
    ac_pool_t *tmp_pool;
    ac_out_t *term_data;
    ac_out_t *global_data;
    uint32_t max_id;
};

struct term_data_s;
typedef struct term_data_s term_data_t;

#pragma pack(push)
#pragma pack(2)
struct term_data_s {
    uint32_t id;
    uint16_t position;
    int32_t value;
};
#pragma pack(pop)

static int compare_term_data(const ac_io_record_t *r1, const ac_io_record_t *r2, void *arg) {
    term_data_t *a = (term_data_t *)r1->record;
    term_data_t *b = (term_data_t *)r2->record;
    int n=strcmp((char *)(a+1), (char *)(b+1));
    if(n)
        return n;
    if(a->id != b->id)
        return (a->id < b->id) ? -1 : 1;
    if(a->position != b->position)
        return (a->position < b->position) ? -1 : 1;
    return 0;
}

static int compare_global_data(const ac_io_record_t *r1, const ac_io_record_t *r2, void *arg) {
    uint32_t *a = (uint32_t *)r1->record;
    uint32_t *b = (uint32_t *)r2->record;
    if(*a != *b)
        return (*a < *b) ? -1 : 1;
    return 0;
}

ac_search_builder_t *ac_search_builder_init(const char *filename, size_t buffer_size) {
    ac_search_builder_t *h = (ac_search_builder_t *)ac_calloc(sizeof(*h) + (strlen(filename)*2) + 50);
    h->base_filename = (char *)(h+1);
    strcpy(h->base_filename, filename);
    h->filename_len = strlen(filename);
    h->filename = h->base_filename + h->filename_len + 1;

    snprintf(h->filename, h->filename_len+40, "%s_data", filename );
    h->term_data = open_sorted(h->filename, compare_term_data, buffer_size);
    snprintf(h->filename, h->filename_len+40, "%s_gbl", filename );
    h->global_data = open_sorted(h->filename, compare_global_data, buffer_size/10);
    h->buffer_size = buffer_size;
    h->bh = ac_buffer_init(256);
    h->tmp_pool = ac_pool_init(1024);
    return h;
}

void ac_search_builder_global(ac_search_builder_t *h, uint32_t id, const void *d, uint32_t len) {
    ac_buffer_set(h->bh, &id, sizeof(id));
    ac_buffer_append(h->bh, d, len);
    ac_out_write_record(h->global_data, ac_buffer_data(h->bh), ac_buffer_length(h->bh));
    if(id > h->max_id)
        h->max_id = id;
}


void ac_search_builder_term(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, const char *term ) {
    if(pos > 0x3FFF)
        return;

    term_data_t t;
    t.id = id;
    t.position = pos;
    t.value = value;
    ac_buffer_set(h->bh, &t, sizeof(t));
    ac_buffer_append(h->bh, term, strlen(term)+1);
    ac_out_write_record(h->term_data, ac_buffer_data(h->bh), ac_buffer_length(h->bh));
}

void ac_search_builder_termf(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, const char *term, ... ) {
  va_list args;
  va_start(args, term);
  ac_pool_clear(h->tmp_pool);
  char *r = ac_pool_strdupvf(h->tmp_pool, term, args);
  va_end(args);
  ac_search_builder_term(h, id, value, pos, r);
}

void ac_search_builder_wterm(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, size_t sp, const char *term ) {
    ac_search_builder_term(h, id, value, pos, term);
    size_t term_len = strlen(term);
    char *r = ac_pool_alloc(h->tmp_pool, term_len+2);
    strcpy(r, term);
    for( size_t i=term_len+1; i>sp; i-- ) {
      size_t ix=i-1;
      r[i] = 0;
      r[i-1] = '*';
      ac_search_builder_term(h, id, value, pos, r);
    }
}

void ac_search_builder_wtermf(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, size_t sp, const char *term, ... ) {
    va_list args;
    va_start(args, term);
    ac_pool_clear(h->tmp_pool);
    char *r = ac_pool_strdupvf(h->tmp_pool, term, args);
    va_end(args);
    ac_search_builder_wterm(h, id, value, pos, sp, r);
}

void ac_search_builder_destroy(ac_search_builder_t *h) {
    ac_io_record_t *r;
    ac_buffer_t *bh = ac_buffer_init(1024*1024);
    ac_buffer_t *out_bh = ac_buffer_init(1024*1024);
    ac_buffer_t *group_bh = ac_buffer_init(1024*1024);
    ac_buffer_t *key = ac_buffer_init(128);
    ac_in_t *in;
    FILE *out_idx, *out_data;
    size_t offs;

    snprintf(h->filename, h->filename_len+40, "%s_gbl_idx", h->base_filename);
    out_idx = fopen(h->filename, "wb");
    snprintf(h->filename, h->filename_len+40, "%s_gbl", h->base_filename);
    out_data = fopen(h->filename, "wb");
    offs = 4;
    uint32_t last_id = 0;
    size_t zero = 0;
    in = ac_out_in(h->global_data);
    while((r=ac_in_advance(in)) != NULL) {
        uint32_t id = (*(uint32_t *)r->record);
        while(last_id < id) {
            fwrite(&zero, sizeof(zero), 1, out_idx);
            last_id++;
        }
        if(last_id != id)
            abort();
        fwrite(&offs, sizeof(offs), 1, out_idx);
        offs += r->length;
        id = r->length - 4;
        fwrite(&id, sizeof(id), 1, out_data);
        fwrite(r->record+4, id, 1, out_data);
        last_id++;
    }
    fclose(out_idx);
    fclose(out_data);
    ac_in_destroy(in);

    snprintf(h->filename, h->filename_len+40, "%s_term_idx", h->base_filename);
    out_idx = fopen(h->filename, "wb");
    snprintf(h->filename, h->filename_len+40, "%s_term_data", h->base_filename);
    out_data = fopen(h->filename, "wb");

    offs = 4;
    in = ac_out_in(h->term_data);
    r=ac_in_advance(in);
    while(r != NULL) {
        ac_buffer_set(key, r->record+sizeof(term_data_t), r->length-sizeof(term_data_t));
        ac_buffer_set(bh, r->record, sizeof(term_data_t));
        while((r=ac_in_advance(in)) != NULL &&
              !strcmp(r->record+sizeof(term_data_t), ac_buffer_data(key))) {
            ac_buffer_append(bh, r->record, sizeof(term_data_t));
        }
        term_data_t *p = (term_data_t *)ac_buffer_data(bh);
        term_data_t *ep = (term_data_t *)ac_buffer_end(bh);
        uint32_t max_term_size = 0;
        ac_buffer_clear(out_bh);
        while(p < ep) {
            term_data_t *cur = p;
            uint32_t id = cur->id & 0xffff000;
            p++;
            while(p < ep && id == (p->id & 0xffff000))
                p++;

            ac_buffer_clear(group_bh);
            term_data_t *p2 = cur;
            while(p2 < p) {
                term_data_t *cur2 = p2;
                p2++;
                id = cur2->id & 0xfff;
                while(p2 < p && id == (p2->id & 0xfff))
                    p2++;
                uint16_t sid = id << 4;
                if(p2-cur2 == 1 && cur2->position == 0) {
                    if(cur2->value >= 0 && cur2->value < 65536) {
                        if(cur2->value < 256) {
                            if(cur2->value < 5) {
                                // encode directly
                                sid += cur2->value;
                                ac_buffer_append(group_bh, &sid, sizeof(sid));
                            }
                            else {
                                // uint8
                                sid |= 5;
                                ac_buffer_append(group_bh, &sid, sizeof(sid));
                                uint8_t value = cur2->value;
                                ac_buffer_append(group_bh, &value, sizeof(value));
                            }
                        }
                        else {
                            // uint16
                            sid |= 6;
                            ac_buffer_append(group_bh, &sid, sizeof(sid));
                            uint16_t value = cur2->value;
                            ac_buffer_append(group_bh, &value, sizeof(value));
                        }
                    }
                    else {
                        // int32
                        sid |= 7;
                        ac_buffer_append(group_bh, &sid, sizeof(sid));
                        int32_t value = cur2->value;
                        ac_buffer_append(group_bh, &value, sizeof(value));
                    }
                }
                else {
                    if(p2-cur2 > max_term_size)
                        max_term_size = p2-cur2;
                    sid |= 8;
                    uint32_t sum = 0;
                    term_data_t *p3 = cur2;
                    while(p3 < p2) {
                        if(p3->value == 0)
                            sum += 2;
                        else if(p3->value > 0 && p3->value < 65536) {
                            if(p3->value < 256)
                                sum += 3;
                            else
                                sum += 4;
                        }
                        else
                            sum += 6;
                        p3++;
                    }

                    sum--; // always at least 2 bytes, zero reserved for extension
                    if(sum > 7) {
                        ac_buffer_append(group_bh, &sid, sizeof(sid));
                        ac_buffer_append(group_bh, &sum, sizeof(sum));
                    }
                    else {
                        sid |= sum;
                        ac_buffer_append(group_bh, &sid, sizeof(sid));
                    }
                    p3 = cur2;
                    while(p3 < p2) {
                        uint16_t pos = p3->position;
                        if(p3->value == 0)
                            ac_buffer_append(group_bh, &pos, sizeof(pos));
                        else if(p3->value > 0 && p3->value < 65536) {
                            if(p3->value < 256) {
                                pos |= (1<<14);
                                ac_buffer_append(group_bh, &pos, sizeof(pos));
                                uint8_t value = p3->value;
                                ac_buffer_append(group_bh, &value, sizeof(value));
                            }
                            else {
                                pos |= (2<<14);
                                ac_buffer_append(group_bh, &pos, sizeof(pos));
                                uint16_t value = p3->value;
                                ac_buffer_append(group_bh, &value, sizeof(value));
                            }
                        }
                        else {
                            pos |= (3<<14);
                            ac_buffer_append(group_bh, &pos, sizeof(pos));
                            int32_t value = p3->value;
                            ac_buffer_append(group_bh, &value, sizeof(value));
                        }
                        p3++;
                    }
                }
            }
            id = (cur->id & 0xffff000) >> 12;
            uint16_t gid = id;
            ac_buffer_append(out_bh, &gid, sizeof(gid));
            uint32_t len = ac_buffer_length(group_bh);
            if(len <= 63) {
                uint8_t value = len;
                ac_buffer_append(out_bh, &value, sizeof(value));
            }
            else if(len <= 16383) {
                uint32_t v = (len & 0x3f00) >> 8;
                v |= (1<<6);
                uint8_t value = v;
                ac_buffer_append(out_bh, &value, sizeof(value));
                v = len & 0xff;
                value = v;
                ac_buffer_append(out_bh, &value, sizeof(value));
            }
            else { // at most 4194303
                uint32_t v = (len & 0x3f0000) >> 16;
                v |= (2<<6);
                uint8_t value = v;
                ac_buffer_append(out_bh, &value, sizeof(value));
                v = len & 0xffffff;
                uint16_t v2 = v;
                ac_buffer_append(out_bh, &v2, sizeof(v2));
            }
            ac_buffer_append(out_bh, ac_buffer_data(group_bh), ac_buffer_length(group_bh));
        }
        fwrite(ac_buffer_data(key), ac_buffer_length(key), 1, out_idx);
        fwrite(&offs, sizeof(offs), 1, out_idx);
        fwrite(&max_term_size, sizeof(max_term_size), 1, out_idx);
        uint32_t len = ac_buffer_length(out_bh);
        offs += len + 4;
        fwrite(&len, sizeof(len), 1, out_data);
        fwrite(ac_buffer_data(out_bh), len, 1, out_data);
    }
    fclose(out_idx);
    fclose(out_data);
    ac_in_destroy(in);

    ac_buffer_destroy(bh);
    ac_buffer_destroy(group_bh);
    ac_buffer_destroy(out_bh);
    ac_buffer_destroy(key);

    ac_buffer_destroy(h->bh);
    ac_pool_destroy(h->tmp_pool);
    ac_free(h);
}


struct ac_search_builder_image_s {
    void **gbls;
    uint32_t num_gbls;

    char *gbl_data;
    size_t gbl_data_len;

    char *term_idx;
    size_t term_idx_len;
    char **terms;
    size_t num_terms;
    char *term_data;
    size_t term_data_len;
};

void ac_search_builder_image_destroy(ac_search_builder_image_t *h) {
    ac_free(h->gbls);
    ac_free(h->gbl_data);
    ac_free(h->term_idx);
    ac_free(h->terms);
    ac_free(h->term_data);
    ac_free(h);
}

const void * ac_search_builder_image_global(ac_search_builder_image_t *h, uint32_t gid) {
    return h->gbls[gid];
}

ac_search_builder_image_t *ac_search_builder_image_init(const char *base) {
    char *p, *ep, **wp;
    size_t filename_len = strlen(base)+50;
    char *filename = (char *)ac_malloc(filename_len);
    ac_search_builder_image_t *h = (ac_search_builder_image_t *)ac_calloc(sizeof(*h));

    size_t len = 0;
    snprintf(filename, filename_len, "%s_gbl_idx", base );
    size_t *gbl_idx = (size_t *)ac_io_read_file(&len, filename);
    len /= sizeof(size_t);

    h->gbls = (void **)ac_calloc(sizeof(void *) * len);
    h->num_gbls = len;

    snprintf(filename, filename_len, "%s_gbl", base );
    h->gbl_data = (char *)ac_io_read_file(&h->gbl_data_len, filename);
    for( size_t i=0; i<len; i++ ) {
        if(gbl_idx[i] > 0)
            h->gbls[i] = (void *)(h->gbl_data + gbl_idx[i]);
    }
    ac_free(gbl_idx);

    snprintf(filename, filename_len, "%s_term_idx", base );
    h->term_idx = (char *)ac_io_read_file(&h->term_idx_len, filename);
    h->num_terms = 0;
    p = h->term_idx;
    ep = p+h->term_idx_len;
    while(p < ep) {
        h->num_terms++;
        p += strlen(p) + 1;
        p += sizeof(size_t) + sizeof(uint32_t); // offset + max_term_size
    }

    h->terms = (char **)ac_calloc(sizeof(char *) * h->num_terms);
    wp = h->terms;
    p = h->term_idx;
    while(p < ep) {
        *wp = p;
        wp++;
        p += strlen(p) + 1;
        p += sizeof(size_t) + sizeof(uint32_t);
    }

    snprintf(filename, filename_len, "%s_term_data", base );
    h->term_data = (char *)ac_io_read_file(&h->term_data_len, filename);
    ac_free(filename);
    return h;
}

static inline int compare_strings(const char *key, const char **v) {
    return strcmp(key, *v);
}

static inline ac_search_m(search_strings, char, char *, compare_strings);

static inline void advance_group(ac_search_builder_image_term_t *t)
{
    uint8_t *p = t->gp;
    uint16_t gid = (*(uint16_t *)p);
    p += 2;
    uint32_t len;
    uint8_t high_bits = (*p) >> 6;
    if(high_bits == 0) {
        len = *p;
        p++;
    }
    else if(high_bits == 1) {
        len = (*p) & 0x3F;
        p++;
        len <<= 8;
        len += (*p);
        p++;
    }
    else if(high_bits == 2) {
        len = (*p) & 0x3F;
        p++;
        len <<= 16;
        len += (*(uint16_t *)p);
        p += 2;
    }
    else
        abort();
    t->p = p;
    t->ep = p+len;
    t->gp = t->ep;
    t->gid = gid;
    t->gid <<= 12;
}

/*
   top 12 bits are id
   bit 3 indicates variable if set
   if set
     bits 0-2 have a value of zero, 4 byte unsigned length (add one to it)
       else length = bits 0-2 + 1
     each position is 14 low bits, 2 bits for value length
       bits 14-15 are 0 if no value
          1 if 1 byte unsigned value
          2 if 2 byte unsigned value
          3 if 4 byte signed value
   else
     bits 0-2 have a value of 0..4 if that is value
       value of 5 if 1 byte unsigned value
       value of 6 if 2 byte unsigned value
       value of 7 if 4 byte int value
*/
static inline void advance_id(ac_search_builder_image_term_t *t) {
    uint8_t *p = t->p;
    uint16_t id = (*(uint16_t *)p);
    p += 2;
    uint32_t control = id & 0xF;
    t->id = id >> 4;
    t->id += t->gid;
    if(control & 0x8) {
        t->value = 0;
        control -= 8;
        if(!control) {
            control = (*(uint32_t *)p);
            p += 4;
        }
        control++;
        t->wp = p;
        p += control;
        t->p = p;
    }
    else {
        if(control < 5)
            t->value = control;
        else if(control == 5) {
            t->value = *p;
            p++;
        }
        else if(control == 6) {
            t->value = (*(uint16_t *)p);
            p += 2;
        }
        else {
            t->value = (*(int32_t *)p);
            p += 4;
        }
        t->p = p;
        t->wp = p;
    }
}

bool ac_search_builder_image_advance(ac_search_builder_image_term_t *t)
{
    if(t->p >= t->ep)
    {
        if(t->gp >= t->egp)
            return false;
        advance_group(t);
    }
    advance_id(t);
    return true;
}

bool ac_search_builder_image_advance_to(ac_search_builder_image_term_t *t, uint32_t id)
{
    if(id <= t->id)
        return true;

    uint32_t gid = id & 0xffff000;
    if(t->gid < gid) {
        while(t->gid < gid) {
            if(t->gp >= t->egp)
                return false;
            advance_group(t);
        }
        advance_id(t);
        if(gid < t->gid)
            return true;
        while(id > t->id && t->p < t->ep)
            advance_id(t);
        if(id <= t->id)
            return true;
        if(t->gp >= t->egp)
            return false;
        advance_group(t);
        advance_id(t);
        return true;
    }
    while(id > t->id && t->p < t->ep)
        advance_id(t);
    if(id <= t->id)
        return true;
    if(t->gp >= t->egp)
        return false;
    advance_group(t);
    advance_id(t);
    return true;
}

void ac_search_builder_fill_term(ac_search_builder_image_t *img, ac_pool_t *pool, ac_search_builder_image_term_t *r, char **termp) {
    char *p = *termp;
    p = p + strlen(p) + 1;
    size_t offs = (*(size_t *)p);
    p += sizeof(offs);
    uint32_t max_term_size = (*(uint32_t *)p);

    r->gp = (uint8_t *)(img->term_data + offs);
    uint32_t len = (*(uint32_t *)(r->gp-4));
    r->egp = r->gp + len;
    r->gid = 0;
    r->id = 0;
    r->p = NULL;
    r->ep = NULL;
    r->wp = NULL;
    r->value = 0;

    r->max_term_size = max_term_size;
    r->term_pos = (ac_search_builder_image_term_pos_t *)ac_pool_alloc(pool, sizeof(ac_search_builder_image_term_pos_t) * (max_term_size+1));
    r->num_term_pos = 0;

    r->term = termp;
    r->eterm = img->terms+img->num_terms;
}

bool ac_search_builder_image_term(ac_search_builder_image_t *img, ac_pool_t *pool, ac_search_builder_image_term_t *r, const char *term) {
    char **termp = search_strings(term, (const char **)img->terms, img->num_terms);
    if(!termp) {
        if(term && term[0] && term[strlen(term)-1] == '*') {
            char *t = ac_pool_strdup(pool, term);
            t[strlen(t)-1] = 0;
            termp = search_strings(t, (const char **)img->terms, img->num_terms);
            if(!termp)
                return false;
        }
        else
            return false;
    }
    ac_search_builder_fill_term(img, pool, r, termp);
    return true;
}

uint32_t ac_search_builder_cursor_advance(ac_search_builder_cursor_t *c)
{
    if(ac_search_builder_image_advance(&c->term)) {
        c->cursor.current = c->term.id;
        return c->cursor.current;
    }
    return ac_s_empty_cursor(&c->cursor);
}

uint32_t ac_search_builder_cursor_advance_to(ac_search_builder_cursor_t *c, uint32_t id)
{
    if(ac_search_builder_image_advance_to(&c->term, id)) {
        c->cursor.current = c->term.id;
        return c->cursor.current;
    }
    return ac_s_empty_cursor(&c->cursor);
}

ac_s_cursor_t *ac_search_builder_init_sb2_cursor(ac_search_builder_image_t *img, ac_pool_t *pool, const char *term, ac_number_range_t *nr) {
    ac_search_builder_cursor_t *r = ac_pool_calloc(pool, sizeof(*r));
    ac_s_cursor_t *c = (ac_s_cursor_t *)r;
    if(!ac_search_builder_image_term(img, pool, &r->term, term))
        return NULL;
    c->pool = pool;
    c->type = 1;
    //if(!nr) {
        c->advance = (ac_s_advance_cb)ac_search_builder_cursor_advance;
        c->advance_to = (ac_s_advance_to_cb)ac_search_builder_cursor_advance_to;
    //}
    c->advance(c);
    ac_s_reset(c);
    return c;
}


uint32_t ac_search_builder_advance(ac_search_builder_cursor2_t *c)
{
    if(ac_search_builder_image_advance(&c->term)) {
        c->cursor.current = c->term.id;
        return c->cursor.current;
    }
    return ac_cursor_empty(&c->cursor);
}

uint32_t ac_search_builder_advance_to(ac_search_builder_cursor2_t *c, uint32_t id)
{
    if(ac_search_builder_image_advance_to(&c->term, id)) {
        c->cursor.current = c->term.id;
        return c->cursor.current;
    }
    return ac_cursor_empty(&c->cursor);
}

ac_cursor_t *ac_search_builder_init_cursor(ac_search_builder_image_t *img, ac_pool_t *pool, const char *term) {
    ac_search_builder_cursor2_t *r = ac_pool_calloc(pool, sizeof(*r));
    ac_cursor_t *c = (ac_cursor_t *)r;

    if(!ac_search_builder_image_term(img, pool, &r->term, term))
        return NULL;
    c->pool = pool;
    c->type = 1;
    c->advance = (ac_cursor_advance_cb)ac_search_builder_advance;
    c->advance_to = (ac_cursor_advance_to_cb)ac_search_builder_advance_to;
    c->advance(c);
    ac_cursor_reset(c);
    return c;
}