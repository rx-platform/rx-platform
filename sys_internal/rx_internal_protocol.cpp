

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.cpp
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


// rx_internal_protocol
#include "sys_internal/rx_internal_protocol.h"

#include "lib/rx_ser_json.h"
#include "system/runtime/rx_blocks.h"
#include "sys_internal/rx_async_functions.h"
#include "lib/rx_io_buffers.h"
#include "rx_internal_subscription.h"
#include "rx_subscription_items.h"
#include "model/rx_meta_internals.h"


namespace rx_internal {

namespace rx_protocol {

// serializing messages
rx_result serialize_message(base_meta_writer& stream, int requestId, messages::rx_message_base& what)
{
	if (!stream.write_header(STREAMING_TYPE_MESSAGE, 0))
		return stream.get_error();

	if (!stream.start_object("header"))
		return stream.get_error();
	if (!stream.write_int("requestId", requestId))
		return stream.get_error();
	if (!stream.write_string("msgType", what.get_type_name().c_str()))
		return stream.get_error();
	if (!stream.end_object())
		return stream.get_error();

	if (!stream.start_object("body"))
		return stream.get_error();
	auto ret = what.serialize(stream);
	if(!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();

	if (!stream.write_footer())
		return stream.get_error();


	return true;
}

// Class rx_internal::rx_protocol::rx_json_protocol_port 

rx_json_protocol_port::rx_json_protocol_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<rx_server_connection>(this);
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
	std::ostringstream ss;
	ss << "Protocol subscription "
		<< data_.subscription_id.to_string()
		<< " created.";
	RUNTIME_LOG_TRACE("", 100, ss.str());
}


rx_protocol_subscription::~rx_protocol_subscription()
{
	destroy();
	std::ostringstream ss;
	ss << "Protocol subscription "
		<< data_.subscription_id.to_string()
		<< " destroyed.";
	RUNTIME_LOG_TRACE("", 100, ss.str());
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

void rx_protocol_subscription::execute_completed (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, data::runtime_values_data data)
{
	if (connection_)
	{
		auto notify_msg = std::make_unique<messages::subscription_messages::subscription_execute_done>();
		notify_msg->request_id = 0;
		notify_msg->subscription_id = data_.subscription_id;
		notify_msg->transaction_id = transaction_id;
		if (result)
		{
			notify_msg->result.first = 0;
			notify_msg->result.second = "";
		}
		else
		{
			notify_msg->result.first = 119;
			notify_msg->result.second = result.errors_line();
		}

		notify_msg->data = std::move(data);
		connection_->data_processed(std::move(notify_msg));
	}
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
		connection_ = rx_server_connection::smart_ptr::null_ptr;
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

rx_result rx_protocol_subscription::execute_item (runtime_transaction_id_t transaction_id, runtime_handle_t handle, data::runtime_values_data data)
{
	auto result = my_subscription_->execute_item(transaction_id, handle, std::move(data));
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


// Class rx_internal::rx_protocol::rx_server_connection 

rx_server_connection::rx_server_connection (runtime::items::port_runtime* port)
      : executer_(-1),
        port_(port),
        stream_version_(RX_CURRENT_SERIALIZE_VERSION)
{
	RXCOMM_LOG_DEBUG("rx_server_connection", 200, "{rx-platform} communication server endpoint created.");
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &rx_server_connection::received_function;
	
	executer_ = port->get_executer();
}


rx_server_connection::~rx_server_connection()
{
	RXCOMM_LOG_DEBUG("rx_server_connection", 200, "{rx-platform} communication server endpoint destroyed.");
}



rx_protocol_stack_endpoint* rx_server_connection::bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	model::platform_types_manager::instance().get_types_resolver().register_subscriber(smart_this(), [this](model::resolver_event_data data)
		{
			auto msg = std::make_unique<messages::rx_connection_notify_message>();
			msg->changed_id = data.id;
			msg->changed_path = data.path;
			data_processed(std::move(msg));
		});
	return &stack_entry_;
}

rx_protocol_result_t rx_server_connection::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_server_connection* self = reinterpret_cast<rx_server_connection*>(reference->user_data);

	return self->received(packet);
}

rx_protocol_result_t rx_server_connection::received (recv_protocol_packet packet)
{
	io::rx_const_io_buffer received(packet.buffer);
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
		uint32_t temp_version = stream_version_ == 0 ? RX_CURRENT_SERIALIZE_VERSION : stream_version_;

		messages::rx_request_id_t request_id = 0;
		// read the header first
		serialization::json_reader reader(temp_version);
		if (!reader.parse_data(json))
			return RX_PROTOCOL_PARSING_ERROR;
		auto request = messages::rx_request_message::create_request_from_stream(request_id, reader);
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

			send_message(std::move(result_msg));
		}
	}
	return RX_PROTOCOL_OK;
}

void rx_server_connection::close_endpoint ()
{
	close_connection();
}

message_ptr rx_server_connection::set_context (api::rx_context ctx, const messages::rx_connection_context_request& req)
{
	auto base_result = rx_protocol_connection::set_context(ctx, req);
	if (base_result)
	{
		return base_result;
	}
	auto request_id = req.request_id;
	if (req.stream_version)
	{
		auto result = request_stream_version(req.stream_version);
		if (!result)
		{
			auto ret_value = std::make_unique<messages::error_message>(result, 13, request_id);
			return ret_value;
		}
	}
	if (!req.directory.empty())
	{
		auto result = set_current_directory(req.directory);
		if (!result)
		{
			auto ret_value = std::make_unique<messages::error_message>(result, 13, request_id);
			return ret_value;
		}
	}
	auto response = std::make_unique<messages::rx_connection_context_response>();
	response->directory = get_current_directory_path();
	response->stream_version = get_stream_version();
	return response;
}

rx_result rx_server_connection::request_stream_version (uint32_t sversion)
{
	if (sversion == 0)
		return RX_INVALID_ARGUMENT;
	stream_version_ = std::min<uint32_t>(sversion, RX_CURRENT_SERIALIZE_VERSION);
	return true;
}

void rx_server_connection::send_message (message_ptr msg)
{
	uint32_t temp_version = RX_CURRENT_SERIALIZE_VERSION;
	if (stream_version_)
		temp_version = stream_version_;
	serialization::json_writer writter(temp_version);
	auto result = serialize_message(writter, msg->request_id, *msg);
	if (result)
	{
		string_type ret_data;
		ret_data = writter.get_string();
		auto buff_result = port_->alloc_io_buffer();
		if(buff_result)
		{
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint16_t)0x7fff);
			result = buff_result.value().write_string(ret_data);
			send_protocol_packet packet = rx_create_send_packet(msg->request_id, &buff_result.value(), 0, 0);

			auto protocol_res = rx_move_packet_down(&stack_entry_, packet);
			if (protocol_res != RX_PROTOCOL_OK)
			{
				std::cout << "Error returned from move_down:"
					<< rx_protocol_error_message(protocol_res)
					<< "\r\n";
			}
			port_->release_io_buffer(buff_result.move_value());
		}
	}
}


// Class rx_internal::rx_protocol::rx_protocol_connection 

rx_protocol_connection::rx_protocol_connection()
      : current_directory_path_("/world")
{
	current_directory_ = rx_gate::instance().get_directory("/world");
}


rx_protocol_connection::~rx_protocol_connection()
{
}



void rx_protocol_connection::data_processed (message_ptr result)
{
	if (current_directory_) // check if we are closed...
	{
		send_message(std::move(result));
	}
}

void rx_protocol_connection::request_received (request_message_ptr&& request)
{
	if (current_directory_)
	{
		api::rx_context ctx;
		ctx.active_path = current_directory_->meta_info().get_full_path();
		ctx.object = smart_this();
		message_ptr result_msg;
		/*if (stream_version_ == 0 && request->get_type_id() != messages::rx_connection_context_request_id)
		{
			result_msg = std::make_unique<messages::error_message>("No connection context."s, 99, request->request_id);
		}
		else*/
		{
			result_msg = request->do_job(ctx, smart_this());
		}
		if (result_msg)
		{
			send_message(std::move(result_msg));
		}
	}
}

rx_result rx_protocol_connection::set_current_directory (const string_type& path)
{
	auto temp = rx_gate::instance().get_directory(path);
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

rx_result rx_protocol_connection::execute_item (const rx_uuid& id, runtime_transaction_id_t transaction_id, runtime_handle_t item, data::runtime_values_data data)
{
	auto it = subscriptions_.find(id);
	if (it != subscriptions_.end())
	{
		return it->second->execute_item(transaction_id, item, std::move(data));
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

void rx_protocol_connection::close_connection ()
{
	current_directory_ = rx_directory_ptr::null_ptr;
	for (auto& one : subscriptions_)
		one.second->destroy();
	subscriptions_.clear();
}

message_ptr rx_protocol_connection::set_context (api::rx_context ctx, const messages::rx_connection_context_request& req)
{
	auto request_id = req.request_id;
	if (!req.directory.empty())
	{
		auto result = set_current_directory(req.directory);
		if (!result)
		{
			auto ret_value = std::make_unique<messages::error_message>(result, 13, request_id);
			return ret_value;
		}
	}
	return message_ptr();
}


// Class rx_internal::rx_protocol::rx_client_connection 

rx_client_connection::rx_client_connection()
{
}


rx_client_connection::~rx_client_connection()
{
}



// Class rx_internal::rx_protocol::rx_local_subscription 


} // namespace rx_protocol
} // namespace rx_internal

