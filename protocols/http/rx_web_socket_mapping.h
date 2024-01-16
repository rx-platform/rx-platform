

/****************************************************************************
*
*  protocols\http\rx_web_socket_mapping.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_web_socket_mapping_h
#define rx_web_socket_mapping_h 1



// dummy
#include "dummy.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"



namespace protocols {

namespace rx_http {
class rx_web_socket_port;





class rx_web_socket_endpoint 
{

  public:
      rx_web_socket_endpoint (rx_web_socket_port* port);

      ~rx_web_socket_endpoint();


      void close_endpoint ();


      rx_web_socket_port* get_port ()
      {
        return port_;
      }



      rx_protocol_stack_endpoint stack_entry;


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      static rx_protocol_result_t transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address);



      rx_web_socket_port* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::connection_transport_port_impl< rx_web_socket_endpoint  > rx_web_socket_port_base;






class rx_web_socket_port : public rx_web_socket_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Web Socket port. Implementation of Web Socket transport.");

    DECLARE_REFERENCE_PTR(rx_web_socket_port);

  public:
      rx_web_socket_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


      rx_platform::runtime::io_types::simple_port_status status;


  protected:

  private:


};


} // namespace rx_http
} // namespace protocols



#endif
