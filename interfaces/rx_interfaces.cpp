

/****************************************************************************
*
*  interfaces\rx_interfaces.cpp
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


#include "pch.h"


// rx_interfaces
#include "interfaces/rx_interfaces.h"



namespace interfaces {

namespace ip_endpoints {

// Class interfaces::ip_endpoints::rx_ethernet_card 

rx_ethernet_card::rx_ethernet_card()
{
}

rx_ethernet_card::rx_ethernet_card(const rx_ethernet_card &right)
{
}


rx_ethernet_card::~rx_ethernet_card()
{
}


rx_ethernet_card & rx_ethernet_card::operator=(const rx_ethernet_card &right)
{
	assert(false);
	return *this;// don't know what to do here not decided jet
}


rx_ethernet_card & rx_ethernet_card::operator=(rx_ethernet_card &&right)
{
	assert(false);
	return *this;
}
// Class interfaces::ip_endpoints::rx_ip_address 

rx_ip_address::rx_ip_address()
{
}

rx_ip_address::rx_ip_address(const rx_ip_address &right)
{
}


rx_ip_address::~rx_ip_address()
{
}


rx_ip_address & rx_ip_address::operator=(const rx_ip_address &right)
{
	assert(false);
	return *this;// don't know what to do here not decided jet
}




rx_ip_address & rx_ip_address::operator=(rx_ip_address &&right)
{
	assert(false);
	return *this;
}
} // namespace ip_endpoints
} // namespace interfaces

