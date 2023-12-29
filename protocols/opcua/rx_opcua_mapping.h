

/****************************************************************************
*
*  protocols\opcua\rx_opcua_mapping.h
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


#ifndef rx_opcua_mapping_h
#define rx_opcua_mapping_h 1



// dummy
#include "dummy.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"

#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"


/////////////////////////////////////////////////////////////
// logging macros for opcua library
#define OPCUA_LOG_INFO(src,lvl,msg) RX_LOG_INFO("OPCUA",src,lvl,msg)
#define OPCUA_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("OPCUA",src,lvl,msg)
#define OPCUA_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("OPCUA",src,lvl,msg)
#define OPCUA_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("OPCUA",src,lvl,msg)
#define OPCUA_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("OPCUA",src,lvl,msg)
#define OPCUA_LOG_TRACE(src,lvl,msg) RX_TRACE("OPCUA",src,lvl,msg)


namespace protocols {

namespace opcua {
void opcua_split_url(const string_type& url, string_type& addr, string_type& path);

namespace opcua_transport {

constexpr size_t opc_ua_endpoint_name_len = 0x100;





class opcua_transport_endpoint : public opcua_transport_protocol_type  
{

  public:
      opcua_transport_endpoint (runtime::items::port_runtime* port);

      ~opcua_transport_endpoint();


      rx_protocol_stack_endpoint* bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      std::function<void(int64_t)> sent_func_;

      std::function<void(int64_t)> received_func_;

      runtime::items::port_runtime* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< opcua_transport_endpoint  > opcua_transport_base;






class opcua_transport_port : public opcua_transport_base  
{
    DECLARE_CODE_INFO("rx", 0, 9, 0, "\
OPC-UA transport port. Implementation of binary OPC-UA transport.");

    DECLARE_REFERENCE_PTR(opcua_transport_port);

  public:
      opcua_transport_port();


      static std::map<rx_node_id, opcua_transport_port::smart_ptr> runtime_instances;


  protected:

  private:


};






class opcua_client_transport_endpoint : public opcua_transport_protocol_type  
{

  public:
      opcua_client_transport_endpoint (runtime::items::port_runtime* port);

      ~opcua_client_transport_endpoint();


      rx_protocol_stack_endpoint* bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      static rx_protocol_result_t disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason);



      std::function<void(int64_t)> sent_func_;

      std::function<void(int64_t)> received_func_;

      runtime::items::port_runtime* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< opcua_client_transport_endpoint  > opcua_client_transport_base;






class opcua_client_transport_port : public opcua_client_transport_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
OPC-UA client transport port. Implementation of binary OPC-UA client transport.");

    DECLARE_REFERENCE_PTR(opcua_client_transport_port);

  public:
      opcua_client_transport_port();


      static std::map<rx_node_id, opcua_client_transport_port::smart_ptr> runtime_instances;


  protected:

  private:


};


} // namespace opcua_transport
} // namespace opcua
} // namespace protocols



#endif
