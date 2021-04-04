

/****************************************************************************
*
*  interfaces\rx_interfaces.h
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


#ifndef rx_interfaces_h
#define rx_interfaces_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"

// rx_udp
#include "interfaces/rx_udp.h"



namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {






class rx_ip_address 
{

  public:
      rx_ip_address();

      rx_ip_address(const rx_ip_address &right);

      virtual ~rx_ip_address();

      rx_ip_address & operator=(const rx_ip_address &right);

	  rx_ip_address & operator=(rx_ip_address &&right);
  protected:

  private:


};






class rx_ethernet_card 
{
	typedef std::vector<rx_ip_address> addresses_type;

  public:
      rx_ethernet_card();

      virtual ~rx_ethernet_card();

	  rx_ethernet_card & operator=(rx_ethernet_card &&right);
  protected:

  private:
      rx_ethernet_card(const rx_ethernet_card &right);

      rx_ethernet_card & operator=(const rx_ethernet_card &right);



      addresses_type addresses_;


};


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
