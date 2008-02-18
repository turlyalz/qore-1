/*
  QoreThreadLock.h

  Qore Programming Language

  Copyright (C) 2003 - 2008 David Nichols, all rights reserved

  Abstract class for objects that need to be able to lock themselves

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

#ifndef _QORE_QORETHREADLOCK_H

#define _QORE_QORETHREADLOCK_H

#include <pthread.h>
#include <assert.h>

//! provides a mutually-exclusive thread lock
/** This class is just a simple wrapper for pthread_mutex_t.  It does not provide any special
    logic for checking for correct usage, etc.  The base class for the intelligent thread
    locking primitive management in Qore, see AbstractSmartLock
    @see AbstractSmartLock
 */
class QoreThreadLock {
      friend class QoreCondition;

   private:
      //! the actual locking primitive wrapped in this class
      pthread_mutex_t ptm_lock;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreThreadLock(const QoreThreadLock&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreThreadLock& operator=(const QoreThreadLock&);

   public:
      //! creates the lock
      DLLEXPORT QoreThreadLock()
      {
	 pthread_mutex_init(&ptm_lock, NULL);
      }

      //! destroys the lock
      DLLEXPORT ~QoreThreadLock()
      {
	 pthread_mutex_destroy(&ptm_lock);
      }

      //! grabs the lock (assumes that the lock is unlocked)
      /** no error checking happens here; if you grab the lock twice it will deadlock
       */
      DLLEXPORT void lock()
      {
	 pthread_mutex_lock(&ptm_lock);
      }
      
      //! releases the lock (assumes that the lock is locked)
      /** no error checking is implemented here
       */
      DLLEXPORT void unlock()
      {
	 pthread_mutex_unlock(&ptm_lock);
      }
      
      //! attempts to acquire the mutex and returns the status immediately; does not block
      /**
	 @return 0 if the lock was acquired, a non-zero error number if the lock was not acquired
       */
      DLLEXPORT int trylock()
      {
	 return pthread_mutex_trylock(&ptm_lock);
      }
};

//! provides a safe and exception-safe way to hold locks in Qore, only to be used on the stack, cannot be dynamically allocated
/** ensures that locks are released by locking the lock when the
    object is created and releasing it when the object is destroyed
    for a similar object that allows for unlocking the lock earlier 
    than the object's destruction, see SafeLocker
    @see SafeLocker
*/
class AutoLocker {
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AutoLocker(const AutoLocker&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AutoLocker& operator=(const AutoLocker&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t);

      //! the pointer to the lock that will be managed
      QoreThreadLock *lck;

   public:
      //! creates the object and grabs the lock
      DLLEXPORT AutoLocker(QoreThreadLock *l)
      {
	 lck = l;
	 lck->lock();
      }

      //! destroys the object and grabs the lock
      DLLEXPORT ~AutoLocker()
      {
	 lck->unlock();
      }
};

//! provides an exception-safe way to manage locks in Qore, only to be used on the stack, cannot be dynamically allocated
/** Ensures that locks are released by locking the lock when the
    object is created and releasing it when the object is destroyed.
    Also allows the lock to be released before the object's destruction
    at the expense of one extra byte on the stack compared to the AutoLocker class.
    @see AutoLocker
*/
class SafeLocker
{
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SafeLocker(const SafeLocker&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SafeLocker& operator=(const SafeLocker&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t);
      
   private:
      //! the pointer to the lock that will be managed      
      QoreThreadLock *lck;

      //! flag indicating if the lock is held or not
      bool locked;
      
   public:
      //! creates the object and grabs the lock
      DLLEXPORT SafeLocker(QoreThreadLock *l) : lck(l)
      {
	 lck->lock();
	 locked = true;
      }

      //! destroys the object and unlocks the lock if it's held
      DLLEXPORT ~SafeLocker()
      {
	 if (locked)
	    lck->unlock();
      }

      //! locks the object and updates the locked flag, assumes that the lock is not already held
      DLLEXPORT void lock()
      {
	 assert(!locked);
	 lck->lock();
	 locked = true;
      }

      //! unlocks the object and updates the locked flag, assumes that the lock is held
      DLLEXPORT void unlock()
      {
	 assert(locked);
	 locked = false;
	 lck->unlock();
      }
};

#endif // _QORE_QORETHREADLOCK_H
