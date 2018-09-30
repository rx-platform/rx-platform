

/****************************************************************************
*
*  host\win32_hosts\rx_win32_console.cpp
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

// rx_win32_console
#include "host/win32_hosts/rx_win32_console.h"


namespace
{
	BOOL ctrl_handler(DWORD fdwCtrlType)
	{
		switch (fdwCtrlType)
		{
			// Handle the CTRL-C signal. 
		case CTRL_C_EVENT:
			printf("\r\n");
			return TRUE;
			// CTRL-CLOSE: confirm that the user wants to exit. 
		case CTRL_CLOSE_EVENT:
			printf("\r\n");
			return TRUE;
		default:
			return FALSE;
		}
	}
}// anonymous


namespace host {

namespace win32 {

// Class host::win32::win32_console_host 

win32_console_host::win32_console_host()
{
}


win32_console_host::~win32_console_host()
{
}



bool win32_console_host::shutdown (const string_type& msg)
{
	if (host::interactive::interactive_console_host::shutdown(msg))
	{
		DWORD dwTmp;
		INPUT_RECORD ir[2];
		ir[0].EventType = KEY_EVENT;
		ir[0].Event.KeyEvent.bKeyDown = TRUE;
		ir[0].Event.KeyEvent.dwControlKeyState = 0;
		ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
		ir[0].Event.KeyEvent.wRepeatCount = 0;
		ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
		ir[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
		ir[1] = ir[0];
		ir[1].Event.KeyEvent.bKeyDown = FALSE;
		WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 1, &dwTmp);
		return true;
	}
	return false;
}

sys_handle_t win32_console_host::get_host_test_file (const string_type& path)
{
	string_type full_path;
	rx::combine_paths(RX_TEST_SCRIPTS_PATH, path, full_path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

bool win32_console_host::start (const string_array& args)
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_handler, TRUE);

	rx_thread_data_t tls = rx_alloc_thread_data();

	char name[0x100];
	DWORD szname = sizeof(name);
	GetComputerNameA(name, &szname);

	rx_initialize_os(GetCurrentProcessId(), true, tls, name);

	rx::log::log_object::instance().start(std::cout, true);

	HANDLE std_out;
	DWORD mode;

	std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(std_out, &mode);
	mode |= 0x4;
	SetConsoleMode(std_out, mode);


	HOST_LOG_INFO("Main", 999, "Starting Console Host...");


	server::configuration_data_t config;

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
	rx::combine_paths(RX_CONSOLE_SCRIPTS_PATH, path, full_path);
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
	interactive::interactive_console_host::get_host_info(hosts);
}


} // namespace win32
} // namespace host

