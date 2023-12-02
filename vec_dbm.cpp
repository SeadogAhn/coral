/*!
    \file       vec_dbm.cpp
    \brief      Actian Vector Database management class
    \details    support ingres IIAPI
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "vec_dbm.h"
#include "defines.h"
#include "variables.h"
#include "exception.h"
#include "log_manager.h"

#include <cstdio>
#include <stdexcept>
#include <cstring>

using namespace std;

extern std::string gv_app_name;

coral::vec_dbm::vec_dbm(const std::string& db, const std::string& user, const std::string& password) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    db_ = db;
    user_ = user;
    password_ = password;

    env_handle_  = nullptr;
    conn_handle_ = nullptr;
    tran_handle_ = nullptr;
    stmt_handle_ = nullptr;

    init();
    connect(db_, user_, password_);
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

coral::vec_dbm::~vec_dbm() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    disconn();
    terminate();
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::select(const std::string& strQuery, std::ostream& os) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << '(' << strQuery << ',' << &os << "):";
    string strMsg;

    try {
        CORAL_D_TRACE("execute: %s\n", strQuery.c_str());
        // Call IIapi_query to execute statement.
        exec_statement(strQuery);
        // get query result descriptors
        get_descriptor(os);
        // get query results
        get_columns(os);
        // close statement
        close();
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::select(const std::string& strQuery, coral::db_data_set_t& db_data_set) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << '(' << strQuery << ',' << &db_data_set << "):";
    string strMsg;

    try {
        CORAL_D_TRACE("execute: %s\n", strQuery.c_str());
        // Call IIapi_query to execute statement.
        exec_statement(strQuery);
        // get query result descriptors
        get_descriptor(db_data_set.col_names);
        // get query results
        get_columns(db_data_set.data_container);
        // close statement
        close();
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::query(const std::string& strQuery) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << '(' << strQuery << "):";
    string strMsg;

    try {
        CORAL_D_TRACE("execute:%s\n", strQuery.c_str());
        // Call IIapi_query to execute statement.
        exec_statement(strQuery);
        // query info
        query_info();
        // close statement
        close();
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::insert(const std::string& strQuery, coral::db_data_set_t& db_data_set) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << '(' << strQuery << "):";
    string strMsg;

    try {
        CORAL_D_TRACE("execute:%s\n", strQuery.c_str());
        // Call IIapi_query to execute statement.
        exec_statement(strQuery);
        // query info
        query_info();
        // close statement
        close();
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}


void coral::vec_dbm::exec_statement(const std::string& strQuery) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << &strQuery << "):";
    try {
        // Call IIapi_query to execute statement.
        query_parm_.qy_genParm.gp_callback = nullptr;
        query_parm_.qy_genParm.gp_closure  = nullptr;
        query_parm_.qy_connHandle          = conn_handle_;
        query_parm_.qy_queryType           = IIAPI_QT_QUERY;
        query_parm_.qy_queryText           = const_cast<II_CHAR*>(strQuery.c_str());
        query_parm_.qy_flags               = 0;
        query_parm_.qy_parameters          = FALSE;
        query_parm_.qy_tranHandle          = tran_handle_;
        query_parm_.qy_stmtHandle          = nullptr;

        IIapi_query(&query_parm_);
        wait(query_parm_.qy_genParm);
        check_error(query_parm_.qy_genParm);
        // Return transaction handle.
        tran_handle_ = query_parm_.qy_tranHandle;
        stmt_handle_ = query_parm_.qy_stmtHandle;
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::query_info() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << &get_qinfo_parm_ << "):";
    try {
        get_qinfo_parm_.gq_genParm.gp_callback = nullptr;
        get_qinfo_parm_.gq_genParm.gp_closure = nullptr;
        get_qinfo_parm_.gq_stmtHandle = stmt_handle_;
        IIapi_getQueryInfo(&get_qinfo_parm_);
        wait(get_qinfo_parm_.gq_genParm);
        check_qinfo();
        check_error(get_qinfo_parm_.gq_genParm);
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::get_descriptor(std::ostream& os) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << &get_descr_parm_ << ',' << &os << "):";
    try {
        get_descr_parm_.gd_genParm.gp_callback = NULL;
        get_descr_parm_.gd_genParm.gp_closure  = NULL;
        get_descr_parm_.gd_stmtHandle          = stmt_handle_;
        get_descr_parm_.gd_descriptorCount     = 0;
        get_descr_parm_.gd_descriptor          = NULL;

        IIapi_getDescriptor(&get_descr_parm_);
        wait(get_descr_parm_.gd_genParm);
        check_error(get_descr_parm_.gd_genParm);

        CORAL_D_TRACE("get_descr_parm_.gd_descriptorCount:%d\n", get_descr_parm_.gd_descriptorCount);
        CORAL_D_TRACE("ds_dataType,ds_nullable,ds_length,ds_precision,ds_scale,ds_columnType,ds_columnName\n");
        ostringstream ostrstr;
        ostrstr << get_descr_parm_.gd_descriptor[0].ds_columnName;
        for (long i = 1; i < get_descr_parm_.gd_descriptorCount; i++) {
            ostrstr << ',' << get_descr_parm_.gd_descriptor[i].ds_columnName;
            CORAL_D_TRACE("%d,%d,%d,%d,%d,%d,%s\n"
                        , get_descr_parm_.gd_descriptor[i].ds_dataType
                        , get_descr_parm_.gd_descriptor[i].ds_nullable
                        , get_descr_parm_.gd_descriptor[i].ds_length
                        , get_descr_parm_.gd_descriptor[i].ds_precision
                        , get_descr_parm_.gd_descriptor[i].ds_scale
                        , get_descr_parm_.gd_descriptor[i].ds_columnType
                        , get_descr_parm_.gd_descriptor[i].ds_columnName
                       );
        }
        os << ostrstr.str();
        os << endl;
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::get_columns(std::ostream& os) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << &get_col_parm_ << ',' << &get_descr_parm_ << ',' << &os << "):";
    try {
        char* pValue[get_descr_parm_.gd_descriptorCount];
        IIAPI_DATAVALUE DataBuffer[get_descr_parm_.gd_descriptorCount];
        get_col_parm_.gc_genParm.gp_callback = NULL;
        get_col_parm_.gc_genParm.gp_closure  = NULL;
        get_col_parm_.gc_rowCount            = 1;
        get_col_parm_.gc_columnCount         = get_descr_parm_.gd_descriptorCount;
        get_col_parm_.gc_rowsReturned        = 0;
        get_col_parm_.gc_stmtHandle          = stmt_handle_;
        get_col_parm_.gc_moreSegments        = 0;
        get_col_parm_.gc_columnData          = DataBuffer;
        //
        for (long i = 0; i < get_descr_parm_.gd_descriptorCount; i++) {
            pValue[i] = new char[get_descr_parm_.gd_descriptor[i].ds_length];
            get_col_parm_.gc_columnData[i].dv_value = pValue[i];
        }

        coral::elapsed_time et1, et2;
        size_t nRowCnt = 0;
        size_t nStreamBufferSize = 10000;
        ostringstream ostrstr;
        do {
            et2.reset();
            IIapi_getColumns(&get_col_parm_);
            wait(get_col_parm_.gc_genParm);
            if (get_col_parm_.gc_genParm.gp_status >= IIAPI_ST_NO_DATA) break;
            ostrstr << convert_data_to_string(get_descr_parm_.gd_descriptor[0], get_col_parm_.gc_columnData[0]);
            for (long i = 1; i < get_descr_parm_.gd_descriptorCount; i++) {
                if (get_col_parm_.gc_columnData[i].dv_null) {
                    ostrstr << ',';
                }
                else {
                    ostrstr << ',' << convert_data_to_string(get_descr_parm_.gd_descriptor[i], get_col_parm_.gc_columnData[i]);
                }
            }
            ostrstr << endl;
            if (nRowCnt % nStreamBufferSize == 0) {
                os << ostrstr.str();
                ostrstr.str(std::string());
                CORAL_D_TRACE("collected data count:%d, elapsed time:%lfms\n", nRowCnt, et2.msec());
            }
            nRowCnt++;
        } while (true);
        // output to ostream
        os << ostrstr.str();
        // free pValue[]
        for (long i = 0; i < get_descr_parm_.gd_descriptorCount; i++) {
            delete [] pValue[i];
        }
        CORAL_D_TRACE("collected data count:%d, elapsed time:%lfs\n", nRowCnt, et1.sec());
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::get_descriptor(coral::db_data_set_t::col_field_t& col_names) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << &get_descr_parm_ << ',' << &col_names << "):";
    try {
        get_descr_parm_.gd_genParm.gp_callback = NULL;
        get_descr_parm_.gd_genParm.gp_closure  = NULL;
        get_descr_parm_.gd_stmtHandle          = stmt_handle_;
        get_descr_parm_.gd_descriptorCount     = 0;
        get_descr_parm_.gd_descriptor          = NULL;

        IIapi_getDescriptor(&get_descr_parm_);
        wait(get_descr_parm_.gd_genParm);
        check_error(get_descr_parm_.gd_genParm);

        CORAL_D_TRACE("get_descr_parm_.gd_descriptorCount:%d\n", get_descr_parm_.gd_descriptorCount);
        CORAL_D_TRACE("ds_dataType,ds_nullable,ds_length,ds_precision,ds_scale,ds_columnType,ds_columnName\n");
        col_names.resize(get_descr_parm_.gd_descriptorCount);
        for (long i = 0; i < get_descr_parm_.gd_descriptorCount; i++) {
            col_names[i] = get_descr_parm_.gd_descriptor[i].ds_columnName;
            CORAL_D_TRACE("%d,%d,%d,%d,%d,%d,%s\n"
                        , get_descr_parm_.gd_descriptor[i].ds_dataType
                        , get_descr_parm_.gd_descriptor[i].ds_nullable
                        , get_descr_parm_.gd_descriptor[i].ds_length
                        , get_descr_parm_.gd_descriptor[i].ds_precision
                        , get_descr_parm_.gd_descriptor[i].ds_scale
                        , get_descr_parm_.gd_descriptor[i].ds_columnType
                        , get_descr_parm_.gd_descriptor[i].ds_columnName
                       );
        }
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::get_columns(coral::db_data_set_t::data_container_t& data_container) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "(" << &get_col_parm_ << ',' << &get_descr_parm_ << ',' << &data_container << "):";
    try {
        char* pValue[get_descr_parm_.gd_descriptorCount];
        IIAPI_DATAVALUE DataBuffer[get_descr_parm_.gd_descriptorCount];
        get_col_parm_.gc_genParm.gp_callback = NULL;
        get_col_parm_.gc_genParm.gp_closure  = NULL;
        get_col_parm_.gc_rowCount            = 1;
        get_col_parm_.gc_columnCount         = get_descr_parm_.gd_descriptorCount;
        get_col_parm_.gc_rowsReturned        = 0;
        get_col_parm_.gc_stmtHandle          = stmt_handle_;
        get_col_parm_.gc_moreSegments        = 0;
        get_col_parm_.gc_columnData          = DataBuffer;
        //
        for (long i = 0; i < get_descr_parm_.gd_descriptorCount; i++) {
            pValue[i] = new char[get_descr_parm_.gd_descriptor[i].ds_length];
            get_col_parm_.gc_columnData[i].dv_value = pValue[i];
        }

        coral::elapsed_time et1, et2;
        size_t nRowCnt = 0;
        do {
            et2.reset();
            IIapi_getColumns(&get_col_parm_);
            wait(get_col_parm_.gc_genParm);
            if (get_col_parm_.gc_genParm.gp_status >= IIAPI_ST_NO_DATA) break;
            coral::db_data_set_t::col_field_t DataSet(get_descr_parm_.gd_descriptorCount);
            for (long i = 0; i < get_descr_parm_.gd_descriptorCount; i++) {
                if (get_col_parm_.gc_columnData[i].dv_null) {
                    DataSet[i] = "";
                }
                else {
                    DataSet[i] = convert_data_to_string(get_descr_parm_.gd_descriptor[i], get_col_parm_.gc_columnData[i]);
                }
            }
            data_container.push_back(DataSet);
            nRowCnt++;
        } while (true);
        // free pValue[]
        for (long i = 0; i < get_descr_parm_.gd_descriptorCount; i++) {
            delete [] pValue[i];
        }
        CORAL_D_TRACE("collected data count:%d, elapsed time:%lfs\n", data_container.size(), et1.sec());
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

std::string coral::vec_dbm::convert_data_to_string(IIAPI_DESCRIPTOR& Desc, IIAPI_DATAVALUE& ColData) noexcept {
    string strValue;

    switch (Desc.ds_dataType) {
    case IIAPI_BYTE_TYPE:
    case IIAPI_CHA_TYPE:
    case IIAPI_CHR_TYPE:
    case IIAPI_LBYTE_TYPE:
    case IIAPI_LVCH_TYPE:
    case IIAPI_LTXT_TYPE:
    case IIAPI_LOGKEY_TYPE:
    case IIAPI_TXT_TYPE:
        strValue.resize(ColData.dv_length);
        memcpy(&strValue[0], (unsigned char*)ColData.dv_value, ColData.dv_length);
        break;
    case IIAPI_VBYTE_TYPE:
    case IIAPI_VCH_TYPE:
        strValue.resize(ColData.dv_length-2);
        memcpy(&strValue[0], (unsigned char*)ColData.dv_value+2, ColData.dv_length-2);
        break;
    case IIAPI_LNVCH_TYPE:
    case IIAPI_NCHA_TYPE:
    case IIAPI_NVCH_TYPE:
        strValue.resize(ColData.dv_length*2-2);
        memcpy(&strValue[0], (unsigned char*)ColData.dv_value+2, ColData.dv_length*2-2);
        break;
    case IIAPI_FLT_TYPE: {
            if (Desc.ds_length == 4) {
                float f = 0.;
                memcpy(&f, ColData.dv_value, Desc.ds_length);
                strValue = to_string(f);
            }
            else if (Desc.ds_length == 8) {
                double d = 0.;
                memcpy(&d, ColData.dv_value, Desc.ds_length);
                strValue = to_string(d);
            }
        }
        break;
    case IIAPI_HNDL_TYPE: {
            unsigned int n = 0;
            memcpy(&n, ColData.dv_value, Desc.ds_length);
            strValue = to_string(n);
        }
        break;
    case IIAPI_BOOL_TYPE:
    case IIAPI_INT_TYPE: {
            long n = 0;
            memcpy(&n, ColData.dv_value, Desc.ds_length);
            strValue = to_string(n);
        }
        break;
    case IIAPI_DTE_TYPE:
    case IIAPI_DATE_TYPE:
    case IIAPI_INTDS_TYPE:
    case IIAPI_TIME_TYPE:
    case IIAPI_TMTZ_TYPE:
    case IIAPI_TMWO_TYPE:
    case IIAPI_TS_TYPE:
    case IIAPI_TSTZ_TYPE:
    case IIAPI_TSWO_TYPE:
        strValue = convert_date_time_to_string(Desc, ColData.dv_value);
        break;
    case IIAPI_DEC_TYPE:
    case IIAPI_INTYM_TYPE:
    case IIAPI_IPV4_TYPE:
    case IIAPI_IPV6_TYPE:
    case IIAPI_LBLOC_TYPE:
    case IIAPI_LCLOC_TYPE:
    case IIAPI_LNLOC_TYPE:
    case IIAPI_MNY_TYPE:
    case IIAPI_TABKEY_TYPE:
    default:
        break;
    } // switch

    return strValue;
}

std::string coral::vec_dbm::convert_date_time_to_string(IIAPI_DESCRIPTOR& Desc, II_PTR pValue) noexcept {
    string strValue;
    strValue.resize(19);

    IIAPI_SETENVPRMPARM setEnvPrmParm;
    IIAPI_CONVERTPARM   convertParm;
    II_LONG             paramvalue;

    setEnvPrmParm.se_envHandle = env_handle_;
    setEnvPrmParm.se_paramID = IIAPI_EP_DATE_FORMAT;
    setEnvPrmParm.se_paramValue = (II_PTR)&paramvalue;
    paramvalue = IIAPI_EPV_DFRMT_YMD;

    IIapi_setEnvParam(&setEnvPrmParm);

    convertParm.cv_srcDesc.ds_dataType  = Desc.ds_dataType;
    convertParm.cv_srcDesc.ds_nullable  = FALSE;
    convertParm.cv_srcDesc.ds_length  = Desc.ds_length;
    convertParm.cv_srcDesc.ds_precision = 0;
    convertParm.cv_srcDesc.ds_scale = 0;
    convertParm.cv_srcDesc.ds_columnType = IIAPI_COL_QPARM;
    convertParm.cv_srcDesc.ds_columnName = NULL;

    convertParm.cv_srcValue.dv_null = FALSE;
    convertParm.cv_srcValue.dv_length = Desc.ds_length;
    convertParm.cv_srcValue.dv_value = pValue;

    convertParm.cv_dstDesc.ds_dataType = IIAPI_CHA_TYPE;
    convertParm.cv_dstDesc.ds_nullable = FALSE;
    convertParm.cv_dstDesc.ds_length = strValue.size();
    convertParm.cv_dstDesc.ds_precision = 0;
    convertParm.cv_dstDesc.ds_scale = 0;
    convertParm.cv_dstDesc.ds_columnType = IIAPI_COL_QPARM;
    convertParm.cv_dstDesc.ds_columnName = NULL;

    convertParm.cv_dstValue.dv_null = FALSE;
    convertParm.cv_dstValue.dv_length = strValue.size();
    convertParm.cv_dstValue.dv_value = &strValue[0];

    IIapi_convertData(&convertParm);

    return strValue;
}

void coral::vec_dbm::init() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    IIAPI_INITPARM  initParm;
    initParm.in_version = IIAPI_VERSION_7;
    initParm.in_timeout = -1;
    IIapi_initialize(&initParm);
    env_handle_ = initParm.in_envHandle;
    CORAL_D_TRACE("%p, %p\n", env_handle_, initParm.in_envHandle);
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::terminate() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    IIAPI_RELENVPARM relParm;
    IIAPI_TERMPARM termParm;
    relParm.re_envHandle = env_handle_;
    IIapi_releaseEnv(&relParm);
    IIapi_terminate(&termParm);
    env_handle_ = nullptr;
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

IIAPI_STATUS coral::vec_dbm::connect(const std::string& dbname, const std::string& user, const std::string& pass) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << '(' << dbname << ',' << user << "):";

    IIAPI_CONNPARM connParm;
    try {
        // init db name & account
        if (!dbname.empty()) db_ = dbname;
            if (!user.empty()) user_ = user;
                if (!pass.empty()) password_ = pass;

        connParm.co_genParm.gp_callback = nullptr;
        connParm.co_genParm.gp_closure = nullptr;
        connParm.co_type = IIAPI_CT_SQL;
        connParm.co_target = const_cast<II_CHAR*>(db_.c_str());
        connParm.co_connHandle = env_handle_;
        connParm.co_tranHandle = nullptr;
        connParm.co_username = const_cast<II_CHAR*>(user_.c_str());
        connParm.co_password = const_cast<II_CHAR*>(password_.c_str());
        connParm.co_timeout = -1;

        IIapi_connect(&connParm);
        wait(connParm.co_genParm);
        check_error(connParm.co_genParm);

        if (connParm.co_genParm.gp_status == IIAPI_ST_SUCCESS) {
            conn_handle_ = connParm.co_connHandle;
            CORAL_D_TRACE("%p, %p\n", conn_handle_, connParm.co_connHandle);
        }
    }
    catch (const coral::exception& error) {
        if (connParm.co_connHandle) abort();
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        if (connParm.co_connHandle) abort();
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        if (connParm.co_connHandle) abort();
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
    return connParm.co_genParm.gp_status;
}

IIAPI_STATUS coral::vec_dbm::disconn() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";

    IIAPI_DISCONNPARM disconnParm;
    try {
        disconnParm.dc_genParm.gp_callback = nullptr;
        disconnParm.dc_genParm.gp_closure = nullptr;
        disconnParm.dc_connHandle = conn_handle_;
        IIapi_disconnect(&disconnParm);
        wait(disconnParm.dc_genParm);
        check_error(disconnParm.dc_genParm);
        conn_handle_ = nullptr;
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
    return disconnParm.dc_genParm.gp_status;
}

//! cancel query
void coral::vec_dbm::cancel() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    string strMsg;

    try {
        IIAPI_CANCELPARM cancelParm;
        cancelParm.cn_genParm.gp_callback = nullptr;
        cancelParm.cn_genParm.gp_closure = nullptr;
        cancelParm.cn_stmtHandle = stmt_handle_;
        IIapi_cancel(&cancelParm);
        wait(cancelParm.cn_genParm);
        check_error(cancelParm.cn_genParm);
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::rollback() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    try {
        IIAPI_ROLLBACKPARM  rollbackParm;
        rollbackParm.rb_genParm.gp_callback = nullptr;
        rollbackParm.rb_genParm.gp_closure = nullptr;
        rollbackParm.rb_tranHandle = tran_handle_;
        rollbackParm.rb_savePointHandle = nullptr;
        IIapi_rollback(&rollbackParm);
        wait(rollbackParm.rb_genParm);
        check_error(rollbackParm.rb_genParm);
        tran_handle_ = nullptr;
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::commit() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    try {
        IIAPI_COMMITPARM commitParm;
        commitParm.cm_genParm.gp_callback = nullptr;
        commitParm.cm_genParm.gp_closure = nullptr;
        commitParm.cm_tranHandle = tran_handle_;
        IIapi_commit(&commitParm);
        wait(commitParm.cm_genParm);
        check_error(commitParm.cm_genParm);
        tran_handle_ = nullptr;
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::autocommit_on() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    try {
        IIAPI_AUTOPARM  autoparm;
        autoparm.ac_genParm.gp_closure  = NULL;
        autoparm.ac_genParm.gp_callback = NULL;
        autoparm.ac_connHandle = conn_handle_;
        autoparm.ac_tranHandle = NULL;

        IIapi_autocommit(&autoparm);
        wait(autoparm.ac_genParm);
        check_error(autoparm.ac_genParm);

        tran_handle_ = autoparm.ac_tranHandle;
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::autocommit_off() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";
    try {
        IIAPI_AUTOPARM  autoparm;
        autoparm.ac_genParm.gp_closure  = NULL;
        autoparm.ac_genParm.gp_callback = NULL;
        autoparm.ac_connHandle = NULL;
        autoparm.ac_tranHandle = tran_handle_;

        IIapi_autocommit(&autoparm);
        wait(autoparm.ac_genParm);
        check_error(autoparm.ac_genParm);
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::abort() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";

    try {
        IIAPI_ABORTPARM abortParm;
        abortParm.ab_genParm.gp_callback = nullptr;
        abortParm.ab_genParm.gp_closure  = nullptr;
        abortParm.ab_connHandle = conn_handle_;
        IIapi_abort(&abortParm);
        wait(abortParm.ab_genParm);
        check_error(abortParm.ab_genParm);
        conn_handle_ = nullptr;
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::close() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << "():";

    try {
        // free query resources
        IIAPI_CLOSEPARM closeParm;
        closeParm.cl_genParm.gp_callback = nullptr;
        closeParm.cl_genParm.gp_closure  = nullptr;
        closeParm.cl_stmtHandle          = stmt_handle_;
        IIapi_close(&closeParm);
        wait(closeParm.cl_genParm);
        check_error(closeParm.cl_genParm);
    }
    catch (const coral::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (const std::exception& error) {
        log_manager::write(gv_app_name, method_info.str() + error.what());
        throw error;
    }
    catch (...) {
        log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
        throw unknown_error(method_info.str() + CORAL_D_STRMSG(EN, ERR, 000010));
    }
    log_manager::write(gv_app_name, method_info.str() + CORAL_D_STRMSG(EN, STR, 000004));
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::check_error(IIAPI_GENPARM& genParm) {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << '(' << genParm.gp_status << "):";

    if (genParm.gp_status !=  IIAPI_ST_SUCCESS) {
        /*
        ** Print (bad) status.
        */
        string strErrMsg, strMsg;
        if (genParm.gp_status >= IIAPI_ST_ERROR) {
            char val[33];
            switch(genParm.gp_status)
            {
                case IIAPI_ST_ERROR :           strMsg = "IIAPI_ST_ERROR";           break;
                case IIAPI_ST_FAILURE :         strMsg = "IIAPI_ST_FAILURE";         break;
                case IIAPI_ST_NOT_INITIALIZED : strMsg = "IIAPI_ST_NOT_INITIALIZED"; break;
                case IIAPI_ST_INVALID_HANDLE :  strMsg = "IIAPI_ST_INVALID_HANDLE";  break;
                case IIAPI_ST_OUT_OF_MEMORY :   strMsg = "IIAPI_ST_OUT_OF_MEMORY";   break;
                default :
                    sprintf(val, "%d", genParm.gp_status);
                    strMsg = val;
                    break;
            }

            print_string(strErrMsg, 1024, "status = %s", strMsg.c_str());
            method_info << strErrMsg << ',';

            CORAL_D_TRACE("\tstatus = %s\n", strMsg.c_str());
        }

        if (genParm.gp_errorHandle) {
            IIAPI_GETEINFOPARM  getErrParm;
            /*
            ** Provide initial error handle.
            */
            getErrParm.ge_errorHandle = genParm.gp_errorHandle;

            /*
            ** Call IIapi_getErrorInfo() in loop until no data.
            */
            do
            {
                IIapi_getErrorInfo(&getErrParm);
                if (getErrParm.ge_status != IIAPI_ST_SUCCESS)
                    break;

                /*
                ** Print error message info.
                */
                switch(getErrParm.ge_type) {
                    case IIAPI_GE_ERROR :   strMsg = "ERROR";           break;
                    case IIAPI_GE_WARNING : strMsg = "WARNING";     break;
                    case IIAPI_GE_MESSAGE : strMsg = "USER MESSAGE";    break;
                    default :               strMsg = "?";               break;
                }
                print_string(strErrMsg, 1024, "Info:%s %s 0x%x:%s", strMsg.c_str(), getErrParm.ge_SQLSTATE, getErrParm.ge_errorCode, getErrParm.ge_message ? getErrParm.ge_message : "NULL");
                method_info << strErrMsg << ',';

                CORAL_D_TRACE("\t%s\n", strErrMsg.c_str());
            } while(1); /* next message */
        }
        throw database_error(method_info.str());
    } // genParm.gp_status !=  IIAPI_ST_SUCCESS
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::check_qinfo() {
    CORAL_D_CLASS_MEMBER_FUNC_START;
    ostringstream method_info;
    method_info << CORAL_D_METHOD_INFO << '(' << get_qinfo_parm_.gq_flags << ',' << get_qinfo_parm_.gq_mask << "):";

    string strMsg;
    /*
    ** Check query result flags.
    */
    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_FAIL) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_FAIL\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_FAIL");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_ALL_UPDATED) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_ALL_UPDATED\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_ALL_UPDATED");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_NULLS_REMOVED) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_NULLS_REMOVED\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_NULLS_REMOVED");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_UNKNOWN_REPEAT_QUERY) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_UNKNOWN_REPEAT_QUERY\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_UNKNOWN_REPEAT_QUERY");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_END_OF_DATA) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_END_OF_DATA\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_END_OF_DATA");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_CONTINUE) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_CONTINUE\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_CONTINUE");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_INVALID_STATEMENT) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_INVALID_STATEMENT\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_INVALID_STATEMENT");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_TRANSACTION_INACTIVE) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_TRANSACTION_INACTIVE\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_TRANSACTION_INACTIVE");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_OBJECT_KEY) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_OBJECT_KEY\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_OBJECT_KEY");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_TABLE_KEY) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_TABLE_KEY\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_TABLE_KEY");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_NEW_EFFECTIVE_USER) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_NEW_EFFECTIVE_USER\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_NEW_EFFECTIVE_USER");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_FLUSH_QUERY_ID) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_FLUSH_QUERY_ID\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_FLUSH_QUERY_ID");
    }

    if (get_qinfo_parm_.gq_flags & IIAPI_GQF_ILLEGAL_XACT_STMT) {
        CORAL_D_TRACE("\tflag = IIAPI_GQF_ILLEGAL_XACT_STMT\n");
        log_manager::write(gv_app_name, method_info.str() + "flag = IIAPI_GQF_ILLEGAL_XACT_STMT");
    }

    /*
    ** Check query result values.
    */
    if (get_qinfo_parm_.gq_mask & IIAPI_GQ_ROW_COUNT) {
        print_string(strMsg, 256, "row count = %d", get_qinfo_parm_.gq_rowCount);
        CORAL_D_TRACE("\t%s\n", strMsg.c_str());
        log_manager::write(gv_app_name, method_info.str() + strMsg);
    }

    if (get_qinfo_parm_.gq_mask & IIAPI_GQ_CURSOR) {
        CORAL_D_TRACE("\treadonly = TRUE\n");
        log_manager::write(gv_app_name, method_info.str() + "readonly = TRUE");
    }

    if (get_qinfo_parm_.gq_mask & IIAPI_GQ_PROCEDURE_RET) {
        print_string(strMsg, 256, "procedure return = %d", get_qinfo_parm_.gq_procedureReturn);
        CORAL_D_TRACE("\t%s\n", strMsg.c_str());
        log_manager::write(gv_app_name, method_info.str() + strMsg);
    }

    if (get_qinfo_parm_.gq_mask & IIAPI_GQ_PROCEDURE_ID) {
        print_string(strMsg, 256, "procedure handle = 0x%p", get_qinfo_parm_.gq_procedureHandle);
        CORAL_D_TRACE("\t%s\n", strMsg.c_str());
        log_manager::write(gv_app_name, method_info.str() + strMsg);
    }

    if (get_qinfo_parm_.gq_mask & IIAPI_GQ_REPEAT_QUERY_ID) {
        print_string(strMsg, 256, "repeat query ID = 0x%p", get_qinfo_parm_.gq_repeatQueryHandle);
        CORAL_D_TRACE("\t%s\n", strMsg.c_str());
        log_manager::write(gv_app_name, method_info.str() + strMsg);
    }

    if (get_qinfo_parm_.gq_mask & IIAPI_GQ_TABLE_KEY) {
        CORAL_D_TRACE("\treceived table key\n");
        log_manager::write(gv_app_name, method_info.str() + "received table key");
    }

    if (get_qinfo_parm_.gq_mask & IIAPI_GQ_OBJECT_KEY) {
        CORAL_D_TRACE("\treceived object key\n");
        log_manager::write(gv_app_name, method_info.str() + "received object key");
    }
    CORAL_D_CLASS_MEMBER_FUNC_END;
}

void coral::vec_dbm::wait(IIAPI_GENPARM& genParm) {
    IIAPI_WAITPARM waitParm;

    while(genParm.gp_completed == FALSE) {
        waitParm.wt_timeout = -1;
        IIapi_wait(&waitParm);

        if (waitParm.wt_status != IIAPI_ST_SUCCESS)
        {
            genParm.gp_status = waitParm.wt_status;
            break;
        }
    }
}
