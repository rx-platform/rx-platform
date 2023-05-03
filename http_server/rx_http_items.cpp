

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
#include "api/rx_meta_api.h"
#include "model/rx_model_algorithms.h"
#include "lib/rx_ser_json.h"
#include "runtime_internal/rx_runtime_internal.h"


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


	string_type type = req.params["_type"];

	rx_item_type type_type;
	rx_node_id type_id;

	if (!type.empty())
	{
		auto resolve_result = api::ns::rx_resolve_reference(rx_item_reference(type), type_type, directories);
		if (resolve_result.name.empty())
		{
			return "Error resolving type " + type;
		}
		type_id = resolve_result.id;
	}

	string_type whose;
	string_type item_path;
	rx_split_item_path(path_val, whose, item_path);
	if (!whose.empty())
	{
		auto dir_ptr = rx_gate::instance().get_directory(whose, &directories);
		if (dir_ptr)
		{
			rx_thread_handle_t executer = rx_thread_context();
			rx_result result = model::algorithms::do_in_meta_with_dir(
				[this, resp, req, sub_item = std::move(item_path), executer, type_id, type_type](rx_directory_ptr data)->rx_result
			{
				item_result_type result;
				result.response = resp;
				result.request = req;

				auto ret = this->do_with_directory(sub_item, type_type, type_id, result.request, result.response, std::move(data));
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
			, [](rx_result) {}, dir_ptr, context);

			return result;
		}
		else
		{
			auto resolve_result = api::ns::rx_resolve_reference(whose, directories);
			if (!resolve_result)
			{
				return resolve_result.errors();

			}

			rx_thread_handle_t executer = rx_thread_context();
			rx_result result = model::algorithms::do_with_runtime_item(resolve_result.value()
				, [this, resp, req, sub_item = std::move(item_path), executer, type_type, type_id](platform_item_ptr data)->rx_result
			{
				item_result_type result;
				result.response = resp;
				result.request = req;

				auto ret = this->do_with_item(sub_item, type_type, type_id, result.request, result.response, std::move(data));
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
			, [](rx_result) {}, context);

			return result;
		}

	}
	return "Invalid path";
}


// Class rx_internal::rx_http_server::http_json_object_reader 


const char* http_json_object_reader::get_content_type ()
{
	return "application/json";
}

rx_result http_json_object_reader::do_with_item (string_view_type sub_item, rx_item_type type_type, rx_node_id type_id, http_request& req, http_response& resp, platform_item_ptr item)
{
	read_struct_data read_data;
	rx_reference_ptr anchor = req.whose;
	bool persist = req.params.count("_persist");

	runtime_value_type val_type = runtime_value_type::simple_runtime_value;
	if (req.params.count("_full"))
		val_type = runtime_value_type::full_runtime_value;
	if (req.params.count("_persist"))
		val_type = runtime_value_type::persistent_runtime_value;

	read_data.callback = read_struct_callback_t(anchor, [this, req = std::move(req), resp = std::move(resp)](rx_result result, data::runtime_values_data data) mutable
	{
		if (req.params.count("_pretty") > 0)
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

rx_result http_json_object_reader::do_with_directory (string_view_type sub_item, rx_item_type type_type, rx_node_id type_id, http_request& req, http_response& resp, rx_directory_ptr item)
{
	read_struct_data read_data;
	rx_reference_ptr anchor = req.whose;
	bool persist = req.params.count("_persist");

	runtime_value_type val_type = runtime_value_type::simple_runtime_value;
	if (req.params.count("_full"))
		val_type = runtime_value_type::full_runtime_value;
	if (req.params.count("_persist"))
		val_type = runtime_value_type::persistent_runtime_value;

	std::set<rx_node_id> included_by_type;

	rx_result result;

	switch (type_type)
	{
		case rx_object_type:
			{
				auto inst_result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_types::object_type>().get_instanced_objects(type_id);
				if (inst_result)
				{
					for (auto& one : inst_result.items)
					{
						included_by_type.insert(one.data.id);
					}
				}
			}
			break;
		case rx_port_type:
			{
				auto inst_result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_types::port_type>().get_instanced_objects(type_id);
				if (inst_result)
				{
					for (auto& one : inst_result.items)
					{
						included_by_type.insert(one.data.id);
					}
				}
			}
			break;
		case rx_application_type:
			{
				auto inst_result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_types::application_type>().get_instanced_objects(type_id);
				if (inst_result)
				{
					for (auto& one : inst_result.items)
					{
						included_by_type.insert(one.data.id);
					}
				}
			}
			break;
		case rx_domain_type:
			{
				auto inst_result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_types::domain_type>().get_instanced_objects(type_id);
				if (inst_result)
				{
					for (auto& one : inst_result.items)
					{
						included_by_type.insert(one.data.id);
					}
				}
			}
			break;
	}



	std::vector<platform_item_ptr> items;

	recursive_list_directory(item, items, included_by_type);

	
	serialization::json_writer writer;
	writer.write_header(STREAMING_TYPE_MESSAGE, 0);
	auto ret = writer.start_array("items", items.size());
	if (ret)
	{
		for (auto& one : items)
		{
			writer.start_object("item");
			result = one->serialize_value(writer, val_type);
			if (!result)
				break;
			writer.end_object();
		}
		if (result)
		{
			result = writer.end_array();
			if (result)
				result = writer.write_footer();
		}
	}
	if (!result)
	{
		result = writer.get_error();
	}
	resp.headers["Content-Type"] = get_content_type();
	resp.result = 200;
	resp.set_string_content(writer.get_string());


	if (req.whose)
		http_server::instance().send_response(req, resp);

	return true;
}

const char* http_json_object_reader::get_extension ()
{
	return "json";
}

rx_result http_json_object_reader::recursive_list_directory (rx_directory_ptr item, std::vector<platform_item_ptr>& rt_items, const std::set<rx_node_id>& by_type)
{

	platform_directories_type dirs;
	platform_items_type sub_items;
	item->list_content(dirs, sub_items, "");
	for(auto& one : dirs)
	{
		recursive_list_directory(one, rt_items, by_type);
	}
	auto end_it = by_type.end();
	for (auto& one : sub_items)
	{
		if (rx_is_runtime(one.get_type()) && (by_type.empty() || by_type.find(one.get_meta().id)!=end_it))
		{
			auto one_item = rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().get_item(one.get_meta().id);
			if (one_item)
				rt_items.push_back(std::move(one_item));
		}
	}
	return true;

}


// Class rx_internal::rx_http_server::http_object_writer 


const char* http_object_writer::get_content_type ()
{
	return "application/json";
}

rx_result http_object_writer::do_with_item (string_view_type sub_item, rx_item_type type_type, rx_node_id type_id, http_request& req, http_response& resp, platform_item_ptr item)
{
	string_type path(sub_item), str_val;
	rx_reference_ptr anchor = req.whose;
	rx_simple_value value;
	
	if(path.empty())
	{
		return "Missing or empty <path> argument.";
	}
	auto it = req.params.find("val");
	if (it != req.params.end())
	{
		str_val = it->second;
	}
	if (str_val.empty())
	{
		return "Missing or empty <val> argument.";
	}
	value.parse(str_val);
	if (value.is_null())
	{
		return "Unable to parse value string";
	}
	auto us1 = rx_get_us_ticks();
	item->write_value(path, std::move(value), write_result_callback_t(anchor, [this, req = std::move(req), resp = std::move(resp), us1](uint32_t signal_level, rx_result&& result) mutable
		{
			uint64_t us2 = rx_get_us_ticks() - us1;
			std::ostringstream ss;

			resp.headers["Content-Type"] = get_content_type();
			resp.result = 200;
			ss << "{\n   \"level\": " << signal_level 
				<< ",\n   \"time\": " << us2/1000.0
				<< ",\n   \"success\": ";
			
			if (result)
			{
				ss << "true\n";
			}
			else
			{
				ss << "false,\n   \"error\": \"";
				ss << result.errors_line();
				ss << "\"\n";
			}
			ss << "}";
			resp.set_string_content(ss.str());
			
			if (req.whose)
				http_server::instance().send_response(req, resp);

		}));
	return true;
}

rx_result http_object_writer::do_with_directory (string_view_type sub_item, rx_item_type type_type, rx_node_id type_id, http_request& req, http_response& resp, rx_directory_ptr item)
{
	return RX_NOT_SUPPORTED;
}

const char* http_object_writer::get_extension ()
{
	return "write";
}


} // namespace rx_http_server
} // namespace rx_internal

