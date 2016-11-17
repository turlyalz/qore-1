/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreInstanceOfOperatorNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREINSTANCEOFOPERATORNODE_H

#define _QORE_QOREINSTANCEOFOPERATORNODE_H

class QoreInstanceOfOperatorNode : public QoreSingleExpressionOperatorNode<QoreOperatorNode> {
protected:
   ClassRefNode* r;

   DLLLOCAL static QoreString InstanceOf_str;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& typeInfo);

public:
   DLLLOCAL QoreInstanceOfOperatorNode(AbstractQoreNode* n_exp, ClassRefNode* ref) : QoreSingleExpressionOperatorNode<QoreOperatorNode>(n_exp), r(ref) {
   }

   DLLLOCAL virtual ~QoreInstanceOfOperatorNode() {
      if (r)
         r->deref();
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return InstanceOf_str.getBuffer();
   }

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return boolTypeInfo;
   }

   DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
      ReferenceHolder<> n_exp(copy_and_resolve_lvar_refs(exp, xsink), xsink);
      if (*xsink)
         return 0;
      assert(r);
      r->ref();
      return new QoreInstanceOfOperatorNode(n_exp.release(), r);
   }
};

#endif