/*!
    \file       Thread.h
    \brief      UNIX POSIX Thread
    \details    thread class using pthread
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_THREAD_H__
#define __CORAL_THREAD_H__

#include <pthread.h>
#include <time.h>

//! Core Library for Applications and Libraries
namespace coral {
    //! Thread manager class
    class Thread {
    public:
        Thread  () : threadid_ (0) {}
        virtual ~Thread () {if (threadid_ != 0) stop();}

        int threadid () const { return (int)threadid_;}

        //! Start the thread running
        bool start ()
        {
            int status;
            status = pthread_create (&threadid_, 0, thread_, this);
            return (status == 0);
        }
        //! Stop the thread. Ungraceful and may result in locking/resource problems.
        bool stop  ()
        {
            pthread_cancel (threadid_);
            return true;
        }
        //! Wait for thread to complete
        bool wait (unsigned int seconds = 0)
        {
            // Time is ignored here
            char* rc;
            pthread_join (threadid_, (void**) &rc);
            return true;
        }
        //! Sleep for the specified amount of time.
        void sleep (unsigned int msec)
        {
            struct timespec spec, rspec;
            spec.tv_sec = msec / 1000;
            spec.tv_nsec = (msec % 1000) * 1000000;
            nanosleep (&spec, &rspec);
        }

    protected:
        pthread_t threadid_;

        //! Thread function
        static void* thread_ (void* obj)
        {
            // Call the overriden thread function
            Thread* t = reinterpret_cast<Thread*>(obj);
            t->thread ();
            return 0;
        }

        //! Thread function, Override this in derived classes.
        virtual void thread () {}

    }; // end Thread class
} // end coral namespace


#endif // __CORAL_THREAD_H__
