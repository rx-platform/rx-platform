

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

// Class rx_internal::rx_protocol::rx_protocol_port 

rx_protocol_port::rx_protocol_port()
{
}



void rx_protocol_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}

rx_protocol_stack_endpoint* rx_protocol_port::create_endpoint ()
{
	auto ep = rx_create_reference<rx_protocol_connection>();
	auto ret = ep->bind_endpoint([](int64_t) {}, [](int64_t) {});
	active_endpoints_.emplace(ret, std::move(ep));
	return ret;
}

void rx_protocol_port::remove_endpoint (rx_protocol_stack_endpoint* what)
{
	auto it = active_endpoints_.find(what);
	if (it != active_endpoints_.end())
		active_endpoints_.erase(it);
}


// Class rx_internal::rx_protocol::rx_json_endpoint 

rx_json_endpoint::rx_json_endpoint()
{
	rx_init_stack_entry(&stack_entry_, this);

	stack_entry_.received_function = &rx_json_endpoint::received_function;
}



rx_protocol_result_t rx_json_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_json_endpoint* self = reinterpret_cast<rx_json_endpoint*>(reference->user_data);

	runtime::io_types::rx_const_io_buffer received(packet.buffer);

	if(self->received_func_)
		self->received_func_((int64_t)rx_get_packet_available_data(packet.buffer));

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
				if (self->connection_)
				{
					rx_post_function_to(self->connection_->get_executer(), self->connection_, 
						[](decltype(connection_) whose, string_type&& json, rx_packet_id_type packet_id) {
							whose->data_received(json, packet_id);
						}, self->connection_, std::move(json), packet.id);
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

rx_result rx_json_endpoint::send_string (const string_type& what)
{
	return true;
}

void rx_json_endpoint::bind (rx_protocol_connection_ptr conn, std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	connection_ = conn;
	received_func_ = received_func;
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
	return result;
}


// Class rx_internal::rx_protocol::subscription_data 


// Class rx_internal::rx_protocol::subscription_item_data 


// Class rx_internal::rx_protocol::rx_protocol_connection 

rx_protocol_connection::rx_protocol_connection()
      : current_directory_path_("/world"),
        executer_(-1)
{
	current_directory_ = rx_gate::instance().get_root_directory()->get_sub_directory("world");
	executer_ = rx_thread_context();
}



void rx_protocol_connection::data_received (const string_type& data, rx_packet_id_type packet_id)
{

	messages::rx_request_id_t request_id = 0;
	auto received = messages::rx_request_message::create_request_from_json(data, request_id);
	if (received)
	{
		api::rx_context ctx;
		ctx.directory = current_directory_;
		ctx.object = smart_this();
		auto result_msg = received.value()->do_job(ctx, smart_this());
		if (result_msg)
		{
			serialization::json_writer writter;
			auto result = serialize_message(writter, received.value()->request_id, *result_msg);
			if (result)
			{
				auto buff_result = allocate_io_buffer(&endpoint_.stack_entry_);
				result = buff_result.value().write_to_buffer((uint8_t)1);
				string_type ret_data;
				writter.get_string(ret_data, true);
				result = buff_result.value().write_string(ret_data);

				send_protocol_packet packet = rx_create_send_packet(packet_id, &buff_result.value(), 0, 0);

				auto protocol_res = rx_move_packet_down(&endpoint_.stack_entry_, packet);
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
			auto buff_result = allocate_io_buffer(&endpoint_.stack_entry_);
			buff_result.value().write_to_buffer((uint8_t)1);
			string_type ret_data;
			writter.get_string(ret_data, true);
			auto ret = buff_result.value().write_string(ret_data);

			send_protocol_packet packet = rx_create_send_packet(packet_id, &buff_result.value(), 0, 0);

			auto protocol_res = rx_move_packet_down(&endpoint_.stack_entry_, packet);
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
	serialization::json_writer writter;
	auto res = serialize_message(writter, result->request_id, *result);
	if (res)
	{
		auto buff_result = allocate_io_buffer(&endpoint_.stack_entry_);
		buff_result.value().write_to_buffer((uint8_t)1);
		string_type ret_data;
		writter.get_string(ret_data, true);
		auto ret = buff_result.value().write_string(ret_data);

		send_protocol_packet packet = rx_create_send_packet(0, &buff_result.value(), 0, 0);

		auto protocol_res = rx_move_packet_down(&endpoint_.stack_entry_, packet);
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
	endpoint_.bind(smart_this(), sent_func, received_func);
	return &endpoint_.stack_entry_;
}


} // namespace rx_protocol
} // namespace rx_internal



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	return 


#endif
