

/****************************************************************************
*
*  testing\rx_test_io.cpp
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


// rx_test_io
#include "testing/rx_test_io.h"

#include "system/server/rx_server.h"


namespace testing {

namespace io_test {

namespace ip_test {
// Class testing::io_test::ip_test::tcp_client_test 

tcp_client_test::tcp_client_test()
	: code_test("tcpip-client")
{
}


tcp_client_test::~tcp_client_test()
{
}



bool tcp_client_test::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	out << "Testing TCP/IP Client\r\n==========================\r\n";
	return true;
}


// Class testing::io_test::ip_test::tcp_test_client 

tcp_test_client::tcp_test_client()
{
}


tcp_test_client::~tcp_test_client()
{
}



bool tcp_test_client::readed (const void* data, size_t count, rx_thread_handle_t destination)
{
	return true;
}

void tcp_test_client::release_buffer (buffer_ptr what)
{
}


void test_tcp_client()
{
	tcp_test_client::smart_ptr client_socket(pointers::_create_new);
	if (client_socket->bind_socket_tcpip_4(server::rx_server::instance().get_runtime().get_io_pool()->get_pool()))
	{
		if (client_socket->connect_to_tcpip_4(server::rx_server::instance().get_runtime().get_io_pool()->get_pool(), "192.168.56.101", 12345))
		{
			rx_msleep(200);
			client_socket->close();
		}
	}

}
} // namespace ip_test
} // namespace io_test
} // namespace testing

