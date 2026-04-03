

/****************************************************************************
*
*  dotnet\dotnet_linux\dotnet_linux.cpp
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
#include <spawn.h>

#include <sys/wait.h>



namespace rx_internal {
namespace dotnet {



int rx_full_path_from_process(const char* rest, char* buff)
{

	return RX_ERROR;
}


void fill_dotnet_wp_path(string_type& path)
{
#ifdef _DEBUG
	path = "/media/sf_RX/GCC/project/bin/Debug/rx-dotnet-wp";
#else
	path = "/usr/lib/rx-platform/dotnet/rx-dotnet-wp";
#endif
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

    ss << " --dotnet-app " << config.app_path
        << " --dotnet-host " << config.host_path;
    if (!config.lib_path.empty())
        ss << " --dotnet-lib " << config.lib_path;

    string_type in_args = ss.str();

    char*  args[0x100];
    string_type path;
    fill_dotnet_wp_path(path);
	std::istringstream stream(in_args);
	std::vector<string_type> args_vec;
	args_vec.push_back(path);
	while(!stream.eof())
	{
        string_type temp;
        stream>>temp;
        if(!temp.empty())
            args_vec.push_back(std::move(temp));
	}
	for(size_t i = 0 ; i<args_vec.size(); i++)
	{
        args[i]=args_vec[i].data();
	}
	args[args_vec.size()] = nullptr;
	sys_handle_t ret = 0;
	pid_t pid;
    posix_spawn_file_actions_t actions;


    posix_spawn_file_actions_init(&actions);

    int dupret = posix_spawn_file_actions_addclose(&actions, pipes->server_read);
    dupret = posix_spawn_file_actions_addclose(&actions, pipes->server_write);

	int sret = posix_spawn(&pid, path.c_str(), &actions, NULL, args, NULL);
    if(sret == 0)
    {
        ret = (sys_handle_t)pid;
    }
    return ret;

}
void rx_terminate_process(sys_handle_t process)
{
    int status = 0;
    pid_t pid = (pid_t)process;
    if(kill(pid, SIGKILL) != -1)
    {
        if(waitpid(pid, &status, 0)!=-1)
        {
            if (WIFEXITED(status))
            {
                printf("Child (PID: %d) exited with status: %d\n", pid, WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                printf("Child (PID: %d) killed by signal: %d\n", pid, WTERMSIG(status));
            }
            else if (WIFSTOPPED(status))
            {
                printf("Child (PID: %d) stopped by signal: %d\n", pid, WSTOPSIG(status));
            }
        }
    }
}

}

}


