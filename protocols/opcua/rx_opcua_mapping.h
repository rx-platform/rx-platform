

/****************************************************************************
*
*  protocols\opcua\rx_opcua_mapping.h
*
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



// dummy
#include "dummy.h"
// rx_endpoints
#include "interfaces/rx_endpoints.h"



namespace protocols {

namespace opc_ua {

constexpr size_t opc_ua_endpoint_name_len = 0x100;




typedef interfaces::io_endpoints::rx_io_address< std::array<char, opc_ua_endpoint_name_len>  > opc_ua_endpoint;







class opc_ua_trasport : public rx_protocol_stack_entry  
{

  public:
	  
  protected:

  private:


};


} // namespace opc_ua
} // namespace protocols



#endif
