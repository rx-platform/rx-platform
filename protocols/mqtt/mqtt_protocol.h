

/****************************************************************************
*
*  protocols\mqtt\mqtt_protocol.h
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


#ifndef mqtt_protocol_h
#define mqtt_protocol_h 1





/////////////////////////////////////////////////////////////
// logging macros for opcua library
#define MQTT_LOG_INFO(src,lvl,msg) RX_LOG_INFO("MQTT",src,lvl,msg)
#define MQTT_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("MQTT",src,lvl,msg)
#define MQTT_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("MQTT",src,lvl,msg)
#define MQTT_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("MQTT",src,lvl,msg)
#define MQTT_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("MQTT",src,lvl,msg)
#define MQTT_LOG_TRACE(src,lvl,msg) RX_TRACE("MQTT",src,lvl,msg)


namespace protocols {
namespace mqtt {


#pragma pack(push)
#pragma pack(1)


enum class mqtt_protocol_version : uint8_t
{
	version311 = 4,
	version50 = 5
};

enum class mqtt_message_type
{
	Reserved0	= 0,//	Reserved
	CONNECT		= 1,//	Client request to connect to Server
	CONNACK		= 2,//	Connect Acknowledgment
	PUBLISH		= 3,//	Publish message
	PUBACK		= 4,//	Publish Acknowledgment
	PUBREC		= 5,//	Publish Received(assured delivery part 1)
	PUBREL		= 6,//	Publish Release(assured delivery part 2)
	PUBCOMP		= 7,//	Publish Complete(assured delivery part 3)
	SUBSCRIBE	= 8,//	Client Subscribe request
	SUBACK		= 9,//	Subscribe Acknowledgment
	UNSUBSCRIBE	= 10,//	Client Unsubscribe request
	UNSUBACK	= 11,//	Unsubscribe Acknowledgment
	PINGREQ		= 12,//	PING Request
	PINGRESP	= 13,//	PING Response
	DISCONNECT	= 14,//	Client is Disconnecting
	Reserved15	= 15 //	Reserved
};

enum class mqtt_qos_level
{
	Level0		= 0,
	Level1		= 1,
	Level2		= 2,
	Reserved	= 3
};

enum class mqtt_retain_handling
{
	at_subscribe = 0,
	at_new_subscribe = 1,
	never = 2
};

struct fix_header
{
	uint8_t header_flags;
	uint8_t length;
};

struct connect_var_header
{
	uint16_t protocol_name_length;
	uint8_t protocol_name_0;
	uint8_t protocol_name_1;
	uint8_t protocol_name_2;
	uint8_t protocol_name_3;
	uint8_t protocol_version;
	uint8_t connect_flags;
	uint16_t keep_alive;
};


struct connect_ack_var_header
{
	uint8_t ack_flags;
	uint8_t reason_code;
};


#pragma pack(pop)





} // namespace mqtt_simple
} // namespace mqtt



#endif
