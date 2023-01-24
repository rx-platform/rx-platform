

/****************************************************************************
*
*  discovery\rx_discovery_configuration.h
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


#ifndef rx_discovery_configuration_h
#define rx_discovery_configuration_h 1




/////////////////////////////////////////////////////////////
// logging macros
#define DISCOVERY_LOG_INFO(src,lvl,msg) RX_LOG_INFO("DISCOVERY",src,lvl,msg)
#define DISCOVERY_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("DISCOVERY",src,lvl,msg)
#define DISCOVERY_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("DISCOVERY",src,lvl,msg)
#define DISCOVERY_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("DISCOVERY",src,lvl,msg)
#define DISCOVERY_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("DISCOVERY",src,lvl,msg)
#define DISCOVERY_LOG_TRACE(src,lvl,msg) RX_TRACE("DISCOVERY",src,lvl,msg)

#define RX_DISCOVERY_CONNECTION_MAX_COUNT 16

#define RX_DISCOVERY_CONNECTION_NAME_PREFIX "peer_" 
#define RX_DISCOVERY_PROTOCOL_CONNECTION_NAME_PREFIX "peerconn_"
#define RX_DISCOVERY_PROTOCOL_CHANNEL_NAME_PREFIX "peerchannel_" 
#define RX_DISCOVERY_PROTOCOL_TRANSPORT_NAME_PREFIX "peertrans_" 
#define RX_DISCOVERY_PROTOCOL_TCP_NAME_PREFIX "peertcp_"
#define RX_DISCOVERY_CONNECTION_ID_FIRST 0x4000001 

#define RX_DISCOVERY_CONNECTION_ID_BEYOND_LAST RX_DISCOVERY_CONNECTION_ID_FIRST + RX_DISCOVERY_CONNECTION_MAX_COUNT



namespace rx_internal {
namespace discovery {
class peer_connection;
typedef rx_reference<peer_connection> peer_connection_ptr;
}
}





#endif
