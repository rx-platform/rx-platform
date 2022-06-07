

/****************************************************************************
*
*  protocols\opcua\rx_opcua_basic.h
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


#ifndef rx_opcua_basic_h
#define rx_opcua_basic_h 1



// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// dummy
#include "dummy.h"
// rx_opcua_addr_space
#include "protocols/opcua/rx_opcua_addr_space.h"
// rx_opcua_server
#include "protocols/opcua/rx_opcua_server.h"
// rx_opcua_std
#include "protocols/opcua/rx_opcua_std.h"

namespace protocols {
namespace opcua {
namespace opcua_server {
class opcua_basic_server_port;

} // namespace opcua_server
} // namespace opcua
} // namespace protocols


#include "rx_opcua_requests.h"


namespace protocols {

namespace opcua {

namespace opcua_server {






class opcua_basic_server_endpoint : public opcua_server_endpoint_base  
{
    DECLARE_REFERENCE_PTR(opcua_basic_server_endpoint);

  public:
      opcua_basic_server_endpoint (const string_type& endpoint_url, const string_type& app_name, const string_type& app_uri, opcua_basic_server_port* port);

      ~opcua_basic_server_endpoint();


      void close_endpoint ();

      rx_result send_response (requests::opcua_response_ptr resp);


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }


      opcua_basic_server_port* get_port ()
      {
        return port_;
      }



      rx_protocol_stack_endpoint stack_entry;


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      rx_thread_handle_t executer_;

      opcua_basic_server_port* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< opcua_basic_server_endpoint  > opcua_basic_server_port_base;






class opcua_simple_address_space : public opcua_addr_space::opcua_address_space_base  
{

  public:
      opcua_simple_address_space();


      void set_parent (opcua_addr_space::opcua_address_space_base* parent);

      rx_result register_node (opcua_addr_space::opcua_node_base* what);

      rx_result unregister_node (opcua_addr_space::opcua_node_base* what);

      void read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const;


  protected:

  private:


      opcua_addr_space::opcua_address_space_base *parent_;


      locks::slim_lock ns_lock_;


};






class opcua_basic_server_port : public opcua_basic_server_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Basic OPC UA protocol port class. Basic implementation OPC UA binary protocol core with simple mapping.");

    DECLARE_REFERENCE_PTR(opcua_basic_server_port);

  public:
      opcua_basic_server_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);


      opcua_addr_space::opcua_std_address_space std_address_space;

      opcua_simple_address_space address_space;


      static std::map<rx_node_id, opcua_basic_server_port::smart_ptr> runtime_instances;


  protected:

  private:


      string_type app_name_;

      string_type app_uri_;


};


} // namespace opcua_server
} // namespace opcua
} // namespace protocols



#endif
