/*!
    \file       utility.h
    \brief      utility functions and classes
    \details    Useful functions and classes
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_UTILITY_H__
#define __CORAL_UTILITY_H__

#include "exception.h"
#include "types.h"
#include "config.h"
#include "thread_lock.h"

#include <cstdio>
#include <ctime>
#include <cctype>
#include <cmath>
#include <cassert>
#include <cstdlib>

#include <iostream>
#include <random>
#include <iterator>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <complex>

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

//! Core Library for Applications and Libraries
namespace coral {
    //-------------------------------------------------------------------------------------------------------------
    // Function Predicator
    //! is comma?
    bool is_comma(char);
    //! is not comma?
    bool is_not_comma(char);
    //! is white space?
    bool is_whitespace(char);
    //! is not white space?
    bool is_not_whitespace(char);
    //! comparison character
    bool compare_char(char, char);
    //! to upper a charactor
    char to_upper(char);
    //! to lower a charactor
    char to_lower(char);
    //! 숫자 문자열 확인 - digit 0~9 only
    /*!
        \param str number values of string type
        \return if all characters are number then true otherwise false
    */
    bool is_string_number(const std::string& str);
    //! does string have a special character?
    /*!
        \param str a string
        \return if string has a special character then return true, otherwise false
    */
    bool is_there_special_char(const std::string& str);
    //! transform string to url style, special character transform to hex num such as & -> %2A
    /*!
        \param str a string
        \return transformed string to URL style
    */
    std::string trans_string_to_url(const std::string& str);
    //! 문자를 T 값으로 변환할 수 있는지 확인하면서 값을 받게한다.
    /*!
        \param str number values of string type
        \param value template value but can be used numeric types only
        \return if str can convert to T type value then true otherwise false
    */
    template<typename T>
    bool string_to_number(const std::string& str, T& value)
    {
        // is empty
        if (str.empty()) {
            return false;
        }
        // value는 template type으로 컴파일 타임에 결정 되어 들어온다.
        std::istringstream parser(str);
        parser >> value;

        return !parser.fail() && parser.eof();
    }
    //! 문자열의 각 문자를 대문자로 변환
    std::string to_upper_string(const std::string& str);
    //! 문자열의 각 문자를 소문자로 변환
    std::string to_lower_string(const std::string& str);
    //! anythings to string, if val is user type then it has to have operator<<
    // using stringstream
    template<typename T>
    std::string to_string(T val) {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }

    /*! string to convert a pair (Key & Value): ex ABC=123 -> Key:ABC, Value:123
        \param str key value type string
        \param ch delimeter character
        \return ex ABC=123 -> first:ABC, second:123
    */
    std::pair<std::string, std::string> string_to_pair(const std::string& str, char ch = '=');
    /*! part to convert a string
        \param p std::pair<type1, type2>
        \param ch delimeter character
        \return a string
    */
    template<typename T1, typename T2>
    std::string pair_to_string(const std::pair<T1, T2>& p, char ch = '=') {
        std::ostringstream oss;
        oss << p.first << ch << p.second;
        return oss.str();
    }
    /*! any to string, default type only
        \param val is a any type
        \return string
    */
    std::string any_to_string(const extlib::any& val);
    //-------------------------------------------------------------------------------------------------------------
    //! 입력된 boolean 값이 true면 PASS, false면 FAIL
    inline std::string bool_to_string(bool flag) { return (flag) ? "PASS" : "FAIL"; }
    //! 입력된 string 값이 PASS 일 경우 true return 이외는 false
    inline bool string_to_bool(const std::string& str) { return ((to_upper_string(str) == "PASS") ? true : false); }
    //-------------------------------------------------------------------------------------------------------------
    // String manipulator
    //! String spliter
    /*!
        \param str string
        \param c delimiter charator
        \param trimoption string trim option
        \return 문자열에서 구분자 사이에 값이 없을 경우라도 "" 값을 return
    */
    Vec_STR split_string(const std::string &str, char c, bool trimoption = true);
    //! String spliter
    /*!
        \param str string
        \param (*predicate) predicator
        \param trimoption string trim option
        \return 문자열에서 구분자 사이에 값이 없을 경우라도 "" 값을 return
    */
    Vec_STR split_string(const std::string &str, bool (*predicate)(char c), bool trimoption = true);
    //!
    /*!
        \param str string
        \param (*predicate) predicator function pointer
        \param (*not_predicate) not predicator function pointer
        \param trimoption string trim option
        \return 문자열에서 구분자 사이에 값이 없을 경우는 무시하고 값이 있을 경우 return
    */
    Vec_STR split_string(const std::string &str, bool (*predicate)(char c),  bool (*not_predicate)(char c), bool trimoption = true);
    //!
    /*!
        \param container standard template container
        \param strSource source string
        \param delimiteriters delimiter characters
    */
    template < typename Container >
    void split_string(Container& container, const std::string& strSource, const std::string& delimiteriters = " \t\n,|") {
        const std::string::size_type len = strSource.length();
        std::string::size_type i = 0;

        while (i < len) {
            // Eat leading whitespace
            i = strSource.find_first_not_of(delimiteriters, i);
            if (i == std::string::npos)
                return;   // Nothing left but white space

            // Find the end of the token
            std::string::size_type j = strSource.find_first_of(delimiteriters, i);
            // Push token
            if (j == std::string::npos) {
                container.insert(container.end(), strSource.substr(i));
                return;
            }
            else {
                container.insert(container.end(), strSource.substr(i, j-i));
            }
            // Set up for next loop
            i = j + 1;
        }
    }

    //! 문자열 공백 제거
    /*!
        ex) "    abcd  efgh  "  -> "abcd"
        \param str string
    */
    std::string trim_string(const std::string &str);
    //! 문자열 오른쪽 공백 제거
    /*!
        ex) "  abcd ef g hi    "  -> "  abcd ef g hi"
        \param str string
    */
    std::string trim_string_right(const std::string &str);
    //! 문자열 왼쪽 공백 제거
    /*!
        ex) "  abcd ef g hi    "  -> "abcd ef g hi    "
        \param str string
    */
    std::string trim_string_left(const std::string &str);
    //! writes the specified printf like format to standard string(strDest).
    /*!
        \param str destnation string
        \param size char literer size
        \param format format string
        \param ... arguments
        \return size of string
    */
    size_t print_string(std::string& str, size_t size, const char* format, ...);
    //! Appends the specified printf like format to standard string(strDest).
    /*!
        \param str destnation string
        \param size char literer size
        \param format format string
        \param ... arguments
        \return size of string
    */
    size_t append_string(std::string& str, size_t size, const char* format, ...);
    //! Merging string with delimiter
    /*!
        \param container string set container
        \param delimiter When strings merge including charactor
        \return merged string
    */
    template<typename Container>
    std::string merge_string_with_delimiter(const Container& container, char delimiter = ',') {
        if (container.empty()) return "";
        typedef typename Container::const_iterator const_iterator;
        const_iterator itor = container.begin();
        std::ostringstream oss;
        oss << *itor;
        while(++itor != container.end()) oss << delimiter << *itor;
        return oss.str();
    }
    //! when reading string from istream, to remove 0x0d char in the end of the string.
    /*!
        \param is input file stream
        \param str string
        \return default true, when occurred error then throw
    */
    bool getline_after_remove_cr(std::istream& is, std::string& str);
    //! getting a splited string by a finding string
    /*!
        strSrc에서 strCmp를 찾아 처음부터 찾은 위치전까지의 값을 리턴한다.
        \param strSrc 입력된 문자열이며 찾는 값을 제외한 문자열을 다시 저장한다.
        \param strCmp 찾고자 하는 문자
        \return 찾고자 하는 문자위치 앞까지의 문자열.
    */
    std::string get_value_from_string(std::string& strSrc, const std::string& strCmp);
    /*! \return system host name */
    // Equipment Information
    //! Equipment Hostname
    std::string get_host_name();
    //-------------------------------------------------------------------------------------------------------------
    // File Controler
    //! uncompress by the file extension name
    /*!
        \param compressed_file_name a compressed file name
        \param uncompress_file_name uncompress file name
        \return uncompress successful then true, otherwise false
    */
    bool uncompress(const std::string& compressed_file_name, std::string& uncompress_file_name);
    //! Files list in directory (regular file only)
    /*!
        \param directory directory path
        \param dir_path_flag full path file_name = true, file_name only = false
        \param recursion_flag searching file recursivelly
        \return file_names vector container
    */
    Vec_STR directory_file_list(const std::string& directory, bool dir_path_flag = true, bool recursion_flag = false);
    //! Files list in directory (regular file only)
    /*!
        \param directory directory path
        \param strPattern finding litteral
        \param dir_path_flag full path file_name = true, file_name only = false
        \param recursion_flag searching file recursivelly
        \return file_names vector container
    */
    Vec_STR directory_file_list(const std::string& directory, const std::string& strPattern, bool dir_path_flag = true, bool recursion_flag = false);
    /*! to get file name in full path string
        \param full_path full path string (ex. /data/datafile)
        \return filen name (ex. datafile)
    */
    std::string get_file_name_from_full_path(const std::string& full_path);
    /*! to get file extension name
        \param file_name a file name
        \return file extension name
    */
    std::string file_name_extension(const std::string& file_name);
    /*! status of file size
        \param file_name size를 확인하고자 하는 file name
        \return file size
    */
    long file_size(const std::string &file_name);
    /*! file move
        \param src_file_name source file name full path
        \param dest_file_name destnation file name full path
        \return stat
    */
    int move_file(const std::string &src_file_name, const std::string& dest_file_name);
    /*! file copy
        \param src_file_name source file name full path
        \param dest_file_name destnation file name full path
        \return if 1 is done, otherwise failure
    */
    int copy_file(const std::string &src_file_name, const std::string& dest_file_name);
    /*! file exist
        \param file_name file name with full path
        \return if value is equal or greater than 0 then exist, otherwise not exist.
    */
    int exist_file(const std::string &file_name);
    /*! Compare file, size and description
        \param c it has to sequence container and element type is string, and have to file name is full name (with path).
        \return if all file are same than true, otherwise false;
    */
    template <typename Container>
    bool compare_file(const Container& c) {
        if (c.size() > 1) {
            for (size_t i = 0; i < c.size() - 1; i++) {
                auto nFileSize1 = coral::file_size(c[i]);
                std::ifstream ifs1(c[i]);
                if (nFileSize1 <= 0 || ifs1.fail()) {
                    return false;
                }
                std::stringstream ss1;
                ss1 << ifs1.rdbuf();
                for (size_t j = i + 1; j < c.size(); j++ ) {
                    // compare file size
                    if (nFileSize1 != coral::file_size(c[j])) {
                        return false;
                    }
                    // open file
                    std::ifstream ifs2(c[j]);
                    if (ifs2.fail()) {
                        return false;
                    }
                    // compare file description
                    std::stringstream ss2;
                    ss2 << ifs2.rdbuf();
                    if (ss1.str() != ss2.str()) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    //-------------------------------------------------------------------------------------------------------------
    // Time generator
    /*! now date & time
        \return yyyymmddHHMMSS
    */
    std::string datetime_now(TIME_STRING_FORMAT timeformat = TIME_STRING_FORMAT::NUMBERDATE1);
    /*! time_t to string
        \param nTime time_t value
        \param timeformat format
        \return a formated string
    */
    std::string time_to_string(const time_t nTime, TIME_STRING_FORMAT timeformat = TIME_STRING_FORMAT::NUMBERDATE3);
    /*! string to time_t
        \param str a formated string
        \param timeformat format
        \return a time_t value
    */
    time_t string_to_time(const std::string& str, TIME_STRING_FORMAT timeformat = TIME_STRING_FORMAT::NUMBERDATE3);

    //-------------------------------------------------------------------------------------------------------------
    //! Randomly selection an element from a container using random generator
    template<typename Iter, typename RandomGenerator>
    Iter select_an_element_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }
    //! Randomly selection an element from a container
    template<typename Iter>
    Iter select_an_element_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_an_element_randomly(start, end, gen);
    }

    //! Container에서 입력된 값의 위치값을 return
    /*!
        \param cnt template container
        \param data a element : must support operator <<
        \return position of the data in the container
    */
    template< typename Container, typename DataType >
    size_t find_data_pos_in_container(const Container& cnt, const DataType& data) {
        // if the compiler don't support c++11 than remove comment and use citor
        // typedef typename Container::const_iterator citor;
        const auto& pos = std::find(cnt.begin(), cnt.end(), data);
        if (pos == cnt.end()) {
            std::ostringstream oss;
            oss << "There is no " << data << " in the container:" << typeid(cnt).name();
            throw coral::data_find_error(oss.str());
        }
#if defined(__SPARC10__)
        return (pos - cnt.begin());
#else
        return (std::distance(cnt.begin(), pos));
#endif
    }
    //! find file position by string
    /*!
        \param infile file stream
        \param str
    */
    std::istream& find_string_pos_in_file(std::istream& infile, const std::string& str);
    //-------------------------------------------------------------------------------------------------------------
    // mathematics function
    /*! round for floating type number, ex) round_float(3.14, 1) => 3.1, round_float(3.15, 1) => 3.2
        \param x floating type number
        \param precision precision
        \return rounded number
    */
    double round_float(double x, int precision);

    //-------------------------------------------------------------------------------------------------------------
    // Statistics Function : template function
    //! median
    /*!
        median of numeric vector container
    */
    template< typename Container >
    double median(const Container& c) {
        typedef typename Container::size_type size_type;
        if (c.empty())
            throw coral::container_empty_error(__FUNCTION__);
        Container c_temp(c);
        size_type size = c_temp.size();
        size_type mid = size/2;
        std::partial_sort(c_temp.begin(), c_temp.begin()+mid+1 , c_temp.end());
        return static_cast<double>(size % 2 == 0 ? (c_temp[mid] + c_temp[mid-1]) / 2 : c_temp[mid]);
    }
    //! calc sum, mean, variance
    /*!
        \param c Sequence Container
        \param sum number of total
        \param mean sample mean
        \param var sample variance
    */
    template< typename Container >
    void mean_var(const Container& c, double& sum, double& mean, double& var) {
        if (c.empty())
            throw coral::container_empty_error(__FUNCTION__);

        typedef typename Container::size_type size_type;
        size_type n = c.size(), j;
        /* sum square */
        double ss;
        //std::assert(n>1);
        sum = mean = var = ss = 0.;
        /*sum = Σ(data) */
        for (j=0 ; j < n ; ++j){
            sum += c[j];
            ss += c[j] * c[j];
        }
        /*mean = sum/n */
        mean = sum/n;
        /*variance = (Σ(data^2) - (Σ(data))^2 /n) / (n-1) */
        var = (ss - (sum * sum) / n) / (n-1);
        if (!std::isfinite(mean)) mean = 0.;
        if (!std::isfinite(var)) var = 0.;
    }
    //! calculate alpha and beta parameters of the beta distribution
    /*!
        \param mean sample mean
        \param var sample variance
        \param alpha a first parameter of beta distribution
        \param beta a second parameter of beta distribution
    */
    void beta_dist_param(double mean, double var, double& alpha, double& beta);
    //! same signbit function
    /*!
        \param f float number
        \return f < 0 -> 1, f >= 0 -> 0
    */
    template< typename T >
    inline int sign(T f) { return ((f < 0) ? 1 : 0); }
    //-------------------------------------------------------------------------------------------------------------
    // similar STL algorithm : template function
    //! copy_if
    template< typename InputIterator, typename OutputIterator, typename Predicate>
    OutputIterator copy_if(InputIterator begin, InputIterator end, OutputIterator destBegin, Predicate p) {
        while(begin != end) {
            if(p(*begin)) *destBegin++ = *begin;
            ++begin;
        }
        return destBegin;
    }
    //! object를 삭제하는 function object class
    /*!
        template으로 타입에 상관없이 new로 생성된 object를 삭제함
        for_each등 function object를 사용하는 algorithm의 predicate로 사용
    */
    struct delete_object {
        template< typename T >
        void operator()(const T* ptr) const {
            delete ptr;
        }
    };

    //! Generating random number class
    /*!
        range [ nRangeMin_, nRangeMax_)
    */
    class random {
    public:
        random(int nMin = 0, int nMax = RAND_MAX) {
            nRangeMin_ = nMin;
            nRangeMax_ = nMax;
            std::srand((unsigned)time(NULL));
        }

        void setRange(int nMin = 0, int nMax = RAND_MAX) {
            nRangeMin_ = nMin;
            nRangeMax_ = nMax;
        }

        int execNumberGenerate() {
            return ((std::rand() % (nRangeMax_ - nRangeMin_)) + nRangeMin_);
        }
    private:
        int nRangeMin_;
        int nRangeMax_;
    };

    //! elapsed time
    class elapsed_time {
    public:
        //! Record the current time
        elapsed_time ();
        //! Return elapsed time, in microseconds
        double usec () const;
        //! Return elapsed time, in milliseconds
        double msec () const;
        //! Return elapsed time, in sec
        double sec  () const;
        //! Reset the current time
        void reset ();

    private:
        std::chrono::steady_clock::time_point starting_;  ///< Starting time
    };

    //! file_watcher
    /*! using boost filesystem, if your compiler is support C++17 higher then you can yous std::filesystem */
    class file_watcher {
    public:
        //! Keep a record of files from the base directory and their last modification time
        file_watcher(const std::string& file_name_with_path, std::chrono::duration<int, std::milli> delay);
        //! Monitor "file_name_with_path_" for changes and in case of a change execute the user supplied "Actor" function
        void start(const std::function<void (const std::string&, coral::FILE_STATUS)>& Actor);
    private:
        //! is there key in umafilePaths_
        bool exist(const std::string& key) const;
        bool bRunning_ = true;                                          ///! loop flag
        std::string file_name_with_path_;                               ///! watch a directory
        std::chrono::duration<int, std::milli> delay_;                  ///! Time interval at which we check the base folder for changes
        std::unordered_map<std::string, std::time_t> umafilePaths_;    ///! file container with last time
    };
} // end coral namesapce

#endif //__CORAL_UTILITY_H__
