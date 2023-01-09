

/****************************************************************************
*
*  protocols\opcua\rx_opcua_client.cpp
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


// rx_opcua_client
#include "protocols/opcua/rx_opcua_client.h"

#include "rx_platform_version.h"
#include "system/server/rx_server.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;
using namespace protocols::opcua::common;
using namespace protocols::opcua::requests::opcua_session;
#include "rx_opcua_mapping.h"

#include "rx_opcua_subs_set.h"
#include "rx_monitoreditem_set.h"
using namespace protocols::opcua::requests::opcua_subscription;
using namespace protocols::opcua::requests::opcua_monitoreditem;


namespace protocols {

namespace opcua {

// Class protocols::opcua::opcua_client_endpoint_base 

opcua_client_endpoint_base::opcua_client_endpoint_base (const string_type& client_type, const application_description& app_descr)
      : current_request_id(1),
        state(opcua_client_state::not_connected),
        client_type_(client_type),
        application_description_(app_descr)
{
}



application_description opcua_client_endpoint_base::fill_application_description (const string_type& app_uri, const string_type& app_name, const string_type& app_bind, const string_type& server_type)
{
	application_description app_descr;

	app_descr.application_uri = "urn:"s
		+ ":" + rx_gate::instance().get_node_name()
		+ ":rx-platform:"
		+ rx_gate::instance().get_instance_name()
		+ ":" + app_uri;

	app_descr.application_name.text = "rx-platform/"s
		+ rx_gate::instance().get_instance_name()
		+ "/" + app_name
		+ "@" + rx_gate::instance().get_node_name();

	std::ostringstream ss;
	ss << "urn:"
		<< "rx-platform."
		<< RX_SERVER_NAME << "."
		<< RX_SERVER_MAJOR_VERSION << "."
		<< RX_SERVER_MINOR_VERSION << ":"
		<< server_type;
	app_descr.product_uri = ss.str();

	app_descr.application_type = application_type_t::client_application_type;

	return app_descr;
}

rx_protocol_result_t opcua_client_endpoint_base::client_connected (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	auto session_request = std::make_unique<opcua_create_session>(
		current_request_id
		, current_request_id++);

	session_request->client_description = application_description_;
	session_request->endpoint_url = "opc.tcp://127.0.0.1:49320";
	session_request->session_name = session_request->client_description.application_uri;
	session_request->client_nounce.assign(0x20, std::byte{ 0 });
	session_request->session_timeout = 120000;
	session_request->max_message_size = 0x1000000;


	auto result = send_request(std::move(session_request));
	if (!result)
	{
		std::ostringstream ss;
		ss << "Error creating Create Session Request";
		ss << result.errors_line();
		OPCUA_LOG_ERROR("opcua_client_endpoint", 800, ss.str());
		return RX_PROTOCOL_COLLECT_ERROR;
	}
	else
	{
		state = opcua_client_state::sent_create_session;
		return RX_PROTOCOL_OK;
	}
}

rx_protocol_result_t opcua_client_endpoint_base::received (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	io::rx_const_io_buffer received(packet.buffer);

	rx_node_id msg_id;

	opcua::binary::ua_binary_istream stream(&received);
	stream >> msg_id;

	requests::opcua_response_ptr response = requests::opcua_requests_repository::instance().get_response(msg_id);
	if (response)
	{
		auto result = response->deserialize_header_binary(stream);
		if (result)
		{
			result = response->deserialize_binary(stream);

			if (result)
			{
				if (response->result == opcid_OK)
				{
					result = response->process_response(smart_this());
				}
				else
				{
					std::ostringstream ss;
					ss << "Error creating session, server returned:";
					ss << result;

					result = ss.str();
				}
			}
		}
		if (!result)
		{
			OPCUA_LOG_ERROR("opcua_basic_server_endpoint", 500, "Error while handling response:"s + result.errors_line());
		}
	}
	else
	{
		RX_ASSERT(false);
	}
	return RX_PROTOCOL_OK;
}

rx_result opcua_client_endpoint_base::create_subscription (uint32_t interval)
{
	uint32_t req_id = current_request_id++;
	auto req = std::make_unique<opcua_create_subs_request>(req_id, req_id);

	req->publish_interval = (double)interval;
	req->enabled = true;

	{
		pending_client_subscription_data data;
		data.interval = interval;

		locks::auto_lock_t _(&transactions_lock);
		subscriptions.pending.emplace(req_id, std::move(data));
	}

	auto result = send_request(std::move(req));
	if (!result)
	{
		{
			locks::auto_lock_t _(&transactions_lock);
			subscriptions.pending.erase(req_id);
		}
		std::ostringstream ss;
		ss << "Error sending Create Subscription Request";
		ss << result.errors_line();
		return ss.str();
	}
	else
	{
		return true;
	}
}

rx_result opcua_client_endpoint_base::create_subscription_items (uint32_t subs_id, std::vector<create_monitored_item_data> to_create)
{
	uint32_t req_id = current_request_id++;
	auto req = std::make_unique<opcua_create_mon_items_request>(req_id, req_id);

	req->subscription_id = subs_id;
	req->timestamps_to_return = timestamps_return_type::both;
	req->to_create = std::move(to_create);

	auto result = send_request(std::move(req));
	if (!result)
	{
		{
			locks::auto_lock_t _(&transactions_lock);
			subscriptions.pending.erase(req_id);
		}
		std::ostringstream ss;
		ss << "Error sending Create Monitored Items Request";
		ss << result.errors_line();
		return ss.str();
	}
	else
	{
		return true;
	}
}


// Class protocols::opcua::opcua_client_session_data 


// Class protocols::opcua::subscriptions_data 


} // namespace opcua
} // namespace protocols

