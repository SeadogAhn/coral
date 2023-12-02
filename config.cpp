/*!
    \file       config.cpp
    \brief      system config value define source
    \details    config value initialize from ./config/config.txt into map container, using the singleton design pattern
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/types.h>

#include "config.h"

using namespace std;

coral::thread_lock coral::config::lock_;
coral::config* coral::config::config_ = NULL;

coral::config* coral::config::instance()
{
    if (config_ == NULL) {
        coral::thread_locker locker(lock_);
        if (config_ == NULL) {
            config_ = new config();
        }
    }
    return config_;
}

bool coral::config::init(const std::string& file_name)
{
    if (file_name.empty()) {
        file_name_ = coral::gv_dir_config + "config_coral.txt";
    }
    else {
        file_name_ = coral::gv_dir_config + file_name;
    }

    std::ifstream ifs(file_name_.c_str(), ios::in);
    if(ifs.fail()){
        std::cerr << "Can't open file " << file_name_ << '\n';
        return false;
    }

    if(!read(ifs)) return false;

    return true;
}

bool coral::config::read(std::ifstream& ifs)
{
    std::string one = "";

    while(!ifs.eof()){
        std::getline(ifs, one);
        if(one[0] == '#' || one.empty() || one == "") {
            continue;
        }
        else{
            size_t idx = one.find('=');
            if(idx == std::string::npos){
#ifdef _DEBUG
                std::cout << "Line = " << one.c_str() << '\n';
                std::cout << "Define failure line in " << file_name_ << '\n';
#endif
                continue;
            }
            std::string key = one;
            key.erase(idx);
            std::string value = one;
            value.erase(0, idx+1);
            container_[key] = value;
        }
    }
    return true;
}

void coral::init_config(const std::string& file_name)
{
    // config value initialize
    if(!coral::config::instance()->init(file_name)){
        throw coral::instance_init_error("init config file");
    }
}
