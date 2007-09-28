/*
 QC_QPainter.cc
 
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

#include "QC_QPainter.h"
#include "QC_QColor.h"
#include "QC_QBrush.h"

DLLLOCAL int CID_QPAINTER;
DLLLOCAL QoreClass *QC_QPainter = 0;

static void QPAINTER_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQPainter *qp;

   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT)
   {
      AbstractPrivateData *apd = p->val.object->getReferencedPrivateData(CID_QPAINTDEVICE, xsink);
      if (apd) {
	 ReferenceHolder<AbstractPrivateData> holder(apd, xsink);
	 QoreAbstractQPaintDevice *qpd = dynamic_cast<QoreAbstractQPaintDevice *>(apd);
	 assert(qpd);
	 //printd(5, "apd=%08p qpb=%08p\n", apd, qpd);
	 qp = new QoreQPainter(qpd->getQPaintDevice());
      }
      else {
	 xsink->raiseException("QPAINTER-CONSTRUCTOR-ERROR", "QPainter::constructor() does not take objects of class '%s' as an argument", p->val.object->getClass()->getName());
	 return;
      }
   }
   else
      qp = new QoreQPainter();

   self->setPrivate(CID_QPAINTER, qp);
}

static void QPAINTER_copy(class Object *self, class Object *old, class QoreQPainter *qp, ExceptionSink *xsink)
{
   xsink->raiseException("QPAINTER-COPY-ERROR", "objects of this class cannot be copied");
}

//const QBrush & background () const
static QoreNode *QPAINTER_background(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreQBrush *q_qr = new QoreQBrush(qp->getQPainter()->background());
   Object *o_qr = new Object(QC_QBrush, getProgram());
   o_qr->setPrivate(CID_QBRUSH, q_qr);
   return new QoreNode(o_qr);
}

//Qt::BGMode backgroundMode () const
static QoreNode *QPAINTER_backgroundMode(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPainter()->backgroundMode());
}

//bool begin ( QPaintDevice * device )
static QoreNode *QPAINTER_begin(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   AbstractPrivateData *apd = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QPAINTDEVICE, xsink) : 0;
   if (!apd)
   {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-BEGIN-PARAM-ERROR", "expecting a QPaintDevice object as first argument to QPainter::begin()");
      return 0;
   }

   ReferenceHolder<AbstractPrivateData> holder(apd, xsink);
   QoreAbstractQPaintDevice *device = dynamic_cast<QoreAbstractQPaintDevice *>(apd);
   assert(device);
   return new QoreNode(qp->getQPainter()->begin(device->getQPaintDevice()));
}

//QRectF boundingRect ( const QRectF & rectangle, int flags, const QString & text )
////QRectF boundingRect ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() )
//QRect boundingRect ( const QRect & rectangle, int flags, const QString & text )
//QRect boundingRect ( int x, int y, int w, int h, int flags, const QString & text )
static QoreNode *QPAINTER_boundingRect(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef) {
	  QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	  if (!rectangle)
	  {
	     if (!xsink->isException())
		xsink->raiseException("QPAINTER-BOUNDINGRECT-PARAM-ERROR", "QPainter::boundingRect() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	     return 0;
	  }
	  ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	  p = get_param(params, 1);
	  int flags = p ? p->getAsInt() : 0;
	  p = get_param(params, 2);
	  if (!p || p->type != NT_STRING) {
	     xsink->raiseException("QPAINTER-BOUNDINGRECT-PARAM-ERROR", "expecting a string as third argument to QPainter::boundingRect()");
	     return 0;
	  }
	  const char *text = p->val.String->getBuffer();
	  
	  QoreQRect *q_qr = new QoreQRect(qp->getQPainter()->boundingRect(*((QRect *)rectangle), flags, text));
	  Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
	  o_qr->setPrivate(CID_QRECT, q_qr);
	  return new QoreNode(o_qr);
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      p = get_param(params, 1);
      int flags = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      if (!p || p->type != NT_STRING) {
	 xsink->raiseException("QPAINTER-BOUNDINGRECT-PARAM-ERROR", "expecting a string as third argument to QPainter::boundingRect()");
	 return 0;
      }
      const char *text = p->val.String->getBuffer();

      QoreQRectF *q_qrf = new QoreQRectF(qp->getQPainter()->boundingRect(*((QRectF *)rectanglef), flags, text));
      Object *o_qrf = new Object(self->getClass(CID_QRECTF), getProgram());
      o_qrf->setPrivate(CID_QRECTF, q_qrf);
      return new QoreNode(o_qrf);
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int h = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int flags = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QPAINTER-BOUNDINGRECT-PARAM-ERROR", "expecting a string as sixth argument to QPainter::boundingRect()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();

   QoreQRect *q_qr = new QoreQRect(qp->getQPainter()->boundingRect(x, y, w, h, flags, text));
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//const QBrush & brush () const
static QoreNode *QPAINTER_brush(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreQBrush *q_qr = new QoreQBrush(qp->getQPainter()->brush());
   Object *o_qr = new Object(QC_QBrush, getProgram());
   o_qr->setPrivate(CID_QBRUSH, q_qr);
   return new QoreNode(o_qr);
}

//QPoint brushOrigin () const
static QoreNode *QPAINTER_brushOrigin(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qp->getQPainter()->brushOrigin());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPainterPath clipPath () const
//static QoreNode *QPAINTER_clipPath(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPainter()->clipPath());
//}

//QRegion clipRegion () const
static QoreNode *QPAINTER_clipRegion(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qr = new Object(QC_QRegion, getProgram());
   QoreQRegion *q_qr = new QoreQRegion(qp->getQPainter()->clipRegion());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//QMatrix combinedMatrix () const
static QoreNode *QPAINTER_combinedMatrix(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qm = new Object(QC_QMatrix, getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qp->getQPainter()->combinedMatrix());
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
}

//QTransform combinedTransform () const
//static QoreNode *QPAINTER_combinedTransform(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPainter()->combinedTransform());
//}

//CompositionMode compositionMode () const
static QoreNode *QPAINTER_compositionMode(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPainter()->compositionMode());
}

//QPaintDevice * device () const
//static QoreNode *QPAINTER_device(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qp->getQPainter()->device();
//}

//const QMatrix & deviceMatrix () const
static QoreNode *QPAINTER_deviceMatrix(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qm = new Object(QC_QMatrix, getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qp->getQPainter()->deviceMatrix());
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
}

//const QTransform & deviceTransform () const
//static QoreNode *QPAINTER_deviceTransform(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPainter()->deviceTransform());
//}

//void drawArc ( const QRectF & rectangle, int startAngle, int spanAngle )
//void drawArc ( const QRect & rectangle, int startAngle, int spanAngle )
//void drawArc ( int x, int y, int width, int height, int startAngle, int spanAngle )
static QoreNode *QPAINTER_drawArc(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWARC-PARAM-ERROR", "QPainter::drawArc() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 p = get_param(params, 1);
	 int startAngle = p ? p->getAsInt() : 0;
	 p = get_param(params, 2);
	 int spanAngle = p ? p->getAsInt() : 0;
	 qp->getQPainter()->drawArc(*((QRect *)rectangle), startAngle, spanAngle);
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      p = get_param(params, 1);
      int startAngle = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int spanAngle = p ? p->getAsInt() : 0;
      qp->getQPainter()->drawArc(*((QRectF *)rectanglef), startAngle, spanAngle);
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int startAngle = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int spanAngle = p ? p->getAsInt() : 0;
   qp->getQPainter()->drawArc(x, y, width, height, startAngle, spanAngle);
   return 0;
}

//void drawChord ( const QRectF & rectangle, int startAngle, int spanAngle )
//void drawChord ( const QRect & rectangle, int startAngle, int spanAngle )
//void drawChord ( int x, int y, int width, int height, int startAngle, int spanAngle )
static QoreNode *QPAINTER_drawChord(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWARC-PARAM-ERROR", "QPainter::drawChord() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 p = get_param(params, 1);
	 int startAngle = p ? p->getAsInt() : 0;
	 p = get_param(params, 2);
	 int spanAngle = p ? p->getAsInt() : 0;
	 qp->getQPainter()->drawChord(*((QRect *)rectangle), startAngle, spanAngle);
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      p = get_param(params, 1);
      int startAngle = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int spanAngle = p ? p->getAsInt() : 0;
      qp->getQPainter()->drawChord(*((QRectF *)rectanglef), startAngle, spanAngle);
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int startAngle = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int spanAngle = p ? p->getAsInt() : 0;
   qp->getQPainter()->drawChord(x, y, width, height, startAngle, spanAngle);
   return 0;
}

//void drawConvexPolygon ( const QPointF * points, int pointCount )
//void drawConvexPolygon ( const QPoint * points, int pointCount )
//void drawConvexPolygon ( const QPolygonF & polygon )
//void drawConvexPolygon ( const QPolygon & polygon )
static QoreNode *QPAINTER_drawConvexPolygon(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPolygonF *polygonf = (QoreQPolygonF *)p->val.object->getReferencedPrivateData(CID_QPOLYGONF, xsink);
      if (!polygonf) {
	 QoreQPolygon *polygon = (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink);
	 if (!polygon) {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWCONVEXPOLYGON-PARAM-ERROR", "QPainter::drawConvexPolygon() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQPolygon> polygonHolder(polygon, xsink);
	 qp->getQPainter()->drawConvexPolygon(*(static_cast<QPolygon *>(polygon)));
	 return 0;
      }
      ReferenceHolder<QoreQPolygonF> polygonHolder(polygonf, xsink);
      qp->getQPainter()->drawConvexPolygon(*(static_cast<QPolygonF *>(polygonf)));
      return 0;
   }

/*
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QPAINTER-DRAWCONVEXPOLYGON-PARAM-ERROR", "expecting QPolygon, QPolygonF, or list of QPoints or QPointFs as argument to QPainter::drawConvexPolygon()");
      return 0;
   }
*/

   xsink->raiseException("QPAINTER-DRAWCONVEXPOLYGON-PARAM-ERROR", "expecting QPolygon, QPolygonF object as argument to QPainter::drawConvexPolygon()");
   return 0;
}

//void drawEllipse ( const QRectF & rectangle )
//void drawEllipse ( const QRect & rectangle )
//void drawEllipse ( int x, int y, int width, int height )
static QoreNode *QPAINTER_drawEllipse(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWARC-PARAM-ERROR", "QPainter::drawEllipse() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 qp->getQPainter()->drawEllipse(*((QRect *)rectangle));
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      qp->getQPainter()->drawEllipse(*((QRectF *)rectanglef));
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   qp->getQPainter()->drawEllipse(x, y, width, height);
   return 0;
}

//void drawImage ( const QRectF & target, const QImage & image, const QRectF & source, Qt::ImageConversionFlags flags = Qt::AutoColor )
//void drawImage ( const QPoint & point, const QImage & image )
//void drawImage ( int x, int y, const QImage & image, int sx = 0, int sy = 0, int sw = -1, int sh = -1, Qt::ImageConversionFlags flags = Qt::AutoColor )
static QoreNode *QPAINTER_drawImage(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *target = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
      if (!target) {
	 QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
	 if (!point) {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWIMAGE-PARAM-ERROR", "expecting a QRectF object as first argument to QPainter::drawImage()");
	    return 0;
	 }
	 ReferenceHolder<QoreQPoint> pointHolder(point, xsink);
	 p = test_param(params, NT_OBJECT, 1);
	 QoreQImage *image = p ? (QoreQImage *)p->val.object->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
	 if (!image) {
	    if (!*xsink) 
	       xsink->raiseException("QPAINTER-DRAWIMAGE-PARAM-ERROR", "this version of QPainter::drawImage() expects a QImage object as second argument");
	    return 0;
	 }
	 ReferenceHolder<QoreQImage> imageHolder(image, xsink);
	 qp->getQPainter()->drawImage(*(static_cast<QPoint *>(point)), *(static_cast<QImage *>(image)));
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(target, xsink);
      p = get_param(params, 1);
      QoreQImage *image = (p && p->type == NT_OBJECT) ? (QoreQImage *)p->val.object->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
      if (!image) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTER-DRAWIMAGE-PARAM-ERROR", "expecting a QImage object as second argument to QPainter::drawImage()");
	 return 0;
      }
      ReferenceHolder<QoreQImage> imageHolder(image, xsink);
      p = get_param(params, 2);
      QoreQRectF *source = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
      if (!source) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTER-DRAWIMAGE-PARAM-ERROR", "expecting a QRectF object as third argument to QPainter::drawImage()");
	 return 0;
      }
      ReferenceHolder<QoreQRectF> sourceHolder(source, xsink);
      p = get_param(params, 3);
      Qt::ImageConversionFlags flags = !is_nothing(p) ? (Qt::ImageConversionFlags)p->getAsInt() : Qt::AutoColor;
      qp->getQPainter()->drawImage(*(static_cast<QRectF *>(target)), *(static_cast<QImage *>(image)), *(static_cast<QRectF *>(source)), flags);
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQImage *image = (p && p->type == NT_OBJECT) ? (QoreQImage *)p->val.object->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
   if (!image) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-DRAWIMAGE-PARAM-ERROR", "this version of QPainter::drawImage() expects an object derived from QImage as the third argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
   p = get_param(params, 3);
   int sx = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int sy = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int sw = !is_nothing(p) ? p->getAsInt() : -1;
   p = get_param(params, 6);
   int sh = !is_nothing(p) ? p->getAsInt() : -1;
   p = get_param(params, 7);
   Qt::ImageConversionFlags flags = !is_nothing(p) ? (Qt::ImageConversionFlags)p->getAsInt() : Qt::AutoColor;
   qp->getQPainter()->drawImage(x, y, *(static_cast<QImage *>(image)), sx, sy, sw, sh, flags);
   return 0;
}

//void drawLine ( const QLineF & line )
//void drawLine ( const QLine & line )
//void drawLine ( const QPoint & p1, const QPoint & p2 )
//void drawLine ( const QPointF & p1, const QPointF & p2 )
//void drawLine ( int x1, int y1, int x2, int y2 )
static QoreNode *QPAINTER_drawLine(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQLine *line = (QoreQLine *)p->val.object->getReferencedPrivateData(CID_QLINE, xsink);
      if (!line) {
         QoreQPoint *p1 = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
         if (!p1) {
            QoreQPointF *p1f = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
            if (!p1f) {
               QoreQLineF *linef = (QoreQLineF *)p->val.object->getReferencedPrivateData(CID_QLINEF, xsink);
               if (!linef) {
                  if (!xsink->isException())
                     xsink->raiseException("QPAINTER-DRAWLINE-PARAM-ERROR", "QPainter::drawLine() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
                  return 0;
               }
               ReferenceHolder<AbstractPrivateData> linefHolder(static_cast<AbstractPrivateData *>(linef), xsink);
               qp->getQPainter()->drawLine(*(static_cast<QLineF *>(linef)));
               return 0;
            }
            ReferenceHolder<AbstractPrivateData> p1fHolder(static_cast<AbstractPrivateData *>(p1f), xsink);
            p = test_param(params, NT_OBJECT, 1);
	    QoreQPointF *p2f = p ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
	    if (!p2f) {
	       if (!xsink->isException())
		  xsink->raiseException("QPAINTER-DRAWLINE-PARAM-ERROR", "this version of QPainter::drawLine() does not know how to handle arguments of class '%s' as passed as the second argument (expecting QPointF)", p->val.object->getClass()->getName());
	       return 0;
	    }
	    ReferenceHolder<AbstractPrivateData> p2Holder(static_cast<AbstractPrivateData *>(p2f), xsink);
	    qp->getQPainter()->drawLine(*(static_cast<QPointF *>(p1f)), *(static_cast<QPointF *>(p2f)));
	    return 0;
	 }
         ReferenceHolder<AbstractPrivateData> p1Holder(static_cast<AbstractPrivateData *>(p1), xsink);
         p = test_param(params, NT_OBJECT, 1);
	 QoreQPoint *p2 = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
	 if (!p2) {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWLINE-PARAM-ERROR", "this version of QPainter::drawLine() does not know how to handle arguments of class '%s' as passed as the second argument (expecting QPoint)", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<AbstractPrivateData> p2Holder(static_cast<AbstractPrivateData *>(p2), xsink);
	 qp->getQPainter()->drawLine(*(static_cast<QPoint *>(p1)), *(static_cast<QPoint *>(p2)));
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> lineHolder(static_cast<AbstractPrivateData *>(line), xsink);
      qp->getQPainter()->drawLine(*(static_cast<QLine *>(line)));
      return 0;
   }
   int x1 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int x2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int y2 = p ? p->getAsInt() : 0;
   qp->getQPainter()->drawLine(x1, y1, x2, y2);
   return 0;
}

//void drawLines ( const QLineF * lines, int lineCount )
//void drawLines ( const QLine * lines, int lineCount )
//void drawLines ( const QPointF * pointPairs, int lineCount )
//void drawLines ( const QPoint * pointPairs, int lineCount )
//void drawLines ( const QVector<QPointF> & pointPairs )
//void drawLines ( const QVector<QPoint> & pointPairs )
//void drawLines ( const QVector<QLineF> & lines )
//void drawLines ( const QVector<QLine> & lines )
//static QoreNode *QPAINTER_drawLines(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QVector<QLine> lines = p;
//   qp->getQPainter()->drawLines(lines);
//   return 0;
//}

//void drawPath ( const QPainterPath & path )
//static QoreNode *QPAINTER_drawPath(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPainterPath path = p;
//   qp->getQPainter()->drawPath(path);
//   return 0;
//}

//void drawPicture ( const QPointF & point, const QPicture & picture )
//void drawPicture ( const QPoint & point, const QPicture & picture )
//void drawPicture ( int x, int y, const QPicture & picture )
static QoreNode *QPAINTER_drawPicture(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         QoreQPointF *pointf = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
         if (!pointf) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-DRAWPICTURE-PARAM-ERROR", "QPainter::drawPicture() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(pointf), xsink);
         p = get_param(params, 1);
         QoreQPicture *picture = (p && p->type == NT_OBJECT) ? (QoreQPicture *)p->val.object->getReferencedPrivateData(CID_QPICTURE, xsink) : 0;
         if (!picture) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-DRAWPICTURE-PARAM-ERROR", "this version of QPainter::drawPicture() expects an object derived from QPicture as the second argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> pictureHolder(static_cast<AbstractPrivateData *>(picture), xsink);
         qp->getQPainter()->drawPicture(*(static_cast<QPointF *>(pointf)), *(static_cast<QPicture *>(picture)));
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      p = get_param(params, 1);
      QoreQPicture *picture = (p && p->type == NT_OBJECT) ? (QoreQPicture *)p->val.object->getReferencedPrivateData(CID_QPICTURE, xsink) : 0;
      if (!picture) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTER-DRAWPICTURE-PARAM-ERROR", "this version of QPainter::drawPicture() expects an object derived from QPicture as the second argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pictureHolder(static_cast<AbstractPrivateData *>(picture), xsink);
      qp->getQPainter()->drawPicture(*(static_cast<QPoint *>(point)), *(static_cast<QPicture *>(picture)));
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQPicture *picture = (p && p->type == NT_OBJECT) ? (QoreQPicture *)p->val.object->getReferencedPrivateData(CID_QPICTURE, xsink) : 0;
   if (!picture) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-DRAWPICTURE-PARAM-ERROR", "this version of QPainter::drawPicture() expects an object derived from QPicture as the third argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pictureHolder(static_cast<AbstractPrivateData *>(picture), xsink);
   qp->getQPainter()->drawPicture(x, y, *(static_cast<QPicture *>(picture)));
   return 0;
}

//void drawPie ( const QRectF & rectangle, int startAngle, int spanAngle )
//void drawPie ( const QRect & rectangle, int startAngle, int spanAngle )
//void drawPie ( int x, int y, int width, int height, int startAngle, int spanAngle )
static QoreNode *QPAINTER_drawPie(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWARC-PARAM-ERROR", "QPainter::drawPie() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 p = get_param(params, 1);
	 int startAngle = p ? p->getAsInt() : 0;
	 p = get_param(params, 2);
	 int spanAngle = p ? p->getAsInt() : 0;
	 qp->getQPainter()->drawPie(*((QRect *)rectangle), startAngle, spanAngle);
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      p = get_param(params, 1);
      int startAngle = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int spanAngle = p ? p->getAsInt() : 0;
      qp->getQPainter()->drawPie(*((QRectF *)rectanglef), startAngle, spanAngle);
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int startAngle = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int spanAngle = p ? p->getAsInt() : 0;
   qp->getQPainter()->drawPie(x, y, width, height, startAngle, spanAngle);
   return 0;
}

//void drawPoint ( const QPointF & position )
//void drawPoint ( const QPoint & position )
//void drawPoint ( int x, int y )
static QoreNode *QPAINTER_drawPoint(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *position = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!position) {
         QoreQPointF *position = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
         if (!position) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-DRAWPOINT-PARAM-ERROR", "QPainter::drawPoint() does not know how to handle arguments of class '%s' as passed as the first argument", 
p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
         qp->getQPainter()->drawPoint(*(static_cast<QPointF *>(position)));
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
      qp->getQPainter()->drawPoint(*(static_cast<QPoint *>(position)));
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   qp->getQPainter()->drawPoint(x, y);
   return 0;
}

//void drawPoints ( const QPointF * points, int pointCount )
//void drawPoints ( const QPoint * points, int pointCount )
//void drawPoints ( const QPolygonF & points )
//void drawPoints ( const QPolygon & points )
//static QoreNode *QPAINTER_drawPoints(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPolygon points = p;
//   qp->getQPainter()->drawPoints(points);
//   return 0;
//}

//void drawPolygon ( const QPointF * points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill )
//void drawPolygon ( const QPoint * points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill )
//void drawPolygon ( const QPolygonF & points, Qt::FillRule fillRule = Qt::OddEvenFill )
//void drawPolygon ( const QPolygon & points, Qt::FillRule fillRule = Qt::OddEvenFill )
static QoreNode *QPAINTER_drawPolygon(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPolygonF *pointsf = p ? (QoreQPolygonF *)p->val.object->getReferencedPrivateData(CID_QPOLYGONF, xsink) : 0;
   if (!pointsf) {
      QoreQPolygon *points = p ? (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink) : 0;
      if (!points) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTER-DRAWPOLYGON-PARAM-ERROR", "QPainter::drawPolygon() was expecting a QPolygon or QPolygonF object as first argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(points), xsink);
      p = get_param(params, 1);
      Qt::FillRule fillRule = !is_nothing(p) ? (Qt::FillRule)p->getAsInt() : Qt::OddEvenFill;
      qp->getQPainter()->drawPolygon(*(static_cast<QPolygon *>(points)), fillRule);
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(pointsf), xsink);
   p = get_param(params, 1);
   Qt::FillRule fillRule = !is_nothing(p) ? (Qt::FillRule)p->getAsInt() : Qt::OddEvenFill;
   qp->getQPainter()->drawPolygon(*(static_cast<QPolygonF *>(pointsf)), fillRule);
   return 0;
}

//void drawPolyline ( const QPointF * points, int pointCount )
//void drawPolyline ( const QPoint * points, int pointCount )
//void drawPolyline ( const QPolygonF & points )
//void drawPolyline ( const QPolygon & points )
static QoreNode *QPAINTER_drawPolyline(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPolygonF *pointsf = p ? (QoreQPolygonF *)p->val.object->getReferencedPrivateData(CID_QPOLYGONF, xsink) : 0;
   if (!pointsf) {
      QoreQPolygon *points = (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink);
      if (!points) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTER-DRAWPOLYLINE-PARAM-ERROR", "QPainter::drawPolyline() was expecting a QPolygon or QPolygonF object as first argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(points), xsink);
      qp->getQPainter()->drawPolyline(*(static_cast<QPolygon *>(points)));
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(pointsf), xsink);
   qp->getQPainter()->drawPolyline(*(static_cast<QPolygonF *>(pointsf)));
   return 0;
}

//void drawRect ( const QRectF & rectangle )
//void drawRect ( const QRect & rectangle )
//void drawRect ( int x, int y, int width, int height )
static QoreNode *QPAINTER_drawRect(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWARC-PARAM-ERROR", "QPainter::drawRect() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 qp->getQPainter()->drawRect(*((QRect *)rectangle));
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      qp->getQPainter()->drawRect(*((QRectF *)rectanglef));
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   qp->getQPainter()->drawRect(x, y, width, height);
   return 0;
}

/*
//void drawRects ( const QRectF * rectangles, int rectCount )
//void drawRects ( const QRect * rectangles, int rectCount )
//void drawRects ( const QVector<QRectF> & rectangles )
//void drawRects ( const QVector<QRect> & rectangles )
static QoreNode *QPAINTER_drawRects(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
}
*/

//void drawRoundRect ( const QRectF & r, int xRnd = 25, int yRnd = 25 )
//void drawRoundRect ( const QRect & r, int xRnd = 25, int yRnd = 25 )
//void drawRoundRect ( int x, int y, int w, int h, int xRnd = 25, int yRnd = 25 )
static QoreNode *QPAINTER_drawRoundRect(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rf = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rf)
      {
	 QoreQRect *r = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!r)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-DRAWROUNDRECT-PARAM-ERROR", "QPainter::drawRoundRect() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(r, xsink);
	 p = get_param(params, 1);
	 int xRnd = !is_nothing(p) ? p->getAsInt() : 25;
	 p = get_param(params, 2);
	 int yRnd = !is_nothing(p) ? p->getAsInt() : 25;
	 qp->getQPainter()->drawRoundRect(*((QRect *)r), xRnd, yRnd);
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rf, xsink);
      p = get_param(params, 1);
      int xRnd = !is_nothing(p) ? p->getAsInt() : 25;
      p = get_param(params, 2);
      int yRnd = !is_nothing(p) ? p->getAsInt() : 25;
      qp->getQPainter()->drawRoundRect(*((QRectF *)rf), xRnd, yRnd);
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int h = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int xRnd = !is_nothing(p) ? p->getAsInt() : 25;
   p = get_param(params, 5);
   int yRnd = !is_nothing(p) ? p->getAsInt() : 25;
   qp->getQPainter()->drawRoundRect(x, y, w, h, xRnd, yRnd);
   return 0;
}

//void drawText ( const QPointF & position, const QString & text )
//void drawText ( const QPoint & position, const QString & text )
//void drawText ( const QRectF & rectangle, int flags, const QString & text, QRectF * boundingRect = 0 )
//void drawText ( const QRect & rectangle, int flags, const QString & text, QRect * boundingRect = 0 )
//void drawText ( int x, int y, const QString & text )
//void drawText ( int x, int y, int width, int height, int flags, const QString & text, QRect * boundingRect = 0 )
static QoreNode *QPAINTER_drawText(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef) {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle) {
	    QoreQPoint *position = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
	    if (!position) {
	       QoreQPointF *positionf = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
	       if (!positionf) {
		  if (!xsink->isException())
		     xsink->raiseException("QPAINTER-DRAWTEXT-PARAM-ERROR", "QPainter::drawText() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
		  return 0;
	       }
	       ReferenceHolder<AbstractPrivateData> positionfHolder(static_cast<AbstractPrivateData *>(positionf), xsink);
	       p = get_param(params, 1);
	       QString text;
	       if (get_qstring(p, text, xsink))
		  return 0;
	       qp->getQPainter()->drawText(*(static_cast<QPointF *>(positionf)), text);
	       return 0;
	    }
	    ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
	    p = get_param(params, 1);
	    QString text;
	    if (get_qstring(p, text, xsink))
	       return 0;
	    qp->getQPainter()->drawText(*(static_cast<QPoint *>(position)), text);
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 p = get_param(params, 1);
	 int flags = p ? p->getAsInt() : 0;
	 p = get_param(params, 2);
	 QString text;
	 if (get_qstring(p, text, xsink))
	    return 0;
	 p = get_param(params, 3);
	 QoreQRect *boundingRect = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
	 ReferenceHolder<QoreQRect> holder1(boundingRect, xsink);
	 qp->getQPainter()->drawText(*((QRect *)rectangle), flags, text, (QRect *)boundingRect);
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      p = get_param(params, 1);
      int flags = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      QString text;
      if (get_qstring(p, text, xsink))
	 return 0;
      p = get_param(params, 3);
      QoreQRectF *boundingRectf = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
      ReferenceHolder<QoreQRectF> holder1(boundingRectf, xsink);
      qp->getQPainter()->drawText(*((QRectF *)rectanglef), flags, text, (QRectF *)boundingRectf);
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);

   QString text;
   if (!get_qstring(p, text, xsink, true)) {
      qp->getQPainter()->drawText(x, y, text);
      return 0;
   }

   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int flags = p ? p->getAsInt() : 0;
   p = get_param(params, 5);

   if (get_qstring(p, text, xsink))
      return 0;

   p = get_param(params, 6);
   QoreQRect *boundingRect = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   ReferenceHolder<QoreQRect> holder(boundingRect, xsink);
   qp->getQPainter()->drawText(x, y, width, height, flags, text, (QRect *)boundingRect);
   return 0;
}

//void drawText ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() )
//static QoreNode *QPAINTER_drawText(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQRectF *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
//   if (!p || !rectangle)
//   {
//      if (!xsink->isException())
//         xsink->raiseException("QPAINTER-DRAWTEXT-PARAM-ERROR", "expecting a QRectF object as first argument to QPainter::drawText()");
//      return 0;
//   }
//   ReferenceHolder<QoreQRectF> holder(rectangle, xsink);
//   p = get_param(params, 1);
//   ???  text = p;
//   p = get_param(params, 2);
//   ???  option = p;
//   qp->getQPainter()->drawText(*((QRectF *)rectangle), text, option);
//   return 0;
//}

//void drawTiledPixmap ( const QRectF & rectangle, const QPixmap & pixmap, const QPointF & position = QPointF() )
//void drawTiledPixmap ( const QRect & rectangle, const QPixmap & pixmap, const QPoint & position = QPoint() )
//void drawTiledPixmap ( int x, int y, int width, int height, const QPixmap & pixmap, int sx = 0, int sy = 0 )
static QoreNode *QPAINTER_drawTiledPixmap(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rectangle) {
         QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
         if (!rectanglef) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-DRAWTILEDPIXMAP-PARAM-ERROR", "QPainter::drawTiledPixmap() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> rectanglefHolder(static_cast<AbstractPrivateData *>(rectanglef), xsink);
         p = get_param(params, 1);
         QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
         if (!pixmap) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-DRAWTILEDPIXMAP-PARAM-ERROR", "this version of QPainter::drawTiledPixmap() expects an object derived from QPixmap as the second argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
	 p = get_param(params, 2);
	 QoreQPointF *position = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
	 if (*xsink)
	    return 0;
	 ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
	 qp->getQPainter()->drawTiledPixmap(*(static_cast<QRectF *>(rectanglef)), *(static_cast<QPixmap *>(pixmap)), *(static_cast<QPointF *>(position)));
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      p = get_param(params, 1);
      QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
      if (!pixmap) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTER-DRAWTILEDPIXMAP-PARAM-ERROR", "this version of QPainter::drawTiledPixmap() expects an object derived from QPixmap as the second argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
      p = get_param(params, 2);
      QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
      if (*xsink)
	 return 0;
      ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
      qp->getQPainter()->drawTiledPixmap(*(static_cast<QRect *>(rectangle)), *(static_cast<QPixmap *>(pixmap)), *(static_cast<QPoint *>(position)));
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-DRAWTILEDPIXMAP-PARAM-ERROR", "this version of QPainter::drawTiledPixmap() expects an object derived from QPixmap as the fifth argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 5);
   int sx = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 6);
   int sy = !is_nothing(p) ? p->getAsInt() : 0;
   qp->getQPainter()->drawTiledPixmap(x, y, width, height, *(static_cast<QPixmap *>(pixmap)), sx, sy);
   return 0;
}

//bool end ()
static QoreNode *QPAINTER_end(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPainter()->end());
}

//void eraseRect ( const QRectF & rectangle )
//void eraseRect ( const QRect & rectangle )
//void eraseRect ( int x, int y, int width, int height )
static QoreNode *QPAINTER_eraseRect(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-ERASERECT-PARAM-ERROR", "QPainter::eraseRect() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 qp->getQPainter()->eraseRect(*((QRect *)rectangle));
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      qp->getQPainter()->eraseRect(*((QRectF *)rectanglef));
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   qp->getQPainter()->eraseRect(x, y, width, height);
   return 0;
}

//void fillPath ( const QPainterPath & path, const QBrush & brush )
//static QoreNode *QPAINTER_fillPath(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPainterPath path = p;
//   p = get_param(params, 1);
//   ???  brush = p;
//   qp->getQPainter()->fillPath(path, brush);
//   return 0;
//}

//void fillRect ( const QRectF & rectangle, const QBrush & brush )
//void fillRect ( const QRect & rectangle, const QBrush & brush )
//void fillRect ( int x, int y, int width, int height, const QBrush & brush )
static QoreNode *QPAINTER_fillRect(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-FILLRECT-PARAM-ERROR", "QPainter::fillRect() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 p = get_param(params, 1);
	 QoreQBrush *brush = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
	 if (!p || !brush)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-FILLRECT-PARAM-ERROR", "expecting a QBrush object as second argument to QPainter::fillRect()");
	    return 0;
	 }
	 ReferenceHolder<QoreQBrush> holder1(brush, xsink);
	 qp->getQPainter()->fillRect(*((QRect *)rectangle), *((QBrush *)brush));
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      p = get_param(params, 1);
      QoreQBrush *brush = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
      if (!p || !brush)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTER-FILLRECT-PARAM-ERROR", "expecting a QBrush object as second argument to QPainter::fillRect()");
	 return 0;
      }
      ReferenceHolder<QoreQBrush> holder1(brush, xsink);
      qp->getQPainter()->fillRect(*((QRectF *)rectanglef), *((QBrush *)brush));
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   QoreQBrush *brush = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!p || !brush)
   {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-FILLRECT-PARAM-ERROR", "expecting a QBrush object as fifth argument to QPainter::fillRect()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> holder(brush, xsink);
   qp->getQPainter()->fillRect(x, y, width, height, *((QBrush *)brush));
   return 0;
}

//const QFont & font () const
static QoreNode *QPAINTER_font(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qf = new Object(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qp->getQPainter()->font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//QFontInfo fontInfo () const
static QoreNode *QPAINTER_fontInfo(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qfi = new Object(QC_QFontInfo, getProgram());
   QoreQFontInfo *q_qfi = new QoreQFontInfo(qp->getQPainter()->fontInfo());
   o_qfi->setPrivate(CID_QFONTINFO, q_qfi);
   return new QoreNode(o_qfi);
}

//QFontMetrics fontMetrics () const
static QoreNode *QPAINTER_fontMetrics(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qfm = new Object(QC_QFontMetrics, getProgram());
   QoreQFontMetrics *q_qfm = new QoreQFontMetrics(qp->getQPainter()->fontMetrics());
   o_qfm->setPrivate(CID_QFONTMETRICS, q_qfm);
   return new QoreNode(o_qfm);
}

//bool hasClipping () const
static QoreNode *QPAINTER_hasClipping(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPainter()->hasClipping());
}

//void initFrom ( const QWidget * widget )
static QoreNode *QPAINTER_initFrom(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !widget)
   {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-INITFROM-PARAM-ERROR", "expecting a QWidget object as first argument to QPainter::initFrom()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(widget, xsink);
   qp->getQPainter()->initFrom(widget->getQWidget());
   return 0;
}

//bool isActive () const
static QoreNode *QPAINTER_isActive(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPainter()->isActive());
}

//Qt::LayoutDirection layoutDirection () const
static QoreNode *QPAINTER_layoutDirection(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPainter()->layoutDirection());
}

//qreal opacity () const
static QoreNode *QPAINTER_opacity(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPainter()->opacity());
}

//QPaintEngine * paintEngine () const
//static QoreNode *QPAINTER_paintEngine(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qp->getQPainter()->paintEngine();
//}

//const QPen & pen () const
static QoreNode *QPAINTER_pen(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPen, getProgram());
   QoreQPen *q_qp = new QoreQPen(qp->getQPainter()->pen());
   o_qp->setPrivate(CID_QPEN, q_qp);
   return new QoreNode(o_qp);
}

//RenderHints renderHints () const
static QoreNode *QPAINTER_renderHints(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPainter()->renderHints());
}

//void resetMatrix ()
static QoreNode *QPAINTER_resetMatrix(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   qp->getQPainter()->resetMatrix();
   return 0;
}

//void resetTransform ()
static QoreNode *QPAINTER_resetTransform(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   qp->getQPainter()->resetTransform();
   return 0;
}

//void restore ()
static QoreNode *QPAINTER_restore(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   qp->getQPainter()->restore();
   return 0;
}

//void rotate ( qreal angle )
static QoreNode *QPAINTER_rotate(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float angle = p ? p->getAsFloat() : 0;
   qp->getQPainter()->rotate(angle);
   return 0;
}

//void save ()
static QoreNode *QPAINTER_save(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   qp->getQPainter()->save();
   return 0;
}

//void scale ( qreal sx, qreal sy )
static QoreNode *QPAINTER_scale(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float sx = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float sy = p ? p->getAsFloat() : 0;
   qp->getQPainter()->scale(sx, sy);
   return 0;
}

//void setBackground ( const QBrush & brush )
static QoreNode *QPAINTER_setBackground(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQBrush *brush = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!p || !brush)
   {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-SETBACKGROUND-PARAM-ERROR", "expecting a QBrush object as first argument to QPainter::setBackground()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> holder(brush, xsink);
   qp->getQPainter()->setBackground(*((QBrush *)brush));
   return 0;
}

//void setBackgroundMode ( Qt::BGMode mode )
static QoreNode *QPAINTER_setBackgroundMode(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::BGMode mode = (Qt::BGMode)(p ? p->getAsInt() : 0);
   qp->getQPainter()->setBackgroundMode(mode);
   return 0;
}

//void setBrush ( const QBrush & brush )
//void setBrush ( Qt::BrushStyle style )
static QoreNode *QPAINTER_setBrush(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQColor *color = (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (!color) {
         QoreQBrush *brush = (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink);
         if (!brush) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-SETBRUSH-PARAM-ERROR", "QPainter::setBrush() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<QoreQBrush> brushHolder(brush, xsink);
         qp->getQPainter()->setBrush(*(static_cast<QBrush *>(brush)));
         return 0;
      }
      ReferenceHolder<QoreQColor> colorHolder(color, xsink);
      qp->getQPainter()->setBrush(*(static_cast<QColor *>(color)));
      return 0;
   }
   if (!p || p->type != NT_BRUSHSTYLE) {
      Qt::GlobalColor color = (Qt::GlobalColor)(p ? p->getAsInt() : 0);
      qp->getQPainter()->setBrush(color);
      return 0;
   }

   Qt::BrushStyle style = (Qt::BrushStyle)(p ? p->getAsInt() : 0);
   qp->getQPainter()->setBrush(style);
   return 0;
}

//void setBrushOrigin ( const QPointF & position )
//void setBrushOrigin ( const QPoint & position )
//void setBrushOrigin ( int x, int y )
static QoreNode *QPAINTER_setBrushOrigin(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *position = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!position) {
         QoreQPointF *positionf = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
         if (!positionf) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-SETBRUSHORIGIN-PARAM-ERROR", "QPainter::setBrushOrigin() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(positionf), xsink);
         qp->getQPainter()->setBrushOrigin(*(static_cast<QPointF *>(positionf)));
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
      qp->getQPainter()->setBrushOrigin(*(static_cast<QPoint *>(position)));
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   qp->getQPainter()->setBrushOrigin(x, y);
   return 0;
}

//void setClipPath ( const QPainterPath & path, Qt::ClipOperation operation = Qt::ReplaceClip )
//static QoreNode *QPAINTER_setClipPath(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPainterPath path = p;
//   p = get_param(params, 1);
//   Qt::ClipOperation operation = (Qt::ClipOperation)(p ? p->getAsInt() : 0);
//   qp->getQPainter()->setClipPath(path, operation);
//   return 0;
//}

//void setClipRect ( const QRectF & rectangle, Qt::ClipOperation operation = Qt::ReplaceClip )
//void setClipRect ( int x, int y, int width, int height, Qt::ClipOperation operation = Qt::ReplaceClip )
//void setClipRect ( const QRect & rectangle, Qt::ClipOperation operation = Qt::ReplaceClip )
static QoreNode *QPAINTER_setClipRect(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {

      QoreQRectF *rectanglef = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectanglef) {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);

	 if (!rectangle) {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-SETCLIPRECT-PARAM-ERROR", "QPainter::setClipRect() cannot handle an argument of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 p = get_param(params, 1);
	 Qt::ClipOperation operation = (Qt::ClipOperation)(p ? p->getAsInt() : 0);
	 qp->getQPainter()->setClipRect(*((QRect *)rectangle), operation);
	 return 0;
      }
      ReferenceHolder<QoreQRectF> holder(rectanglef, xsink);
      p = get_param(params, 1);
      Qt::ClipOperation operation = (Qt::ClipOperation)(p ? p->getAsInt() : 0);
      qp->getQPainter()->setClipRect(*((QRectF *)rectanglef), operation);
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   Qt::ClipOperation operation = (Qt::ClipOperation)(p ? p->getAsInt() : 0);
   qp->getQPainter()->setClipRect(x, y, width, height, operation);
   return 0;
}

//void setClipRegion ( const QRegion & region, Qt::ClipOperation operation = Qt::ReplaceClip )
static QoreNode *QPAINTER_setClipRegion(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegion *region = (p && p->type == NT_OBJECT) ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!region) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-SETCLIPREGION-PARAM-ERROR", "expecting a QRegion object as first argument to QPainter::setClipRegion()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> regionHolder(static_cast<AbstractPrivateData *>(region), xsink);
   p = get_param(params, 1);
   Qt::ClipOperation operation = !is_nothing(p) ? (Qt::ClipOperation)p->getAsInt() : Qt::ReplaceClip;
   qp->getQPainter()->setClipRegion(*(static_cast<QRegion *>(region)), operation);
   return 0;
}

//void setClipping ( bool enable )
static QoreNode *QPAINTER_setClipping(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qp->getQPainter()->setClipping(enable);
   return 0;
}

//void setCompositionMode ( CompositionMode mode )
static QoreNode *QPAINTER_setCompositionMode(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPainter::CompositionMode mode = (QPainter::CompositionMode)(p ? p->getAsInt() : 0);
   qp->getQPainter()->setCompositionMode(mode);
   return 0;
}

//void setFont ( const QFont & font )
static QoreNode *QPAINTER_setFont(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!p || !font)
   {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QPainter::setFont()");
      return 0;
   }
   ReferenceHolder<QoreQFont> holder(font, xsink);
   qp->getQPainter()->setFont(*((QFont *)font));
   return 0;
}

//void setLayoutDirection ( Qt::LayoutDirection direction )
static QoreNode *QPAINTER_setLayoutDirection(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::LayoutDirection direction = (Qt::LayoutDirection)(p ? p->getAsInt() : 0);
   qp->getQPainter()->setLayoutDirection(direction);
   return 0;
}

//void setOpacity ( qreal opacity )
static QoreNode *QPAINTER_setOpacity(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float opacity = p ? p->getAsFloat() : 0;
   qp->getQPainter()->setOpacity(opacity);
   return 0;
}

//void setPen ( const QPen & pen )
//static QoreNode *QPAINTER_setPen(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPen pen = p;
//   qp->getQPainter()->setPen(pen);
//   return 0;
//}

//void setPen ( const QColor & color )
//void setPen ( Qt::PenStyle style )
static QoreNode *QPAINTER_setPen(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQColor *color = (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (!color)
      {
	 QoreQPen *pen = (QoreQPen *)p->val.object->getReferencedPrivateData(CID_QPEN, xsink);
	 if (!pen) {
	    if (!xsink->isException())
	       xsink->raiseException("QPAINTER-SETPEN-PARAM-ERROR", "expecting a QColor object as first argument to QPainter::setPen()");
	    return 0;
	 }
	 ReferenceHolder<QoreQPen> penHolder(pen, xsink);
	 qp->getQPainter()->setPen(*pen);
	 return 0;
      }
      ReferenceHolder<QoreQColor> holder(color, xsink);
      qp->getQPainter()->setPen(*((QColor *)color));
      return 0;
   }
   else if (p && p->type == NT_PENSTYLE)
      qp->getQPainter()->setPen((Qt::PenStyle)p->val.intval);
   else {  // assume it's a color value
      Qt::GlobalColor color = (Qt::GlobalColor)(p ? p->getAsInt() : 0);
      qp->getQPainter()->setPen(color);
   }
   return 0;
}

//void setRenderHint ( RenderHint hint, bool on = true )
static QoreNode *QPAINTER_setRenderHint(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPainter::RenderHint hint = (QPainter::RenderHint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool on = !is_nothing(p) ? p->getAsBool() : true;
   qp->getQPainter()->setRenderHint(hint, on);
   return 0;
}

//void setRenderHints ( RenderHints hints, bool on = true )
static QoreNode *QPAINTER_setRenderHints(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPainter::RenderHints hints = (QPainter::RenderHints)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool on = !is_nothing(p) ? p->getAsBool() : true;
   qp->getQPainter()->setRenderHints(hints, on);
   return 0;
}

//void setTransform ( const QTransform & transform, bool combine = false )
//static QoreNode *QPAINTER_setTransform(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QTransform transform = p;
//   p = get_param(params, 1);
//   bool combine = p ? p->getAsBool() : 0;
//   qp->getQPainter()->setTransform(transform, combine);
//   return 0;
//}

//void setViewTransformEnabled ( bool enable )
static QoreNode *QPAINTER_setViewTransformEnabled(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qp->getQPainter()->setViewTransformEnabled(enable);
   return 0;
}

//void setViewport ( const QRect & rectangle )
//void setViewport ( int x, int y, int width, int height )
static QoreNode *QPAINTER_setViewport(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rectangle)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTER-SETVIEWPORT-PARAM-ERROR", "expecting a QRect object as first argument to QPainter::setViewport()");
	 return 0;
      }
      ReferenceHolder<QoreQRect> holder(rectangle, xsink);
      qp->getQPainter()->setViewport(*((QRect *)rectangle));
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   qp->getQPainter()->setViewport(x, y, width, height);
   return 0;
}

//void setWindow ( const QRect & rectangle )
//void setWindow ( int x, int y, int width, int height )
static QoreNode *QPAINTER_setWindow(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rectangle)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTER-SETWINDOW-PARAM-ERROR", "expecting a QRect object as first argument to QPainter::setWindow()");
	 return 0;
      }
      ReferenceHolder<QoreQRect> holder(rectangle, xsink);
      qp->getQPainter()->setWindow(*((QRect *)rectangle));
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   qp->getQPainter()->setWindow(x, y, width, height);
   return 0;
}

//void setWorldMatrix ( const QMatrix & matrix, bool combine = false )
static QoreNode *QPAINTER_setWorldMatrix(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQMatrix *matrix = (p && p->type == NT_OBJECT) ? (QoreQMatrix *)p->val.object->getReferencedPrivateData(CID_QMATRIX, xsink) : 0;
   if (!matrix) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTER-SETWORLDMATRIX-PARAM-ERROR", "expecting a QMatrix object as first argument to QPainter::setWorldMatrix()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
   p = get_param(params, 1);
   bool combine = p ? p->getAsBool() : false;
   qp->getQPainter()->setWorldMatrix(*(static_cast<QMatrix *>(matrix)), combine);
   return 0;
}

//void setWorldMatrixEnabled ( bool enable )
static QoreNode *QPAINTER_setWorldMatrixEnabled(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qp->getQPainter()->setWorldMatrixEnabled(enable);
   return 0;
}

//void setWorldTransform ( const QTransform & matrix, bool combine = false )
//static QoreNode *QPAINTER_setWorldTransform(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QTransform matrix = p;
//   p = get_param(params, 1);
//   bool combine = p ? p->getAsBool() : 0;
//   qp->getQPainter()->setWorldTransform(matrix, combine);
//   return 0;
//}

//void shear ( qreal sh, qreal sv )
static QoreNode *QPAINTER_shear(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float sh = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float sv = p ? p->getAsFloat() : 0;
   qp->getQPainter()->shear(sh, sv);
   return 0;
}

//void strokePath ( const QPainterPath & path, const QPen & pen )
//static QoreNode *QPAINTER_strokePath(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPainterPath path = p;
//   p = get_param(params, 1);
//   ???  pen = p;
//   qp->getQPainter()->strokePath(path, pen);
//   return 0;
//}

//bool testRenderHint ( RenderHint hint ) const
static QoreNode *QPAINTER_testRenderHint(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPainter::RenderHint hint = (QPainter::RenderHint)(p ? p->getAsInt() : 0);
   return new QoreNode(qp->getQPainter()->testRenderHint(hint));
}

//const QTransform & transform () const
//static QoreNode *QPAINTER_transform(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPainter()->transform());
//}

//void translate ( const QPointF & offset )
//void translate ( const QPoint & offset )
//void translate ( qreal dx, qreal dy )
static QoreNode *QPAINTER_translate(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *offset = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!offset) {
         QoreQPointF *offsetf = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
         if (!offsetf) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTER-TRANSLATE-PARAM-ERROR", "QPainter::translate() does not know how to handle arguments of class '%s' as passed as the first argument", 
p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> offsetHolder(static_cast<AbstractPrivateData *>(offsetf), xsink);
         qp->getQPainter()->translate(*(static_cast<QPointF *>(offsetf)));
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> offsetHolder(static_cast<AbstractPrivateData *>(offset), xsink);
      qp->getQPainter()->translate(*(static_cast<QPoint *>(offset)));
      return 0;
   }
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   qp->getQPainter()->translate(dx, dy);
   return 0;
}

//bool viewTransformEnabled () const
static QoreNode *QPAINTER_viewTransformEnabled(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPainter()->viewTransformEnabled());
}

//QRect viewport () const
static QoreNode *QPAINTER_viewport(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qp->getQPainter()->viewport());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//QRect window () const
static QoreNode *QPAINTER_window(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qp->getQPainter()->window());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//const QMatrix & worldMatrix () const
static QoreNode *QPAINTER_worldMatrix(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qm = new Object(QC_QMatrix, getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qp->getQPainter()->worldMatrix());
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
}

//bool worldMatrixEnabled () const
static QoreNode *QPAINTER_worldMatrixEnabled(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPainter()->worldMatrixEnabled());
}

//const QTransform & worldTransform () const
//static QoreNode *QPAINTER_worldTransform(Object *self, QoreQPainter *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPainter()->worldTransform());
//}

class QoreClass *initQPainterClass()
{
   tracein("initQPainterClass()");
   
   QC_QPainter = new QoreClass("QPainter", QDOM_GUI);
   CID_QPAINTER = QC_QPainter->getID();
   QC_QPainter->setConstructor(QPAINTER_constructor);
   QC_QPainter->setCopy((q_copy_t)QPAINTER_copy);

   QC_QPainter->addMethod("background",                  (q_method_t)QPAINTER_background);
   QC_QPainter->addMethod("backgroundMode",              (q_method_t)QPAINTER_backgroundMode);
   QC_QPainter->addMethod("begin",                       (q_method_t)QPAINTER_begin);
   QC_QPainter->addMethod("boundingRect",                (q_method_t)QPAINTER_boundingRect);
   QC_QPainter->addMethod("brush",                       (q_method_t)QPAINTER_brush);
   QC_QPainter->addMethod("brushOrigin",                 (q_method_t)QPAINTER_brushOrigin);
   //QC_QPainter->addMethod("clipPath",                    (q_method_t)QPAINTER_clipPath);
   QC_QPainter->addMethod("clipRegion",                  (q_method_t)QPAINTER_clipRegion);
   QC_QPainter->addMethod("combinedMatrix",              (q_method_t)QPAINTER_combinedMatrix);
   //QC_QPainter->addMethod("combinedTransform",           (q_method_t)QPAINTER_combinedTransform);
   QC_QPainter->addMethod("compositionMode",             (q_method_t)QPAINTER_compositionMode);
   //QC_QPainter->addMethod("device",                      (q_method_t)QPAINTER_device);
   QC_QPainter->addMethod("deviceMatrix",                (q_method_t)QPAINTER_deviceMatrix);
   //QC_QPainter->addMethod("deviceTransform",             (q_method_t)QPAINTER_deviceTransform);
   QC_QPainter->addMethod("drawArc",                     (q_method_t)QPAINTER_drawArc);
   QC_QPainter->addMethod("drawChord",                   (q_method_t)QPAINTER_drawChord);
   QC_QPainter->addMethod("drawConvexPolygon",           (q_method_t)QPAINTER_drawConvexPolygon);
   QC_QPainter->addMethod("drawEllipse",                 (q_method_t)QPAINTER_drawEllipse);
   QC_QPainter->addMethod("drawImage",                   (q_method_t)QPAINTER_drawImage);
   QC_QPainter->addMethod("drawLine",                    (q_method_t)QPAINTER_drawLine);
   //QC_QPainter->addMethod("drawLines",                   (q_method_t)QPAINTER_drawLines);
   //QC_QPainter->addMethod("drawPath",                    (q_method_t)QPAINTER_drawPath);
   QC_QPainter->addMethod("drawPicture",                 (q_method_t)QPAINTER_drawPicture);
   QC_QPainter->addMethod("drawPie",                     (q_method_t)QPAINTER_drawPie);
   //QC_QPainter->addMethod("drawPixmap",                  (q_method_t)QPAINTER_drawPixmap);
   //QC_QPainter->addMethod("drawPoint",                   (q_method_t)QPAINTER_drawPoint);
   QC_QPainter->addMethod("drawPoint",                   (q_method_t)QPAINTER_drawPoint);
   //QC_QPainter->addMethod("drawPoints",                  (q_method_t)QPAINTER_drawPoints);
   QC_QPainter->addMethod("drawPolygon",                 (q_method_t)QPAINTER_drawPolygon);
   QC_QPainter->addMethod("drawPolyline",                (q_method_t)QPAINTER_drawPolyline);
   QC_QPainter->addMethod("drawRect",                    (q_method_t)QPAINTER_drawRect);
   //QC_QPainter->addMethod("drawRects",                   (q_method_t)QPAINTER_drawRects);
   QC_QPainter->addMethod("drawRoundRect",               (q_method_t)QPAINTER_drawRoundRect);
   QC_QPainter->addMethod("drawText",                    (q_method_t)QPAINTER_drawText);
   QC_QPainter->addMethod("drawTiledPixmap",             (q_method_t)QPAINTER_drawTiledPixmap);
   QC_QPainter->addMethod("end",                         (q_method_t)QPAINTER_end);
   QC_QPainter->addMethod("eraseRect",                   (q_method_t)QPAINTER_eraseRect);
   //QC_QPainter->addMethod("fillPath",                    (q_method_t)QPAINTER_fillPath);
   QC_QPainter->addMethod("fillRect",                    (q_method_t)QPAINTER_fillRect);
   QC_QPainter->addMethod("font",                        (q_method_t)QPAINTER_font);
   QC_QPainter->addMethod("fontInfo",                    (q_method_t)QPAINTER_fontInfo);
   QC_QPainter->addMethod("fontMetrics",                 (q_method_t)QPAINTER_fontMetrics);
   QC_QPainter->addMethod("hasClipping",                 (q_method_t)QPAINTER_hasClipping);
   QC_QPainter->addMethod("initFrom",                    (q_method_t)QPAINTER_initFrom);
   QC_QPainter->addMethod("isActive",                    (q_method_t)QPAINTER_isActive);
   QC_QPainter->addMethod("layoutDirection",             (q_method_t)QPAINTER_layoutDirection);
   QC_QPainter->addMethod("opacity",                     (q_method_t)QPAINTER_opacity);
   //QC_QPainter->addMethod("paintEngine",                 (q_method_t)QPAINTER_paintEngine);
   QC_QPainter->addMethod("pen",                         (q_method_t)QPAINTER_pen);
   QC_QPainter->addMethod("renderHints",                 (q_method_t)QPAINTER_renderHints);
   QC_QPainter->addMethod("resetMatrix",                 (q_method_t)QPAINTER_resetMatrix);
   QC_QPainter->addMethod("resetTransform",              (q_method_t)QPAINTER_resetTransform);
   QC_QPainter->addMethod("restore",                     (q_method_t)QPAINTER_restore);
   QC_QPainter->addMethod("rotate",                      (q_method_t)QPAINTER_rotate);
   QC_QPainter->addMethod("save",                        (q_method_t)QPAINTER_save);
   QC_QPainter->addMethod("scale",                       (q_method_t)QPAINTER_scale);
   QC_QPainter->addMethod("setBackground",               (q_method_t)QPAINTER_setBackground);
   QC_QPainter->addMethod("setBackgroundMode",           (q_method_t)QPAINTER_setBackgroundMode);
   QC_QPainter->addMethod("setBrush",                    (q_method_t)QPAINTER_setBrush);
   QC_QPainter->addMethod("setBrushOrigin",              (q_method_t)QPAINTER_setBrushOrigin);
   //QC_QPainter->addMethod("setClipPath",                 (q_method_t)QPAINTER_setClipPath);
   QC_QPainter->addMethod("setClipRect",                 (q_method_t)QPAINTER_setClipRect);
   QC_QPainter->addMethod("setClipRegion",               (q_method_t)QPAINTER_setClipRegion);
   QC_QPainter->addMethod("setClipping",                 (q_method_t)QPAINTER_setClipping);
   QC_QPainter->addMethod("setCompositionMode",          (q_method_t)QPAINTER_setCompositionMode);
   QC_QPainter->addMethod("setFont",                     (q_method_t)QPAINTER_setFont);
   QC_QPainter->addMethod("setLayoutDirection",          (q_method_t)QPAINTER_setLayoutDirection);
   QC_QPainter->addMethod("setOpacity",                  (q_method_t)QPAINTER_setOpacity);
   QC_QPainter->addMethod("setPen",                      (q_method_t)QPAINTER_setPen);
   QC_QPainter->addMethod("setRenderHint",               (q_method_t)QPAINTER_setRenderHint);
   QC_QPainter->addMethod("setRenderHints",              (q_method_t)QPAINTER_setRenderHints);
   //QC_QPainter->addMethod("setTransform",                (q_method_t)QPAINTER_setTransform);
   QC_QPainter->addMethod("setViewTransformEnabled",     (q_method_t)QPAINTER_setViewTransformEnabled);
   QC_QPainter->addMethod("setViewport",                 (q_method_t)QPAINTER_setViewport);
   QC_QPainter->addMethod("setWindow",                   (q_method_t)QPAINTER_setWindow);
   QC_QPainter->addMethod("setWorldMatrix",              (q_method_t)QPAINTER_setWorldMatrix);
   QC_QPainter->addMethod("setWorldMatrixEnabled",       (q_method_t)QPAINTER_setWorldMatrixEnabled);
   //QC_QPainter->addMethod("setWorldTransform",           (q_method_t)QPAINTER_setWorldTransform);
   QC_QPainter->addMethod("shear",                       (q_method_t)QPAINTER_shear);
   //QC_QPainter->addMethod("strokePath",                  (q_method_t)QPAINTER_strokePath);
   QC_QPainter->addMethod("testRenderHint",              (q_method_t)QPAINTER_testRenderHint);
   //QC_QPainter->addMethod("transform",                   (q_method_t)QPAINTER_transform);
   QC_QPainter->addMethod("translate",                   (q_method_t)QPAINTER_translate);
   QC_QPainter->addMethod("viewTransformEnabled",        (q_method_t)QPAINTER_viewTransformEnabled);
   QC_QPainter->addMethod("viewport",                    (q_method_t)QPAINTER_viewport);
   QC_QPainter->addMethod("window",                      (q_method_t)QPAINTER_window);
   QC_QPainter->addMethod("worldMatrix",                 (q_method_t)QPAINTER_worldMatrix);
   QC_QPainter->addMethod("worldMatrixEnabled",          (q_method_t)QPAINTER_worldMatrixEnabled);
   //QC_QPainter->addMethod("worldTransform",              (q_method_t)QPAINTER_worldTransform);

   traceout("initQPainterClass()");
   return QC_QPainter;
}
