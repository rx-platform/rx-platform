

/****************************************************************************
*
*  interfaces\rx_file_ports.h
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


#ifndef rx_file_ports_h
#define rx_file_ports_h 1



// dummy
#include "dummy.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"

namespace rx_internal {
namespace interfaces {
namespace file_endpoints {
class file_port;
class file_endpoint;

} // namespace file_endpoints
} // namespace interfaces
} // namespace rx_internal




namespace rx_internal {

namespace interfaces {

namespace file_endpoints {





class file_endpoint 
{

  public:
      file_endpoint();

      ~file_endpoint();


      rx_protocol_stack_endpoint* get_stack_endpoint ();

      runtime::items::port_runtime* get_port ();

      rx_protocol_result_t send_packet (send_protocol_packet packet);

      rx_result open (const string_type& directory, const string_type& filter, security::security_context_ptr identity, file_port* port);

      rx_result close ();

      bool is_connected () const;

      void watch_callback (uint32_t watch_id, int action, const char* rootdir, const char* filepath, const char* oldfilepath);

      void received_data (string_type name, byte_string data);


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      rx_protocol_stack_endpoint stack_endpoint_;

      file_port *my_port_;


      uint32_t watch_id_;

      string_type file_filter_;


};







typedef rx_platform::runtime::io_types::ports_templates::extern_singleton_port_impl< file_endpoint  > file_port_base;


struct dmon_initializer
{
    dmon_initializer();
    ~dmon_initializer();
};




class file_port : public file_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
File port class. implementation of file share communication port");

    DECLARE_REFERENCE_PTR(file_port);

  public:
      file_port();

      ~file_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      virtual rx_result_with<port_connect_result> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      rx_result stop_passive ();

      void received_data (file_endpoint* ep, string_type name, byte_string data);


      rx_platform::runtime::io_types::external_port_status status;


  protected:

  private:


      std::unique_ptr<file_endpoint> endpoint_;


      string_type dir_path_;

      local_value<string_type> file_filter_;

      local_value<uint32_t> file_timeout_;


};


} // namespace file_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
