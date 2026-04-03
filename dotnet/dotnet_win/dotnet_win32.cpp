

/****************************************************************************
*
*  dotnet\dotnet_win\dotnet_win32.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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
#include "dotnet_wp/dotnet_wp_protocol.h"



namespace rx_internal {
namespace dotnet {



int rx_full_path_from_process(const char* rest, char* buff)
{
	buff[0] = '\0';

	if (GetModuleFileNameA(NULL, buff, 1024))
	{
		int found = 0;
		size_t j = strlen(buff);
		for (size_t i = j - 1; i > 0; i--)
		{
			if (buff[i] == L'\\' || buff[i] == L'/')
			{
				found++;
#ifndef _DEBUG
				if (found == 2)
#endif
				{
					buff[i + 1] = L'\0';
					break;
				}
			}
		}

		strcat_s(buff, 1024, rest);

		return RX_OK;
	}
	return RX_ERROR;
}

sys_handle_t create_dotnet_process(const rx_dotnet_config_data_t& config, pipe_server* pipes)
{
	std::ostringstream ss;
	ss << "-i " << (uint64_t)pipes->client_read << " -o " << (uint64_t)pipes->client_write
		<< " --dotnet-version " << config.requested_version
		<< " --dotnet-threads-min " << config.min_threads << " --dotnet-threads-max " << config.max_threads;
	if (!config.server_gc)
		ss << " --dotnet-no-server-gc";
	if (!config.concurrent_gc)
		ss << " --dotnet-non-concurrent-gc";

	if (config.launch_debugger)
		ss << " --dotnet-debug-break";

	ss << " --dotnet-app \"" << config.app_path << "\""
		<< " --dotnet-host \"" << config.host_path << "\"";
	if (!config.lib_path.empty())
		ss << " --dotnet-lib \"" << config.lib_path << "\"";

	string_type args = ss.str();

	char full_path[1024];
#ifdef _DEBUG
	if (rx_full_path_from_process("rx-dotnet-wp.exe", full_path) != RX_OK)
#else
	if (rx_full_path_from_process("bin\\rx-dotnet-wp.exe", full_path) != RX_OK)
#endif
		return RX_ERROR;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	std::ostringstream ess;
	ess << "\"" << full_path << "\" " << args;
	std::string cmdline = ess.str();
	if (!CreateProcessA(NULL, cmdline.data(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		return RX_ERROR;
	}
	CloseHandle(pi.hThread);
	return pi.hProcess;
}
void rx_terminate_process(sys_handle_t process)
{
	TerminateProcess(process, 0);
	CloseHandle(process);
}




} // namespace dotnet
} // namespace rx_internal

