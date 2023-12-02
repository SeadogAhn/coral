/*!
    \file       Coral.h
    \brief      CORAL header
    \details    include heders
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_H__
#define __CORAL_H__

// CORAL exceptions
#include "exception.h"
// CORAL types declaration
#include "types.h"
// CORAL utility functions
#include "utility.h"
// CORAL configuration
#include "config.h"
// Alarm controller
#include "alarm_controller.h"
// Log manager
#include "log_manager.h"
// File Object
#include "file_object.h"
// Thread
#include "thread.h"
#include "thread_lock.h"
// need c++11 higher
#ifdef __GNU_MODERN_CPP_THREAD_SUPPORT__
// Thread pool
#include "thread_pool.h"

#endif // __GNU_MODERN_CPP_THREAD_SUPPORT__

#ifndef _BIGDATA    // not bigdata on cloud portal
// oracle database manipulator
#include "ora_dbm.h"
// network interface
#include "net_interface.h"
#include "net_server.h"
#include "net_client.h"
// file trans
#include "file_trans.h"

// need II_API
#ifdef __VECTOR_DB_SUPPORT__
// vector database
#include "vec_dbm.h"

#endif // __VECTOR_DB_SUPPORT__

#endif //_BIGDATA

#endif //__CORAL_H__
