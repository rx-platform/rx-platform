

/****************************************************************************
*
*  system\runtime\rx_transaction_limiter.h
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


#ifndef rx_transaction_limiter_h
#define rx_transaction_limiter_h 1



// dummy
#include "dummy.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_lib {





class limiter_endpoint 
{

  public:
      limiter_endpoint (runtime::items::port_runtime* port);


      rx_protocol_stack_endpoint* bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);



      rx_protocol_stack_endpoint stack_endpoint_;


      runtime::items::port_runtime* port_;


};







typedef ports_templates::transport_port_impl< rx_platform::runtime::io_types::ports_lib::limiter_endpoint  > transaction_limiter_port_base;






class transaction_limiter_port : public transaction_limiter_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Transaction limiter port, limits the transactions sent by master/client.\r\n\
Use limit value 1 for enabling half-duplex connection, or 0 for no limit.");

    DECLARE_REFERENCE_PTR(transaction_limiter_port);

  public:

  protected:

  private:


};


} // namespace ports_lib
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
