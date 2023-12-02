/*!
    \file       file_trans.h
    \brief      File Trans - using ftp library
    \details    not support multiprocess, a file transfer through ftp port
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_FILE_TRANSFER_H__
#define __CORAL_FILE_TRANSFER_H__

#include "types.h"
#include "curl/curl.h"

#include <string>
#include <memory>

//! Core Library for Applications and Libraries
namespace coral{
    //! file info for getting a file
    typedef struct _tag_ftp_file {
        const char *file_name;
        FILE *file;
    } ftp_file_info_t;
    //! enumeration of transfer command
    enum class FILE_TRANS_MODE{
        PUT   ///< put
        , GET ///< get
    };
    //! File Trans Class
    /*!
        a file transfer to a destination host using ftp default port
    */
    class file_trans{
    public:
        //! single file trans construct
        /*!
            \param ftp_info Host and file Information
            \param trans_mode file trans mode, default put
            \param verbose display ftp result
        */
        explicit file_trans(const ftp_info_t& ftp_info, FILE_TRANS_MODE trans_mode = FILE_TRANS_MODE::PUT, long verbose = 0L);
        //! default destruct
        ~file_trans();
        /*!
            \return if true is done, otherwise false
        */
        bool trans_file();
        /*! parse headers for Content-Length */
        static size_t get_content_length(void *ptr, size_t size, size_t nmemb, void *stream);
        /*! discard downloaded data */
        static size_t discard_stream(void *ptr, size_t size, size_t nmemb, void *stream);
        /*! read data to upload */
        static size_t read(void *ptr, size_t size, size_t nmemb, void *stream);
        /*! write file from download */
        static size_t write(void *buffer, size_t size, size_t nmemb, void *stream);
        /*! upload */
        static bool upload(const std::string& remote_file, const std::string& local_file, long timeout, long tries);
        /*! download */
        static bool download(const std::string& remote_file, ftp_file_info_t& ftp_file_info);
        /*! CURL Handler */
        static CURL* curl_handle_;
        static long verbose_;  ///< if 1L then verbose ftp log to stdout else 0L then Nothing
    private:
        /*! initialize variables */
        void init();
        // private variables
        ftp_info_t      ftp_info_;   ///< destnation host inforamtion
        FILE_TRANS_MODE trans_mode_; ///< transfer command
    };
}

#endif //__CORAL_FILE_TRANSFER_H__
