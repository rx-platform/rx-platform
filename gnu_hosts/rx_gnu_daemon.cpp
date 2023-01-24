

/****************************************************************************
*
*  gnu_hosts\rx_gnu_daemon.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"

#include "rx_gnu_daemon_version.h"

// rx_gnu_daemon
#include "gnu_hosts/rx_gnu_daemon.h"

#include "third-party/cxxopts/include/cxxopts.hpp"
#include <sys/stat.h>
bool g_forked_daemon = false;


namespace gnu {

// Class gnu::gnu_daemon_host 

gnu_daemon_host::gnu_daemon_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : stop_(false),
        is_daemon_(false)
  , gnu_headless_host(storages)
{
}


gnu_daemon_host::~gnu_daemon_host()
{
}



int gnu_daemon_host::daemon_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins)
{
    if(argc < 2 || strcmp(argv[1], "--nodaemon") != 0)
    {
        pid_t pid, sid;
        // fork the deamon
        pid=fork();
        if(pid<0)
        {
            // fork error?
            return -1;
        }
        else if(pid>0)
        {
            // startup process exiting
            return 0;
        }
        else
        {
            is_daemon_ = true;
            // we are a deamon now!
            umask(0);

            sid=setsid();
            if(sid<0)
            {
                printf("Jbg 7 %d, %s\r\n", errno, strerror(errno));
                // log error?
                return -1;
            }
            else
            {
                /* Change the current working directory */
                if ((chdir("/")) < 0) {
                    /* Log any failure here */
                    return -7;
                }


                /* Close out the standard file descriptors */
                close(STDIN_FILENO);

                return headless_main(argc,argv,plugins);
            }
        }
    }

    return headless_main(argc,argv,plugins);
}

int gnu_daemon_host::headless_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins)
{
    rx_result result = initialize_platform(argc, argv, RX_GNU_DAEMON_HOST
        , log::log_subscriber::smart_ptr::null_ptr
        , host::headless::synchronize_callback_t(), plugins);
    if(result)
    {
        result = start_platform();
        if(result)
        {
            stop_.wait_handle();

            stop_platform();
        }
        deinitialize_platform();
    }
    if(!result)
    {
        printf("ERROR:%s", result.errors_line().c_str());
        return -2;
    }
    return 0;
}

void gnu_daemon_host::get_host_info (string_array& hosts)
{
    hosts.emplace_back(get_gnu_daemon_host_info());
    host::headless::headless_platform_host::get_host_info(hosts);
}

string_type gnu_daemon_host::get_gnu_daemon_host_info ()
{
    static char ret[0x60] = { 0 };
    if (!ret[0])
    {
        ASSIGN_MODULE_VERSION(ret, RX_GNU_DAEMON_HOST_NAME, RX_GNU_DAEMON_HOST_MAJOR_VERSION, RX_GNU_DAEMON_HOST_MINOR_VERSION, RX_GNU_DAEMON_HOST_BUILD_NUMBER);
    }
    return ret;
}

string_type gnu_daemon_host::get_host_name ()
{
    return RX_GNU_DAEMON_HOST;
}

bool gnu_daemon_host::shutdown (const string_type& msg)
{
    stop_.set();
    return true;
}

void gnu_daemon_host::add_command_line_options (hosting::command_line_options_t& options, rx_platform::configuration_data_t& config)
{
    static bool dummy = false;// dummy it is parsed at start and has to be the first one
    gnu_headless_host::add_command_line_options(options, config);
    options.add_options()
        ("nodaemon", "Start as a regular process rather than as a daemon.", cxxopts::value<bool>(dummy));
}

string_type gnu_daemon_host::get_default_user_storage () const
{
    return "/etc/rx-platform/storage/rx-daemon";
}


} // namespace gnu

