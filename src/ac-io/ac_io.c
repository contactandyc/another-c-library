/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "another-c-library/ac_io.h"

#include "another-c-library/ac_allocator.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "another-c-library/ac_lz4.h"

ac_sort_compare_arg_m(ac_io_sort_records, ac_io_record_t);

bool ac_io_keep_first(ac_io_record_t *res, const ac_io_record_t *r,
                      size_t num_r, ac_buffer_t *bh, void *tag) {
  *res = *r;
  return true;
}

size_t ac_io_hash_partition(const ac_io_record_t *r, size_t num_part,
                            void *arg) {
  size_t offs = arg ? (*(size_t *)arg) : 0;
  uint64_t hash = ac_lz4_hash64(r->record + offs, (r->length - offs) + 1);
  return hash % num_part;
}

bool ac_io_extension(const char *filename, const char *extension) {
  if(!filename)
    return false;
  const char *r = strrchr(filename, '/');
  if (r)
    filename = r + 1;
  r = strrchr(filename, '.');
  if (!extension || extension[0] == 0)
    return r ? false : true;
  else if (!r)
    return false;
  return strcmp(r + 1, extension) ? false : true;
}

ac_io_format_t ac_io_delimiter(int delim) {
  delim++;
  return -delim;
}

ac_io_format_t ac_io_csv_delimiter(int delim) {
  delim += 257;
  return -delim;
}

ac_io_format_t ac_io_fixed(int size) { return size; }

ac_io_format_t ac_io_prefix() { return 0; }

bool ac_io_make_directory(const char *path) {
  DIR *d = opendir(path);
  if (d)
    closedir(d);
  else {
    size_t cmd_len = 11 + strlen(path);
    char *cmd = (char *)ac_malloc(cmd_len);
    snprintf(cmd, cmd_len, "mkdir -p %s", path);
    if (system(cmd) != 0) {
      ac_free(cmd);
      return false;
    }
    ac_free(cmd);
  }
  chmod(path,
        S_IWUSR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  return true;
}

bool ac_io_make_path_valid(char *filename) {
  char *slash = strrchr(filename, '/');
  if (!slash)
    return true;
  *slash = 0;
  if (!ac_io_make_directory(filename)) {
    *slash = '/';
    return false;
  }
  *slash = '/';
  return true;
}

bool ac_io_file_info(ac_io_file_info_t *fi) {
  if (!fi || !fi->filename)
    return false;
  struct stat sb;
  if (stat(fi->filename, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
    return false;
  fi->last_modified = sb.st_mtime;
  fi->size = sb.st_size;
  fi->hash = ac_lz4_hash64(fi->filename, strlen(fi->filename));
  return true;
}

bool ac_io_directory(const char *filename) {
  if (!filename)
    return false;
  struct stat sb;
  if (stat(filename, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFDIR)
    return false;
  return true;
}

bool ac_io_file(const char *filename) {
  if (!filename)
    return false;
  struct stat sb;
  if (stat(filename, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
    return false;
  return true;
}

time_t ac_io_modified(const char *filename) {
  if (!filename)
    return 0;
  struct stat sb;
  if (stat(filename, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
    return 0;
  return sb.st_mtime;
}

size_t ac_io_file_size(const char *filename) {
  if (!filename)
    return 0;
  struct stat sb;
  if (stat(filename, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
    return 0;
  return sb.st_size;
}

bool ac_io_file_exists(const char *filename) {
  if (!filename)
    return false;
  struct stat sb;
  if (stat(filename, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
    return false;
  return true;
}

ac_io_file_info_t *ac_io_select_file_info(ac_pool_t *pool, size_t *num_res,
                                          ac_io_file_info_t *inputs,
                                          size_t num_inputs, size_t partition,
                                          size_t num_partitions) {
  ac_io_file_info_t *p = inputs;
  ac_io_file_info_t *ep = p + num_inputs;
  size_t num_matching = 0;
  while (p < ep) {
    if ((p->hash % num_partitions) == partition)
      num_matching++;
    p++;
  }
  *num_res = num_matching;
  if (!num_matching)
    return NULL;
  p = inputs;
  ac_io_file_info_t *res = (ac_io_file_info_t *)ac_pool_alloc(
      pool, sizeof(ac_io_file_info_t) * num_matching);
  ac_io_file_info_t *wp = res;
  while (p < ep) {
    if ((p->hash % num_partitions) == partition) {
      *wp = *p;
      wp++;
    }
    p++;
  }
  return res;
}

typedef struct ac_io_file_info_link_s {
  ac_io_file_info_t fi;
  struct ac_io_file_info_link_s *next;
} ac_io_file_info_link_t;

typedef struct {
  ac_io_file_info_link_t *head;
  ac_io_file_info_link_t *tail;
  size_t num_files;
  size_t bytes;
} ac_io_file_info_root_t;

void _ac_io_list(ac_io_file_info_root_t *root, const char *path,
                 ac_pool_t *pool, ac_file_valid_cb file_valid, void *arg) {
  if (!path)
    path = "";
  DIR *dp = opendir(path[0] ? path : ".");
  if (!dp)
    return;

  char *filename = (char *)ac_malloc(8192);
  struct dirent *entry;

  while ((entry = readdir(dp)) != NULL) {
    if (entry->d_name[0] == '.')
      continue;
    snprintf(filename, 8192, "%s/%s", path, entry->d_name);
    ac_io_file_info_t fi;
    fi.filename = filename;
    if (!ac_io_file_info(&fi)) {
      if (ac_io_directory(filename))
        _ac_io_list(root, filename, pool, file_valid, arg);
    } else {
      if (!file_valid || file_valid(filename, arg)) {
        size_t len = strlen(filename) + 1 + sizeof(ac_io_file_info_link_t);
        ac_io_file_info_link_t *n;
        if (pool)
          n = (ac_io_file_info_link_t *)ac_pool_alloc(pool, len);
        else
          n = (ac_io_file_info_link_t *)ac_malloc(len);
        n->fi.filename = (char *)(n + 1);
        n->fi.tag = 0;
        n->next = NULL;
        strcpy(n->fi.filename, filename);
        ac_io_file_info(&(n->fi));
        if (!root->head)
          root->head = root->tail = n;
        else {
          root->tail->next = n;
          root->tail = n;
        }
        root->bytes += strlen(filename) + 1;
        root->num_files++;
      }
    }
  }
  ac_free(filename);
  (void)closedir(dp);
}

static ac_io_file_info_t *
__ac_io_list(ac_pool_t *pool, const char *path, size_t *num_files,
             ac_file_valid_cb file_valid, void *arg) {
  ac_io_file_info_root_t root;
  root.head = root.tail = NULL;
  root.num_files = root.bytes = 0;
  ac_pool_t *tmp_pool = ac_pool_init(4096);
  _ac_io_list(&root, path, tmp_pool, file_valid, arg);
  *num_files = root.num_files;
  if (!root.num_files) {
    ac_pool_destroy(tmp_pool);
    return NULL;
  }
  ac_io_file_info_t *res;
  if (pool)
    res = (ac_io_file_info_t *)ac_pool_calloc(
        pool, (sizeof(ac_io_file_info_t) * root.num_files) + root.bytes);
  else
    res = (ac_io_file_info_t *)ac_calloc(
        (sizeof(ac_io_file_info_t) * root.num_files) + root.bytes);
  char *mem = (char *)(res + root.num_files);
  ac_io_file_info_t *rp = res;
  ac_io_file_info_link_t *n = root.head;
  while (n) {
    *rp = n->fi;
    rp->filename = mem;
    strcpy(rp->filename, n->fi.filename);
    mem += strlen(rp->filename) + 1;
    rp++;
    n = n->next;
  }
  ac_pool_destroy(tmp_pool);
  return res;
}

ac_io_file_info_t *
ac_io_list(const char *path, size_t *num_files,
           ac_file_valid_cb file_valid, void *arg) {
  return __ac_io_list(NULL, path, num_files, file_valid, arg);
}

ac_io_file_info_t *
ac_pool_io_list(ac_pool_t *pool, const char *path, size_t *num_files,
                ac_file_valid_cb file_valid, void *arg) {
  return __ac_io_list(pool, path, num_files, file_valid, arg);
}

static inline
int compare_ac_io_file_info(const ac_io_file_info_t *a, const ac_io_file_info_t *b) {
    if(a->last_modified != b->last_modified)
        return (a->last_modified < b->last_modified) ? -1 : 1;
    return 0;
}

static ac_sort_m(_sort_ac_io_file_info, ac_io_file_info_t, compare_ac_io_file_info);

void ac_io_sort_file_info_by_last_modified(ac_io_file_info_t *files, size_t num_files) {
    _sort_ac_io_file_info(files, num_files);
}

#ifdef _AC_MEMORY_CHECK_
char *_ac_io_read_file(size_t *len, const char *filename, const char *caller) {
#else
char *_ac_io_read_file(size_t *len, const char *filename) {
#endif
  *len = 0;
  if (!filename)
    return NULL;

  int fd = open(filename, O_RDONLY);
  if (fd == -1)
    return NULL;

  size_t length = ac_io_file_size(filename);
  if (length) {
#ifdef _AC_MEMORY_CHECK_
    char *buf = (char *)_ac_malloc_d(NULL, caller, length + 1, false);
#else
    char *buf = (char *)ac_malloc(length + 1);
#endif
    if (buf) {
      size_t s = length;
      size_t pos = 0;
      size_t chunk = 64 * 1024 * 1024;
      while (s > chunk) {
        if (read(fd, buf + pos, chunk) != chunk) {
          ac_free(buf);
          close(fd);
          return NULL;
        }
        pos += chunk;
        s -= chunk;
      }
      if (read(fd, buf + pos, s) == (ssize_t)s) {
        close(fd);
        buf[length] = 0;
        *len = length;
        return buf;
      }
      ac_free(buf);
      buf = NULL;
    }
  }
  close(fd);
  return NULL;
}
