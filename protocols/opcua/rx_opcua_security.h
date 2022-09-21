

/****************************************************************************
*
*  protocols\opcua\rx_opcua_security.h
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


#ifndef rx_opcua_security_h
#define rx_opcua_security_h 1


#include "protocols/ansi_c/opcua_c/rx_opcua_binary_sec.h"

// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"
// dummy
#include "dummy.h"

namespace protocols {
namespace opcua {
namespace opcua_transport {
class opcua_sec_none_client_port;
class opcua_sec_none_port;

} // namespace opcua_transport
} // namespace opcua
} // namespace protocols




namespace protocols {

namespace opcua {

namespace opcua_transport {






class opcua_sec_none_endpoint : public opcua_sec_none_protocol_type  
{

  public:
      opcua_sec_none_endpoint (opcua_sec_none_port* port);

      ~opcua_sec_none_endpoint();


      opcua_sec_none_port* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      static rx_protocol_result_t transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address);



      opcua_sec_none_port* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::connection_transport_port_impl< opcua_sec_none_endpoint  > opcua_sec_none_base;






class opcua_sec_none_port : public opcua_sec_none_base  
{
    DECLARE_CODE_INFO("rx", 0, 9, 0, "\
OPC-UA Security None port. Implementation of binary OPC-UA Security None server transport channel.");

    DECLARE_REFERENCE_PTR(opcua_sec_none_port);

  public:
      opcua_sec_none_port();


      static std::map<rx_node_id, opcua_sec_none_port::smart_ptr> runtime_instances;


  protected:

  private:


};







class opcua_sec_none_client_endpoint : public opcua_sec_none_protocol_type  
{

  public:
      opcua_sec_none_client_endpoint (opcua_sec_none_client_port* port);

      ~opcua_sec_none_client_endpoint();


      opcua_sec_none_client_port* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      opcua_sec_none_client_port* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< protocols::opcua::opcua_transport::opcua_sec_none_client_endpoint  > opcua_sec_none_client_base;






class opcua_sec_none_client_port : public opcua_sec_none_client_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
OPC-UA Security None port. Implementation of binary OPC-UA Security None client transport channel.");

    DECLARE_REFERENCE_PTR(opcua_sec_none_client_port);

  public:
      opcua_sec_none_client_port();


      static std::map<rx_node_id, opcua_sec_none_client_port::smart_ptr> runtime_instances;


  protected:

  private:


};


} // namespace opcua_transport
} // namespace opcua
} // namespace protocols



#endif
