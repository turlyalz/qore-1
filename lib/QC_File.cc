/*
  QC_File.cc

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
#include <qore/intern/QC_File.h>

int CID_FILE;

static void FILE_system_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   //printd(5, "FILE_constructor() self=%08p, params=%08p\n", self, params);
   self->setPrivate(CID_FILE, new File(QCS_DEFAULT));
}

static void FILE_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   // get character set name if available
   const QoreEncoding *cs;
   QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
   {
      cs = QEM.findCreate(p0);
      //printd(0, "FILE_constructor() str=%s, cs=%08p\n", p0->getBuffer(), cs);
   }
   else
      cs = QCS_DEFAULT;

   self->setPrivate(CID_FILE, new File(cs));
}

static void FILE_copy(class QoreObject *self, class QoreObject *old, class File *f, class ExceptionSink *xsink)
{
   self->setPrivate(CID_FILE, new File(f->getEncoding()));
}

// open(filename, [flags, mode, charset])
static class AbstractQoreNode *FILE_open(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("FILE-OPEN-PARAMETER-ERROR", "expecting string filename as first argument of File::open()");
      return NULL;
   }

   int flags, mode;
   AbstractQoreNode *p = get_param(params, 1);
   if (!is_nothing(p))
      flags = p->getAsInt();
   else
      flags = O_RDONLY;

   p = get_param(params, 2);
   if (!is_nothing(p))
      mode = p->getAsInt();
   else
      mode = 0666;

   QoreStringNode *pstr = test_string_param(params, 3);
   const QoreEncoding *charset;
   if (pstr)
      charset = QEM.findCreate(pstr);
   else
      charset = QCS_DEFAULT;

   return new QoreBigIntNode(f->open(p0->getBuffer(), flags, mode, charset));
}

// open2(filename, [flags, mode, charset])
// thrown an exception if there is an error
static class AbstractQoreNode *FILE_open2(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   int flags, mode;
   const QoreEncoding *charset;
   p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("FILE-OPEN2-PARAMETER-ERROR", "expecting string filename as first argument of File::open2()");
      return NULL;
   }
   
   AbstractQoreNode *p = get_param(params, 1);
   if (!is_nothing(p))
      flags = p->getAsInt();
   else
      flags = O_RDONLY;

   p = get_param(params, 2);
   if (!is_nothing(p))
      mode = p->getAsInt();
   else
      mode = 0666;

   QoreStringNode *pstr = test_string_param(params, 3);
   if (p)
      charset = QEM.findCreate(pstr);
   else
      charset = QCS_DEFAULT;
   
   f->open2(xsink, p0->getBuffer(), flags, mode, charset);
   return 0;
}

static class AbstractQoreNode *FILE_close(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(f->close());
}

static class AbstractQoreNode *FILE_sync(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(f->sync());
}

static class AbstractQoreNode *FILE_read(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int size;
   AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      size = p0->getAsInt();
   else
      size = 0;

   if (!size)
   {
      xsink->raiseException("FILE-READ-PARAMETER-ERROR", "expecting size as first parameter of File::read()");
      return NULL;
   }

   return f->read(size, xsink);
}

static class AbstractQoreNode *FILE_readu1(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned char c;
   if (f->readu1(&c, xsink))
      return NULL;
   return new QoreBigIntNode(c);
}

static class AbstractQoreNode *FILE_readu2(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned short s;
   if (f->readu2(&s, xsink))
      return NULL;
   return new QoreBigIntNode(s);
}

static class AbstractQoreNode *FILE_readu4(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned int i;
   if (f->readu4(&i, xsink))
      return NULL;
   
   return new QoreBigIntNode(i);
}

static class AbstractQoreNode *FILE_readu2LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned short s;
   if (f->readu2LSB(&s, xsink))
      return NULL;
   
   return new QoreBigIntNode(s);
}

static class AbstractQoreNode *FILE_readu4LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned int i;
   if (f->readu4LSB(&i, xsink))
      return NULL;
   
   return new QoreBigIntNode(i);
}

static class AbstractQoreNode *FILE_readi1(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   char c;
   if (f->readi1(&c, xsink))
      return NULL;
   return new QoreBigIntNode(c);
}

static class AbstractQoreNode *FILE_readi2(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   short s;
   if (f->readi2(&s, xsink))
      return NULL;
   return new QoreBigIntNode(s);
}

static class AbstractQoreNode *FILE_readi4(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int i;
   if (f->readi4(&i, xsink))
      return NULL;

   return new QoreBigIntNode(i);
}

static class AbstractQoreNode *FILE_readi8(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 i;
   if (f->readi8(&i, xsink))
      return NULL;
   
   return new QoreBigIntNode(i);
}

static class AbstractQoreNode *FILE_readi2LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   short s;
   if (f->readi2LSB(&s, xsink))
      return NULL;

   return new QoreBigIntNode(s);
}

static class AbstractQoreNode *FILE_readi4LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int i;
   if (f->readi4LSB(&i, xsink))
      return NULL;

   return new QoreBigIntNode(i);
}

static class AbstractQoreNode *FILE_readi8LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 i;
   if (f->readi8LSB(&i, xsink))
      return NULL;
   
   return new QoreBigIntNode(i);
}

static class AbstractQoreNode *FILE_readBinary(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);

   int size = p0 ? p0->getAsInt() : 0;
   if (!size)
   {
      xsink->raiseException("FILE-READ-BINARY-PARAMETER-ERROR", "expecting size as first parameter of File::readBinary()");
      return NULL;
   }

   return f->readBinary(size, xsink);
}

static class AbstractQoreNode *FILE_write(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_STRING && p0->type != NT_BINARY))
   {
      xsink->raiseException("FILE-WRITE-PARAMETER-ERROR", "expecting string or binary object to write as first parameter of File::write()");
      return NULL;
   }

   int rc;
   if (p0->type == NT_STRING)
      rc = f->write(reinterpret_cast<const QoreStringNode *>(p0), xsink);
   else
      rc = f->write(reinterpret_cast<const BinaryNode *>(p0), xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_writei1(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   char c;
   if (!p0)
      c = 0;
   else
      c = p0->getAsInt();

   int rc = f->writei1(c, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_writei2(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   int rc = f->writei2(s, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_writei4(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   int rc = f->writei4(i, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_writei8(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   int64 i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsBigInt();
   
   int rc = f->writei8(i, xsink);
   if (xsink->isEvent())
      return NULL;
   
   return new QoreBigIntNode(rc);
}
static class AbstractQoreNode *FILE_writei2LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   int rc = f->writei2LSB(s, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_writei4LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   int rc = f->writei4LSB(i, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_writei8LSB(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   int64 i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsBigInt();
   
   int rc = f->writei8LSB(i, xsink);
   if (xsink->isEvent())
      return NULL;
   
   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_printf(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   TempQoreStringNode str(q_sprintf(params, 0, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_vprintf(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   TempQoreStringNode str(q_vsprintf(params, 0, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_f_printf(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   TempQoreStringNode str(q_sprintf(params, 1, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_f_vprintf(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   TempQoreStringNode str(q_vsprintf(params, 1, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FILE_readLine(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return f->readLine(xsink);
}

static class AbstractQoreNode *FILE_setCharset(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreEncoding *charset;
   QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      charset = QEM.findCreate(p0);
   else
      charset = QCS_DEFAULT;

   f->setEncoding(charset);
   return NULL;
}

static class AbstractQoreNode *FILE_getCharset(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(f->getEncoding()->getCode());
}

static class AbstractQoreNode *FILE_setPos(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int pos;
   AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      pos = p0->getAsInt();
   else
      pos = 0;

   return new QoreBigIntNode(f->setPos(pos));
}

/*
static class AbstractQoreNode *FILE_setPosFromEnd(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   //f->open();
   return NULL;
}

static class AbstractQoreNode *FILE_setPosFromCurrent(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   //f->open();
   return NULL;
}
*/

static class AbstractQoreNode *FILE_getPos(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(f->getPos());
}

static class AbstractQoreNode *FILE_getchar(class QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return f->getchar();
}

class QoreClass *initFileClass()
{
   tracein("initFileClass()");

   class QoreClass *QC_FILE = new QoreClass("File", QDOM_FILESYSTEM);
   CID_FILE = QC_FILE->getID();
   QC_FILE->setSystemConstructor(FILE_system_constructor);
   QC_FILE->setConstructor(FILE_constructor);
   QC_FILE->setCopy((q_copy_t)FILE_copy);
   QC_FILE->addMethod("open",              (q_method_t)FILE_open);
   QC_FILE->addMethod("open2",             (q_method_t)FILE_open2);
   QC_FILE->addMethod("close",             (q_method_t)FILE_close);
   QC_FILE->addMethod("sync",              (q_method_t)FILE_sync);
   QC_FILE->addMethod("read",              (q_method_t)FILE_read);
   QC_FILE->addMethod("readu1",            (q_method_t)FILE_readu1);
   QC_FILE->addMethod("readu2",            (q_method_t)FILE_readu2);
   QC_FILE->addMethod("readu4",            (q_method_t)FILE_readu4);
   QC_FILE->addMethod("readu2LSB",         (q_method_t)FILE_readu2LSB);
   QC_FILE->addMethod("readu4LSB",         (q_method_t)FILE_readu4LSB);
   QC_FILE->addMethod("readi1",            (q_method_t)FILE_readi1);
   QC_FILE->addMethod("readi2",            (q_method_t)FILE_readi2);
   QC_FILE->addMethod("readi4",            (q_method_t)FILE_readi4);
   QC_FILE->addMethod("readi8",            (q_method_t)FILE_readi8);
   QC_FILE->addMethod("readi2LSB",         (q_method_t)FILE_readi2LSB);
   QC_FILE->addMethod("readi4LSB",         (q_method_t)FILE_readi4LSB);
   QC_FILE->addMethod("readi8LSB",         (q_method_t)FILE_readi8LSB);
   QC_FILE->addMethod("readBinary",        (q_method_t)FILE_readBinary);
   QC_FILE->addMethod("write",             (q_method_t)FILE_write);
   QC_FILE->addMethod("writei1",           (q_method_t)FILE_writei1);
   QC_FILE->addMethod("writei2",           (q_method_t)FILE_writei2);
   QC_FILE->addMethod("writei4",           (q_method_t)FILE_writei4);
   QC_FILE->addMethod("writei8",           (q_method_t)FILE_writei8);
   QC_FILE->addMethod("writei2LSB",        (q_method_t)FILE_writei2LSB);
   QC_FILE->addMethod("writei4LSB",        (q_method_t)FILE_writei4LSB);
   QC_FILE->addMethod("writei8LSB",        (q_method_t)FILE_writei8LSB);
   QC_FILE->addMethod("readLine",          (q_method_t)FILE_readLine);
   QC_FILE->addMethod("setCharset",        (q_method_t)FILE_setCharset);
   QC_FILE->addMethod("getCharset",        (q_method_t)FILE_getCharset);
   QC_FILE->addMethod("setPos",            (q_method_t)FILE_setPos);
   //QC_FILE->addMethod("setPosFromEnd",     (q_method_t)FILE_setPosFromEnd);
   //QC_FILE->addMethod("setPosFromCurrent", (q_method_t)FILE_setPosFromCurrent);
   QC_FILE->addMethod("getPos",            (q_method_t)FILE_getPos);
   QC_FILE->addMethod("getchar",           (q_method_t)FILE_getchar);
   QC_FILE->addMethod("printf",            (q_method_t)FILE_printf);
   QC_FILE->addMethod("vprintf",           (q_method_t)FILE_vprintf);
   QC_FILE->addMethod("f_printf",          (q_method_t)FILE_f_printf);
   QC_FILE->addMethod("f_vprintf",         (q_method_t)FILE_f_vprintf);
   traceout("initFileClass()");
   return QC_FILE;
}
