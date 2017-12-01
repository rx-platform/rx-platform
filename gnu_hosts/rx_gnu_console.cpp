

/****************************************************************************
*
*  gnu_hosts\rx_gnu_console.cpp
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


//#include <readline/readline.h>
//#include <readline/history.h>

#include "rx_gnu_console_version.h"

// rx_gnu_console
#include "gnu_hosts/rx_gnu_console.h"


#include <signal.h>

namespace
{

pthread_t main_pid;

std::atomic<uint_fast8_t> g_console_canceled(0);

void sig_handler(int s)
{
	if(s==SIGINT)
		g_console_canceled.store(1);
}


} // anonymous namespace


namespace gnu {

// Class gnu::gnu_console_host

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
        return true;
    }
    return false;
}

sys_handle_t gnu_console_host::get_host_test_file (const string_type& path)
{
    string_type full_path;
    rx_combine_paths(RX_TEST_SCRIPTS_PATHS,path,full_path);
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
        perror("sigaction1");
    ret = sigaction(SIGHUP,&act,&old);
    if(ret!=0)
        perror("sigaction2");

	HOST_LOG_INFO("Main", 999, "Starting Console Host...");


	rx_platform::configuration_data_t config;

	// execute main loop of the console host
	console_loop(config);


	HOST_LOG_INFO("Main", 999, "Console Host exited.");

	rx::log::log_object::instance().deinitialize();

	rx_deinitialize_os();

	return true;

}

sys_handle_t gnu_console_host::get_host_console_script_file (const string_type& path)
{
    string_type full_path;
    rx_combine_paths(RX_CONSOLE_SCRIPTS_PATHS,path,full_path);
    sys_handle_t file=rx_file(full_path.c_str(),RX_FILE_OPEN_READ,RX_FILE_OPEN_EXISTING);
    return file;
}

void gnu_console_host::get_host_info (string_array& hosts)
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_GNU_HOST_NAME, RX_GNU_HOST_MAJOR_VERSION, RX_GNU_HOST_MINOR_VERSION, RX_GNU_HOST_BUILD_NUMBER);
	}
	hosts.emplace_back(ret);
	host::interactive::interactive_console_host::get_host_info(hosts);
}

bool gnu_console_host::get_next_line (string_type& line)
{
  //std::cin>>line;
  //char* ret=getline(nullptr);
  //line=ret;
  //free(ret);
  std::getline(std::cin,line);
  return true;
}

bool gnu_console_host::is_canceling () const
{
	return g_console_canceled.exchange(0) != 0;
}

bool gnu_console_host::break_host (const string_type& msg)
{
    int ret=pthread_kill(main_pid,SIGHUP);
    if(ret<0)
        perror("kill");
    return true;
}


} // namespace gnu

