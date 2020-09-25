

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_binary_sec.h
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


#ifndef rx_opcua_binary_sec_h
#define rx_opcua_binary_sec_h 1





rx_protocol_result_t opcua_parse_open_message(struct rx_protocol_stack_endpoint* reference, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id);
rx_protocol_result_t opcua_parse_close_message(struct rx_protocol_stack_endpoint* reference, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id);




#endif
