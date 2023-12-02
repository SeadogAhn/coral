/*!
    \file       thread_lock.h
    \brief      Thread mutex management
    \details    Thread locker and Mutex attribute
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_THREADLOCK_H__
#define __CORAL_THREADLOCK_H__

#include <pthread.h>

//! Core Library for Applications and Libraries
namespace coral {
    //! mutex attribute
    /*!
        Our version of thread_lock resuses the same pthread_mutexattr_t object
        each time. This object initializes it. The value() method returns
        a pointer that you can pass to pthread_mutex_init
    */
    class mutex_attribute
    {
    public:
        static mutex_attribute& gOnly ();
        pthread_mutexattr_t* value () { return &attr_;}

    private:
        static mutex_attribute* sOnly_;
        pthread_mutexattr_t attr_;

        mutex_attribute ();
        ~mutex_attribute ();
    }; // end mutex_attribute class

    //! thread_lock is to lock thread
    class thread_lock
    {
    public:
        thread_lock  () { pthread_mutex_init (&lock_, mutex_attribute::gOnly().value()); }
        ~thread_lock () { pthread_mutex_destroy (&lock_);}
        thread_lock (const thread_lock&) { pthread_mutex_init (&lock_, mutex_attribute::gOnly().value()); }
        thread_lock& operator= (const thread_lock&) { return *this;}

        // Get the lock
        bool lock () const { return pthread_mutex_lock (&lock_) == 0;}

        // Release the lock
        bool unlock () const { return pthread_mutex_unlock (&lock_) == 0;}

    private:
        mutable pthread_mutex_t lock_;
    }; // end thread_lock class

    //! thread_locker is a generic version of RAII (resource acquisition is initialization).
    /*!
        1) Create a global lock that you will use for your resource
             ex:  thread_lock coutLock;
        2) Use an instance of apLocker to manage this resource
             ex:
             {
               thread_locker lock(coutLock);
               std::cout << "Locked access to std::cout" << std::endl;
             }
    */
    class thread_locker
    {
    public:
        thread_locker (thread_lock& lock) : lock_ (lock)  { lock_.lock();}
        ~thread_locker () { lock_.unlock();}

    private:
        thread_lock& lock_;

        // Prohibit copy and assignment
        thread_locker            (const thread_locker& src);
        thread_locker& operator= (const thread_locker& src);
    }; // end thread_locker class

} // end coral namespace

#endif // __CORAL_THREADLOCK_H__
