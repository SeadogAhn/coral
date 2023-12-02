/*!
    \file       types.h
    \brief      type declaration and definition
    \details    typedef and using and user defined structures and so on,
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_TYPES_H__
#define __CORAL_TYPES_H__

#include "defines.h"
#include "variables.h"
#include "exception.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include "occi.h"

// library namespace
#if __cplusplus < 201703
    #include "boost/filesystem.hpp"
    #include "boost/any.hpp"
    #include "boost/regex.hpp"
#else
    #include <filesystem>
    #include <any>
    #include <regex>
#endif

//! Core Library for Applications and Libraries
namespace coral {
    typedef unsigned char       UCHR;
    typedef unsigned int        UINT;
    typedef unsigned long       ULONG;
    typedef unsigned long long  UDLONG;
    typedef bool                BOOL;
    typedef char                CHR;
    typedef int                 INT;
    typedef long                LONG;
    typedef long long           DLONG;
    typedef float               FLT;
    typedef double              DBL;
    // PAIR
    typedef std::pair<int, int> Pair_INTINT;
    typedef std::pair<std::string, std::string> Pair_STRSTR;
    //STL Container Type definition
    // vector types
    typedef const std::vector<CHR> Vec_I_CHR;
    typedef std::vector<CHR> Vec_CHR, Vec_O_CHR, Vec_IO_CHR;
    typedef const std::vector<UCHR> Vec_I_UCHR;
    typedef std::vector<UCHR> Vec_UCHR, Vec_O_UCHR, Vec_IO_UCHR;
    typedef const std::vector<INT> Vec_I_INT;
    typedef std::vector<INT> Vec_INT, Vec_O_INT, Vec_IO_INT;
    typedef const std::vector<UINT> Vec_I_UINT;
    typedef std::vector<UINT> Vec_UINT, Vec_O_UINT, Vec_IO_UINT;
    typedef const std::vector<DBL> Vec_I_DBL;
    typedef std::vector<DBL> Vec_DBL, Vec_O_DBL, Vec_IO_DBL;
    typedef const std::vector<std::string> Vec_I_STR;
    typedef std::vector<std::string> Vec_STR, Vec_O_STR, Vec_IO_STR;
    // set types
    typedef const std::set<std::string> Set_I_STR;
    typedef std::set<std::string> Set_STR, Set_IO_STR, Set_O_STR;
    typedef const std::set<INT> Set_I_INT;
    typedef std::set<INT> Set_INT, Set_IO_INT, Set_O_INT;
    typedef const std::multiset<std::string> MSet_I_STR;
    typedef std::multiset<std::string> MSet_STR;
    // unordered set types
    typedef const std::unordered_set<std::string> USet_I_STR;
    typedef std::unordered_set<std::string> USet_STR, USet_IO_STR, USet_O_STR;
    typedef const std::unordered_set<INT> USet_I_INT;
    typedef std::unordered_set<INT> USet_INT, USet_IO_INT, USet_O_INT;
    typedef const std::unordered_multiset<std::string> UMSet_I_STR;
    typedef std::unordered_multiset<std::string> UMSet_STR;
    // map types
    typedef const std::map<std::string, std::string> Map_I_STRSTR;
    typedef std::map<std::string, std::string> Map_STRSTR, Map_IO_STRSTR, Map_O_STRSTR;
    typedef const std::map<INT, INT> Map_I_INTINT;
    typedef std::map<INT, INT> Map_INTINT, Map_IO_INTINT, Map_O_INTINT;
    typedef const std::multimap<std::string, Vec_STR> MMap_I_STR_Vec_STR;
    typedef std::multimap<std::string, Vec_STR> MMap_STR_Vec_STR;
    // unordered_map types
    typedef const std::unordered_map<std::string, std::string> UMap_I_STRSTR;
    typedef std::unordered_map<std::string, std::string> UMap_STRSTR, UMap_IO_STRSTR, UMap_O_STRSTR;
    typedef const std::unordered_map<INT, INT> UMap_I_INTINT;
    typedef std::unordered_map<INT, INT> UMap_INTINT, UMap_IO_INTINT, UMap_O_INTINT;
    typedef const std::unordered_multimap<std::string, Vec_STR> UMMap_I_STR_Vec_STR;
    typedef std::unordered_multimap<std::string, Vec_STR> UMMap_STR_Vec_STR;
    //--------------------------------------------------------------------------------
    // enumeration type declaration & definition
    //--------------------------------------------------------------------------------
    //! 시간 형태를 선택하기 위한 enumeration
    enum TIME_STRING_FORMAT {
         NUMBERDATE1   // 19991231235959       yyyymmddhhmiss
        ,NUMBERDATE2   // 31121999235959       ddmmyyyyhhmiss
        ,NUMBERDATE3   // 1999-12-31 23:59:59  yyyy-mm-dd hh:mi:ss
        ,NUMBERDATE4   // 1999/12/31 23:59:59  yyyy-mm-dd hh:mi:ss
        ,STRINGDATE1   // 31Dec1999235959      ddmbyyyyhhmiss
        ,STRINGDATE2   // 31 Dec 1999 23:59:59 dd mb yyyy hh mi ss
    };

    const std::unordered_map<int, std::string> TIME_FORMAT {
         {TIME_STRING_FORMAT::NUMBERDATE1, "%Y%m%d%H%M%S"}
        ,{TIME_STRING_FORMAT::NUMBERDATE2, "%d%m%Y%H%M%S"}
        ,{TIME_STRING_FORMAT::NUMBERDATE3, "%Y-%m-%d %H:%M:%S"}
        ,{TIME_STRING_FORMAT::NUMBERDATE4, "%Y/%m/%d %H:%M:%S"}
        ,{TIME_STRING_FORMAT::STRINGDATE1, "%d%b%Y%H%M%S"}
        ,{TIME_STRING_FORMAT::STRINGDATE2, "%d %b %Y %H:%M:%S"}
    };

    //! enumeration of file type mode
    enum class FILE_TYPE {
        BINARY  ///< binary
        , ASCII ///< ascii
    };
    //! enumeration of rotating angle
    enum class ROTATE_ANGLE {
          HALF           ///< 180˚
        , CW90           ///< clockwise 90˚
        , CCW90          ///< counterclockwise 90˚
        , FLIPHORIZONTAL ///< flip horizontal
        , FLIPVERTICAL   ///< flip vertical
    };
    //! file status for file_watcher
    enum class FILE_STATUS {
          CREATED       ///< a new file has been created
        , MODIFIED      ///< a file has been modifyed
        , ERASED        ///< a file has been deleted
    };
    //--------------------------------------------------------------------------------
    // structure & class type declaration & definition
    //--------------------------------------------------------------------------------
    //! host information struct
    struct host_info_t {
        std::string host;       ///< hostname or host IP Address
        std::string user;       ///< user ID
        std::string password;   ///< password
    };
    //! FTP Information type,
    struct ftp_info_t : public host_info_t {
        std::string remote_path;    ///< remote full path except user's path and add '/' CHRacter at first and last position
        std::string remote_file;    ///< remote file name
        std::string local_path;     ///< local full path with '/' CHRacter at first and last position
        std::string local_file;     ///< local file name
        FILE_TYPE file_type;        ///< file type, ascii or binary
    };
    //! type of network info of client
    struct client_info_t {
        struct sockaddr_in address; ///< address struct of clients
        int socket;                 ///< the socket of clients
    };

    //! Network Message Type Index
    enum NET_MSG_TYPE {
            NET_MSG_TYPE_BOOL = 0
          , NET_MSG_TYPE_CHAR
          , NET_MSG_TYPE_INT
          , NET_MSG_TYPE_LONG
          , NET_MSG_TYPE_LONGLONG
          , NET_MSG_TYPE_UCHAR
          , NET_MSG_TYPE_UINT
          , NET_MSG_TYPE_ULONG
          , NET_MSG_TYPE_ULONGLONG
          , NET_MSG_TYPE_FLOAT
          , NET_MSG_TYPE_DOUBLE
          , NET_MSG_TYPE_STRING
    };
    //! Network Message Type Code
    constexpr char NET_MSG_TYPE_CODE[] = {
          'b'   // boolean
        , 'c'   // char
        , 'i'   // integer
        , 'l'   // long
        , 'x'   // long long
        , 'h'   // unsigned char
        , 'j'   // unsigned int
        , 'm'   // unsigned long
        , 'y'   // unsigned long long
        , 'f'   // float
        , 'd'   // double
        , 's'   // char*, char const *, string, string const &
        };
    // message type for communication on network using TCP/IP
    class net_msg_t {
    public:
        using key_type = std::string;
        using value_type = extlib::any;
        using data_container_t = std::unordered_map<key_type, value_type>;

        int cmd = -1;    ///< command code
        int size() const { return data_container_.size(); }  ///< size of data_container_

        /*! get data_container_
            \return data_container_
        */
        const data_container_t& data_container() const { return data_container_; }
        /*! copy data_container to data_container_
            \param data_container
        */
        void data_container(const data_container_t& data_container) {
            data_container_ = data_container;
        }
        /*! insert an element(key and value) at data_container_
            \param key
            \param value
        */
        void data_container(const key_type& key, const value_type& value);
        /*! get value by key from data_container_
            \param key
            \return a value
        */
        const value_type& data_container(const key_type& key);
        /*! write a net_msg_t value to a file
            \param fd a descriptor of a file
            \return to be witten data size
        */
        int write_to_network(int fd);
        /*! read a net_msg_t value from a file
            \param fd a descriptor of a file
            \return to be read data size
        */
        int read_from_network(int fd);
        //! clear
        void clear();
        //! clear data container
        void clear_data_container();
        //! to string
        const std::string to_string() const;

    private:
        /*! std::string to binary
            \param fd
            \param str
        */
        int write_string_to_network(int fd, const std::string& str);
        /*! binary to std::string
            \param fd
            \param str
        */
        int read_string_from_network(int fd, std::string& str);
        // key type is a string only
        // value type is a any but any type can be used string('s'), double('d'), int('i') only
        data_container_t data_container_;
    }; // class net_msg_t

    //! Database data type
    /*!
        col_list\n
        -> A, B, ... \n
        data_container\n
        [0]-> 1, 2, 3, 4\n
        [1]-> 1, 2, 3, 4\n
        ...
    */
    struct db_data_set_t {
        typedef std::string                 value_type;
        typedef std::vector<value_type>     col_field_t, row_record_t;
        typedef std::vector<row_record_t>   data_container_t;

        col_field_t      col_names;      ///< column names
        data_container_t data_container;  ///< row container
        /*!
            looking for column position by field name
            \param col_name table field name
            \return pos if -1 there isn't any correct field name, otherwise found pos.
        */
        int col_name_pos(const value_type& col_name) const;
        /*!
            get col_name's size
        */
        size_t col_size() const { return col_names.size(); }
        /*!
            get data container's size
        */
        size_t row_size() const { return data_container.size(); }
        //! clear all data
        void clear();
        /*! is empty
            \return if col_names or data container is empty then true, otherwise false
        */
        bool empty() const { return col_names.empty() || data_container.empty(); }
    }; // struct db_data_set_t
    /*! Database data any type
        col_meta_data oracle::occi::MetaData
        data_container extlib::any
    */
    struct db_any_data_set_t {
        typedef oracle::occi::MetaData       ora_meta_data_t;
        typedef std::vector<ora_meta_data_t> col_meta_t;
        typedef extlib::any                  value_type;
        typedef std::vector<value_type>      row_record_t;
        typedef std::vector<row_record_t>    data_container_t;

        col_meta_t          col_meta_data;  ///< column meta data
        data_container_t    data_container; ///< row record container
        /*! looking for column position by field name
            \param col_name table field name
            \return pos if -1 there isn't any correct field name, otherwise found pos.
        */
        int col_name_pos(const std::string& col_name) const;
        /*! get col_meta_data size */
        size_t col_size() const { return col_meta_data.size(); }
        /*! get data container's size */
        size_t row_size() const { return data_container.size(); }
        //! clear all data
        void clear();
        /*! is empty
            \return if col_meta_data or data container is empty then true, otherwise false
        */
        bool empty() const { return col_meta_data.empty() || data_container.empty(); }
    }; // struct db_any_data_set_t

    //! 좌표 데이터 템플릿 구조체
    /*!
        template으로 숫자형 변수에 한하여 동일 타입으로 x,y를 생성하고 관리
    */
    template <typename T>
    struct point {
        typedef T value_type;
        //! constructor
        point() : x(T()), y(T()) {}
        //! overloaded constructor
        point(const T& v1, const T& v2) : x(v1), y(v2) {}
        //! copy constructor
        point(const point<T>& p) : x(p.x), y(p.y) {}
        //! euclidean distance
        T distance() const { return static_cast<T>(std::sqrt(x*x + y*y)); }
        T x;  ///< coordinate x
        T y;  ///< coordinate y
    };

    template <typename T>
    inline bool operator==(const point<T>& p1, const point<T>& p2) {
        return p1.x == p2.x && p1.y == p2.y;
    }

    template <typename T>
    inline bool operator<(const point<T>& p1, const point<T>& p2) {
        return p1.x < p2.x || (!(p2.x < p1.x) && p1.y < p2.y);
    }

    template <typename T>
    inline bool operator!=(const point<T>& p1, const point<T>& p2) {
        return !(p1 == p2);
    }

    template <typename T>
    inline bool operator>(const point<T>& p1, const point<T>& p2) {
        return p2 < p1;
    }

    template <typename T>
    inline bool operator<=(const point<T>& p1, const point<T>& p2) {
        return !(p1 > p2);
    }

    template <typename T>
    inline bool operator>=(const point<T>& p1, const point<T>& p2) {
        return !(p1 < p2);
    }

    //-------------------------------------------------------------------------------------------------------------
    //! recttangle class
    template <typename T>
    struct rect : public point<T> {
        //! constructor
        rect() : point<T>() {}
        //! overloaded constructor using left-top point, width and height
        rect(point<T> ul, T w, T h) : point<T>(ul), width(w), height(h) {}
        //! overloaded constructor using two point
        rect(point<T> ul, point<T> lr) : point<T>(ul), width(lr.x-ul.x), height(lr.y-ul.y) {}
        rect(T x, T y, T w, T h) : point<T>(x,y), width (w), height (h) {}
        //! Returns upper-left coordinate
        const point<T>& ul() const { return point<T>(this->x,this->y);}
        //! Returns lower-right coordinates
        point<T> lr() const { return point<T>(this->x + this->width, this->y - this->height); }
        //! Returns true if the rectangle is null
        bool is_null() const { return width == 0 && height == 0;}
        //! is same
        bool operator==(const rect<T>& r) const { return this->x == r.x && this->y == r.y && this->width == r.width && this->height == r.height; }
        //! not equal
        bool operator!=(const rect<T>& r) const { return !(*this == r); }

        T width;      ///< Image width
        T height;     ///< Image height
    };

    //! 2 Dimension matrix
    template <class T>
    class mat_2d {
    public:
        /* ------------------------------------------------------------------------
            public type definition
           ------------------------------------------------------------------------ */
        typedef size_t      size_type;      ///< matrix row & column size type
        typedef T           value_type;     ///< element value type
        typedef T&          reference;      ///< reference type
        typedef const T&    const_reference;///< const reference type
        typedef T*          iterator;       ///< pointer type
        typedef const T*    const_iterator; ///< const pointer type
        /* ------------------------------------------------------------------------
            construct & destruct
           ------------------------------------------------------------------------ */
        //! Default constructor
        mat_2d();
        //! destructor
        ~mat_2d();
        //! Zero-based array
        /*!
            \param r row size
            \param c colum size
        */
        mat_2d(size_type r, size_type c);
        //! Initialize to constant reference
        /*!
            \param a constant reference value
            \param r row size
            \param c column size
        */
        mat_2d(const T& a, size_type r, size_type c);
        //! Initialize to constant pointer
        /*!
            \param a constant pointer value
            \param r row size
            \param c column size
        */
        mat_2d(const T* a, size_type r, size_type c);
        //! Copy constructor
        /*!
            \param rhs matrix data
            \return made new matrix data
        */
        mat_2d(const mat_2d& rhs);
        //! assignment
        mat_2d& operator=(const mat_2d &rhs);
        //! assign a to every element
        mat_2d& operator=(const T& a);
        //! multiply by scalar
        mat_2d operator*(const T& rhs);
        //! product
        mat_2d operator*(const mat_2d& rhs);
        //! addition
        mat_2d operator+(const mat_2d& rhs);
        //! subtraction
        mat_2d operator-(const mat_2d& rhs);
        /* ------------------------------------------------------------------------
            matrix modify operator
           ------------------------------------------------------------------------ */
        //! rotating row & column of matrix
        /*! cw = clockwise, ccw = counterclockwise
            original matrix (om)

            1 2 3
            4 5 6

            om -> 180  6 5 4   om -> 90ccw  3 6   om -> 90cw  4 1
                       3 2 1                2 5               5 2
                                            1 4               6 3

            om -> flip horizontal  3 2 1   om -> flip vertical  4 5 6
                                   6 5 4                        1 2 3
        */
        void rotate(ROTATE_ANGLE direction);
        /* ------------------------------------------------------------------------
            accepting operator member variables
           ------------------------------------------------------------------------ */
        //! subscripting: point to row i
        iterator operator[](const size_type i) { return data_[i]; }
        //! subscripting: const point to row i
        const_iterator operator[](const size_type i) const { return data_[i]; }
        //! return rows size
        size_type row_size() const {return row_size_;}
        //! return columns size
        size_type col_size() const {return col_size_;}

    private:
        //! data swapper
        void swap(mat_2d<T> & rhs) throw() {
            std::swap(row_size_, rhs.row_size_);
            std::swap(col_size_, rhs.col_size_);
            std::swap(data_, rhs.data_);
        }
        //! rows size
        size_type row_size_;
        //! column size
        size_type col_size_;
        //! data point
        T** data_;
    };

    template <class T>
    mat_2d<T>::mat_2d() : row_size_(0), col_size_(0), data_(0) {}

    template <class T>
    mat_2d<T>::~mat_2d() {
        if (data_ != 0) {
            delete[] (data_[0]);
            delete[] (data_);
        }
    }

    template <class T>
    mat_2d<T>::mat_2d(size_type r, size_type c) : row_size_(r), col_size_(c), data_(new T*[r]) {
        size_type i,j;
        T a = T();
        data_[0] = new T[col_size_*row_size_];
        for (i=1; i< row_size_; i++)
            data_[i] = data_[i-1] + col_size_;
        for (i=0; i< row_size_; i++)
            for (j=0; j<col_size_; j++)
                data_[i][j] = a;
    }

    template <class T>
    mat_2d<T>::mat_2d(const T& a, size_type r, size_type c) : row_size_(r), col_size_(c), data_(new T*[r]) {
        size_type i,j;
        data_[0] = new T[col_size_*row_size_];
        for (i=1; i< row_size_; i++)
            data_[i] = data_[i-1] + col_size_;
        for (i=0; i< row_size_; i++)
            for (j=0; j<col_size_; j++)
                data_[i][j] = a;
    }

    template <class T>
    mat_2d<T>::mat_2d(const T* a, size_type r, size_type c) : row_size_(r), col_size_(c), data_(new T*[r]) {
        size_type i,j;
        data_[0] = new T[col_size_*row_size_];
        for (i=1; i< row_size_; i++)
            data_[i] = data_[i-1] + col_size_;
        for (i=0; i< row_size_; i++)
            for (j=0; j<col_size_; j++)
                data_[i][j] = *a;
    }

    template <class T>
    mat_2d<T>::mat_2d(const mat_2d &rhs) : row_size_(rhs.row_size_), col_size_(rhs.col_size_), data_(new T*[row_size_]) {
        size_type i,j;
        data_[0] = new T[col_size_*row_size_];
        for (i=1; i< row_size_; i++)
            data_[i] = data_[i-1] + col_size_;
        for (i=0; i< row_size_; i++)
            for (j=0; j<col_size_; j++)
                data_[i][j] = rhs[i][j];
    }

    template <class T>
    mat_2d<T>& mat_2d<T>::operator=(const mat_2d<T> &rhs) {
        mat_2d<T> temp(rhs);
        swap(temp);
        return *this;
    }

    template <class T>
    mat_2d<T>& mat_2d<T>::operator=(const T &a) {
        for (size_type i=0; i< row_size_; i++)
            for (size_type j=0; j<col_size_; j++)
                data_[i][j] = a;
        return *this;
    }

    template<typename T>
    mat_2d<T> mat_2d<T>::operator*(const T &rhs) {
        mat_2d<T> res(row_size_, col_size_);
        for(size_t i = 0 ; i < row_size_ ; i++){
            for(size_t j = 0 ; j < col_size_ ; j++){
                res[i][j] = data_[i][j] * rhs;
            }
        }
        return res;
    }

    template<typename T>
    mat_2d<T> mat_2d<T>::operator*(const mat_2d<T> &rhs) {
        if(col_size_ != rhs.get_row_size())
            throw domain_error("col_size_ != rhs.get_row_size()");

        size_t co = col_size_;
        size_t r = row_size_, c = rhs.get_col_size();

        mat_2d<T> res(r, c);
        for(size_t i = 0 ; i < r ; i++){
            for(size_t j = 0 ; j < c ; j++){
                for(size_t c = 0 ; c < co ; c++){
                    res[i][j] += data_[i][c] * rhs[c][j];
                }
            }
        }
        return res;
    }

    template<typename T>
    mat_2d<T> mat_2d<T>::operator+(const mat_2d<T> &rhs)
    {
        if(row_size_ != rhs.get_row_size() || col_size_ != rhs.get_col_size())
            throw domain_error("row_size_ != rhs.get_row_size() || col_size_ != rhs.get_col_size()");

        mat_2d<T> res(row_size_, col_size_);
        for(size_t i = 0 ; i < row_size_ ; i++){
            for(size_t j = 0 ; j < col_size_ ; j++){
                res[i][j] = data_[i][j] + rhs[i][j];
            }
        }
        return res;
    }

    template<typename T>
    mat_2d<T> mat_2d<T>::operator-(const mat_2d<T> &rhs)
    {
        if(row_size_ != rhs.get_row_size() || col_size_ != rhs.get_col_size())
            throw domain_error("row_size_ != rhs.get_row_size() || col_size_ != rhs.get_col_size()");

        mat_2d<T> res(row_size_, col_size_);
        for(size_t i = 0 ; i < row_size_ ; i++){
            for(size_t j = 0 ; j < col_size_ ; j++){
                res[i][j] = data_[i][j] - rhs[i][j];
            }
        }
        return res;
    }

    template <class T>
    void  mat_2d<T>::rotate(ROTATE_ANGLE direction)
    {
        switch (direction) {
        case ROTATE_ANGLE::HALF:
            {
                mat_2d< T > matTemp(row_size_, col_size_);
                for (size_type r = 0 ; r < row_size_ ; r++) {
                    for (size_type c = 0 ; c < col_size_ ; c++) {
                        matTemp.data_[r][c] = data_[row_size_ - (r+1)][col_size_ - (c+1)];
                    }
                }
                swap(matTemp);
            }
            break;
        case ROTATE_ANGLE::CW90:
            {
                std::swap(row_size_, col_size_);
                mat_2d< T > matTemp(row_size_, col_size_);
                for (size_type r = 0 ; r < row_size_ ; r++) {
                    for (size_type c = 0 ; c < col_size_ ; c++) {
                        matTemp.data_[r][c] = data_[col_size_ - (c+1)][r];
                    }
                }
                swap(matTemp);
            }
            break;
        case ROTATE_ANGLE::CCW90:
            {
                std::swap(row_size_, col_size_);
                mat_2d< T > matTemp(row_size_, col_size_);
                for (size_type r = 0 ; r < row_size_ ; r++) {
                    for (size_type c = 0 ; c < col_size_ ; c++) {
                        matTemp.data_[r][c] = data_[c][row_size_ - (r+1)];
                    }
                }
                swap(matTemp);
            }
            break;
        case ROTATE_ANGLE::FLIPHORIZONTAL:
            {
                mat_2d< T > matTemp(row_size_, col_size_);
                for (size_type r = 0 ; r < row_size_ ; r++) {
                    for (size_type c = 0 ; c < col_size_ ; c++) {
                        matTemp.data_[r][c] = data_[r][col_size_ - (c+1)];
                    }
                }
                swap(matTemp);
            }
            break;
        case ROTATE_ANGLE::FLIPVERTICAL:
            {
                mat_2d< T > matTemp(row_size_, col_size_);
                for (size_type r = 0 ; r < row_size_ ; r++) {
                    for (size_type c = 0 ; c < col_size_ ; c++) {
                        matTemp.data_[r][c] = data_[row_size_ - (r+1)][c];
                    }
                }
                swap(matTemp);
            }
            break;
        }
    }

    //! 3 Dimension matrix
    template <class T>
    class mat_3d {
    public:
        typedef size_t  size_type;      // row & column size type
        typedef T       value_type;     // value type

        mat_3d();
        ~mat_3d();
        explicit mat_3d(size_type n, size_type m, size_type k);
        inline T** operator[](const size_type i);
        inline const T* const * operator[](const size_type i) const;
        size_type dim1() const {return nn;}
        size_type dim2() const {return mm;}
        size_type dim3() const {return kk;}

    private:
        size_type nn;        // dimension 1 z
        size_type mm;        // dimension 2 y
        size_type kk;        // dimension 3 x
        T*** data_;
    };

    template <class T>
    mat_3d<T>::mat_3d(): nn(0), mm(0), kk(0), data_(0) {}

    template <class T>
    mat_3d<T>::mat_3d(size_type n, size_type m, size_type k) : nn(n), mm(m), kk(k), data_(new T**[n]) {
        size_type i,j;
        data_[0] = new T*[n*m];
        data_[0][0] = new T[n*m*k];
        for(j=1; j<m; j++)
            data_[0][j] = data_[0][j-1] + k;
        for(i=1; i<n; i++) {
            data_[i] = data_[i-1] + m;
            data_[i][0] = data_[i-1][0] + m*k;
            for(j=1; j<m; j++)
                data_[i][j] = data_[i][j-1] + k;
        }
    }

    template <class T>
    inline T** mat_3d<T>::operator[](const size_type i) {
        return data_[i];
    }

    template <class T>
    inline const T* const * mat_3d<T>::operator[](const size_type i) const {
        return data_[i];
    }

    template <class T>
    mat_3d<T>::~mat_3d() {
        if (data_ != 0) {
            delete[] (data_[0][0]);
            delete[] (data_[0]);
            delete[] (data_);
        }
    }

    // Matrix types
    typedef const mat_2d<bool> Mat_I_BOOL;
    typedef mat_2d<bool> Mat_BOOL, Mat_O_BOOL, Mat_IO_BOOL;

    typedef const mat_2d<CHR> Mat_I_CHR;
    typedef mat_2d<CHR> Mat_CHR, Mat_O_CHR, Mat_IO_CHR;

    typedef const mat_2d<unsigned CHR> Mat_I_UCHR;
    typedef mat_2d<unsigned CHR> Mat_UCHR, Mat_O_UCHR, Mat_IO_UCHR;

    typedef const mat_2d<int> Mat_I_INT;
    typedef mat_2d<int> Mat_INT, Mat_O_INT, Mat_IO_INT;

    typedef const mat_2d<unsigned int> Mat_I_UINT;
    typedef mat_2d<unsigned int> Mat_UINT, Mat_O_UINT, Mat_IO_UINT;

    typedef const mat_2d<long> Mat_I_LNG;
    typedef mat_2d<long> Mat_LNG, Mat_O_LNG, Mat_IO_LNG;

    typedef const mat_2d<unsigned long> Mat_I_ULNG;
    typedef mat_2d<unsigned long> Mat_ULNG, Mat_O_ULNG, Mat_IO_ULNG;

    typedef const mat_2d<float> Mat_I_FLT;
    typedef mat_2d<float> Mat_FLT, Mat_O_FLT, Mat_IO_FLT;

    typedef const mat_2d<DBL> Mat_I_DBL;
    typedef mat_2d<DBL> Mat_DBL, Mat_O_DBL, Mat_IO_DBL;

    typedef const mat_2d<std::complex<float> > Mat_I_CPLX_FLT;
    typedef mat_2d<std::complex<float> > Mat_CPLX_FLT, Mat_O_CPLX_FLT, Mat_IO_CPLX_FLT;

    typedef const mat_2d<std::complex<DBL> > Mat_I_CPLX_DBL;
    typedef mat_2d<std::complex<DBL> > Mat_CPLX_DBL, Mat_O_CPLX_DBL, Mat_IO_CPLX_DBL;

    // 3D Matrix types
    typedef const mat_3d<DBL> mat_3d_I_DBL;
    typedef mat_3d<DBL> mat_3d_DBL, mat_3d_O_DBL, mat_3d_IO_DBL;
} // end coral namespace

/*! overloading operator<< for net_msg
    \param os output stream
    \param msg point
    \return output stream
*/
std::ostream& operator<<(std::ostream& os, coral::net_msg_t& msg);

/*! overloading operator>> for point
    \param is input stream
    \param p point
    \return input stream
*/
template <typename T>
std::istream& operator>>(std::istream& is, coral::point<T>& p) {
    is >> p.x >> p.y;
    return is;
}
/*! overloading operator<< for point
    \param os output stream
    \param p point
    \return output stream
*/
template <typename T>
std::ostream & operator<<(std::ostream &os, const coral::point<T> &p) {
    os << p.x << ',' << p.y;
    return os;
}
/*! overloading operator>> for rect
    \param is input stream
    \param rect rect
    \return input stream
*/
template <typename T>
std::istream& operator>>(std::istream& is, coral::rect<T>& rect)
{
    is >> rect.x >> rect.y >> rect.width >> rect.height;
    return is;
}
/*! overloading operator<< for rect
    \param os output stream
    \param rect rect
    \return output stream
*/
template <typename T>
std::ostream& operator<<(std::ostream& os, const coral::rect<T>& rect) {
    os << rect.x << ',' << rect.y << ',' << rect.width << ',' << rect.height;
    return os;
}
/*! overloading operator<< for db_data_set_t
    \param os outstream
    \param db_data_set db_data_set_t
    \return output stream
*/
std::ostream& operator<<(std::ostream& os, const coral::db_data_set_t& db_data_set);
/*! overloading operator<< for mat_2d<T>
    \param os outstream
    \param mat mat_2d
    \return output stream
*/
template<typename T>
std::ostream& operator<<(std::ostream &os, const coral::mat_2d<T>& mat)
{
    for(size_t r = 0 ; r < mat.row_size() ; r++) {
        for(size_t c = 0 ; c < mat.col_size() ; c++) {
            os << mat[r][c] << ',';
        }
        os << '\n';
    }

    return os;
}

#endif //__CORAL_TYPES_H__
