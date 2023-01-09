

/****************************************************************************
*
*  interfaces\rx_tcp_server.h
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


#ifndef rx_tcp_server_h
#define rx_tcp_server_h 1


#include "interfaces/rx_endpoints.h"
#include "system/runtime/rx_value_templates.h"

// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"
// dummy
#include "dummy.h"
// rx_stream_io
#include "interfaces/rx_stream_io.h"

namespace rx_internal {
namespace interfaces {
namespace ip_endpoints {
class tcp_server_port;

} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal


#include "sys_internal/rx_inf.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {





class tcp_server_endpoint 
{
    struct socket_holder_t : public tcp_socket_std_buffer
    {
        DECLARE_REFERENCE_PTR(tcp_server_endpoint::socket_holder_t);
        friend class tcp_server_endpoint;
    private:
        tcp_server_endpoint* whose = nullptr;
    protected:
        void release_buffer(buffer_ptr what);
        bool readed(const void* data, size_t count, rx_security_handle_t identity);
        void on_shutdown(rx_security_handle_t identity);
    public:
        socket_holder_t(tcp_server_endpoint* whose, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr);
        socket_holder_t(socket_holder_t&& right) noexcept;
        void disconnect();
    };
    friend struct tcp_server_endpoint::socket_holder_t;
  public:
    typedef std::unique_ptr<tcp_server_endpoint> endpoint_ptr;
    typedef rx_reference<socket_holder_t> socket_ptr;

  public:
      tcp_server_endpoint();

      ~tcp_server_endpoint();


      rx_result_with<tcp_server_endpoint::socket_ptr> open (tcp_server_port* my_port, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher, security::security_context_ptr identity);

      rx_result close ();

      rx_protocol_stack_endpoint* get_stack_endpoint ();

      void set_receive_timeout (uint32_t val);

      void set_send_timeout (uint32_t val);

      runtime::items::port_runtime* get_port ();

      void release_buffer (buffer_ptr what);


      const rx_reference<socket_holder_t> get_tcp_socket () const
      {
        return tcp_socket_;
      }


      tcp_server_endpoint(const tcp_server_endpoint&) = delete;
      tcp_server_endpoint(tcp_server_endpoint&& right) = default;
      tcp_server_endpoint& operator=(const tcp_server_endpoint&) = delete;
      tcp_server_endpoint& operator=(tcp_server_endpoint&&) = default;
  protected:

  private:

      void disconnected (rx_security_handle_t identity);

      bool readed (const void* data, size_t count, rx_security_handle_t identity);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      rx_reference<socket_holder_t> tcp_socket_;

      rx_protocol_stack_endpoint stack_endpoint_;

      tcp_server_port *my_port_;


      io::ip4_address remote_address_;

      io::ip4_address local_address_;

      security::security_context_ptr identity_;


};







typedef rx_platform::runtime::io_types::ports_templates::extern_port_impl< tcp_server_endpoint  > tcp_server_base;






class tcp_server_port : public tcp_server_base  
{
    DECLARE_CODE_INFO("rx", 1, 1, 0, "\
TCP Server port class. implementation of an TCP/IP4 server side, listen, accept, clients list...");

    DECLARE_REFERENCE_PTR(tcp_server_port);


    typedef std::stack< buffer_ptr, std::vector< buffer_ptr> > free_buffers_type;
    typedef std::map<rx_protocol_stack_endpoint*, tcp_server_endpoint::endpoint_ptr> active_endpoints_type;

  public:

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result stop_passive ();

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);

      void release_buffer (buffer_ptr what);

      buffer_ptr get_buffer ();


      rx_platform::runtime::io_types::external_port_status status;


  protected:

  private:


      rx_reference<tcp_listent_std_buffer> listen_socket_;


      io::ip4_address bind_address_;

      runtime::local_value<uint32_t> recv_timeout_;

      runtime::local_value<uint32_t> send_timeout_;

      free_buffers_type free_buffers_;

      locks::slim_lock free_buffers_lock_;


};






class system_server_port_base : public tcp_server_port  
{
    DECLARE_REFERENCE_PTR(system_server_port_base);

  public:

      virtual uint16_t get_configuration_port () const = 0;

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);


  protected:

  private:


};






class system_http_port : public system_server_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
System HTTP TCP Server port class.");

    DECLARE_REFERENCE_PTR(system_http_port);

  public:

      uint16_t get_configuration_port () const;


  protected:

  private:


};






class system_rx_port : public system_server_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
System rx-protocol TCP Server port class.");

    DECLARE_REFERENCE_PTR(system_rx_port);

  public:

      uint16_t get_configuration_port () const;


  protected:

  private:


};






class system_opcua_port : public system_server_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
System a OPC UA Binary TCP Server port class.");

    DECLARE_REFERENCE_PTR(system_opcua_port);

  public:

      uint16_t get_configuration_port () const;


  protected:

  private:


};


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
