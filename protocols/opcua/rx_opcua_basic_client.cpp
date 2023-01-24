

/****************************************************************************
*
*  protocols\opcua\rx_opcua_basic_client.cpp
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


// rx_opcua_basic_client
#include "protocols/opcua/rx_opcua_basic_client.h"

#include "system/server/rx_server.h"
#include "rx_platform_version.h"
#include "rx_opcua_mapping.h"
#include "rx_monitoreditem_set.h"
#include "rx_opcua_identifiers.h"
#include "rx_opcua_value.h"
using namespace protocols::opcua::ids;
using namespace protocols::opcua::common;
using namespace protocols::opcua::requests::opcua_session;
using namespace protocols::opcua::requests::opcua_monitoreditem;


namespace protocols {

namespace opcua {

namespace opcua_basic_client {

// Class protocols::opcua::opcua_basic_client::opcua_basic_client_port 

std::map<rx_node_id, opcua_basic_client_port::smart_ptr> opcua_basic_client_port::runtime_instances;

opcua_basic_client_port::opcua_basic_client_port()
      : publish_interval_(200)
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<opcua_basic_client_endpoint>(this
			, "BasicClient", application_description_, publish_interval_);
		auto entry = rt->get_endpoint();
		return construct_func_type::result_type{ entry, rt };
	};
}



void opcua_basic_client_port::stack_assembled ()
{
    auto result = connect(nullptr, nullptr);
}

rx_result opcua_basic_client_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	string_type app_uri = ctx.get_item_static("Options.AppUri", ""s);
	string_type app_name = ctx.get_item_static("Options.AppName", ""s);
	string_type app_bind = ctx.get_item_static("Connect.Endpoint", ""s);

	publish_interval_ = ctx.get_item_static("Options.PublishInterval", publish_interval_);

	if (app_name.empty())
		app_name = app_bind.c_str();

	if (app_uri.empty())
		app_uri = app_name;

	application_description_ = opcua_client_endpoint_base::fill_application_description(app_uri, app_name, app_bind, "BasicClient");

    return true;
}

rx_result opcua_basic_client_port::start_runtime (runtime::runtime_start_context& ctx)
{
	timer_ = ctx.create_timer_function(smart_this(), [this]()
		{
			//source_tick(rx_time::now());
		});
	return true;
}

rx_result opcua_basic_client_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	if (timer_)
	{
		timer_->cancel();
		timer_ = rx_timer_ptr::null_ptr;
	}
	return true;
}

void opcua_basic_client_port::fill_application_description (const string_type& app_uri, const string_type& app_name, const string_type& app_bind, const string_type& server_type)
{
}

rx_result opcua_basic_client_port::register_source (basic_source_ptr who)
{
	const auto& value_id = who->get_value_id();
	std::pair<rx_node_id, attribute_id> key{ value_id.node_id, value_id.attr_id };
	locks::auto_lock_t _(&sources_lock_);
	auto it = registered_items_.find(key);
	if (it == registered_items_.end())
	{
		auto ret = registered_items_.emplace(key, std::make_unique<basic_monitored_item>());
		RX_ASSERT(ret.second);
		it = ret.first;
		pending_item_data pending_data;
		pending_data.handle = next_item_handle_++;
		pending_data.key = std::move(key);
		queued_items_.push_back(std::move(pending_data));
	}
	it->second->register_source(who);
	return true;
}

rx_result opcua_basic_client_port::unregister_source (basic_source_ptr who)
{
	const auto& value_id = who->get_value_id();
	std::pair<rx_node_id, attribute_id> key{ value_id.node_id, value_id.attr_id };
	locks::auto_lock_t _(&sources_lock_);
	auto it = registered_items_.find(key);
	if (it != registered_items_.end())
	{
		return it->second->unregister_source(who);
	}
	else
	{
		RX_ASSERT(false);// should not happened
		return RX_INVALID_ARGUMENT;
	}
}

void opcua_basic_client_port::subscription_notification (requests::opcua_subscription::opcua_data_change_notification* data)
{
	locks::auto_lock_t _(&sources_lock_);
	// 1. check for pending stuff (using server as timer saves resources ;) )
	if (!queued_items_.empty() && pending_items_.empty())
	{
		auto ep = active_endpoint();
		if (ep)
		{
			std::copy(queued_items_.begin(), queued_items_.end(), std::back_inserter(pending_items_));
			std::vector<create_monitored_item_data> to_create;
			to_create.reserve(pending_items_.size());
			for (const auto& one : pending_items_)
			{
				create_monitored_item_data one_data;
				one_data.to_monitor.node_id = one.key.first;
				one_data.to_monitor.attr_id = one.key.second;
				one_data.mode = monitoring_mode_t::reporting;
				one_data.parameters.client_handle = one.handle;
				one_data.parameters.discard_oldest = true;
				one_data.parameters.interval = -1;
				one_data.parameters.queue_size = 1;
				/*auto filter = std::make_unique<opcua_data_change_filter>();
				filter->data_change_trigger = data_change_trigger_t::STATUS_VALUE;
				filter->deadband_type = 0;
				filter->deadband_value = 0;
				one_data.parameters.filter_ptr = std::move(filter);*/

				to_create.push_back(std::move(one_data));
			}
			auto result = ep->add_subscription_items(std::move(to_create));
			if (!result)
			{
				pending_items_.clear();
				std::ostringstream ss;
				ss << "Error creating monitored items:";
				ss << result.errors_line();
				OPCUA_LOG_ERROR("opcua_basic_client_port", 500, ss.str());
			}
		}
	}
	// 2. send notifications to active ones if there is some data
	if (data)
	{
		if (!data->monitored_items.empty() && !active_items_.empty())
		{
			for (const auto& one : data->monitored_items)
			{
				auto it_active = active_items_.find(one.client_handle);
				if (it_active != active_items_.end())
				{
					auto it_item = registered_items_.find(it_active->second);
					if (it_item != registered_items_.end())
					{
						it_item->second->data_changed(one.value);
					}
				}
			}
		}
	}
}

void opcua_basic_client_port::subscription_disconnected (requests::opcua_subscription::opcua_data_change_notification* data)
{
	locks::auto_lock_t _(&sources_lock_);
	active_items_.clear();
	queued_items_.clear();
	pending_items_.clear();
	if (registered_items_.empty())
	{
		rx_time now(rx_time::now());
		for (auto& one : registered_items_)
		{
			one.second->subscription_disconnected(now);
		}
	}
}

rx_result opcua_basic_client_port::create_items_response (const std::vector<create_monitored_item_result>& results)
{
	size_t count = results.size();
	locks::auto_lock_t _(&sources_lock_);
	if (results.empty() || pending_items_.size() != count)
	{// error occurred, no result items, or wrong number
		pending_items_.clear();
	}
	else
	{
		bool had_one = false;
		pending_type new_queue;
		// had to be because of previous condition, never the less..
		RX_ASSERT(pending_items_.size() == count);		
		for (size_t i = 0; i < count; i++)
		{
			if (results[i].status == opcid_OK)
			{
				had_one = true;
				auto result = active_items_.emplace(pending_items_[i].handle, pending_items_[i].key);
				RX_ASSERT(result.second);
			}
			else
			{
				new_queue.push_back(std::move(pending_items_[i]));
			}
		}
		if (!new_queue.empty() && had_one)
		{
			queued_items_ = std::move(new_queue);
		}
		else
		{
			queued_items_.clear();
		}
		pending_items_.clear();
	}
	return true;
}


// Class protocols::opcua::opcua_basic_client::opcua_basic_client_endpoint 

opcua_basic_client_endpoint::opcua_basic_client_endpoint (opcua_basic_client_port* port, const string_type& client_type, const application_description& app_descr, uint32_t interval)
      : executer_(-1),
        port_(port),
        current_request_id_(1),
        state_(opcua_client_state::not_connected),
        publish_interval_(interval),
        subscription_id_(0)
	, opcua_client_endpoint_base(client_type, app_descr)
{

	OPCUA_LOG_DEBUG("opcua_client_endpoint", 200, "OPCUA client endpoint created.");
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &opcua_basic_client_endpoint::received_function;
	stack_entry_.connected_function = &opcua_basic_client_endpoint::connected_function;
	stack_entry_.disconnected_function = &opcua_basic_client_endpoint::disconnected_function;

	executer_ = port->get_executer();
}


opcua_basic_client_endpoint::~opcua_basic_client_endpoint()
{

	OPCUA_LOG_DEBUG("opcua_client_endpoint", 200, "OPCUA client endpoint destroyed.");
}



rx_protocol_result_t opcua_basic_client_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	opcua_basic_client_endpoint* me = (opcua_basic_client_endpoint*)reference->user_data;
	return me->client_connected(reference, session);
}

rx_protocol_result_t opcua_basic_client_endpoint::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t opcua_basic_client_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	opcua_basic_client_endpoint* me = (opcua_basic_client_endpoint*)reference->user_data;
	return me->received(reference, packet);
}

void opcua_basic_client_endpoint::close_endpoint ()
{
}

rx_protocol_stack_endpoint* opcua_basic_client_endpoint::get_endpoint ()
{
	return &stack_entry_;
}

rx_result opcua_basic_client_endpoint::send_request (requests::opcua_request_ptr req)
{
	rx_result result;
	auto to_send = port_->alloc_io_buffer();
	if (to_send)
	{
		opcua::binary::ua_binary_ostream ostream(&to_send.value());
		req->authentication_token = session.authentication_token;
		ostream << req->get_binary_request_id();
		result = req->serialize_header_binary(ostream);
		if (result)
		{
			result = req->serialize_binary(ostream);
			if (result)
			{
				auto packet = rx_create_send_packet(req->request_id, &to_send.value(), 0, 0);
				auto protocol_result = rx_move_packet_down(&stack_entry_, packet);
				if (protocol_result != RX_PROTOCOL_OK)
				{
					result = rx_protocol_error_message(protocol_result);
				}
			}
		}
		port_->release_io_buffer(to_send.move_value());
	}
	else
	{
		result = to_send.errors();
	}
	return result;
}

rx_result opcua_basic_client_endpoint::session_activated ()
{
	// session is now active so create subscription
	return create_subscription(publish_interval_);
}

rx_result opcua_basic_client_endpoint::subscription_created (uint32_t subscription_id)
{
	subscription_id_ = subscription_id;
	subscription_notification(nullptr);
	return true;
}

void opcua_basic_client_endpoint::subscription_notification (requests::opcua_subscription::opcua_data_change_notification* data)
{
	port_->subscription_notification(data);
}

rx_result opcua_basic_client_endpoint::add_subscription_items (std::vector<create_monitored_item_data> to_create)
{
	if (subscription_id_ == 0)
		return RX_INVALID_STATE;

	return create_subscription_items(subscription_id_, std::move(to_create));
}

rx_result opcua_basic_client_endpoint::create_items_response (const std::vector<create_monitored_item_result>& results)
{
	if (port_)
		return port_->create_items_response(results);
	else
		return RX_INVALID_STATE;
}


// Class protocols::opcua::opcua_basic_client::opcua_basic_source 

opcua_basic_source::opcua_basic_source()
{
}


opcua_basic_source::~opcua_basic_source()
{
}



rx_result opcua_basic_source::initialize_source (runtime::runtime_init_context& ctx)
{
	string_type full_path = ctx.get_item_static(".SimplePath", ""s);
	uint32_t numeric_id = ctx.get_item_static(".NumericId", 0);
	uint16_t nspace = ctx.get_item_static<uint16_t>(".Namespace", 2);
	uint8_t attr_id = ctx.get_item_static<uint8_t>(".AttrId", 0x0d);
	if (full_path.empty())
		full_path = ctx.meta.name + RX_OBJECT_DELIMETER + ctx.path.get_parent_path(1);

	if (numeric_id)
		value_id_.node_id = rx_node_id(numeric_id, nspace);
	else
		value_id_.node_id = rx_node_id(full_path.c_str(), nspace);
	if (attr_id > (uint8_t)attribute_id::max_id || attr_id < (uint8_t)attribute_id::min_id)
	{
		OPCUA_LOG_WARNING("opcua_basic_source", 900, "Invalid Attribute Id, dropping to Attribute Id 13");
		value_id_.attr_id = attribute_id::value;
	}
	else
	{
		value_id_.attr_id = (attribute_id)attr_id;
	}

	return true;
}

void opcua_basic_source::port_connected (port_ptr_t port)
{
	port->register_source(smart_this());
}

void opcua_basic_source::port_disconnected (port_ptr_t port)
{
	port->unregister_source(smart_this());
}

void opcua_basic_source::opcua_value_changed (const data_value& val)
{
	rx_value rval;
	auto result = val.fill_rx_value(rval);
	if (!result)
		rval.set_quality(RX_BAD_QUALITY_SYNTAX_ERROR);
	rval.increment_signal_level();
	source_value_changed(std::move(rval));
}


// Class protocols::opcua::opcua_basic_client::basic_monitored_item 

basic_monitored_item::basic_monitored_item()
      : server_handle_(0)
{
}


basic_monitored_item::~basic_monitored_item()
{
}



rx_result basic_monitored_item::register_source (basic_source_ptr who)
{
	auto ret = registered_sources_.insert(who);
	if (ret.second)
	{
		if (!value_.value.is_null())
			who->opcua_value_changed(value_);
		return true;
	}
	else
	{
		RX_ASSERT(false);
		return RX_INVALID_ARGUMENT;
	}
}

rx_result basic_monitored_item::unregister_source (basic_source_ptr who)
{
	auto ret = registered_sources_.erase(who);
	if (ret)
	{
		return true;
	}
	else
	{
		RX_ASSERT(false);
		return RX_INVALID_ARGUMENT;
	}
}

void basic_monitored_item::subscription_disconnected (rx_time now)
{
}

void basic_monitored_item::data_changed (const data_value& value)
{
	for (auto one : registered_sources_)
	{
		one->opcua_value_changed(value);
	}
}


} // namespace opcua_basic_client
} // namespace opcua
} // namespace protocols

