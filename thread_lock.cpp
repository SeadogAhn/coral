/*!
    \file       thread_lock.cpp
    \brief      Thread management
    \details    Thread locker and Mutex attribute
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "thread_lock.h"

coral::mutex_attribute* coral::mutex_attribute::sOnly_ = 0;

coral::mutex_attribute& coral::mutex_attribute::gOnly ()
{
  if (!sOnly_)
    sOnly_ = new coral::mutex_attribute ();
  return *sOnly_;
}

coral::mutex_attribute::mutex_attribute ()
{
  pthread_mutexattr_init (&attr_);
  pthread_mutexattr_settype (value(), PTHREAD_MUTEX_RECURSIVE);
}


coral::mutex_attribute::~mutex_attribute ()
{ pthread_mutexattr_destroy (&attr_);}
