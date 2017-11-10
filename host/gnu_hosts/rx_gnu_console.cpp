

/****************************************************************************
*
*  host\gnu_hosts\rx_gnu_console.cpp
*
*  Copyright (c) 2017 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"


// rx_gnu_console
#include "host/gnu_hosts/rx_gnu_console.h"


#include <signal.h>

namespace
{

pthread_t main_pid;

void sig_handler(int s)
{
    printf("\r\n");
}


} // anonymous namespace


namespace host {

namespace gnu {

// Class host::gnu::gnu_console_host 

gnu_console_host::gnu_console_host()
{
}


gnu_console_host::~gnu_console_host()
{
}



bool gnu_console_host::shutdown (const string_type& msg)
{
    if(host::interactive::interactive_console_host::shutdown(msg))
    {
        int ret=pthread_kill(main_pid,SIGINT);
        if(ret<0)
            perror("kill");
        return true;
    }
    return false;
}

sys_handle_t gnu_console_host::get_host_test_file (string_type& path)
{
    string_type full_path;
    rx::combine_paths(TEST_SCRIPT_PATHS,path,full_path);
    sys_handle_t file=rx_file(full_path.c_str(),RX_FILE_OPEN_READ,RX_FILE_OPEN_EXISTING);
    return file;
}

bool gnu_console_host::start (const string_array& args)
{
    main_pid=pthread_self();

    char buff[0x100];
    gethostname(buff,sizeof(buff));

    rx_thread_data_t tls=rx_alloc_thread_data();
    rx_initialize_os(getpid(),true,tls,buff);

    rx::log::log_object::instance().start(std::cout,true);



	struct sigaction act;
	struct sigaction old;

	memzero(&act,sizeof(act));

	act.sa_handler=sig_handler;

    int ret = sigaction(SIGINT,&act,&old);
    if(ret!=0)
        perror("sigaction");


	HOST_LOG_INFO("Main", 999, "Starting Console Host...");


	server::configuration_data_t config;

	// execute main loop of the console host
	console_loop(config);


	HOST_LOG_INFO("Main", 999, "Console Host exited.");

	rx::log::log_object::instance().deinitialize();

	rx_deinitialize_os();

	return true;

}


} // namespace gnu
} // namespace host

