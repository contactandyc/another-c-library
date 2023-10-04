#include "another-c-library/ac-utils/ac_csv.h"

char *ac_csv_encode(ac_pool_t *h, char delim, const char *s) {
    char *p = (char *)s;
    while(*p) {
        if(*p == delim || *p == '\n' || *p == '\"')
            break;
        p++;
    }
    if(*p == 0)
        return (char *)s;

    int num = 0;
    p = (char *)s;
    while(*p) {
        if(*p == '\"')
            num++;
        p++;
    }
    char *res = (char *)ac_pool_alloc(h, (p-s)+3+num);
    char *wp = res;
    *wp++ = '\"';
    p = (char *)s;
    while(*p) {
        if(*p == '\"')
            *wp++ = '\"';
        *wp++ = *p++;
    }
    *wp++ = '\"';
    *wp = 0;
    return res;
}

char *ac_csv_encodef(ac_pool_t *h, char delim, const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return ac_csv_encode(h, delim, r);
}

char **_ac_csv_split(ac_pool_t *h, size_t *num_splits, char delim,
                          char *s) {
  static char *nil = NULL;
  if (!s) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }
  char *p = s;
  size_t num = 1;
  while (*p != 0) {
    if (*p == '\"') {
      p++;
      encoded_quote:
      while(*p != 0 && *p != '\"')
        p++;
      if(*p != 0 && p[1] == '\"') {
        p += 2;
        goto encoded_quote;
      }
    }
    else if (*p == delim)
      num++;
    p++;
  }
  if (num_splits)
    *num_splits = num;
  char **r = (char **)ac_pool_alloc(h, sizeof(char *) * (num + 1));
  char **wr = r;
  *wr = s;
  char *wp = s;
  wr++;
    // ,,, => "","","",""

  while (*s != 0) {
    if (*s == '\"') {
      s++;
      encoded_quote2:
      while(*s != 0 && *s != '\"') {
        *wp++ = *s;
        s++;
      }
      if(*s != 0 && s[1] == '\"') {
        *wp++ = *s;
        s += 2;
        goto encoded_quote2;
      }
      s++;
    }
    else if (*s == delim) {
      *wp = 0;
      s++;
      *wr = s;
      wp = s;
      wr++;
    } else
      *wp++ = *s++;
  }
  *wp = 0;
  *wr = NULL;
  return r;
}

char **ac_csv_split(ac_pool_t *h, size_t *num_splits, char delim,
                         const char *p) {
  return _ac_csv_split(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_csv_splitf(ac_pool_t *h, size_t *num_splits, char delim,
                          const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_csv_split(h, num_splits, delim, r);
}

char **_ac_csv_split2(ac_pool_t *h, size_t *num_splits, char delim, char *s) {
  size_t num_res = 0;
  char **res = _ac_csv_split(h, &num_res, delim, s);
  char **wp = res;
  char **p = res;
  char **ep = p+num_res;
  while(p < ep) {
    if(*p[0] != 0)
        *wp++ = *p;
    p++;
  }
  *num_splits = wp-res;
  *wp++ = NULL;
  return res;
}

char **ac_csv_split2(ac_pool_t *h, size_t *num_splits, char delim,
                          const char *p) {
  return _ac_csv_split2(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_csv_split2f(ac_pool_t *h, size_t *num_splits, char delim,
                           const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_csv_split2(h, num_splits, delim, r);
}