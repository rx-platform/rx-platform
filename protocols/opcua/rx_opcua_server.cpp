

/****************************************************************************
*
*  protocols\opcua\rx_opcua_server.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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
#include "rx_opcua_mapping.h"
#include "rx_opcua_resources.h"


namespace protocols {

namespace opcua {

// Class protocols::opcua::opcua_server_endpoint_base 

opcua_server_endpoint_base::opcua_server_endpoint_base (const string_type& server_type, const application_description& app_description, const string_type& ep_path, opcua_addr_space::opcua_address_space_base* addr_space, opcua_subscriptions::opcua_subscriptions_collection* subs, const string_type& port_path)
      : subscriptions_(subs),
        server_type_(server_type),
        address_space_(addr_space),
        ep_path_(ep_path),
        port_path_(port_path),
        app_description_(app_description)
{
}



common::endpoint_description opcua_server_endpoint_base::get_endpoint_description (const string_type& ep_url, bool discovery)
{
	endpoint_description ep_descr;
	ep_descr.application = app_description_;

	string_type addr, path;
	opcua_split_url(ep_url, addr, path);
	if (discovery)
	{
		ep_descr.url = ep_url;
		ep_descr.application.discovery_urls.push_back(ep_url);
	}
	else
	{
		ep_descr.application.discovery_urls.push_back(ep_url);
		ep_descr.url = ep_path_.empty() ? addr : addr + '/' + ep_path_;
	}

	opcua_resources_repository::instance().fill_endpoint_description(ep_descr, port_path_);


	user_token_policy pol;
	pol.policy_id = "Anonymous";
	pol.token_type = 0;

	ep_descr.user_tokens.push_back(std::move(pol));

	return ep_descr;
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

	app_descr.ep_bind = app_bind;

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
			resp_ptr->results.push_back(active_data->results[i].first);
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

void opcua_server_endpoint_base::queue_execute_request (opcua_execute_request_ptr req)
{
	execute_request_data data;
	size_t count = req->to_execute.size();
	data.pending_count = count;
	data.results.reserve(count);
	for(size_t i=0; i<count; i++)
	{
        execute_result temp;
        temp.status_code = opcid_RxPending;
        data.results.emplace_back( 0, std::move(temp));
	}

	execute_request_data* active_data;

	locks::auto_lock_t _(&transactions_lock_);

	auto it = execute_requests_.begin();
	size_t trans_idx = 0;
	while (it != execute_requests_.end() && it->request_ptr)
	{
		it++;
		trans_idx++;
	}
	if (it == execute_requests_.end())
	{
		active_data = &execute_requests_.emplace_back(std::move(data));
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
		auto& execute_ref = active_data->request_ptr->to_execute[i];
		auto res = address_space_->execute(execute_ref.node_id, execute_ref.method_id, execute_ref.arguments, ep);
		if (!res.second)
		{
			active_data->results[i].first = res.first;
			active_data->results[i].second.status_code = res.second;
			active_data->pending_count--;
		}
		else
		{
			active_data->results[i].first = res.second;
			execute_cache_.emplace(res.second, trans_idx);
		}
	}
	if (active_data->pending_count == 0)
	{// everything done
		auto resp_ptr = std::make_unique<requests::opcua_method::opcua_call_response>(*active_data->request_ptr);
		active_data->request_ptr.reset();
		for (size_t i = 0; i < count; i++)
		{
			execute_result temp;
			temp.status_code = active_data->results[i].first;
			resp_ptr->results.push_back(std::move(temp));
		}
		send_response(std::move(resp_ptr));
	}
}

void opcua_server_endpoint_base::execute_response (opcua_result_t status, rx_simple_value out_data, runtime_transaction_id_t trans_id)
{
	locks::auto_lock_t _(&transactions_lock_);
	auto it = execute_cache_.find(trans_id);
	if (it != execute_cache_.end())
	{
		size_t idx = it->second;
		execute_cache_.erase(it);
		if (idx < execute_requests_.size())
		{
			execute_request_data& data = execute_requests_[idx];
			if (data.request_ptr)
			{
				size_t count = data.request_ptr->to_execute.size();
				for (size_t i = 0; i < count; i++)
				{
					if (data.results[i].first == trans_id)
					{
						if (data.results[i].second.status_code == opcid_RxPending)
						{
							data.results[i].second.status_code = status;
							if (out_data.is_struct())
							{
								std::vector<variant_type> out_args;
								for (int i = 0; i < (int)out_data.struct_size(); i++)
								{
									if (out_data[i].is_array() || out_data[i].is_struct())
									{
										RX_ASSERT(false);
										return;
									}
									variant_type temp_var;
									temp_var.from_rx_value(out_data[i]);
									out_args.push_back(std::move(temp_var));
								}
								data.results[i].second.out_arguments = std::move(out_args);
							}
							data.pending_count--;
							if (data.pending_count == 0)
							{// everything done
								auto resp_ptr = std::make_unique<requests::opcua_method::opcua_call_response>(*data.request_ptr);
								data.request_ptr.reset();
								for (size_t i = 0; i < count; i++)
								{
									resp_ptr->results.push_back(std::move(data.results[i].second));
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

rx_protocol_result_t opcua_server_endpoint_base::connected_function (rx_session* session)
{
	if (session->local_addr && session->local_addr->type == protocol_address_string)
	{
		const char* addr = nullptr;
		rx_extract_string_address(session->local_addr, &addr);
		if (addr)
		{
			ep_path_ = addr;
		}
	}
	return RX_PROTOCOL_OK;
}


// Class protocols::opcua::opcua_server_endpoint_data 

opcua_server_endpoint_data::opcua_server_endpoint_data()
{
}


opcua_server_endpoint_data::~opcua_server_endpoint_data()
{
}



} // namespace opcua
} // namespace protocols

