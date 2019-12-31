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

#include "ac_io.h"

#include "ac_allocator.h"

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

bool ac_io_extension(const char *filename, const char *extension) {
  if (filename && strlen(filename) > strlen(extension) &&
      !strcmp(filename + strlen(filename) - strlen(extension), extension))
    return true;
  return false;
}

ac_io_format_t ac_io_delimiter(int delim) {
  delim++;
  return -delim;
}

ac_io_format_t ac_io_fixed(int size) { return size; }

ac_io_format_t ac_io_prefix() { return 0; }

bool ac_io_make_directory(const char *path) {
  DIR *d = opendir(path);
  if (d)
    closedir(d);
  else {
    char *cmd = (char *)ac_malloc(11 + strlen(path));
    sprintf(cmd, "mkdir -p %s", path);
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

size_t ac_io_file_size(const char *filename) {
  if (!filename)
    return 0;
  struct stat sb;
  if (stat(filename, &sb) == -1 || (sb.st_mode & S_IFMT) != S_IFREG)
    return 0;
  return sb.st_size;
}

#ifdef _AC_DEBUG_MEMORY_
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
#ifdef _AC_DEBUG_MEMORY_
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
