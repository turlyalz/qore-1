/*
  QC_Condition.cpp

  Qore Programming Language
  
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

#include <qore/Qore.h>
#include <qore/intern/QC_Condition.h>
#include <qore/intern/QC_AbstractSmartLock.h>
#include <qore/ReferenceHolder.h>

#include <errno.h>

qore_classid_t CID_CONDITION;

static void CONDITION_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_CONDITION, new Condition);
}

static void CONDITION_copy(QoreObject *self, QoreObject *old, Condition *c, ExceptionSink *xsink) {
   self->setPrivate(CID_CONDITION, new Condition);
}

AbstractQoreNode *CONDITION_signal(QoreObject *self, Condition *c, const QoreListNode *params, ExceptionSink *xsink) {
   if (c->signal())
      xsink->raiseException("CONDITION-SIGNAL-ERROR", strerror(errno)); 

   return 0;
}

static AbstractQoreNode *CONDITION_broadcast(QoreObject *self, Condition *c, const QoreListNode *params, ExceptionSink *xsink) {
   if (c->broadcast())
      xsink->raiseException("CONDITION-BROADCAST-ERROR", strerror(errno));

   return 0;
}

// FIXME: make base Qore class for all thread primitive classes that can wait on a condition variable
static AbstractQoreNode *CONDITION_wait(QoreObject *self, Condition *c, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_PARAM(m, AbstractSmartLock, params, 0, CID_MUTEX, xsink);
   if (*xsink)
      return 0;

   assert(m);
   ReferenceHolder<AbstractSmartLock> holder(m, xsink);

   int timeout = getMsZeroInt(get_param(params, 1));
   int rc = timeout ? c->wait(m, timeout, xsink) : c->wait(m, xsink);

   if (rc && rc != ETIMEDOUT && !*xsink) {
      xsink->raiseException("CONDITION-WAIT-ERROR", "unknown system error code returned from Condition::wait(lock=%s, timeout=%d): rc=%d: %s", m->getName(), timeout, rc, strerror(rc));
      return 0;
   }
   return new QoreBigIntNode(rc);   
}

static AbstractQoreNode *CONDITION_wait_count(QoreObject *self, Condition *c, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_PARAM(m, AbstractSmartLock, params, 0, CID_MUTEX, xsink);
   if (*xsink)
      return 0;

   assert(m);
   ReferenceHolder<AbstractSmartLock> holder(m, xsink);

   return new QoreBigIntNode(c->wait_count(m));
}

QoreClass *initConditionClass(QoreClass *AbstractSmartLock) {
   QORE_TRACE("initConditionClass()");

   QoreClass *QC_CONDITION = new QoreClass("Condition", QDOM_THREAD_CLASS);
   CID_CONDITION = QC_CONDITION->getID();

   QC_CONDITION->setConstructor(CONDITION_constructor);
   QC_CONDITION->setCopy((q_copy_t)CONDITION_copy);

   QC_CONDITION->addMethodExtended("signal",        (q_method_t)CONDITION_signal, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_CONDITION->addMethodExtended("broadcast",     (q_method_t)CONDITION_broadcast, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_CONDITION->addMethodExtended("wait",          (q_method_t)CONDITION_wait, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, AbstractSmartLock->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_CONDITION->addMethodExtended("wait",          (q_method_t)CONDITION_wait, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, AbstractSmartLock->getTypeInfo(), QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   QC_CONDITION->addMethodExtended("wait",          (q_method_t)CONDITION_wait, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, AbstractSmartLock->getTypeInfo(), QORE_PARAM_NO_ARG, dateTypeInfo, QORE_PARAM_NO_ARG);
   QC_CONDITION->addMethodExtended("wait_count",    (q_method_t)CONDITION_wait_count, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, AbstractSmartLock->getTypeInfo(), QORE_PARAM_NO_ARG);


   return QC_CONDITION;
}
