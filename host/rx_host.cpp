

/****************************************************************************
*
*  host\rx_host.cpp
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


// rx_security
#include "lib/security/rx_security.h"
// rx_host
#include "host/rx_host.h"



namespace host {

// Class host::rx_server_host 

rx_server_host::rx_server_host()
      : _parent(nullptr)
{
}

rx_server_host::rx_server_host(const rx_server_host &right)
      : _parent(nullptr)
{
	RX_ASSERT(false);
}


rx_server_host::~rx_server_host()
{
}


rx_server_host & rx_server_host::operator=(const rx_server_host &right)
{
	RX_ASSERT(false);
	return *this;
}



void rx_server_host::get_host_info (string_array& hosts)
{
}

void rx_server_host::server_started_event ()
{
}

void rx_server_host::server_stopping_event ()
{
}

bool rx_server_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{

	std::ostream out(out_buffer.unsafe_ptr());
	out << "dummy inmplmentation by base interface class\r\n";

	return true;
}

sys_handle_t rx_server_host::get_host_test_file (const string_type& path)
{
	return 0;
}

sys_handle_t rx_server_host::get_host_console_script_file (const string_type& path)
{
	return 0;
}


// Class host::host_security_context 

host_security_context::host_security_context()
{
	_user_name = "host";
	_full_name = _user_name + "@";
	_full_name += _location;
	_port = "internal";
}


host_security_context::~host_security_context()
{
}



bool host_security_context::is_system () const
{
  return true;

}


} // namespace host

