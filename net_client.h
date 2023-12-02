/*!
    \file       net_client.h
    \brief      Network(Socket) program client base class
    \details    Network client
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_NETCLIENT_H__
#define __CORAL_NETCLIENT_H__

#include "utility.h"

//! Core Library for Applications and Libraries
namespace coral {
    //! client socket class
    class net_client {
    public:
        //! default construct
        net_client();
        //! default destruct
        virtual ~net_client();
        // overriden function
        virtual int init_socket(const std::string& ip_address, const std::string& port_no);
        virtual void run(coral::net_msg_t& msg);

    protected:
        //! get connected socket
        int get_socket() { return client_socket_; }
        //! close connected socket
        void close_socket() {
            if (client_socket_ > 0) {
                close(client_socket_);
            }
        }

    private:
        int client_socket_;
        struct sockaddr_in server_address_;
    }; // end net_client class
} // end coral namespace

#endif // __CORAL_NETCLIENT_H__
