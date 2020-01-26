

/****************************************************************************
*
*  system\runtime\rx_objbase.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_objbase
#include "system/runtime/rx_objbase.h"

#include "rx_configuration.h"


namespace rx_platform {
void rx_split_item_path(const string_type& full_path, string_type& object_path, string_type& item_path)
{
	auto idx = full_path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// no directory stuff
		idx = full_path.find(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{// just plain object name
			object_path = full_path;
		}
		else
		{
			object_path = full_path.substr(0, idx);
			item_path = full_path.substr(idx + 1);
		}
	}
	else
	{// we have directory stuff
		idx = full_path.find(RX_OBJECT_DELIMETER, idx + 1);
		if (idx == string_type::npos)
		{// just object path
			object_path = full_path;
		}
		else
		{
			object_path = full_path.substr(0, idx);
			item_path = full_path.substr(idx + 1);
		}
	}
}

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::object_runtime 

rx_item_type object_runtime::type_id = rx_item_type::rx_object;

object_runtime::object_runtime()
{
}


object_runtime::~object_runtime()
{
}



rx_result object_runtime::initialize_runtime (runtime_init_context& ctx)
{
	return true;
}

rx_result object_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result object_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result object_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::items::application_runtime 

rx_item_type application_runtime::type_id = rx_item_type::rx_application;

application_runtime::application_runtime()
{
}


application_runtime::~application_runtime()
{
}



rx_result application_runtime::initialize_runtime (runtime_init_context& ctx)
{
	return true;
}

rx_result application_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result application_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result application_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::items::domain_runtime 

rx_item_type domain_runtime::type_id = rx_item_type::rx_domain;

domain_runtime::domain_runtime()
{
}


domain_runtime::~domain_runtime()
{
}



rx_result domain_runtime::initialize_runtime (runtime_init_context& ctx)
{
	return true;
}

rx_result domain_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result domain_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result domain_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}

// has to be before call

// Class rx_platform::runtime::items::port_runtime 

rx_item_type port_runtime::type_id = rx_item_type::rx_port;

port_runtime::port_runtime()
      : context_(nullptr),
        executer_(0)
{
}


port_runtime::~port_runtime()
{
}



bool port_runtime::write (buffer_ptr what)
{
	return false;
}

bool port_runtime::readed (buffer_ptr what, rx_thread_handle_t destination)
{
	return true;
}

rx_protocol_stack_entry* port_runtime::get_stack_entry ()
{
	RX_ASSERT(false);
	return nullptr;
}

rx_result_with<io_types::rx_io_buffer> port_runtime::allocate_io_buffer (size_t initial_capacity)
{
	io_types::rx_io_buffer ret;
	auto result = rx_init_packet_buffer(&ret, initial_capacity, get_stack_entry());
	if (result == RX_PROTOCOL_OK)
		return ret;
	else
		return rx_protocol_error_message(result);
}

rx_result port_runtime::initialize_runtime (runtime_init_context& ctx)
{
	return true;
}

rx_result port_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result port_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result port_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}


} // namespace items
} // namespace runtime
} // namespace rx_platform

