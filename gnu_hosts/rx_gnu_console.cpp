

/****************************************************************************
*
*  gnu_hosts\rx_gnu_console.cpp
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


//#include <readline/readline.h>
//#include <readline/history.h>

#include "rx_gnu_console_version.h"
#include "rx_gnu_common.h"

// rx_gnu_console
#include "gnu_hosts/rx_gnu_console.h"


void dump_termio(struct termios* tm)
{
    return;
    //printf("Termios:\r\nc_iflag:%08x\r\nc_oflag:%08xc_iflag:%08x")

    int flags;

	flags=fcntl(STDIN_FILENO, F_GETFL, 0);
	if(flags<0)
	{
        perror("Error reading fl");
	}
}

namespace
{

pthread_t main_pid;

std::atomic<uint_fast8_t> g_console_canceled(0);
std::function<void()> g_term_size_callback;

void sig_handler(int s)
{
	if(s==SIGINT)
		g_console_canceled.store(1);
}

void sig_handler_swinch(int s)
{
	if (s == SIGWINCH && g_term_size_callback)
		g_term_size_callback();
}



} // anonymous namespace


namespace gnu {

// Class gnu::gnu_console_host 

gnu_console_host::gnu_console_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : width_(0),
        height_(0),
        win_changed_(true)
	, host::interactive::interactive_console_host(storages)
{
	g_term_size_callback = [this]()
	{
		adjust_terminal_size();
	};
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

void gnu_console_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_gnu_interactive_info());
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
  static bool first=true;

  if(first)
  {
    first=false;
    adjust_terminal_size();
  }

  int ret_val = read(STDIN_FILENO,&chars[0],0x100);
  if(ret_val<0)
  {
    return false;
  }
  else
  {
    count=ret_val;
    return true;
  }
}

bool gnu_console_host::write_stdout (const void* data, size_t size)
{
  uint32_t written=0;
  bool ret = RX_OK == rx_file_write(STDOUT_FILENO,data,size,&written);
  return ret;
}

rx_result gnu_console_host::setup_console (int argc, char* argv[])
{
	main_pid = pthread_self();

	// setup blocking calls for stdin


	termios ttynew;

	if (tcgetattr(STDIN_FILENO, &ttyold_) != 0)
	{
		perror("Failed getting terminal attributes\n");
	}
	else
	{
		ttynew = ttyold_;

        cfmakeraw(&ttynew);
		//ttynew.c_iflag = 0;
		//ttynew.c_oflag = 0;

		// disable canonical mode (don't buffer by line)
		//ttynew.c_lflag &= ~ICANON;

		// disable local echo
		//ttynew.c_lflag &= ~ECHO;

		// do not process ETX (Ctrl-C)
	//	ttynew.c_lflag &= ~VINTR;

	//	ttynew.c_cc[VMIN] = 1;
	//	ttynew.c_cc[VTIME] = 1;

		// set new terminal attributes
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ttynew) != 0)
			perror("Failed setting terminal attributes\n");
	}


	struct sigaction act;
	struct sigaction old;

	memzero(&act, sizeof(act));

	act.sa_handler = sig_handler;

	int ret = sigaction(SIGINT, &act, &old);
	if (ret != 0)
		perror("sigaction1");
	ret = sigaction(SIGHUP, &act, &old);
	if (ret != 0)
		perror("sigaction2");

    signal(SIGWINCH, sig_handler_swinch);

	return true;
}

void gnu_console_host::restore_console ()
{
	//set old terminal attributes
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ttyold_) != 0)
		perror("Failed setting terminal attributes\n");

}

string_type gnu_console_host::get_gnu_interactive_info ()
{
	static char ret[0x100] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_GNU_CON_HOST_NAME, RX_GNU_CON_HOST_MAJOR_VERSION, RX_GNU_CON_HOST_MINOR_VERSION, RX_GNU_CON_HOST_BUILD_NUMBER);
	}
	return ret;
}

rx_result gnu_console_host::fill_host_directories (hosting::rx_host_directories& data)
{
	return build_directories(data);
}

bool gnu_console_host::supports_ansi () const
{
  return true;

}

string_type gnu_console_host::get_full_path (const string_type& path)
{
    return get_full_path_from_relative(path);
}

void gnu_console_host::adjust_terminal_size ()
{
    struct winsize winsz;

    ioctl(0, TIOCGWINSZ, &winsz);
    if (width_ != (int)winsz.ws_col || height_ != (int)winsz.ws_row)
    {
        width_ = (int)winsz.ws_col;
        height_ = (int)winsz.ws_row;
        terminal_size_changed(width_, height_);
    }
}

void gnu_console_host::fill_plugin_libs (string_array& paths)
{
	fill_plugin_paths(paths);
}


} // namespace gnu

