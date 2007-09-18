/*
 QC_QSpinBox.h
 
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

#ifndef _QORE_QT_QC_QSPINBOX_H

#define _QORE_QT_QC_QSPINBOX_H

#include <QSpinBox>
#include "QoreAbstractQAbstractSpinBox.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QSPINBOX;
DLLLOCAL extern class QoreClass *QC_QSpinBox;

DLLLOCAL class QoreClass *initQSpinBoxClass(QoreClass *);

class myQSpinBox : public QSpinBox, public QoreQWidgetExtension
{
#define QOREQTYPE QSpinBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQSpinBox(Object *obj, QWidget* parent = 0) : QSpinBox(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQSpinBox : public QoreAbstractQAbstractSpinBox
{
   public:
      QPointer<myQSpinBox> qobj;

      DLLLOCAL QoreQSpinBox(Object *obj, QWidget* parent = 0) : qobj(new myQSpinBox(obj, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QWidget *getQWidget() const
      {
         return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractSpinBox *getQAbstractSpinBox() const
      {
         return static_cast<QAbstractSpinBox *>(&(*qobj));
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

#endif // _QORE_QT_QC_QSPINBOX_H
