

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
	if (args.count("_full"))
		val_type = runtime_value_type::full_runtime_value;
	if (args.count("_persist"))
		val_type = runtime_value_type::persistent_runtime_value;

	read_data.callback = read_struct_callback_t(anchor, [this, trans_id, args = args](rx_result result, data::runtime_values_data data) mutable
	{
		uint32_t http_result = 501;
		string_type content;
		if (result)
		{
			if (args.count("_pretty") > 0)
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
		finish_read_transaction(trans_id, http_result, c_json_content_type, content);
	});

	read_data.type = val_type;
	item->read_struct(sub_path, std::move(read_data));
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
					uint64_t us2 = rx_get_us_ticks() - us1;
					uint32_t http_result = 501;
					string_type content;

					if (result)
					{
						if (args.count("_pretty") > 0)
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
					uint64_t us2 = rx_get_us_ticks() - us1;
					uint32_t http_result = 501;
					string_type content;

					if (result)
					{
						if (args.count("_pretty") > 0)
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


} // namespace enterprise
} // namespace rx_internal

