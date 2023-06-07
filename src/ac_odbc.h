#ifndef _ac_odbc_h
#define _ac_odbc_h

#include <stdbool.h>
#include <sql.h>
#include <inttypes.h>

struct ac_odbc_s;
typedef struct ac_odbc_s ac_odbc_t;
struct ac_odbc_conn_s;
typedef struct ac_odbc_conn_s ac_odbc_conn_t;

#define ac_odbc_IS_NULL(c) (c.length==SQL_NULL_DATA)

typedef struct {
  SQLTCHAR name[128];
  SQLSMALLINT type;
  SQLULEN precision;
  SQLLEN indicator;
  SQLSMALLINT scale;
  SQLSMALLINT nullable;

  double double_value;
  int64_t int_value;
  char bool_value;

  void *data;
  SQLLEN length;
} ac_odbc_column_t;

ac_odbc_t *ac_odbc_init(const char *application_name, const char *connection_string);

void ac_odbc_destroy(ac_odbc_t *h);
bool ac_odbc_reconnect(ac_odbc_t *h);


ac_odbc_conn_t *ac_odbc_conn_init(ac_odbc_t *h);
bool ac_odbc_conn_close(ac_odbc_conn_t *h);
void ac_odbc_conn_destroy(ac_odbc_conn_t *h);

bool ac_odbc_execute(ac_odbc_conn_t *h, const char *request);

ac_odbc_column_t *ac_odbc_column_info(ac_odbc_conn_t *h, int column);
void ac_odbc_dump_column_info(ac_odbc_conn_t *h, int column);
void ac_odbc_dump_columns_info(ac_odbc_conn_t *h);

int ac_odbc_get_num_effected_rows(ac_odbc_conn_t *h);

bool ac_odbc_next(ac_odbc_conn_t *h);

int ac_odbc_get_num_columns(ac_odbc_conn_t *h);

bool ac_odbc_okay(ac_odbc_t *h);

int ac_odbc_errors(ac_odbc_t *h);
int ac_odbc_conn_errors(ac_odbc_conn_t *h);

#endif