/*!
    \file       net_server.cpp
    \brief      Network(Socket) program server base class
    \details    Network server
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "net_server.h"
#include "log_manager.h"
#include "thread_pool.h"

//! global application name
extern std::string gv_app_name;

coral::net_server::net_server() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    server_socket_ = 0;
    client_count_ = 0;
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

coral::net_server::~net_server() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    if (server_socket_ > 0) close(server_socket_);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

int coral::net_server::init_socket(const std::string& ip_address, const std::string& port_no) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    coral::elapsed_time et;
    std::string log_message;
    std::ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << ip_address << ',' << port_no << "):";

    server_socket_= socket(PF_INET, SOCK_STREAM, 0);

    if (server_socket_ <= 0) {
        log_manager::write(gv_app_name, method_info.str() + "socket() error");
        return -1;
    }

    memset(&server_address_, 0, sizeof(server_address_));
    server_address_.sin_family = AF_INET;
    server_address_.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address_.sin_port = htons(atoi(port_no.c_str()));

    socklen_t server_address_size_ = sizeof(server_address_);
    if (bind(server_socket_, (struct sockaddr*)&server_address_, server_address_size_) < 0) {
        log_manager::write(gv_app_name, method_info.str() + "bind() error");
        close(server_socket_);
        CORAL_D_CLASS_MEMBER_FUNC_END;
        return -1;
    }

    if (listen(server_socket_, std::stoi(coral::config::instance()->get_value("NET_SERVER_LISTENER"))) < 0) {
        log_manager::write(gv_app_name, method_info.str() + "listen() error");
        close(server_socket_);
        CORAL_D_CLASS_MEMBER_FUNC_END;
        return -1;
    }

    std::ostringstream oss;
    oss << "ServerSocket=" << server_socket_;
    log_manager::write(gv_app_name, method_info.str() + oss.str());
    CORAL_D_CLASS_MEMBER_FUNC_END;
    return server_socket_;
}

void coral::net_server::run() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    coral::elapsed_time et;
    std::string log_message;
    std::ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    try {
        coral::thread_pool tp(std::stoi(coral::config::instance()->get_value("NET_SERVER_THREAD_POOL")));
        bool is_using_thread_pool = coral::config::instance()->get_value("NET_SERVER_USING_THREAD_POOL") == "TRUE" ? true : false;

        while (true) {
            coral::client_info_t client_info;
            int client_address_size = sizeof(client_info.address);
            // accept
#ifdef __linux__
            client_info.socket = accept(server_socket_, (struct sockaddr*)&client_info.address, (socklen_t *)&client_address_size);
#else
            client_info.socket = accept(server_socket_, (struct sockaddr*)&client_info.address, &client_address_size);
#endif
            if (client_info.socket < 0) {
                throw coral::network_error("accept() error.");
            }
            client_count_.fetch_add(1);
            coral::print_string(log_message, 64, "to be connected client count:%d", static_cast<int>(client_count_));
            coral::log_manager::write(gv_app_name, method_info.str() + log_message);

            if (is_using_thread_pool) {
                auto f = tp.enqueue_job(std::bind(&coral::net_server::thread_method, this, std::placeholders::_1), client_info);
            }
            else {
                auto f = std::async(std::launch::async, &coral::net_server::thread_method, this, client_info);
            }
            //usleep(1000);// wait 1mm sec
        }
    }
    catch (coral::exception& error) {
        coral::log_manager::write(gv_app_name, method_info.str()+error.what());
        throw error;
    }
    catch (std::exception& error) {
        coral::log_manager::write(gv_app_name, method_info.str()+error.what());
        throw error;
    }
    catch (...) {
        coral::log_manager::write(gv_app_name, method_info.str()+CORAL_D_STRMSG(EN, ERR, 000010));
        throw coral::domain_error(method_info.str()+CORAL_D_STRMSG(EN, ERR, 000010));
    }

    coral::print_string(log_message, gv_string_msg_size, "%s:ElapsedTime:%.6lfs", CORAL_D_STRMSG(EN, STR, 000004), et.sec());
    coral::log_manager::write(gv_app_name, method_info.str() + log_message);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::net_server::thread_method(const coral::client_info_t& client_info)
{
    coral::elapsed_time et;
    std::ostringstream method_info;
    method_info << CORAL_D_FUNCTION_INFO << "(" << client_info.socket << ',' << inet_ntoa(client_info.address.sin_addr) << "):";
    std::string log_message;
    try {
        net_msg_t net_msg;
        while(true) {
            net_msg.read_from_network(client_info.socket);
            if (net_msg.cmd == -1) {
                break;
            }
            std::cout << "[CLIENT MSG]:" << net_msg << '\n';
            net_msg.write_to_network(client_info.socket);
            coral::log_manager::write(gv_app_name, net_msg.to_string());
        };
    }
    catch (coral::exception& error) {
        coral::log_manager::write(gv_app_name, method_info.str()+error.what());
    }
    catch (std::exception& error) {
        coral::log_manager::write(gv_app_name, method_info.str()+error.what());
    }
    catch (...) {
        coral::log_manager::write(gv_app_name, method_info.str()+CORAL_D_STRMSG(EN, ERR, 000010));
    }

    // 접속 중인 소켓을 닫는다.
    close(client_info.socket);
    client_count_.fetch_sub(1); // decrease client count

    coral::print_string(log_message, gv_string_msg_size, "%s:ElapsedTime:%.6lfs", CORAL_D_STRMSG(EN, STR, 000004), et.sec());
    coral::log_manager::write(gv_app_name, method_info.str() + log_message);
}
