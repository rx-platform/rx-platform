

/****************************************************************************
*
*  enterprise\rx_json_ent.cpp
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


// rx_json_ent
#include "enterprise/rx_json_ent.h"

#include "api/rx_namespace_api.h"
#include "api/rx_meta_api.h"
#include "model/rx_model_algorithms.h"
#include "lib/rx_ser_json.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "model/rx_model_algorithms.h"


namespace rx_internal {

namespace enterprise {
const char* c_json_content_type = "application/json";

// Class rx_internal::enterprise::json_enterprise_interface 

json_enterprise_interface::json_enterprise_interface()
{
}


json_enterprise_interface::~json_enterprise_interface()
{
}



void json_enterprise_interface::sync_read (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, const enterprise_args_t& args, rx_reference_ptr anchor)
{

	read_struct_data read_data;

	runtime_value_type val_type = runtime_value_type::simple_runtime_value;
	if (args.count("full"))
		val_type = runtime_value_type::full_runtime_value;
	if (args.count("persist"))
		val_type = runtime_value_type::persistent_runtime_value;

	read_data.callback = read_struct_callback_t(anchor, [this, trans_id, args = args](rx_result result, data::runtime_values_data data) mutable
	{
		uint32_t http_result = 501;
		string_type content;
		if (result)
		{
			if (args.count("pretty") > 0)
			{
				serialization::pretty_json_writer writer;
				writer.write_header(STREAMING_TYPE_MESSAGE, 0);

				writer.write_bool("success", true);
				writer.write_init_values("data", data);
				writer.write_footer();

				content = writer.get_string();
				http_result = 200;
			}
			else
			{
				serialization::json_writer writer;
				writer.write_header(STREAMING_TYPE_MESSAGE, 0);

				writer.write_bool("success", true);
				writer.write_init_values("data", data);
				writer.write_footer();

				content = writer.get_string();
				http_result = 200;
			}
		}
		else
		{
			content = create_error_response(2, result.errors_line());
			http_result = 400;
		}


		/*string_type temp = "Content type is:"s + string_type(c_json_content_type);
		OutputDebugStringA(temp.c_str());*/

		finish_read_transaction(trans_id, http_result, c_json_content_type, content);
	});

	read_data.type = val_type;
	item->read_struct(sub_path, std::move(read_data));
}

void json_enterprise_interface::sync_read (uint64_t trans_id, rx_directory_ptr dir, const rx_node_id& type_id, rx_item_type type_type, const enterprise_args_t& args, rx_reference_ptr anchor)
{

	read_struct_data read_data;

	runtime_value_type val_type = runtime_value_type::simple_runtime_value;
	if (args.count("full"))
		val_type = runtime_value_type::full_runtime_value;
	if (args.count("persist"))
		val_type = runtime_value_type::persistent_runtime_value;

	std::set<rx_node_id> included_by_type;

	rx_result result;

	uint32_t http_result = 501;
	string_type content;

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
		default:
			;
	}



	std::vector<platform_item_ptr> items;

	recursive_list_directory(dir, items, included_by_type);

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
	else
	{
		http_result = 200;
	}
	content = writer.get_string();

	finish_read_transaction(trans_id, http_result, c_json_content_type, content);

}

void json_enterprise_interface::sync_write (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, string_view_type data, const enterprise_args_t& args, rx_reference_ptr anchor)
{
	string_type path(sub_path), str_val;

	if (path.empty())
	{
		string_type result = create_error_response(2, RX_INVALID_PATH);
		finish_write_transaction(trans_id, 404, c_json_content_type, result);
	}


	serialization::json_reader reader;
	if (reader.parse_data(string_type(data)))
	{
		data::runtime_values_data vals;
		if (reader.read_init_values(nullptr, vals))
		{
			auto us1 = rx_get_us_ticks();
			item->write_value(path, false, std::move(vals), write_result_callback_t(anchor
				, [this, trans_id, args = args, us1](uint32_t signal_level, rx_result&& result) mutable
				{
					uint32_t http_result = 501;
					string_type content;

					if (result)
					{
						if (args.count("pretty") > 0)
						{
							serialization::pretty_json_writer writer;
							writer.write_header(STREAMING_TYPE_MESSAGE, 0);

							writer.write_bool("success", true);
							writer.write_footer();

							content = writer.get_string();
							http_result = 200;
						}
						else
						{
							serialization::json_writer writer;
							writer.write_header(STREAMING_TYPE_MESSAGE, 0);

							writer.write_bool("success", true);
							writer.write_footer();

							content = writer.get_string();
							http_result = 200;
						}
					}
					else
					{
						content = create_error_response(3, result.errors_line());
						http_result = 400;
					}
					finish_write_transaction(trans_id, http_result, c_json_content_type, content);
				}));
		}
		else
		{

			string_type result = create_error_response(400, "JSON reading values error:"s + reader.get_error());
			finish_write_transaction(trans_id, 501, c_json_content_type, result);
		}
	}
	else
	{
		string_type result = create_error_response(400, "JSON parsing error:"s + reader.get_error());
		finish_write_transaction(trans_id, 501, c_json_content_type, result);
	}
}

void json_enterprise_interface::sync_execute (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, string_view_type data, const enterprise_args_t& args, rx_reference_ptr anchor)
{
	string_type path(sub_path), str_val;

	if (path.empty())
	{
		string_type result = create_error_response(2, RX_INVALID_PATH);
		finish_write_transaction(trans_id, 404, c_json_content_type, result);
	}


	serialization::json_reader reader;
	if (reader.parse_data(string_type(data)))
	{
		data::runtime_values_data vals;
		if (reader.read_init_values(nullptr, vals))
		{
			auto us1 = rx_get_us_ticks();
			item->execute_method(path, false, std::move(vals), named_execute_method_callback_t(anchor
				, [this, trans_id, args = args, us1](uint32_t signal_level, rx_result&& result, data::runtime_values_data data) mutable
				{
					uint32_t http_result = 501;
					string_type content;

					if (result)
					{
						if (args.count("pretty") > 0)
						{
							serialization::pretty_json_writer writer;
							writer.write_header(STREAMING_TYPE_MESSAGE, 0);

							writer.write_bool("success", true);
							writer.write_init_values("data", data);
							writer.write_footer();

							content = writer.get_string();
							http_result = 200;
						}
						else
						{
							serialization::json_writer writer;
							writer.write_header(STREAMING_TYPE_MESSAGE, 0);

							writer.write_bool("success", true);
							writer.write_init_values("data", data);
							writer.write_footer();

							content = writer.get_string();
							http_result = 200;
						}
					}
					else
					{
						if (result.errors_line() == RX_NOT_SUPPORTED)
							http_result = 405;
						else
							http_result = 400;
						content = create_error_response(3, result.errors_line());

					}
					finish_execute_transaction(trans_id, http_result, c_json_content_type, content);
				}));
		}
		else
		{

			string_type result = create_error_response(400, "JSON reading values error:"s + reader.get_error());
			finish_execute_transaction(trans_id, 501, c_json_content_type, result);
		}
	}
	else
	{
		string_type result = create_error_response(400, "JSON parsing error:"s + reader.get_error());
		finish_execute_transaction(trans_id, 501, c_json_content_type, result);
	}
}

string_type json_enterprise_interface::create_error_response (int code, string_type message)
{
	serialization::pretty_json_writer writer;
	writer.write_header(STREAMING_TYPE_MESSAGE, 0);
	writer.write_bool("success", false);
	writer.start_object("error");
	{
		writer.write_int("code", code);
		if (!message.empty())
			writer.write_string("msg", message.c_str());
	}
	writer.end_object();
	writer.write_footer();

	return writer.get_string();
}

string_view_type json_enterprise_interface::get_content_type ()
{
	return c_json_content_type;
}

string_view_type json_enterprise_interface::get_name ()
{
	return "json";
}

rx_result json_enterprise_interface::recursive_list_directory (rx_directory_ptr item, std::vector<platform_item_ptr>& rt_items, const std::set<rx_node_id>& by_type)
{
	platform_directories_type dirs;
	platform_items_type sub_items;
	item->list_content(dirs, sub_items, "");
	for (auto& one : dirs)
	{
		recursive_list_directory(one, rt_items, by_type);
	}
	auto end_it = by_type.end();
	for (auto& one : sub_items)
	{
		if (rx_is_runtime(one.get_type()) && (by_type.empty() || by_type.find(one.get_meta().id) != end_it))
		{
			auto one_item = rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().get_item(one.get_meta().id);
			if (one_item)
				rt_items.push_back(std::move(one_item));
		}
	}
	return true;
}


} // namespace enterprise
} // namespace rx_internal

