

/****************************************************************************
*
*  interfaces\rx_ether_defs.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_ether_defs_h
#define rx_ether_defs_h 1





namespace rx_internal {
namespace interfaces {
namespace ethernet {


#define ETH_TYPE_VLAN 0x8100

#pragma pack(push)
#pragma pack(1)

struct eth_header
{
	uint8_t destination_mac[MAC_ADDR_SIZE];
	uint8_t source_mac[MAC_ADDR_SIZE];
	uint16_t eth_type;
};


struct eth_vlan_header
{
	uint8_t destination_mac[MAC_ADDR_SIZE];
	uint8_t source_mac[MAC_ADDR_SIZE];
	uint16_t vlan_type;
	uint16_t vlan;
	uint16_t eth_type;
};


#pragma pack(pop)






} // namespace ethernet
} // namespace interfaces
} // namespace rx_internal


#endif
