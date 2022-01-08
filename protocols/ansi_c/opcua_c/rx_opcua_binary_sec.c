

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_binary_sec.c
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#include "pch.h"

#include "rx_opcua_transport.h"

// rx_opcua_binary_sec
#include "protocols/ansi_c/opcua_c/rx_opcua_binary_sec.h"


#pragma pack(push)
#pragma pack(1)
enum opcua_sec_token_request_type
{
	ISSUE_0 = 0,
	RENEW_1 = 1
};


typedef struct opcua_request_header_def
{
	uint8_t auth_token[2];// always 0,0
	uint64_t timestamp;
	uint32_t request_handle;
	uint32_t diagnostics;// anything than zero is not supported
	uint32_t audit_entry_id;// -1 is the only supported
	uint32_t timeout_hint;
	uint8_t extension_object[3]; // supported 0,0,0 // no object!!!
} opcua_request_header;


typedef struct opcua_response_header_def
{
	uint64_t timestamp;
	uint32_t request_handle;
	uint32_t result;
	uint8_t diagnostics; // supported 0, no info
	uint32_t diagnostic_strings; // no diagnostics supported so 0 or -1
	uint8_t extension_object[3]; // supported 0,0,0 // no object!!!

} opcua_response_header;

#pragma pack(pop)


typedef struct opcua_simetric_security_header_def
{
	int policy_len;
	uint8_t* policy;
	
	int cert_len;
	uint8_t* cert;

	int thumb_len;
	uint8_t* thumb;

} opcua_simetric_security_header;


rx_protocol_result_t check_request_header()
{
	return RX_PROTOCOL_OK;
}
rx_protocol_result_t parse_asymetric_security_header(struct rx_protocol_stack_endpoint* reference, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{

	uint32_t channel_id;
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	int32_t temp_len;
	uint8_t* policy_buffer = NULL;
	uint8_t* cert_buffer = NULL;
	uint8_t* thumb_buffer = NULL;


	result = rx_pop_from_packet(buffer, &channel_id, sizeof(channel_id));
	if (result != RX_PROTOCOL_OK)
		return result;

	result = rx_pop_from_packet(buffer, &temp_len, sizeof(temp_len));
	if (result != RX_PROTOCOL_OK)
		return result;
	if (temp_len > 0)
	{
		policy_buffer = malloc(temp_len);
		result = rx_pop_from_packet(buffer, policy_buffer, temp_len);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	result = rx_pop_from_packet(buffer, &temp_len, sizeof(temp_len));
	if (result != RX_PROTOCOL_OK)
		return result;
	if (temp_len > 0)
	{
		cert_buffer = malloc(temp_len);
		result = rx_pop_from_packet(buffer, cert_buffer, temp_len);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	result = rx_pop_from_packet(buffer, &temp_len, sizeof(temp_len));
	if (result != RX_PROTOCOL_OK)
		return result;
	if (temp_len > 0)
	{
		thumb_buffer = malloc(temp_len);
		result = rx_pop_from_packet(buffer, thumb_buffer, temp_len);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	return result;
}

const uint8_t open_channel_node_id[] = { 0x01, 0x00, 0xbe, 0x01 };

rx_protocol_result_t opcua_parse_open_message(struct rx_protocol_stack_endpoint* reference, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	opcua_sequence_header* sequence_header;
	const struct opcua_request_header_def* prequest = NULL;

	rx_protocol_result_t result = parse_asymetric_security_header(reference, transport, header, buffer, id);
	if (result != RX_PROTOCOL_OK)
		return result;

	const uint8_t* ext_node_id = rx_get_from_packet(buffer, sizeof(open_channel_node_id), &result);
	if (result != RX_PROTOCOL_OK)
		return result;

	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_sequence_header), &result);
	if (!sequence_header)
		return result;

	// check nodeId for OpenSecureChannelRequest
	if (memcmp(ext_node_id, open_channel_node_id, sizeof(open_channel_node_id)) != 0)
		return RX_PROTOCOL_INVALID_SEQUENCE;

	prequest = rx_get_from_packet(buffer, sizeof(struct opcua_request_header_def), &result);
	if (!prequest)
		return result;

	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t opcua_parse_close_message(struct rx_protocol_stack_endpoint* reference, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}


