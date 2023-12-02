/*!
    \file       variables.h
    \brief      CORAL Global variables
    \details    Environment & Defined directories
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_VARIABLES_H__
#define __CORAL_VARIABLES_H__

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <atomic>

//environment CORALDIR=$CORAL_HOME, you must define the $CORAL_HOME variable on default shell
#ifndef CORALDIR
#define CORALDIR "CORAL_HOME"
#endif

//! Core Library for Applications and Libraries
namespace coral {
    //--------------------------------------------------------------------------------
    // Directories Name Initialize
    //--------------------------------------------------------------------------------
    const std::string gv_main_dir                 = std::string(std::getenv(CORALDIR));    ///< MAIN directory
    const std::string gv_dir_root                 = gv_main_dir + "/";              ///< MAIN directory
    const std::string gv_dir_bin                  = gv_dir_root + "bin/";           ///< excutable application directory
    const std::string gv_dir_log                  = gv_dir_root + "log/";           ///< log directory
    const std::string gv_dir_config               = gv_dir_root + "config/";        ///< system configuration directory
    const std::string gv_dir_metafiles            = gv_dir_root + "metafiles/"; ///< metafile directory
    //--------------------------------------------------------------------------------
    //! Socket Message Buffer Size = 1Kbyte
    const size_t gv_string_msg_size = 0x400;
} // end CORAL namespace

#endif //__CORAL_VARIABLES_H__
