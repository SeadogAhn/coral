/*!
    \file       alarm_controller.h
    \brief      Alarm signal control
    \details    using time alarm from unix signal
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/

#ifndef __CORAL_ALARMCONTROLLER_H__
#define __CORAL_ALARMCONTROLLER_H__

#include <signal.h>
#include <unistd.h>

#include "exception.h"

//! Core Library for Applications and Libraries
namespace coral {
    //! alarm controler class
    class  alarm_controller
    {
    public:
        //! default construct
        alarm_controller()          { init_signal(); }
        //! construct overloading
        alarm_controller(int n)     { init_signal(); alarm(n); }
        //! default destructor
        ~alarm_controller()         { alarm(0); }
        //! time setting - second
        void set_duration(int n)    { alarm(n); }
        //! alarm handler
        static void exec_alarm_handler(int signum) {
            throw coral::timeout_error("the time has passed.");
        }

    private:
        // copy construct & assignment operator hide.
        alarm_controller(const alarm_controller&);
        void operator=(const alarm_controller&);
        //! signal function initialize
        void init_signal() {
            ::signal(SIGALRM, coral::alarm_controller::exec_alarm_handler);
        }
    }; // alarm_controller
}; // coral namesapce

#endif //__CORAL_ALARMCONTROLLER_H__
