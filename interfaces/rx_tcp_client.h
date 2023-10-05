

/****************************************************************************
*
*  interfaces\rx_tcp_client.h
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


#ifndef rx_tcp_client_h
#define rx_tcp_client_h 1


#include "system/runtime/rx_value_templates.h"

// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"
// dummy
#include "dummy.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_stream_io
#include "interfaces/rx_stream_io.h"

namespace rx_internal {
namespace interfaces {
namespace ip_endpoints {
class tcp_client_port;
class tcp_client_endpoint;

} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal


#include "sys_internal/rx_inf.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {





class tcp_client_endpoint 
{

    enum class tcp_state
    {
        not_active = 0,
        not_connected = 1,
        connecting = 2,
        connected = 3,
        stopped = 4
    };
    struct socket_holder_t : public tcp_client_socket_std_buffer
    {
        DECLARE_REFERENCE_PTR(tcp_client_endpoint::socket_holder_t);
        friend class tcp_client_endpoint;
    private:
        tcp_client_endpoint* whose = nullptr;
    protected:
        void release_buffer(buffer_ptr what);
        bool readed(const void* data, size_t count, rx_security_handle_t identity);
        void on_shutdown(rx_security_handle_t identity);
        bool connect_complete(sockaddr_in* addr, sockaddr_in* local_addr);
    public:
        socket_holder_t(tcp_client_endpoint* whose);
        socket_holder_t(socket_holder_t&& right) noexcept;
        void disconnect();
    };
    friend struct tcp_client_endpoint::socket_holder_t;
public:
    typedef std::unique_ptr<tcp_client_endpoint> endpoint_ptr;
    typedef rx_reference<socket_holder_t> socket_ptr;

  public:
      tcp_client_endpoint();

      ~tcp_client_endpoint();


      rx_result close ();

      bool tick ();

      rx_result open (const protocol_address* addr, const protocol_address* remote_addr, security::security_context_ptr identity, tcp_client_port* port);

      rx_protocol_stack_endpoint* get_stack_endpoint ();

      runtime::items::port_runtime* get_port ();

      bool is_connected () const;

      void release_buffer (buffer_ptr what);


  protected:

  private:

      void disconnected (rx_security_handle_t identity);

      bool readed (const void* data, size_t count, rx_security_handle_t identity);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      bool connected (sockaddr_in* addr, sockaddr_in* local_addr);

      void start_timer (bool fire_now);

      void suspend_timer ();



      rx_reference<socket_holder_t> tcp_socket_;

      tcp_client_port *my_port_;

      rx_protocol_stack_endpoint stack_endpoint_;


      tcp_state current_state_;

      rx_timer_ptr timer_;

      locks::slim_lock state_lock_;

      io::ip4_address local_addr_;

      io::ip4_address remote_addr_;

      security::security_context_ptr identity_;


};







typedef rx_platform::runtime::io_types::ports_templates::extern_singleton_port_impl< tcp_client_endpoint  > tcp_client_base;






class tcp_client_port : public tcp_client_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
TCP Server port class. implementation of an TCP/IP4 client side, connect...");

    DECLARE_REFERENCE_PTR(tcp_client_port);

    typedef std::stack< buffer_ptr, std::vector< buffer_ptr> > free_buffers_type;

  public:
      tcp_client_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      uint32_t get_reconnect_timeout () const;

      rx_result_with<port_connect_result> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      rx_result stop_passive ();

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);

      void release_buffer (buffer_ptr what);

      buffer_ptr get_buffer ();

      uint32_t get_receive_timeout () const;

      uint32_t get_send_timeout () const;

      uint32_t get_connect_timeout () const;


      rx_platform::runtime::io_types::external_port_status status;


  protected:

  private:


      std::unique_ptr<tcp_client_endpoint> endpoint_;


      runtime::local_value<uint32_t> recv_timeout_;

      runtime::local_value<uint32_t> send_timeout_;

      runtime::local_value<uint32_t> connect_timeout_;

      runtime::local_value<uint32_t> reconnect_timeout_;

      io::ip4_address bind_address_;

      io::ip4_address connect_address_;

      free_buffers_type free_buffers_;

      locks::slim_lock free_buffers_lock_;


};


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
