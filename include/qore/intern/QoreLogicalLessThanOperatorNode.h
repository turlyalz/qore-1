/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreLogicalLessThanOperatorNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QORELOGICALLESSTHANOPERATORNODE_H

#define _QORE_QORELOGICALLESSTHANOPERATORNODE_H

class QoreLogicalLessThanOperatorNode : public QoreBoolBinaryOperatorNode {
OP_COMMON
protected:
   // type of pointer to optimized versions depending on arguments found at parse-time
   typedef bool(QoreLogicalLessThanOperatorNode::*eval_t)(ExceptionSink* xsink) const;
   // pointer to optimized versions depending on arguments found at parse-time
   eval_t pfunc;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      return parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);
   }

   DLLLOCAL AbstractQoreNode *parseInitIntern(const char* name, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL bool floatLessThan(ExceptionSink* xsink) const;
   DLLLOCAL bool bigIntLessThan(ExceptionSink* xsink) const;

public:
   DLLLOCAL QoreLogicalLessThanOperatorNode(AbstractQoreNode* n_left, AbstractQoreNode* n_right) : QoreBoolBinaryOperatorNode(n_left, n_right), pfunc(0) {
   }

   DLLLOCAL static bool doLessThan(QoreValue l, QoreValue r, ExceptionSink* xsink);
};

#endif
