/*
  BuiltinFunctionList.h

  Qore programming language

  Copyright 2003 - 2010 David Nichols

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

#ifndef _QORE_BUILTINFUNCTIONLIST_H

#define _QORE_BUILTINFUNCTIONLIST_H

#include <qore/common.h>
#include <qore/hash_map.h>
#include <qore/Restrictions.h>
#include <qore/QoreThreadLock.h>

#include <stdarg.h>

/** @file BuiltinFunctionList.h
    defines the BuiltinFunctionList class for the Qore library
 */

DLLLOCAL void init_builtin_functions();

//! the container that manages all builtin functions in the library
/** The object is thread-safe; a hash or hash-map is used for lookups.
    There is only one of these, therefore we have static members and methods.
 */
class BuiltinFunctionList {
   friend class BuiltinFunctionListOptionalLockHelper;
   private:
      DLLLOCAL static bool init_done;
      DLLLOCAL static hm_bf_t hm;
      DLLLOCAL static QoreThreadLock mutex;

      // not implemented
      DLLLOCAL BuiltinFunctionList(const BuiltinFunctionList&);
      DLLLOCAL BuiltinFunctionList& operator=(const BuiltinFunctionList&);
      DLLLOCAL void *operator new(size_t);
      DLLLOCAL static int add_intern(BuiltinFunction *bf);

   public:
      DLLLOCAL BuiltinFunctionList();
      DLLLOCAL ~BuiltinFunctionList();
      DLLLOCAL void clear();

      //! adds a new builtin function to the list
      /**
	 @param name the name of the function
	 @param f a pointer to the actual C++ function to be executed when the function is called
	 @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       */
      DLLEXPORT static void add(const char *name, q_func_t f, int functional_domain = QDOM_DEFAULT);

      //! adds a new builtin function to the list and allows for the return type and parameter list to be set
      /**
	 @param name the name of the function
	 @param f a pointer to the actual C++ function to be executed when the function is called
	 @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
      */
      DLLEXPORT static void add2(const char *name, q_func_t f, int functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...);

      //! returns the number of functions in the hash
      /**
	 @return the number of functions in the hash
       */
      DLLEXPORT static int size();

      //! finds a function by its name
      /**
	 @return a pointer to the function found
       */
      DLLEXPORT static const BuiltinFunction *find(const char *name);

      DLLLOCAL static void init();
};

//! the global list of builtin functions in the qore library
DLLEXPORT extern BuiltinFunctionList builtinFunctions;

#endif // _QORE_BUILTINFUNCTIONLIST_H
