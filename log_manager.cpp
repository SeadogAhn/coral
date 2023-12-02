/*!
    \file       log_manager.cpp
    \brief      output log to the file stream
    \details    Writing a log to the log file, the log file name is defined pgmname
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "log_manager.h"

using namespace std;

int coral::log_manager::default_log_level = coral::LOG_LEVEL::LOG_LEVEL_NONE;

coral::log_manager::log_manager()
{
    CORAL_D_CLASS_MEMBER_FUNC_START;
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

coral::log_manager::~log_manager()
{
    CORAL_D_CLASS_MEMBER_FUNC_START;
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::log_manager::write(const string& pgmname, const string& msg)
{
    coral::thread_lock lock;
    coral::thread_locker locker(lock);

    // 입력된 pgmname이 full path로 실행된 프로그램의 경우 파일이름만 취한다.
    string file_name = get_file_name_from_full_path(pgmname);
    string data_time = datetime_now();
    string log_file_name = gv_dir_log + file_name + ".log";
    string message = data_time  + "," + msg + "\n";

    output_to_file(log_file_name, message);
}

void coral::log_manager::write(std::ostream& os, const std::string& msg, coral::LOG_LEVEL log_level)
{
    coral::thread_lock lock;
    coral::thread_locker locker(lock);
    if (log_level >= default_log_level) {
        os << LOG_DESC[log_level] << ':' << msg;
    }
}

void coral::log_manager::output_to_file(const string& file_name, const string& msg)
{
    ofstream ofs(file_name.c_str(), ios::out|ios::app);
    if(ofs.fail()){
        cerr << file_name << "couldn't open file!!" << endl;
        throw stream_error("log_manager::write - " + file_name);
    }

    ofs << msg;

    ofs.clear();
    ofs.close();
}
