

/****************************************************************************
*
*  protocols\http\rx_http_path_mapping.h
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


#ifndef rx_http_path_mapping_h
#define rx_http_path_mapping_h 1



// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"
// dummy
#include "dummy.h"

namespace protocols {
namespace rx_http {
class rx_http_host_name_port;
class rx_http_addressing_port;

} // namespace rx_http
} // namespace protocols




namespace protocols {

namespace rx_http {





class rx_http_path_endpoint 
{

  public:
      rx_http_path_endpoint (rx_reference<rx_http_addressing_port> port, bool host_name);

      ~rx_http_path_endpoint();


      rx_reference<rx_http_addressing_port> get_port ();


      rx_protocol_stack_endpoint stack_entry;


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



      rx_reference<rx_http_addressing_port> my_port_;


      io::rx_io_buffer my_buffer_;

      bool use_host_name_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< rx_http_path_endpoint  > rx_http_addressing_port_base;






class rx_http_addressing_port : public rx_http_addressing_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
HTTP addersing port class. Extracts just path of the request.");

    DECLARE_REFERENCE_PTR(rx_http_addressing_port);

  public:
      rx_http_addressing_port();

      ~rx_http_addressing_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);


      rx_platform::runtime::io_types::simple_port_status status;


  protected:

  private:


      async_local_value<uint32_t> max_packet_size_;

      bool use_host_name_;


};






class rx_http_host_name_endpoint 
{

  public:
      rx_http_host_name_endpoint (rx_reference<rx_http_host_name_port> port, bool host_name);

      ~rx_http_host_name_endpoint();


      rx_reference<rx_http_host_name_port> get_port ();

      void close_endpoint ();


      rx_protocol_stack_endpoint stack_entry;


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



      rx_reference<rx_http_host_name_port> my_port_;


      bool use_host_name_;

      io::rx_io_buffer my_buffer_;

      std::atomic<bool> connected_;


};







typedef rx_platform::runtime::io_types::ports_templates::connection_transport_port_impl< rx_http_host_name_endpoint  > rx_htt_host_name_port_base;






class rx_http_host_name_port : public rx_htt_host_name_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
HTTP addersing port class. Extracts just Host of the request.");

    DECLARE_REFERENCE_PTR(rx_http_host_name_port);

  public:
      rx_http_host_name_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


      rx_platform::runtime::io_types::simple_port_status status;


  protected:

  private:


      async_local_value<uint32_t> max_packet_size_;

      bool use_host_name_;


};


} // namespace rx_http
} // namespace protocols



#endif
