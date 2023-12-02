/*!
    \file       log_manager.h
    \brief      output log to the file stream
    \details    log write to the log file, log file name is defined gv_app_name
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_LOGMGR_H__
#define __CORAL_LOGMGR_H__

#include <string>

#include "variables.h"
#include "utility.h"
#include "thread_lock.h"

//! Core Library for Applications and Libraries
namespace coral{
    enum LOG_LEVEL {
        LOG_LEVEL_NONE
        , LOG_LEVEL_DEBUG
        , LOG_LEVEL_INFORM
        , LOG_LEVEL_WARNING
        , LOG_LEVEL_ERROR
    };
    const std::string LOG_DESC[] {
        "DEBUG"
        , "INFORM"
        , "WARNING"
        , "ERROR"
    };
    //! Program Log 생성/기록
    /*!
        프로그램에 해당하는 파일에 로그 기록
    */
    class log_manager{
    public:
        log_manager();
        ~log_manager();
        //! static function - log 기록
        /*!
            \param pgmname program name
            \param msg 기록할 내용
        */
        static void write(const std::string& pgmname, const std::string& msg);

        //! static function - log 기록
        /*!
            \param os ostream
            \param msg 기록할 내용
            \param log_level
        */
        static void write(std::ostream& os, const std::string& msg, coral::LOG_LEVEL log_level = LOG_LEVEL_INFORM);
        //! default log level
        static int default_log_level;

    private:
        //! write msg to file
        /*!
            \param file_name a logfile name
            \param msg 기록할 내용
        */
        static void output_to_file(const std::string& file_name, const std::string& msg);

        log_manager(const log_manager &);
        log_manager& operator=(const log_manager &);
    };
}
#endif //__CORAL_LOGMGR_H__
