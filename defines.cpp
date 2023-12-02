/*!
    \file       defines.cpp
    \brief      CORAL Main header
    \details    MACRO & Functions
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "defines.h"
#include "utility.h"
#include "thread_lock.h"

extern std::string gv_app_name;

void CORAL_D_TRACE(const char* format, ...)
{
#ifdef _DEBUG
    coral::thread_lock lock;
    coral::thread_locker locker(lock);
    static char char_tmp[CORAL_D_STR_LEN_MAX];
    memset(char_tmp, 0x00, CORAL_D_STR_LEN_MAX);
    va_list ap;
    va_start(ap, format);
    vsnprintf(char_tmp, CORAL_D_STR_LEN_MAX, format, ap);
    va_end(ap);
    std::cout << char_tmp;
#endif
}
