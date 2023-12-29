

/****************************************************************************
*
*  interfaces\rx_stxetx.h
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


#ifndef rx_stxetx_h
#define rx_stxetx_h 1



// dummy
#include "dummy.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"
// rx_io_buffers
#include "lib/rx_io_buffers.h"

namespace rx_internal {
namespace interfaces {
namespace ports_lib {
class stxetx_port;

} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"


namespace rx_internal {

namespace interfaces {

namespace ports_lib {





class stxetx_endpoint 
{

  public:
      stxetx_endpoint (stxetx_port* port);

      ~stxetx_endpoint();


      rx_protocol_stack_endpoint* bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);

      runtime::items::port_runtime* get_port ();


  protected:

  private:

      rx_protocol_result_t send_packet (send_protocol_packet packet);

      rx_protocol_result_t received_packet (recv_protocol_packet packet);



      rx_protocol_stack_endpoint stack_endpoint_;

      stxetx_port *port_;

      rx::io::rx_io_buffer collect_buffer_;


      bool collecting_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< stxetx_endpoint  > stxetx_port_base;






class stxetx_port : public stxetx_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
STX/ETX ASCII transport port.\r\n\
Extracts packet based on the start and end character");

    DECLARE_REFERENCE_PTR(stxetx_port);

  public:
      stxetx_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);


      uint32_t max_buffer_size;


  protected:

  private:


};


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal



#endif
