

/****************************************************************************
*
*  protocols\opcua\rx_opcua_server.cpp
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

#include "rx_platform_version.h"

// rx_opcua_server
#include "protocols/opcua/rx_opcua_server.h"

#include "system/server/rx_server.h"
#include "runtime_internal/rx_runtime_internal.h"
using namespace protocols::opcua::common;


namespace protocols {

namespace opcua {

// Class protocols::opcua::opcua_server_endpoint_base 

opcua_server_endpoint_base::opcua_server_endpoint_base (const string_type& server_type, const application_description& app_descr, opcua_addr_space::opcua_address_space_base* addr_space, opcua_subscriptions::opcua_subscriptions_collection* subs)
      : subscriptions_(subs),
        server_type_(server_type),
        address_space_(addr_space),
        application_description_(app_descr)
{
}



common::endpoint_description opcua_server_endpoint_base::get_endpoint_description (const string_type& ep_url, bool discovery)
{
	endpoint_description ep_descr;
	ep_descr.application = get_application_description(ep_url);
	if (discovery)
	{
		ep_descr.url = ep_url;
		ep_descr.application.discovery_urls[0] = ep_url;
	}
	else
	{
		ep_descr.url = ep_descr.application.discovery_urls[0];
	}

	ep_descr.security_mode = security_mode_t::none;
	ep_descr.policy_uri = "http://opcfoundation.org/UA/SecurityPolicy#None";
	ep_descr.security_level = 0;
	ep_descr.transport_profile_uri = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";

	user_token_policy pol;
	pol.policy_id = "Anonymous";
	pol.token_type = 0;

	ep_descr.user_tokens.push_back(std::move(pol));

	return ep_descr;
}

common::application_description opcua_server_endpoint_base::get_application_description (const string_type& ep_url)
{
	application_description app_descr(application_description_);
	if (!application_description_.discovery_urls.empty())
	{
		string_type resolved_ep;
		app_descr.discovery_urls.clear();
		auto pos = ep_url.find("://");
		if (pos != string_type::npos)
			pos = pos + 3;
		else
			pos = 0;
		pos = ep_url.find("/", pos);
		if (pos == string_type::npos)
			resolved_ep = ep_url;
		else
			resolved_ep = ep_url.substr(0, pos);
				
		for (const auto& one : application_description_.discovery_urls)
		{
			app_descr.discovery_urls.push_back(resolved_ep + '/' + one);
		}
	}
	else
		app_descr.discovery_urls.push_back(ep_url);

	return app_descr;
}

opcua_addr_space::opcua_address_space_base* opcua_server_endpoint_base::get_address_space ()
{
	return address_space_;
}

application_description opcua_server_endpoint_base::fill_application_description (const string_type& app_uri, const string_type& app_name, const string_type& app_bind, const string_type& server_type)
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

	app_descr.application_type = application_type_t::server_application_type;

	string_type endpoint_path = app_bind;
	if (!endpoint_path.empty())
		endpoint_path += "/";
	endpoint_path += app_name;
	app_descr.discovery_urls.emplace_back(endpoint_path);

	return app_descr;
}

opcua_subscriptions::opcua_subscriptions_collection* opcua_server_endpoint_base::get_subscriptions ()
{
	return subscriptions_;
}

void opcua_server_endpoint_base::queue_write_request (opcua_write_request_ptr req)
{
	write_request_data data;
	size_t count = req->to_write.size();
	data.pending_count = count;
	data.results.assign(count, { 0, opcid_RxPending });

	write_request_data* active_data;
	
	locks::auto_lock_t _(&transactions_lock_);

	auto it = write_requests_.begin();
	size_t trans_idx = 0;
	while (it != write_requests_.end() && it->request_ptr)
	{
		it++;
		trans_idx++;
	}
	if (it == write_requests_.end())
	{
		active_data = &write_requests_.emplace_back(std::move(data));
	}
	else
	{
		*it = std::move(data);
		active_data = &(*it);
	}
	active_data->request_ptr = std::move(req);
	smart_ptr ep = smart_this();
	for (size_t i = 0; i < count; i++)
	{
		auto& write_ref = active_data->request_ptr->to_write[i];
		auto res = address_space_->write_attribute(write_ref.node_id, write_ref.attr_id, write_ref.range, write_ref.value, ep);
		if (!res.second)
		{
			active_data->results[i] = res;
			active_data->pending_count--;
		}
		else
		{
			active_data->results[i].first = res.second;
			write_cache_.emplace(res.second, trans_idx);
		}
	}
	if (active_data->pending_count == 0)
	{// everything done
		auto resp_ptr = std::make_unique<requests::opcua_attributes::opcua_write_response>(*active_data->request_ptr);
		active_data->request_ptr.reset();
		for (size_t i = 0; i < count; i++)
		{
			resp_ptr->results.push_back(active_data->results[i].second);
		}
		send_response(std::move(resp_ptr));
	}
}

void opcua_server_endpoint_base::write_response (opcua_result_t status, runtime_transaction_id_t trans_id)
{
	locks::auto_lock_t _(&transactions_lock_);
	auto it = write_cache_.find(trans_id);
	if (it != write_cache_.end())
	{
		size_t idx = it->second;
		write_cache_.erase(it);
		if (idx < write_requests_.size())
		{
			write_request_data& data = write_requests_[idx];
			if (data.request_ptr)
			{
				size_t count = data.request_ptr->to_write.size();
				for (size_t i = 0; i < count; i++)
				{
					if (data.results[i].first == trans_id)
					{
						if (data.results[i].second == opcid_RxPending)
						{
							data.results[i].second = status;
							data.pending_count--;
							if (data.pending_count == 0)
							{// everything done
								auto resp_ptr = std::make_unique<requests::opcua_attributes::opcua_write_response>(*data.request_ptr);
								data.request_ptr.reset();
								for (size_t i = 0; i < count; i++)
								{
									resp_ptr->results.push_back(data.results[i].second);
								}
								send_response(std::move(resp_ptr));
							}
						}
					}
				}
			}
		}
	}
}


} // namespace opcua
} // namespace protocols

