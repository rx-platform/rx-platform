

/****************************************************************************
*
*  win32_hosts\rx_win32_interactive.cpp
*
*  Copyright (c) 2018-2019 Dusan Ciric
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


#include "pch.h"

#include "rx_win32_console_version.h"

// rx_win32_interactive
#include "win32_hosts/rx_win32_interactive.h"

namespace
{
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
			return TRUE;
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
					padapters[i].name = names[i];
					padapters[i].ip_address = addresses[i];
					padapters[i].index = ctxs[i];
					padapters[i].status = status_disconnected;
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

win32_console_host::win32_console_host (rx_platform::hosting::rx_platform_storage::smart_ptr storage)
      : out_handle_(NULL),
        in_handle_(NULL)
	, host::interactive::interactive_console_host(storage)
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

	//in_bits.reset(0);
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
	bool ret = (FALSE != ReadConsole(in_handle_, &chars[0], 0x100, &read, NULL));
	count = read;
	return ret;
}

bool win32_console_host::write_stdout (const void* data, size_t size)
{
	DWORD written = 0;
	return FALSE != WriteFile(out_handle_, data, (DWORD)size, &written, NULL);
}

std::vector<ETH_interface> win32_console_host::get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	std::vector<ETH_interface> ret;
	return ret;
}

std::vector<IP_interface> win32_console_host::get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	std::vector<IP_interface> ret;
	IP_interface* itfs;

	size_t count = list_ip_adapters(&itfs);
	for (size_t i = 0; i < count; i++)
	{

	}

	return ret;
}


} // namespace win32

