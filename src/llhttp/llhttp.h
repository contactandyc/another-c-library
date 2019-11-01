#ifndef INCLUDE_LLHTTP_H_
#define INCLUDE_LLHTTP_H_

#define LLHTTP_VERSION_MAJOR 1
#define LLHTTP_VERSION_MINOR 1
#define LLHTTP_VERSION_PATCH 4

#ifndef INCLUDE_LLHTTP_ITSELF_H_
#define INCLUDE_LLHTTP_ITSELF_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Informational 1xx */
#define HTTP_STATUS_100 "100 Continue"
#define HTTP_STATUS_101 "101 Switching Protocols"
#define HTTP_STATUS_102 "102 Processing"
/* Successful 2xx */
#define HTTP_STATUS_200 "200 OK"
#define HTTP_STATUS_201 "201 Created"
#define HTTP_STATUS_202 "202 Accepted"
#define HTTP_STATUS_203 "203 Non-Authoritative Information"
#define HTTP_STATUS_204 "204 No Content"
#define HTTP_STATUS_205 "205 Reset Content"
#define HTTP_STATUS_206 "206 Partial Content"
#define HTTP_STATUS_207 "207 Multi-Status"
/* Redirection 3xx */
#define HTTP_STATUS_300 "300 Multiple Choices"
#define HTTP_STATUS_301 "301 Moved Permanently"
#define HTTP_STATUS_302 "302 Moved Temporarily"
#define HTTP_STATUS_303 "303 See Other"
#define HTTP_STATUS_304 "304 Not Modified"
#define HTTP_STATUS_305 "305 Use Proxy"
#define HTTP_STATUS_307 "307 Temporary Redirect"
/* Client Error 4xx */
#define HTTP_STATUS_400 "400 Bad Request"
#define HTTP_STATUS_401 "401 Unauthorized"
#define HTTP_STATUS_402 "402 Payment Required"
#define HTTP_STATUS_403 "403 Forbidden"
#define HTTP_STATUS_404 "404 Not Found"
#define HTTP_STATUS_405 "405 Method Not Allowed"
#define HTTP_STATUS_406 "406 Not Acceptable"
#define HTTP_STATUS_407 "407 Proxy Authentication Required"
#define HTTP_STATUS_408 "408 Request Time-out"
#define HTTP_STATUS_409 "409 Conflict"
#define HTTP_STATUS_410 "410 Gone"
#define HTTP_STATUS_411 "411 Length Required"
#define HTTP_STATUS_412 "412 Precondition Failed"
#define HTTP_STATUS_413 "413 Request Entity Too Large"
#define HTTP_STATUS_414 "414 Request-URI Too Large"
#define HTTP_STATUS_415 "415 Unsupported Media Type"
#define HTTP_STATUS_416 "416 Requested Range Not Satisfiable"
#define HTTP_STATUS_417 "417 Expectation Failed"
#define HTTP_STATUS_418 "418 I'm a teapot"
#define HTTP_STATUS_422 "422 Unprocessable Entity"
#define HTTP_STATUS_423 "423 Locked"
#define HTTP_STATUS_424 "424 Failed Dependency"
#define HTTP_STATUS_425 "425 Unordered Collection"
#define HTTP_STATUS_426 "426 Upgrade Required"
#define HTTP_STATUS_428 "428 Precondition Required"
#define HTTP_STATUS_429 "429 Too Many Requests"
#define HTTP_STATUS_431 "431 Request Header Fields Too Large"
/* Server Error 5xx */
#define HTTP_STATUS_500 "500 Internal Server Error"
#define HTTP_STATUS_501 "501 Not Implemented"
#define HTTP_STATUS_502 "502 Bad Gateway"
#define HTTP_STATUS_503 "503 Service Unavailable"
#define HTTP_STATUS_504 "504 Gateway Time-out"
#define HTTP_STATUS_505 "505 HTTP Version Not Supported"
#define HTTP_STATUS_506 "506 Variant Also Negotiates"
#define HTTP_STATUS_507 "507 Insufficient Storage"
#define HTTP_STATUS_509 "509 Bandwidth Limit Exceeded"
#define HTTP_STATUS_510 "510 Not Extended"
#define HTTP_STATUS_511 "511 Network Authentication Required"

typedef struct llhttp__internal_s llhttp__internal_t;
typedef llhttp__internal_t llhttp_t;
typedef struct llhttp_settings_s llhttp_settings_t;

typedef int (*llhttp_data_cb)(llhttp_t *, const char *at, size_t length);
typedef int (*llhttp_cb)(llhttp_t *);

typedef int (*llhttp_ucb)(llhttp_t *s, const unsigned char *p,
                          const unsigned char *endp);

struct llhttp__internal_s {
  int32_t _index;
  unsigned char *_span_pos0;
  llhttp_ucb _span_cb0;
  int32_t error;
  const char *reason;
  const char *error_pos;
  void *data;
  void *_current;
  uint64_t content_length;
  uint8_t type;
  uint8_t method;
  uint8_t http_major;
  uint8_t http_minor;
  uint8_t header_state;
  uint8_t flags;
  uint8_t upgrade;
  uint16_t status_code;
  uint8_t finish;
  void *settings;
};

int llhttp__internal_init(llhttp__internal_t *s);
int llhttp__internal_execute(llhttp__internal_t *s, const char *p,
                             const char *endp);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* INCLUDE_LLHTTP_ITSELF_H_ */

#ifndef LLLLHTTP_C_HEADERS_
#define LLLLHTTP_C_HEADERS_
#ifdef __cplusplus
extern "C" {
#endif

enum llhttp_errno {
  HPE_OK = 0,
  HPE_INTERNAL = 1,
  HPE_STRICT = 2,
  HPE_LF_EXPECTED = 3,
  HPE_UNEXPECTED_CONTENT_LENGTH = 4,
  HPE_CLOSED_CONNECTION = 5,
  HPE_INVALID_METHOD = 6,
  HPE_INVALID_URL = 7,
  HPE_INVALID_CONSTANT = 8,
  HPE_INVALID_VERSION = 9,
  HPE_INVALID_HEADER_TOKEN = 10,
  HPE_INVALID_CONTENT_LENGTH = 11,
  HPE_INVALID_CHUNK_SIZE = 12,
  HPE_INVALID_STATUS = 13,
  HPE_INVALID_EOF_STATE = 14,
  HPE_CB_MESSAGE_BEGIN = 15,
  HPE_CB_HEADERS_COMPLETE = 16,
  HPE_CB_MESSAGE_COMPLETE = 17,
  HPE_CB_CHUNK_HEADER = 18,
  HPE_CB_CHUNK_COMPLETE = 19,
  HPE_PAUSED = 20,
  HPE_PAUSED_UPGRADE = 21,
  HPE_USER = 22
};
typedef enum llhttp_errno llhttp_errno_t;

enum llhttp_flags {
  F_CONNECTION_KEEP_ALIVE = 0x1,
  F_CONNECTION_CLOSE = 0x2,
  F_CONNECTION_UPGRADE = 0x4,
  F_CHUNKED = 0x8,
  F_UPGRADE = 0x10,
  F_CONTENT_LENGTH = 0x20,
  F_SKIPBODY = 0x40,
  F_TRAILING = 0x80
};
typedef enum llhttp_flags llhttp_flags_t;

enum llhttp_type { HTTP_BOTH = 0, HTTP_REQUEST = 1, HTTP_RESPONSE = 2 };
typedef enum llhttp_type llhttp_type_t;

enum llhttp_finish {
  HTTP_FINISH_SAFE = 0,
  HTTP_FINISH_SAFE_WITH_CB = 1,
  HTTP_FINISH_UNSAFE = 2
};
typedef enum llhttp_finish llhttp_finish_t;

enum llhttp_method {
  HTTP_DELETE = 0,
  HTTP_GET = 1,
  HTTP_HEAD = 2,
  HTTP_POST = 3,
  HTTP_PUT = 4,
  HTTP_CONNECT = 5,
  HTTP_OPTIONS = 6,
  HTTP_TRACE = 7,
  HTTP_COPY = 8,
  HTTP_LOCK = 9,
  HTTP_MKCOL = 10,
  HTTP_MOVE = 11,
  HTTP_PROPFIND = 12,
  HTTP_PROPPATCH = 13,
  HTTP_SEARCH = 14,
  HTTP_UNLOCK = 15,
  HTTP_BIND = 16,
  HTTP_REBIND = 17,
  HTTP_UNBIND = 18,
  HTTP_ACL = 19,
  HTTP_REPORT = 20,
  HTTP_MKACTIVITY = 21,
  HTTP_CHECKOUT = 22,
  HTTP_MERGE = 23,
  HTTP_MSEARCH = 24,
  HTTP_NOTIFY = 25,
  HTTP_SUBSCRIBE = 26,
  HTTP_UNSUBSCRIBE = 27,
  HTTP_PATCH = 28,
  HTTP_PURGE = 29,
  HTTP_MKCALENDAR = 30,
  HTTP_LINK = 31,
  HTTP_UNLINK = 32,
  HTTP_SOURCE = 33
};
typedef enum llhttp_method llhttp_method_t;

#define HTTP_ERRNO_MAP(XX)                                                     \
  XX(0, OK, OK)                                                                \
  XX(1, INTERNAL, INTERNAL)                                                    \
  XX(2, STRICT, STRICT)                                                        \
  XX(3, LF_EXPECTED, LF_EXPECTED)                                              \
  XX(4, UNEXPECTED_CONTENT_LENGTH, UNEXPECTED_CONTENT_LENGTH)                  \
  XX(5, CLOSED_CONNECTION, CLOSED_CONNECTION)                                  \
  XX(6, INVALID_METHOD, INVALID_METHOD)                                        \
  XX(7, INVALID_URL, INVALID_URL)                                              \
  XX(8, INVALID_CONSTANT, INVALID_CONSTANT)                                    \
  XX(9, INVALID_VERSION, INVALID_VERSION)                                      \
  XX(10, INVALID_HEADER_TOKEN, INVALID_HEADER_TOKEN)                           \
  XX(11, INVALID_CONTENT_LENGTH, INVALID_CONTENT_LENGTH)                       \
  XX(12, INVALID_CHUNK_SIZE, INVALID_CHUNK_SIZE)                               \
  XX(13, INVALID_STATUS, INVALID_STATUS)                                       \
  XX(14, INVALID_EOF_STATE, INVALID_EOF_STATE)                                 \
  XX(15, CB_MESSAGE_BEGIN, CB_MESSAGE_BEGIN)                                   \
  XX(16, CB_HEADERS_COMPLETE, CB_HEADERS_COMPLETE)                             \
  XX(17, CB_MESSAGE_COMPLETE, CB_MESSAGE_COMPLETE)                             \
  XX(18, CB_CHUNK_HEADER, CB_CHUNK_HEADER)                                     \
  XX(19, CB_CHUNK_COMPLETE, CB_CHUNK_COMPLETE)                                 \
  XX(20, PAUSED, PAUSED)                                                       \
  XX(21, PAUSED_UPGRADE, PAUSED_UPGRADE)                                       \
  XX(22, USER, USER)

#define HTTP_METHOD_MAP(XX)                                                    \
  XX(0, DELETE, DELETE)                                                        \
  XX(1, GET, GET)                                                              \
  XX(2, HEAD, HEAD)                                                            \
  XX(3, POST, POST)                                                            \
  XX(4, PUT, PUT)                                                              \
  XX(5, CONNECT, CONNECT)                                                      \
  XX(6, OPTIONS, OPTIONS)                                                      \
  XX(7, TRACE, TRACE)                                                          \
  XX(8, COPY, COPY)                                                            \
  XX(9, LOCK, LOCK)                                                            \
  XX(10, MKCOL, MKCOL)                                                         \
  XX(11, MOVE, MOVE)                                                           \
  XX(12, PROPFIND, PROPFIND)                                                   \
  XX(13, PROPPATCH, PROPPATCH)                                                 \
  XX(14, SEARCH, SEARCH)                                                       \
  XX(15, UNLOCK, UNLOCK)                                                       \
  XX(16, BIND, BIND)                                                           \
  XX(17, REBIND, REBIND)                                                       \
  XX(18, UNBIND, UNBIND)                                                       \
  XX(19, ACL, ACL)                                                             \
  XX(20, REPORT, REPORT)                                                       \
  XX(21, MKACTIVITY, MKACTIVITY)                                               \
  XX(22, CHECKOUT, CHECKOUT)                                                   \
  XX(23, MERGE, MERGE)                                                         \
  XX(24, MSEARCH, M - SEARCH)                                                  \
  XX(25, NOTIFY, NOTIFY)                                                       \
  XX(26, SUBSCRIBE, SUBSCRIBE)                                                 \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE)                                             \
  XX(28, PATCH, PATCH)                                                         \
  XX(29, PURGE, PURGE)                                                         \
  XX(30, MKCALENDAR, MKCALENDAR)                                               \
  XX(31, LINK, LINK)                                                           \
  XX(32, UNLINK, UNLINK)                                                       \
  XX(33, SOURCE, SOURCE)

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* LLLLHTTP_C_HEADERS_ */

#ifndef INCLUDE_LLHTTP_API_H_
#define INCLUDE_LLHTTP_API_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

struct llhttp_settings_s {
  /* Possible return values 0, -1, `HPE_PAUSED` */
  llhttp_cb on_message_begin;

  llhttp_data_cb on_url;
  llhttp_data_cb on_status;
  llhttp_data_cb on_header_field;
  llhttp_data_cb on_header_value;

  /* Possible return values:
   * 0  - Proceed normally
   * 1  - Assume that request/response has no body, and proceed to parsing the
   *      next message
   * 2  - Assume absence of body (as above) and make `llhttp_execute()` return
   *      `HPE_PAUSED_UPGRADE`
   * -1 - Error
   * `HPE_PAUSED`
   */
  llhttp_cb on_headers_complete;

  llhttp_data_cb on_body;

  /* Possible return values 0, -1, `HPE_PAUSED` */
  llhttp_cb on_message_complete;

  /* When on_chunk_header is called, the current chunk length is stored
   * in parser->content_length.
   * Possible return values 0, -1, `HPE_PAUSED`
   */
  llhttp_cb on_chunk_header;
  llhttp_cb on_chunk_complete;
};

/* Initialize the parser with specific type and user settings */
void llhttp_init(llhttp_t *parser, llhttp_type_t type,
                 const llhttp_settings_t *settings);

/* Initialize the settings object */
void llhttp_settings_init(llhttp_settings_t *settings);

/* Parse full or partial request/response, invoking user callbacks along the
 * way.
 *
 * If any of `llhttp_data_cb` returns errno not equal to `HPE_OK` - the parsing
 * interrupts, and such errno is returned from `llhttp_execute()`. If
 * `HPE_PAUSED` was used as a errno, the execution can be resumed with
 * `llhttp_resume()` call.
 *
 * In a special case of CONNECT/Upgrade request/response `HPE_PAUSED_UPGRADE`
 * is returned after fully parsing the request/response. If the user wishes to
 * continue parsing, they need to invoke `llhttp_resume_after_upgrade()`.
 *
 * NOTE: if this function ever returns a non-pause type error, it will continue
 * to return the same error upon each successive call up until `llhttp_init()`
 * is called.
 */
llhttp_errno_t llhttp_execute(llhttp_t *parser, const char *data, size_t len);

/* This method should be called when the other side has no further bytes to
 * send (e.g. shutdown of readable side of the TCP connection.)
 *
 * Requests without `Content-Length` and other messages might require treating
 * all incoming bytes as the part of the body, up to the last byte of the
 * connection. This method will invoke `on_message_complete()` callback if the
 * request was terminated safely. Otherwise a error code would be returned.
 */
llhttp_errno_t llhttp_finish(llhttp_t *parser);

/* Returns `1` if the incoming message is parsed until the last byte, and has
 * to be completed by calling `llhttp_finish()` on EOF
 */
int llhttp_message_needs_eof(const llhttp_t *parser);

/* Returns `1` if there might be any other messages following the last that was
 * successfuly parsed.
 */
int llhttp_should_keep_alive(const llhttp_t *parser);

/* Make further calls of `llhttp_execute()` return `HPE_PAUSED` and set
 * appropriate error reason.
 *
 * Important: do not call this from user callbacks! User callbacks must return
 * `HPE_PAUSED` if pausing is required.
 */
void llhttp_pause(llhttp_t *parser);

/* Might be called to resume the execution after the pause in user's callback.
 * See `llhttp_execute()` above for details.
 *
 * Call this only if `llhttp_execute()` returns `HPE_PAUSED`.
 */
void llhttp_resume(llhttp_t *parser);

/* Might be called to resume the execution after the pause in user's callback.
 * See `llhttp_execute()` above for details.
 *
 * Call this only if `llhttp_execute()` returns `HPE_PAUSED_UPGRADE`
 */
void llhttp_resume_after_upgrade(llhttp_t *parser);

/* Returns the latest return error */
llhttp_errno_t llhttp_get_errno(const llhttp_t *parser);

/* Returns the verbal explanation of the latest returned error.
 *
 * Note: User callback should set error reason when returning the error. See
 * `llhttp_set_error_reason()` for details.
 */
const char *llhttp_get_error_reason(const llhttp_t *parser);

/* Assign verbal description to the returned error. Must be called in user
 * callbacks right before returning the errno.
 *
 * Note: `HPE_USER` error code might be useful in user callbacks.
 */
void llhttp_set_error_reason(llhttp_t *parser, const char *reason);

/* Returns the pointer to the last parsed byte before the returned error. The
 * pointer is relative to the `data` argument of `llhttp_execute()`.
 *
 * Note: this method might be useful for counting the number of parsed bytes.
 */
const char *llhttp_get_error_pos(const llhttp_t *parser);

/* Returns textual name of error code */
const char *llhttp_errno_name(llhttp_errno_t err);

/* Returns textual name of HTTP method */
const char *llhttp_method_name(llhttp_method_t method);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* INCLUDE_LLHTTP_API_H_ */

#endif /* INCLUDE_LLHTTP_H_ */
