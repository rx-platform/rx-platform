

/****************************************************************************
*
*  host\rx_pipe.cpp
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


#include "pch.h"

#include "rx_pipe_version.h"

// rx_pipe
#include "host/rx_pipe.h"



namespace host {

namespace pipe {

// Class host::pipe::rx_pipe_host 

rx_pipe_host::rx_pipe_host (rx_platform::hosting::rx_platform_storage::smart_ptr storage)
      : exit_(false)
	, hosting::rx_platform_host(storage)
{
}


rx_pipe_host::~rx_pipe_host()
{
}



void rx_pipe_host::get_host_info (string_array& hosts)
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_HOST_NAME, RX_HOST_MAJOR_VERSION, RX_HOST_MINOR_VERSION, RX_HOST_BUILD_NUMBER);
	}
	hosts.emplace_back(ret);
	rx_platform_host::get_host_info(hosts);
}

void rx_pipe_host::server_started_event ()
{
}

bool rx_pipe_host::shutdown (const string_type& msg)
{
	return true;
}

bool rx_pipe_host::exit () const
{
	return exit_;
}

void rx_pipe_host::get_host_objects (std::vector<rx_platform::runtime::object_runtime_ptr>& items)
{
}

void rx_pipe_host::get_host_classes (std::vector<rx_platform::meta::object_class_ptr>& items)
{
}

bool rx_pipe_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{
	return true;
}

bool rx_pipe_host::write_stdout (const string_type& lines)
{
	return true;
}

bool rx_pipe_host::start (const string_array& args)
{
	return true;
}

bool rx_pipe_host::break_host (const string_type& msg)
{
	return true;
}

bool rx_pipe_host::read_stdin (std::array<char,0x100>& chars, size_t& count)
{
	return true;
}

bool rx_pipe_host::write_stdout (const void* data, size_t size)
{
	return true;
}


} // namespace pipe
} // namespace host

