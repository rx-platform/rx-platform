

/****************************************************************************
*
*  interfaces\rx_ip_endpoints.h
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


#ifndef rx_ip_endpoints_h
#define rx_ip_endpoints_h 1



// dummy
#include "dummy.h"
// rx_io
#include "lib/rx_io.h"
// rx_endpoints
#include "interfaces/rx_endpoints.h"



namespace interfaces {

namespace ip_endpoints {





class rx_udp_endpoint : public rx_protocol_stack_entry  
{

  public:
      rx_udp_endpoint();


      rx_result open (const string_type& addr, uint16_t port);

      rx_result close ();


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer);



};






class udp_port : public io_endpoints::physical_port  
{
	DECLARE_CODE_INFO("rx", 0, 0, 1, "\
UDP port class. implementation of an TCP/IP4 UDP port");

	DECLARE_REFERENCE_PTR(udp_port);

	struct socket_holder_t : public rx::io::udp_socket_std_buffer
	{
	private:
		udp_port* whose;
	protected:
		void release_buffer(buffer_ptr what)
		{
		}
		bool readed(const void* data, size_t count, rx_thread_handle_t destination)
		{
			if (whose)
			{
				whose->update_received_counters(count);
				rx_const_packet_buffer buff{};
				rx_init_const_packet_buffer(&buff, data, count);
				auto result = rx_move_packet_up(&whose->udp_endpoint_, nullptr, &buff);
			}
			return true;
		}
	public:
		socket_holder_t(udp_port* whose)
		{
			this->whose = whose;
		}
		void disconnect()
		{
			whose = nullptr;
		}
	};
	friend struct socket_holder_t;

  public:
      udp_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


  protected:

  private:


      rx_udp_endpoint udp_endpoint_;

      rx_reference<socket_holder_t> udp_socket_;


};


} // namespace ip_endpoints
} // namespace interfaces



#endif
