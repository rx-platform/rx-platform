

/****************************************************************************
*
*  protocols\http\rx_http_mapping.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_http_mapping_h
#define rx_http_mapping_h 1



// dummy
#include "dummy.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_io_buffers
#include "system/runtime/rx_io_buffers.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_http_server
#include "http_server/rx_http_server.h"

#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "protocols/ansi_c/http_c/rx_http_c_impl.h"

using namespace rx_platform::http;



namespace protocols {

namespace rx_http {






class rx_http_endpoint : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(rx_http_endpoint);

  public:
      rx_http_endpoint (runtime::items::port_runtime* port);

      ~rx_http_endpoint();


      rx_protocol_stack_endpoint* bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);

      void close_endpoint ();

      rx_result send_response (http_response response);


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      rx_protocol_result_t create_and_forward_request (const char* method, size_t method_len, const char* path, size_t path_len, phr_header* headers, size_t num_headers);



      rx_protocol_stack_endpoint stack_entry_;

      rx_platform::runtime::io_types::rx_io_buffer receive_buffer_;


      rx_thread_handle_t executer_;

      runtime::items::port_runtime* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< protocols::rx_http::rx_http_endpoint  > rx_http_port_base;






class rx_http_port : public rx_http_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
HTTP protocol port class. Implementation of a rx-platform HTTP protocol");

    DECLARE_REFERENCE_PTR(rx_http_port);

  public:
      rx_http_port();


      void stack_assembled ();


  protected:

  private:


};


} // namespace rx_http
} // namespace protocols



#endif
