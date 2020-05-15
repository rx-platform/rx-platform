

/****************************************************************************
*
*  interfaces\rx_ip_endpoints.h
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


#ifndef rx_ip_endpoints_h
#define rx_ip_endpoints_h 1


#include "interfaces/rx_endpoints.h"

// dummy
#include "dummy.h"
// rx_port_types
#include "system/runtime/rx_port_types.h"
// rx_io
#include "lib/rx_io.h"

namespace rx_internal {
namespace interfaces {
namespace ip_endpoints {
class connection_endpoint;

} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal


#include "sys_internal/rx_inf.h"


namespace rx_internal {

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

      static rx_protocol_result_t received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer);



};






class udp_port : public rx_platform::runtime::io_types::physical_port  
{
	DECLARE_CODE_INFO("rx", 0, 0, 2, "\
UDP port class. implementation of an UDP/IP4 port");

	DECLARE_REFERENCE_PTR(udp_port);

	struct socket_holder_t : public rx::io::udp_socket_std_buffer
	{
	private:
		udp_port* whose;
	protected:
		void release_buffer(buffer_ptr what)
		{
		}
		bool readed(const void* data, size_t count, rx_security_handle_t identity)
		{
            security::secured_scope _(identity);
			if (whose)
			{
				whose->update_received_counters(count);
				rx_const_packet_buffer buff{};
				rx_init_const_packet_buffer(&buff, data, count);
				auto result = rx_move_packet_up(&whose->udp_endpoint_, &buff, 0);
				if(result!=RX_PROTOCOL_OK)
                    return false;
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

      rx_protocol_stack_entry* get_stack_entry ();


  protected:

  private:


      rx_udp_endpoint udp_endpoint_;

      rx_reference<socket_holder_t> udp_socket_;


};







typedef rx_platform::runtime::io_types::physical_multiple_port_impl< rx_internal::interfaces::ip_endpoints::connection_endpoint  > tcp_server_base;






class connection_endpoint : public rx_protocol_stack_entry  
{
    struct socket_holder_t : public rx::io::tcp_socket_std_buffer
    {
        DECLARE_REFERENCE_PTR(connection_endpoint::socket_holder_t);
        friend class connection_endpoint;
    private:
        connection_endpoint* whose = nullptr;
    protected:
        void release_buffer(buffer_ptr what)
        {
        }
        bool readed(const void* data, size_t count, rx_security_handle_t identity)
        {
            security::secured_scope _(identity);
            if (whose)
                return whose->readed(data, count, identity);
            else
                return false;
        }
        void on_shutdown(rx_security_handle_t identity)
        {
            security::secured_scope _(identity);
            if(whose)
                whose->disconnected(identity);
        }
    public:
        socket_holder_t(connection_endpoint* whose, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr)
            : whose(whose)
            , rx::io::tcp_socket_std_buffer(handle, addr, local_addr, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool())
        {
        }
        socket_holder_t(socket_holder_t&& right) noexcept
        {
            whose = right.whose;
            right.whose = nullptr;
        }
        void disconnect()
        {
            whose = nullptr;
        }
    };
    friend struct socket_holder_t;
  public:
    typedef std::unique_ptr<connection_endpoint> endpoint_ptr;
    typedef rx_reference<socket_holder_t> socket_ptr;

  public:
      connection_endpoint();

      connection_endpoint (const string_type& remote_port, const string_type& local_port);


      rx_result_with<connection_endpoint::socket_ptr> open (tcp_server_base* my_port, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher, rx_security_handle_t identity);

      rx_result close ();


      const rx_reference<socket_holder_t> get_tcp_socket () const
      {
        return tcp_socket_;
      }


      connection_endpoint(const connection_endpoint&) = delete;
      connection_endpoint(connection_endpoint&& right) noexcept;
      connection_endpoint& operator=(const connection_endpoint&) = delete;
      connection_endpoint& operator=(connection_endpoint&&) = delete;
  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_entry* reference, rx_packet_buffer* buffer, rx_packet_id_type packet_id);

      void disconnected (rx_security_handle_t identity);

      bool readed (const void* data, size_t count, rx_security_handle_t identity);

      void bind ();

      static rx_protocol_result_t send_function (rx_protocol_stack_entry* reference, rx_packet_buffer* buffer, rx_packet_id_type packet_id);



      rx_reference<socket_holder_t> tcp_socket_;

      tcp_server_base *my_port_;


      string_type remote_port_;

      string_type local_port_;


};







class tcp_server_port : public tcp_server_base  
{
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
TCP Server port class. implementation of an TCP/IP4 server side, listen, accept, clients list...");

    DECLARE_REFERENCE_PTR(tcp_server_port);

    typedef std::set<connection_endpoint::endpoint_ptr> connections_type;

  public:
      tcp_server_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void update_received_counters (size_t count);

      void update_sent_counters (size_t count);


  protected:

  private:


      rx_reference<rx::io::tcp_listent_std_buffer> listen_socket_;


      runtime_handle_t rx_recv_timeout_;

      runtime_handle_t rx_send_timeout_;


};


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
