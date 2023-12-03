/*!
    \file       ora_dbm.h
    \brief      Oracle Database management base class
    \details    support Oracle OCCI
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_ORA_DBM_H__
#define __CORAL_ORA_DBM_H__

#include "types.h"
#include "thread_lock.h"

extern std::string gv_app_name;

//! Core Library for Applications and Libraries
namespace coral {
    //! oracle database manapulator
    class ora_dbm {
    public:
        // copy construct & assignment operator (사용하지 않는다. 대입 복사 불가).
        //! copy construct
        ora_dbm(const ora_dbm& d) = delete;
        //! assignment operator
        ora_dbm& operator=(const ora_dbm& d) = delete;
        //!default construct
        ora_dbm() = default;
        /*! construct overloading
            \param user user
            \param password password
            \param db_name database service name
            \param mode oracle environment
        */
        ora_dbm(const std::string& user,
              const std::string& password,
              const std::string& db_name,
              oracle::occi::Environment::Mode mode = oracle::occi::Environment::THREADED_MUTEXED);
        //!default destruct
        virtual ~ora_dbm();
        /*! get table's meta information
            \return oracle::occi::MetaData
        */
        oracle::occi::MetaData get_metadata(const std::string& table_name);
        /*! print metadata to output stream
            \param os output stream
            \param data oracle::occi::MetaData
        */
        void print_metadata(std::ostream& os, const oracle::occi::MetaData& data);
        /*! to excute function for literal query, delete, insert,
            \param query query sentence
        */
        void execute_query(const std::string& query);
        /*! to excute function for literal query, select only
            \param query query sentence
            \param db_data_set
        */
        void execute_query(const std::string& query, coral::db_data_set_t& db_data_set);
        /*! get data from resultset which is a cursor
            \param rst object of the oracle ResultSet
            \param db_data_set db_data_set_t
        */
        void get_result(oracle::occi::ResultSet& rst, coral::db_data_set_t& db_data_set);
        /*! to excute function for literal query, select only
            \param query query sentence
            \param db_any_data_set database any data set
        */
        void execute_query(const std::string& query, coral::db_any_data_set_t& db_any_data_set);
        /*! get data from resultset which is a cursor
            \param rst object of the oracle ResultSet
            \param db_any_data_set database any data set
        */
        void get_result(oracle::occi::ResultSet& rst, coral::db_any_data_set_t& db_any_data_set);
    protected:
        /*! connect to database
            \param user database user name
            \param password password of user
            \param db_name tnsname
            \param mode environment mode of oracle database
        */
        void connect_to_db(const std::string& user, const std::string& password, const std::string& db_name, oracle::occi::Environment::Mode mode);
        //! disconnect from database
        void disconnect_from_db();

        oracle::occi::Environment*  environment_p_ = nullptr;   ///< oracle environment
        oracle::occi::Connection*   connection_p_  = nullptr;   ///< oracle connection

        /*! get a string from converted column value in ResultSet
            \param rst ResultSet
            \param pos col position
            \param type metadata type
            \return a string, after convert every value to std::string
        */
        std::string value_to_string(oracle::occi::ResultSet& rst, size_t pos, int type);
        /*! blob to std::string
            \param blob oracle::occi::Blob class reference
            \return string, get from blob
        */
        std::string blob_to_string(oracle::occi::Blob blob);
        /*! clob to std::string
            \param clob oracle::occi::Clob class reference
            \return string, get from clob
        */
        std::string clob_to_string(oracle::occi::Clob clob);

        /*! set parameter size to Statment
            \param stmt Statement
            \param metadata MetaData container
        */
        void set_max_param_size(oracle::occi::Statement& stmt, const coral::db_any_data_set_t::col_meta_t& metadata);
        /*! set param value to Stament
            \param stmt Statement
            \param metadata MetaData container
            \param records std::vector<extlib::any>
        */
        void set_param_value(oracle::occi::Statement& stmt, const coral::db_any_data_set_t::col_meta_t& metadata, const coral::db_any_data_set_t::row_record_t& records);
        /*!
        */
        std::string any_ora_type_to_string(const extlib::any& val, int type);
        /*!
        */
        extlib::any value_to_any(oracle::occi::ResultSet& rst, size_t pos, int type);
        /*! get column's data type name
            \param type oracle::occi::MetaData::ATTR_NAME
            \return oracle data type name string
        */
        std::string get_col_data_type_name(int type);
        /*! bind parameter to std::string
            \param n parameters size
            \param offset start + offset
            \return string, if n is 5 than ":1, :2, :3, :4, :5"
        */
        std::string bind_param_string(size_t n, size_t offset = 0);
    };//end ora_dbm class
}// end coral namespace

#endif //__CORAL_ORA_DBM_H__
