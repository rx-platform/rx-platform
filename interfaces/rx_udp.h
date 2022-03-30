

/****************************************************************************
*
*  interfaces\rx_udp.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_udp_h
#define rx_udp_h 1


#include "interfaces/rx_endpoints.h"

// dummy
#include "dummy.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"
// rx_datagram_io
#include "interfaces/rx_datagram_io.h"

namespace rx_internal {
namespace interfaces {
namespace ip_endpoints {
class udp_endpoint;
class udp_port;

} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal


#include "sys_internal/rx_inf.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {





class udp_endpoint 
{
    enum class udp_state
    {
        not_active = 0,
        not_binded = 1,
        binded = 2,
        stopped = 3
    };

    struct socket_holder_t : public udp_socket_std_buffer
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
    typedef rx_reference<socket_holder_t> socket_ptr;

    friend struct udp_endpoint::socket_holder_t;

  public:
      udp_endpoint();

      ~udp_endpoint();


      rx_protocol_stack_endpoint* get_stack_endpoint ();

      runtime::items::port_runtime* get_port ();

      rx_protocol_result_t send_packet (send_protocol_packet packet);

      rx_result open (io::ip4_address addr, security::security_context_ptr identity, udp_port* port);

      rx_result close ();

      bool tick ();

      bool is_connected () const;

      void release_buffer (buffer_ptr what);


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      void disconnected (rx_security_handle_t identity);

      bool readed (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity);

      void start_timer (bool fire_now);

      void suspend_timer ();



      rx_protocol_stack_endpoint stack_endpoint_;

      udp_port *my_port_;

      rx_reference<socket_holder_t> udp_socket_;


      udp_state current_state_;

      rx_timer_ptr timer_;

      locks::slim_lock state_lock_;

      io::ip4_address bind_address_;

      security::security_context_ptr identity_;


};






typedef rx_platform::runtime::io_types::ports_templates::extern_singleton_port_impl< udp_endpoint  > udp_server_base;






class udp_port : public udp_server_base  
{
    DECLARE_CODE_INFO("rx", 0, 5, 0, "\
UDP port class. implementation of an UDP/IP4 port");

    typedef std::stack< buffer_ptr, std::vector<buffer_ptr> > free_buffers_type;
    DECLARE_REFERENCE_PTR(udp_port);

  public:
      udp_port();

      ~udp_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      uint32_t get_reconnect_timeout () const;

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      virtual rx_result_with<port_connect_result> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      rx_result stop_passive ();

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);

      void release_buffer (buffer_ptr what);

      buffer_ptr get_buffer ();


      rx_platform::runtime::io_types::external_port_status status;


  protected:

  private:


      std::unique_ptr<udp_endpoint> endpoint_;


      io::ip4_address bind_address_;

      runtime::local_value<uint32_t> recv_timeout_;

      runtime::local_value<uint32_t> send_timeout_;

      runtime::local_value<uint32_t> reconnect_timeout_;

      free_buffers_type free_buffers_;

      locks::slim_lock free_buffers_lock_;


};


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
