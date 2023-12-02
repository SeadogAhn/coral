/*!
    \file       file_trans.cpp
    \brief      File Trans - using curl library
    \details    not support multiprocess, a file transfer through ftp port
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "file_trans.h"
#include "config.h"
#include "utility.h"
#include "log_manager.h"
#include "thread_lock.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

extern std::string gv_app_name;


CURL* coral::file_trans::curl_handle_ = NULL;
long coral::file_trans::verbose_ = 0L;

//! default destruct
coral::file_trans::~file_trans() {
    curl_global_cleanup();
}

coral::file_trans::file_trans(const ftp_info_t& ftp_info, FILE_TRANS_MODE trans_mode, long verbose) {
    ftp_info_ = ftp_info;
    trans_mode_ = trans_mode;
    verbose_ = verbose;
    init();
}

void coral::file_trans::init() {
   // global initialization
    curl_global_init(CURL_GLOBAL_ALL);
}

size_t coral::file_trans::get_content_length(void *ptr, size_t size, size_t nmemb, void *stream) {
    int r;
    long len = 0;

    r = sscanf((const char*)ptr, "Content-Length: %ld\n", &len);
    if(r) *((long *) stream) = len;
    return size * nmemb;
}

size_t coral::file_trans::discard_stream(void *ptr, size_t size, size_t nmemb, void *stream) {
    (void)ptr;
    (void)stream;
    return size * nmemb;
}

size_t coral::file_trans::read(void *ptr, size_t size, size_t nmemb, void *stream) {
    FILE *file = (FILE *)stream;
    size_t n;
    if(ferror(file)) {
        return CURL_READFUNC_ABORT;
    }
    n = fread(ptr, size, nmemb, file) * size;
    return n;
}

size_t coral::file_trans::write(void *buffer, size_t size, size_t nmemb, void *stream) {
    ftp_file_info_t* out = (ftp_file_info_t*) stream;
    if(!out->file) {
        /* open file for writing */
        out->file = fopen(out->file_name, "wb");
        if(!out->file) {
            return -1; /* failure, can't open file to write */
        }
    }
    return fwrite(buffer, size, nmemb, out->file);
}

bool coral::file_trans::upload(const std::string& remote_file, const std::string& local_file, long timeout, long tries) {
    ostringstream oss_log;
    oss_log << CORAL_D_FUNCTION_INFO << '(' << remote_file << ',' << local_file << ',' << timeout << ',' << tries << "):";

    FILE* file;
    long uploaded_len = 0;
    CURLcode r = CURLE_GOT_NOTHING;
    int c;

    file = fopen(local_file.c_str(), "rb");
    if (!file) {
        perror(NULL);
        return false;
    }

    curl_easy_setopt(curl_handle_, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl_handle_, CURLOPT_URL, remote_file.c_str());

    if (timeout)
        curl_easy_setopt(curl_handle_, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);

    curl_easy_setopt(curl_handle_, CURLOPT_HEADERFUNCTION, get_content_length);
    curl_easy_setopt(curl_handle_, CURLOPT_HEADERDATA, &uploaded_len);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, discard_stream);
    curl_easy_setopt(curl_handle_, CURLOPT_READFUNCTION, read);
    curl_easy_setopt(curl_handle_, CURLOPT_READDATA, file);
    /* disable passive mode */
    curl_easy_setopt(curl_handle_, CURLOPT_FTPPORT, "-");
    curl_easy_setopt(curl_handle_, CURLOPT_FTP_CREATE_MISSING_DIRS, 0L);
    curl_easy_setopt(curl_handle_, CURLOPT_VERBOSE, verbose_);

    for (c = 0; (r != CURLE_OK) && (c < tries); c++) {
        /* are we resuming? */
        if (c) { /* yes */
          /* determine the length of the file already written */
          /*
           * With NOBODY and NOHEADER, libcurl will issue a SIZE
           * command, but the only way to retrieve the result is
           * to parse the returned Content-Length header. Thus,
           * get_content_length(). We need discard_stream() above
           * because HEADER will dump the headers to stdout
           * without it.
           */
            curl_easy_setopt(curl_handle_, CURLOPT_NOBODY, 1L);
            curl_easy_setopt(curl_handle_, CURLOPT_HEADER, 1L);

            r = curl_easy_perform(curl_handle_);
            if (r != CURLE_OK)
                continue;

            curl_easy_setopt(curl_handle_, CURLOPT_NOBODY, 0L);
            curl_easy_setopt(curl_handle_, CURLOPT_HEADER, 0L);

            fseek(file, uploaded_len, SEEK_SET);

            curl_easy_setopt(curl_handle_, CURLOPT_APPEND, 1L);
        }
        else { /* no */
            curl_easy_setopt(curl_handle_, CURLOPT_APPEND, 0L);
        }

        r = curl_easy_perform(curl_handle_);
    }

    fclose(file);

    if (r != CURLE_OK) {
        log_manager::write(gv_app_name, oss_log.str() + curl_easy_strerror(r));
        return false;
    }
    return true;
}

bool coral::file_trans::download(const std::string& remote_file, ftp_file_info_t& ftp_file_info) {
    ostringstream oss_log;
    oss_log << CORAL_D_FUNCTION_INFO << '(' << remote_file << ',' << ftp_file_info.file_name << "):";
    CURLcode r = CURLE_GOT_NOTHING;
    if(curl_handle_) {
        /*
         * You better replace the URL with one that works!
         */
        curl_easy_setopt(curl_handle_, CURLOPT_URL, remote_file.c_str());
        /* Define our callback to get called when there's data to be written */
        curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, write);
        /* Set a pointer to our struct to pass to the callback */
        curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &ftp_file_info);
        /* Switch on full protocol/debug output */
        curl_easy_setopt(curl_handle_, CURLOPT_VERBOSE, verbose_);

        r = curl_easy_perform(curl_handle_);

        if(r != CURLE_OK) {
            log_manager::write(gv_app_name, oss_log.str() + curl_easy_strerror(r));
            return false;
        }
        CORAL_D_TRACE("%sDONE\n", oss_log.str().c_str());
    }
    if(ftp_file_info.file) fclose(ftp_file_info.file); /* close the local file */
    return true;
}

bool coral::file_trans::trans_file() {
    ostringstream oss_log;
    oss_log << CORAL_D_METHOD_INFO << "()";
    string log_msg;
    bool result_flag = false;
    int loop_limit = atoi(config::instance()->get_value("FTP_LOOP_LIMIT").c_str());
    int timeout_limit = atoi(config::instance()->get_value("FTP_TIMEOUT_LIMIT").c_str());
    string slash;
    slash = (!ftp_info_.remote_path.empty() && ftp_info_.remote_path.back() != '/') ? "/" : "";
    string remote_file = ftp_info_.remote_path + slash + ftp_info_.remote_file;

    slash = (!ftp_info_.local_path.empty() && ftp_info_.local_path.back() != '/') ? "/" : "";
    string local_file = ftp_info_.local_path + slash + ftp_info_.local_file;

    log_manager::write(gv_app_name, oss_log.str() + "Remote:" + ftp_info_.user + '@' + ftp_info_.host + remote_file);
    log_manager::write(gv_app_name, oss_log.str() + "Local:" + local_file);

    // you have to remember the FTP description at "https://curl.haxx.se/libcurl/c/CURLOPT_URL.html"
    /* ftp://user:password@ftp.example.com/readme.txt
            - This retrieves the readme.txt file from the user's home directory. When a username and password is specified,
              everything that is specified in the path part is relative to the user's home directory. To retrieve files from
              the root directory or a directory underneath the root directory then the absolute path must be specified
              by prepending an additional forward slash to the beginning of the path.
       ftp://user:password@ftp.example.com//readme.txt
            - This retrieves the readme.txt from the root directory when logging in as a specified user.
    */
    ostringstream oss_temp;

    if (is_there_special_char(ftp_info_.password)) {
        oss_temp << "ftp://" << ftp_info_.user << ':' << trans_string_to_url(ftp_info_.password) << '@' << ftp_info_.host << remote_file;
    }
    else {
        oss_temp << "ftp://" << ftp_info_.user << ':' << ftp_info_.password << '@' << ftp_info_.host << remote_file;
    }

    // init curl
    curl_handle_ = curl_easy_init();
    switch (trans_mode_) {
    case FILE_TRANS_MODE::PUT: {
            result_flag = upload(oss_temp.str(), local_file, timeout_limit, loop_limit);
        }
        break;
    case FILE_TRANS_MODE::GET: {
            ftp_file_info_t ftp_file_info = {local_file.c_str(), NULL};
            result_flag = download(oss_temp.str(), ftp_file_info);
        }
        break;
    default:
        log_manager::write(gv_app_name, "It is incorrect TransMode value.");
        result_flag = false;
    }
    // clean up curl
    if (curl_handle_)
        curl_easy_cleanup(curl_handle_);

    // log write
    string strVerv = (trans_mode_ == FILE_TRANS_MODE::PUT) ? "uploaded" : "downloaded";
    string strSucc = (result_flag) ? "successfully" : "unsuccessfully";
    print_string(log_msg, 1000, "%s has been %s %s.", local_file.c_str(), strVerv.c_str(), strSucc.c_str());
    log_manager::write(gv_app_name, oss_log.str() + log_msg);
    return result_flag;
}
