

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
// Class testing::io_test::ip_test::tcp_test 

tcp_test::tcp_test()
	: test_category("tcpip")
{
	register_test_case(test_client_basics::smart_ptr(pointers::_create_new));
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

bool tcp_test_client::connect_complete ()
{
	rx::io::tcp_client_socket_std_buffer::connect_complete();
	printf("Stigo callback\r\n");
	return true;
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
// Class testing::io_test::ip_test::test_client_basics 

test_client_basics::test_client_basics()
	: test_case("client-basic")
{
}


test_client_basics::~test_client_basics()
{
}



bool test_client_basics::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type addr;
	uint16_t port=0;
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
			if (client->bind_socket_tcpip_4(rx_server::instance().get_runtime().get_io_pool()->get_pool()))
			{
				out << "Sending Connect...\r\n";
				if (client->connect_to_tcpip_4(rx_server::instance().get_runtime().get_io_pool()->get_pool(), addr, port))
				{
					rx_msleep(200);
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
	return true;
}


} // namespace ip_test
} // namespace io_test
} // namespace testing

