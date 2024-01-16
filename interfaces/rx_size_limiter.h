

/****************************************************************************
*
*  interfaces\rx_size_limiter.h
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


#ifndef rx_size_limiter_h
#define rx_size_limiter_h 1



// dummy
#include "dummy.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"

namespace rx_internal {
namespace interfaces {
namespace ports_lib {
class size_limiter_port;

} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal


using namespace rx_platform::runtime;


namespace rx_internal {

namespace interfaces {

namespace ports_lib {





class size_limiter_endpoint 
{

  public:
      size_limiter_endpoint (size_limiter_port* port);

      ~size_limiter_endpoint();


      rx_protocol_stack_endpoint* bind ();

      runtime::items::port_runtime* get_port ();


  protected:

  private:

      rx_protocol_result_t send_packet (send_protocol_packet packet);



      rx_protocol_stack_endpoint stack_endpoint_;

      size_limiter_port *port_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< size_limiter_endpoint  > size_limiter_port_base;






class size_limiter_port : public size_limiter_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 1, 0, "\
Transaction limiter port, limits the transactions sent by master/client.\r\n\
Use limit value 1 for enabling half-duplex connection, or 0 for no limit.");

    DECLARE_REFERENCE_PTR(size_limiter_port);

  public:
      size_limiter_port();


      rx_result initialize_runtime (runtime_init_context& ctx);

      uint32_t get_limit () const;


      rx_platform::runtime::io_types::simple_port_status status;


  protected:

  private:


      async_local_value<uint32_t> limit_;


};


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal



#endif
