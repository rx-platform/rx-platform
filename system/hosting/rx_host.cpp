

/****************************************************************************
*
*  system\hosting\rx_host.cpp
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


// rx_security
#include "lib/security/rx_security.h"
// rx_host
#include "system/hosting/rx_host.h"

#include "system/server/rx_ns.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace hosting {

// Class rx_platform::hosting::rx_platform_host 

rx_platform_host::rx_platform_host(const rx_platform_host &right)
      : parent_(nullptr)
{
	RX_ASSERT(false);
}

rx_platform_host::rx_platform_host (rx_host_storages& storage)
      : parent_(nullptr)
	, system_storage_(storage.system_storage)
	, user_storage_(storage.user_storage)
	, test_storage_(storage.test_storage)
{
}


rx_platform_host::~rx_platform_host()
{
}


rx_platform_host & rx_platform_host::operator=(const rx_platform_host &right)
{
	RX_ASSERT(false);
	return *this;
}



void rx_platform_host::get_host_info (string_array& hosts)
{
}

void rx_platform_host::server_started_event ()
{
}

void rx_platform_host::server_stopping_event ()
{
}

bool rx_platform_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{

	std::ostream out(out_buffer.unsafe_ptr());
	out << "dummy implementation by base interface class\r\n";

	return true;
}

std::vector<ETH_interface> rx_platform_host::get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	std::vector<ETH_interface> ret;
	return ret;
}

std::vector<IP_interface> rx_platform_host::get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	std::vector<IP_interface> ret;
	return ret;
}

bool rx_platform_host::read_config_file (configuration_reader& reader, rx_platform::configuration_data_t& config)
{
	string_type config_path = rx_combine_paths(get_config_path(), "platform.yml");
	if (!config_path.empty())
	{
		string_type settings_buff;
		bool ret = false;
		{
			rx_source_file file;
			if (file.open(config_path.c_str()))
			{
				ret = file.read_string(settings_buff);
			}
		}
		if(ret)
		{
			ret = reader.parse_configuration(settings_buff, config);
		}
		return ret;
	}
	return false;
}


// Class rx_platform::hosting::host_security_context 

host_security_context::host_security_context()
{
	user_name_ = "host";
	full_name_ = user_name_ + "@";
	full_name_ += location_;
	port_ = "internal";
}


host_security_context::~host_security_context()
{
}



bool host_security_context::is_system () const
{
  return true;

}


// Class rx_platform::hosting::rx_platform_storage 

rx_platform_storage::rx_platform_storage()
{
}


rx_platform_storage::~rx_platform_storage()
{
}



rx_result rx_platform_storage::init_storage (const string_type& storage_reference)
{
	return false;
}

rx_result rx_platform_storage::deinit_storage ()
{
	return true;
}


// Class rx_platform::hosting::rx_storage_item 

rx_storage_item::rx_storage_item (const string_type& path)
      : path_(path)
{
}


rx_storage_item::~rx_storage_item()
{
}



// Class rx_platform::hosting::configuration_reader 


} // namespace hosting
} // namespace rx_platform

