#include "another-c-library/ac_file_sync.h"
#include "another-c-library/ac_sort.h"
#include "another-c-library/ac_io.h"
#include "another-c-library/ac_json.h"
#include "another-c-library/ac_allocator.h"
#include "another-c-library/ac_in.h"

#include <stdio.h>
#include <string.h>

static inline int compare_ac_file_sync_entry(const ac_file_sync_entry_t *a, const ac_file_sync_entry_t *b) {
    int n=strcmp(a->type, b->type);
    if(n)
        return n;
    if(a->index != b->index)
        return (a->index < b->index) ? -1 : 1;
    return 0;
}

static inline ac_sort_m(sort_ac_file_sync_entry, ac_file_sync_entry_t, compare_ac_file_sync_entry);

void ac_file_sync_destroy(ac_file_sync_t *h) {
    ac_pool_t *pool = h->pool;
    ac_pool_destroy(pool);
}

static void write_number(ac_pool_t *pool, const char *path, const char *filename, uint32_t num) {
    char *fn = ac_pool_strdupf(pool, "%s/%s", path, filename );
    FILE *out = fopen(fn, "wb");
    fprintf(out, "%u", num);
    fclose(out);
}

static bool check_file(const char *filename, void *arg) {
    ac_file_sync_entry_t *entry = (ac_file_sync_entry_t *)arg;
    size_t len = strlen(filename);
    if(entry->gzip)
        return len > 8 && !strcmp(filename+len-8, ".json.gz");
    else
        return len > 5 && !strcmp(filename+len-5, ".json");
}

static inline int compare_uint32_t(const uint32_t *a, const uint32_t *b) {
    if(*a != *b)
        return (*a < *b) ? -1 : 1;
    return 0;
}

static inline ac_sort_m(sort_uint32_t, uint32_t, compare_uint32_t);

typedef struct {
    uint32_t *files;
    size_t num_files;
    size_t buffer_size;
    size_t idx;
    bool use_checkpoint;
    ac_file_sync_entry_t *entry;
    ac_pool_t *pool;
} filenames_t;

static ac_in_t *in_cb(void *arg) {
    filenames_t *f = (filenames_t *)arg;
    while(true) {
        if(f->idx >= f->num_files)
            return NULL;
        uint32_t cur_id = f->files[f->idx];
        f->idx++;
        char *filename = ac_pool_strdupf(f->pool, "%s/%u.json%s",
                f->entry->path,
                cur_id,
                f->entry->gzip ? ".gz" : ""
            );
        ac_in_t *in = ac_in_quick_init(filename, ac_io_delimiter('\n'), f->entry->buffer_size * 1024);
        if(!in)
            continue;
        ac_io_record_t *r = ac_in_advance(in);
        while(r) {
            char *p = strchr(r->record, ',');
            if(!p) {
                r = ac_in_advance(in);
                continue;
            }
            *p = 0;
            uint32_t id = 0;
            if(sscanf(r->record, "%u", &id) != 1)
                id = 0;
            *p = ',';
            if(!f->use_checkpoint || id > f->entry->checkpoint)
                break;
            r = ac_in_advance(in);
        }
        if(!r) {
            ac_in_destroy(in);
            continue;
        }
        ac_in_reset(in);
        return in;
    }
}

static ac_in_t *file_sync_read(ac_pool_t *pool, ac_file_sync_entry_t *entry, bool use_checkpoint) {
    size_t num_f = 0;
    uint32_t *files = entry->files;
    num_f = entry->num_files;
    size_t sp = 0;
    if(use_checkpoint) {
        for(; sp<num_f; sp++) {
            if(sp+1 == num_f || files[sp+1] > entry->checkpoint+1)
                break;
        }
    }
    files += sp;
    num_f -= sp;
    filenames_t *arg = (filenames_t *)ac_pool_calloc(pool, sizeof(filenames_t));
    arg->files = files;
    arg->num_files = num_f;
    arg->idx = 0;
    arg->use_checkpoint = use_checkpoint;
    arg->pool = pool;
    arg->entry = entry;
    return ac_in_init_from_cb(in_cb, arg);
}

typedef struct {
    ac_file_sync_t *h;
    size_t idx;
    char *type;
    bool use_checkpoint;
    ac_pool_t *pool;
} entries_t;

static ac_in_t *entries_in_cb(void *arg) {
    entries_t *f = (entries_t *)arg;
    while(true) {
        if(f->idx >= f->h->num_entries)
            return NULL;
        ac_file_sync_entry_t *entry = f->h->entries + f->idx;
        f->idx++;
        if(strcmp(entry->type, f->type))
            continue;
        ac_in_t *in = file_sync_read(f->pool, entry, f->use_checkpoint);
        if(!in)
            continue;
        return in;
    }
}

static ac_in_t *_ac_file_sync_read(ac_pool_t *pool, ac_file_sync_t *h, const char *type, bool from_checkpoint) {
    entries_t *arg = (entries_t *)ac_pool_calloc(pool, sizeof(entries_t));
    arg->h = h;
    arg->use_checkpoint = from_checkpoint;
    arg->idx = 0;
    arg->type = ac_pool_strdup(pool, type);
    arg->pool = pool;
    return ac_in_init_from_cb(entries_in_cb, arg);
}

ac_in_t *ac_file_sync_read_from_checkpoint(ac_pool_t *pool, ac_file_sync_t *h, const char *type) {
    return _ac_file_sync_read(pool, h, type, true);
}

bool ac_file_sync_has_data(ac_pool_t *pool, ac_file_sync_t *h, const char *type) {
    ac_in_t *in = _ac_file_sync_read(pool, h, type, true);
    if(in) {
        ac_in_destroy(in);
        return true;
    }
    return false;
}

ac_in_t *ac_file_sync_read_from_start(ac_pool_t *pool, ac_file_sync_t *h, const char *type) {
    return _ac_file_sync_read(pool, h, type, false);
}

void file_sync_checkpoint(ac_pool_t *pool, ac_file_sync_entry_t *entry ) {
    if(entry->checkpoint != entry->local)
        write_number(pool, entry->path, "checkpoint", entry->local );
}

void ac_file_sync_checkpoint(ac_pool_t *pool, ac_file_sync_t *h, const char *type) {
    for( size_t i=0; i<h->num_entries; i++ ) {
        if(!strcmp(h->entries[i].type, type))
            file_sync_checkpoint(pool, h->entries+i);
    }
}

static uint32_t read_number(ac_pool_t *pool, const char *path, const char *filename) {
    char *fn = ac_pool_strdupf(pool, "%s/%s", path, filename );
    size_t len = 0;
    char *buffer = ac_io_read_file(&len, fn);
    if(!buffer)
        return 0;
    uint32_t res = 0;
    if(sscanf(buffer, "%u", &res) != 1)
        res = 0;
    ac_free(buffer);
    return res;
}

void ac_file_sync_append(ac_pool_t *pool, ac_file_sync_t *h, const char *type,
                         const char *data, size_t len ) {
    if(!len)
        return;
    ac_file_sync_entry_t *entry = NULL;
    for( size_t i=h->num_entries; i>0; i-- ) {
        uint32_t idx = i-1;
        entry = h->entries+idx;
        if(entry->writable && !strcmp(entry->type, type))
            break;
        entry = NULL;
    }
    if(!entry)
        return;

    uint32_t max_id = 0;
    if(!entry->num_files) {
        entry->files = (uint32_t *)ac_pool_calloc(h->pool, sizeof(uint32_t));
        entry->num_files = 1;
    }
    max_id = entry->files[entry->num_files-1];
    if(entry->step && ((entry->local % entry->step) > (max_id % entry->step))) {
        max_id = entry->local+1;
        uint32_t *files = (uint32_t *)ac_pool_alloc(h->pool, sizeof(uint32_t) * (entry->num_files+1));
        memcpy(files, entry->files, entry->num_files*sizeof(uint32_t));
        files[entry->num_files] = max_id;
        entry->num_files++;
    }

    uint32_t id = max_id;
    FILE *out = fopen(ac_pool_strdupf(pool, "%s/%u.json", entry->path, max_id ), "ab");
    const char *p = data;
    const char *ep = p+len;
    while(p < ep) {
        id++;
        fprintf( out, "%u,%s\n", id, p );
        p += strlen(p)+1;
    }
    fclose(out);
    if(id > max_id) {
        entry->local = id;
        write_number(pool, entry->path, "local", id );
        write_number(pool, entry->path, "checkpoint", id );
    }
}

ac_file_sync_t *ac_file_sync_init(const char *filename, const char *type) {
    size_t len = 0;
    char *buffer = ac_io_read_file(&len, filename);
    if(!buffer)
        return NULL;
    ac_pool_t *pool = ac_pool_init(1024+((len+10)*4));
    char *s = ac_pool_strdup(pool, buffer);
    ac_free(buffer);
    ac_json_t *j = ac_json_parse(pool, s, s+strlen(s));
    if(!j->type) {
        ac_pool_destroy(pool);
        return NULL;
    }
    ac_file_sync_t *h = (ac_file_sync_t *)ac_pool_calloc(pool, sizeof(*h));
    h->pool = pool;
    h->base_path = ac_pool_strdup(pool, filename);
    char *p = strrchr(h->base_path, '/');
    if(p)
        p[1] = 0;
    else
        h->base_path[0] = 0;

    int num_rows = ac_jsona_count(j);
    h->entries = (ac_file_sync_entry_t *)ac_pool_calloc(pool, sizeof(ac_file_sync_entry_t) * num_rows);
    ac_file_sync_entry_t *wp = h->entries;

    ac_jsona_t *n = ac_jsona_first(j);
    while(n) {
        if(ac_json_is_object(n->value)) {
            ac_jsono_t *o = ac_jsono_first(n->value);
            wp->buffer_size = 16;
            while(o) {
                char *key = o->key;
                char *value = ac_jsond(pool, o->value);
                if(!strcmp(key, "type"))
                    wp->type = value;
                else if(!strcmp(key, "path")) {
                    if(value[0] == '/')
                        wp->path = value;
                    else
                        wp->path = ac_pool_strdupf(pool, "%s%s", h->base_path, value);
                    printf( "%s\n", wp->path );
                }
                else if(!strcmp(key, "buffer_size")) {
                    uint32_t buffer_size = 0;
                    if(sscanf(value, "%u", &buffer_size) == 1 && buffer_size > 16)
                        wp->buffer_size = buffer_size;
                }
                else if(!strcmp(key, "step")) {
                    uint32_t step = 0;
                    if(sscanf(value, "%u", &step) == 1)
                        wp->step = step;
                }
                else if(!strcmp(key, "gzip")) {
                    if(!strcmp(value, "true"))
                        wp->gzip = true;
                }
                else if(!strcmp(key, "writable")) {
                    if(!strcmp(value, "true"))
                        wp->writable = true;
                }
                o = ac_jsono_next(o);
            }
            if(wp->path && wp->type && (!type || !strcmp(wp->type, type))) {
                wp->index = wp-h->entries;
                wp->checkpoint = read_number(pool, wp->path, "checkpoint");
                wp->local = read_number(pool, wp->path, "local");
                wp->remote = read_number(pool, wp->path, "remote");

                size_t num_f = 0;
                ac_io_file_info_t *f =
                    ac_pool_io_list(pool, wp->path, &num_f, check_file, wp);
                uint32_t *files = (uint32_t *)ac_pool_alloc(pool, sizeof(uint32_t) * num_f);
                uint32_t *wptr = files;
                for( size_t i=0; i<num_f; i++ ) {
                    char *filename = f[i].filename;
                    char *p = strrchr(filename, '/');
                    if(p)
                        filename = p+1;
                    p = strchr(filename, '.');
                    if(p)
                        *p = 0;
                    uint32_t id;
                    if(sscanf(filename, "%u", &id) == 1)
                        *wptr++ = id;
                }
                num_f = wptr-files;
                sort_uint32_t(files, num_f);
                wp->files = files;
                wp->num_files = num_f;
                wp++;
            }
        }
        n = ac_jsona_next(n);
    }
    h->num_entries = wp-h->entries;
    return h;
}
