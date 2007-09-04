/*
 QC_QShortcut.cc
 
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

#include "QC_QShortcut.h"
#include "QC_QKeySequence.h"

int CID_QSHORTCUT;
class QoreClass *QC_QShortcut = 0;

void *static_void_args[] = { 0 };
QByteArray static_void_sig = QMetaObject::normalizedSignature("void a()");

//void QShortcut ( QWidget * parent )
//void QShortcut ( const QKeySequence & key, QWidget * parent, const char * member = 0, const char * ambiguousMember = 0, Qt::ShortcutContext context = Qt::WindowShortcut )
static void QSHORTCUT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQShortcut *qs;
   QoreNode *p = get_param(params, 0);

   QoreAbstractQWidget *parent = (QoreAbstractQWidget *)((p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0);
   if (!parent) {
      QoreQKeySequence *key = (QoreQKeySequence *)((p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink) : 0);
      ReferenceHolder<QoreQKeySequence> keyHolder(key, xsink);
      int nkey = 0;
      if (!key)
	 nkey = p ? p->getAsInt() : 0;

      p = test_param(params, NT_OBJECT, 1);
      parent = (QoreAbstractQWidget *)(p ? p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0);
      if (!parent) {
	 if (!xsink->isException())
	    xsink->raiseException("QSHORTCUT-CONSTRUCTOR-PARAM-ERROR", "QShortcut::constructor() was expecting an object derived from QWidget as the second argument when the first argument is a QKeySequence object");
	 return;
      }
      ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
      p = get_param(params, 2);
      const char *member = p && p->val.String->strlen() ? p->val.String->getBuffer() : 0;
      p = get_param(params, 3);
      const char *ambiguousMember = p && p->val.String->strlen() ? p->val.String->getBuffer() : 0;
      p = get_param(params, 4);
      Qt::ShortcutContext context = !is_nothing(p) ? (Qt::ShortcutContext)p->getAsInt() : Qt::WindowShortcut;

      //printd(5, "QShortcut::constructor() key=%08p nkey=%d context=%d\n", key, nkey, context);
      if (key)
	 qs = new QoreQShortcut(self, *(static_cast<QKeySequence *>(key)), parent, context);
      else
	 qs = new QoreQShortcut(self, nkey, parent, context);

      ReferenceHolder<QoreQShortcut> qsHolder(qs, xsink);

      if (member)
	 qs->setMember(member, xsink);
      if (*xsink)
	 return;

      if (ambiguousMember)
	 qs->setAmbiguousMember(ambiguousMember, xsink);
      if (*xsink)
	 return;

      qsHolder.release();
   }
   else {
      ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
      qs = new QoreQShortcut(self, parent);
   }
   self->setPrivate(CID_QSHORTCUT, qs);
}

static void QSHORTCUT_copy(class Object *self, class Object *old, class QoreQShortcut *qa, ExceptionSink *xsink)
{
   xsink->raiseException("QSHORTCUT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool autoRepeat () const
static QoreNode *QSHORTCUT_autoRepeat(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qs->qobj->autoRepeat());
}

//Qt::ShortcutContext context ()
static QoreNode *QSHORTCUT_context(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->context());
}

//int id () const
static QoreNode *QSHORTCUT_id(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->id());
}

//bool isEnabled () const
static QoreNode *QSHORTCUT_isEnabled(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qs->qobj->isEnabled());
}

//QKeySequence key () const
static QoreNode *QSHORTCUT_key(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qks = new Object(QC_QKeySequence, getProgram());
   QoreQKeySequence *q_qks = new QoreQKeySequence(qs->qobj->key());
   o_qks->setPrivate(CID_QKEYSEQUENCE, q_qks);
   return new QoreNode(o_qks);
}

//QWidget * parentWidget () const
//static QoreNode *QSHORTCUT_parentWidget(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qs->qobj->parentWidget();
//}

//void setAutoRepeat ( bool on )
static QoreNode *QSHORTCUT_setAutoRepeat(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qs->qobj->setAutoRepeat(on);
   return 0;
}

//void setContext ( Qt::ShortcutContext context )
static QoreNode *QSHORTCUT_setContext(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ShortcutContext context = (Qt::ShortcutContext)(p ? p->getAsInt() : 0);
   qs->qobj->setContext(context);
   return 0;
}

//void setEnabled ( bool enable )
static QoreNode *QSHORTCUT_setEnabled(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qs->qobj->setEnabled(enable);
   return 0;
}

//void setKey ( const QKeySequence & key )
static QoreNode *QSHORTCUT_setKey(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQKeySequence *key = (p && p->type == NT_OBJECT) ? (QoreQKeySequence *)p->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink) : 0;
   if (!key) {
      if (!xsink->isException())
         xsink->raiseException("QSHORTCUT-SETKEY-PARAM-ERROR", "expecting a QKeySequence object as first argument to QShortcut::setKey()");
      return 0;
   }
   ReferenceHolder<QoreQKeySequence> holder(key, xsink);
   qs->qobj->setKey(*(static_cast<QKeySequence *>(key)));
   return 0;
}

//void setWhatsThis ( const QString & text )
static QoreNode *QSHORTCUT_setWhatsThis(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QSHORTCUT-SETWHATSTHIS-PARAM-ERROR", "expecting a string as first argument to QShortcut::setWhatsThis()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   qs->qobj->setWhatsThis(text);
   return 0;
}

//QString whatsThis () const
static QoreNode *QSHORTCUT_whatsThis(Object *self, QoreQShortcut *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qs->qobj->whatsThis().toUtf8().data(), QCS_UTF8));
}

class QoreClass *initQShortcutClass(class QoreClass *qobject)
{
   tracein("initQShortcutClass()");
   
   QC_QShortcut = new QoreClass("QShortcut", QDOM_GUI);
   CID_QSHORTCUT = QC_QShortcut->getID();

   QC_QShortcut->addBuiltinVirtualBaseClass(qobject);

   QC_QShortcut->setConstructor(QSHORTCUT_constructor);
   QC_QShortcut->setCopy((q_copy_t)QSHORTCUT_copy);

   QC_QShortcut->addMethod("autoRepeat",                  (q_method_t)QSHORTCUT_autoRepeat);
   QC_QShortcut->addMethod("context",                     (q_method_t)QSHORTCUT_context);
   QC_QShortcut->addMethod("id",                          (q_method_t)QSHORTCUT_id);
   QC_QShortcut->addMethod("isEnabled",                   (q_method_t)QSHORTCUT_isEnabled);
   QC_QShortcut->addMethod("key",                         (q_method_t)QSHORTCUT_key);
   //QC_QShortcut->addMethod("parentWidget",                (q_method_t)QSHORTCUT_parentWidget);
   QC_QShortcut->addMethod("setAutoRepeat",               (q_method_t)QSHORTCUT_setAutoRepeat);
   QC_QShortcut->addMethod("setContext",                  (q_method_t)QSHORTCUT_setContext);
   QC_QShortcut->addMethod("setEnabled",                  (q_method_t)QSHORTCUT_setEnabled);
   QC_QShortcut->addMethod("setKey",                      (q_method_t)QSHORTCUT_setKey);
   QC_QShortcut->addMethod("setWhatsThis",                (q_method_t)QSHORTCUT_setWhatsThis);
   QC_QShortcut->addMethod("whatsThis",                   (q_method_t)QSHORTCUT_whatsThis);

   traceout("initQShortcutClass()");
   return QC_QShortcut;
}
