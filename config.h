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
        //! container_���� �Էµ� string���� key �˻�
        /*!
            \param key config������ ã���� �ϴ� key string
            \return map container���� key�� �ش��ϴ� value
        */
        std::string get_value (std::string key) { return container_[key]; }

    private:
        std::string file_name_;     ///< config file name
        static config*  config_;    ///< static config point
        //! config file stream���� memory�� �����͸� �о� ���δ�.
        /*!
            \param *ifstr config file stream pointer
            \return ���� ó���� ���������� �Ϸ� �Ǿ����� ��� true �̻��� ���� ��� false
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
