/*
  modules/Tuxedo/connection_parameters_helper.h

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#ifndef QORE_TUXEDO_CONNECTION_PARAMETERS_HELPER_H_
#define QORE_TUXEDO_CONNECTION_PARAMETERS_HELPER_H_

#include <qore/common.h>
#include <qore/support.h>
#include <qore/RMutex.h>
#include <atmi.h>
#include <vector>
#include <string>
#include <utility>

class ExceptionSink;
class Hash;

//------------------------------------------------------------------------------
class Tuxedo_connection_parameters
{
  static RMutex m_mutex; // must be static
  bool m_mutex_locked;
  TPINIT* m_tpinit_data;
  std::vector<std::pair<std::string, std::string> > m_old_environment;

  bool set_environment_variable(char* name, Hash* params, ExceptionSink* xsink);
  bool set_flag(char* name, unsigned flag, Hash* params, ExceptionSink* xsink, long& flags);
  bool set_string(char* name, Hash* params, ExceptionSink* xsink, std::string& str);

public:
  Tuxedo_connection_parameters();
  ~Tuxedo_connection_parameters();
  void process_parameters(QoreNode* params, ExceptionSink* xsink); // parses hash with all parameters for Tuxedo tpinit()
  TPINIT* get_tpinit_data() { return m_tpinit_data; }
};

#endif

// EOF

