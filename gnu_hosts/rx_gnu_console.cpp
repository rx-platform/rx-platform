

/****************************************************************************
*
*  gnu_hosts\rx_gnu_console.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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

gnu_console_host::gnu_console_host (rx_platform::hosting::rx_platform_storage::smart_ptr storage)
	: host::interactive::interactive_console_host(storage)
{
}


gnu_console_host::~gnu_console_host()
{
}



bool gnu_console_host::shutdown (const string_type& msg)
{
    if(host::interactive::interactive_console_host::shutdown(msg))
    {
        //set old terminal attributes
		if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyold_) != 0)
			fprintf(stderr, "Failed setting terminal attributes\n");
        return true;
    }
    return false;
}

bool gnu_console_host::start (const string_array& args)
{
    main_pid=pthread_self();

    char buff[0x100];
    gethostname(buff,sizeof(buff));

    rx_thread_data_t tls=rx_alloc_thread_data();
    rx_initialize_os(getpid(),true,tls,buff);

    rx::log::log_object::instance().start(std::cout,true);

	termios ttynew;

	if (tcgetattr(STDIN_FILENO, &ttyold_) != 0)
	{
		fprintf(stderr, "Failed getting terminal attributes\n");
    }
    else
    {
		ttynew = ttyold_;

		ttynew.c_iflag = 0;
		ttynew.c_oflag = 0;

		// disable canonical mode (don't buffer by line)
		ttynew.c_lflag &= ~ICANON;

		// disable local echo
		ttynew.c_lflag &= ~ECHO;

		ttynew.c_cc[VMIN] = 1;
		ttynew.c_cc[VTIME] = 1;

		// set new terminal attributes
		if (tcsetattr(STDIN_FILENO, TCSANOW, &ttynew) != 0)
			fprintf(stderr, "Failed setting terminal attributes\n");
	}



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

bool gnu_console_host::read_stdin (std::array<char,0x100>& chars, size_t& count)
{
  uint32_t read=0;
  bool ret = (RX_OK == rx_file_read(STDIN_FILENO,&chars[0],0x100, &read));
  count=read;
  return ret;
}

bool gnu_console_host::write_stdout (const void* data, size_t size)
{
  uint32_t written=0;
  bool ret = RX_OK == rx_file_write(STDOUT_FILENO,data,size,&written);
  return ret;
}


} // namespace gnu

