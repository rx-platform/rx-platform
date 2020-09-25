

/****************************************************************************
*
*  interfaces\rx_tcp_client.h
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


#ifndef rx_tcp_client_h
#define rx_tcp_client_h 1



// dummy
#include "dummy.h"
// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"
// rx_stream_io
#include "lib/rx_stream_io.h"

namespace rx_internal {
namespace interfaces {
namespace ip_endpoints {
class tcp_client_port;

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
    
    struct socket_holder_t : public rx::io::tcp_client_socket_std_buffer
    {
        DECLARE_REFERENCE_PTR(tcp_client_endpoint::socket_holder_t);
        friend class tcp_client_endpoint;
    private:
        tcp_client_endpoint* whose = nullptr;
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
            if (whose)
                whose->disconnected(identity);
        }
        bool connect_complete(sockaddr_in* addr, sockaddr_in* local_addr)
        {
            whose->connected(addr, local_addr);
            return rx::io::tcp_client_socket_std_buffer::connect_complete(addr, local_addr);
        }
    public:
        socket_holder_t(tcp_client_endpoint* whose)
            : whose(whose)
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
            close();
        }
    };
    friend struct tcp_client_endpoint::socket_holder_t;
public:
    typedef std::unique_ptr<tcp_client_endpoint> endpoint_ptr;
    typedef rx_reference<socket_holder_t> socket_ptr;

  public:
      tcp_client_endpoint();

      tcp_client_endpoint (const string_type& remote_port, const string_type& local_port);


      rx_result stop ();

      bool tick ();

      rx_result start (tcp_client_port* my_port, const string_type& addr, uint16_t port, const string_type& remote_addr, uint16_t remote_port);

      rx_protocol_stack_endpoint* get_stack_endpoint ();


  protected:

  private:

      void disconnected (rx_security_handle_t identity);

      bool readed (const void* data, size_t count, rx_security_handle_t identity);

      void bind ();

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


};







typedef rx_platform::runtime::io_types::ports_templates::extern_port_impl< rx_internal::interfaces::ip_endpoints::tcp_client_endpoint  > tcp_client_base;






class tcp_client_port : public tcp_client_base  
{
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
TCP Server port class. implementation of an TCP/IP4 client side, connect...");

    DECLARE_REFERENCE_PTR(tcp_client_port);

  public:
      tcp_client_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      uint32_t get_reconnect_timeout () const;

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);


  protected:

  private:

      rx_protocol_stack_endpoint* get_stack_entry ();



      tcp_client_endpoint session_;


      runtime_handle_t rx_recv_timeout_;

      runtime_handle_t rx_connect_timeout_;

      runtime_handle_t rx_reconnect_timeout_;


};


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
