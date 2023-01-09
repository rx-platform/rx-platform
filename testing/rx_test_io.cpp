

/****************************************************************************
*
*  testing\rx_test_io.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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

#include "system/server/rx_server.h"
#ifndef EXCLUDE_TEST_CODE

// rx_test_io
#include "testing/rx_test_io.h"

#include "terminal/rx_terminal_style.h"
#include "system/server/rx_server.h"
#include "sys_internal/rx_inf.h"


namespace testing {

namespace io_test {

namespace ip_test {
// Class testing::io_test::ip_test::tcp_test 

tcp_test::tcp_test()
	: test_category("tcpip")
{
	register_test_case(rx_create_reference<test_client_basics>());
}


tcp_test::~tcp_test()
{
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
	printf("stiglo %d bajtova\r\n", (int)count);
	return true;
}

void tcp_test_client::release_buffer (buffer_ptr what)
{
}

bool tcp_test_client::connect_complete (sockaddr_in* addr, sockaddr_in* local_addr)
{
	rx_internal::interfaces::ip_endpoints::tcp_client_socket_std_buffer::connect_complete(addr, local_addr);
	printf("Callback arrived\r\n");
	return true;
}


void test_tcp_client()
{
	tcp_test_client::smart_ptr client_socket(pointers::_create_new);
	if (client_socket->bind_socket_tcpip_4())
	{
		if (client_socket->connect_to_tcpip_4("192.168.56.101", 12345, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool()))
		{
			rx_ms_sleep(200);
			client_socket->close();
		}
	}

}
// Class testing::io_test::ip_test::test_client_basics 

test_client_basics::test_client_basics()
	: test_case("client-basic")
{
}


test_client_basics::~test_client_basics()
{
}



bool test_client_basics::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	out << "Waiting for ports implementation\r\n";
	ctx->set_failed();
	return true;
	string_type addr="127.0.0.1";
	uint16_t port=12345;
	if (!in.eof())
	{
		in >> addr;
		if (!in.eof())
		{
			in >> port;
		}
	}
	if (!addr.empty() && port != 0)
	{
		out << "Testing TCP/IP Client Basic\r\n==========================\r\n";

		for (int i = 0; i < 10; i++)
		{
			out << "Connecting to " << addr << ":" << port << "...\r\n";
			out << "Creating TCP client...\r\n";
			tcp_test_client::smart_ptr client(pointers::_create_new);
			out << "Binding TCP client...\r\n";
			if (client->bind_socket_tcpip_4())
			{
				out << "Sending Connect...\r\n";
				if (client->connect_to_tcpip_4(addr, port, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool()))
				{
					rx_ms_sleep(200);
					out << "Closing Connection...\r\n";
				}
			}
			client->close();
		}

	}
	else
	{
		out << "Test usage:\r\n" ANSI_COLOR_YELLOW " test tcpip " << get_name() << " <tcp/ip address> <port>" ANSI_COLOR_RESET "\r\n";
	}
	ctx->set_passed();
	return true;
}


} // namespace ip_test
} // namespace io_test
} // namespace testing

#endif //EXCLUDE_TEST_CODE
