/*!
    \file       net_server.h
    \brief      Network(Socket) program server base class
    \details    Network server
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_NETSERVER_H__
#define __CORAL_NETSERVER_H__

#include "utility.h"

//! Core Library for Applications and Libraries
namespace coral {
    //! network server class
    /*!
        net_server Class는 서버용 base interface class로 사용하고
        derive하는 class는 run()과 thread_method()를 override하여 구현하여야 한다.
    */
    class net_server {
    public:
        //! default construct in protected area.
        net_server();
        //! default desturct
        virtual ~net_server();
        //! server socket initialize
        int init_socket(const std::string& ip_address, const std::string& port_no);
        //! virtual member method - server run
        virtual void run();

    protected:
        //! active method, it'll be overrided by derived class
        virtual void thread_method(const coral::client_info_t& client_info);
        // Member variables
        //sockets
        int server_socket_; ///< the socket of a server
        struct sockaddr_in server_address_;        ///< address struct of a server
        //! atomic the number of the client count
        std::atomic<int> client_count_;
    }; // end net_server class
} // end coral namespace

#endif // __CORAL_NETSERVER_H__
