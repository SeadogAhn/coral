/*!
    \file       exception.h
    \brief      process error, exception definition
    \details    derived exception from std::exception
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_EXCEPTION_H__
#define __CORAL_EXCEPTION_H__

#include <iostream>
#include <stdexcept>
#include <exception>
#include <string>
#include <cstdlib>

//! Core Library for Applications and Libraries
namespace coral {
    //-------------------------------------------------------------------------------------------------------------
    /*! coral error handler : process halt
        \param msg error message
    */
    inline void error(const std::string msg) {
        std::cerr << "coral run-time error..." << std::endl;
        std::cerr << msg << std::endl;
        std::cerr << "...now exiting to system..." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    //! coral exception Base class
    /*!
        coral Library의 예외 처리를 위한 최상위 클래스
        예외 종류별 처리 될 수 있도록 상속하여 관리함.
    */
    class exception : public std::exception {
    public:
        exception(std::string name = "") : std::exception(), name_(name) {}
        virtual ~exception() throw() {}
        virtual const char* what() const noexcept { return name_.c_str(); }
    protected:
        std::string name_;
    };

    //! domain error exception
    class domain_error : public exception {
    public:
        domain_error(std::string name="")
            : exception("coral - domain exception:" + name) {}
    };

    //! time out of process exception
    class timeout_error : public exception {
    public:
        timeout_error(std::string name="")
            : exception("coral - time out of process exception:" + name) {}
    };

    //! memory allocation exception
    class memory_alloc_error : public exception {
    public:
        memory_alloc_error(std::string name="")
            : exception("coral - memory allocation exception:" + name) {}
    };

    //! data initialize exception
    class data_init_error : public exception {
    public:
        data_init_error(std::string name="")
            : exception("coral - data initialize exception:" + name) {}
    };

    //! data find exception
    class data_find_error : public exception {
    public:
        data_find_error(std::string name="")
            : exception("coral - data find exception:" + name) {}
    };

    //! database exception
    class database_error : public exception {
    public:
        database_error(std::string name="")
            : exception("coral - database exception:" + name) {}
    };

    //! container empty exception
    class container_empty_error : public exception {
    public:
        container_empty_error(std::string name="")
            : exception("coral - container empty exception:" + name) {}
    };

    //! process exception
    class process_error : public exception {
    public:
        process_error(std::string name="")
            : exception("coral - process exception:" + name) {}
    };

    //!instance initialize exception
    class instance_init_error : public exception {
    public:
        instance_init_error(std::string name="")
            : exception("coral - instance initialize exception:" + name) {}
    };

    //!IO stream initialize exception
    class stream_error : public exception {
    public:
        stream_error(std::string name="")
            : exception("coral - IO stream exception:" + name) {}
    };

    //!network process exception
    class network_error : public exception {
    public:
        network_error(std::string name="")
            : exception("coral - network exception:" + name) {}
    };

    //!thread stream initialize exception
    class thread_error : public exception {
    public:
        thread_error(std::string name="")
            : exception("coral - thread exception:" + name) {}
    };

    //!unknow exception
    class unknown_error : public exception {
    public:
        unknown_error(std::string name="")
            : exception("coral - unknown exception:" + name) {}
    };
}

#endif //__CORAL_EXCEPTION_H__
