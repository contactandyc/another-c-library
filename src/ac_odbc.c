#include "ac_odbc.h"
#include "ac_allocator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <iodbcext.h>
#include <pthread.h>

#define OUT_DNS_LEN 4096

struct ac_odbc_s
{
    HENV henv;
    HDBC hdbc;
    int connected;
    SQLTCHAR outdsn[OUT_DNS_LEN];
    int status;
    const char *errmsg;
    char *application_name;
    char *connection_string;
    char *driver_version;
    char *driver_name;

};

struct ac_odbc_conn_s
{
    ac_odbc_t *h;
    int status;
    const char *errmsg;
    bool cursor_open;
    int num_cols;
    ac_odbc_column_t *cols;
    HSTMT hstmt;
};

ac_odbc_t *ac_odbc_init(const char *application_name, const char *connection_string)
{
    short buflen;
    SQLTCHAR dsn[33];
    SQLTCHAR desc[255];
    SQLTCHAR driverInfo[255];
    SQLSMALLINT len1, len2;
    int status;

    ac_odbc_t *r = (ac_odbc_t *)ac_calloc(sizeof(ac_odbc_t));
    r->henv = SQL_NULL_HANDLE;
    r->hdbc = SQL_NULL_HANDLE;

    if (SQLAllocHandle(SQL_HANDLE_ENV, NULL, &r->henv) != SQL_SUCCESS)
    {
        r->errmsg = "Unable to allocate iODBC environment";
        r->status = -1;
        return r;
    }

    SQLSetEnvAttr(r->henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3,
                  SQL_IS_UINTEGER);

    if (SQLAllocHandle(SQL_HANDLE_DBC, r->henv, &r->hdbc) != SQL_SUCCESS)
    {
        r->errmsg = "Unable to allocate iODBC connection";
        r->status = -1;
        return r;
    }

    SQLSetConnectOption(r->hdbc, SQL_APPLICATION_NAME, (SQLULEN)application_name);

    r->application_name = strdup(application_name);
    r->connection_string = strdup(connection_string);

    r->status = SQLDriverConnect(r->hdbc, 0, (SQLCHAR *)r->connection_string, SQL_NTS,
                                 (SQLCHAR *)r->outdsn, OUT_DNS_LEN, &buflen, SQL_DRIVER_COMPLETE);

    if (r->status != SQL_SUCCESS && r->status != SQL_SUCCESS_WITH_INFO)
    {
        r->errmsg = "SQLDriverConnect";
        return r;
    }

    r->connected = 1;

    status = SQLGetInfo(r->hdbc, SQL_DRIVER_VER,
                        driverInfo, 255, &len1);
    if (status == SQL_SUCCESS)
        r->driver_version = strdup((const char *)driverInfo);
    status = SQLGetInfo(r->hdbc, SQL_DRIVER_NAME,
                        driverInfo, 255, &len1);
    if (status == SQL_SUCCESS)
        r->driver_name = strdup((const char *)driverInfo);

    return r;
}

void ac_odbc_destroy(ac_odbc_t *h)
{
    if (h->connected)
        SQLDisconnect(h->hdbc);

    if (h->hdbc)
        SQLFreeHandle(SQL_HANDLE_DBC, h->hdbc);

    if (h->henv)
        SQLFreeHandle(SQL_HANDLE_ENV, h->henv);

    if (h->driver_version)
        free(h->driver_version);

    if (h->driver_name)
        free(h->driver_name);

    if (h->connection_string)
        free(h->connection_string);

    if (h->application_name)
        free(h->application_name);

    free(h);
}

bool ac_odbc_reconnect(ac_odbc_t *h)
{
    SQLRETURN status;
    SQLTCHAR buf[4096];
    SQLSMALLINT len;

    SQLDisconnect(h->hdbc);

    status = SQLDriverConnect(h->hdbc, 0, h->outdsn, SQL_NTS,
                              buf, sizeof(buf), &len, SQL_DRIVER_NOPROMPT);

    if (!SQL_SUCCEEDED(status))
    {
        h->errmsg = "DriverConnect (reconnect)";
        return false;
    }

    return true;
}

ac_odbc_conn_t *ac_odbc_conn_init(ac_odbc_t *h) {
    ac_odbc_conn_t *conn = (ac_odbc_conn_t *)ac_calloc(sizeof(*conn));
    conn->hstmt = SQL_NULL_HANDLE;
    conn->h = h;
    if (SQLAllocHandle(SQL_HANDLE_STMT, h->hdbc, &conn->hstmt) != SQL_SUCCESS)
    {
        conn->errmsg = "Unable to allocate iODBC statement handle";
        return conn;
    }
    return conn;
}

bool ac_odbc_conn_close(ac_odbc_conn_t *h)
{
    if (h->cursor_open)
    {
        for (int i = 0; i < h->num_cols; i++)
        {
            if (h->cols[i].data)
                free(h->cols[i].data);
        }
        free(h->cols);
        h->num_cols = 0;
        h->cols = NULL;
        SQLCloseCursor(h->hstmt);
        h->cursor_open = false;
    }
    return false;
}

void ac_odbc_conn_destroy(ac_odbc_conn_t *h)
{
    if (h->cursor_open)
        ac_odbc_conn_close(h);
    SQLFreeHandle(SQL_HANDLE_STMT, h->hstmt);
    free(h);
}

bool ac_odbc_execute(ac_odbc_conn_t *h, const char *request)
{
    h->cursor_open = false;
    if (SQLPrepare(h->hstmt, (SQLTCHAR *)request,
                   SQL_NTS) != SQL_SUCCESS)
    {
        return false;
    }
    SQLRETURN sts;
    if ((sts = SQLExecute(h->hstmt)) != SQL_SUCCESS)
    {
        if (sts != SQL_SUCCESS_WITH_INFO)
            return false;
    }

    h->num_cols = 0;
    h->cursor_open = true;
    short numCols;
    if (SQLNumResultCols(h->hstmt, &numCols) != SQL_SUCCESS)
        return ac_odbc_conn_close(h);
    h->num_cols = numCols;

    if (h->num_cols > 0)
    {
        h->cols = (ac_odbc_column_t *)ac_calloc(sizeof(ac_odbc_column_t) * h->num_cols);
        ac_odbc_column_t *cols = h->cols;
        for (int i = 1; i <= h->num_cols; i++)
        {
            if (SQLDescribeCol(h->hstmt, i,
                               (SQLTCHAR *)cols[i - 1].name, 128, NULL,
                               &cols[i - 1].type, &cols[i - 1].precision, &cols[i - 1].scale,
                               &cols[i - 1].nullable) != SQL_SUCCESS)
            {
                return ac_odbc_conn_close(h);
            }
        }
        for (int i = 0; i < h->num_cols; i++)
        {
            if (cols[i].type == SQL_DOUBLE)
            {
                if (SQLBindCol(h->hstmt, i + 1, SQL_C_DOUBLE, &cols[i].double_value, 8, &cols[i].length) != SQL_SUCCESS)
                    return ac_odbc_conn_close(h);
            }
            else if (cols[i].type == SQL_DECIMAL)
            {
                if (SQLBindCol(h->hstmt, i + 1, SQL_C_LONG, &cols[i].int_value, 8, &cols[i].length) != SQL_SUCCESS)
                    return ac_odbc_conn_close(h);
            }
            else if (cols[i].type == SQL_BIT)
            {
                if (SQLBindCol(h->hstmt, i + 1, SQL_C_BIT, &cols[i].bool_value, 1, &cols[i].length) != SQL_SUCCESS)
                    return ac_odbc_conn_close(h);
            }
            else if (cols[i].type == SQL_VARCHAR)
            {
                cols[i].data = ac_malloc(cols[i].precision + 1);
                if (SQLBindCol(h->hstmt, i + 1, SQL_C_CHAR,
                               &cols[i].data,
                               cols[i].precision + 1,
                               &cols[i].length) != SQL_SUCCESS)
                    return ac_odbc_conn_close(h);
            }
        }
    }
    return true;
}

ac_odbc_column_t *ac_odbc_column_info(ac_odbc_conn_t *h, int column)
{
    if (column >= 0 && column < h->num_cols)
        return h->cols + column;
    return NULL;
}

void ac_odbc_dump_column_info(ac_odbc_conn_t *h, int column)
{
    ac_odbc_column_t *col = ac_odbc_column_info(h, column);
    if (!col)
        return;

    printf("[%d] %s\tt:%d\tp:%lu\ti:%ld\ts:%d\tn:%d\n",
           column,
           col->name,
           col->type,
           col->precision,
           col->indicator,
           col->scale,
           col->nullable);
}

void ac_odbc_dump_columns_info(ac_odbc_conn_t *h)
{
    for (int i = 0; i < h->num_cols; i++)
        ac_odbc_dump_column_info(h, i);
}

int ac_odbc_get_num_columns(ac_odbc_conn_t *h)
{
    return h->num_cols;
}

int ac_odbc_get_num_effected_rows(ac_odbc_conn_t *h)
{
    if (!h->cursor_open)
        return 0;
    SQLLEN nrows = 0;
    SQLRowCount(h->hstmt, &nrows);
    return nrows;
}

bool ac_odbc_next(ac_odbc_conn_t *h)
{
    if (!h->cursor_open)
        return false;
    int sts = SQLFetchScroll(h->hstmt, SQL_FETCH_NEXT, 1);
    if (sts == SQL_NO_DATA_FOUND)
        return ac_odbc_conn_close(h);

    if (sts != SQL_SUCCESS)
        return ac_odbc_conn_close(h);
    return true;
}

/*
 *  Show all the error information that is available
 */
int ac_odbc_errors(ac_odbc_t *h)
{
    SQLTCHAR buf[512];
    SQLTCHAR sqlstate[15];
    SQLINTEGER native_error = 0;
    int force_exit = 0;
    SQLRETURN sts;

    int i;

    /*
   *  Get connection errors
   */
    i = 0;
    while (h->hdbc && i < 5)
    {
        sts = SQLGetDiagRec(SQL_HANDLE_DBC, h->hdbc, ++i,
                            sqlstate, &native_error, buf, 512, NULL);
        if (!SQL_SUCCEEDED(sts))
            break;

        fprintf(stderr, "%d: %s (%ld) SQLSTATE=%s\n",
                i, buf, (long)native_error, sqlstate);

        /*
       *  If the driver could not be loaded, there is no point in
       *  continuing, after reading all the error messages
       */
        if (!strcmp((const char *)sqlstate, "IM003"))
            force_exit = 1;
    }

    /*
   *  Get environment errors
   */
    i = 0;
    while (h->henv && i < 5)
    {
        sts = SQLGetDiagRec(SQL_HANDLE_ENV, h->henv, ++i,
                            sqlstate, &native_error, buf, 512, NULL);
        if (!SQL_SUCCEEDED(sts))
            break;

        fprintf(stderr, "%d: %s (%ld) SQLSTATE=%s\n",
                i, buf, (long)native_error, sqlstate);

        /*
       *  If the driver could not be loaded, there is no point in
       *  continuing, after reading all the error messages
       */
        if (!strcmp((const char *)sqlstate, "IM003"))
            force_exit = 1;
    }

    /*
   *  Force an exit status
   */
    if (force_exit)
        exit(-1);

    return -1;
}

/*
 *  Show all the error information that is available
 */
int ac_odbc_conn_errors(ac_odbc_conn_t *h)
{
    SQLTCHAR buf[512];
    SQLTCHAR sqlstate[15];
    SQLINTEGER native_error = 0;
    int force_exit = 0;
    SQLRETURN sts;

    int i;

    /*
   *  Get statement errors
   */
    i = 0;
    while (h->hstmt && i < 5)
    {
        sts = SQLGetDiagRec(SQL_HANDLE_STMT, h->hstmt, ++i,
                            sqlstate, &native_error, buf, 512, NULL);
        if (!SQL_SUCCEEDED(sts))
            break;

        fprintf(stderr, "%d: %s (%ld) SQLSTATE=%s\n",
                i, buf, (long)native_error, sqlstate);

        /*
       *  If the driver could not be loaded, there is no point in
       *  continuing, after reading all the error messages
       */
        if (!strcmp((const char *)sqlstate, "IM003"))
            force_exit = 1;
    }

    /*
   *  Force an exit status
   */
    if (force_exit)
        exit(-1);

    return -1;
}

bool ac_odbc_okay(ac_odbc_t *h) {
    if(h->status != -1)
        return true;
    return false;
}


