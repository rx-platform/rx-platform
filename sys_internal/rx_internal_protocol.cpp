

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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

// Class rx_internal::rx_protocol::rx_client_connection 

rx_client_connection::rx_client_connection (rx_json_protocol_client_port* port)
      : executer_(-1),
        port_(port),
        stream_version_(RX_CURRENT_SERIALIZE_VERSION),
        last_sent_(0)
{
	RXCOMM_LOG_DEBUG("rx_client_connection", 200, "{rx-platform} communication client endpoint created.");
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &rx_client_connection::received_function;
	stack_entry_.connected_function = &rx_client_connection::connected_function;
	stack_entry_.disconnected_function = &rx_client_connection::disconnected_function;

	executer_ = port->get_executer();
}


rx_client_connection::~rx_client_connection()
{
	RXCOMM_LOG_DEBUG("rx_client_connection", 200, "{rx-platform} communication client endpoint destroyed.");
}



rx_protocol_result_t rx_client_connection::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	rx_client_connection* self = reinterpret_cast<rx_client_connection*>(reference->user_data);
	messages::rx_connection_context_request con_context;
	con_context.request_id = 1;
	con_context.stream_version = RX_CURRENT_SERIALIZE_VERSION;
	self->send_message(con_context);
	return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_client_connection::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
	rx_client_connection* self = reinterpret_cast<rx_client_connection*>(reference->user_data);
	self->notify_disconnected();
	return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_client_connection::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_client_connection* self = reinterpret_cast<rx_client_connection*>(reference->user_data);
	self->port_->status.received_packet();
	return self->received(packet);
}

void rx_client_connection::close_endpoint ()
{
	if (port_)
		port_->notify_disconnected();
}

rx_protocol_stack_endpoint* rx_client_connection::get_endpoint ()
{
	return &stack_entry_;
}

rx_protocol_stack_endpoint* rx_client_connection::bind_endpoint ()
{
	return &stack_entry_;
}

void rx_client_connection::send_message (messages::rx_message_base& msg)
{
	uint32_t temp_version = RX_CURRENT_SERIALIZE_VERSION;
	if (stream_version_)
		temp_version = stream_version_;
	serialization::json_writer writter(temp_version);
	auto result = serialize_message(writter, msg.request_id, msg);
	if (result)
	{
		string_type ret_data;
		ret_data = writter.get_string();
		auto buff_result = port_->alloc_io_buffer();
		if (buff_result)
		{
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint8_t)1);
			buff_result.value().write_to_buffer((uint16_t)0x7fff);
			result = buff_result.value().write_string(ret_data);
			send_protocol_packet packet = rx_create_send_packet(msg.request_id, &buff_result.value(), 0, 0);

			auto protocol_res = rx_move_packet_down(&stack_entry_, packet);
			if (protocol_res != RX_PROTOCOL_OK)
			{
				std::cout << "Error returned from move_down:"
					<< rx_protocol_error_message(protocol_res)
					<< "\r\n";
			}
			else
			{
				last_sent_ = rx_get_tick_count();
				port_->status.sent_packet();
			}
			port_->release_io_buffer(buff_result.move_value());
		}
	}
}

rx_protocol_result_t rx_client_connection::received (recv_protocol_packet packet)
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

		string_type msgType;

		if (!reader.start_object("header"))
			return RX_PROTOCOL_PARSING_ERROR;
		if (!reader.read_int("requestId", request_id))
			return RX_PROTOCOL_PARSING_ERROR;
		if (!reader.read_string("msgType", msgType))
			return RX_PROTOCOL_PARSING_ERROR;
		if (!reader.end_object())
			return RX_PROTOCOL_PARSING_ERROR;

		if (request_id == 0)
		{
			if (msgType == "connCtxResp")
			{
				messages::rx_connection_context_response resp;

				rx_result temp_res;

				temp_res = reader.start_object("body");
				if (temp_res)
				{
					temp_res = resp.deserialize(reader);
					if (temp_res)
					{
						temp_res = reader.end_object();
					}
				}
				if (temp_res)
				{
					stream_version_ = resp.stream_version;

					notify_connected();
				}
				else
				{
					RXCOMM_LOG_ERROR("rx_client_connection", 500, "Error connection context response:"s + temp_res.errors_line());
				}
			}
			else if (msgType == "connectionNotify")
			{
				messages::rx_connection_notify_message resp;

				rx_result temp_res;

				temp_res = reader.start_object("body");
				if (temp_res)
				{
					temp_res = resp.deserialize(reader);
					if (temp_res)
					{
						temp_res = reader.end_object();
					}
				}
				if (temp_res)
				{
					notify_item_change(resp.changed_id, resp.changed_path);
				}
				else
				{
					RXCOMM_LOG_ERROR("rx_client_connection", 500, "Error connection context response:"s + temp_res.errors_line());
				}
			}

		}
		else
		{
			auto transaction = get_transaction(request_id);
			if (transaction)
			{
				auto temp_res = transaction->deserialize(reader, msgType);
				if (temp_res)
				{
					rx_post_function_to(get_executer(), smart_this(),
						[](smart_ptr whose, messages::rx_transaction_ptr&& transaction) {
							whose->transaction_received(std::move(transaction));
						}, smart_this(), std::move(transaction));
				}
				else
				{
					RXCOMM_LOG_ERROR("rx_client_connection", 500, "Error deserializing response:"s + temp_res.errors_line());
				}
			}
		}
	}
	return RX_PROTOCOL_OK;
}

void rx_client_connection::notify_connected ()
{
	if (port_)
		port_->notify_connected();
}

void rx_client_connection::notify_disconnected ()
{
	if (port_)
		port_->notify_disconnected();
}

void rx_client_connection::notify_item_change (const rx_node_id& id, const string_type& path)
{
	if (port_)
		port_->notify_item_change(id, path);
}

void rx_client_connection::timer_tick ()
{
	if (rx_get_tick_count() > last_sent_ + 8000)
	{
		messages::rx_keep_alive_message msg;
		msg.request_id = g_next_request_id++;
		if (msg.request_id == 0)
			msg.request_id = g_next_request_id++;

		send_message(msg);
	}
}


// Class rx_internal::rx_protocol::rx_json_protocol_client_port 

std::map<rx_node_id,rx_json_protocol_client_port::smart_ptr> rx_json_protocol_client_port::runtime_instances;

rx_json_protocol_client_port::rx_json_protocol_client_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<rx_client_connection>(this);
		auto entry = rt->bind_endpoint();
		return construct_func_type::result_type{ entry, rt };
	};
}



void rx_json_protocol_client_port::stack_assembled ()
{
	connect(nullptr, nullptr);
}

rx_result rx_json_protocol_client_port::send_request (messages::rx_transaction_ptr trans, uint32_t timeout)
{
	auto ep_ptr = active_endpoint();
	if (ep_ptr)
		return ep_ptr->send_request(std::move(trans), timeout);
	else
		return RX_NOT_CONNECTED;
}

void rx_json_protocol_client_port::register_user (rx_protocol_client_user::smart_ptr user)
{
	RX_ASSERT(!user_);
	user_ = user;
}

void rx_json_protocol_client_port::unregister_user (rx_protocol_client_user::smart_ptr user)
{
	RX_ASSERT(user_ == user);
	user_ = rx_protocol_client_user::smart_ptr::null_ptr;
}

void rx_json_protocol_client_port::notify_connected ()
{
	if (user_)
		user_->client_connected();
}

void rx_json_protocol_client_port::notify_disconnected ()
{
	if (user_)
		user_->client_disconnected();
}

void rx_json_protocol_client_port::notify_item_change (const rx_node_id& id, const string_type& path)
{
	if (user_)
		user_->item_changed(id, path);;
}

rx_result rx_json_protocol_client_port::initialize_runtime (runtime_init_context& ctx)
{
	auto result = status.initialize(ctx);
	return result;
}

rx_result rx_json_protocol_client_port::start_runtime (runtime_start_context& ctx)
{
	rx_timer_ptr timer_ = create_timer_function([this]()
		{
			auto ep = active_endpoint();
			if(ep)
				ep->timer_tick();
		});
	timer_->start(200);
	return true;
}

rx_result rx_json_protocol_client_port::stop_runtime (runtime_stop_context& ctx)
{
	if (timer_)
	{
		timer_->suspend();
		timer_->cancel();
		timer_ = rx_timer_ptr::null_ptr;
	}
	return true;
}


// Class rx_internal::rx_protocol::rx_protocol_client_connection 

std::atomic<messages::rx_request_id_t> rx_protocol_client_connection::g_next_request_id = 1;

rx_protocol_client_connection::rx_protocol_client_connection()
{
}


rx_protocol_client_connection::~rx_protocol_client_connection()
{
}



void rx_protocol_client_connection::transaction_received (messages::rx_transaction_ptr response)
{
	auto result = response->process();
	if (!result)
	{
		RXCOMM_LOG_ERROR("rx_protocol_client_connection", 500, "Error processing response:"s + result.errors_line());
	}
}

void rx_protocol_client_connection::register_transaction (messages::rx_request_id_t id, messages::rx_transaction_ptr trans)
{
	locks::auto_lock_t _(&transactions_lock_);
	pending_transactions_.emplace(id, std::move(trans));
}

messages::rx_transaction_ptr rx_protocol_client_connection::get_transaction (messages::rx_request_id_t id)
{
	locks::auto_lock_t _(&transactions_lock_);
	auto it = pending_transactions_.find(id);
	if (it != pending_transactions_.end())
	{
		messages::rx_transaction_ptr ret = std::move(it->second);
		pending_transactions_.erase(it);
		return ret;
	}
	else
	{
		return messages::rx_transaction_ptr::null_ptr;
	}
}

void rx_protocol_client_connection::set_context (api::rx_context ctx, const messages::rx_connection_context_response& req)
{
}

rx_result rx_protocol_client_connection::send_request (messages::rx_transaction_ptr trans, uint32_t timeout)
{
	trans->get_request().request_id = g_next_request_id++;
	if(trans->get_request().request_id==0)
		trans->get_request().request_id = g_next_request_id++;
	messages::rx_request_id_t req_id = trans->get_request().request_id;

	register_transaction(req_id, trans);
	send_message(trans->get_request());
	return true;
}

void rx_protocol_client_connection::timer_tick ()
{
}


// Class rx_internal::rx_protocol::rx_protocol_client_user 


// Class rx_internal::rx_protocol::rx_json_protocol_port 

rx_json_protocol_port::rx_json_protocol_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<rx_server_connection>(this);
		auto entry = rt->bind_endpoint();
		return construct_func_type::result_type{ entry, rt };
	};
}



void rx_json_protocol_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}

rx_result rx_json_protocol_port::initialize_runtime (runtime_init_context& ctx)
{
	auto result = status.initialize(ctx);
	return result;
}


// Class rx_internal::rx_protocol::rx_local_subscription 


// Class rx_internal::rx_protocol::rx_protocol_connection 

rx_protocol_connection::rx_protocol_connection (rx_mode_type mode)
      : current_directory_path_("/world"),
        mode_(mode)
{
	current_directory_ = rx_gate::instance().get_directory("/world");
}


rx_protocol_connection::~rx_protocol_connection()
{
	RXCOMM_LOG_DEBUG("rx_server_connection", 200, "{rx-platform} communication server endpoint destroyed.");
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

void rx_protocol_connection::response_received (response_message_ptr&& response)
{
	if (current_directory_)
	{
		api::rx_context ctx;
		ctx.active_path = current_directory_->meta_info().get_full_path();
		ctx.object = smart_this();

		auto result = response->process_response(ctx, smart_this());

		if (!result)
		{

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
		auto temp = std::make_unique<rx_protocol_subscription>(data, smart_this(), mode_);
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
		return it->second->write_items(transaction_id, mode_.is_test(), std::move(values), results);
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
	RX_ASSERT(subscriptions_.empty());
	mode_ = req.mode;
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


// Class rx_internal::rx_protocol::rx_protocol_subscription 

rx_protocol_subscription::rx_protocol_subscription (subscription_data& data, rx_protocol_connection_ptr conn, rx_mode_type mode)
      : data_(data)
	, connection_(conn)
{
	my_subscription_ = rx_create_reference<sys_runtime::subscriptions::rx_subscription>(this, mode);
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

void rx_protocol_subscription::execute_completed (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, values::rx_simple_value data)
{
	/*if (connection_)
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
	}*/
	RX_ASSERT(false);
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

void rx_protocol_subscription::write_completed (runtime_transaction_id_t transaction_id, std::vector<write_result_item> results)
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
			auto it = items_.find(one.handle);
			if (it != items_.end())
			{
				one.handle = it->second.client_handle;
				notify_msg->add_result(std::move(one));
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

rx_result rx_protocol_subscription::write_items (runtime_transaction_id_t transaction_id, bool test, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results)
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


// Class rx_internal::rx_protocol::rx_server_connection 

rx_server_connection::rx_server_connection (rx_json_protocol_port* port)
      : executer_(-1),
        port_(port),
        stream_version_(RX_CURRENT_SERIALIZE_VERSION)
	, rx_protocol_connection(rx_mode_type())
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



rx_protocol_stack_endpoint* rx_server_connection::bind_endpoint ()
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
	self->port_->status.received_packet();
	return self->received(packet);
}

rx_protocol_result_t rx_server_connection::received (recv_protocol_packet packet)
{

	if (packet.buffer && rx_get_packet_available_data(packet.buffer) > 0)
	{
		io::rx_const_io_buffer json_buffer(packet.buffer);
		uint32_t temp_version = stream_version_ == 0 ? RX_CURRENT_SERIALIZE_VERSION : stream_version_;

		string_type json;
		if(!json_buffer.read_chars(json))
			return RX_PROTOCOL_PARSING_ERROR;

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
	model::platform_types_manager::instance().get_types_resolver().unregister_subscriber(smart_this());
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
	response->mode = req.mode;
	response->network_id = rx_gate::instance().get_network_id();
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
			result = buff_result.value().write_chars(ret_data);
			send_protocol_packet packet = rx_create_send_packet(msg->request_id, &buff_result.value(), 0, 0);

			auto protocol_res = rx_move_packet_down(&stack_entry_, packet);
			if (protocol_res != RX_PROTOCOL_OK)
			{
				std::cout << "Error returned from move_down:"
					<< rx_protocol_error_message(protocol_res)
					<< "\r\n";
			}
			else
			{
				port_->status.sent_packet();
			}
			port_->release_io_buffer(buff_result.move_value());
		}
	}
}


// Class rx_internal::rx_protocol::subscription_data 


// Class rx_internal::rx_protocol::subscription_item_data 


// Class rx_internal::rx_protocol::rx_opc_protocol_adapter_port 

rx_opc_protocol_adapter_port::rx_opc_protocol_adapter_port()
{
	construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
		{
			auto rt = std::make_unique<rx_opc_adapter_endpoint>(this);
			rx_protocol_stack_endpoint* entry = &rt->get_stack_entry();
			return construct_func_type::result_type{ entry, std::move(rt) };
		};
}



rx_result rx_opc_protocol_adapter_port::initialize_runtime (runtime_init_context& ctx)
{
	auto result = status.initialize(ctx);
	return result;
}


// Class rx_internal::rx_protocol::rx_opc_adapter_endpoint 

rx_opc_adapter_endpoint::rx_opc_adapter_endpoint (rx_opc_protocol_adapter_port* port)
      : port_(port)
{
	RXCOMM_LOG_DEBUG("rx_opc_adapter_endpoint", 900, "OPC Adapter endpoint created.");
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &rx_opc_adapter_endpoint::received_function;
	stack_entry_.send_function = &rx_opc_adapter_endpoint::send_function;
}


rx_opc_adapter_endpoint::~rx_opc_adapter_endpoint()
{
	RXCOMM_LOG_DEBUG("rx_opc_adapter_endpoint", 900, "OPC Adapter endpoint destoryed.");
}



rx_protocol_result_t rx_opc_adapter_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_opc_adapter_endpoint* self = reinterpret_cast<rx_opc_adapter_endpoint*>(reference->user_data);
	if (self->port_)
	{
		self->port_->status.received_packet();
		return self->received(packet);
	}
	else
	{
		return RX_PROTOCOL_DISCONNECTED;
	}
}

rx_protocol_result_t rx_opc_adapter_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	rx_opc_adapter_endpoint* self = reinterpret_cast<rx_opc_adapter_endpoint*>(reference->user_data);
	if (self->port_)
	{
		self->port_->status.sent_packet();
		return self->send(packet);
	}
	else
	{
		return RX_PROTOCOL_DISCONNECTED;
	}
}

rx_protocol_result_t rx_opc_adapter_endpoint::received (recv_protocol_packet packet)
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
	if (result && !json.empty())
	{
		rx_const_packet_buffer pack{};
		rx_init_const_packet_buffer(&pack, json.c_str() , json.size());
		recv_protocol_packet up = rx_create_recv_packet(packet.id, &pack, packet.from, packet.to);
		return rx_move_packet_up(&stack_entry_, up);
	}
	
	return RX_PROTOCOL_PARSING_ERROR;
}

rx_protocol_result_t rx_opc_adapter_endpoint::send (send_protocol_packet packet)
{
	if(packet.buffer == nullptr)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;

	io::rx_io_buffer send_buffer;
	send_buffer.attach(packet.buffer);
	send_buffer.write_to_buffer_front((uint32_t)send_buffer.size);
	send_buffer.write_to_buffer_front((uint16_t)0x7fff);
	send_buffer.write_to_buffer_front((uint8_t)1);
	send_buffer.write_to_buffer_front((uint8_t)1);
	send_buffer.detach(packet.buffer);

	return rx_move_packet_down(&stack_entry_ , packet);
}


} // namespace rx_protocol
} // namespace rx_internal

