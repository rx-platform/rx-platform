

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.cpp
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


#include "pch.h"


// rx_internal_protocol
#include "sys_internal/rx_internal_protocol.h"

#include "system/server/rx_server.h"
#include "system/runtime/rx_io_buffers.h"


namespace sys_internal {

namespace rx_protocol {
// serializing messages
rx_result serialize_message(base_meta_writer& stream, int requestId, messages::rx_message_base& what)
{
	if (!stream.write_header(STREAMING_TYPE_MESSAGE, 0))
		return false;

	if (!stream.start_object("header"))
		return false;
	if (!stream.write_int("requestId", requestId))
		return false;
	if (!stream.write_string("msgType", what.get_type_name()))
		return false;
	if (!stream.end_object())
		return false;

	if (!stream.start_object("body"))
		return false;
	if (!what.serialize(stream))
		return false;
	if (!stream.end_object())
		return false;

	if (!stream.write_footer())
		return false;


	return true;
}

// Class sys_internal::rx_protocol::rx_protocol_port 

rx_protocol_port::rx_protocol_port()
      : current_directory_path_("/world")
{
	stack_entry_.my_port_ = smart_this();
	//current_directory_ = rx_gate::instance().get_root_directory()->get_sub_directory("world");
}



void rx_protocol_port::data_received (const string_type& data)
{
	auto received=messages::rx_request_message::create_request_from_json(data);
	if (received)
	{
		api::rx_context ctx;
		ctx.directory = current_directory_;
		ctx.object = smart_this();
		auto result_msg =received.value()->do_job(ctx, smart_this());
		if (result_msg)
		{
			serialization::json_writer writter;
			auto result = serialize_message(writter, received.value()->requestId, *result_msg);
			if (result)
			{
				auto buff_result = allocate_io_buffer();
				buff_result.value().write_to_buffer((uint8_t)1);
				string_type ret_data;
				writter.get_string(ret_data, true);
				auto ret = buff_result.value().write_string(ret_data);

				auto protocol_res = rx_move_packet_down(get_stack_entry(), nullptr, &buff_result.value());
				if (protocol_res != RX_PROTOCOL_OK)
				{
					std::cout << "Error returned from move_down:"
						<< rx_protocol_error_message(protocol_res)
						<< "\r\n";
				}
				else
					buff_result.value().detach(nullptr);
			}
		}
	}
}

rx_protocol_stack_entry* rx_protocol_port::get_stack_entry ()
{
	return &stack_entry_;
}

void rx_protocol_port::data_processed (message_ptr result)
{
	serialization::json_writer writter;
	auto res = serialize_message(writter, result->requestId, *result);
	if (res)
	{
		auto buff_result = allocate_io_buffer();
		buff_result.value().write_to_buffer((uint8_t)1);
		string_type ret_data;
		writter.get_string(ret_data, true);
		auto ret = buff_result.value().write_string(ret_data);

		auto protocol_res = rx_move_packet_down(get_stack_entry(), nullptr, &buff_result.value());
		if (protocol_res != RX_PROTOCOL_OK)
		{
			std::cout << "Error returned from move_down:"
				<< rx_protocol_error_message(protocol_res)
				<< "\r\n";
		}
		else
			buff_result.value().detach(nullptr);
	}
}

rx_result rx_protocol_port::set_current_directory (const string_type& path)
{
	auto temp = rx_gate::instance().get_root_directory()->get_sub_directory(path);
	if (!temp)
		return "Directory " + path + " not exists!";
	else
		return true;
}


// Class sys_internal::rx_protocol::rx_json_protocol 

rx_json_protocol::rx_json_protocol()
{
	rx_protocol_stack_entry* mine_entry = this;

	mine_entry->downward = nullptr;
	mine_entry->upward = nullptr;

	mine_entry->send_function = nullptr;
	mine_entry->sent_function = nullptr;
	mine_entry->received_function = &rx_json_protocol::received_function;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;

	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;
}



rx_protocol_result_t rx_json_protocol::received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	rx_json_protocol* self = reinterpret_cast<rx_json_protocol*>(reference);

	runtime::io_types::rx_const_io_buffer received(buffer);

	uint8_t type;
	auto result = received.read_from_buffer(type);
	if (result)
	{
		switch (type)
		{
		case 1:
		{
			string_type json;
			result = received.read_string(json);
			if (result)
			{
				if (self->my_port_)
				{
					rx_post_function<decltype(my_port_)>([json](decltype(my_port_) whose) {
							whose->data_received(json);
						}
						, self->my_port_
						, self->my_port_->get_executer());
				}
			}
		}
		break;
		default:
			return RX_PROTOCOL_INVALID_SEQUENCE;
		}
		
	}
	return RX_PROTOCOL_OK;
}

rx_result rx_json_protocol::send_string (const string_type& what)
{
	return true;
}


} // namespace rx_protocol
} // namespace sys_internal

