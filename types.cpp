/*!
    \file       types.cpp
    \brief      type declaration and definition
    \details    defined structures initialization,
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "types.h"
#include "utility.h"

void coral::net_msg_t::data_container(const coral::net_msg_t::key_type& key, const coral::net_msg_t::value_type& value)
{
    data_container_[key] = value;
}

const coral::net_msg_t::value_type& coral::net_msg_t::data_container(const coral::net_msg_t::key_type& key)
{
    if (data_container_.find(key) == data_container_.end())
        throw std::domain_error("There is no " + key + " in the data_container.");
    return data_container_.at(key);
}

int coral::net_msg_t::write_to_network(int fd)
{
    int nWrittenSize = 0;
    int cmd_temp = htonl(cmd);
    int size_temp = htonl(size());
    nWrittenSize += write(fd, &cmd_temp, sizeof(int));
    nWrittenSize += write(fd, &size_temp, sizeof(int));
    for (const auto& e : data_container_) {
        // wirte key
        nWrittenSize += write_string_to_network(fd, extlib::any_cast<std::string>(e.first));
        // wirte value
        if (e.second.type() == typeid(bool)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_BOOL], 1);
            bool b = extlib::any_cast<bool>(e.second);
            nWrittenSize += write(fd, &b, sizeof(bool));
        }
        else if (e.second.type() == typeid(char)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_CHAR], 1);
            char c = extlib::any_cast<char>(e.second);
            nWrittenSize += write(fd, &c, sizeof(char));
        }
        else if (e.second.type() == typeid(int)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_INT], 1);
            int n = htonl(extlib::any_cast<int>(e.second));
            nWrittenSize += write(fd, &n, sizeof(int));
        }
        else if (e.second.type() == typeid(long)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_LONG], 1);
            long n = htonl(extlib::any_cast<long>(e.second));
            nWrittenSize += write(fd, &n, sizeof(long));
        }
        else if (e.second.type() == typeid(long long)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_LONGLONG], 1);
            long long n = htobe64(extlib::any_cast<long long>(e.second));
            nWrittenSize += write(fd, &n, sizeof(long long));
        }
        else if (e.second.type() == typeid(unsigned char)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_UCHAR], 1);
            unsigned char c = extlib::any_cast<unsigned char>(e.second);
            nWrittenSize += write(fd, &c, sizeof(unsigned char));
        }
        else if (e.second.type() == typeid(unsigned int)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_UINT], 1);
            unsigned int n = htonl(extlib::any_cast<unsigned int>(e.second));
            nWrittenSize += write(fd, &n, sizeof(unsigned int));
        }
        else if (e.second.type() == typeid(unsigned long)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_ULONG], 1);
            unsigned long n = htonl(extlib::any_cast<unsigned long>(e.second));
            nWrittenSize += write(fd, &n, sizeof(unsigned long));
        }
        else if (e.second.type() == typeid(unsigned long long)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_ULONGLONG], 1);
            unsigned long long n = htobe64(extlib::any_cast<unsigned long long>(e.second));
            nWrittenSize += write(fd, &n, sizeof(unsigned long long));
        }
        else if (e.second.type() == typeid(float)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_FLOAT], 1);
            double dbl = extlib::any_cast<float>(e.second);
            nWrittenSize += write(fd, &dbl, sizeof(float));
        }
        else if (e.second.type() == typeid(double)) {
            nWrittenSize += write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_DOUBLE], 1);
            double dbl = extlib::any_cast<double>(e.second);
            nWrittenSize += write(fd, &dbl, sizeof(double));
        }
        else if (e.second.type() == typeid(char*)) {
            std::string str(extlib::any_cast<char*>(e.second));
            nWrittenSize += write_string_to_network(fd, str);
        }
        else if (e.second.type() == typeid(const char*)) {
            std::string str(extlib::any_cast<const char*>(e.second));
            nWrittenSize += write_string_to_network(fd, str);
        }
        else if (e.second.type() == typeid(std::string)) {
            nWrittenSize += write_string_to_network(fd, extlib::any_cast<std::string>(e.second));
        }
    }
    return nWrittenSize;
}

int coral::net_msg_t::read_from_network(int fd)
{
    int nReadSize = 0;
    data_container_.clear();
    nReadSize += read(fd, &cmd, sizeof(int));
    cmd = ntohl(cmd);
    int size = 0;
    nReadSize += read(fd, &size, sizeof(int));
    size = ntohl(size);
    for (int i = 0; i < size; i++) {
        char key_type = 0;
        nReadSize += read(fd, &key_type, sizeof(char));
        std::string key_str;
        nReadSize += read_string_from_network(fd, key_str);

        char val_type = 0;
        nReadSize += read(fd, &val_type, sizeof(char));
        switch (val_type) {
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_BOOL]: {
                bool val = false;
                nReadSize += read(fd, &val, sizeof(bool));
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_CHAR]: {
                char val = 0;
                nReadSize += read(fd, &val, sizeof(char));
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_INT]: {
                int val = 0;
                nReadSize += read(fd, &val, sizeof(int));
                val = ntohl(val);
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_LONG]: {
                long val = 0;
                nReadSize += read(fd, &val, sizeof(long));
                val = ntohl(val);
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_LONGLONG]: {
                long long val = 0;
                nReadSize += read(fd, &val, sizeof(long long));
                val = be64toh(val);
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_UCHAR]: {
                unsigned char val = 0;
                nReadSize += read(fd, &val, sizeof(unsigned char));
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_UINT]: {
                unsigned int val = 0;
                nReadSize += read(fd, &val, sizeof(unsigned int));
                val = ntohl(val);
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_ULONG]: {
                unsigned long val = 0;
                nReadSize += read(fd, &val, sizeof(unsigned long));
                val = ntohl(val);
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_ULONGLONG]: {
                unsigned long long val = 0;
                nReadSize += read(fd, &val, sizeof(unsigned long long));
                val = be64toh(val);
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_FLOAT]: {
                float val = 0;
                nReadSize += read(fd, &val, sizeof(float));
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_DOUBLE]: {
                double val = 0;
                nReadSize += read(fd, &val, sizeof(double));
                data_container_[key_str] = val;
                break;
            }
            case NET_MSG_TYPE_CODE[NET_MSG_TYPE_STRING]: {
                std::string val;
                nReadSize += read_string_from_network(fd, val);
                data_container_[key_str] = std::move(val);
                break;
            }
        }
    }
    return nReadSize;
}

//! clear
void coral::net_msg_t::clear() {
    cmd = 0;
    clear_data_container();
}

//! clear
void coral::net_msg_t::clear_data_container() {
    data_container_.clear();
}

const std::string coral::net_msg_t::to_string() const {
    std::ostringstream oss;
    oss << "CMD:" << cmd << ',' << "SIZE:" << size() << ',' << "DATA:";
    for (const auto& e: data_container_) {
        oss << e.first << '=';
        if (e.second.type() == typeid(bool))                    { oss << extlib::any_cast<bool>(e.second);              }
        else if (e.second.type() == typeid(char))               { oss << extlib::any_cast<char>(e.second);              }
        else if (e.second.type() == typeid(int))                { oss << extlib::any_cast<int>(e.second);               }
        else if (e.second.type() == typeid(long))               { oss << extlib::any_cast<long>(e.second);              }
        else if (e.second.type() == typeid(long long))          { oss << extlib::any_cast<long long>(e.second);         }
        else if (e.second.type() == typeid(unsigned char))      { oss << extlib::any_cast<unsigned char>(e.second);     }
        else if (e.second.type() == typeid(unsigned int))       { oss << extlib::any_cast<unsigned int>(e.second);      }
        else if (e.second.type() == typeid(unsigned long))      { oss << extlib::any_cast<unsigned long>(e.second);     }
        else if (e.second.type() == typeid(unsigned long long)) { oss << extlib::any_cast<unsigned long long>(e.second);}
        else if (e.second.type() == typeid(float))              { oss << extlib::any_cast<float>(e.second);             }
        else if (e.second.type() == typeid(double))             { oss << extlib::any_cast<double>(e.second);            }
        else if (e.second.type() == typeid(char*))              { oss << extlib::any_cast<char*>(e.second);             }
        else if (e.second.type() == typeid(char const *))       { oss << extlib::any_cast<char const *>(e.second);      }
        else if (e.second.type() == typeid(std::string))        { oss << extlib::any_cast<std::string>(e.second);       }
        oss << ',';
    }
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, coral::net_msg_t& msg) {
    os << msg.to_string();
    return os;
}

int coral::net_msg_t::write_string_to_network(int fd, const std::string& str)
{
    int nCumSize = ::write(fd, &NET_MSG_TYPE_CODE[NET_MSG_TYPE_STRING], 1);
    int size = htonl(str.size());
    nCumSize += ::write(fd, &size, sizeof(int));
    nCumSize += ::write(fd, str.c_str(), str.size());
    return nCumSize;
}

int coral::net_msg_t::read_string_from_network(int fd, std::string& str)
{
    str.clear();
    int size = 0;
    int nCumSize = ::read(fd, &size, sizeof(int));
    size = ntohl(size);
    str.resize(size);
    nCumSize += ::read(fd, &str[0], size);
    return nCumSize;
}

int coral::db_data_set_t::col_name_pos(const std::string& col_name) const
{
    return find_data_pos_in_container(col_names, col_name);
}

void coral::db_data_set_t::clear()
{
    col_names.clear();
    data_container.clear();
}

std::ostream& operator<<(std::ostream& os, const coral::db_data_set_t& db_data_set)
{
    for (const auto& col: db_data_set.col_names) {
        os << col << ',';
    }
    os << '\n';
    for (const auto& rec: db_data_set.data_container) {
        for (size_t i = 0; i < rec.size(); i++) {
            os << rec[i] << ',';
        }
        os << '\n';
    }
    return os;
}

int coral::db_any_data_set_t::col_name_pos(const std::string& col_name) const
{
    int pos = -1;
    for (size_t i = 0; i < col_meta_data.size(); i++) {
        if (col_name == col_meta_data[i].getString(oracle::occi::MetaData::ATTR_NAME)) {
            pos = i+1;
            break;
        }
    }
    return pos;
}

void coral::db_any_data_set_t::clear()
{
    col_meta_data.clear();
    data_container.clear();
}
