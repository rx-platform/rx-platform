

/****************************************************************************
*
*  win32_hosts\rx_win32_interactive.cpp
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

#include "rx_win32_console_version.h"

// rx_win32_interactive
#include "win32_hosts/rx_win32_interactive.h"

namespace
{
	std::atomic<uint_fast8_t> g_console_canceled = 0;
	BOOL ctrl_handler(DWORD fdwCtrlType)
	{
		switch (fdwCtrlType)
		{
			// Handle the CTRL-C signal. 
		case CTRL_C_EVENT:
			g_console_canceled.store(1, std::memory_order_relaxed);
			return TRUE;
			// CTRL-CLOSE: confirm that the user wants to exit. 
		case CTRL_BREAK_EVENT:
			g_console_canceled.store(1, std::memory_order_relaxed);
			return TRUE;
		case CTRL_CLOSE_EVENT:
			return TRUE;
		default:
			return FALSE;
		}
	}
}


namespace win32 {

// Class win32::win32_console_host 

win32_console_host::win32_console_host()
      : out_handle_(NULL),
        in_handle_(NULL)
{
}


win32_console_host::~win32_console_host()
{
}



bool win32_console_host::shutdown (const string_type& msg)
{
	if (host::interactive::interactive_console_host::shutdown(msg))
	{		
		return true;
	}
	return false;
}

sys_handle_t win32_console_host::get_host_test_file (const string_type& path)
{
	string_type full_path;
	rx_combine_paths(RX_TEST_SCRIPTS_PATH, path, full_path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

bool win32_console_host::start (const string_array& args)
{
	BOOL ret = SetConsoleCtrlHandler(NULL,FALSE);
	ret = SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_handler, TRUE);

	rx_thread_data_t tls = rx_alloc_thread_data();

	char name[0x100];
	DWORD szname = sizeof(name);
	GetComputerNameA(name, &szname);

	rx_initialize_os(GetCurrentProcessId(), true, tls, name);

	rx::log::log_object::instance().start(std::cout, true);
	//////////////////////////////////////////////

	
	
	HOST_LOG_INFO("Main", 999, "Starting Console Host...");


	/////////////////////////////////////////////
	out_handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
	in_handle_ = GetStdHandle(STD_INPUT_HANDLE);

	DWORD in_mode = 0;
	DWORD out_mode = 0;
	GetConsoleMode(in_handle_, &in_mode);
	GetConsoleMode(out_handle_, &out_mode);

	std::bitset<32> in_bits(in_mode);
	std::bitset<32> out_bits(out_mode);

	in_bits.reset(0);
	in_bits.reset(1);
	in_bits.reset(2);
	in_bits.set(3);
	in_bits.set(9);

	out_bits.set(2);
	//out_bits.reset(0);
	//out_bits.reset(3);

	SetConsoleMode(in_handle_, in_bits.to_ulong());
	SetConsoleMode(out_handle_, out_bits.to_ulong());


	rx_platform::configuration_data_t config;

	// execute main loop of the console host
	console_loop(config);


	HOST_LOG_INFO("Main", 999, "Console Host exited.");

	rx::log::log_object::instance().deinitialize();

	rx_deinitialize_os();

	return true;

}

sys_handle_t win32_console_host::get_host_console_script_file (const string_type& path)
{
	string_type full_path;
	rx_combine_paths(RX_CONSOLE_SCRIPTS_PATH, path, full_path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

void win32_console_host::get_host_info (string_array& hosts)
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_WIN32_HOST_NAME, RX_WIN32_HOST_MAJOR_VERSION, RX_WIN32_HOST_MINOR_VERSION, RX_WIN32_HOST_BUILD_NUMBER);
	}
	hosts.emplace_back(ret);
	host::interactive::interactive_console_host::get_host_info(hosts);
}

bool win32_console_host::is_canceling () const
{
	return g_console_canceled.exchange(0) != 0;
}

bool win32_console_host::break_host (const string_type& msg)
{
	return GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0) != FALSE;
}

bool win32_console_host::read_stdin (std::array<char,0x100>& chars, size_t& count)
{
	DWORD read = 0;
	bool ret = (FALSE != ReadFile(in_handle_, &chars[0], 0x100, &read, NULL));
	count = read;
	return ret;
}

bool win32_console_host::write_stdout (const string_type& lines)
{
	DWORD written = 0;
	return FALSE != WriteFile(out_handle_, lines.c_str(), (DWORD)lines.size(), &written, NULL);
}


} // namespace win32

