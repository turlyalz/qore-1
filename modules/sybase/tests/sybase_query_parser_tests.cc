#ifdef DEBUG

#include "sybase_tests_common.h"

namespace sybase_tests_1739 {


//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  sybase_query_parameter p1;
  assert(p1.is_input_parameter());

  sybase_query_parameter p2("ooo");
  assert(!p2.is_input_parameter());
  
  std::vector<sybase_query_parameter> v;
  v.push_back(p1);
  v.push_back(p2);

  processed_sybase_query q("aaa", v, true);
}

//------------------------------------------------------------------------------
TEST()
{
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  ExceptionSink xsink; 
  processed_sybase_query res = parse_sql_query("aaa", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa");
  assert(res.m_parameters.size() == 0);
  assert(res.m_is_procedure == false);

  
  res = parse_sql_query("select * from x where a = %v and b = %v", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_parameters.size() == 2);
  assert(res.m_result_dynamic_query_text == "select * from x where a =  ? and b =  ?");
  assert(res.m_is_procedure == false);

  res = parse_sql_query("select * from x where a = '%v' and b = %v", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_parameters.size() == 1);
  assert(res.m_result_dynamic_query_text == "select * from x where a = '%v' and b =  ?");
  assert(res.m_is_procedure == false);

  res = parse_sql_query("select * from x where a = '%v\"%v' and b = %v", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_parameters.size() == 1);
  assert(res.m_result_dynamic_query_text == "select * from x where a = '%v\"%v' and b =  ?");
  assert(res.m_is_procedure == false);

  res = parse_sql_query("select * from x where a = \"%v\" and b = %v", &xsink);
  if (xsink.isException()) {
    assert(false);
  }

  assert(res.m_parameters.size() == 1);
  assert(res.m_result_dynamic_query_text == "select * from x where a = \"%v\" and b =  ?");
  assert(res.m_is_procedure == false);

  res = parse_sql_query("select * from x where a = \"%v'%v\" and b = %v", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_parameters.size() == 1);
  assert(res.m_result_dynamic_query_text == "select * from x where a = \"%v'%v\" and b =  ?");
  assert(res.m_is_procedure == false);
}

//------------------------------------------------------------------------------
TEST()
{
/* needs to be removed or rewritten
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  ExceptionSink xsink;
  processed_sybase_query res = parse_procedure_call(" exec aaa ", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa");
  assert(res.m_parameters.size() == 0);
  assert(res.m_is_procedure == true);

  res = parse_procedure_call(" EXECUTE aaa ", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa");
  assert(res.m_parameters.size() == 0);
  assert(res.m_is_procedure == true);

  res = parse_procedure_call(" EXECUTE aaa ( )", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa");
  assert(res.m_parameters.size() == 0);
  assert(res.m_is_procedure == true);

  res = parse_procedure_call(" EXECUTE aaa2 (%v , %v )", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa2");
  assert(res.m_parameters.size() == 2);
  assert(res.m_parameters[0].m_input_parameter);
  assert(res.m_parameters[1].m_input_parameter);
  assert(res.m_is_procedure == true);

  res = parse_procedure_call(" EXECUTE aaa_1(%v, :abc, %v )", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa_1");
  assert(res.m_parameters.size() == 3);
  assert(res.m_parameters[0].m_input_parameter);
  assert(!res.m_parameters[1].m_input_parameter);
  assert(res.m_parameters[1].m_placeholder == "abc");
  assert(res.m_parameters[2].m_input_parameter);
  assert(res.m_is_procedure == true);
*/
}

//------------------------------------------------------------------------------
TEST()
{
/* delete or rewrite
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  ExceptionSink xsink;
  processed_sybase_query res = parse_sybase_query("aaa", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa");
  assert(res.m_parameters.size() == 0);
  assert(res.m_is_procedure == false);

  res = parse_sybase_query(" EXECUTE aaa_1(%v, :abc, %v )", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa_1");
  assert(res.m_parameters.size() == 3);
  assert(res.m_parameters[0].m_input_parameter);
  assert(!res.m_parameters[1].m_input_parameter);
  assert(res.m_parameters[1].m_placeholder == "abc");
  assert(res.m_parameters[2].m_input_parameter);
  assert(res.m_is_procedure == true);
*/
}

//------------------------------------------------------------------------------
TEST()
{
/* delete or rewrite
  // test %d
  printf("running test %s[%d]\n", __FILE__, __LINE__);
  ExceptionSink xsink;
  processed_sybase_query res = parse_sybase_query("aaa", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa");
  assert(res.m_parameters.size() == 0);
  assert(res.m_is_procedure == false);

  res = parse_sybase_query(" EXECUTE aaa_1(%d, :abc, %v )", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(res.m_result_dynamic_query_text == "aaa_1");
  assert(res.m_parameters.size() == 3);
  assert(res.m_parameters[0].m_input_parameter);
  assert(res.m_parameters[0].m_is_integer_type == true);

  assert(!res.m_parameters[1].m_input_parameter);
  assert(res.m_parameters[1].m_placeholder == "abc");
  assert(res.m_parameters[1].m_is_integer_type == false);

  assert(res.m_parameters[2].m_input_parameter);
  assert(res.m_is_procedure == true);
  assert(res.m_parameters[2].m_is_integer_type == false);
*/
}

TEST()
{
  // test the routing making @parX from ? (used for ct_command(CS_LANG_CMD))
  std::string s = processed_sybase_query::generate_static_query_parameter_names("select x from y");
  assert(s == "select x from y");
  s = processed_sybase_query::generate_static_query_parameter_names("select x from ?");
  assert(s == "select x from @par0");
  s = processed_sybase_query::generate_static_query_parameter_names("select x from ? and ?");
  assert(s == "select x from @par0 and @par1");
  s = processed_sybase_query::generate_static_query_parameter_names("select x from \"?\" and '?'");
  assert(s == "select x from \"?\" and '?'");
  s = processed_sybase_query::generate_static_query_parameter_names("select x from \"\\\"?\" and '\\\'?'");
  assert(s == "select x from \"\\\"?\" and '\\\'?'");
}

} // namespace
#endif

// EOF



