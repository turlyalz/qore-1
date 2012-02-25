/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  DBI.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_DBI_H
#define _QORE_DBI_H

//! @file DBI.h describes Qore's DBI interface for writing database drivers

// DBI Driver capabilities
#define DBI_CAP_NONE                     0
#define DBI_CAP_TIME_ZONE_SUPPORT        (1 << 0)
#define DBI_CAP_CHARSET_SUPPORT          (1 << 1)
#define DBI_CAP_TRANSACTION_MANAGEMENT   (1 << 2)
#define DBI_CAP_STORED_PROCEDURES        (1 << 3)
#define DBI_CAP_LOB_SUPPORT              (1 << 4)
#define DBI_CAP_BIND_BY_VALUE            (1 << 5)
#define DBI_CAP_BIND_BY_PLACEHOLDER      (1 << 6)
#define DBI_CAP_HAS_EXECRAW              (1 << 7)
#define DBI_CAP_HAS_STATEMENT            (1 << 8)
#define DBI_CAP_HAS_SELECT_ROW           (1 << 9)

#define BN_PLACEHOLDER  0
#define BN_VALUE        1

#define DBI_DEFAULT_STR_LEN 512

// DBI method codes
#define QDBI_METHOD_OPEN                      1
#define QDBI_METHOD_CLOSE                     2
#define QDBI_METHOD_SELECT                    3
#define QDBI_METHOD_SELECT_ROWS               4
#define QDBI_METHOD_EXEC                      5
#define QDBI_METHOD_COMMIT                    6
#define QDBI_METHOD_ROLLBACK                  7
#define QDBI_METHOD_BEGIN_TRANSACTION         8
#define QDBI_METHOD_ABORT_TRANSACTION_START   9
#define QDBI_METHOD_GET_SERVER_VERSION       10
#define QDBI_METHOD_GET_CLIENT_VERSION       11
#define QDBI_METHOD_EXECRAW                  12
#define QDBI_METHOD_STMT_PREPARE             13
#define QDBI_METHOD_STMT_PREPARE_RAW         14
#define QDBI_METHOD_STMT_BIND                15
#define QDBI_METHOD_STMT_BIND_PLACEHOLDERS   16
#define QDBI_METHOD_STMT_BIND_VALUES         17
#define QDBI_METHOD_STMT_EXEC                18
#define QDBI_METHOD_STMT_FETCH_ROW           19
#define QDBI_METHOD_STMT_FETCH_ROWS          20
#define QDBI_METHOD_STMT_FETCH_COLUMNS       21
#define QDBI_METHOD_STMT_NEXT                22
#define QDBI_METHOD_STMT_CLOSE               23
#define QDBI_METHOD_STMT_AFFECTED_ROWS       24
#define QDBI_METHOD_STMT_GET_OUTPUT          25
#define QDBI_METHOD_STMT_GET_OUTPUT_ROWS     26
#define QDBI_METHOD_STMT_DEFINE              27
#define QDBI_METHOD_SELECT_ROW               28

#define QDBI_VALID_CODES 28

class Datasource;
class ExceptionSink;
class QoreString;
class QoreListNode;
class AbstractQoreNode;
class QoreHashNode;
class QoreNamespace;
class SQLStatement;

// DBI method signatures - note that only get_client_version uses a "const Datasource" 
// the others do not so that automatic reconnects can be supported (which will normally
// require writing to the Datasource)

//! signature for the DBI "open" method - must be defined in each DBI driver
/** @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
*/
typedef int (*q_dbi_open_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the DBI "close" method - must be defined in each DBI driver
/** this function cannot throw an exception and currently any return error code is ignored
    @param ds the Datasource for the connection to close
    @return 0 for OK, non-zero for error
*/
typedef int (*q_dbi_close_t)(Datasource *ds);

//! signature for the DBI "select" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param str the SQL string to execute, may not be in the encoding of the Datasource
    @param args arguments for placeholders or DBI formatting codes in the SQL string
    @param xsink if any errors occur, error information should be added to this object
    @return the data returned by executing the SQL or 0
*/
typedef AbstractQoreNode *(*q_dbi_select_t)(Datasource *ds, const QoreString *str, const QoreListNode *args, ExceptionSink *xsink);

//! signature for the DBI "selectRows" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param str the SQL string to execute, may not be in the encoding of the Datasource
    @param args arguments for placeholders or DBI formatting codes in the SQL string
    @param xsink if any errors occur, error information should be added to this object
    @return the data returned by executing the SQL or 0
*/
typedef AbstractQoreNode *(*q_dbi_select_rows_t)(Datasource *ds, const QoreString *str, const QoreListNode *args, ExceptionSink *xsink);

//! signature for the DBI "selectRow" method - must be defined in each DBI driver
/** if the SQL causes more than 1 row to be returned, then the driver must raise an exception
    @param ds the Datasource for the connection
    @param str the SQL string to execute, may not be in the encoding of the Datasource; must cause at most one row to be returned
    @param args arguments for placeholders or DBI formatting codes in the SQL string
    @param xsink if any errors occur, error information should be added to this object
    @return the row data returned by executing the SQL or 0
    @since qore 0.8.2
*/
typedef QoreHashNode *(*q_dbi_select_row_t)(Datasource *ds, const QoreString *str, const QoreListNode *args, ExceptionSink *xsink);

//! signature for the DBI "execSQL" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param str the SQL string to execute, may not be in the encoding of the Datasource
    @param args arguments for placeholders or DBI formatting codes in the SQL string
    @param xsink if any errors occur, error information should be added to this object
    @return the data returned by executing the SQL or 0
*/
typedef AbstractQoreNode *(*q_dbi_exec_t)(Datasource *ds, const QoreString *str, const QoreListNode *args, ExceptionSink *xsink);

//! signature for the DBI "execRawSQL" method - must be defined in each DBI driver
/**
   @param ds the Datasource for the connection
   @param str the SQL string to execute, may not be in the encoding of the Datasource
   @param xsink if any errors occur, error information should be added to this object
   @return the data returned by executing the SQL or 0
*/
typedef AbstractQoreNode *(*q_dbi_execraw_t)(Datasource *ds, const QoreString *str, ExceptionSink *xsink);

//! signature for the DBI "commit" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
*/
typedef int (*q_dbi_commit_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the DBI "rollback" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
*/
typedef int (*q_dbi_rollback_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the DBI "begin_transaction" method, should only be defined for drivers needing this to explicitly start a transaction 
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
*/
typedef int (*q_dbi_begin_transaction_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the rollback method to be executed when the first statement in an explicit transaction started implicitly with the DBI "begin_transaction" method fails
/** this should just be a pointer to the rollback method for those drivers that need it (ex: pgsql)
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
*/
typedef int (*q_dbi_abort_transaction_start_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the "get_server_version" method
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return a value describing the server's version
*/
typedef AbstractQoreNode *(*q_dbi_get_server_version_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the "get_client_version" method
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return a value describing the client's version
*/
typedef AbstractQoreNode *(*q_dbi_get_client_version_t)(const Datasource *ds, ExceptionSink *xsink);

// FIXME: document
//! prepare statement and process placeholder specifications and bind parameters
/** @returns -1 = an exception occurred, 0 = OK
 */
typedef int (*q_dbi_stmt_prepare_t)(SQLStatement *stmt, const QoreString &str, const QoreListNode *args, ExceptionSink *xsink);

//! prepare statement with no bind parsing
/** @returns -1 = an exception occurred, 0 = OK
 */
typedef int (*q_dbi_stmt_prepare_raw_t)(SQLStatement *stmt, const QoreString &str, ExceptionSink *xsink);

//! bind input values and optionally describe output parameters
/** @returns -1 = an exception occurred, 0 = OK
 */
typedef int (*q_dbi_stmt_bind_t)(SQLStatement *stmt, const QoreListNode &l, ExceptionSink *xsink);

//! execute statement
/** @returns -1 = an exception occurred, 0 = OK
 */
typedef int (*q_dbi_stmt_exec_t)(SQLStatement *stmt, ExceptionSink *xsink);

//! get number of affected rows
/** @returns number of rows affected by last query
 */
typedef int (*q_dbi_stmt_affected_rows_t)(SQLStatement *stmt, ExceptionSink *xsink);

//! get output values, any row sets are returned as a hash of lists
/** @returns a hash of output values, any row sets are returned as a hash of lists
 */
typedef QoreHashNode *(*q_dbi_stmt_get_output_t)(SQLStatement *stmt, ExceptionSink *xsink);

//! get output values, any row sets are returned as a list of hashes
/** @returns a hash of output values, any row sets are returned as a list of hashes
 */
typedef QoreHashNode *(*q_dbi_stmt_get_output_rows_t)(SQLStatement *stmt, ExceptionSink *xsink);

typedef int (*q_dbi_stmt_define_t)(SQLStatement *stmt, ExceptionSink *xsink);
typedef QoreHashNode *(*q_dbi_stmt_fetch_row_t)(SQLStatement *stmt, ExceptionSink *xsink);
typedef QoreHashNode *(*q_dbi_stmt_fetch_columns_t)(SQLStatement *stmt, int rows, ExceptionSink *xsink);
typedef QoreListNode *(*q_dbi_stmt_fetch_rows_t)(SQLStatement *stmt, int rows, ExceptionSink *xsink);
typedef bool (*q_dbi_stmt_next_t)(SQLStatement *stmt, ExceptionSink *xsink);
typedef int (*q_dbi_stmt_close_t)(SQLStatement *stmt, ExceptionSink *xsink);

typedef std::pair<int, void *> qore_dbi_method_t;

typedef safe_dslist<qore_dbi_method_t> dbi_method_list_t;

//! this is the data structure Qore DBI drivers will use to pass the supported DBI methods
/** the minimum methods that must be supported are: open, close, select, selectRows, execSQL, execRawSQL, commit, and rollback
 */
class qore_dbi_method_list {
private:
   struct qore_dbi_mlist_private *priv; // private implementation

   // not implemented
   DLLLOCAL qore_dbi_method_list(const qore_dbi_method_list&);
   DLLLOCAL qore_dbi_method_list& operator=(const qore_dbi_method_list&);

public:
   DLLEXPORT qore_dbi_method_list();
   DLLEXPORT ~qore_dbi_method_list();

   // covers open, commit, rollback, and begin transaction
   DLLEXPORT void add(int code, q_dbi_open_t method);
   // for close
   DLLEXPORT void add(int code, q_dbi_close_t method);
   // covers select, select_rows, and exec
   DLLEXPORT void add(int code, q_dbi_select_t method);
   // covers select_row
   DLLEXPORT void add(int code, q_dbi_select_row_t method);
   // covers execRaw
   DLLEXPORT void add(int code, q_dbi_execraw_t method);
   // covers get_server_version
   DLLEXPORT void add(int code, q_dbi_get_server_version_t method);
   // covers get_client_version
   DLLEXPORT void add(int code, q_dbi_get_client_version_t method);

   // covers prepare
   DLLEXPORT void add(int code, q_dbi_stmt_prepare_t method);
   // covers prepare_raw
   DLLEXPORT void add(int code, q_dbi_stmt_prepare_raw_t method);
   // covers bind, bind_placeholders, bind_values
   DLLEXPORT void add(int code, q_dbi_stmt_bind_t method);
   // covers exec, close, affected_rows, and define
   DLLEXPORT void add(int code, q_dbi_stmt_exec_t method);
   // covers fetch_row, get_output, and get_output_rows
   DLLEXPORT void add(int code, q_dbi_stmt_fetch_row_t method);
   // covers fetch_columns
   DLLEXPORT void add(int code, q_dbi_stmt_fetch_columns_t method);
   // covers fetch_rows
   DLLEXPORT void add(int code, q_dbi_stmt_fetch_rows_t method);
   // covers next
   DLLEXPORT void add(int code, q_dbi_stmt_next_t method);

   // internal interface
   DLLLOCAL dbi_method_list_t *getMethods() const;
};

//! this class provides the internal link to the database driver for Qore's DBI layer
/**
   most of these functions are not exported; the Datasource class should be used 
   instead of using the DBIDriver class directly
   @see Datasource
*/
class DBIDriver {
private:
   //! private implementation
   struct qore_dbi_private *priv;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL DBIDriver(const DBIDriver&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL DBIDriver& operator=(const DBIDriver&);

public:
   //! this is the only public exported function available in this class
   /**
      @return the name of the driver (ex: "oracle")
   */
   DLLEXPORT const char *getName() const;

   DLLLOCAL DBIDriver(const char *name, const dbi_method_list_t &methods, int cps);
   DLLLOCAL ~DBIDriver();
   DLLLOCAL int init(Datasource *ds, ExceptionSink *xsink) const;
   DLLLOCAL int close(Datasource *ds) const;
   DLLLOCAL AbstractQoreNode *select(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *selectRows(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL QoreHashNode *selectRow(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *execSQL(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *execRawSQL(Datasource *ds, const QoreString *sql, ExceptionSink *xsink) const;
   DLLLOCAL int commit(Datasource *, ExceptionSink *xsink) const;
   DLLLOCAL int rollback(Datasource *, ExceptionSink *xsink) const;
   DLLLOCAL int autoCommit(Datasource *, ExceptionSink *xsink) const;
   DLLLOCAL int beginTransaction(Datasource *, ExceptionSink *xsink) const;
   DLLLOCAL int abortTransactionStart(Datasource *, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *getServerVersion(Datasource *, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *getClientVersion(const Datasource *, ExceptionSink *xsink) const;

   DLLLOCAL int stmt_prepare(SQLStatement *stmt, const QoreString &str, const QoreListNode *args, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_prepare_raw(SQLStatement *stmt, const QoreString &str, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_bind(SQLStatement *stmt, const QoreListNode &l, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_bind_placeholders(SQLStatement *stmt, const QoreListNode &l, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_bind_values(SQLStatement *stmt, const QoreListNode &l, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_exec(SQLStatement *stmt, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_affected_rows(SQLStatement *stmt, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_define(SQLStatement *stmt, ExceptionSink *xsink) const;
   DLLLOCAL QoreHashNode *stmt_get_output(SQLStatement *stmt, ExceptionSink *xsink) const;
   DLLLOCAL QoreHashNode *stmt_get_output_rows(SQLStatement *stmt, ExceptionSink *xsink) const;

   DLLLOCAL QoreHashNode *stmt_fetch_row(SQLStatement *stmt, ExceptionSink *xsink) const;
   DLLLOCAL QoreListNode *stmt_fetch_rows(SQLStatement *stmt, int rows, ExceptionSink *xsink) const;
   DLLLOCAL QoreHashNode *stmt_fetch_columns(SQLStatement *stmt, int rows, ExceptionSink *xsink) const;
   DLLLOCAL bool stmt_next(SQLStatement *stmt, ExceptionSink *xsink) const;
   DLLLOCAL int stmt_close(SQLStatement *stmt, ExceptionSink *xsink) const;

   DLLLOCAL bool hasStatementAPI() const;

   DLLLOCAL int getCaps() const;
   DLLLOCAL QoreListNode *getCapList() const;
};

struct qore_dbi_dlist_private;

//! this class is used to register and find DBI drivers loaded in qore
/**
   this class will all use the ModuleManager to try and load a driver if it is not already loaded when find() is called
   @see ModuleManager
*/
class DBIDriverList {
private:
   //! private implementation
   struct qore_dbi_dlist_private *priv;

   DLLLOCAL DBIDriver *find_intern(const char *name) const;

public:
   //! registers a new DBI driver
   /**
      @param name the name of the driver (ex: "oracle")
      @param methods the list of methods the driver supports
      @param caps the capabilities the driver supports
      @return the DBIDriver object created
   */
   DLLEXPORT class DBIDriver *registerDriver(const char *name, const qore_dbi_method_list &methods, int caps);

   //! finds a driver, will try to load the driver using the ModuleManager if no such driver is already present
   /**
      @param name the name of the driver to find (or load)
      @return the DBIDriver found or 0 if not found and was not loaded
      @see ModuleManager
   */
   DLLEXPORT DBIDriver *find(const char *name) const;

   //! finds a driver, will try to load the driver using the ModuleManager if no such driver is already present
   /** 
       @param name the name of the driver to find (or load)
       @param xsink Qore-language exceptions saved here if any occur
       @return the DBIDriver found or 0 if not found and was not loaded
       @see ModuleManager
   */
   DLLEXPORT DBIDriver *find(const char *name, ExceptionSink *xsink) const;

   DLLLOCAL DBIDriverList();
   DLLLOCAL ~DBIDriverList();
   DLLLOCAL QoreListNode *getDriverList() const;
};

//! list of DBI drivers currently reigsted by the Qore library
DLLEXPORT extern DBIDriverList DBI;

//! parses a datasource string and returns a hash of the component parts
DLLEXPORT QoreHashNode *parseDatasource(const char *ds, ExceptionSink *xsink);

//! concatenates a numeric value to the QoreString from the QoreNode
DLLEXPORT void DBI_concat_numeric(QoreString *str, const AbstractQoreNode *v);

//! concatenates a string value to the QoreString from the AbstractQoreNode
/** NOTE: no escaping is done here
    this function is most useful for table prefixes, etc in queries
*/
DLLEXPORT int DBI_concat_string(QoreString *str, const AbstractQoreNode *v, ExceptionSink *xsink);

#endif  // _QORE_DBI_H
