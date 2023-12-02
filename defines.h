/*!
    \file       defines.h
    \brief      Defined MACRO for coral and derived namespaceses and classes
    \details    Environment and Define MACROs
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_DEFINES_H__
#define __CORAL_DEFINES_H__

#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <cstdarg>
#include <cstring>

// library namespace
#if __cplusplus < 201703
    #define extlib boost
#else
    #define extlib std
#endif

// extern "C" macro
#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

// new
#define EXPORT_C_CLASS_NEW(classname) EXPORT_C \
            classname * classname##_new(void)

#define EXPORT_C_CLASS_NEW_DEFINE(classname) \
            EXPORT_C_CLASS_NEW(classname) \
            { return new classname (); }

// repeat as much as you want. allows passing parameters to the constructor
#define EXPORT_C_CLASS_NEW_1(classname, param1) EXPORT_C \
            classname * classname##_new(param1 p1)

#define EXPORT_C_CLASS_NEW_1_DEFINE(classname, param1) \
            EXPORT_C_CLASS_NEW_1(classname, param1) \
            { return new classname (p1); }

// delete
#define EXPORT_C_CLASS_DELETE(classname) EXPORT_C \
            void classname##_delete(classname * this)

#define EXPORT_C_CLASS_DELETE_DEFINE(classname) \
            EXPORT_C_CLASS_DELETE(classname) \
            { delete this; }

// functions
#define EXPORT_C_CLASS_METHOD(classname, methodname, ret) EXPORT_C \
            ret classname##_##methodname##(classname * this)

#define EXPORT_C_CLASS_METHOD_DEFINE(classname, methodname, ret) \
            EXPORT_C_CLASS_METHOD(classname, methodname, ret) \
            { return this->##methodname##(); }

// and repeat as necessary.
#define EXPORT_C_CLASS_METHOD_1(classname, methodname, ret, param1) EXPORT_C \
            ret classname##_##methodname(classname * this, param1 p1)

#define EXPORT_C_CLASS_METHOD_1_DEFINE(classname, methodname, ret, param1) \
            EXPORT_C_CLASS_METHOD_1(classname, methodname, ret, param1) \
            { return this->##methodname##(p1); }

// Inform of a method of a class
#define CORAL_D_METHOD_INFO      std::setw(8) << std::setfill('0') << std::hex << std::right << std::uppercase \
                                << getpid() << "::" << pthread_self() << "::" << std::dec << std::left << std::nouppercase \
                                << typeid(*this).name() << "::" << __FUNCTION__
#define CORAL_D_METHOD_START     "METHOD:START:" << CORAL_D_METHOD_INFO
#define CORAL_D_METHOD_END       "METHOD:END--:" << CORAL_D_METHOD_INFO
// Inform of a function
#define CORAL_D_FUNCTION_INFO    std::setw(8) << std::setfill('0') << std::hex << std::right << std::uppercase \
                                << getpid() << "::" << pthread_self() << "::" << std::dec << std::left << std::nouppercase << __FUNCTION__
// file name & the line number
#define CORAL_D_FILE_INFO        __FILE__ << "::" << __LINE__

// max length when output string to std stream 1024
#define CORAL_D_STR_LEN_MAX      0x400

// print inform of a method of a class, using MACROs below when debug mode.
#ifdef _DEBUG
    #define CORAL_D_CLASS_MEMBER_FUNC_INFO    std::cout << CORAL_D_METHOD_INFO  << std::endl;
    #define CORAL_D_CLASS_MEMBER_FILE_INFO    std::cout << CORAL_D_FILE_INFO    << std::endl;
    #define CORAL_D_CLASS_MEMBER_FUNC_START   std::cout << CORAL_D_METHOD_START << std::endl;
    #define CORAL_D_CLASS_MEMBER_FUNC_END     std::cout << CORAL_D_METHOD_END   << std::endl;
    void CORAL_D_TRACE(const char* format, ...);
#else
    #define CORAL_D_CLASS_MEMBER_FUNC_INFO
    #define CORAL_D_CLASS_MEMBER_FILE_INFO
    #define CORAL_D_CLASS_MEMBER_FUNC_START
    #define CORAL_D_CLASS_MEMBER_FUNC_END
    void CORAL_D_TRACE( const char* /* format */, ...);
#endif


//-----------------------------------------------------------------------------
// STRING Messages Definition: usage CORAL_D_STRMSG(EN, STR, 0000001) is CORAL_D_EN_STR_000001
#define CORAL_D_STRMSG(LOC, TYPE, NO) CORAL_D_ ## LOC ## _ ## TYPE ## _ ## NO

//-----------------------------------------------------------------------------
// exception, Error Strings
//-----------------------------------------------------------------------------
// process
#define CORAL_D_EN_ERR_000001   "Process has been halted because an error of the process was caught by the exception module."
#define CORAL_D_EN_ERR_000010   "Unknown exception caught."
// domain error
#define CORAL_D_EN_ERR_001001   "Wrong input parameters."
#define CORAL_D_EN_ERR_001002   "Wrong input dataset."
#define CORAL_D_EN_ERR_001003   "Unknown data group code."
#define CORAL_D_EN_ERR_001004   "Unknown command."
#define CORAL_D_EN_ERR_001005   "Wrong data format."
#define CORAL_D_EN_ERR_001006   "The file format is invalid."
// file error
#define CORAL_D_EN_ERR_002001   "IO exception has been occured when the file opening."   //
#define CORAL_D_EN_ERR_002002   "File format error."    //
// data container
#define CORAL_D_EN_ERR_003001   "The data container is empty."
// algorithm

//-----------------------------------------------------------------------------
// String
//-----------------------------------------------------------------------------
#define CORAL_D_EN_STR_000001   "Process:%s, Message:%s, ElapsedTime:%.6lfs"
#define CORAL_D_EN_STR_000004   "DONE"
//-----------------------------------------------------------------------------

#endif // __CORAL_DEFINES_H__
