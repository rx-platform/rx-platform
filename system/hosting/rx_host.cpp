

/****************************************************************************
*
*  system\hosting\rx_host.cpp
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


#include "stdafx.h"


// rx_security
#include "lib/security/rx_security.h"
// rx_host
#include "system/hosting/rx_host.h"



namespace rx_platform {

namespace hosting {

// Class rx_platform::hosting::rx_platform_host 

rx_platform_host::rx_platform_host(const rx_platform_host &right)
      : parent_(nullptr)
{
	RX_ASSERT(false);
}

rx_platform_host::rx_platform_host (rx_platform_storage::smart_ptr storage)
      : parent_(nullptr)
	, storage_(storage)
{
	storage_->init_storage();
}


rx_platform_host::~rx_platform_host()
{
	storage_->deinit_storage();
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
	out << "dummy inmplmentation by base interface class\r\n";

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



sys_handle_t rx_platform_storage::get_host_test_file (const string_type& path)
{
	return 0;
}

sys_handle_t rx_platform_storage::get_host_console_script_file (const string_type& path)
{
	return 0;
}

void rx_platform_storage::init_storage ()
{
}

void rx_platform_storage::deinit_storage ()
{
}


} // namespace hosting
} // namespace rx_platform

