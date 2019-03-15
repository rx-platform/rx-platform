

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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
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

rx_result rx_platform_host::read_config_file (configuration_reader& reader, rx_platform::configuration_data_t& config)
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
			std::map<string_type, string_type> config_values;
			ret = reader.parse_configuration(settings_buff, config_values);
			if (ret)
			{
				for (auto& row : config_values)
				{
					if (config.namespace_data.system_storage_reference.empty() && row.first == "storage.system")
						config.namespace_data.system_storage_reference = row.second;
					else if (config.namespace_data.user_storage_reference.empty() && row.first == "storage.user")
						config.namespace_data.user_storage_reference = row.second;
					else if (config.namespace_data.test_storage_reference.empty() && row.first == "storage.test")
						config.namespace_data.test_storage_reference = row.second;
				}
			}
		}
		return ret;
	}
	return false;
}

rx_result rx_platform_host::initialize_storages (rx_platform::configuration_data_t& config)
{
	rx_result ret;
	if (config.namespace_data.system_storage_reference.empty())
		ret = "No valid system storage reference!";
	else
		ret = system_storage_->init_storage(config.namespace_data.system_storage_reference);
	if (ret)
	{
		if (config.namespace_data.user_storage_reference.empty())
			ret = "No valid user storage reference!";
		else
			ret = user_storage_->init_storage(config.namespace_data.user_storage_reference);
		if (ret)
		{
			if (!config.namespace_data.test_storage_reference.empty())
			{
				ret = test_storage_->init_storage(config.namespace_data.test_storage_reference);
				if (!ret)
				{
					ret.register_error("Error initializing test storage!");
				}
			}
		}
		else
		{
			ret.register_error("Error initializing user storage!");
		}
	}
	else
	{
		ret.register_error("Error initializing system storage!");
	}
	return ret;
}

rx_result rx_platform_host::deinitialize_storages ()
{
	test_storage_->deinit_storage();
	user_storage_->deinit_storage();
	system_storage_->deinit_storage();
	return true;
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

