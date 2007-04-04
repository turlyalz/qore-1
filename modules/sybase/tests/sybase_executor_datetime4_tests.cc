#ifdef DEBUG

#include "sybase_tests_common.h"
#include <qore/Hash.h>
#include <qore/DateTime.h>

namespace sybase_tests_018162 {

//------------------------------------------------------------------------------
static void create_datetime4_table()
{
  const char* cmd =  "create table datetime4_table (datetime4_col smalldatetime)";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
}

//------------------------------------------------------------------------------
static void delete_datetime4_table(bool quiet = false)
{
  const char* cmd =  "drop table datetime4_table";

  sybase_connection c;
  ExceptionSink xsink;
  c.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  sybase_low_level_execute_directly_command(c.getConnection(), cmd, &xsink);
  if (xsink.isException()) {
    if (quiet) {
      xsink.clear();
    } else {
      assert(false);
    }
  }
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_datetime4_table(true);
  create_datetime4_table();
  delete_datetime4_table();
}

//------------------------------------------------------------------------------
TEST()
{
  // testing datetime parameter
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_datetime4_table(true);
  create_datetime4_table();
  ON_BLOCK_EXIT(delete_datetime4_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_dynamic_query_text = "select * from datetime4_table where datetime4_col = ?";
  executor.m_parsed_query.m_is_procedure = false;

  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  executor.m_test_encoding = QCS_DEFAULT;
  executor.m_test_autocommit = false;
  executor.m_test_connection = &conn;
  List* l= new List;
  l->push(new QoreNode(new DateTime));
  executor.m_args = l;

  QoreNode* n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n);
  if (n) n->deref(&xsink);

  QoreNode* aux = new QoreNode(l);
  aux->deref(&xsink);
}

//------------------------------------------------------------------------------
TEST()
{
  // testing insert, delete, drop etc using executor
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  delete_datetime4_table(true);
  create_datetime4_table();
  ON_BLOCK_EXIT(delete_datetime4_table, false);

  sybase_executor executor;
  executor.m_parsed_query.m_result_dynamic_query_text = "insert into datetime4_table values (?)";
  executor.m_parsed_query.m_is_procedure = false;

  sybase_connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_test_encoding = QCS_DEFAULT;
  executor.m_test_autocommit = false;
  executor.m_test_connection = &conn;
  List* l= new List;
  l->push(new QoreNode(new DateTime));
  executor.m_args = l;

  QoreNode* n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(!n);

  n = executor.exec(&xsink); // once more
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_dynamic_query_text = "select count(*) from datetime4_table";
  executor.m_args = new List;
  n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_HASH);
  QoreNode* x = n->val.hash->getKeyValue("column1");
  assert(x);
  assert(x->type == NT_INT);
  assert(x->val.intval == 2);

  executor.m_parsed_query.m_result_dynamic_query_text = "select * from datetime4_table";
  n = executor.selectRows(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_LIST);
  assert(n->val.list->size() == 2);
  x = n->val.list->retrieve_entry(0);
  assert(x);
  assert(x->type == NT_HASH);
  assert(x->val.hash->size() == 1);
  x = x->val.hash->getKeyValue("datetime4_col");
  assert(x);
  assert(x->type == NT_DATE);
  DateTime aux;
  int64 sec1 = aux.getEpochSeconds();
  int64 sec2 = x->val.date_time->getEpochSeconds();
  assert(sec1 == sec2);

  executor.m_parsed_query.m_result_dynamic_query_text = "delete from datetime4_table";
  executor.m_args = new List;
  n = executor.exec(&xsink);
  if (xsink.isException()) {
    assert(false);
  }

  executor.m_parsed_query.m_result_dynamic_query_text = "select count(*) from datetime4_table";
  n = executor.select(&xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(n);
  assert(n->type == NT_HASH);
  x = n->val.hash->getKeyValue("column1");
  assert(x->type == NT_INT);
  assert(x->val.intval == 0);
}

} // namespace
#endif

// EOF

