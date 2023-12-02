/*!
    \file       config.h
    \brief      system config value define header
    \details    config value initialize from $HOME/config/config.txt into map container
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_CONFIG_H__
#define __CORAL_CONFIG_H__

#include <map>
#include <string>
#include <fstream>
#include <unistd.h>

#include "exception.h"
#include "variables.h"
#include "thread_lock.h"

//! Core Library for Applications and Libraries
namespace coral {
    //! system configuration class
    class config{
    public:
        //! key/value container container_t declaration. key : string, value : string
        typedef std::map<std::string, std::string> container_t;
        //! singletone pattern - instance() function declaration
        static  config* instance();
        //! config file load in container_
        bool init(const std::string& file_name = "");
        //! container_에서 입력된 string으로 key 검색
        /*!
            \param key config내에서 찾고자 하는 key string
            \return map container에서 key에 해당하는 value
        */
        std::string get_value (std::string key) { return container_[key]; }

    private:
        std::string file_name_;     ///< config file name
        static config*  config_;    ///< static config point
        //! config file stream에서 memory로 데이터를 읽어 들인다.
        /*!
            \param *ifstr config file stream pointer
            \return 파일 처리가 정상적으로 완료 되었을일 경우 true 이상이 있을 경우 false
        */
        bool read(std::ifstream &ifstr);
        container_t container_; ///< map< string, string > type data container
        static coral::thread_lock lock_;   ///< thread lock(mutex)
    }; // end config class
} // end coral namespace

//! Core Library for Applications and Libraries
namespace coral {
    //! global dcims initialize function
    void init_config(const std::string& file_name = "");
} // end coral namespace

#endif //__CORAL_CONFIG_H__
