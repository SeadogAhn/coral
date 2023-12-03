/*!
    \file       ora_dbm.cpp
    \brief      Oracle Database management base class
    \details    support Oracle OCCI
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "ora_dbm.h"
#include "utility.h"
#include "log_manager.h"
#include "alarm_controller.h"

extern std::string gv_app_name;

coral::ora_dbm::ora_dbm(const std::string& user, const std::string& password, const std::string& db_name, oracle::occi::Environment::Mode mode) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    connect_to_db(user, password, db_name, mode);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

coral::ora_dbm::~ora_dbm() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    disconnect_from_db();
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::ora_dbm::connect_to_db(const std::string& user, const std::string& password, const std::string& db_name, oracle::occi::Environment::Mode mode) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    bool flag = true;
    int loop_count = 1;
    int loop_limit = 6; // 6 times
    do {
        try {
#ifdef _DEBUG
            std::cout << "Connect Info - User:" << user << ", Password:" << password << ", DB:" << db_name << std::endl;
#endif
            environment_p_ = oracle::occi::Environment::createEnvironment("KO16KSC5601", "UTF8", mode);
            connection_p_ = environment_p_->createConnection(user, password, db_name);
            flag = false;
        }
        catch(const oracle::occi::SQLException& error) {
            std::string strErrMessage;
            coral::print_string(strErrMessage, 1024, "Error number:%d:%s", error.getErrorCode(), error.getMessage().substr(0, error.getMessage().size()-1).c_str());
            if (loop_count < loop_limit) {
                sleep(10);
            }
            else {
                throw coral::database_error(strErrMessage);
            }
        }
        catch(std::exception& error) {
            throw error;
        }
        catch(...) {
            throw unknown_error(CORAL_D_STRMSG(EN, ERR, 000010));
        }
    }
    while (flag);

    CORAL_D_TRACE("DB Connect to %s, user:%s\n", db_name.c_str(), user.c_str());
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::ora_dbm::disconnect_from_db() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    environment_p_->terminateConnection(connection_p_);
    oracle::occi::Environment::terminateEnvironment(environment_p_);
    CORAL_D_TRACE("DB disconnect\n");
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

/*! get table's meta information
    \return db_any_data_set_t::ora_meta_data_t
*/
coral::db_any_data_set_t::ora_meta_data_t coral::ora_dbm::get_metadata(const std::string& table_name) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    CORAL_D_CLASS_MEMBER_FUNC_END;
    /* Call the getMetaData method on the Connection object obtainedv*/
    return connection_p_->getMetaData(table_name, oracle::occi::MetaData::PTYPE_TABLE);
}

void coral::ora_dbm::execute_query(const std::string& query) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    coral::thread_lock lock;
    coral::thread_locker locker(lock);
    coral::alarm_controller alarm(std::stoi(coral::config::instance()->get_value("DB_PRC_LIMIT_TIME")));
    coral::elapsed_time et;
    std::string log_message;
    std::ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";

    oracle::occi::Statement* statement_p = nullptr;   ///< oracle statement

    try {
        statement_p = connection_p_->createStatement(query);
        statement_p->executeUpdate();
    }
    catch (oracle::occi::SQLException& error) {
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::print_string(log_message, 1024, "Error number:%d:%s", error.getErrorCode(), error.getMessage().substr(0, error.getMessage().size()-1).c_str());
        coral::log_manager::write(gv_app_name, method_info.str() + log_message);
        throw database_error(log_message);
    }
    catch (coral::exception& error) {
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (std::exception& error) {
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(CORAL_D_STRMSG(EN, ERR, 000010));
    }

    if (statement_p) connection_p_->terminateStatement (statement_p);
    coral::print_string(log_message, 1024, "Message:%s,ElapsedTime:%.6lf", CORAL_D_STRMSG(EN, STR, 000004), et.sec());
    coral::log_manager::write(gv_app_name, method_info.str() + log_message);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::ora_dbm::execute_query(const std::string& query, coral::db_data_set_t& db_data_set) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    coral::thread_lock lock;
    coral::thread_locker locker(lock);
    coral::alarm_controller alarm(std::stoi(coral::config::instance()->get_value("DB_PRC_LIMIT_TIME")));
    coral::elapsed_time et;
    std::string log_message;
    std::ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";

    oracle::occi::Statement*    statement_p   = nullptr;   ///< oracle statement
    oracle::occi::ResultSet*    resultset_p   = nullptr;   ///< oracle resultset

    try {
        statement_p = connection_p_->createStatement(query);
        statement_p->executeQuery();
        resultset_p = statement_p->getResultSet();
        get_result(*resultset_p, db_data_set);
    }
    catch (oracle::occi::SQLException& error) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::print_string(log_message, 1024, "Error number:%d:%s", error.getErrorCode(), error.getMessage().substr(0, error.getMessage().size()-1).c_str());
        coral::log_manager::write(gv_app_name, method_info.str() + log_message);
        throw database_error(log_message);
    }
    catch (coral::exception& error) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (std::exception& error) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(CORAL_D_STRMSG(EN, ERR, 000010));
    }

    if (resultset_p) statement_p->closeResultSet (resultset_p);
    if (statement_p) connection_p_->terminateStatement (statement_p);
    coral::print_string(log_message, 1024, "Message:%s,ElapsedTime:%.6lf", CORAL_D_STRMSG(EN, STR, 000004), et.sec());
    coral::log_manager::write(gv_app_name, method_info.str() + log_message);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::ora_dbm::get_result(oracle::occi::ResultSet& rst, coral::db_data_set_t& db_data_set) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    db_data_set.clear();
    const auto& meta_data = rst.getColumnListMetaData();
    for (size_t i = 0; i < meta_data.size(); i++) {
        db_data_set.col_names.push_back(meta_data[i].getString(oracle::occi::MetaData::ATTR_NAME));
        const auto type = meta_data[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE);
        const auto size = meta_data[i].getInt(oracle::occi::MetaData::ATTR_DATA_SIZE);
        if (type == oracle::occi::OCCISTRING
            || type == oracle::occi::OCCICHAR
            || type == oracle::occi::OCCI_SQLT_CHR
            || type == oracle::occi::OCCI_SQLT_STR
            || type == oracle::occi::OCCI_SQLT_VCS) {
            rst.setMaxColumnSize(i+1, size);
        }
        if (type == oracle::occi::OCCI_SQLT_LNG) {
            rst.setMaxColumnSize(i+1, 32767); // caution to be limited
        }
    }
    // loop iterator cursor
    while (rst.next()) {
        coral::db_data_set_t::row_record_t row;
        for (size_t i = 0; i < meta_data.size(); i++) {
            row.push_back(value_to_string(rst, i+1, meta_data[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE)));
        }
        db_data_set.data_container.push_back(row);
    }
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::ora_dbm::execute_query(const std::string& query, coral::db_any_data_set_t& db_any_data_set) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    coral::thread_lock lock;
    coral::thread_locker locker(lock);
    coral::alarm_controller alarm(std::stoi(coral::config::instance()->get_value("DB_PRC_LIMIT_TIME")));
    coral::elapsed_time et;
    std::string log_message;
    std::ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";

    oracle::occi::Statement* statement_p = nullptr;   ///< oracle statement
    oracle::occi::ResultSet* resultset_p = nullptr;   ///< oracle resultset

    try {
        statement_p = connection_p_->createStatement(query);
        statement_p->executeQuery();
        resultset_p = statement_p->getResultSet();
        get_result(*resultset_p, db_any_data_set);
    }
    catch (oracle::occi::SQLException& error) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::print_string(log_message, 1024, "Error number:%d:%s", error.getErrorCode(), error.getMessage().substr(0, error.getMessage().size()-1).c_str());
        coral::log_manager::write(gv_app_name, method_info.str() + log_message);
        throw database_error(log_message);
    }
    catch (coral::exception& error) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (std::exception& error) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        if (resultset_p) statement_p->closeResultSet (resultset_p);
        if (statement_p) connection_p_->terminateStatement (statement_p);
        coral::log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(CORAL_D_STRMSG(EN, ERR, 000010));
    }

    if (resultset_p) statement_p->closeResultSet (resultset_p);
    if (statement_p) connection_p_->terminateStatement (statement_p);
    coral::print_string(log_message, 1024, "Message:%s,ElapsedTime:%.6lf", CORAL_D_STRMSG(EN, STR, 000004), et.sec());
    coral::log_manager::write(gv_app_name, method_info.str() + log_message);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::ora_dbm::get_result(oracle::occi::ResultSet& rst, coral::db_any_data_set_t& db_any_data_set) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    db_any_data_set.clear();
    db_any_data_set.col_meta_data = rst.getColumnListMetaData();
    const auto& meta_data = db_any_data_set.col_meta_data;
    for (size_t i = 0; i < meta_data.size(); i++) {
        const auto type = meta_data[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE);
        const auto size = meta_data[i].getInt(oracle::occi::MetaData::ATTR_DATA_SIZE);
        if (type == oracle::occi::OCCISTRING
            || type == oracle::occi::OCCICHAR
            || type == oracle::occi::OCCI_SQLT_CHR
            || type == oracle::occi::OCCI_SQLT_STR
            || type == oracle::occi::OCCI_SQLT_VCS) {
            rst.setMaxColumnSize(i+1, size);
        }
        if (type == oracle::occi::OCCI_SQLT_LNG) {
            rst.setMaxColumnSize(i+1, 32767); // caution to be limited
        }
    }
    // loop iterator cursor
    while (rst.next()) {
        coral::db_any_data_set_t::row_record_t row;
        for (size_t i = 0; i < meta_data.size(); i++) {
            row.push_back(value_to_any(rst, i+1, meta_data[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE)));
        }
        db_any_data_set.data_container.push_back(row);
    }
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

std::string coral::ora_dbm::value_to_string(oracle::occi::ResultSet& rst, size_t pos, int type) {
    switch (type) {
    case oracle::occi::OCCIBOOL       : return std::to_string(rst.getInt(pos));
    case oracle::occi::OCCIINT        : return std::to_string(rst.getInt(pos));
    case oracle::occi::OCCIROWID      : return std::to_string(rst.getUInt(pos));
    case oracle::occi::OCCINUMBER     : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCI_SQLT_NUM  : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCIFLOAT      : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCIBFLOAT     : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCIBDOUBLE    : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCIIBFLOAT    : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCIIBDOUBLE   : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCIDOUBLE     : return std::to_string(rst.getDouble(pos));
    case oracle::occi::OCCISTRING     : return rst.getString(pos);
    case oracle::occi::OCCI_SQLT_CHR  : return rst.getString(pos);
    case oracle::occi::OCCI_SQLT_STR  : return rst.getString(pos);
    case oracle::occi::OCCI_SQLT_LNG  : return rst.getString(pos);
    case oracle::occi::OCCI_SQLT_VCS  : return rst.getString(pos);
    case oracle::occi::OCCICHAR       : return rst.getString(pos);
    case oracle::occi::OCCIDATE       : return rst.getDate(pos).toText("YYYY/MM/DD HH24:MI:SS");
    case oracle::occi::OCCI_SQLT_DAT  : return rst.getDate(pos).toText("YYYY/MM/DD HH24:MI:SS");
    case oracle::occi::OCCIBLOB       : return blob_to_string(rst.getBlob(pos));
    case oracle::occi::OCCI_SQLT_BLOB : return blob_to_string(rst.getBlob(pos));
    case oracle::occi::OCCICLOB       : return clob_to_string(rst.getClob(pos));
    case oracle::occi::OCCI_SQLT_CLOB : return clob_to_string(rst.getClob(pos));
    default: return "";
    }
}

std::string coral::ora_dbm::blob_to_string(oracle::occi::Blob blob)
{
    if (blob.isNull()) return "";
    if (!blob.isOpen()) blob.open(oracle::occi::OCCI_LOB_READONLY);
    int size = blob.length();   // size of bytes
    std::string str;
    if (size) {
        str.resize(size);
        oracle::occi::Stream *instream = blob.getStream(1,0);
        instream->readBuffer(&str[0], size);
        blob.closeStream(instream);
    }
    if (blob.isOpen()) blob.close();
    return str;
}
//
std::string coral::ora_dbm::clob_to_string(oracle::occi::Clob clob)
{
    if (clob.isNull()) return "";
    if (!clob.isOpen()) clob.open(oracle::occi::OCCI_LOB_READONLY);
    size_t size = clob.length(); // size of characters
    if (to_upper_string(clob.getCharSetId()) == "KO16KSC5601") size *= 2; // 2 byte wide character
    std::string str;
    if (size) {
        char *arr = new char[size+1];
        std::memset(arr, 0, size+1);
        oracle::occi::Stream *instream = clob.getStream(1,0);
        instream->readBuffer(arr, size);
        clob.closeStream (instream);
        str = arr;
        if (arr) delete [] arr;
    }
    if (clob.isOpen()) clob.close();
    return str;
}
//
extlib::any coral::ora_dbm::value_to_any(oracle::occi::ResultSet& rst, size_t pos, int type)
{
    extlib::any val;
    switch (type) {
    case oracle::occi::OCCIBOOL       : val = rst.getInt(pos);    break;
    case oracle::occi::OCCIINT        : val = rst.getInt(pos);    break;
    case oracle::occi::OCCIROWID      : val = rst.getUInt(pos);   break;
    case oracle::occi::OCCINUMBER     : val = rst.getDouble(pos); break;
    case oracle::occi::OCCI_SQLT_NUM  : val = rst.getDouble(pos); break;
    case oracle::occi::OCCIFLOAT      : val = rst.getDouble(pos); break;
    case oracle::occi::OCCIBFLOAT     : val = rst.getDouble(pos); break;
    case oracle::occi::OCCIBDOUBLE    : val = rst.getDouble(pos); break;
    case oracle::occi::OCCIIBFLOAT    : val = rst.getDouble(pos); break;
    case oracle::occi::OCCIIBDOUBLE   : val = rst.getDouble(pos); break;
    case oracle::occi::OCCIDOUBLE     : val = rst.getDouble(pos); break;
    case oracle::occi::OCCISTRING     : val = rst.getString(pos); break;
    case oracle::occi::OCCI_SQLT_CHR  : val = rst.getString(pos); break;
    case oracle::occi::OCCI_SQLT_STR  : val = rst.getString(pos); break;
    case oracle::occi::OCCI_SQLT_LNG  : val = rst.getString(pos); break;
    case oracle::occi::OCCI_SQLT_VCS  : val = rst.getString(pos); break;
    case oracle::occi::OCCICHAR       : val = rst.getString(pos); break;
    case oracle::occi::OCCIDATE       : val = rst.getDate(pos);   break;
    case oracle::occi::OCCI_SQLT_DAT  : val = rst.getDate(pos);   break;
    case oracle::occi::OCCIBLOB       : val = rst.getBlob(pos);   break;
    case oracle::occi::OCCI_SQLT_BLOB : val = rst.getBlob(pos);   break;
    case oracle::occi::OCCICLOB       : val = rst.getClob(pos);   break;
    case oracle::occi::OCCI_SQLT_CLOB : val = rst.getClob(pos);   break;
    }
    return val;
}
//
std::string coral::ora_dbm::any_ora_type_to_string(const extlib::any& val, int type)
{
    switch (type) {
    case oracle::occi::OCCIBOOL       : return std::to_string(extlib::any_cast<int>(val));
    case oracle::occi::OCCIINT        : return std::to_string(extlib::any_cast<int>(val));
    case oracle::occi::OCCIROWID      : return std::to_string(extlib::any_cast<unsigned int>(val));
    case oracle::occi::OCCINUMBER     : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCI_SQLT_NUM  : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCIFLOAT      : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCIBFLOAT     : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCIBDOUBLE    : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCIIBFLOAT    : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCIIBDOUBLE   : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCIDOUBLE     : return std::to_string(extlib::any_cast<double>(val));
    case oracle::occi::OCCISTRING     : return extlib::any_cast<std::string>(val);
    case oracle::occi::OCCI_SQLT_CHR  : return extlib::any_cast<std::string>(val);
    case oracle::occi::OCCI_SQLT_STR  : return extlib::any_cast<std::string>(val);
    case oracle::occi::OCCI_SQLT_LNG  : return extlib::any_cast<std::string>(val);
    case oracle::occi::OCCI_SQLT_VCS  : return extlib::any_cast<std::string>(val);
    case oracle::occi::OCCICHAR       : return extlib::any_cast<std::string>(val);
    case oracle::occi::OCCIDATE       : return extlib::any_cast<oracle::occi::Date>(val).toText("YYYY/MM/DD HH24:MI:SS");
    case oracle::occi::OCCI_SQLT_DAT  : return extlib::any_cast<oracle::occi::Date>(val).toText("YYYY/MM/DD HH24:MI:SS");
    case oracle::occi::OCCIBLOB       : return blob_to_string(extlib::any_cast<oracle::occi::Blob>(val));
    case oracle::occi::OCCI_SQLT_BLOB : return blob_to_string(extlib::any_cast<oracle::occi::Blob>(val));
    case oracle::occi::OCCICLOB       : return clob_to_string(extlib::any_cast<oracle::occi::Clob>(val));
    case oracle::occi::OCCI_SQLT_CLOB : return clob_to_string(extlib::any_cast<oracle::occi::Clob>(val));
    }
    return "";
}
//
void coral::ora_dbm::set_max_param_size(oracle::occi::Statement& stmt, const coral::db_any_data_set_t::col_meta_t& metadata)
{
    for (size_t i = 0; i < metadata.size(); i++) {
        const auto type = metadata[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE);
        const auto size = metadata[i].getInt(oracle::occi::MetaData::ATTR_DATA_SIZE);
        if (type == oracle::occi::OCCISTRING
            || type == oracle::occi::OCCICHAR
            || type == oracle::occi::OCCI_SQLT_CHR
            || type == oracle::occi::OCCI_SQLT_STR
            || type == oracle::occi::OCCI_SQLT_VCS) {
            stmt.setMaxParamSize(i+1, size);
        }
        if (type == oracle::occi::OCCI_SQLT_LNG) {
            stmt.setMaxParamSize(i+1, 32767);
        }
    }
}
//
void coral::ora_dbm::set_param_value(oracle::occi::Statement& stmt, const coral::db_any_data_set_t::col_meta_t& metadata, const coral::db_any_data_set_t::row_record_t& records)
{
    for (size_t i = 0; i < records.size(); i++) {
        switch (metadata[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE)) {
        case oracle::occi::OCCIBOOL       : stmt.setInt(i+1, extlib::any_cast<int>(records[i]));    break;
        case oracle::occi::OCCIINT        : stmt.setInt(i+1, extlib::any_cast<int>(records[i]));    break;
        case oracle::occi::OCCIROWID      : stmt.setUInt(i+1, extlib::any_cast<int>(records[i]));   break;
        case oracle::occi::OCCINUMBER     : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCI_SQLT_NUM  : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCIFLOAT      : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCIBFLOAT     : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCIBDOUBLE    : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCIIBFLOAT    : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCIIBDOUBLE   : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCIDOUBLE     : stmt.setDouble(i+1, extlib::any_cast<double>(records[i])); break;
        case oracle::occi::OCCISTRING     : stmt.setString(i+1, extlib::any_cast<std::string>(records[i])); break;
        case oracle::occi::OCCI_SQLT_CHR  : stmt.setString(i+1, extlib::any_cast<std::string>(records[i])); break;
        case oracle::occi::OCCI_SQLT_STR  : stmt.setString(i+1, extlib::any_cast<std::string>(records[i])); break;
        case oracle::occi::OCCI_SQLT_LNG  : stmt.setString(i+1, extlib::any_cast<std::string>(records[i])); break;
        case oracle::occi::OCCI_SQLT_VCS  : stmt.setString(i+1, extlib::any_cast<std::string>(records[i])); break;
        case oracle::occi::OCCICHAR       : stmt.setString(i+1, extlib::any_cast<std::string>(records[i])); break;
        case oracle::occi::OCCIDATE       : stmt.setDate(i+1, extlib::any_cast<oracle::occi::Date>(records[i]));   break;
        case oracle::occi::OCCI_SQLT_DAT  : stmt.setDate(i+1, extlib::any_cast<oracle::occi::Date>(records[i]));   break;
        case oracle::occi::OCCIBLOB       : stmt.setBlob(i+1, extlib::any_cast<oracle::occi::Blob>(records[i]));   break;
        case oracle::occi::OCCI_SQLT_BLOB : stmt.setBlob(i+1, extlib::any_cast<oracle::occi::Blob>(records[i]));   break;
        case oracle::occi::OCCICLOB       : stmt.setClob(i+1, extlib::any_cast<oracle::occi::Clob>(records[i]));   break;
        case oracle::occi::OCCI_SQLT_CLOB : stmt.setClob(i+1, extlib::any_cast<oracle::occi::Clob>(records[i]));   break;
        }
    }
}

void coral::ora_dbm::print_metadata(std::ostream& os, const oracle::occi::MetaData& meta_data)
{
    CORAL_D_CLASS_MEMBER_FUNC_START;
    // output table metadata
    os << "Object ID        :" << meta_data.getUInt(oracle::occi::MetaData::ATTR_OBJ_ID)       << std::endl;
    os << "Name             :" << meta_data.getString(oracle::occi::MetaData::ATTR_OBJ_NAME)   << std::endl;
    os << "Schema           :" << meta_data.getString(oracle::occi::MetaData::ATTR_OBJ_SCHEMA) << std::endl;
    oracle::occi::Timestamp tstamp = meta_data.getTimestamp(oracle::occi::MetaData::ATTR_TIMESTAMP);
    int year;
    unsigned int month, day;
    tstamp.getDate(year, month, day);
    os << "Timestamp        :" << year << '.' << month << '.' << day << std::endl;
    os << "Number of Columns:" << meta_data.getInt(oracle::occi::MetaData::ATTR_NUM_COLS) << std::endl;
    // output column metadata
    os << "ColumnName,DataType,DataSize,Null" << std::endl;
    for (const auto& e: meta_data.getVector(oracle::occi::MetaData::ATTR_LIST_COLUMNS)) {
        os << e.getString(oracle::occi::MetaData::ATTR_NAME)
            << "," << get_col_data_type_name(e.getInt(oracle::occi::MetaData::ATTR_DATA_TYPE))
            << "," << e.getInt(oracle::occi::MetaData::ATTR_DATA_SIZE)
            << "," << e.getBoolean(oracle::occi::MetaData::ATTR_IS_NULL)
            << std::endl;
    }
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

std::string coral::ora_dbm::get_col_data_type_name(int type)
{
    switch (type) {
    case oracle::occi::OCCI_SQLT_CHR          : return "VARCHAR2";
    case oracle::occi::OCCI_SQLT_NUM          : return "NUMBER";
    case oracle::occi::OCCIINT                : return "INTEGER";
    case oracle::occi::OCCIFLOAT              : return "FLOAT";
    case oracle::occi::OCCIBFLOAT             : return "BFLOAT";
    case oracle::occi::OCCIBDOUBLE            : return "BDOUBLE";
    case oracle::occi::OCCIIBFLOAT            : return "IBFLOAT";
    case oracle::occi::OCCIIBDOUBLE           : return "IBDOUBLE";
    case oracle::occi::OCCI_SQLT_STR          : return "NULL terminated string";
    case oracle::occi::OCCI_SQLT_VNU          : return "VARNUM";
    case oracle::occi::OCCI_SQLT_PDN          : return "OCCI_SQLT_PDN";
    case oracle::occi::OCCI_SQLT_LNG          : return "LONG";
    case oracle::occi::OCCI_SQLT_VCS          : return "VARCHAR";
    case oracle::occi::OCCI_SQLT_NON          : return "OCCI_SQLT_NON";
    case oracle::occi::OCCI_SQLT_RID          : return "ROWID";
    case oracle::occi::OCCI_SQLT_DAT          : return "DATE";
    case oracle::occi::OCCI_SQLT_VBI          : return "VARRAW";
    case oracle::occi::OCCI_SQLT_BIN          : return "RAW";
    case oracle::occi::OCCI_SQLT_LBI          : return "LONG RAW";
    case oracle::occi::OCCIUNSIGNED_INT       : return "UNSIGNED INT";
    case oracle::occi::OCCI_SQLT_SLS          : return "OCCI_SQLT_SLS";
    case oracle::occi::OCCI_SQLT_LVC          : return "LONG VARCHAR";
    case oracle::occi::OCCI_SQLT_LVB          : return "LONG VARRAW";
    case oracle::occi::OCCI_SQLT_AFC          : return "CHAR";
    case oracle::occi::OCCI_SQLT_AVC          : return "CHARZ";
    case oracle::occi::OCCI_SQLT_CUR          : return "OCCI_SQLT_CUR";
    case oracle::occi::OCCI_SQLT_RDD          : return "ROWID descriptor";
    case oracle::occi::OCCI_SQLT_LAB          : return "OCCI_SQLT_LAB";
    case oracle::occi::OCCI_SQLT_OSL          : return "OCCI_SQLT_OSL";
    case oracle::occi::OCCI_SQLT_NTY          : return "NAMED DATA TYPE";
    case oracle::occi::OCCI_SQLT_REF          : return "REF";
    case oracle::occi::OCCI_SQLT_CLOB         : return "CLOB";
    case oracle::occi::OCCI_SQLT_BLOB         : return "BLOB";
    case oracle::occi::OCCI_SQLT_RSET         : return "OCCI_SQLT_RSET";
    case oracle::occi::OCCI_SQLT_NCO          : return "OCCI_SQLT_NCO";
    case oracle::occi::OCCI_SQLT_VST          : return "string";
    case oracle::occi::OCCI_SQLT_ODT          : return "OCCI_SQLT_ODT";
    case oracle::occi::OCCI_SQLT_DATE         : return "OCCI_SQLT_DATE";
    case oracle::occi::OCCI_SQLT_TIME         : return "OCCI_SQLT_TIME";
    case oracle::occi::OCCI_SQLT_TIME_TZ      : return "OCCI_SQLT_TIME_TZ";
    case oracle::occi::OCCI_SQLT_TIMESTAMP    : return "TIMESTAMP";
    case oracle::occi::OCCI_SQLT_TIMESTAMP_TZ : return "TIMESTAMP WITH TIME ZONE";
    case oracle::occi::OCCI_SQLT_INTERVAL_YM  : return "INTERVAL YEAR TO MONTH";
    case oracle::occi::OCCI_SQLT_INTERVAL_DS  : return "INTERVAL DAY TO SECOND";
    case oracle::occi::OCCI_SQLT_TIMESTAMP_LTZ: return "TIMESTAMP WITH LOCAL TIME ZONE";
    case oracle::occi::OCCI_SQLT_FILE         : return "FBFILE";
    case oracle::occi::OCCICHAR               : return "OCCICHAR";
    case oracle::occi::OCCIDOUBLE             : return "OCCIDOUBLE";
    case oracle::occi::OCCIBOOL               : return "BOOL";
    case oracle::occi::OCCIANYDATA            : return "ANYDATA";
    case oracle::occi::OCCINUMBER             : return "NUMBER";
    case oracle::occi::OCCIBLOB               : return "BLOB";
    case oracle::occi::OCCIBYTES              : return "BYTES";
    case oracle::occi::OCCICLOB               : return "CLOB";
    case oracle::occi::OCCIVECTOR             : return "VECTOR";
    case oracle::occi::OCCIMETADATA           : return "METADATA";
    case oracle::occi::OCCIPOBJECT            : return "POBJECT";
    case oracle::occi::OCCIREF                : return "REF";
    case oracle::occi::OCCIREFANY             : return "REFANY";
    case oracle::occi::OCCISTRING             : return "STRING";
    case oracle::occi::OCCISTREAM             : return "STREAM";
    case oracle::occi::OCCIDATE               : return "DATE";
    case oracle::occi::OCCIINTERVALDS         : return "INTERVALDS";
    case oracle::occi::OCCIINTERVALYM         : return "INTERVALYM";
    case oracle::occi::OCCITIMESTAMP          : return "TIMESTAMP";
    case oracle::occi::OCCIROWID              : return "ROWID";
    case oracle::occi::OCCICURSOR             : return "CURSOR";
    // duplicated numbers
    // case OCCIBFILE              : cout << "BFILE"  << '\n' ; break;
    // case OCCI_SQLT_BFILE        : cout << "OCCI_SQLT_BFILE        "  << '\n' ; break;
    // case OCCI_SQLT_CFILE        : cout << "OCCI_SQLT_CFILE        "  << '\n' ; break;
    // case OCCI_SQLT_BFILEE       : cout << "OCCI_SQLT_BFILEE       "  << '\n' ; break;
    // case OCCI_SQLT_CFILEE       : cout << "OCCI_SQLT_CFILEE       "  << '\n' ; break;
    default: return "";
    }
}

std::string coral::ora_dbm::bind_param_string(size_t n, size_t offset)
{
    std::ostringstream oss;
    if (n > 0) {
        oss << ':' << (1 + offset);
        for (size_t i = 2 + offset; i <= n + offset; i++) {
            oss << ", :" << i;
        }
    }
    return oss.str();
}
