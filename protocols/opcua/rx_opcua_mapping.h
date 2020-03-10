

/****************************************************************************
*
*  protocols\opcua\rx_opcua_mapping.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_opcua_mapping_h
#define rx_opcua_mapping_h 1



// rx_port_types
#include "system/runtime/rx_port_types.h"
// dummy
#include "dummy.h"

#include "protocols/ansi_c/common_c/rx_protocol_base.h"
#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"


namespace protocols {

namespace opcua {

constexpr size_t opc_ua_endpoint_name_len = 0x100;




class opcua_transport_endpoint : public opcua_transport_protocol_type  
{

  public:
      opcua_transport_endpoint();


      void bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_entry* reference,const protocol_endpoint* end_point, rx_const_packet_buffer* buffer);

      static rx_protocol_result_t send_function (rx_protocol_stack_entry* reference,const protocol_endpoint* end_point, rx_packet_buffer* buffer);



      std::function<void(int64_t)> sent_func_;

      std::function<void(int64_t)> received_func_;


};






class opcua_transport_port : public rx_platform::runtime::io_types::transport_port  
{
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
OPC-UA transport port. Implementation of binary OPC-UA transport and simplified local pipe version without secure channel.");

    DECLARE_REFERENCE_PTR(opcua_transport_port);

  public:
      opcua_transport_port();


      rx_protocol_stack_entry* create_stack_entry ();

      void bind_port ();


  protected:

  private:


      opcua_transport_endpoint endpoint_;


};


} // namespace opcua
} // namespace protocols



#endif
