#ifndef _ac_serve_fill_H
#define _ac_serve_fill_H

#include <inttypes.h>

static const char access_control_headers_s[] =
  "\nAccess-Control-Allow-Headers: Origin, X-Atmosphere-tracking-id, "
  "X-Atmosphere-Framework, X-Cache-Date, Content-Type, "
  "X-Atmosphere-Transport, *\r\nAccess-Control-Allow-Methods: POST, GET, "
  "OPTIONS, PUT\r\nAccess-Control-Allow-Origin: "
  "*\r\nAccess-Control-Request-Headers: Origin, "
  "X-Atmosphere-tracking-id, X-Atmosphere-Framework, X-Cache-Date, "
  "Content-Type, X-Atmosphere-Transport, *\r\n";

static inline char *fill_default_access_control_headers(char *p) {
  p--;
  uint64_t *sh = (uint64_t *)access_control_headers_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  return (char *)wp;
}

static char content_type_s[] = "\r\nContent-type: ";

static inline char *fill_content_type(char *p, const char *content_type) {
  p -= 2;
  uint64_t *sh = (uint64_t *)content_type_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  sh = (uint64_t *)content_type;
  size_t len = strlen(content_type);
  content_type += len;
  while (len > 8) {
    *wp++ = *sh++;
    len -= 8;
  }
  p = (char *)wp;
  content_type -= len;
  while (*content_type)
    *p++ = *content_type++;
  *p++ = '\r';
  *p++ = '\n';
  return p;
}

static char anotherclibrary_s[] = "Server: AnotherCLibrary\r\n";

static inline char *fill_anotherclibrary(char *p) {
  uint64_t *sh = (uint64_t *)anotherclibrary_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = (char *)wp;
  *p++ = '\n';
  return p;
}

static inline char *u64_to_str(uint64_t num, char *str) {
  if (!num) {
    *str = '0';
    return str + 1;
  }
  char *p = str;
  while (num) {
    uint64_t digit = num % 10;
    *p++ = digit + '0';
    num /= 10;
  }
  char *ep = p - 1;
  while (str < ep) {
    char tmp = *str;
    *str = *ep;
    *ep = tmp;
    str++;
    ep--;
  }
  return p;
}

static char timing_s[] = "Timing: ";

static inline char *fill_timing(char *p, uint64_t ts) {
  uint64_t *sh = (uint64_t *)timing_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh;
  p = u64_to_str(ts, (char *)wp);
  *p++ = '\r';
  *p++ = '\n';
  return p;
}

static char http_s[] = "HTTP/1.1";

static inline char *fill_status_line(char *p, const char *status_line,
                                     const char *date_line) {
  uint64_t *wp = (uint64_t *)p;
  uint64_t *sh = (uint64_t *)http_s;
  *wp = *sh;
  p += 8;
  *p++ = ' ';
  while (*status_line)
    *p++ = *status_line++;
  *p++ = '\r';
  *p++ = '\n';
  wp = (uint64_t *)p;
  sh = (uint64_t *)date_line;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = (char *)wp;
  return p;
}

static const char content_length_s[] = "Content-length: ";

static inline char *fill_content_length(char *p, uint64_t len) {
  uint64_t *sh = (uint64_t *)content_length_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = u64_to_str(len, (char *)wp);
  *p++ = '\r';
  *p++ = '\n';
  return p;
}

/* The XXXX is chopped off at end */
static const char chunk_encoded_s[] = "Transfer-Encoding: chunked\r\nXXXX";

static inline char *fill_chunk_encoded(char *p) {
  uint64_t *sh = (uint64_t *)chunk_encoded_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = (char *)wp;
  return p - 4;
}

static const char keep_alive_s[] = "Connection: keep-alive\r\n";

static inline char *fill_keep_alive(char *p) {
  uint64_t *sh = (uint64_t *)keep_alive_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  return (char *)wp;
}

// Thre ad-I D: 0 0000 00\r\n
static char date_s[] =
    "Date: Fri, 26 Aug 2011 00:31:53 GMT\r\nThread-ID: 000001\r\n";

static inline char *fill_header(serve_request_t *sr, const char *status_line,
                                uint64_t ts, const char *content_type,
                                int keep_alive) {
  char *p = sr->header;
  p = fill_status_line(p, status_line, sr->request.service->date);
  p = fill_default_access_control_headers(p);
  p = fill_anotherclibrary(p);
  p = fill_timing(p, ts);
  if (content_type)
    p = fill_content_type(p, content_type);
  if (keep_alive)
    p = fill_keep_alive(p);

  if (sr->request.chunk_encoded)
    p = fill_chunk_encoded(p);
  else
    p = fill_content_length(p, sr->request.output.len);

  *p = 0;
  return p;
}

#endif
