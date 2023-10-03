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

#include "another-c-library/ac_json.h"

#include "another-c-library/ac_allocator.h"
#include "another-c-library/ac_pool.h"
#include "another-c-library/ac_sort.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define AC_JSON_NATURAL_NUMBER_CASE                                            \
  '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7'        \
      : case '8' : case '9'

#define AC_JSON_NUMBER_CASE '0' : case AC_JSON_NATURAL_NUMBER_CASE

#define AC_JSON_SPACE_CASE 32 : case 9 : case 13 : case 10

#ifdef AC_JSON_DEBUG
#define AC_JSON_BAD_CHARACTER                                                  \
  line = __LINE__;                                                             \
  goto bad_character

#define AC_JSON_KEYED_START_STRING                                             \
  line2 = __LINE__;                                                            \
  goto keyed_start_string

#define AC_JSON_KEYED_ADD_STRING                                               \
  line2 = __LINE__;                                                            \
  goto keyed_add_string

#define AC_JSON_KEYED_NEXT_DIGIT                                               \
  line2 = __LINE__;                                                            \
  goto keyed_next_digit

#define AC_JSON_KEYED_DECIMAL_NUMBER                                           \
  line2 = __LINE__;                                                            \
  goto keyed_decimal_number

#define AC_JSON_START_KEY                                                      \
  line2 = __LINE__;                                                            \
  goto start_key

#define AC_JSON_START_VALUE                                                    \
  line2 = __LINE__;                                                            \
  goto start_value

#define AC_JSON_START_STRING                                                   \
  line2 = __LINE__;                                                            \
  goto start_string

#define AC_JSON_ADD_STRING                                                     \
  line2 = __LINE__;                                                            \
  goto add_string

#define AC_JSON_NEXT_DIGIT                                                     \
  line2 = __LINE__;                                                            \
  goto next_digit

#define AC_JSON_DECIMAL_NUMBER                                                 \
  line2 = __LINE__;                                                            \
  goto decimal_number
#else
#define AC_JSON_BAD_CHARACTER goto bad_character

#define AC_JSON_KEYED_START_STRING goto keyed_start_string

#define AC_JSON_KEYED_ADD_STRING goto keyed_add_string

#define AC_JSON_KEYED_NEXT_DIGIT goto keyed_next_digit

#define AC_JSON_KEYED_DECIMAL_NUMBER goto keyed_decimal_number

#define AC_JSON_START_KEY goto start_key

#define AC_JSON_START_VALUE goto start_value

#define AC_JSON_START_STRING goto start_string

#define AC_JSON_ADD_STRING goto add_string

#define AC_JSON_NEXT_DIGIT goto next_digit

#define AC_JSON_DECIMAL_NUMBER goto decimal_number
#endif

static void ac_json_dump_object_to_buffer(ac_buffer_t *bh, _ac_jsono_t *a);
static void ac_json_dump_array_to_buffer(ac_buffer_t *bh, _ac_jsona_t *a);

void ac_json_dump_to_buffer(ac_buffer_t *bh, ac_json_t *a) {
  if (a->type >= AC_JSON_STRING) {
    if (a->type == AC_JSON_STRING) {
      ac_buffer_appendc(bh, '\"');
      ac_buffer_append(bh, a->value, a->length);
      ac_buffer_appendc(bh, '\"');
    } else
      ac_buffer_append(bh, a->value, a->length);
  } else if (a->type == AC_JSON_OBJECT) {
    ac_json_dump_object_to_buffer(bh, (_ac_jsono_t *)a);
  } else if (a->type == AC_JSON_ARRAY) {
    ac_json_dump_array_to_buffer(bh, (_ac_jsona_t *)a);
  } else if (a->type == AC_JSON_BINARY) {
    ac_buffer_append(bh, "nb", 2);
    uint32_t len = a->length;
    ac_buffer_append(bh, &len, sizeof(len));
    ac_buffer_append(bh, a->value, a->length);
  }
}

static void ac_json_dump_object_to_buffer(ac_buffer_t *bh, _ac_jsono_t *a) {
  ac_buffer_appendc(bh, '{');
  ac_jsono_t *n = a->head;
  while (n) {
    ac_jsono_t *next = n->next;
    while (next && next->value == NULL)
      next = next->next;
    ac_buffer_appendc(bh, '\"');
    ac_buffer_appends(bh, n->key);
    ac_buffer_append(bh, "\":", 2);
    ac_json_dump_to_buffer(bh, n->value);
    if (next)
      ac_buffer_appendc(bh, ',');
    n = next;
  }

  ac_buffer_appendc(bh, '}');
}

static void ac_json_dump_array_to_buffer(ac_buffer_t *bh, _ac_jsona_t *a) {
  ac_buffer_appendc(bh, '[');
  ac_jsona_t *n = a->head;
  while (n) {
    ac_jsona_t *next = n->next;
    while (next && next->value == NULL)
      next = next->next;
    ac_json_dump_to_buffer(bh, n->value);
    if (next)
      ac_buffer_appendc(bh, ',');
    n = next;
  }
  ac_buffer_appendc(bh, ']');
}

static void ac_json_dump_object(FILE *out, _ac_jsono_t *a);
static void ac_json_dump_array(FILE *out, _ac_jsona_t *a);

void ac_json_dump(FILE *out, ac_json_t *a) {
  if (a->type >= AC_JSON_STRING) {
    if (a->type == AC_JSON_STRING)
      fprintf(out, "\"%s\"", a->value);
    else
      fprintf(out, "%s", a->value);
  } else if (a->type == AC_JSON_OBJECT) {
    ac_json_dump_object(out, (_ac_jsono_t *)a);
  } else if (a->type == AC_JSON_ARRAY) {
    ac_json_dump_array(out, (_ac_jsona_t *)a);
  } else if (a->type == AC_JSON_BINARY) {
    fprintf(out, "b");
    uint32_t len = a->length;
    fwrite(&len, sizeof(len), 1, out);
    fwrite(a->value, a->length, 1, out);
  }
}

static void ac_json_dump_object(FILE *out, _ac_jsono_t *a) {
  fprintf(out, "{");
  ac_jsono_t *n = a->head;
  while (n) {
    ac_jsono_t *next = n->next;
    while (next && next->value == NULL)
      next = next->next;

    fprintf(out, "\"%s\":", n->key);
    ac_json_dump(out, n->value);
    if (next)
      fprintf(out, ",");
    n = next;
  }

  fprintf(out, "}");
}

static void ac_json_dump_array(FILE *out, _ac_jsona_t *a) {
  fprintf(out, "[");
  ac_jsona_t *n = a->head;
  while (n) {
    ac_jsona_t *next = n->next;
    while (next && next->value == NULL)
      next = next->next;

    ac_json_dump(out, n->value);
    if (next)
      fprintf(out, ",");
    n = next;
  }
  fprintf(out, "]");
}

static int unicode_to_utf8(char *dest, char **src) {
  char *s = *src;
  unsigned int ch;
  int c = *s++;
  if (c >= '0' && c <= '9')
    ch = c - '0';
  else if (c >= 'A' && c <= 'F')
    ch = c - 'A' + 10;
  else if (c >= 'a' && c <= 'f')
    ch = c - 'a' + 10;
  else
    return -1;

  c = *s++;
  if (c >= '0' && c <= '9')
    ch = (ch << 4) + c - '0';
  else if (c >= 'A' && c <= 'F')
    ch = (ch << 4) + c - 'A' + 10;
  else if (c >= 'a' && c <= 'f')
    ch = (ch << 4) + c - 'a' + 10;
  else
    return -1;

  c = *s++;
  if (c >= '0' && c <= '9')
    ch = (ch << 4) + c - '0';
  else if (c >= 'A' && c <= 'F')
    ch = (ch << 4) + c - 'A' + 10;
  else if (c >= 'a' && c <= 'f')
    ch = (ch << 4) + c - 'a' + 10;
  else
    return -1;

  c = *s++;
  if (c >= '0' && c <= '9')
    ch = (ch << 4) + c - '0';
  else if (c >= 'A' && c <= 'F')
    ch = (ch << 4) + c - 'A' + 10;
  else if (c >= 'a' && c <= 'f')
    ch = (ch << 4) + c - 'a' + 10;
  else
    return -1;

  if (ch >= 0xD800 && ch <= 0xDBFF) {
    if (*s != '\\' || s[1] != 'u')
      return -1;
    s += 2;
    unsigned int ch2;
    c = *s++;
    if (c >= '0' && c <= '9')
      ch2 = c - '0';
    else if (c >= 'A' && c <= 'F')
      ch2 = c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
      ch2 = c - 'a' + 10;
    else
      return -1;

    c = *s++;
    if (c >= '0' && c <= '9')
      ch2 = (ch2 << 4) + c - '0';
    else if (c >= 'A' && c <= 'F')
      ch2 = (ch2 << 4) + c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
      ch2 = (ch2 << 4) + c - 'a' + 10;
    else
      return -1;

    c = *s++;
    if (c >= '0' && c <= '9')
      ch2 = (ch2 << 4) + c - '0';
    else if (c >= 'A' && c <= 'F')
      ch2 = (ch2 << 4) + c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
      ch2 = (ch2 << 4) + c - 'a' + 10;
    else
      return -1;

    c = *s++;
    if (c >= '0' && c <= '9')
      ch2 = (ch2 << 4) + c - '0';
    else if (c >= 'A' && c <= 'F')
      ch2 = (ch2 << 4) + c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
      ch2 = (ch2 << 4) + c - 'a' + 10;
    else
      return -1;
    if (ch2 < 0xDC00 || ch2 > 0xDFFF)
      return -1;
    ch = ((ch - 0xD800) << 10) + (ch2 - 0xDC00) + 0x10000;
  }

  *src = s;

  if (ch < 0x80) {
    dest[0] = (char)ch;
    return 1;
  }
  if (ch < 0x800) {
    dest[0] = (ch >> 6) | 0xC0;
    dest[1] = (ch & 0x3F) | 0x80;
    return 2;
  }
  if (ch < 0x10000) {
    dest[0] = (ch >> 12) | 0xE0;
    dest[1] = ((ch >> 6) & 0x3F) | 0x80;
    dest[2] = (ch & 0x3F) | 0x80;
    return 3;
  }
  if (ch < 0x110000) {
    dest[0] = (ch >> 18) | 0xF0;
    dest[1] = ((ch >> 12) & 0x3F) | 0x80;
    dest[2] = ((ch >> 6) & 0x3F) | 0x80;
    dest[3] = (ch & 0x3F) | 0x80;
    return 4;
  }
  return 0;
}

static inline char *_ac_json_decode(ac_pool_t *pool, char **eptr, char *s, char *p,
                                    size_t length) {
  char *sp;
  char *res = (char *)ac_pool_alloc(pool, length + 1);
  size_t pos = p - s;
  memcpy(res, s, pos);
  char *rp = res + pos;
  char *ep = s + length;
  while (p < ep) {
    int ch = *p++;
    if (ch != '\\')
      *rp++ = ch;
    else {
      ch = *p++;
      switch (ch) {
      case '\"':
        *rp++ = '\"';
        break;
      case '\\':
        *rp++ = '\\';
        break;
      case '/':
        *rp++ = '/';
        break;
      case 'b':
        *rp++ = 8;
        break;
      case 'f':
        *rp++ = 12;
        break;
      case 'n':
        *rp++ = 10;
        break;
      case 'r':
        *rp++ = 13;
        break;
      case 't':
        *rp++ = 9;
        break;
      case 'u':
        sp = p - 2;
        ch = unicode_to_utf8(rp, &p);
        if (ch == -1) {
          p = sp;
          *rp++ = *p++;
          *rp++ = *p++;
          *rp++ = *p++;
          *rp++ = *p++;
          *rp++ = *p++;
          *rp++ = *p++;
        } else
          rp += ch;
        break;
      }
    }
  }
  *rp = 0;
  *eptr = rp;
  return res;
}

char *_ac_json_encode(ac_pool_t *pool, char *s, char *p, size_t length) {
  char *res = (char *)ac_pool_alloc(pool, (length * 2) + 3);
  char *wp = res;
  memcpy(res, s, p - s);
  wp += (p - s);
  char *ep = s + length;
  while (p < ep) {
    switch (*p) {
    case '\"':
      *wp++ = '\\';
      *wp++ = '\"';
      p++;
      break;
    case '\\':
      *wp++ = '\\';
      *wp++ = '\\';
      p++;
      break;
    case '/':
      *wp++ = '\\';
      *wp++ = '/';
      p++;
      break;
    case '\b':
      *wp++ = '\\';
      *wp++ = 'b';
      p++;
      break;
    case '\f':
      *wp++ = '\\';
      *wp++ = 'f';
      p++;
      break;
    case '\n':
      *wp++ = '\\';
      *wp++ = 'n';
      p++;
      break;
    case '\r':
      *wp++ = '\\';
      *wp++ = 'r';
      p++;
      break;
    case '\t':
      *wp++ = '\\';
      *wp++ = 't';
      p++;
      break;
    default:
      *wp++ = *p++;
      break;
    }
  }
  *wp = 0;
  return res;
}

char *ac_json_encode(ac_pool_t *pool, char *s, size_t length) {
  char *p = s;
  char *ep = p + length;
  if(*ep != 0) {
    s = ac_pool_dup(pool, s, length+1);
    s[length] = 0;
    p = s;
    ep = p + length;
  }
  while (p < ep) {
    switch (*p) {
    case '\0':
    case '\"':
    case '\\':
    case '/':
    case '\b':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
      return _ac_json_encode(pool, s, p, length);
    default:
      p++;
    }
  }
  return s;
}

char *ac_json_decode(ac_pool_t *pool, char *s, size_t length) {
  char *p = s;
  char *ep = p + length;
  for (;;) {
    if (p == ep)
      return s;
    else if (*p == '\\')
      break;
    p++;
  }
  char *eptr = NULL;
  return _ac_json_decode(pool, &eptr, s, p, length);
}

char *ac_json_decode2(size_t *rlen, ac_pool_t *pool, char *s, size_t length) {
  char *p = s;
  char *ep = p + length;
  for (;;) {
    if (p == ep) {
      *rlen = length;
      return s;
    }
    else if (*p == '\\')
      break;
    p++;
  }
  char *eptr = NULL;
  char *r = _ac_json_decode(pool, &eptr, s, p, length);
  *rlen = eptr - r;
  return r;
}


void ac_json_dump_error_to_buffer(ac_buffer_t *bh, ac_json_t *j) {
  ac_json_error_t *err = (ac_json_error_t *)j;
  char *p = err->source;
  char *ep = err->error_at;
  int row = 1;
  char *srow = p;
  while (p < ep) {
    if (*p == '\\') {
      p += 2;
      continue;
    } else if (*p == '\n') {
      p++;
      row++;
      srow = p;
    } else
      p++;
  }
#ifdef AC_JSON_DEBUG
  ac_buffer_appendf(bh,
                    "Error at row %d, column: %ld (%ld bytes into json) "
                    "thrown from %d/%d\n",
                    row, (p - srow) + 1, ep - err->source, err->line,
                    err->line2);
#else
  ac_buffer_appendf(bh, "Error at row %d, column: %ld (%ld bytes into json)\n",
                    row, (p - srow) + 1, ep - err->source);
#endif
}

void ac_json_dump_error(FILE *out, ac_json_t *j) {
  ac_json_error_t *err = (ac_json_error_t *)j;
  char *p = err->source;
  char *ep = err->error_at;
  int row = 1;
  char *srow = p;
  while (p < ep) {
    if (*p == '\\') {
      p += 2;
      continue;
    } else if (*p == '\n') {
      p++;
      row++;
      srow = p;
    } else
      p++;
  }
#ifdef AC_JSON_DEBUG
  fprintf(out,
          "Error at row %d, column: %ld (%ld bytes into json) thrown from "
          "%d/%d\n",
          row, (p - srow) + 1, ep - err->source, err->line, err->line2);
#else
  fprintf(out, "Error at row %d, column: %ld (%ld bytes into json)\n", row,
          (p - srow) + 1, ep - err->source);
#endif
}

ac_json_t *ac_json_parse(ac_pool_t *pool, char *p, char *ep) {
#ifdef AC_JSON_DEBUG
  int line, line2 = 0;
#endif
  char *sp = p;
  char ch;
  char *key = NULL;
  char *stringp = NULL;
  ac_jsona_t *anode;
  _ac_jsono_t *obj;
  _ac_jsona_t *arr = NULL, *arr2;
  ac_json_t *j;
  ac_json_t *res = NULL;
  _ac_jsono_t *root = NULL;

  int data_type;
  uint32_t string_length;

  if (p >= ep) {
    p++;
    AC_JSON_BAD_CHARACTER;
  }

  if (*p != '{')
    goto start_value;
  root = (_ac_jsono_t *)ac_pool_calloc(pool, sizeof(_ac_jsono_t));
  root->type = AC_JSON_OBJECT;
  root->pool = pool;
  p++;
  res = (ac_json_t *)root;

start_key:;
  if (p >= ep) {
    AC_JSON_BAD_CHARACTER;
  }
  ch = *p++;
  switch (ch) {
  case '\"':
    key = p;
    goto get_end_of_key;
  case AC_JSON_SPACE_CASE:
    AC_JSON_START_KEY;
  case '}':
    // if (mode == AC_JSON_RDONLY)
#ifdef AC_JSON_FILL_TEST
#ifdef AC_JSON_SORT_TEST
    _ac_jsono_fill(root);
#else
    _ac_jsono_fill_tree(root);
#endif
#endif
    root = (_ac_jsono_t *)root->parent;
    if (!root)
      return res;

    ch = *p;
    if (root->type == AC_JSON_OBJECT) {
      goto look_for_key;
    } else {
      arr = (_ac_jsona_t *)root;
      goto look_for_next_object;
    }
  default:
    AC_JSON_BAD_CHARACTER;
  };

get_end_of_key:;
  while (p < ep && *p != '\"')
    p++;
  if (p < ep) {
    if (p[-1] == '\\') {
        // if odd number of \, then skip "
        char *backtrack = p-1;
        while(*backtrack == '\\')
            backtrack--;
        if(((p-backtrack) & 1) == 0) {
            p++;
            goto get_end_of_key;
        }
    }
    *p = 0;
  }
  p++;
  while (p < ep && *p != ':')
    p++;
  p++;

start_key_object:;
  if (p >= ep) {
    AC_JSON_BAD_CHARACTER;
  }
  ch = *p++;
  switch (ch) {
  case '\"':
    stringp = p;
    data_type = AC_JSON_STRING;
    AC_JSON_KEYED_START_STRING;
  case AC_JSON_SPACE_CASE:
    goto start_key_object;
  case '{':
    obj = (_ac_jsono_t *)ac_pool_calloc(pool, sizeof(_ac_jsono_t));
    obj->type = AC_JSON_OBJECT;
    obj->pool = pool;
    // obj->parent = (ac_json_t *)root;
    ac_jsono_append((ac_json_t *)root, key, (ac_json_t *)obj, false);

    root = obj;
    AC_JSON_START_KEY;
  case '[':
    arr = (_ac_jsona_t *)ac_pool_calloc(pool, sizeof(_ac_jsona_t));
    arr->type = AC_JSON_ARRAY;
    arr->pool = pool;
    // arr->parent = (ac_json_t *)root;
    ac_jsono_append((ac_json_t *)root, key, (ac_json_t *)arr, false);
    root = (_ac_jsono_t *)arr;
    AC_JSON_START_VALUE;
  case '-':
    stringp = p - 1;
    ch = *p++;
    if (ch >= '1' && ch <= '9') {
      AC_JSON_KEYED_NEXT_DIGIT;
    } else if (ch == '0') {
      ch = *p;
      if (p + 1 < ep) {
        if (ch != '.') {
          stringp = (char *)"0";
          data_type = AC_JSON_ZERO;
          string_length = 1;
          AC_JSON_KEYED_ADD_STRING;
        } else {
          p++;
          AC_JSON_KEYED_DECIMAL_NUMBER;
        }
      } else {
        stringp = (char *)"0";
        data_type = AC_JSON_ZERO;
        string_length = 1;
        AC_JSON_KEYED_ADD_STRING;
      }
    }
    AC_JSON_BAD_CHARACTER;
  case '0':
    stringp = p - 1;
    ch = *p;
    if (p + 1 < ep) {
      if (ch != '.') {
        stringp = (char *)"0";
        data_type = AC_JSON_ZERO;
        string_length = 1;
        AC_JSON_KEYED_ADD_STRING;
      } else {
        p++;
        AC_JSON_KEYED_DECIMAL_NUMBER;
      }
    } else {
      stringp = (char *)"0";
      data_type = AC_JSON_ZERO;
      string_length = 1;
      AC_JSON_KEYED_ADD_STRING;
    }
  case AC_JSON_NATURAL_NUMBER_CASE:
    stringp = p - 1;
    AC_JSON_KEYED_NEXT_DIGIT;
  case 't':
    if (p + 3 >= ep) {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'r') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'u') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'e') {
      AC_JSON_BAD_CHARACTER;
    }
    stringp = (char *)"true";
    data_type = AC_JSON_TRUE;
    string_length = 4;
    ch = *p;
    AC_JSON_KEYED_ADD_STRING;
  case 'f':
    if (p + 4 >= ep) {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'a') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'l') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 's') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'e') {
      AC_JSON_BAD_CHARACTER;
    }
    stringp = (char *)"false";
    ch = *p;
    data_type = AC_JSON_FALSE;
    string_length = 5;
    AC_JSON_KEYED_ADD_STRING;
  case 'n':
    ch = *p++;
    if (ch != 'u') {
      if (ch == 'b') {
        if (p + 3 >= ep) {
          AC_JSON_BAD_CHARACTER;
        }
        string_length = *(uint32_t *)(p);
        p += 4;
        if (p + string_length >= ep) {
          AC_JSON_BAD_CHARACTER;
        }
        data_type = AC_JSON_BINARY;
        stringp = p;
        p += string_length;
        ch = *p;
        AC_JSON_KEYED_ADD_STRING;
      }
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'l') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'l') {
      AC_JSON_BAD_CHARACTER;
    }
    stringp = (char *)"null";
    ch = *p;
    data_type = AC_JSON_NULL;
    string_length = 4;
    AC_JSON_KEYED_ADD_STRING;
  default:
    AC_JSON_BAD_CHARACTER;
  };

keyed_start_string:;
  while (p < ep && *p != '\"')
    p++;
  if (p >= ep) {
    AC_JSON_BAD_CHARACTER;
  }
  if (p[-1] == '\\') {
    // if odd number of \, then skip "
    char *backtrack = p-1;
    while(*backtrack == '\\')
        backtrack--;
    if(((p-backtrack) & 1) == 0) {
        p++;
        goto keyed_start_string;
    }
  }
  *p = 0;
  string_length = p - stringp;
  p++;
  ch = *p;

keyed_add_string:;
  j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  // j->parent = (ac_json_t *)root;
  j->type = data_type;
#ifdef AC_JSON_DECODE_TEST
  if (data_type == AC_JSON_STRING)
    j->value = ac_json_decode(pool, stringp, string_length);
  else
    j->value = stringp;
#else
  j->value = stringp;
#endif
  j->length = string_length;
  ac_jsono_append((ac_json_t *)root, key, j, false);

look_for_key:;
  switch (ch) {
  case ',':
    if (p >= ep) {
      AC_JSON_BAD_CHARACTER;
    }
    p++;
    AC_JSON_START_KEY;
  case '}':
// if (mode == AC_JSON_RDONLY)
#ifdef AC_JSON_FILL_TEST
#ifdef AC_JSON_SORT_TEST
    _ac_jsono_fill(root);
#else
    _ac_jsono_fill_tree(root);
#endif
#endif
    root = (_ac_jsono_t *)root->parent;
    if (!root)
      return res;

    p++;
    ch = *p;
    if (root->type == AC_JSON_OBJECT) {
      goto look_for_key;
    } else {
      arr = (_ac_jsona_t *)root;
      goto look_for_next_object;
    }

  case AC_JSON_SPACE_CASE:
    p++;
    ch = *p;
    goto look_for_key;
  default:
    AC_JSON_BAD_CHARACTER;
  };

keyed_next_digit:;
  /* add e/E support */
  ch = *p++;
  while (ch >= '0' && ch <= '9')
    ch = *p++;
  if (ch != '.') {
    p--;
    *p = 0;
    data_type = AC_JSON_NUMBER;
    string_length = p - stringp;
    AC_JSON_KEYED_ADD_STRING;
  }

keyed_decimal_number:;
  ch = *p++;
  if (ch < '0' || ch > '9') {
    AC_JSON_BAD_CHARACTER;
  }

  ch = *p++;
  while (ch >= '0' && ch <= '9')
    ch = *p++;

  p--;
  *p = 0;
  data_type = AC_JSON_DECIMAL;
  string_length = p - stringp;
  AC_JSON_KEYED_ADD_STRING;

start_value:;
  if (p >= ep) {
    AC_JSON_BAD_CHARACTER;
  }
  ch = *p++;
  switch (ch) {
  case '\"':
    stringp = p;
    data_type = AC_JSON_STRING;
    AC_JSON_START_STRING;
  case AC_JSON_SPACE_CASE:
    AC_JSON_START_VALUE;
  case '{':
    anode = (ac_jsona_t *)ac_pool_calloc(pool, sizeof(ac_jsona_t) +
                                                   sizeof(_ac_jsono_t));
    obj = (_ac_jsono_t *)(anode + 1);
    anode->value = (ac_json_t *)obj;
    if (arr) {
      arr->num_entries++;
      if (!arr->head)
        arr->head = arr->tail = anode;
      else {
        anode->previous = arr->tail;
        arr->tail->next = anode;
        arr->tail = anode;
      }
    } else
      res = (ac_json_t *)obj;
    obj->type = AC_JSON_OBJECT;
    obj->pool = pool;

    obj->parent = (ac_json_t *)arr;
    root = obj;
    AC_JSON_START_KEY;
  case '[':
    anode = (ac_jsona_t *)ac_pool_calloc(pool, sizeof(ac_jsona_t) +
                                                   sizeof(_ac_jsona_t));
    arr2 = (_ac_jsona_t *)(anode + 1);
    anode->value = (ac_json_t *)arr2;
    if (arr) {
      arr->num_entries++;
      if (!arr->head)
        arr->head = arr->tail = anode;
      else {
        anode->previous = arr->tail;
        arr->tail->next = anode;
        arr->tail = anode;
      }
    } else
      res = (ac_json_t *)arr2;
    arr2->type = AC_JSON_ARRAY;
    arr2->pool = pool;
    arr2->parent = (ac_json_t *)arr;
    arr = arr2;
    root = (_ac_jsono_t *)arr;
    AC_JSON_START_VALUE;
  case ']':
// if (mode == AC_JSON_RDONLY)
#ifdef AC_JSON_FILL_TEST
    _ac_jsona_fill(arr);
#endif
    root = (_ac_jsono_t *)arr->parent;
    if (!root)
      return res;
    ch = *p;
    if (root->type == AC_JSON_OBJECT) {
      goto look_for_key;
    } else {
      arr = (_ac_jsona_t *)root;
      goto look_for_next_object;
    }
  case '-':
    stringp = p - 1;
    ch = *p++;
    if (ch >= '1' && ch <= '9') {
      AC_JSON_NEXT_DIGIT;
    } else if (ch == '0') {
      ch = *p;
      if (p + 1 < ep) {
        if (ch != '.') {
          stringp = (char *)"0";
          data_type = AC_JSON_ZERO;
          string_length = 1;
          AC_JSON_ADD_STRING;
        } else {
          p++;
          AC_JSON_DECIMAL_NUMBER;
        }
      } else {
        stringp = (char *)"0";
        data_type = AC_JSON_ZERO;
        string_length = 1;
        AC_JSON_ADD_STRING;
      }
    }
    AC_JSON_BAD_CHARACTER;
  case '0':
    stringp = p - 1;
    ch = *p;
    if (p + 1 < ep) {
      if (ch != '.') {
        stringp = (char *)"0";
        data_type = AC_JSON_ZERO;
        string_length = 1;
        AC_JSON_ADD_STRING;
      } else {
        p++;
        AC_JSON_DECIMAL_NUMBER;
      }
    } else {
      stringp = (char *)"0";
      data_type = AC_JSON_ZERO;
      string_length = 1;
      AC_JSON_ADD_STRING;
    }
  case AC_JSON_NATURAL_NUMBER_CASE:
    stringp = p - 1;
    AC_JSON_NEXT_DIGIT;
  case 't':
    ch = *p++;
    if (ch != 'r') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'u') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'e') {
      AC_JSON_BAD_CHARACTER;
    }
    stringp = (char *)"true";
    data_type = AC_JSON_TRUE;
    string_length = 4;
    ch = *p;
    AC_JSON_ADD_STRING;
  case 'f':
    ch = *p++;
    if (ch != 'a') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'l') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 's') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'e') {
      AC_JSON_BAD_CHARACTER;
    }
    stringp = (char *)"false";
    data_type = AC_JSON_FALSE;
    string_length = 5;
    ch = *p;
    AC_JSON_ADD_STRING;
  case 'n':
    ch = *p++;
    if (ch != 'u') {
      if (ch == 'b') {
        if (p + 3 >= ep) {
          AC_JSON_BAD_CHARACTER;
        }
        string_length = *(uint32_t *)(p);
        p += 4;
        if (p + string_length >= ep) {
          AC_JSON_BAD_CHARACTER;
        }
        data_type = AC_JSON_BINARY;
        stringp = p;
        p += string_length;
        ch = *p;
        AC_JSON_ADD_STRING;
      }

      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'l') {
      AC_JSON_BAD_CHARACTER;
    }
    ch = *p++;
    if (ch != 'l') {
      AC_JSON_BAD_CHARACTER;
    }
    stringp = (char *)"null";
    data_type = AC_JSON_NULL;
    string_length = 4;
    ch = *p;
    AC_JSON_ADD_STRING;

  default:
    AC_JSON_BAD_CHARACTER;
  };

start_string:;
  while (p < ep && *p != '\"')
    p++;
  if (p >= ep) {
    AC_JSON_BAD_CHARACTER;
  }
  if (p[-1] == '\\') {
    // if odd number of \, then skip "
    char *backtrack = p-1;
    while(*backtrack == '\\')
        backtrack--;
    if(((p-backtrack) & 1) == 0) {
        p++;
        goto start_string;
    }
  }
  *p = 0;
  string_length = p - stringp;
  p++;
  ch = *p;

add_string:;
  anode = (ac_jsona_t *)ac_pool_calloc(pool,
                                       sizeof(ac_jsona_t) + sizeof(ac_json_t));
  j = anode->value = (ac_json_t *)(anode + 1);
  j->type = data_type;
#ifdef AC_JSON_DECODE_TEST
  if (data_type == AC_JSON_STRING)
    j->value = ac_json_decode(pool, stringp, string_length);
  else
    j->value = stringp;
#else
  j->value = stringp;
#endif
  j->length = string_length;
  j->parent = (ac_json_t *)arr;

  if (!arr)
    return j;

  arr->num_entries++;
  if (!arr->head)
    arr->head = arr->tail = anode;
  else {
    anode->previous = arr->tail;
    arr->tail->next = anode;
    arr->tail = anode;
  }

look_for_next_object:;
  switch (ch) {
  case ',':
    if (p >= ep) {
      AC_JSON_BAD_CHARACTER;
    }
    p++;
    AC_JSON_START_VALUE;
  case ']':
// if (mode == AC_JSON_RDONLY)
#ifdef AC_JSON_FILL_TEST
    _ac_jsona_fill(arr);
#endif
    root = (_ac_jsono_t *)arr->parent;
    if (!root)
      return res;
    p++;
    ch = *p;
    if (root->type == AC_JSON_OBJECT) {
      goto look_for_key;
    } else {
      arr = (_ac_jsona_t *)root;
      goto look_for_next_object;
    }
  case AC_JSON_SPACE_CASE:
    p++;
    ch = *p;
    goto look_for_next_object;
  default:
    AC_JSON_BAD_CHARACTER;
  };

next_digit:;
  /* add e/E support */
  ch = *p++;
  while (ch >= '0' && ch <= '9')
    ch = *p++;
  if (ch != '.') {
    p--;
    *p = 0;
    data_type = AC_JSON_NUMBER;
    string_length = p - stringp;
    AC_JSON_ADD_STRING;
  }

decimal_number:;
  ch = *p++;
  if (ch < '0' || ch > '9') {
    AC_JSON_BAD_CHARACTER;
  }

  ch = *p++;
  while (ch >= '0' && ch <= '9')
    ch = *p++;

  p--;
  *p = 0;
  data_type = AC_JSON_DECIMAL;
  string_length = p - stringp;
  AC_JSON_ADD_STRING;

bad_character:;
  ac_json_error_t *err =
      (ac_json_error_t *)ac_pool_alloc(pool, sizeof(ac_json_error_t));
  err->type = AC_JSON_ERROR;
#ifdef AC_JSON_DEBUG
  err->line = line;
  err->line2 = line2;
#else
  err->line = 0;
  err->line2 = 0;
#endif
  err->error_at = p;
  err->source = sp;
  return (ac_json_t *)err;
}

static inline int ac_json_compare(const ac_jsono_t **a, const ac_jsono_t **b) {
  return strcmp((*a)->key, (*b)->key);
}

ac_sort_m(__ac_json_sort, ac_jsono_t *, ac_json_compare);

void _ac_jsono_fill(_ac_jsono_t *o) {
  o->root = (ac_map_t *)ac_pool_alloc(
      o->pool, (sizeof(ac_jsono_t *) * (o->num_entries + 1)));
  ac_jsono_t **base = (ac_jsono_t **)o->root;
  ac_jsono_t **awp = base;
  ac_jsono_t *n = o->head;
  while (n) {
    *awp++ = n;
    n = n->next;
  }
  o->num_sorted_entries = awp - base;
  if (o->num_sorted_entries)
    __ac_json_sort(base, o->num_sorted_entries);
  else
    o->root = NULL;
}
