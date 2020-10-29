

/****************************************************************************
*
*  interfaces\rx_udp.h
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


#ifndef rx_udp_h
#define rx_udp_h 1


#include "interfaces/rx_endpoints.h"

// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"
// dummy
#include "dummy.h"
// rx_datagram_io
#include "lib/rx_datagram_io.h"

namespace rx_internal {
namespace interfaces {
namespace ip_endpoints {
class udp_port;

} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal


#include "sys_internal/rx_inf.h"
#include "system/runtime/address_endpoint.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {





class udp_endpoint 
{
    struct socket_holder_t : public rx::io::udp_socket_std_buffer
    {
        DECLARE_REFERENCE_PTR(udp_endpoint::socket_holder_t);
        friend class udp_endpoint;
    private:
        udp_endpoint* whose = nullptr;
    protected:
        void release_buffer(buffer_ptr what);
        bool readed(const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity);
        void on_shutdown(rx_security_handle_t identity);
    public:
        socket_holder_t(udp_endpoint* whose);
        socket_holder_t(socket_holder_t&& right) noexcept;
        void disconnect();
    };
    friend struct udp_endpoint::socket_holder_t;
public:
    typedef std::unique_ptr<udp_endpoint> endpoint_ptr;
    typedef rx_reference<socket_holder_t> socket_ptr;

    friend struct udp_endpoint::socket_holder_t;

  public:
      udp_endpoint();

      ~udp_endpoint();


      rx_result open (io::ip4_address addr, uint32_t session_timeout, rx_security_handle_t identity, udp_port* port);

      rx_result close ();

      rx_protocol_stack_endpoint* get_stack_endpoint ();

      runtime::items::port_runtime* get_port ();


  protected:

  private:

      void disconnected (rx_security_handle_t identity);

      bool readed (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      rx_protocol_stack_endpoint stack_endpoint_;

      udp_port *my_port_;

      rx_reference<socket_holder_t> udp_socket_;


      io::ip4_address bind_address_;

      uint32_t session_timeout_;


};







typedef rx_platform::runtime::io_types::ports_templates::extern_routed_port_impl< rx_internal::interfaces::ip_endpoints::udp_endpoint , runtime::io_types::ports_templates::routing_endpoint<runtime::io_types::ports_templates::address_routing_translator<io::ip4_address>, io::ip4_address>  > udp_server_base;






class udp_port : public udp_server_base  
{
    DECLARE_CODE_INFO("rx", 0, 0, 2, "\
UDP port class. implementation of an UDP/IP4 port");

    DECLARE_REFERENCE_PTR(udp_port);

    struct session_data_t
    {
        rx_protocol_stack_endpoint* entry;
        uint32_t last_tick;
    };

    typedef std::map<io::ip4_address, session_data_t> sessions_type;

    

  public:
      udp_port();

      ~udp_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      void timer_tick (uint32_t tick);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result stop_passive ();

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


  protected:

      bool packet_arrived (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity);


  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      runtime_handle_t rx_recv_timeout_;

      runtime_handle_t rx_send_timeout_;

      io::ip4_address bind_address_;


};


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
