

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

#include "sys_internal/rx_async_functions.h"
#include "system/runtime/rx_io_buffers.h"
#include "rx_internal_subscription.h"
#include "rx_subscription_items.h"


namespace rx_internal {

namespace rx_protocol {
rx_result_with<runtime::io_types::rx_io_buffer> allocate_io_buffer(rx_protocol_stack_endpoint* entry, size_t initial_capacity = 0)
{
	runtime::io_types::rx_io_buffer ret;
	auto result = rx_init_packet_buffer(&ret, initial_capacity, entry);
	if (result == RX_PROTOCOL_OK)
		return ret;
	else
		return rx_protocol_error_message(result);
}

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

// Class rx_internal::rx_protocol::rx_json_protocol_port 

rx_json_protocol_port::rx_json_protocol_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<rx_protocol_connection>(this);
		auto entry = rt->bind_endpoint([this](int64_t count)
			{
			},
			[this](int64_t count)
			{
			});
		return construct_func_type::result_type{ entry, rt };
	};
}



void rx_json_protocol_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}


// Class rx_internal::rx_protocol::rx_protocol_subscription 

rx_protocol_subscription::rx_protocol_subscription (subscription_data& data, rx_protocol_connection_ptr conn)
      : data_(data)
	, connection_(conn)
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
	if (!items.empty() && connection_)
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
		connection_->data_processed(std::move(notify_msg));
	}
}

void rx_protocol_subscription::transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items)
{
}

void rx_protocol_subscription::write_completed (runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_result> > results)
{
	if (!results.empty() && connection_)
	{
		auto notify_msg = std::make_unique<messages::subscription_messages::subscription_write_done>();
		notify_msg->request_id = 0;
		notify_msg->subscription_id = data_.subscription_id;
		notify_msg->transaction_id = transaction_id;
		notify_msg->results.reserve(results.size());
		for (auto&& one : results)
		{
			auto it = items_.find(one.first);
			if (it != items_.end())
			{
				notify_msg->add_result(it->second.client_handle, std::move(one.second));
			}
		}
		connection_->data_processed(std::move(notify_msg));
	}
}

void rx_protocol_subscription::destroy ()
{
	if (my_subscription_)
	{
		if (data_.active)
			my_subscription_->deactivate();
		my_subscription_ = sys_runtime::subscriptions::rx_subscription::smart_ptr::null_ptr;
	}
	if (connection_)
		connection_ = rx_protocol_connection::smart_ptr::null_ptr;
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
	auto result = my_subscription_->write_items(transaction_id, std::move(values), results);
	return result;
}

rx_result rx_protocol_subscription::remove_items (std::vector<runtime_handle_t >&& items, std::vector<rx_result>& results)
{
	auto result = my_subscription_->disconnect_items(items, results);
	if (result)
	{
		size_t count = items.size();
		for (size_t i = 0; i < count; i++)
		{
			items_.erase(items[i]);
			auto it = handles_.find(items[i]);
			if (it != handles_.end())
			{
				items_.erase(it->second);
				handles_.erase(it);
			}
		}
	}
	return result;
}


// Class rx_internal::rx_protocol::subscription_data 


// Class rx_internal::rx_protocol::subscription_item_data 


// Class rx_internal::rx_protocol::rx_protocol_connection 

rx_protocol_connection::rx_protocol_connection (runtime::items::port_runtime* port)
      : current_directory_path_("/world"),
        executer_(-1),
        port_(port)
{
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &rx_protocol_connection::received_function;

	current_directory_ = rx_gate::instance().get_root_directory()->get_sub_directory("world");
	executer_ = port->get_executer();
}


rx_protocol_connection::~rx_protocol_connection()
{
}



void rx_protocol_connection::request_received (request_message_ptr&& request)
{
	api::rx_context ctx;
	ctx.directory = current_directory_;
	ctx.object = smart_this();
	auto result_msg = request->do_job(ctx, smart_this());
	if (result_msg)
	{
		serialization::json_writer writter;
		auto result = serialize_message(writter, request->request_id, *result_msg);
		if (result)
		{
			auto buff_result = allocate_io_buffer(&stack_entry_);
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint16_t)0x7fff);
			string_type ret_data;
			writter.get_string(ret_data, true);
			result = buff_result.value().write_string(ret_data);

			send_protocol_packet packet = rx_create_send_packet(request->request_id, &buff_result.value(), 0, 0);

			auto protocol_res = rx_move_packet_down(&stack_entry_, packet);
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

void rx_protocol_connection::data_processed (message_ptr result)
{
	if (current_directory_) // check if we are closed...
	{
		serialization::json_writer writter;
		auto res = serialize_message(writter, result->request_id, *result);
		if (res)
		{
			auto buff_result = allocate_io_buffer(&stack_entry_);
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint16_t)0x7fff);
			string_type ret_data;
			writter.get_string(ret_data, true);
			auto ret = buff_result.value().write_string(ret_data);

			send_protocol_packet packet = rx_create_send_packet(0, &buff_result.value(), 0, 0);

			auto protocol_res = rx_move_packet_down(&stack_entry_, packet);
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

rx_result rx_protocol_connection::set_current_directory (const string_type& path)
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

rx_result rx_protocol_connection::connect_subscription (subscription_data& data)
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

rx_result rx_protocol_connection::delete_subscription (const rx_uuid& id)
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

rx_result rx_protocol_connection::update_subscription (subscription_data& data)
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

rx_result rx_protocol_connection::add_items (const rx_uuid& id, const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results)
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

rx_result rx_protocol_connection::write_items (const rx_uuid& id, runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results)
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

rx_result rx_protocol_connection::remove_items (const rx_uuid& id, std::vector<runtime_handle_t>&& items, std::vector<rx_result>& results)
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

rx_protocol_stack_endpoint* rx_protocol_connection::bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	return &stack_entry_;
}

rx_protocol_result_t rx_protocol_connection::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_protocol_connection* self = reinterpret_cast<rx_protocol_connection*>(reference->user_data);

	return self->received(packet);
}

rx_protocol_result_t rx_protocol_connection::received (recv_protocol_packet packet)
{
	runtime::io_types::rx_const_io_buffer received(packet.buffer);
	uint8_t type;
	uint8_t namespace_id;
	uint16_t num_id;
	auto result = received.read_from_buffer(type);
	if (!result)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;
	if (type != 0x1)// has to be string value
		return RX_PROTOCOL_PARSING_ERROR;
	result = received.read_from_buffer(namespace_id);
	if (!result)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;
	result = received.read_from_buffer(num_id);
	if (!result)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;
	if (num_id != 0x7fff)// has to be exact value
		return RX_PROTOCOL_PARSING_ERROR;
	
	string_type json;
	result = received.read_string(json);
	if (result)
	{
		messages::rx_request_id_t request_id = 0;
		auto request = messages::rx_request_message::create_request_from_json(json, request_id);
		if (request)
		{
			rx_post_function_to(get_executer(), smart_this(),
				[](smart_ptr whose, request_message_ptr&& request) {
					whose->request_received(std::move(request));
				}, smart_this(), request.move_value());
		}
		else
		{
			auto result_msg = std::make_unique<messages::error_message>(std::move(request), 21, request_id);

			serialization::json_writer writter;
			auto result = serialize_message(writter, request_id, *result_msg);
			if (result)
			{
				auto buff_result = allocate_io_buffer(&stack_entry_);
				buff_result.value().write_to_buffer((uint8_t)1);
				buff_result.value().write_to_buffer((uint8_t)1);
				buff_result.value().write_to_buffer((uint16_t)0x7fff);
				string_type ret_data;
				writter.get_string(ret_data, true);
				auto ret = buff_result.value().write_string(ret_data);

				send_protocol_packet packet = rx_create_send_packet(request_id, &buff_result.value(), 0, 0);

				auto protocol_res = rx_move_packet_down(&stack_entry_, packet);
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
	return RX_PROTOCOL_OK;
}

void rx_protocol_connection::close_endpoint ()
{
	current_directory_ = rx_directory_ptr::null_ptr;
}


} // namespace rx_protocol
} // namespace rx_internal

