#include "another-c-library/ac-utils/ac_split.h"

char **_ac_split_by_str(ac_pool_t *h, size_t *num_splits, const char *delim, char *s) {
  size_t delim_len = strlen(delim);
  if(delim_len == 0) {
    *num_splits = 1;
    char **r = (char **)ac_pool_alloc(h, sizeof(char *)*2);
    r[0] = s;
    r[1] = NULL;
    return r;
  }
  else if(delim_len == 1)
    return _ac_pool_split(h, num_splits, delim[0], s);

  delim_len--;
  static char *nil = NULL;
  if (!s) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }
  char *p = s;
  size_t num = 1;
  while (*p != 0) {
    if (*p == *delim && !strncmp(delim+1, p+1, delim_len)) {
      p += delim_len;
      num++;
    }
    p++;
  }
  if (num_splits)
    *num_splits = num;
  char **r = (char **)ac_pool_alloc(h, sizeof(char *) * (num + 1));
  char **wr = r;
  *wr = s;
  wr++;
  while (*s != 0) {
    if (*s == *delim && !strncmp(delim+1, s+1, delim_len)) {
      *s = 0;
      s += delim_len+1;
      *wr = s;
      wr++;
    } else
      s++;
  }
  *wr = NULL;
  return r;
}

char **ac_split_by_str(ac_pool_t *h, size_t *num_splits, const char *delim,
                         const char *p) {
  return _ac_split_by_str(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_split_by_strf(ac_pool_t *h, size_t *num_splits, const char *delim,
                      const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_split_by_str(h, num_splits, delim, r);
}

char **_ac_split_by_str2(ac_pool_t *h, size_t *num_splits, const char *delim, char *s) {
  size_t num_res = 0;
  char **res = _ac_split_by_str(h, &num_res, delim, s);
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

char **ac_split_by_str2(ac_pool_t *h, size_t *num_splits, const char *delim,
                      const char *p) {
  return _ac_split_by_str2(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_split_by_str2f(ac_pool_t *h, size_t *num_splits, const char *delim,
                       const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_split_by_str2(h, num_splits, delim, r);
}


static inline void setup_tokenize(uint64_t *bits, const char *delim) {
  bits[0] = bits[1] = 0;
  const char *p = delim;
  while (true) {
    if (*p > 0) {
      if (*p < 64)
        bits[0] |= (1 << (*p));
      else
        bits[1] |= (1 << ((*p) - 64));
    } else if (*p == 0)
      break;

    p++;
  }
}

static inline char *skip_delimiter(uint64_t *bits, char *p) {
  while (*p > 0) {
    if (*p < 64) {
      if (bits[0] & (1 << (*p)))
        p++;
      else
        return p;
    } else {
      if (bits[1] & (1 << ((*p) - 64)))
        p++;
      else
        return p;
    }
  }
  return p;
}

char **_ac_split_by_chars(ac_pool_t *h, size_t *num_splits, const char *delim,
                          char *s) {
  static char *nil = NULL;
  if (!s) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }
  uint64_t bits[2];
  setup_tokenize(bits, delim);
  char *p = skip_delimiter(bits, s);
  s = p;
  if (*p == 0) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }

  size_t num = 1;
  while (true) {
    if (*p > 0) {
      if (*p < 64) {
        if (bits[0] & (1 << (*p))) {
          num++;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0) {
            num--;
            break;
          }
        } else
          p++;
      } else {
        if (bits[1] & (1 << ((*p) - 64))) {
          num++;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0) {
            num--;
            break;
          }
        } else
          p++;
      }
    } else if (*p == 0)
      break;
    else
      p++;
  }
  if (num_splits)
    *num_splits = num;
  char **r = (char **)ac_pool_alloc(h, sizeof(char *) * (num + 1));
  char **wr = r;
  *wr = s;
  p = s;
  wr++;
  while (true) {
    if (*p > 0) {
      if (*p < 64) {
        if (bits[0] & (1 << (*p))) {
          *p = 0;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0)
            break;
          *wr = p;
          wr++;
        } else
          p++;
      } else {
        if (bits[1] & (1 << ((*p) - 64))) {
          *p = 0;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0)
            break;
          *wr = p;
          wr++;
        } else
          p++;
      }
    } else if (*p == 0)
      break;
    else
      p++;
  }

  *wr = NULL;
  return r;
}

char **ac_split_by_chars(ac_pool_t *h, size_t *num_splits, const char *delim,
                        const char *s) {
  return _ac_split_by_chars(h, num_splits, delim,
                            s ? ac_pool_strdup(h, s) : NULL);
}
