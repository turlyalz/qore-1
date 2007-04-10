#ifdef DEBUG

#include "common.h"
#include "connection.h"
#include "initiate_language_command.h"

namespace sybase_tests_571925222 {

//------------------------------------------------------------------------------
TEST()
{
  // test send_command
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  connection conn;
  ExceptionSink xsink;
  conn.init(SYBASE_TEST_SETTINGS, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  command cmd(conn, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  initiate_language_command(cmd, "select * from syskeys", &xsink);
  if (xsink.isException()) {
    assert(false);
  } 
  send_command(cmd, &xsink);
  if (xsink.isException()) {
    assert(false);
  } 
  printf("send_command() for simple query works\n");
}

} // namespace
#endif

// EOF

