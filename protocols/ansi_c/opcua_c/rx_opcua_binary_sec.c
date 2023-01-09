

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_binary_sec.c
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


#include "pch.h"


// rx_opcua_binary_sec
#include "protocols/ansi_c/opcua_c/rx_opcua_binary_sec.h"

#include "rx_opcua_binary_ser.h"
#include "common/rx_common.h"


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


typedef struct opcua_request_open_channel_def
{
	opcua_request_header request_header;

	uint32_t client_protocol;
	uint32_t request_type;
	uint32_t security_mode;
	int32_t client_nonce;
	//uint32_t lifetime;

} opcua_request_open_channel;


typedef struct opcua_request_close_channel_def
{
	opcua_request_header request_header;

} opcua_request_close_channel;


typedef struct opcua_response_header_def
{
	uint64_t timestamp;
	uint32_t request_handle;
	uint32_t result;
	uint8_t diagnostics; // supported 0, no info
	uint32_t diagnostic_strings; // no diagnostics supported so 0 or -1
	uint8_t extension_object[3]; // supported 0,0,0 // no object!!!

} opcua_response_header;


typedef struct opcua_response_open_channel_def
{
	opcua_response_header response_header;

	uint32_t server_protocol;
	uint32_t secure_channel_id;
	uint32_t token_id;
	uint64_t created;
	uint32_t lifetime;

	int32_t nonce_size;

	// uint8_t nonce[nonce_size]

} opcua_response_open_channel;


typedef struct opcua_response_close_channel_def
{
	opcua_response_header response_header;

} opcua_response_close_channel;

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


const uint32_t invalid_sequence_id = (uint32_t)(-1);


const size_t opcua_regular_headers_size = sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header) + sizeof(opcua_sequence_header);



rx_protocol_result_t opcua_init_sec_none_transport_state(opcua_sec_none_protocol_type* transport)
{
	rx_protocol_result_t result;


	transport->current_state = opcua_transport_idle;
	memset(&transport->connection_data, 0, sizeof(opcua_acknowledge_message));
	transport->connection_data.protocol_version = RX_OPCUA_MAX_VERSION;
	transport->received_sequence_id = invalid_sequence_id;
	transport->last_chunk_received = 0;
	transport->current_request_id = 0;
	transport->current_sequence_id = 1;

	result = rx_init_packet_buffer(&transport->receive_buffer, 0x10000, 0x0);

	return result;
}


rx_protocol_result_t parse_asymetric_security_header(struct rx_protocol_stack_endpoint* reference, opcua_sec_none_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
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

const uint8_t open_channel_request_node_id[] = { 0x01, 0x00, 0xbe, 0x01 };
const uint8_t open_channel_response_node_id[] = { 0x01, 0x00, 0xc1, 0x01 };

rx_protocol_result_t opcua_parse_open_message(struct rx_protocol_stack_endpoint* reference, opcua_sec_none_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	opcua_sequence_header* sequence_header;
	const struct opcua_request_open_channel_def* prequest = NULL;
	uint32_t channel_id = 0;
	uint8_t nonce[] = { 0x01 };
	uint8_t client_nonce[0x40]; // max 40 bytes for nonce
	uint32_t requested_lifetime = 0;

	rx_protocol_result_t result = parse_asymetric_security_header(reference, transport, header, buffer, id);
	if (result != RX_PROTOCOL_OK)
		return result;

	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_sequence_header), &result);
	if (!sequence_header)
		return result;

	transport->current_request_id = sequence_header->request_id;

	const uint8_t* ext_node_id = rx_get_from_packet(buffer, sizeof(open_channel_request_node_id), &result);
	if (result != RX_PROTOCOL_OK)
		return result;

	// check nodeId for OpenSecureChannelRequest
	if (memcmp(ext_node_id, open_channel_request_node_id, sizeof(open_channel_request_node_id)) != 0)
		return RX_PROTOCOL_INVALID_SEQUENCE;

	prequest = rx_get_from_packet(buffer, sizeof(struct opcua_request_open_channel_def), &result);
	if (!prequest)
		return result;

	if (prequest->client_nonce > 0)
	{
		if ((uint32_t)prequest->client_nonce > sizeof(client_nonce) / sizeof(client_nonce[0]))
			return RX_PROTOCOL_INVALID_ARG;
		result = rx_pop_from_packet(buffer, client_nonce, (size_t)prequest->client_nonce);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	result = rx_pop_from_packet(buffer, &requested_lifetime, sizeof(requested_lifetime));
	if (result != RX_PROTOCOL_OK)
		return result;

	rx_packet_buffer result_buffer;
	result = reference->allocate_packet(reference, &result_buffer);
	if (result != RX_PROTOCOL_OK)
		return result;


	opcua_transport_header* header_ptr = (opcua_transport_header*)rx_alloc_from_packet(&result_buffer
		, sizeof(opcua_transport_header), &result);
	if (result != RX_PROTOCOL_OK)
		return result;
	header_ptr->is_final = 'F';
	header_ptr->message_type[0] = 'O';
	header_ptr->message_type[1] = 'P';
	header_ptr->message_type[2] = 'N';
	header_ptr->message_size = 0;


	uint32_t rand_id;
	do
	{
		if (RX_OK == rx_crypt_gen_random(&rand_id, sizeof(rand_id)))
		{
			if (rand_id != 0)// && active_handles_.find(new_id) == active_handles_.end())
				break;
		}
	} while (1);

	result = rx_push_to_packet(&result_buffer, &rand_id, sizeof(rand_id));
	if (result != RX_PROTOCOL_OK)
		return result;

	result = opcua_write_string(&result_buffer, "http://opcfoundation.org/UA/SecurityPolicy#None");
	if (result != RX_PROTOCOL_OK)
		return result;

	int32_t null_len = -1;

	result = rx_push_to_packet(&result_buffer, &null_len, sizeof(null_len));
	if (result != RX_PROTOCOL_OK)
		return result;

	result = rx_push_to_packet(&result_buffer, &null_len, sizeof(null_len));
	if (result != RX_PROTOCOL_OK)
		return result;

	sequence_header = (opcua_sequence_header*)rx_alloc_from_packet(&result_buffer
		, sizeof(opcua_sequence_header), &result);
	if (result != RX_PROTOCOL_OK)
		return result;



	result = rx_push_to_packet(&result_buffer, &open_channel_response_node_id, sizeof(open_channel_response_node_id));
	if (result != RX_PROTOCOL_OK)
		return result;


	opcua_response_open_channel response_data;
	memzero(&response_data, sizeof(response_data));
	response_data.response_header.request_handle = prequest->request_header.request_handle;
	rx_time_struct now;
	rx_os_get_system_time(&now);
	response_data.created = now.t_value;
	response_data.response_header.timestamp = now.t_value;
	response_data.lifetime = requested_lifetime;
	response_data.secure_channel_id = rand_id;
	response_data.token_id = rand_id;
	response_data.nonce_size = sizeof(nonce)/sizeof(nonce[0]);

	transport->channel_id = rand_id;
	transport->token_id = rand_id;

	result = rx_push_to_packet(&result_buffer, &response_data, sizeof(response_data));
	if (result != RX_PROTOCOL_OK)
		return result;


	result = rx_push_to_packet(&result_buffer, nonce, sizeof(nonce));
	if (result != RX_PROTOCOL_OK)
		return result;


	header_ptr->message_size = (uint32_t)rx_get_packet_usable_data(&result_buffer);


	sequence_header->request_id = transport->current_request_id;
	sequence_header->sequence_number = transport->current_sequence_id;
	// TODO - handle rollover
	transport->current_sequence_id++;

	send_protocol_packet to_send = rx_create_send_packet(sequence_header->request_id, &result_buffer, 0, 0);

	result = rx_move_packet_down(reference, to_send);

	return result;
}


const uint8_t close_channel_request_node_id[] = { 0x01, 0x00, 0xc4, 0x01 };
const uint8_t close_channel_response_node_id[] = { 0x01, 0x00, 0xc7, 0x01 };

rx_protocol_result_t opcua_parse_close_message(struct rx_protocol_stack_endpoint* reference, opcua_sec_none_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;
	const struct opcua_request_close_channel_def* prequest = NULL;
	rx_protocol_result_t result;

	// retrieve headers
	sec_header = (opcua_security_simetric_header*)rx_get_from_packet(buffer, sizeof(opcua_security_simetric_header), &result);
	if (!sec_header)
		return result;

	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_sequence_header), &result);
	if (!sequence_header)
		return result;

	// check security header
	if (sec_header->channel_id != transport->channel_id || sec_header->token_id != transport->token_id)
		return RX_PROTOCOL_PARSING_ERROR;

	// sequence header
	if (transport->received_sequence_id != invalid_sequence_id)
	{
		// TODO - handle rollover
		transport->received_sequence_id++;
		if (transport->received_sequence_id != sequence_header->sequence_number)
			return RX_PROTOCOL_INVALID_SEQUENCE;
		if (transport->current_request_id != sequence_header->request_id)
		{// new request
			if (!transport->last_chunk_received)
				return RX_PROTOCOL_COLLECT_ERROR;
			result = rx_reinit_packet_buffer(&transport->receive_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;
			transport->received_sequence_id = sequence_header->sequence_number;
			transport->current_request_id = sequence_header->request_id;
		}
	}
	else
	{
		// this is a first regular transport packet
		transport->received_sequence_id = sequence_header->sequence_number;
		transport->current_request_id = sequence_header->request_id;
		result = rx_reinit_packet_buffer(&transport->receive_buffer);
		if (result != RX_PROTOCOL_OK)
			return result;
	}

	transport->current_request_id = sequence_header->request_id;

	if (header->is_final != 'F' || !transport->last_chunk_received)
		return RX_PROTOCOL_COLLECT_ERROR;

	const uint8_t* ext_node_id = rx_get_from_packet(buffer, sizeof(close_channel_request_node_id), &result);
	if (result != RX_PROTOCOL_OK)
		return result;

	// check nodeId for OpenSecureChannelRequest
	if (memcmp(ext_node_id, close_channel_request_node_id, sizeof(close_channel_request_node_id)) != 0)
		return RX_PROTOCOL_INVALID_SEQUENCE;

	prequest = rx_get_from_packet(buffer, sizeof(struct opcua_request_close_channel_def), &result);
	if (!prequest)
		return result;

	return result;

}



rx_protocol_result_t opcua_bin_init_sec_none_transport(opcua_sec_none_protocol_type* transport
	, int server_side)
{
	rx_protocol_result_t result;
	memset(transport, 0, sizeof(opcua_sec_none_protocol_type));
	rx_init_stack_entry(&transport->stack_entry, transport);

	// fill protocol stack header
	if (!server_side)
		transport->stack_entry.connected_function = opcua_bin_sec_none_client_connected;
	transport->stack_entry.received_function = opcua_bin_sec_none_bytes_received;
	transport->stack_entry.send_function = opcua_bin_sec_none_bytes_send;
	//transport->stack_entry.closed_function = opcua_bin_closed;
	//transport->stack_entry.close_function = opcua_bin_close;
	// fill options
	transport->server_side = server_side;
	// decoder
	
	// fill state
	result = opcua_init_sec_none_transport_state(transport);
	if (result != RX_PROTOCOL_OK)
		return result;

	result = rx_init_packet_buffer(&transport->receive_buffer, 0x10000, 0x0);
	if (result != RX_PROTOCOL_OK)
		return result;

	return RX_PROTOCOL_OK;
}
rx_protocol_result_t opcua_bin_init_sec_none_client_transport(opcua_sec_none_protocol_type* transport)
{
	return opcua_bin_init_sec_none_transport(transport, 0);
}
rx_protocol_result_t opcua_bin_init_sec_none_server_transport(opcua_sec_none_protocol_type* transport)
{
	return opcua_bin_init_sec_none_transport(transport, 1);
}


rx_protocol_result_t opcua_bin_deinit_sec_none_transport(opcua_sec_none_protocol_type* transport)
{
	rx_deinit_packet_buffer(&transport->receive_buffer);

	return RX_PROTOCOL_OK;
}


rx_protocol_result_t opcua_parse_regular_message(struct rx_protocol_stack_endpoint* stack, opcua_sec_none_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	rx_protocol_result_t result;
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;
	size_t usable_size;
	const void* buffer_ptr;
	rx_const_packet_buffer upward_buffer;

	// check message sizes
	if (header->message_size <= opcua_regular_headers_size/*at leas one byte in payload, so <= !!!*/ || header->message_size > transport->connection_data.receive_buffer_size)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;

	// retrieve headers
	sec_header = (opcua_security_simetric_header*)rx_get_from_packet(buffer, sizeof(opcua_security_simetric_header), &result);
	if (!sec_header)
		return result;
	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_sequence_header), &result);
	if (!sequence_header)
		return result;

	// check security header
	if (sec_header->channel_id != transport->channel_id || sec_header->token_id != transport->token_id)
		return RX_PROTOCOL_PARSING_ERROR;

	// sequence header
	if (transport->received_sequence_id != invalid_sequence_id)
	{
		// TODO - handle rollover
		transport->received_sequence_id++;
		if (transport->received_sequence_id != sequence_header->sequence_number)
			return RX_PROTOCOL_INVALID_SEQUENCE;
		if (transport->current_request_id != sequence_header->request_id)
		{// new request
			if (!transport->last_chunk_received)
				return RX_PROTOCOL_COLLECT_ERROR;
			result = rx_reinit_packet_buffer(&transport->receive_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;
			transport->received_sequence_id = sequence_header->sequence_number;
			transport->current_request_id = sequence_header->request_id;
		}
	}
	else
	{
		// this is a first regular transport packet
		transport->received_sequence_id = sequence_header->sequence_number;
		transport->current_request_id = sequence_header->request_id;
		result = rx_reinit_packet_buffer(&transport->receive_buffer);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	if (header->is_final == 'A')
	{// abort the message
		transport->last_chunk_received = 1;
		result = rx_reinit_packet_buffer(&transport->receive_buffer);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	else if (header->is_final == 'F' && transport->last_chunk_received)
	{
		recv_protocol_packet pack = rx_create_recv_packet(transport->current_request_id, buffer, 0, 0);
		// one packet for all, send it directly
		result = rx_move_packet_up(stack, pack);
	}
	else
	{
		usable_size = header->message_size - opcua_regular_headers_size;
		buffer_ptr = rx_get_from_packet(buffer, usable_size, &result);
		if (!buffer_ptr)
			return result;
		result = rx_push_to_packet(&transport->receive_buffer, buffer_ptr, usable_size);
		if (result != RX_PROTOCOL_OK)
			return result;

		if (header->is_final == 'F')
		{// final chunk
			transport->last_chunk_received = 1;
			result = rx_init_const_from_packet_buffer(&upward_buffer, &transport->receive_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;//TODO //not sure if this is enough

			recv_protocol_packet pack = rx_create_recv_packet(transport->current_request_id, &upward_buffer, 0, 0);

			result = rx_move_packet_up(stack, pack);
			if (result == RX_PROTOCOL_OK)
				rx_reinit_packet_buffer(&transport->receive_buffer);
		}
		else if (header->is_final == 'C')
		{// intermediate
			transport->last_chunk_received = 0;
		}
		else
		{
			return RX_PROTOCOL_PARSING_ERROR;
		}
	}

	return result;
}

rx_protocol_result_t opcua_bin_sec_none_send_open_message(struct rx_protocol_stack_endpoint* reference, opcua_sec_none_protocol_type* transport)
{

	opcua_sequence_header* sequence_header;
	uint32_t channel_id = 0;
	uint8_t nonce[] = { 0x01 };
	uint32_t requested_lifetime = 300000;
	rx_packet_buffer request_buffer;

	rx_protocol_result_t result = RX_PROTOCOL_OK;


	// allocate the response
	result = reference->allocate_packet(reference, &request_buffer);
	if (result != RX_PROTOCOL_OK)
		return result;

	opcua_transport_header* header_ptr = (opcua_transport_header*)rx_alloc_from_packet(&request_buffer
		, sizeof(opcua_transport_header), &result);
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}
	header_ptr->is_final = 'F';
	header_ptr->message_type[0] = 'O';
	header_ptr->message_type[1] = 'P';
	header_ptr->message_type[2] = 'N';
	header_ptr->message_size = 0;

	result = rx_push_to_packet(&request_buffer, &channel_id, sizeof(channel_id));
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}

	result = opcua_write_string(&request_buffer, "http://opcfoundation.org/UA/SecurityPolicy#None");
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}

	int32_t null_len = -1;

	result = rx_push_to_packet(&request_buffer, &null_len, sizeof(null_len));
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}

	result = rx_push_to_packet(&request_buffer, &null_len, sizeof(null_len));
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}

	sequence_header = (opcua_sequence_header*)rx_alloc_from_packet(&request_buffer
		, sizeof(opcua_sequence_header), &result);
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}

	result = rx_push_to_packet(&request_buffer, &open_channel_request_node_id, sizeof(open_channel_request_node_id));
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}

	opcua_request_open_channel request_data;
	memzero(&request_data, sizeof(request_data));
	request_data.request_header.request_handle =1;
	rx_time_struct now;
	rx_os_get_system_time(&now);
	request_data.request_header.timestamp = now.t_value;
	request_data.security_mode = 1;// security mode none
	request_data.client_nonce = (uint32_t)sizeof(nonce);

	result = rx_push_to_packet(&request_buffer, &request_data, sizeof(request_data));
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}


	result = rx_push_to_packet(&request_buffer, nonce, sizeof(nonce));
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}



	result = rx_push_to_packet(&request_buffer, &requested_lifetime, sizeof(requested_lifetime));
	if (result != RX_PROTOCOL_OK)
	{
		reference->release_packet(reference, &request_buffer);
		return result;
	}

	header_ptr->message_size = (uint32_t)rx_get_packet_usable_data(&request_buffer);


	sequence_header->request_id = transport->current_request_id++;
	sequence_header->sequence_number = transport->current_sequence_id;
	// TODO - handle rollover
	transport->current_sequence_id++;

	send_protocol_packet to_send = rx_create_send_packet(sequence_header->request_id, &request_buffer, 0, 0);

	result = rx_move_packet_down(reference, to_send);


	reference->release_packet(reference, &request_buffer);

	return result;
}


rx_protocol_result_t opcua_parse_open_response_message(struct rx_protocol_stack_endpoint* reference, opcua_sec_none_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	opcua_sequence_header* sequence_header;
	const struct opcua_response_open_channel_def* presponse = NULL;
	uint32_t channel_id = 0;
	uint8_t server_nonce[0x40]; // max 40 bytes for nonce
	uint32_t revised_lifetime = 0;

	rx_protocol_result_t result = parse_asymetric_security_header(reference, transport, header, buffer, id);
	if (result != RX_PROTOCOL_OK)
		return result;

	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_sequence_header), &result);
	if (!sequence_header)
		return result;

	transport->current_request_id = sequence_header->request_id;

	const uint8_t* ext_node_id = rx_get_from_packet(buffer, sizeof(open_channel_response_node_id), &result);
	if (result != RX_PROTOCOL_OK)
		return result;

	// check nodeId for OpenSecureChannelRequest
	if (memcmp(ext_node_id, open_channel_response_node_id, sizeof(open_channel_response_node_id)) != 0)
		return RX_PROTOCOL_INVALID_SEQUENCE;

	presponse = rx_get_from_packet(buffer, sizeof(struct opcua_response_open_channel_def), &result);
	if (!presponse)
		return result;

	if (presponse->nonce_size > 0)
	{
		if ((uint32_t)presponse->nonce_size > sizeof(server_nonce) / sizeof(server_nonce[0]))
			return RX_PROTOCOL_INVALID_ARG;
		result = rx_pop_from_packet(buffer, server_nonce, (size_t)presponse->nonce_size);
		if (result != RX_PROTOCOL_OK)
			return result;
	}

	transport->channel_id = presponse->secure_channel_id;
	transport->token_id = presponse->token_id;

	rx_session session = rx_create_session(NULL, NULL, 0, 0, NULL);
	result = rx_notify_connected(reference, &session);

	return result;
}


rx_protocol_result_t opcua_bin_sec_none_bytes_received(struct rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	opcua_acknowledge_message* ack_data;
	size_t available_data;
	rx_protocol_result_t result;
	opcua_transport_header* header;
	uint32_t message_type;
	opcua_sec_none_protocol_type* transport = (opcua_sec_none_protocol_type*)reference->user_data;

	result = RX_PROTOCOL_OK;

	available_data = rx_get_packet_available_data(packet.buffer);
	// check message sizes
	if (available_data <= sizeof(opcua_transport_header))
	{
		opcua_init_sec_none_transport_state(transport);
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;
	}

	// check message type
	header = (opcua_transport_header*)rx_get_from_packet(packet.buffer, sizeof(opcua_transport_header), &result);
	if (result != RX_PROTOCOL_OK)
		return result;

//	printf("*********OPCUA Received %d bytes message type=%c%c%c%c\r\n", header->message_size,
//		header->message_type[0], header->message_type[1], header->message_type[2], header->is_final);

	message_type = (header->message_type[0] << 16) | (header->message_type[1] << 8) | (header->message_type[2]);

	// check correct message size in header
	if (header->message_size > available_data)
	{
		opcua_init_sec_none_transport_state(transport);
		return RX_PROTOCOL_PARSING_ERROR;
	}

	if (transport->server_side)
	{
		switch (message_type)
		{
		case opcua_ack_msg_type:
			{
				if (transport->current_state == opcua_transport_idle)
				{
					ack_data = (opcua_acknowledge_message*)rx_get_from_packet(packet.buffer, sizeof(opcua_acknowledge_message), &result);
					if (result == RX_PROTOCOL_OK)
					{
						transport->current_state = opcua_transport_opening;
						// Connected!!!
						transport->connection_data = *ack_data;
					}
				}
				else //if (transport->current_state == opcua_transport_opening)
				{
					transport->current_state = opcua_transport_idle;
					return RX_PROTOCOL_WRONG_STATE;
				}
			}
			break;
		case opcua_regular_msg_type:
			if (transport->current_state == opcua_transport_active)
			{
				result = opcua_parse_regular_message(reference, transport, header, packet.buffer, packet.id);
			}
			else //if (transport->current_state == opcua_transport_opening)
			{
				transport->current_state = opcua_transport_idle;
				return RX_PROTOCOL_WRONG_STATE;
			}

			break;
		case opcua_open_msg_type:
			if (transport->current_state == opcua_transport_opening)
			{
				result = opcua_parse_open_message(reference, transport, header, packet.buffer, packet.id);
				if (result == RX_PROTOCOL_OK)
				{
					transport->current_state = opcua_transport_active;
					if (transport->transport_connected)
						result = transport->transport_connected(reference, NULL, NULL);
					else
						result = RX_PROTOCOL_STACK_STRUCTURE_ERROR;
				}
			}
			else
			{
				transport->current_state = opcua_transport_idle;
				return RX_PROTOCOL_WRONG_STATE;
			}
			break;
		case opcua_close_msg_type:
			if (transport->current_state == opcua_transport_active)
			{
				result = opcua_parse_close_message(reference, transport, header, packet.buffer, packet.id);
				if (result == RX_PROTOCOL_OK)
				{
					transport->current_state = opcua_transport_closing;
					// Opened!!!
					rx_close(reference, RX_PROTOCOL_OK);
				}
			}
			else
			{
				transport->current_state = opcua_transport_idle;
				return RX_PROTOCOL_WRONG_STATE;
			}
			break;
		default:
			result = RX_PROTOCOL_PARSING_ERROR;
		}
	}
	else
	{
		switch (message_type)
		{

		case opcua_ack_msg_type:
			{
				if (transport->current_state == opcua_transport_hello_ack)
				{
					ack_data = (opcua_acknowledge_message*)rx_get_from_packet(packet.buffer, sizeof(opcua_acknowledge_message), &result);
					if (result == RX_PROTOCOL_OK)
					{
						// Connected!!!
						transport->connection_data = *ack_data;

						// send connect message
						result = opcua_bin_sec_none_send_open_message(reference, transport);

						if(result==RX_PROTOCOL_OK)
							transport->current_state = opcua_transport_opening;
					}
				}
				else //if (transport->current_state == opcua_transport_opening)
				{
					transport->current_state = opcua_transport_idle;
					return RX_PROTOCOL_WRONG_STATE;
				}
			}
			break;
		case opcua_regular_msg_type:
			result = opcua_parse_regular_message(reference, transport, header, packet.buffer, packet.id);
			break;
		case opcua_open_msg_type:
			{
				if (transport->current_state == opcua_transport_opening)
				{
					result = opcua_parse_open_response_message(reference, transport, header, packet.buffer, packet.id);

					if (result == RX_PROTOCOL_OK)
						transport->current_state = opcua_transport_active;
				}
				else
				{
					result = RX_PROTOCOL_WRONG_STATE;
				}
			}
			break;
		case opcua_close_msg_type:
			result = opcua_parse_close_message(reference, transport, header, packet.buffer, packet.id);
			break;
		default:
			result = RX_PROTOCOL_PARSING_ERROR;
		}
	}

	if (result != RX_PROTOCOL_OK)
	{
		opcua_init_sec_none_transport_state(transport);
	}

	return result;
}

rx_protocol_result_t opcua_bin_sec_none_client_connected(struct rx_protocol_stack_endpoint* reference, rx_session* session)
{
	opcua_sec_none_protocol_type* transport = (opcua_sec_none_protocol_type*)reference->user_data;
	if (transport->current_state == opcua_transport_idle)
	{
		transport->current_state = opcua_transport_hello_ack;
		return RX_PROTOCOL_OK;
	}
	else
	{
		opcua_init_sec_none_transport_state(transport);
		return RX_PROTOCOL_WRONG_STATE;
	}
}

rx_protocol_result_t opcua_bin_sec_none_bytes_send(struct rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	uint8_t* buffer_front_ptr = NULL;
	rx_protocol_result_t result;
	opcua_transport_header* header;
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;
	opcua_sec_none_protocol_type* transport = (opcua_sec_none_protocol_type*)reference->user_data;
	rx_packet_buffer my_buffer;
	size_t headers_size = sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header) + sizeof(opcua_sequence_header);


	size_t size = rx_get_packet_usable_data(packet.buffer);
	if (size + headers_size <= transport->connection_data.send_buffer_size)
	{// one packet, just handle header stuff
		buffer_front_ptr = (uint8_t*)rx_alloc_from_packet_front(packet.buffer
			, headers_size, &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		header = (opcua_transport_header*)buffer_front_ptr;
		sec_header = (opcua_security_simetric_header*)&buffer_front_ptr[sizeof(opcua_transport_header)];
		sequence_header = (opcua_sequence_header*)&buffer_front_ptr[sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header)];
		// header
		header->is_final = 'F';
		header->message_type[0] = 'M';
		header->message_type[1] = 'S';
		header->message_type[2] = 'G';
		header->message_size = (uint32_t)rx_get_packet_usable_data(packet.buffer);
		// security header
		sec_header->channel_id = transport->channel_id;
		sec_header->token_id = transport->token_id;
		// sequence header
		sequence_header->request_id = packet.id;
		sequence_header->sequence_number = transport->current_sequence_id;
		// TODO - handle rollover
		transport->current_sequence_id++;

		packet.id = sequence_header->request_id;

		result = rx_move_packet_down(reference, packet);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	else
	{
		int last = 0;
		size_t idx = 0;
		size_t packet_size = 0;
		size_t max_data = transport->connection_data.send_buffer_size - headers_size;
		send_protocol_packet splited_packet = packet;
		while (idx < size)
		{
			result = reference->allocate_packet(reference, &my_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;
			if (size - idx > max_data)
			{
				packet_size = max_data;
			}
			else
			{
				packet_size = size - idx;
				last = 1;
			}
			result = rx_push_to_packet(&my_buffer, packet.buffer->buffer_ptr + idx, packet_size);
			if (result != RX_PROTOCOL_OK)
				return result;
			idx += packet_size;

			buffer_front_ptr = (uint8_t*)rx_alloc_from_packet_front(&my_buffer
				, headers_size, &result);
			if (result != RX_PROTOCOL_OK)
				return result;
			header = (opcua_transport_header*)buffer_front_ptr;
			sec_header = (opcua_security_simetric_header*)&buffer_front_ptr[sizeof(opcua_transport_header)];
			sequence_header = (opcua_sequence_header*)&buffer_front_ptr[sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header)];
			// header
			header->is_final = last != 0 ? 'F' : 'C';
			header->message_type[0] = 'M';
			header->message_type[1] = 'S';
			header->message_type[2] = 'G';
			header->message_size = (uint32_t)rx_get_packet_usable_data(&my_buffer);
			// security header
			sec_header->channel_id = 0;
			sec_header->token_id = 0;
			// sequence header
			sequence_header->request_id = packet.id;
			sequence_header->sequence_number = transport->current_sequence_id;
			// TODO - handle rollover
			transport->current_sequence_id++;

			packet.id = sequence_header->request_id;

			splited_packet.buffer = &my_buffer;
			result = rx_move_packet_down(reference, splited_packet);

			reference->release_packet(reference, &my_buffer);

			if (result != RX_PROTOCOL_OK)
				return result;
		}
	}

	return RX_PROTOCOL_OK;
}



