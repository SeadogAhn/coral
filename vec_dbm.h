/*!
    \file       vec_dbm.h
    \brief      Actian Vector Database management class
    \details    support ingres IIAPI
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_VEC_DBM_H__
#define __CORAL_VEC_DBM_H__

#include "types.h"
#include "iiapi.h"
#include <string>

//! Vector Database Manipulator namespace
namespace coral {
    struct varchar {
        short length;
        char value[1024];
    };

    //! Vector Database Manipulator Class
    /*
        this class provides functions below
        procedure :
            * constructor
                default construct init only
                * init()                        Initialize API access.
                if oveloaded constructor with dbname use
                * connect()                     connect DBMS
            * something to do                   ~~ blabla
                query() only
            * commit(), rollback() or cancel()  choice one
            * destructor
                * disconn()                     Release DBMS connection.
                * terminate()                   Terminate API access.
    */
    class vec_dbm {
    public:
        /*! overloded constructor \n
            initialize API access and connect to DBMS
            \param dbname example "@12.56.209.124,tcp_ip,VW[user,password]::vectordb"
            \param user user id
            \param pass password
        */
        vec_dbm(const std::string& dbname, const std::string& user = "", const std::string& pass = "");
        /*! destructor \n
            disconnect from DBMS and release API access
        */
        virtual ~vec_dbm();
        //! hide copy constructor & assignment operator
        vec_dbm(const vec_dbm& d) = delete;
        vec_dbm& operator=(const vec_dbm& d) = delete;
        //! select query
        /*!
            \param strQuery query string
            \param os out stream
        */
        virtual void select(const std::string& strQuery, std::ostream& os);
        //! select query
        /*!
            \param strQuery query string
            \param db_data_set output dataset
        */
        virtual void select(const std::string& strQuery, coral::db_data_set_t& db_data_set);
        /*! simple query
            \param strQuery query string
        */
        virtual void query(const std::string& strQuery);
        /*! batch insert
            \param strQuery query string
            \param db_data_set output dataset
        */
        virtual void insert(const std::string& strQuery, coral::db_data_set_t& db_data_set);
        //! cacel query
        void cancel();
        //! rellback
        void rollback();
        //! commit
        void commit();
        //! auto commit
        void autocommit_on();
        //! desable autocommit
        void autocommit_off();
    protected:
        //! execute statement
        /*!
            \param strQuery query
        */
        void exec_statement(const std::string& strQuery);
        //! // query info
        void query_info();
        //! get query result descriptors
        /*!
            \param os out stream
        */
        virtual void get_descriptor(std::ostream& os);
        //! get query result descriptors
        /*!
            \param col_names column names of table data
        */
        virtual void get_descriptor(coral::db_data_set_t::col_field_t& col_names);
        //! get query results
        /*!
            \param os out stream
        */
        virtual void get_columns(std::ostream& os);
        //! get query results
        /*!
            \param data_container data set of table data
        */
        virtual void get_columns(coral::db_data_set_t::data_container_t& data_container);
        //! convert db data type to std::string
        /*!
            \param Desc data type describes
            \param ColData data value of column
            \return converted db data
        */
        std::string convert_data_to_string(IIAPI_DESCRIPTOR& Desc, IIAPI_DATAVALUE& ColData) noexcept;
        //! convert date time to std::string
        /*!
            \param Desc data type describes
            \param pValue void type
            \return date format
        */
        std::string convert_date_time_to_string(IIAPI_DESCRIPTOR& Desc, II_PTR pValue) noexcept;
        //! Open connection with target Database.
        /*!
            \param dbname example "@12.56.189.4,tcp_ip,VW[user,password]::vectordb"
            \param user user id
            \param pass password
        */
        IIAPI_STATUS connect(const std::string& dbname = "", const std::string& user = "", const std::string& pass = "");
        //! release DBMS connection
        IIAPI_STATUS disconn();
        //! initialize API access
        void init();
        //! terminate API access
        void terminate();
        //! connection abort
        void abort();
        //! close, free query resources
        void close();
        //! check QInfo
        void check_qinfo();
        //! wait param
        /*!
            \param genParm generated param
        */
        void wait(IIAPI_GENPARM& genParm);
        //! check error of a generated param
        /*!
            \param genParm generated param
        */
        void check_error(IIAPI_GENPARM& genParm);

        std::string db_;        ///< DB Name
        std::string user_;      ///< user id
        std::string password_;    ///< password

        // handle
        II_PTR  env_handle_;     ///< environment handle
        II_PTR  conn_handle_;    ///< connection handle
        II_PTR  tran_handle_;    ///< transaction handle
        II_PTR  stmt_handle_;    ///< statement handle

        // parameters
        IIAPI_QUERYPARM     query_parm_;        ///< query parameter
        IIAPI_GETDESCRPARM  get_descr_parm_;    ///< descript parameter
        IIAPI_GETCOLPARM    get_col_parm_;      ///< column parameter
        IIAPI_GETQINFOPARM  get_qinfo_parm_;    ///< query info parameter
    };
} // coral namespace

#endif // __CORAL_VEC_DBM_H__
