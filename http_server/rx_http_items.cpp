

/****************************************************************************
*
*  http_server\rx_http_items.cpp
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


// rx_http_mapping
#include "protocols/http/rx_http_mapping.h"
// rx_http_items
#include "http_server/rx_http_items.h"

#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "lib/rx_ser_json.h"


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
	context.active_path = "/world";
	context.object = req.whose;
	auto idx = req.path.rfind('.');
	if (idx != string_type::npos)
	{
		req.path = req.path.substr(0, idx);
	}
	const char* path_val = req.path.c_str();
	if (!req.path.empty() && req.path[0] == '/')
		path_val = &req.path[1];


	string_type whose;
	string_type item_path;
	rx_split_item_path(path_val, whose, item_path);
	if (!whose.empty())
	{

		auto resolve_result = api::ns::rx_resolve_reference(whose, directories);
		if (!resolve_result)
		{
			return resolve_result.errors();

		}

		rx_thread_handle_t executer = rx_thread_context();
		rx_result result = model::algorithms::do_with_runtime_item(resolve_result.value()
			, [this, resp, req, sub_item = std::move(item_path), executer](platform_item_ptr data)->rx_result
		{
			item_result_type result;
			result.response = resp;
			result.request = req;

			auto ret = this->do_with_item(sub_item, result.request, result.response, std::move(data));
			if (!ret)
			{
				result.response.headers["Content-Type"] = "text/plain";
				result.response.result = 501;
				result.response.set_string_content(ret.errors_line());
				if (result.request.whose)
					http_server::instance().send_response(result.request, result.response);
			}

			return ret;
		}
		, [this](rx_result result)
		{
		}, context);

		return result;


	}
	return "Invalid path";
}


// Class rx_internal::rx_http_server::http_json_object_reader 


const char* http_json_object_reader::get_content_type ()
{
	return "application/json";
}

rx_result http_json_object_reader::do_with_item (string_view_type sub_item, http_request& req, http_response& resp, platform_item_ptr item)
{
	read_struct_data read_data;
	rx_reference_ptr anchor = req.whose;
	bool persist = req.params.count("persist");

	runtime_value_type val_type = runtime_value_type::simple_runtime_value;
	if (req.params.count("full"))
		val_type = runtime_value_type::full_runtime_value;
	if (req.params.count("persist"))
		val_type = runtime_value_type::persistent_runtime_value;

	read_data.callback = read_struct_callback_t(anchor, [this, req = std::move(req), resp = std::move(resp)](rx_result result, data::runtime_values_data data) mutable
	{
		if (req.params.count("pretty") > 0)
		{
			serialization::pretty_json_writer writer;
			if (result)
			{
				writer.write_header(STREAMING_TYPE_MESSAGE, 0);
				if (writer.write_init_values(nullptr, data))
				{
					result = writer.write_footer();
				}
				else
				{
					result = writer.get_error();
				}
			}
			if (result)
			{

				resp.headers["Content-Type"] = get_content_type();
				resp.result = 200;
				resp.set_string_content(writer.get_string());
			}
			else
			{
				resp.headers["Content-Type"] = "text/plain";
				resp.result = 501;
				resp.set_string_content(result.errors_line());
			}
		}
		else
		{
			serialization::json_writer writer;
			if (result)
			{
				writer.write_header(STREAMING_TYPE_MESSAGE, 0);
				if (writer.write_init_values(nullptr, data))
				{
					result = writer.write_footer();
				}
				else
				{
					result = writer.get_error();
				}
			}
			if (result)
			{

				resp.headers["Content-Type"] = get_content_type();
				resp.result = 200;
				resp.set_string_content(writer.get_string());
			}
			else
			{
				resp.headers["Content-Type"] = "text/plain";
				resp.result = 501;
				resp.set_string_content(result.errors_line());
			}
		}
		
		
		if (req.whose)
			http_server::instance().send_response(req, resp);
	});

	read_data.type = val_type;
	item->read_struct(sub_item, std::move(read_data));
	return true;
}

const char* http_json_object_reader::get_extension ()
{
	return "json";
}


} // namespace rx_http_server
} // namespace rx_internal

