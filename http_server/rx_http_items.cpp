

/****************************************************************************
*
*  http_server\rx_http_items.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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
// rx_http_items
#include "http_server/rx_http_items.h"

#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"


namespace rx_internal {

namespace rx_http_server {

// Class rx_internal::rx_http_server::http_rx_item_handler 


rx_result http_rx_item_handler::handle_request (http_request& req, http_response& resp)
{
	resp.result = 0;

	string_type path("/world");
	rx_directory_resolver directories;
	directories.add_paths({ path , "/" });
	api::rx_context context;
	context.directory = rx_gate::instance().get_root_directory()->get_sub_directory("world");
	context.object = req.whose;
	auto idx = req.path.rfind('.');
	if (idx != string_type::npos)
	{
		req.path = req.path.substr(0, idx);
	}
	const char* path_val = req.path.c_str();
	if (!req.path.empty() && req.path[0] == '/')
		path_val = &req.path[1];

	auto resolve_result = api::ns::rx_resolve_reference(path_val, directories);
	if (!resolve_result)
	{
		return resolve_result.errors();

	}

	rx_thread_handle_t executer = rx_thread_context();
	rx_result result = model::algorithms::do_with_runtime_item(resolve_result.value()
		, [this, resp = std::move(resp), req = std::move(req) , executer](rx_result_with<platform_item_ptr>&& data) ->rx_result_with<item_result_type>
		{
			item_result_type result;
			result.response = resp;
			result.request = req;
			if (data)
			{
				auto ret = this->do_with_item(result.request, result.response, data.move_value());
				if (ret)
				{
					result.response.result = 200; 
					result.response.headers["Content-Type"] = get_content_type();
				}
				else
				{
					result.response.headers["Content-Type"] = "text/plain";
					result.response.result = 501;
					result.response.set_string_content(ret.errors_line());
				}
			}
			else
			{
				result.response.headers["Content-Type"] = "text/plain";
				result.response.result = 501;
				result.response.set_string_content(data.errors_line());
			}
			return std::move(result);
		}
		, rx_result_with_callback<item_result_type>(context.object, [](rx_result_with<item_result_type>&& result)
			{
				if (result && result.value().request.whose)
				{
					http_server::instance().send_response(result.value().request, result.value().response);
				}
			}), context);

	return result;
}


// Class rx_internal::rx_http_server::http_json_object_reader 


const char* http_json_object_reader::get_content_type ()
{
	return "application/json";
}

rx_result http_json_object_reader::do_with_item (http_request& req, http_response& resp, platform_item_ptr item)
{
	rx_value value;
	auto result = item->read_value("", value);
	if (result)
	{
		resp.set_string_content(value.get_storage().get_string_value());
	}
	return result;
}

const char* http_json_object_reader::get_extension ()
{
	return "json";
}


} // namespace rx_http_server
} // namespace rx_internal

