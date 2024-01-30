

/****************************************************************************
*
*  discovery\rx_discovery_protocol.h
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


#ifndef rx_discovery_protocol_h
#define rx_discovery_protocol_h 1





#define RX_FIRST_DISCOVERY_VERSION 0x10000

#define RX_CURRENT_DISCOVERY_VERSION RX_FIRST_DISCOVERY_VERSION
#define RX_DISCOVERY_KNOWN_UINT 0xAABBCCDD

namespace rx_internal {
namespace discovery {

#pragma pack(push)
#pragma pack(1)

struct rx_discovery_header_t
{
	uint32_t known;//0xAABBCCDD (RX_DISCOVERY_KNOWN_UINT)
	rx_uuid_t idenity;
	uint32_t version;
	uint32_t status;
	uint8_t type;
	uint16_t size;
};

#define DISCOVERY_MESSAGE_MIN 1
#define DISCOVERY_NEW_MASTER DISCOVERY_MESSAGE_MIN
#define DISCOVERY_FALLBACK DISCOVERY_MESSAGE_MIN + 1
#define DISCOVERY_QUERY DISCOVERY_MESSAGE_MIN + 2
#define DISCOVERY_MASTERLIST + DISCOVERY_MESSAGE_MIN + 3

#define DISCOVERY_MESSAGE_MAX DISCOVERY_MESSAGE_MIN + 3



#pragma pack(pop)



} // namespace discovery
} // namespace rx_internal


#endif
