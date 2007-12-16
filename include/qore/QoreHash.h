/*
  QoreHash.h

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

#ifndef _QORE_HASH_H

#define _QORE_HASH_H

#include <qore/common.h>
#include <qore/hash_map.h>

#include <stdio.h>
#include <string.h>

// FIXME: use STL list instead
// to maintain the order of inserts
class HashMember {
   public:
      class QoreNode *node;
      char *key;
      class HashMember *next;
      class HashMember *prev;
};

class HashIterator
{
   private:
      class QoreHash *h;
      class HashMember *ptr;

      // not implemented
      DLLLOCAL HashIterator(const HashIterator&);
      DLLLOCAL HashIterator& operator=(const HashIterator&);

   public:
      DLLEXPORT HashIterator(class QoreHash *h);
      DLLEXPORT HashIterator(class QoreHash &h);
      DLLEXPORT class HashMember *next();
      DLLEXPORT const char *getKey() const;
      DLLEXPORT class QoreString *getKeyString() const;
      DLLEXPORT class QoreNode *getValue() const;
      DLLEXPORT class QoreNode *takeValueAndDelete();
      DLLEXPORT class QoreNode **getValuePtr() const;
      DLLEXPORT class QoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT bool first() const;
      DLLEXPORT bool last() const;
      //DLLEXPORT void setValue(class QoreNode *val, class ExceptionSink *xsink);
};

class QoreHash
{
      friend class HashIterator;

   private:
      class HashMember *member_list;
      class HashMember *tail;
      hm_hm_t hm;
      bool needs_eval;

      DLLLOCAL class QoreNode **newKeyValue(const char *key, class QoreNode *value);
      DLLLOCAL void internDeleteKey(class HashMember *m);
      DLLLOCAL void deref_intern(class ExceptionSink *xsink);

      // not implemented
      DLLLOCAL QoreHash(const QoreHash&);
      DLLLOCAL QoreHash& operator=(const QoreHash&);

  protected:
      DLLEXPORT ~QoreHash();

   public:
      DLLEXPORT QoreHash(bool ne = false);

      DLLEXPORT const char *getFirstKey() const;
      DLLEXPORT const char *getLastKey() const;
      // returns (QoreNode *)-1 if the key doesn't exist
      DLLEXPORT class QoreNode *getKeyValueExistence(const char *key) const;
      // returns (QoreNode *)-1 if the key doesn't exist
      DLLEXPORT class QoreNode *getKeyValueExistence(const class QoreString *key, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *getKeyValue(const class QoreString *key, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *getKeyValue(const char *key) const;
      DLLEXPORT class QoreHash *copy() const;
      DLLEXPORT class QoreNode **getKeyValuePtr(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getKeyValuePtr(const char *key);
      DLLEXPORT class QoreNode **getExistingValuePtr(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getExistingValuePtr(const char *key);
      DLLEXPORT void merge(const class QoreHash *h, class ExceptionSink *xsink);
      DLLEXPORT void assimilate(class QoreHash *h, class ExceptionSink *xsink);
      DLLEXPORT class QoreHash *eval(class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *evalKey(const char *key, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *evalKeyExistence(const char *key, class ExceptionSink *xsink) const;
      DLLEXPORT void setKeyValue(const class QoreString *key, class QoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void setKeyValue(const char *key, class QoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(const char *key, class ExceptionSink *xsink);
      // "takes" the value of the key from the hash and removes the key from the hash and returns the value
      DLLEXPORT class QoreNode *takeKeyValue(const class QoreString *key, class ExceptionSink *xsink);
      // "takes" the value of the key from the hash and removes the key from the hash and returns the value
      DLLEXPORT class QoreNode *takeKeyValue(const char *key);
      DLLEXPORT class QoreList *getKeys() const;
      DLLEXPORT bool compareSoft(class QoreHash *h, class ExceptionSink *xsink) const;
      DLLEXPORT bool compareHard(class QoreHash *h, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *evalFirstKeyValue(class ExceptionSink *xsink) const;
      DLLEXPORT void dereference(class ExceptionSink *xsink);
      DLLEXPORT void derefAndDelete(class ExceptionSink *xsink);
      DLLEXPORT int size() const;
      DLLEXPORT bool needsEval() const;

      DLLLOCAL void clearNeedsEval();
};

class StackHash : public QoreHash
{
   private:
      class ExceptionSink *xsink;
   
      // none of these operators/methods are implemented - here to make sure they are not used
      DLLLOCAL void *operator new(size_t); 
      DLLLOCAL StackHash();
      DLLLOCAL StackHash(bool i);
      DLLLOCAL void derefAndDelete(class ExceptionSink *xsink);
   
   public:
      DLLEXPORT StackHash(class ExceptionSink *xs)
      {
	 xsink = xs;
      }
      DLLEXPORT ~StackHash()
      {
	 dereference(xsink);
      }
};

#endif // _QORE_HASH_H
