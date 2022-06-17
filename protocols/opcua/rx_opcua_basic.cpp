

/****************************************************************************
*
*  protocols\opcua\rx_opcua_basic.cpp
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


// rx_opcua_basic
#include "protocols/opcua/rx_opcua_basic.h"

#include "protocols/opcua/rx_opcua_mapping.h"
#include "protocols/opcua/rx_opcua_binary.h"
#include "protocols/opcua/rx_opcua_identifiers.h"
#include "protocols/opcua/rx_opcua_requests.h"
#include "protocols/opcua/rx_opcua_builder.h"

using namespace protocols::opcua::ids;


namespace protocols {

namespace opcua {

namespace opcua_server {

// Class protocols::opcua::opcua_server::opcua_basic_server_port 

std::map<rx_node_id, opcua_basic_server_port::smart_ptr> opcua_basic_server_port::runtime_instances;

opcua_basic_server_port::opcua_basic_server_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<opcua_basic_server_endpoint>(
			"opc.tcp://"s + rx_gate::instance().get_node_name()
			, app_name_, app_uri_, this);
		return construct_func_type::result_type{ &rt->stack_entry, rt };
	};
	address_space.set_parent(&std_address_space);
}



void opcua_basic_server_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}

rx_result opcua_basic_server_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	app_uri_ = ctx.get_item_static("Options.AppUri", ""s);
	app_name_ = ctx.get_item_static("Options.AppName", ""s);
	if (app_name_.empty())
		app_name_ = rx_gate::instance().get_instance_name();
	if (app_uri_.empty())
		app_uri_ = app_name_ + "@" + rx_gate::instance().get_node_name();

	auto result = opcua_addr_space::build_standard_address_space(std_address_space, app_uri_);
	if (!result)
		return result;

	return result;
}


// Class protocols::opcua::opcua_server::opcua_basic_server_endpoint 

opcua_basic_server_endpoint::opcua_basic_server_endpoint (const string_type& endpoint_url, const string_type& app_name, const string_type& app_uri, opcua_basic_server_port* port)
      : executer_(-1),
        port_(port)
		, opcua_server_endpoint_base(endpoint_url, app_name, app_uri, &port->address_space)
{
    OPCUA_LOG_DEBUG("opcua_basic_server_endpoint", 200, "Basic OPC UA Server endpoint created.");
    rx_init_stack_entry(&stack_entry, this);
    stack_entry.received_function = &opcua_basic_server_endpoint::received_function;

}


opcua_basic_server_endpoint::~opcua_basic_server_endpoint()
{
	OPCUA_LOG_DEBUG("opcua_basic_server_endpoint", 200, "Basic OPC UA Server endpoint destoryed.");
}



rx_protocol_result_t opcua_basic_server_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	opcua_basic_server_endpoint* me = (opcua_basic_server_endpoint*)reference->user_data;
	io::rx_const_io_buffer received(packet.buffer);

	rx_node_id msg_id;

	opcua::binary::ua_binary_istream stream(&received);
	stream >> msg_id;

	requests::opcua_request_ptr request = requests::opcua_requests_repository::instance().get_request(msg_id);
	if (request)
	{
		auto result = request->deserialize_header_binary(stream);
		if (result)
		{
			result = request->deserialize_binary(stream);

			if (result)
			{
				request->request_id = packet.id;
				requests::opcua_response_ptr resp = request->do_job(me->smart_this());

				opcua::binary::ua_binary_istream stream(&received);

				if (resp)
				{
					result = me->send_response(std::move(resp));
				}
			}
		}
		if(!result)
		{
			OPCUA_LOG_ERROR("opcua_basic_server_endpoint", 500, "Error while handling request:"s + result.errors_line());
		}
	}
	else
	{
		RX_ASSERT(false);
	}
	return RX_PROTOCOL_OK;
}

rx_protocol_result_t opcua_basic_server_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

void opcua_basic_server_endpoint::close_endpoint ()
{
}

rx_result opcua_basic_server_endpoint::send_response (requests::opcua_response_ptr resp)
{
	rx_result result;
	auto to_send = port_->alloc_io_buffer();
	if (to_send)
	{
		opcua::binary::ua_binary_ostream ostream(&to_send.value());
		ostream << resp->get_binary_response_id();
		result = resp->serialize_header_binary(ostream);
		if (result)
		{
			result = resp->serialize_binary(ostream);
			if (result)
			{
				auto packet = rx_create_send_packet(resp->request_id, &to_send.value(), 0, 0);
				auto protocol_result = rx_move_packet_down(&stack_entry, packet);
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


// Class protocols::opcua::opcua_server::opcua_simple_address_space 

opcua_simple_address_space::opcua_simple_address_space()
      : parent_(nullptr)
{
}



void opcua_simple_address_space::set_parent (opcua_addr_space::opcua_address_space_base* parent)
{
	RX_ASSERT(parent_ == nullptr);
	parent_ = parent;
}

rx_result opcua_simple_address_space::register_node (opcua_addr_space::opcua_node_base* what)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result opcua_simple_address_space::unregister_node (opcua_addr_space::opcua_node_base* what)
{
	return RX_NOT_IMPLEMENTED;
}

void opcua_simple_address_space::read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const
{
	if (parent_ != nullptr)
		parent_->read_attributes(to_read, values);
}

void opcua_simple_address_space::browse (const opcua_view_description& view, const std::vector<opcua_browse_description>& to_browse, std::vector<browse_result_internal>& results) const
{
	if (parent_ != nullptr)
		parent_->browse(view, to_browse, results);
}

rx_result opcua_simple_address_space::fill_relation_types (const rx_node_id& base_id, bool include_subtypes, std::set<rx_node_id>& buffer) const
{
	if (parent_ != nullptr)
		return parent_->fill_relation_types(base_id, include_subtypes, buffer);
	else
		return RX_NOT_IMPLEMENTED;
}

rx_result opcua_simple_address_space::set_node_value (const rx_node_id& id, values::rx_value&& val)
{
	if (parent_ != nullptr)
		return parent_->set_node_value(id, std::move(val));
	else
		return RX_INVALID_PATH;
}


} // namespace opcua_server
} // namespace opcua
} // namespace protocols

