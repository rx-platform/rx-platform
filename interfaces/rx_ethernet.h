

/****************************************************************************
*
*  interfaces\rx_ethernet.h
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


#ifndef rx_ethernet_h
#define rx_ethernet_h 1



// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"
// dummy
#include "dummy.h"
// rx_thread
#include "system/threads/rx_thread.h"

namespace rx_internal {
namespace interfaces {
namespace ethernet {
class ethernet_port;
class ethernet_endpoint;

} // namespace ethernet
} // namespace interfaces
} // namespace rx_internal




namespace rx_internal {

namespace interfaces {

namespace ethernet {

extern std::map<string_type, byte_string> local_mac;
void rebuild_addresses();




class ethernet_endpoint : public rx_platform::threads::thread  
{

  public:
      ethernet_endpoint();

      ~ethernet_endpoint();


      rx_protocol_stack_endpoint* get_stack_endpoint ();

      runtime::items::port_runtime* get_port ();

      rx_protocol_result_t send_packet (send_protocol_packet packet);

      void release_buffer (buffer_ptr what);

      rx_result open (const string_type& port_name, security::security_context_ptr identity, ethernet_port* port, const std::vector<uint16_t>& ether_types);

      rx_result close ();

      bool is_connected () const;


  protected:

      uint32_t handler ();


  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      void process_packet (const uint8_t* buffer, size_t size);



      rx_protocol_stack_endpoint stack_endpoint_;

      ethernet_port *my_port_;


      security::security_context_ptr identity_;

      string_type port_name_;

      peth_socket handle_;

      bool stop_;

      std::vector<uint16_t> ether_types_;

      byte_string local_mac_;


};







typedef rx_platform::runtime::io_types::ports_templates::extern_singleton_port_impl< ethernet_endpoint  > ethernet_port_base;






class ethernet_port : public ethernet_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Ethernet port class. implementation of an ethernet level port");

    typedef std::stack< buffer_ptr, std::vector<buffer_ptr> > free_buffers_type;
    DECLARE_REFERENCE_PTR(ethernet_port);

  public:
      ethernet_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      virtual rx_result_with<port_connect_result> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      rx_result stop_passive ();

      void release_buffer (buffer_ptr what);

      buffer_ptr get_buffer ();


      rx_platform::runtime::io_types::external_port_status status;


  protected:

  private:


      std::unique_ptr<ethernet_endpoint> endpoint_;


      free_buffers_type free_buffers_;

      locks::slim_lock free_buffers_lock_;

      string_type port_name_;

      std::vector<uint16_t> ether_types_;


};


} // namespace ethernet
} // namespace interfaces
} // namespace rx_internal



#endif
