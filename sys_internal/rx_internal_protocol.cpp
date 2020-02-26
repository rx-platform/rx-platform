

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.cpp
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


// rx_internal_protocol
#include "sys_internal/rx_internal_protocol.h"

#include "system/server/rx_async_functions.h"
#include "system/runtime/rx_io_buffers.h"
#include "rx_internal_subscription.h"
#include "rx_subscription_items.h"


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
	current_directory_ = rx_gate::instance().get_root_directory()->get_sub_directory("world");
}



void rx_protocol_port::data_received (const string_type& data)
{
	messages::rx_request_id_t request_id = 0;
	auto received=messages::rx_request_message::create_request_from_json(data, request_id);
	if (received)
	{
		api::rx_context ctx;
		ctx.directory = current_directory_;
		ctx.object = smart_this();
		auto result_msg =received.value()->do_job(ctx, smart_this());
		if (result_msg)
		{
			serialization::json_writer writter;
			auto result = serialize_message(writter, received.value()->request_id, *result_msg);
			if (result)
			{
				auto buff_result = allocate_io_buffer();
				result = buff_result.value().write_to_buffer((uint8_t)1);
				string_type ret_data;
				writter.get_string(ret_data, true);
				result = buff_result.value().write_string(ret_data);

				auto protocol_res = rx_move_packet_down(&stack_entry_, nullptr, &buff_result.value());
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
	else
	{
		auto result_msg = std::make_unique<messages::error_message>(std::move(received), 21, request_id);

		serialization::json_writer writter;
		auto result = serialize_message(writter, request_id, *result_msg);
		if (result)
		{
			auto buff_result = allocate_io_buffer();
			buff_result.value().write_to_buffer((uint8_t)1);
			string_type ret_data;
			writter.get_string(ret_data, true);
			auto ret = buff_result.value().write_string(ret_data);

			auto protocol_res = rx_move_packet_down(&stack_entry_, nullptr, &buff_result.value());
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

rx_protocol_stack_entry* rx_protocol_port::create_stack_entry ()
{
	return &stack_entry_;
}

void rx_protocol_port::data_processed (message_ptr result)
{
	serialization::json_writer writter;
	auto res = serialize_message(writter, result->request_id, *result);
	if (res)
	{
		auto buff_result = allocate_io_buffer();
		buff_result.value().write_to_buffer((uint8_t)1);
		string_type ret_data;
		writter.get_string(ret_data, true);
		auto ret = buff_result.value().write_string(ret_data);

		auto protocol_res = rx_move_packet_down(&stack_entry_, nullptr, &buff_result.value());
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
	{
		return "Directory " + path + " not exists!";
	}
	else
	{
		current_directory_ = temp;
		return true;
	}
}

rx_result rx_protocol_port::connect_subscription (subscription_data& data)
{
	if (data.subscription_id.is_null())
	{
		data.subscription_id = rx_uuid::create_new();
		auto temp = std::make_unique<rx_protocol_subscription>(data, smart_this());
		subscriptions_.emplace(data.subscription_id, std::move(temp));
		return true;
	}
	else
	{
		return "Subscription reuse not implemented yet!";
	}
}

rx_result rx_protocol_port::delete_subscription (const rx_uuid& id)
{
	auto it = subscriptions_.find(id);
	if (it != subscriptions_.end())
	{
		it->second->destroy();
		subscriptions_.erase(it);
		return true;
	}
	else
	{
		return "Invalid subscription Id";
	}
}

rx_result rx_protocol_port::update_subscription (subscription_data& data)
{
	auto it = subscriptions_.find(data.subscription_id);
	if (it != subscriptions_.end())
	{
		return it->second->update_subscription(data);
	}
	else
	{
		return "Invalid subscription Id";
	}
}

rx_result rx_protocol_port::add_items (const rx_uuid& id, const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results)
{
	auto it = subscriptions_.find(id);
	if (it != subscriptions_.end())
	{
		return it->second->add_items(items, results);
	}
	else
	{
		return "Invalid subscription Id";
	}
}

rx_result rx_protocol_port::write_items (const rx_uuid& id, runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results)
{
	auto it = subscriptions_.find(id);
	if (it != subscriptions_.end())
	{
		return it->second->write_items(transaction_id, std::move(values), results);
	}
	else
	{
		return "Invalid subscription Id";
	}
}

rx_result rx_protocol_port::remove_items (const rx_uuid& id, std::vector<runtime_handle_t>&& items, std::vector<rx_result>& results)
{
	auto it = subscriptions_.find(id);
	if (it != subscriptions_.end())
	{
		return it->second->remove_items(std::move(items), results);
	}
	else
	{
		return "Invalid subscription Id";
	}
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

	mine_entry->connected_function = nullptr;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;

	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;
}



rx_protocol_result_t rx_json_protocol::received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	rx_json_protocol* self = reinterpret_cast<rx_json_protocol*>(reference);

	runtime::io_types::rx_const_io_buffer received(buffer);


	self->my_port_->update_received_counters((int64_t)rx_get_packet_available_data(buffer));

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


// Class sys_internal::rx_protocol::rx_protocol_subscription 

rx_protocol_subscription::rx_protocol_subscription (subscription_data& data, rx_reference<rx_protocol_port> port)
      : data_(data),
        my_port_(port)
{
	my_subscription_ = rx_create_reference<sys_runtime::subscriptions::rx_subscription>(this);
	if (data.active)
		my_subscription_->activate();
}


rx_protocol_subscription::~rx_protocol_subscription()
{
	destroy();
}



rx_result rx_protocol_subscription::update_subscription (subscription_data& data)
{
	RX_ASSERT(data.subscription_id == data_.subscription_id);
	if(data.active && !data_.active)
		my_subscription_->activate();
	else if(!data.active && data_.active)
		my_subscription_->deactivate();
	data_ = data;
	return true;
}

void rx_protocol_subscription::items_changed (const std::vector<update_item>& items)
{
	if (!items.empty() && my_port_)
	{
		auto notify_msg = std::make_unique<messages::subscription_messages::subscription_items_change>();
		notify_msg->request_id = 0;
		notify_msg->subscription_id = data_.subscription_id;
		notify_msg->items.reserve(items.size());
		for (const auto& one : items)
		{
			auto it = items_.find(one.handle);
			if(it!=items_.end())
			{
				notify_msg->items.emplace_back(update_item{ it->second.client_handle, one.value });
			}
		}
		my_port_->data_processed(std::move(notify_msg));
	}
}

void rx_protocol_subscription::transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items)
{
}

void rx_protocol_subscription::destroy ()
{
	if (my_subscription_)
	{
		if (data_.active)
			my_subscription_->deactivate();
		my_subscription_ = sys_runtime::subscriptions::rx_subscription::smart_ptr::null_ptr;
	}
	if (my_port_)
		my_port_ = rx_protocol_port::smart_ptr::null_ptr;
}

rx_result rx_protocol_subscription::add_items (const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results)
{
	string_array paths;
	for (const auto& one : items)
	{
		paths.emplace_back(one.path);
	}
	auto result = my_subscription_->connect_items(paths, results);
	if (!result)
		return result;
	size_t idx = 0;
	for (const auto& one : results)
	{
		subscription_item_data temp;
		temp.local_handle = one.value();
		temp.client_handle = items[idx].client_handle;
		temp.path = items[idx].path;
		temp.active = items[idx].active;
		temp.trigger_type = items[idx].trigger_type;
		items_.emplace(one.value(), std::move(temp));
		handles_.emplace(items[idx].client_handle, one.value());
		idx++;
	}
	return true;
}

rx_result rx_protocol_subscription::write_items (runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results)
{
	/*for (auto& one : values)
	{
		auto it = handles_.find(one.first);
		if (it != handles_.end())
			one.first = it->second;
		else
			one.first = 0;
	}*/
	auto result = my_subscription_->write_items(transaction_id, std::move(values), results);
	return result;
}

rx_result rx_protocol_subscription::remove_items (std::vector<runtime_handle_t >&& items, std::vector<rx_result>& results)
{
	auto result = my_subscription_->disconnect_items(items, results);
	return result;
}


// Class sys_internal::rx_protocol::subscription_data 


// Class sys_internal::rx_protocol::subscription_item_data 


} // namespace rx_protocol
} // namespace sys_internal

