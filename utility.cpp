/*!
    \file       utility.cpp
    \brief      Useful functions and classes
    \details    Improving management efficiency and maintaining consistency
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#include "utility.h"
#include <cassert>
#include <cstdarg>
#include <sstream>
#include <climits>
#include <cstring>
#include <numeric>
#include <sys/wait.h>
#include <sys/types.h>

// function predicator
//! is comma?
bool coral::is_comma(char c) { return c == ','; }
//! is not comma?
bool coral::is_not_comma(char c) { return !coral::is_comma(c);}
//! is white space?
bool coral::is_whitespace(char c) { return c == ' '; }
//! is not white space?
bool coral::is_not_whitespace(char c) { return !coral::is_whitespace(c);}
//! comparison character
bool coral::compare_char(char c1, char c2) { return c1 == c2; }
//! to upper
char coral::to_upper(char c) { return std::toupper(c); }
//! to lower
char coral::to_lower(char c) { return std::tolower(c); }
//! trans a = b -> pair<a, b>, a & b are both std::string
std::pair<std::string, std::string> coral::string_to_pair(const std::string& str, char ch) {
    std::pair<std::string, std::string> pair_str_str;
    if (!str.empty()) {
        size_t idx = str.find(ch);
        if(idx != std::string::npos){
            pair_str_str.first = trim_string(str.substr(0,idx));
            pair_str_str.second = trim_string(str.substr(idx+1));
        }
        else {
            pair_str_str.first = str;
            pair_str_str.second = "";
        }
    }
    return pair_str_str;
}

std::string coral::any_to_string(const extlib::any& val) {
    if (val.type() == typeid(int))                  return std::to_string(extlib::any_cast<int>(val));
    if (val.type() == typeid(long))                 return std::to_string(extlib::any_cast<long>(val));
    if (val.type() == typeid(long long))            return std::to_string(extlib::any_cast<long long>(val));
    if (val.type() == typeid(unsigned))             return std::to_string(extlib::any_cast<unsigned>(val));
    if (val.type() == typeid(unsigned long))        return std::to_string(extlib::any_cast<unsigned long>(val));
    if (val.type() == typeid(unsigned long long))   return std::to_string(extlib::any_cast<unsigned long long>(val));
    if (val.type() == typeid(float))                return std::to_string(extlib::any_cast<float>(val));
    if (val.type() == typeid(double))               return std::to_string(extlib::any_cast<double>(val));
    if (val.type() == typeid(long double))          return std::to_string(extlib::any_cast<long double>(val));
    if (val.type() == typeid(std::string))          return extlib::any_cast<std::string>(val);
    return "";
}

//! is a std::string the number?
bool coral::is_string_number(const std::string& str) {
    for(const auto& ch : str){
        if(!std::isdigit(ch))
            return false;
    }
    return true;
}
//! is a special character included at a std::string?
bool coral::is_there_special_char(const std::string& str) {
    return std::find_if(str.begin(), str.end(), [](char ch) { return !(isalnum(ch) || ch == '_');}) != str.end();
}
//!
std::string coral::trans_string_to_url(const std::string& str) {
    std::string strTemp;
    for (char ch : str) {
        if (std::isalnum(ch) || ch == '_') {
            strTemp += ch;
        }
        else {
            char literal[4];
            std::sprintf(literal, "%%%2X", ch);
            strTemp += literal;
        }
    }
    return strTemp;
}

//! 문자열의 각 문자를 대문자로 변환
std::string coral::to_upper_string(const std::string& str) {
    std::string temp;
    std::transform(str.begin(), str.end(), std::back_inserter(temp), ::toupper);
    return temp;
}

//! 문자열의 각 문자를 소문자로 변환
std::string coral::to_lower_string(const std::string& str) {
    std::string temp;
    std::transform(str.begin(), str.end(), std::back_inserter(temp), ::tolower);
    return temp;
}

coral::Vec_STR coral::split_string(const std::string &str, char c, bool trimoption) {
    typedef std::string::const_iterator itor;
    coral::Vec_STR vs;
    itor i=str.begin();

    while(i!=str.end()){
        itor j=std::find_if(i,str.end(), std::bind(compare_char, std::placeholders::_1, c));
        if(i == j) vs.push_back("");
        else{
            if(trimoption){
                vs.push_back(trim_string(std::string(i,j)));
            }
            else{
                vs.push_back(std::string(i,j));
            }
        }

        if(j != str.end()) i = ++j;
        else i = j;
    }
    return vs;
}

coral::Vec_STR coral::split_string(const std::string &str, bool (*predicate)(char c), bool trimoption) {
    typedef std::string::const_iterator itor;
    coral::Vec_STR vs;
    itor i=str.begin();

    while(i!=str.end()){
        itor j=std::find_if(i,str.end(),predicate);
        if(i == j) vs.push_back("");
        else{
            if(trimoption){
                vs.push_back(trim_string(std::string(i,j)));
            }
            else{
                vs.push_back(std::string(i,j));
            }
        }

        if(j != str.end()) i = ++j;
        else i = j;
    }
    return vs;
}

coral::Vec_STR coral::split_string(const std::string &str, bool (*predicate)(char c),  bool (*not_predicate)(char c), bool trimoption) {
    typedef std::string::const_iterator itor;
    coral::Vec_STR vs;
    itor i=str.begin();

    while(i!=str.end())
    {
        i=std::find_if(i,str.end(),not_predicate);
        itor j=std::find_if(i,str.end(),predicate);
        if(i!=str.end()){
            if(trimoption){
                vs.push_back(trim_string(std::string(i,j)));
            }
            else{
                vs.push_back(std::string(i,j));
            }
        }
        i=j;
    }
    return vs;
}

std::string coral::trim_string(const std::string& str) {
    std::string::const_iterator i = std::find_if(str.begin(), str.end(), is_not_whitespace);
    std::string::const_iterator j = std::find_if(i, str.end(), is_whitespace);
    return (std::string(i,j));
}


std::string coral::trim_string_right(const std::string &str) {
    // "  abcd def  " -> "  abcd def"
    std::string::const_reverse_iterator ritor;
    if ((ritor = std::find_if(str.rbegin(), str.rend(), is_not_whitespace)) != str.rend()) {
        return (std::string(str.begin(), ritor.base()));
    }
    return str;
}

std::string coral::trim_string_left(const std::string &str) {
    // "  abcd def  " -> "  abcd def"
    std::string::const_iterator itor;
    itor = std::find_if(str.begin(), str.end(), is_not_whitespace);
    return (std::string(itor,str.end()));
}

// writes the specified printf like format to standard string(str).
size_t coral::print_string(std::string& str, size_t size, const char* format, ...) {
    coral::thread_lock lock;
    coral::thread_locker locker(lock);
    static char char_tmp[CORAL_D_STR_LEN_MAX];
    memset(char_tmp, 0x00, CORAL_D_STR_LEN_MAX);
    va_list ap;
    va_start(ap, format);
    vsnprintf(char_tmp, size, format, ap);
    va_end(ap);
    str.assign(char_tmp);
    return str.size();
}

// Appends the specified printf like format to standard string(str).
size_t coral::append_string(std::string& str, size_t size, const char *format, ...) {
    coral::thread_lock lock;
    coral::thread_locker locker(lock);
    static char char_tmp[CORAL_D_STR_LEN_MAX];
    memset(char_tmp, 0x00, CORAL_D_STR_LEN_MAX);
    va_list ap;
    va_start(ap, format);
    vsnprintf(char_tmp, size, format, ap);
    va_end(ap);
    str.append(char_tmp);
    return str.size();
}

bool coral::getline_after_remove_cr(std::istream& is, std::string& str) {
    std::getline(is, str);
    if (str.find(0x0d) != std::string::npos) {
        str.erase(std::remove(str.begin(), str.end(), 0x0d), str.end());
    }
    return !is.eof();
}

std::string coral::get_value_from_string(std::string& strSrc, const std::string& strCmp) {
    std::string strTemp;
    size_t nPos = strSrc.find(strCmp);
    if (nPos != std::string::npos) {
        strTemp = strSrc.substr(0, nPos);
        strSrc = strSrc.substr(nPos+1);
    }
    return strTemp;
}

std::string coral::get_host_name() {
    // host name variable
    const size_t nHostNameSize = 20;
    char hostName[nHostNameSize] = { 0, };
    // system host name getting
    gethostname(hostName, nHostNameSize);
    // find out Equipment in config.txt

    return std::string(hostName);
}

bool coral::uncompress(const std::string& compressed_file_name, std::string& uncompress_file_name) {
    std::string strAppPath; // app name with full path
    std::string strAppName; // app name only
    std::string strOption;  // options
    std::string strExtension = file_name_extension(compressed_file_name);

    if (strExtension == "bz2") {        //bzip2
        strAppPath = "/usr/bin/bzip2"; strAppName = "bzip2"; strOption = "-df";
    }
    else if (strExtension == "gz") {    //gzip
        strAppPath = "/usr/bin/gzip"; strAppName = "gzip"; strOption = "-df";
    }
    else if (strExtension == "xz") {    //xz
        strAppPath = "/usr/bin/xz"; strAppName = "xz"; strOption = "-df";
    }
    else if (strExtension == "Z") {     //compress
        strAppPath = "/usr/bin/compress"; strAppName = "compress"; strOption = "-df";
    }
    else {
        return false;
    }

    // for uncompressing xz file type
    pid_t cpid = fork();
    switch(cpid) {
        case -1 :
            return false;
            break;
        case 0 :
            execl(strAppPath.c_str(), strAppName.c_str(), strOption.c_str(), compressed_file_name.c_str(), NULL);
            break;
        default :
            wait(NULL);
            uncompress_file_name = compressed_file_name.substr(0, compressed_file_name.size()-(strExtension.size()+1));
            break;
    }

    return true;
}

std::string coral::file_name_extension(const std::string& file_name) {
    if (file_name.find('.') == std::string::npos)
        return file_name;
    else
        return file_name.substr(file_name.rfind('.')+1);
}

coral::Vec_STR coral::directory_file_list(const std::string& directory, bool dir_path_flag, bool recursion_flag) {
    return directory_file_list(directory, "", dir_path_flag, recursion_flag);
}

coral::Vec_STR coral::directory_file_list(const std::string& directory, const std::string& strPattern, bool dir_path_flag, bool recursion_flag) {
    /* variable for directory - unix style */
    struct stat     statBuffer;
    DIR             *pdir;
    struct dirent   *pdirent;
    std::string     file_name_only;
    std::string     file_name_with_path;
    coral::Vec_STR  file_names;

    // if empty than return empty container
    if (directory.empty()) {
        return file_names;
    }

    /* directory open */
    if(!(pdir = opendir(directory.c_str())))
        throw domain_error("Directory opendir failure.");

    /* file list of directory */
    while((pdirent = readdir(pdir))) {
        file_name_only = pdirent->d_name;
        // skip, . and ..
        if (file_name_only == "." || file_name_only == "..")
            continue;
        if (directory.back() == '/')
            file_name_with_path = directory + file_name_only;
        else
            file_name_with_path = directory + '/' + file_name_only;
        if(stat(file_name_with_path.c_str(), &statBuffer))
            throw domain_error("Sub Directory's entry stat initialize failure.");
        if(S_ISREG(statBuffer.st_mode)) {
            // if strPattern is not empty and there is no strPattern in file_name_only then next
            if (!strPattern.empty() && file_name_only.find(strPattern) == std::string::npos)
                continue;
            if(dir_path_flag)
                file_names.push_back(file_name_with_path);
            else
                file_names.push_back(file_name_only);
        }
        if (recursion_flag && S_ISDIR(statBuffer.st_mode)) {
            for ( const auto& v: directory_file_list(file_name_with_path, strPattern, dir_path_flag, recursion_flag)) {
                file_names.push_back(v);
            }
        }
    }
    closedir(pdir);

    return file_names;
}

std::string coral::get_file_name_from_full_path(const std::string& full_path) {
    if (full_path.find('/') == std::string::npos)
        return full_path;
    else
        return full_path.substr(full_path.rfind('/')+1);
}

long coral::file_size(const std::string& file_name)
{
    struct stat statbuf;
    if (!(stat(file_name.c_str(), &statbuf) < 0) && (S_ISREG(statbuf.st_mode))) {
        return statbuf.st_size;
    }
    else{
        return (-1);
    }
}

int coral::move_file(const std::string &src_file_name, const std::string &dest_file_name) {
    int stat = 0;

    if(exist_file(src_file_name) < 0 || src_file_name.empty() || dest_file_name.empty() || (src_file_name == dest_file_name))
        stat = -1;
    else {
        stat = rename(src_file_name.c_str(), dest_file_name.c_str());
        // if occured error than one more after sleep 1 sec
        if (stat < 0) {
            std::ifstream source_file(src_file_name.c_str(), std::ios_base::binary);
            std::ofstream dest_file(dest_file_name.c_str(), std::ios_base::binary);
            dest_file << source_file.rdbuf();
            dest_file.close();
            source_file.close();
            stat = remove(src_file_name.c_str());
        }
    }
    return stat;
}

int coral::copy_file(const std::string& src_file_name, const std::string& dest_file_name) {
    if(exist_file(src_file_name) < 0 || src_file_name.empty() || dest_file_name.empty() || (src_file_name == dest_file_name))
        return (-1);
    else {
        std::ifstream source_file(src_file_name.c_str(), std::ios_base::binary);
        std::ofstream dest_file(dest_file_name.c_str(), std::ios_base::binary);
        dest_file << source_file.rdbuf();
        dest_file.close();
        source_file.close();
        return (1);
    }
}

int coral::exist_file(const std::string& file_name) {
    return access(file_name.c_str(), F_OK);
}

std::string coral::datetime_now(coral::TIME_STRING_FORMAT timeformat) {
    time_t rawtime;
    time (&rawtime);
    return time_to_string(rawtime, timeformat);
}

std::string coral::time_to_string(time_t nTime, coral::TIME_STRING_FORMAT timeformat) {
    struct tm * tm_p;
    int size = 24;
    char timebuffer[size];
    memset(timebuffer, 0, size);
    tm_p = localtime (&nTime);
    strftime(timebuffer, size, TIME_FORMAT.at(timeformat).c_str(), tm_p);
    return timebuffer;
}

time_t coral::string_to_time(const std::string& str, coral::TIME_STRING_FORMAT timeformat) {
    struct tm tm_s;
    memset(&tm_s, 0, sizeof(tm));
    strptime(str.c_str(), TIME_FORMAT.at(timeformat).c_str(), &tm_s);
    return mktime(&tm_s);
}

std::istream& coral::find_string_pos_in_file(std::istream& infile, const std::string& str) {
    // file 처음위치로 이동
    infile.seekg (0, std::ios::beg);
    std::string strtemp;
    getline_after_remove_cr(infile, strtemp);

    // 문장내 검색이 아니라 찾는 문자열이 라인 첫번째에 있어야함.
    while(strtemp.find(str)){
        getline_after_remove_cr(infile, strtemp);
        if(infile.eof()){
            throw data_find_error(__FUNCTION__ + str + ", there is no the str in the file.");
        }
    }
    return infile;
}

coral::elapsed_time::elapsed_time () {
    reset();
}

double coral::elapsed_time::usec () const {
    return sec() * 100000;
}

double coral::elapsed_time::msec () const {
    return sec() * 1000;
}

double coral::elapsed_time::sec () const {
    std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
    return  std::chrono::duration<double>(current - starting_).count();
}

void coral::elapsed_time::reset () {
    starting_ = std::chrono::steady_clock::now();
}

double coral::round_float(double x, int precision) {
    const auto exponent = std::pow(10.,precision);
    x = x * exponent;
    if(x>=0)
        if(x-(long long)x>=0.5) x++;
    if(x<0)
        if(x-(long long)x<-0.5) x--;
    x=(long long)x / exponent;

    return x;
}

void coral::beta_dist_param(double mean, double var, double& alpha, double& beta) {
    alpha = ((1. - mean) / var - 1 / mean) * (mean * mean);
    beta = alpha * (1 / mean - 1);
}

coral::file_watcher::file_watcher(const std::string& file_name_with_path, std::chrono::duration<int, std::milli> delay)
    : file_name_with_path_{file_name_with_path}, delay_{delay} {
    for(auto &file : extlib::filesystem::recursive_directory_iterator(file_name_with_path_)) {
        if (extlib::filesystem::is_regular_file(file.symlink_status())) {
            umafilePaths_[file.path().string()] = extlib::filesystem::last_write_time(file);
        }
    }
}

void coral::file_watcher::start(const std::function<void (const std::string&, coral::FILE_STATUS)>& Actor) {
    while(bRunning_) {
        // Wait for "delay_" milliseconds
        std::this_thread::sleep_for(delay_);
        auto itor = umafilePaths_.begin();
        while (itor != umafilePaths_.end()) {
            if (!extlib::filesystem::exists(itor->first)) {
                Actor(itor->first, FILE_STATUS::ERASED);
                itor = umafilePaths_.erase(itor);
            }
            else {
                itor++;
            }
        }
        // Check if a file was CREATED or MODIFIED
        for(auto &file : extlib::filesystem::recursive_directory_iterator(file_name_with_path_)) {
            if (extlib::filesystem::is_regular_file(file.symlink_status())) {
                auto current_file_last_write_time = extlib::filesystem::last_write_time(file);
                // File modification
                if(exist(file.path().string())) {
                    if(umafilePaths_[file.path().string()] != current_file_last_write_time) {
                        umafilePaths_[file.path().string()] = current_file_last_write_time;
                        Actor(file.path().string(), FILE_STATUS::MODIFIED);
                    }
                }
                // File creation
                else {
                    umafilePaths_[file.path().string()] = current_file_last_write_time;
                    Actor(file.path().string(), FILE_STATUS::CREATED);
                }
            }
        }
    }
}

bool coral::file_watcher::exist(const std::string& key) const {
    return umafilePaths_.find(key) != umafilePaths_.end();
}
