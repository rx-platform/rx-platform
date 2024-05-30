

/****************************************************************************
*
*  http_server\rx_http_query.cpp
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


// rx_http_mapping
#include "protocols/http/rx_http_mapping.h"
// rx_http_query
#include "http_server/rx_http_query.h"

#include "http_server/rx_http_server.h"
#include "rx_http_items.h"
#include "system/server/rx_file_helpers.h"
#include "rx_http_query.h"
#include "api/rx_platform_api.h"
#include "api/rx_namespace_api.h"
#include "api/rx_runtime_api.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "runtime_internal/rx_runtime_internal.h"
using namespace rx_platform;


namespace rx_internal {

namespace rx_http_server {

// Class rx_internal::rx_http_server::http_enterprise_handler 

http_enterprise_handler::http_enterprise_handler (string_view_type itf_type)
      : my_interface_(nullptr),
        name_(itf_type)
{
    my_interface_ = enterprise::enterprise_manager::instance().get_interface(string_type(itf_type));
    if (my_interface_ == nullptr)
    {
        HTTP_LOG_CRITICAL("http_enterprise_handler", 900
            , "Error activating HTTP handler for "s + name_ + ", interface not registered!");
    }
}



rx_result http_enterprise_handler::handle_request (http_request& req, http_response& resp)
{
    RX_ASSERT(my_interface_);
    if (my_interface_)
    {
		resp.result = 0;
		enterprise::enterprise_args_t args;
		for (const auto& one : req.params)
			args.emplace(one.first, one.second);

		auto idx = req.path.rfind('.');
		if (idx != string_type::npos)
		{
			req.path = req.path.substr(0, idx);
		}
		
		switch (req.method)
		{
		case rx_http_method::get:
			{
				auto trans_id = callback_.register_request(req, resp);
				my_interface_->begin_read(trans_id, req.path, args, &callback_, req.whose);
			}
			break;
		case rx_http_method::put:
			{
				auto trans_id = callback_.register_request(req, resp);
				my_interface_->begin_write(trans_id, req.path, args, req.get_content_as_string(), &callback_, req.whose);
			}
			break;
		case rx_http_method::post:
			{
				auto trans_id = callback_.register_request(req, resp);
				my_interface_->begin_execute(trans_id, req.path, args, req.get_content_as_string(), &callback_, req.whose);
			}
			break;
		default:
			{
				resp.headers["Content-Type"] = "text/plain";
				resp.result = 501;
				resp.set_string_content("The HTTP method is not supported by the server and cannot be handled.");
				if (req.whose)
					http_server::instance().send_response(req, resp);
			}
		}
		return true;
    }
    else
    {
        return RX_INTERNAL_ERROR;
    }
}

const char* http_enterprise_handler::get_extension ()
{
    if (my_interface_)
        return name_.c_str();
    else
        return nullptr;
}


// Class rx_internal::rx_http_server::enterprise_handler_callback 

enterprise_handler_callback::enterprise_handler_callback()
{
}


enterprise_handler_callback::~enterprise_handler_callback()
{
}



void enterprise_handler_callback::read_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{

	request_complete(trans_id, result, cont_type, data);
}

void enterprise_handler_callback::write_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	request_complete(trans_id, result, cont_type, data);
}

void enterprise_handler_callback::execute_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	request_complete(trans_id, result, cont_type, data);
}

void enterprise_handler_callback::query_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	RX_ASSERT(false);// not supported yet!!!!
}

uint64_t enterprise_handler_callback::register_request (http_request& req, http_response& resp)
{
	auto trans_id = sys_runtime::platform_runtime_manager::instance().get_new_transaction_id();
	enterprise_request_type trans;
	trans.request = req;
	trans.response = resp;

	std::scoped_lock _(requests_lock_);
	pending_requests_.emplace(trans_id, std::move(trans));
	return trans_id;
}

void enterprise_handler_callback::request_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	enterprise_request_type trans;
	{
		std::scoped_lock _(requests_lock_);
		auto it = pending_requests_.find(trans_id);
		if (it != pending_requests_.end())
		{
			trans = std::move(it->second);

			trans.response.result = result;
			trans.response.headers["Content-Type"] = string_type(cont_type);
			trans.response.set_string_content(string_type(data));

			pending_requests_.erase(it);
		}
	}
	if (trans.request.whose)
	{
		http_server::instance().send_response(trans.request, trans.response);
	}
}


} // namespace rx_http_server
} // namespace rx_internal

