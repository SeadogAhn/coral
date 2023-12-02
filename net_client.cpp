/*!
    \file       net_client.cpp
    \brief      Network(Socket) program client base class
    \details    Network client
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "net_client.h"
#include "log_manager.h"

//! global application name
extern std::string gv_app_name;

//! client socket class
coral::net_client::net_client() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    client_socket_ = 0;
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

coral::net_client::~net_client() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    close_socket();
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

int coral::net_client::init_socket(const std::string& ip_address, const std::string& port_no)
{
    CORAL_D_CLASS_MEMBER_FUNC_START;
    coral::elapsed_time et;
    std::string log_message;
    std::ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << ip_address << ',' << port_no << "):";
    try {
        client_socket_ = socket(PF_INET, SOCK_STREAM, 0);
        memset(&server_address_, 0, sizeof(server_address_));
        server_address_.sin_family = AF_INET;
        server_address_.sin_addr.s_addr = inet_addr(ip_address.c_str());
        server_address_.sin_port = htons(atoi(port_no.c_str()));

        int server_address_size_= sizeof(server_address_);
        if (connect(client_socket_, (struct sockaddr*)&server_address_, (socklen_t)server_address_size_) == -1) {
            throw network_error("connect() error");
        }
    }
    catch (coral::exception& error) {
        throw error;
    }
    catch (std::exception& error) {
        throw error;
    }
    catch (...) {
        throw unknown_error(CORAL_D_STRMSG(EN, ERR, 000010));
    }

    coral::print_string(log_message, gv_string_msg_size, "%s:ElapsedTime:%.6lfs", CORAL_D_STRMSG(EN, STR, 000004), et.sec());
    coral::log_manager::write(gv_app_name, method_info.str() + log_message);
    CORAL_D_CLASS_MEMBER_FUNC_END;
    return client_socket_;
}

void coral::net_client::run(coral::net_msg_t& msg)
{
    CORAL_D_CLASS_MEMBER_FUNC_START;
    coral::elapsed_time et;
    std::string log_message;
    std::ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << &msg << "):";
    try {
        msg.write_to_network(get_socket());
        msg.read_from_network(get_socket());
        std::cout << "[SERVER MSG]:" << msg << '\n';
    }
    catch (coral::exception& error) {
        throw error;
    }
    catch (std::exception& error) {
        throw error;
    }
    catch (...) {
        throw unknown_error(CORAL_D_STRMSG(EN, ERR, 000010));
    }

    coral::print_string(log_message, gv_string_msg_size, "%s:ElapsedTime:%.6lfs", CORAL_D_STRMSG(EN, STR, 000004), et.sec());
    coral::log_manager::write(gv_app_name, method_info.str() + log_message);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}
