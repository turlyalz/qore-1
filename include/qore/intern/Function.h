/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Function.h

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

#ifndef _QORE_FUNCTION_H

#define _QORE_FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

// these data structures are all private to the library

DLLLOCAL AbstractQoreNode* doPartialEval(class AbstractQoreNode* n, bool* is_self_ref, ExceptionSink* xsink);

class LocalVar;
class VarRefNode;
class BCAList;
class QoreOperatorNode;
class BarewordNode;
class QoreFunction;

typedef std::vector<QoreParseTypeInfo* > ptype_vec_t;
typedef std::vector<LocalVar* > lvar_vec_t;

class AbstractFunctionSignature {
protected:
   unsigned short num_param_types,    // number of parameters that have type information
      min_param_types;                // minimum number of parameters with type info (without default args)

   const QoreTypeInfo* returnTypeInfo;
   type_vec_t typeList;
   arg_vec_t defaultArgList;
   name_vec_t names;

   // parameter signature string
   std::string str;

public:
   DLLLOCAL AbstractFunctionSignature(const QoreTypeInfo* n_returnTypeInfo = 0) : num_param_types(0), min_param_types(0), returnTypeInfo(n_returnTypeInfo) {
   }
   DLLLOCAL AbstractFunctionSignature(const QoreTypeInfo* n_returnTypeInfo, const type_vec_t& n_typeList, const arg_vec_t& n_defaultArgList, const name_vec_t& n_names) : num_param_types(0), min_param_types(0), returnTypeInfo(n_returnTypeInfo), typeList(n_typeList), defaultArgList(n_defaultArgList), names(n_names) {
   }
   DLLLOCAL virtual ~AbstractFunctionSignature() {
      // delete all default argument expressions
      for (arg_vec_t::iterator i = defaultArgList.begin(), e = defaultArgList.end(); i != e; ++i)
         if (*i)
            (*i)->deref(0);
   }
   // called at parse time to include optional type resolution
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const = 0;

   DLLLOCAL virtual const QoreParseTypeInfo* getParseParamTypeInfo(unsigned num) const = 0;

   DLLLOCAL const QoreTypeInfo* getReturnTypeInfo() const {
      return returnTypeInfo;
   }

   DLLLOCAL const arg_vec_t& getDefaultArgList() const {
      return defaultArgList;
   }

   DLLLOCAL const type_vec_t& getTypeList() const {
      return typeList;
   }

   DLLLOCAL AbstractQoreNode* evalDefaultArg(unsigned i, ExceptionSink* xsink) const {
      assert(i < defaultArgList.size());
      return defaultArgList[i] ? defaultArgList[i]->eval(xsink) : 0;
   }

   DLLLOCAL const char* getSignatureText() const {
      return str.c_str();
   }

   DLLLOCAL unsigned numParams() const {
      return (unsigned)typeList.size();
   }

   // number of params with type information
   DLLLOCAL unsigned getParamTypes() const {
      return num_param_types;
   }

   DLLLOCAL unsigned getMinParamTypes() const {
      return min_param_types;
   }

   DLLLOCAL const QoreTypeInfo* getParamTypeInfo(unsigned num) const {
      return num >= typeList.size() ? 0 : typeList[num];
   }

   DLLLOCAL bool hasDefaultArg(unsigned i) const {
      return i >= defaultArgList.size() || !defaultArgList[i] ? false : true;
   }

   // adds a description of the given default argument to the signature string
   DLLLOCAL void addDefaultArgument(const AbstractQoreNode* arg);

   DLLLOCAL const char* getName(unsigned i) const {
      return i < names.size() ? names[i].c_str() : 0;
   }
};

// used to store return type info during parsing for user code
class RetTypeInfo {
   QoreParseTypeInfo* parseTypeInfo;
   const QoreTypeInfo* typeInfo;

public:

   DLLLOCAL RetTypeInfo(QoreParseTypeInfo* n_parseTypeInfo, const QoreTypeInfo* n_typeInfo) : parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
   }
   DLLLOCAL ~RetTypeInfo() {
      delete parseTypeInfo;      
   }
   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      return typeInfo;
   }
   DLLLOCAL QoreParseTypeInfo* takeParseTypeInfo() {
      QoreParseTypeInfo* rv = parseTypeInfo;
      parseTypeInfo = 0;
      return rv;
   }
};

class UserSignature : public AbstractFunctionSignature {
protected:
   ptype_vec_t parseTypeList;
   QoreParseTypeInfo* parseReturnTypeInfo;

   int first_line, last_line;
   const char* parse_file;

   DLLLOCAL void pushParam(BarewordNode* b, bool needs_types);
   DLLLOCAL void pushParam(QoreOperatorNode* t, bool needs_types);
   DLLLOCAL void pushParam(VarRefNode* v, AbstractQoreNode* defArg, bool needs_types);

   DLLLOCAL static void param_error() {
      parse_error("parameter list contains non-variable reference expressions");
   }

public:
   lvar_vec_t lv;
   LocalVar* argvid;
   LocalVar* selfid;
   bool resolved;

   DLLLOCAL UserSignature(int n_first_line, int n_last_line, AbstractQoreNode* params, RetTypeInfo* retTypeInfo);

   DLLLOCAL virtual ~UserSignature() {
      for (ptype_vec_t::iterator i = parseTypeList.begin(), e = parseTypeList.end(); i != e; ++i)
	 delete* i;
      delete parseReturnTypeInfo;
   }

   DLLLOCAL void setFirstParamType(const QoreTypeInfo* typeInfo) {
      assert(!typeList.empty());
      typeList[0] = typeInfo;
   }

   DLLLOCAL void setSelfId(LocalVar* n_selfid) {
      assert(!selfid);
      selfid = n_selfid;
   }

   DLLLOCAL virtual const QoreParseTypeInfo* getParseParamTypeInfo(unsigned num) const {
      return num < parseTypeList.size() ? parseTypeList[num] : 0;
   }
      
   // resolves all parse types to the final types
   DLLLOCAL void resolve();

   // called at parse time to ensure types are resolved
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const {
      const_cast<UserSignature* >(this)->resolve();
      return returnTypeInfo;
   }

   DLLLOCAL int firstLine() const {
      return first_line;
   }
   
   DLLLOCAL int lastLine() const {
      return last_line;
   }
   
   DLLLOCAL const char* getParseFile() const {
      return parse_file;
   }

   DLLLOCAL bool hasReturnTypeInfo() const {
      return parseReturnTypeInfo || returnTypeInfo;
   }

   DLLLOCAL void parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo);

   // returns the $argv reference count
   DLLLOCAL void parseInitPopLocalVars();
};

class AbstractQoreFunctionVariant;

class CodeEvaluationHelper {
protected:
   qore_call_t ct;
   const char* name;
   ExceptionSink* xsink;
   const char* class_name,* o_fn;
   int o_ln, o_eln;
   QoreListNodeEvalOptionalRefHolder tmp;
   const QoreTypeInfo* returnTypeInfo; // saved return type info
   QoreProgram* pgm; // program used when evaluated (to find stacks for references)

public:
   // saves current program location in case there's an exception
   DLLLOCAL CodeEvaluationHelper(ExceptionSink* n_xsink, const QoreFunction* func, const AbstractQoreFunctionVariant*& variant, const char* n_name, const QoreListNode* args = 0, const char* n_class_name = 0, qore_call_t n_ct = CT_UNUSED);

   DLLLOCAL ~CodeEvaluationHelper() {
      if (returnTypeInfo != (const QoreTypeInfo* )-1)
         saveReturnTypeInfo(returnTypeInfo);
      if (ct != CT_UNUSED && xsink->isException())
	 xsink->addStackInfo(ct, class_name, name, o_fn, o_ln, o_eln);
   }
   DLLLOCAL void setReturnTypeInfo(const QoreTypeInfo* n_returnTypeInfo) {
      returnTypeInfo = saveReturnTypeInfo(n_returnTypeInfo);
   }
   // once this is set, exception information will be raised in the destructor if an exception has been raised
   DLLLOCAL void setCallType(qore_call_t n_ct) {
      ct = n_ct;
   }
   DLLLOCAL int processDefaultArgs(const QoreFunction* func, const AbstractQoreFunctionVariant* variant, bool check_args);

   DLLLOCAL void setArgs(QoreListNode* n_args) {
      assert(!*tmp);
      tmp.assign(true, n_args);
   }

   DLLLOCAL const QoreListNode* getArgs() const {
      return *tmp;
   }
   // returns the QoreProgram object where the call originated
   DLLLOCAL QoreProgram* getSourceProgram() const {
      return pgm;
   }
   DLLLOCAL void restorePosition() const {
      update_pgm_counter_pgm_file(o_ln, o_eln, o_fn);   
   }
};

class UserVariantBase;

// describes the details of the function variant
class AbstractQoreFunctionVariant : protected QoreReferenceCounter {
protected:
   // code flags
   int64 flags;
   bool is_user;

   DLLLOCAL virtual ~AbstractQoreFunctionVariant() {}

public:
   DLLLOCAL AbstractQoreFunctionVariant(int64 n_flags, bool n_is_user = false) : flags(n_flags), is_user(n_is_user) {}

   DLLLOCAL virtual AbstractFunctionSignature* getSignature() const = 0;
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const = 0;

   DLLLOCAL const QoreTypeInfo* getReturnTypeInfo() const {
      return getSignature()->getReturnTypeInfo();
   }

   DLLLOCAL unsigned numParams() const {
      AbstractFunctionSignature* sig = getSignature();
      return sig ? sig->numParams() : 0;
   }

   DLLLOCAL qore_call_t getCallType() const {
      return is_user ? CT_USER : CT_BUILTIN;
   }

   DLLLOCAL int64 getFlags() const {
      return flags;
   }

   DLLLOCAL virtual int64 getFunctionality() const = 0;

   // set flag to recheck params against committed variants in stage 2 parsing after type resolution (only for user variants); should never be called with builtin variants
   DLLLOCAL virtual void setRecheck() {
      assert(false);
   }

   DLLLOCAL virtual UserVariantBase* getUserVariantBase() {
      return 0;
   }

   DLLLOCAL const UserVariantBase* getUserVariantBase() const {
      return const_cast<AbstractQoreFunctionVariant* >(this)->getUserVariantBase();
   }

   DLLLOCAL virtual AbstractQoreNode* evalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      assert(false);
      return 0;
   }

   DLLLOCAL virtual int64 bigIntEvalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalFunction(name, ceh, xsink), xsink);
      return rv ? rv->getAsBigInt() : 0;
   }

   DLLLOCAL virtual int intEvalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalFunction(name, ceh, xsink), xsink);
      return rv ? rv->getAsInt() : 0;
   }

   DLLLOCAL virtual bool boolEvalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalFunction(name, ceh, xsink), xsink);
      return rv ? rv->getAsBool() : false;
   }

   DLLLOCAL virtual double floatEvalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      ReferenceHolder<AbstractQoreNode> rv(evalFunction(name, ceh, xsink), xsink);
      return rv ? rv->getAsFloat() : 0.0;
   }

   DLLLOCAL virtual const QoreClass* getClass() const {
      return 0;
   }

   DLLLOCAL const char* className() const {
      const QoreClass* qc = getClass();
      return qc ? qc->getName() : 0;
   }

   DLLLOCAL AbstractQoreFunctionVariant* ref() { ROreference(); return this; }
   DLLLOCAL void deref() { if (ROdereference()) { delete this; } }

   DLLLOCAL bool isUser() const {
      return is_user;
   }

   // the default implementation of this function does nothing
   DLLLOCAL virtual void parseInit(QoreFunction* f) {
   }
};

class VRMutex;
class UserVariantExecHelper;

// base implementation shared between all user variants
class UserVariantBase {
   friend class UserVariantExecHelper;

protected:
   UserSignature signature;
   StatementBlock* statements;
   // for "synchronized" functions
   VRMutex* gate;
public:
   QoreProgram* pgm;
protected:
   // flag to recheck params against committed after type resolution
   bool recheck;
   // flag to tell if variant has been initialized or not (still in pending list)
   bool init;

   DLLLOCAL AbstractQoreNode* evalIntern(ReferenceHolder<QoreListNode>& argv, QoreObject* self, ExceptionSink* xsink, const char* class_name) const;
   DLLLOCAL AbstractQoreNode* eval(const char* name, CodeEvaluationHelper* ceh, QoreObject* self, ExceptionSink* xsink, const char* class_name = 0) const;
   DLLLOCAL int setupCall(CodeEvaluationHelper* ceh, ReferenceHolder<QoreListNode>& argv, ExceptionSink* xsink) const;

public:

   DLLLOCAL UserVariantBase(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced);
   DLLLOCAL virtual ~UserVariantBase();
   DLLLOCAL UserSignature* getUserSignature() const {
      return const_cast<UserSignature* >(&signature);
   }

   DLLLOCAL void setInit() {
      init = true;
   }

   DLLLOCAL bool getInit() const {
      return init;
   }

   DLLLOCAL void parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo);

   DLLLOCAL void parseInitPopLocalVars();
};

// the following defines the pure virtual functions that are common to all user variants
#define COMMON_USER_VARIANT_FUNCTIONS DLLLOCAL virtual int64 getFunctionality() const { return QDOM_DEFAULT; } \
   using AbstractQoreFunctionVariant::getUserVariantBase; \
   DLLLOCAL virtual UserVariantBase* getUserVariantBase() { return static_cast<UserVariantBase* >(this); } \
   DLLLOCAL virtual AbstractFunctionSignature* getSignature() const { return const_cast<UserSignature* >(&signature); } \
   DLLLOCAL virtual const QoreTypeInfo* parseGetReturnTypeInfo() const { return signature.parseGetReturnTypeInfo(); } \
   DLLLOCAL virtual void setRecheck() { recheck = true; }

// this class ensures that instantiated variables in user code are uninstantiated, even if an exception occurs
class UserVariantExecHelper {
protected:
   const UserVariantBase* uvb;
   ReferenceHolder<QoreListNode> argv;
   ExceptionSink* xsink;

public:
   DLLLOCAL UserVariantExecHelper(const UserVariantBase* n_uvb, CodeEvaluationHelper* ceh, ExceptionSink* n_xsink) : uvb(n_uvb), argv(n_xsink), xsink(n_xsink) {
      if (uvb->setupCall(ceh, argv, xsink))
	 uvb = 0;
   }
   DLLLOCAL ~UserVariantExecHelper();
   DLLLOCAL operator bool() const {
      return uvb;
   }
   DLLLOCAL ReferenceHolder<QoreListNode>& getArgv() {
      return argv;
   }
};

class UserFunctionVariant : public AbstractQoreFunctionVariant, public UserVariantBase {
protected:
   DLLLOCAL virtual ~UserFunctionVariant() {
   }

public:
   DLLLOCAL UserFunctionVariant(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced, int64 n_flags = QC_NO_FLAGS) : AbstractQoreFunctionVariant(n_flags, true), UserVariantBase(b, n_sig_first_line, n_sig_last_line, params, rv, synced) {
   }

   // the following defines the virtual functions that are common to all user variants
   COMMON_USER_VARIANT_FUNCTIONS

   DLLLOCAL virtual AbstractQoreNode* evalFunction(const char* name, CodeEvaluationHelper& ceh, ExceptionSink* xsink) const {
      return eval(name,& ceh, 0, xsink);
   }

   DLLLOCAL virtual void parseInit(QoreFunction* f);
};

#define UFV(f) (reinterpret_cast<UserFunctionVariant* >(f))
#define UFV_const(f) (reinterpret_cast<const UserFunctionVariant* >(f))

// type for lists of function variants
// this type will be read at runtime and could be appended to simultaneously at parse time (under a lock)
typedef safe_dslist<AbstractQoreFunctionVariant* > vlist_t;

class VList : public vlist_t {
public:
   DLLLOCAL ~VList() {
      del();
   }
   DLLLOCAL void del() {
      // dereference all variants
      for (vlist_t::iterator i = begin(), e = end(); i != e; ++i)
	 (*i)->deref();
      clear();
   }
};

class QoreFunction : protected QoreReferenceCounter {
protected:
   std::string name;

   // inheritance list type
   typedef std::vector<QoreFunction* > ilist_t;

   // list of function variants
   VList vlist;

   // list of pending user-code function variants
   VList pending_vlist;

   // list of inherited methods for variant matching; the first pointer is always a pointer to "this"
   ilist_t ilist;

   // if true means all variants have the same return value
   bool same_return_type, parse_same_return_type;
   int64 unique_functionality;
   int64 unique_flags;

   // same as above but for variants without QC_RUNTIME_NOOP
   bool nn_same_return_type;
   int64 nn_unique_functionality;
   int64 nn_unique_flags;
   int nn_count;
   bool parse_rt_done;
   bool parse_init_done;
   bool has_user;                   // has at least 1 committed user variant
   bool has_builtin;                // has at least 1 committed builtin variant

   const QoreTypeInfo* nn_uniqueReturnType;

   DLLLOCAL void parseCheckReturnType() {
      if (parse_rt_done)
         return;

      parse_rt_done = true;

      if (!same_return_type || pending_vlist.empty())
         return;         

      for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
         reinterpret_cast<UserSignature* >((*i)->getUserVariantBase()->getUserSignature())->resolve();
         const QoreTypeInfo* rti = (*i)->getReturnTypeInfo();

         if (i == pending_vlist.begin()) {
            if (!vlist.empty()) {
               if (!rti->isOutputIdentical(first()->getReturnTypeInfo())) {
                  parse_same_return_type = false;
                  break;
               }
            }
            continue;
         }

         if (!rti->isOutputIdentical(pending_first()->getReturnTypeInfo())) {
            parse_same_return_type = false;
            break;
         }
      }
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL const AbstractQoreFunctionVariant* first() const {
      assert(!vlist.empty());
      return *(vlist.begin());
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL AbstractQoreFunctionVariant* first() {
      assert(!vlist.empty());
      return *(vlist.begin());
   }

   // convenience function for returning the first variant in the list
   DLLLOCAL const AbstractQoreFunctionVariant* pending_first() const {
      assert(!pending_vlist.empty());
      return *(pending_vlist.begin());
   }

   // returns 0 for OK, -1 for error
   DLLLOCAL int parseCheckDuplicateSignature(AbstractQoreFunctionVariant* variant);

   // FIXME: does not check unparsed types properly
   DLLLOCAL void addVariant(AbstractQoreFunctionVariant* variant) {
      const QoreTypeInfo* rti = variant->getReturnTypeInfo();
      if (same_return_type && !vlist.empty() && !rti->isOutputIdentical(first()->getReturnTypeInfo()))
	 same_return_type = false;

      int64 vf = variant->getFunctionality();
      int64 vflags = variant->getFlags();

      bool rtn = (bool)(vflags & QC_RUNTIME_NOOP);

      if (vlist.empty()) {
	 unique_functionality = vf;
         unique_flags = vflags;
      }
      else {
         unique_functionality &= vf;
         unique_flags &= vflags;
      }

      if (!rtn) {
         if (!nn_count) {
            nn_unique_functionality = vf;
            nn_unique_flags = vflags;
            nn_uniqueReturnType = rti;
            ++nn_count;
         }
         else {
            nn_unique_functionality &= vf;
            nn_unique_flags &= vflags;
            if (nn_uniqueReturnType && !rti->isOutputIdentical(nn_uniqueReturnType))
               nn_uniqueReturnType = 0;
            ++nn_count;
         }
      }

      vlist.push_back(variant);
   }

   DLLLOCAL virtual ~QoreFunction() {
   }

public:
   DLLLOCAL QoreFunction(const char* n_name) : name(n_name), same_return_type(true), parse_same_return_type(true), unique_functionality(QDOM_DEFAULT), unique_flags(QC_NO_FLAGS),
                                               nn_same_return_type(true), nn_unique_functionality(QDOM_DEFAULT), nn_unique_flags(QC_NO_FLAGS), nn_count(0), parse_rt_done(true), 
                                               parse_init_done(true), has_user(false), has_builtin(false), nn_uniqueReturnType(0) {
      ilist.push_back(this);
   }

   // copy constructor (used by method functions when copied)
   DLLLOCAL QoreFunction(const QoreFunction& old, int64 po = 0) : name(old.name), same_return_type(old.same_return_type), 
                                                                  parse_same_return_type(true), 
                                                                  unique_functionality(old.unique_functionality),
                                                                  unique_flags(old.unique_flags),
                                                                  nn_same_return_type(old.nn_same_return_type), 
                                                                  nn_unique_functionality(old.nn_unique_functionality),
                                                                  nn_unique_flags(old.nn_unique_flags),
                                                                  nn_count(old.nn_count),
                                                                  parse_rt_done(true), parse_init_done(true),
                                                                  has_user(old.has_user), has_builtin(old.has_builtin),
                                                                  nn_uniqueReturnType(old.nn_uniqueReturnType) {
      // copy variants by reference
      for (vlist_t::const_iterator i = old.vlist.begin(), e = old.vlist.end(); i != e; ++i) {
         if ((po & PO_NO_USER_FUNC_VARIANTS) && (*i)->isUser())
            continue;
         if ((po & PO_NO_SYSTEM_FUNC_VARIANTS) && !(*i)->isUser())
            continue;
         vlist.push_back((*i)->ref());
      }

      // make sure the new variant list is not empty if the parent also wasn't
      assert(old.vlist.empty() || !vlist.empty());

      assert(!old.ilist.empty());
      assert(old.ilist.front() == &old);

      // resolve initial ilist entry to this function
      ilist.push_back(this);

      // the rest of ilist is copied in method base class
      // do not copy pending variants
   }

   // returns 0 for OK, -1 for error
   DLLLOCAL int parseCheckDuplicateSignatureCommitted(AbstractFunctionSignature* sig);

   DLLLOCAL const char* getName() const {
      return name.c_str();
   }

   DLLLOCAL virtual const QoreClass* getClass() const {
      return 0;
   }

   DLLLOCAL void ref() {
      ROreference();
   }

   DLLLOCAL void deref() {
      if (ROdereference())
	 delete this;
   }

   DLLLOCAL const char* className() const {
      const QoreClass* qc = getClass();
      return qc ? qc->getName() : 0;
   }

   DLLLOCAL void addAncestor(QoreFunction* ancestor) {
      ilist.push_back(ancestor);
   }
   
   DLLLOCAL void addNewAncestor(QoreFunction* ancestor) {
      for (ilist_t::iterator i = ilist.begin(), e = ilist.end(); i != e; ++i)
         if (*i == ancestor)
            return;
      ilist.push_back(ancestor);
   }

   // resolves all types in signatures and return types in pending variants; called during the "parseInit" phase
   DLLLOCAL void resolvePendingSignatures();

   DLLLOCAL AbstractFunctionSignature* getUniqueSignature() const {
      return vlist.singular() ? first()->getSignature() : 0;
   }

   DLLLOCAL AbstractFunctionSignature* parseGetUniqueSignature() const;

   DLLLOCAL int64 getUniqueFunctionality() const {
      if (getProgram()->getParseOptions64() & PO_REQUIRE_TYPES)
         return nn_unique_functionality;
      return unique_functionality;
   }

   DLLLOCAL int64 getUniqueFlags() const {
      if (getProgram()->getParseOptions64() & PO_REQUIRE_TYPES)
         return nn_unique_flags;
      return unique_flags;
   }

   // object takes ownership of variant or deletes it if it can't be added
   DLLLOCAL int addPendingVariant(AbstractQoreFunctionVariant* variant);

   DLLLOCAL void addBuiltinVariant(AbstractQoreFunctionVariant* variant);

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();

   DLLLOCAL const QoreTypeInfo* getUniqueReturnTypeInfo() const {
      if (getProgram()->getParseOptions64() & PO_REQUIRE_TYPES)
         return nn_uniqueReturnType;

      return same_return_type && !vlist.empty() ? first()->getReturnTypeInfo() : 0;
   }

   DLLLOCAL const QoreTypeInfo* parseGetUniqueReturnTypeInfo() {
      parseCheckReturnType();

      if (getProgram()->getParseOptions64() & PO_REQUIRE_TYPES) {
         if (!nn_same_return_type || !parse_same_return_type)
            return 0;

         return nn_count ? nn_uniqueReturnType : (!pending_vlist.empty() ? pending_first()->getReturnTypeInfo() : 0);
      }

      if (!same_return_type || !parse_same_return_type)
         return 0;

      if (!vlist.empty())
         return first()->getReturnTypeInfo();

      assert(!pending_vlist.empty());
      return pending_first()->getReturnTypeInfo();
   }

   // if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
   DLLLOCAL virtual AbstractQoreNode* evalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram* pgm, ExceptionSink* xsink) const;
   DLLLOCAL int64 bigIntEvalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram* pgm, ExceptionSink* xsink) const;
   DLLLOCAL int intEvalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram* pgm, ExceptionSink* xsink) const;
   DLLLOCAL bool boolEvalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram* pgm, ExceptionSink* xsink) const;
   DLLLOCAL double floatEvalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram* pgm, ExceptionSink* xsink) const;

   // finds a variant and checks variant capabilities against current program parse options and executes the variant
   DLLLOCAL AbstractQoreNode* evalDynamic(const QoreListNode* args, ExceptionSink* xsink) const;

   // find variant at parse time, throw parse exception if no variant can be matched
   DLLLOCAL const AbstractQoreFunctionVariant* parseFindVariant(const type_vec_t& argTypeInfo);

   // returns true if there are no committed variants in the function
   DLLLOCAL bool committedEmpty() const {
      return vlist.empty();
   }

   // returns true if there are no pending variants in the function
   DLLLOCAL bool pendingEmpty() const {
      return pending_vlist.empty();
   }

   DLLLOCAL bool existsVariant(const type_vec_t& paramTypeInfo) const;

   // find variant at runtime
   // if only_user is set, then no exception is raised if the user variant is not found
   DLLLOCAL const AbstractQoreFunctionVariant* findVariant(const QoreListNode* args, bool only_user, ExceptionSink* xsink) const;

   DLLLOCAL const AbstractQoreFunctionVariant* runtimeFindVariant(const type_vec_t& argTypeList, bool only_user = false) const;

   // convenience function for returning the first variant in the list
   DLLLOCAL AbstractQoreFunctionVariant* pending_first() {
      assert(!pending_vlist.empty());
      return *(pending_vlist.begin());
   }

   DLLLOCAL bool hasUser() const {
      return has_user;
   }

   DLLLOCAL bool hasBuiltin() const {
      return has_builtin;
   }
};

class MethodVariantBase;
class MethodFunctionBase;
#define METHFB(f) (reinterpret_cast<MethodFunctionBase* >(f))

class MethodFunctionBase : public QoreFunction {
protected:
   bool all_private, 
      pending_all_private,
      is_static;
   const QoreClass* qc;   

   // pointer to copy, only valid during copy
   mutable MethodFunctionBase* new_copy;

public:
   DLLLOCAL MethodFunctionBase(const char* nme, const QoreClass* n_qc, bool n_is_static) : QoreFunction(nme), all_private(true), pending_all_private(true), is_static(n_is_static), qc(n_qc), new_copy(0) {
   }

   // copy constructor, only copies comitted variants
   DLLLOCAL MethodFunctionBase(const MethodFunctionBase& old, const QoreClass* n_qc) 
      : QoreFunction(old), 
        all_private(old.all_private), 
        pending_all_private(true),
        is_static(old.is_static),
        qc(n_qc) {
      //printd(5, "MethodFunctionBase() copying old=%p -> new=%p %p %s::%s() %p %s::%s()\n",& old, this, old.qc, old.qc->getName(), old.getName(), qc, qc->getName(), old.getName());

      // set a pointer to the new function
      old.new_copy = this;

      // copy ilist, will be adjusted for new class pointers after all classes have been copied
      ilist.reserve(old.ilist.size());
      ilist_t::const_iterator i = old.ilist.begin(), e = old.ilist.end();
      ++i;
      for (; i != e; ++i)
         ilist.push_back(*i);
   }

   DLLLOCAL void resolveCopy() {
      ilist_t::iterator i = ilist.begin(), e = ilist.end(); 
      ++i;
      for (; i != e; ++i) {
         MethodFunctionBase* mfb = METHFB(*i);
#ifdef DEBUG
         if (!mfb->new_copy)
            printd(0, "error resolving %p %s::%s() base method %p %s::%s() nas no new method pointer\n", qc, qc->getName(), getName(), mfb->qc, mfb->qc->getName(), getName());
         assert(mfb->new_copy);
         //printd(5, "resolving %p %s::%s() base method %p %s::%s() from %p -> %p\n", qc, qc->getName(), getName(), mfb->qc, mfb->qc->getName(), getName(), mfb, mfb->new_copy);
#endif
        *i = mfb->new_copy;
      }
   }

   // returns -1 for error, 0 = OK
   DLLLOCAL int parseAddUserMethodVariant(MethodVariantBase* variant);
   // maintains all_private flag and commits the builtin variant
   DLLLOCAL void addBuiltinMethodVariant(MethodVariantBase* variant);
   // maintains all_private flag and commits user variants
   DLLLOCAL void parseCommitMethod(QoreString& csig, bool is_static);
   DLLLOCAL void parseRollbackMethod();
   DLLLOCAL bool isUniquelyPrivate() const {
      return all_private;
   }
   DLLLOCAL bool parseIsUniquelyPrivate() const {
      return all_private && pending_all_private;
   }

   DLLLOCAL virtual const QoreClass* getClass() const {
      return qc;
   }

   DLLLOCAL const char* getClassName() const {
      return qc->getName();
   }

   DLLLOCAL bool isStatic() const {
      return is_static;
   }

   // virtual copy constructor
   DLLLOCAL virtual MethodFunctionBase* copy(const QoreClass* n_qc) const = 0;
};

class UserParamListLocalVarHelper {
protected:
   UserVariantBase* uvb;

public:
   DLLLOCAL UserParamListLocalVarHelper(UserVariantBase* n_uvb, const QoreTypeInfo* classTypeInfo = 0) : uvb(n_uvb) {
      uvb->parseInitPushLocalVars(classTypeInfo);
   }

   DLLLOCAL ~UserParamListLocalVarHelper() {
      uvb->parseInitPopLocalVars();
   }
};

class UserClosureVariant : public UserFunctionVariant {
protected:
public:
   DLLLOCAL UserClosureVariant(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced = false, int64 n_flags = QC_NO_FLAGS) : UserFunctionVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced, n_flags) {
   }

   DLLLOCAL virtual void parseInit(QoreFunction* f);

   DLLLOCAL AbstractQoreNode* evalClosure(CodeEvaluationHelper& ceh, QoreObject* self, ExceptionSink* xsink) const {
      return eval("<anonymous closure>",& ceh, self, xsink);
   }
};

#define UCLOV(f) (reinterpret_cast<UserClosureVariant* >(f))
#define UCLOV_const(f) (reinterpret_cast<const UserClosureVariant* >(f))

class UserClosureFunction : public QoreFunction {
protected:
   lvar_set_t varlist;  // closure local variable environment
   const QoreTypeInfo* classTypeInfo;

public:
   DLLLOCAL UserClosureFunction(StatementBlock* b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced = false, int64 n_flags = QC_NO_FLAGS) : QoreFunction("<anonymous closure>"), classTypeInfo(0) {
      addPendingVariant(new UserClosureVariant(b, n_sig_first_line, n_sig_last_line, params, rv, synced, n_flags));
   }

   DLLLOCAL bool parseStage1HasReturnTypeInfo() const {
      return reinterpret_cast<const UserClosureVariant* >(pending_first())->getUserSignature()->hasReturnTypeInfo();
   }

   DLLLOCAL void parseInitClosure(const QoreTypeInfo* classTypeInfo);
   
   DLLLOCAL AbstractQoreNode* evalClosure(const QoreListNode* args, QoreObject* self, ExceptionSink* xsink) const;

   DLLLOCAL void setClassType(const QoreTypeInfo* cti) {
      classTypeInfo = cti;
   }

   DLLLOCAL const QoreTypeInfo* getClassType() const {
      return classTypeInfo;
   }

   DLLLOCAL lvar_set_t* getVList() {
      return &varlist;
   }
};

#endif // _QORE_FUNCTION_H
