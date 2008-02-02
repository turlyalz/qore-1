/*
  QoreNullNode.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include <qore/Qore.h>

QoreNullNode::QoreNullNode() : SimpleQoreNode(NT_NULL)
{
}

QoreNullNode::~QoreNullNode()
{
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreNullNode::getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const
{
   str.concat(&NullTypeString);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreNullNode::getAsString(bool &del, int foff, class ExceptionSink *xsink) const
{
   del = false;
   return &NullTypeString;
}

AbstractQoreNode *QoreNullNode::realCopy() const
{
   return new QoreNullNode();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL int QoreNullNode::compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreNullNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return dynamic_cast<const QoreNullNode *>(v);
}

bool QoreNullNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return dynamic_cast<const QoreNullNode *>(v);
}

// returns the data type
const QoreType *QoreNullNode::getType() const
{
   return NT_NULL;
}

// returns the type name as a c string
const char *QoreNullNode::getTypeName() const
{
   return "NULL";
}
