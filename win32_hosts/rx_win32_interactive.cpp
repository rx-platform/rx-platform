

/****************************************************************************
*
*  win32_hosts\rx_win32_interactive.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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

#include "rx_win32_console_version.h"
#include "rx_win32_common.h"

// rx_win32_interactive
#include "win32_hosts/rx_win32_interactive.h"

#define WINDOWS_TERMINAL_COLOR_BLACK		0
#define WINDOWS_TERMINAL_COLOR_BLUE			1
#define WINDOWS_TERMINAL_COLOR_GREEN		2
#define WINDOWS_TERMINAL_COLOR_CYAN			3
#define WINDOWS_TERMINAL_COLOR_RED			4
#define WINDOWS_TERMINAL_COLOR_MAGENTA		5
#define WINDOWS_TERMINAL_COLOR_BROWN		6
#define WINDOWS_TERMINAL_COLOR_LIGHTGRAY	7
#define WINDOWS_TERMINAL_COLOR_DARKGRAY		8
#define WINDOWS_TERMINAL_COLOR_LIGHTBLUE	9
#define WINDOWS_TERMINAL_COLOR_LIGHTGREEN	10
#define WINDOWS_TERMINAL_COLOR_LIGHTCYAN	11
#define WINDOWS_TERMINAL_COLOR_LIGHTRED		12
#define WINDOWS_TERMINAL_COLOR_LIGHTMAGENTA	13
#define WINDOWS_TERMINAL_COLOR_YELLOW		14
#define WINDOWS_TERMINAL_COLOR_WHITE		15

#define WINDOWS_TERMINAL_SET_COLOR_ATTRIBUTE(a, c) ((a&0xfff8)|c)



namespace
{

void move_cur_horizontal(HANDLE hndl, int amount, CONSOLE_SCREEN_BUFFER_INFO& console_info)
{
	int current_x = console_info.dwCursorPosition.X;
	int max = console_info.dwSize.X;
	if (max > 0)
	{
		int result = current_x + amount;
		if (result < 0)
			result = 0;
		else if (result >= max)
			result = max - 1;
		COORD cord;
		cord.X = result;
		cord.Y = console_info.dwCursorPosition.Y;
		SetConsoleCursorPosition(hndl, cord);
	}
}
void move_cur_left(HANDLE hndl, int amount, CONSOLE_SCREEN_BUFFER_INFO& console_info)
{
	if (amount == 0)
		amount = 1;
	move_cur_horizontal(hndl, -amount, console_info);
}
void move_cur_right(HANDLE hndl, int amount, CONSOLE_SCREEN_BUFFER_INFO& console_info)
{
	if (amount == 0)
		amount = 1;
	move_cur_horizontal(hndl, amount, console_info);
}

typedef uint32_t dword;
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
		return TRUE;// break it
	case CTRL_CLOSE_EVENT:
		return TRUE;
	default:
		return FALSE;
	}
}


typedef struct mac_addr_t
{
	byte addr[MAC_ADDR_SIZE];
} mac_addr_t;

typedef struct eth_adapter_def
{
	char* name;
	char* desc;
	byte mac_address[MAC_ADDR_SIZE];
	dword index;
	dword state;
} eth_adapter;

typedef struct eth_adapters_list_def
{
	size_t count;
	eth_adapter* adapters;
} eth_adapters;



DWORD get_ip_addresses(size_t* count, char*** addresses, char*** names, ip_addr_ctx_t** ctxs, dword** states)
{

	DWORD dwRetVal = 0;
	int i = 0;
	ULONG outBufLen = 0;
	PIP_ADAPTER_ADDRESSES pAddresses;
	PIP_ADAPTER_ADDRESSES pCurrAddresses;
	IP_ADDR_STRING Addresses;

	IP_ADAPTER_INFO info[20];
	char** locnames;
	ip_addr_ctx_t* locctxs;
	dword* locstates;
	size_t idx;
	char** locaddrs;

	ULONG size = sizeof(info);

	char* itf_name;
	char* conn_name;
	DWORD ret;

	ret = GetAdaptersInfo(info, &size);

	outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);

	dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen);

	if (dwRetVal == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
		dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen);
	}
		
	if (ret == ERROR_SUCCESS)
	{
		IP_ADAPTER_INFO* iter = info;
			
		*count = 0;
		// iterate to get count first
		do
		{

			if (iter->Type == MIB_IF_TYPE_ETHERNET)// || iter->Type==IF_TYPE_IEEE80211)
				(*count)++;

			iter = iter->Next;

		} while (iter);

		//allocate some space
		*names = (char**)malloc((*count) * sizeof(char**));
		*addresses = (char**)malloc((*count) * sizeof(char**));
		*ctxs = (ip_addr_ctx_t*)malloc((*count) * sizeof(dword));
		*states = (dword*)malloc((*count) * sizeof(dword));

		// easier this way
		locnames = *names;
		locaddrs = *addresses;
		locctxs = *ctxs;
		locstates = *states;

		idx = 0;
		iter = info;

		do
		{
			if (iter->Type == MIB_IF_TYPE_ETHERNET)// || iter->Type==IF_TYPE_IEEE80211)
			{
				itf_name = NULL;
				conn_name = NULL;
				int i = 0;
				// try to find it in addresses
				for (pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
				{
					locnames[i] = iter->Description;
					Addresses = iter[i].IpAddressList;
					while (Addresses.Next)
					{
						locnames[i] = Addresses.IpAddress.String;
					}
					i++;
						
				}
				idx++;
					
			}

			iter = iter->Next;

		} while (iter);

	}

	*count = idx;
		
	return ret;
}


size_t list_ip_adapters(IP_interface** interfaces)
{
	size_t ret_count = 0;
	size_t count;
	dword ret;
	size_t i;
	IP_interface* padapters;
	char** names;
	char** addresses;
	ip_addr_ctx_t* ctxs;
	dword* states;
		
	ret = get_ip_addresses(&count, &addresses, &names, &ctxs, &states);

	if (ERROR_SUCCESS == ret)
	{
		if (count > 0)
		{
			padapters = (IP_interface*)malloc(sizeof(IP_interface)*count);
			for (i = 0; i <count; i++)
			{
				strcpy(padapters[i].name, names[i]);
				strcpy(padapters[i].ip_address, addresses[i]);
				padapters[i].index = ctxs[i];
				padapters[i].status = interface_status_disconnected;
				ret_count++;
			}
			free(names);
			free(addresses);
			free(ctxs);
			free(states);
		}
	}
	return ret;
}

}


namespace win32 {

// Class win32::win32_console_host 

win32_console_host::win32_console_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : out_handle_(NULL),
        in_handle_(NULL),
        supports_ansi_(false),
        default_attribute_(0),
        no_ansi_(false),
        use_ansi_(false),
        width_(0),
        height_(0)
	, host::interactive::interactive_console_host(storages)
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

void win32_console_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_win32_interactive_info());
	host::interactive::interactive_console_host::get_host_info(hosts);
}

bool win32_console_host::is_canceling () const
{
	return g_console_canceled.exchange(0) != 0;
}

bool win32_console_host::break_host (const string_type& msg)
{
	return GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0) != FALSE;
}

bool win32_console_host::read_stdin (std::array<char,0x100>& chars, size_t& count)
{
	DWORD read = 0;
	CONSOLE_READCONSOLE_CONTROL ctrl;
	ZeroMemory(&ctrl, sizeof(ctrl));
	ctrl.nLength = sizeof(ctrl);
	ctrl.dwCtrlWakeupMask = (1 << 4) | (1 << 26) | (1 << 3);
	bool has = false;

	CONSOLE_SCREEN_BUFFER_INFO con_info;
	int temp_width, temp_height;

	while (!has && !exit())
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(in_handle_, 20))
		{
			//// see about the sizes
			BOOL info = GetConsoleScreenBufferInfo(out_handle_, &con_info);
			if (info)
			{
				temp_width = con_info.srWindow.Right - con_info.srWindow.Left;
				temp_height = con_info.srWindow.Bottom - con_info.srWindow.Top;
				DWORD err = GetLastError();
				if (info && (temp_width != width_ || temp_height != height_))
				{// send coordinates
					width_ = temp_width;
					height_ = temp_height;
					terminal_size_changed(width_, height_);
				}
			}
			continue;
		}
		
		read = 0;
		INPUT_RECORD inputs[0x10];
		BOOL peek = ReadConsoleInputA(in_handle_, inputs, sizeof(inputs) / sizeof(inputs[0]), &read);
		if (!peek)
		{
			RX_ASSERT(false);
		}
		else
		{
			if (read)
			{
				for (DWORD i = 0; i < read; i++)
				{

					if (inputs[i].EventType == KEY_EVENT)
					{
						if (inputs[i].Event.KeyEvent.bKeyDown)
						{
							for (WORD j = 0; j < inputs[i].Event.KeyEvent.wRepeatCount; j++)
							{
								has = true;
								chars[count] = (char)(inputs[i].Event.KeyEvent.uChar.AsciiChar);
								count++;
							}
						}
					}
				}
			}
		}
	}
	if (is_canceling())
		return false;

	return true;
}

bool win32_console_host::write_stdout (const void* data, size_t size)
{
	static WORD convert_array[]{
			WINDOWS_TERMINAL_COLOR_RED,
			WINDOWS_TERMINAL_COLOR_GREEN,
			WINDOWS_TERMINAL_COLOR_YELLOW,
			WINDOWS_TERMINAL_COLOR_BLUE,
			WINDOWS_TERMINAL_COLOR_MAGENTA,
			WINDOWS_TERMINAL_COLOR_CYAN
		};
	DWORD written = 0;
	if (!supports_ansi())
	{
		char* out_ptr = (char*)data;
		char helper_buffer[0x10];
		size_t current = 0;
		size_t start = 0;
		int status = 0;
		while (current < size)
		{
			if (out_ptr[current] == '\x1b')
			{// this is a control sequence
				// flush data that is parsed before:
				if (current > start)
				{// write items to data
					written = 0;
					if (!WriteFile(out_handle_, &out_ptr[start], (DWORD)(current - start), &written, NULL))
					{
						return false;
					}
				}
				current++;
				if (current < size)
				{
					// read second char
					char second_char = out_ptr[current];
					// now read until the end of controled sequence
					current++;
					if (second_char == '[')
					{
						CONSOLE_SCREEN_BUFFER_INFO console_info;
						if (GetConsoleScreenBufferInfo(out_handle_, &console_info))
						{
							size_t start_color = current;
							while (current < size && out_ptr[current] <= 'A')
								current++;

							if (current < size)
							{
								size_t diff = current - start_color;
								if (diff < sizeof(helper_buffer) - 1)
								{
									memcpy(helper_buffer, &out_ptr[start_color], diff);
									helper_buffer[diff] = '\0';
									int result = atoi(helper_buffer);
									if (out_ptr[current] == 'm' && current > start_color)
									{
										WORD attribute = console_info.wAttributes;
										if (result == 0)
										{// set default
											SetConsoleTextAttribute(out_handle_, default_attribute_);
										}
										else if (result == 1)
										{// set bold

											SetConsoleTextAttribute(out_handle_, attribute | FOREGROUND_INTENSITY);
										}
										else if (result >= 31 && result <= 36)
										{
											SetConsoleTextAttribute(out_handle_, WINDOWS_TERMINAL_SET_COLOR_ATTRIBUTE(attribute, convert_array[result - 31]));
										}
										else if (result == 90)
										{
											SetConsoleTextAttribute(out_handle_, WINDOWS_TERMINAL_SET_COLOR_ATTRIBUTE(attribute, WINDOWS_TERMINAL_COLOR_DARKGRAY));
										}
									}
									else if (out_ptr[current] == 'D')
									{
										move_cur_left(out_handle_, result, console_info);
									}
									else if (out_ptr[current] == 'C')
									{
										move_cur_right(out_handle_, result, console_info);
									}
								}
							}
						}
					}
					else
					{
						while (current < size && out_ptr[current] <= 'A')
							current++;
					}
					start = current + 1;
				}
			}
			else
				current++;
		}
		// flush data that is parsed before:
		if (current > start)
		{// write items to data
			written = 0;
			if (!WriteFile(out_handle_, &out_ptr[start], (DWORD)(current - start), &written, NULL))
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return FALSE != WriteFile(out_handle_, data, (DWORD)size, &written, NULL);
	}
}

rx_result win32_console_host::setup_console (int argc, char* argv[])
{
	BOOL ret = SetConsoleCtrlHandler(NULL, FALSE);
	ret = SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrl_handler, TRUE);

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
	out_bits.set(0);
	//out_bits.reset(3);

	SetConsoleMode(in_handle_, in_bits.to_ulong());
	SetConsoleMode(out_handle_, out_bits.to_ulong());

	if (GetConsoleMode(out_handle_, &out_mode) && (ENABLE_VIRTUAL_TERMINAL_PROCESSING & out_mode))
		supports_ansi_ = true;

	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(out_handle_, &info))
	{
		default_attribute_ = info.wAttributes;
	}

	return true;
}

string_type win32_console_host::get_default_name () const
{
	string_type ret;
	get_win_host_name(ret);
	return ret;
}

string_type win32_console_host::get_win32_interactive_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_WIN32_CON_HOST_NAME, RX_WIN32_CON_HOST_MAJOR_VERSION, RX_WIN32_CON_HOST_MINOR_VERSION, RX_WIN32_CON_HOST_BUILD_NUMBER);
	}
	return ret;
}

rx_result win32_console_host::fill_host_directories (rx_host_directories& data)
{
	return build_directories(data);
}

bool win32_console_host::supports_ansi () const
{
	if (use_ansi_)
		return true;
	else if (no_ansi_)
		return false;
	else
		return supports_ansi_;
}

string_type win32_console_host::get_full_path (const string_type& path)
{
	return get_full_path_from_relative(path);
}

void win32_console_host::add_command_line_options (hosting::command_line_options_t& options, rx_platform::configuration_data_t& config)
{
	interactive_console_host::add_command_line_options(options, config);
	options.add_options()
		("use-ansi", "Force use of the ANSI escape sequences even when these are not supported.", cxxopts::value<bool>(use_ansi_))
		("no-ansi", "Force not to use the ANSI escape sequences even when these are supported.", cxxopts::value<bool>(no_ansi_));
}


} // namespace win32

