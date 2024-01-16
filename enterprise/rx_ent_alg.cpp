

/****************************************************************************
*
*  enterprise\rx_ent_alg.cpp
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


// rx_ent_alg
#include "enterprise/rx_ent_alg.h"

#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"


namespace rx_internal {

namespace enterprise {

// Class rx_internal::enterprise::enterprise_handler 


void enterprise_handler::begin_read (uint64_t trans_id, string_view_type path, runtime_value_type type, enterprise_interface_callback* callback)
{
}

void enterprise_handler::begin_write (uint64_t trans_id, string_view_type path, runtime_value_type type, data::runtime_values_data data, enterprise_interface_callback* callback)
{
}

void enterprise_handler::begin_execute (uint64_t trans_id, string_view_type path, runtime_value_type type, data::runtime_values_data data, enterprise_interface_callback* callback)
{
}

void enterprise_handler::begin_query (uint64_t trans_id, string_view_type path, runtime_value_type type, data::runtime_values_data data, enterprise_interface_callback* callback)
{
}


// Class rx_internal::enterprise::enterprise_interface_callback 

enterprise_interface_callback::~enterprise_interface_callback()
{
}



// Class rx_internal::enterprise::std_enterprise_interface 

std_enterprise_interface::std_enterprise_interface()
      : g_last_id_(1)
{
	resolver_dirs_.add_paths({ "world" , "/" });
}


std_enterprise_interface::~std_enterprise_interface()
{
}



void std_enterprise_interface::begin_read (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, enterprise_callback* callback, rx_reference_ptr anchor)
{
	int result = 0;

	string_type path_val;
	if (!path.empty() && path[0] == '/')
		path_val = path.substr(1);
	else
		path_val = path;

	string_type whose;
	string_type item_path;
	rx_split_item_path(path_val, whose, item_path);
	if (!whose.empty())
	{
		rx_directory_resolver directories;
		directories.add_paths({ "/world" , "/" });

		auto resolve_result = api::ns::rx_resolve_reference(whose, directories);
		if (resolve_result)
		{
			auto item_result = model::algorithms::get_working_runtime(resolve_result.value());
			if (item_result)
			{
				auto item = item_result.move_value();
				auto item_executer = item->get_executer();

				uint64_t my_id = register_transaction(trans_id, callback, anchor);

				rx_post_function_to(item_executer, rx_reference_ptr(),
					[this, trans_id = my_id, args, sub_item = std::move(item_path)](platform_item_ptr data, rx_reference_ptr anchor)
				{

					sync_read(trans_id, std::move(data), sub_item, args, anchor);
				}, std::move(item), std::move(anchor));
			}
			else
			{
				resolve_result.register_error("Error finding runtime "s + whose);
				callback->read_complete(trans_id, 404, get_content_type()
					, create_error_response(1, item_result.errors_line()));
			}
		}
		else
		{
			resolve_result.register_error("Error resolving reference "s + whose);
			callback->read_complete(trans_id, 404, get_content_type()
				, create_error_response(1, resolve_result.errors_line()));

		}

	}
	else
	{
		callback->read_complete(trans_id, 400, get_content_type(), create_error_response(1, RX_INVALID_PATH));
	}
}

void std_enterprise_interface::begin_write (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor)
{
	string_type path_val;
	if (!path.empty() && path[0] == '/')
		path_val = path.substr(1);
	else
		path_val = path;

	string_type whose;
	string_type item_path;
	rx_split_item_path(path_val, whose, item_path);
	if (!whose.empty())
	{
		rx_directory_resolver directories;
		directories.add_paths({ "/world" , "/" });

		auto resolve_result = api::ns::rx_resolve_reference(whose, directories);
		if (resolve_result)
		{
			auto item_result = model::algorithms::get_working_runtime(resolve_result.value());
			if (item_result)
			{
				auto item = item_result.move_value();
				auto item_executer = item->get_executer();

				uint64_t my_id = register_transaction(trans_id, callback, anchor);
				string_type my_data(data);

				rx_post_function_to(item_executer, rx_reference_ptr(),
					[this, trans_id = my_id, my_data =std::move(my_data), args, sub_item = std::move(item_path)](platform_item_ptr data, rx_reference_ptr anchor)
				{

					sync_write(trans_id, std::move(data), sub_item, my_data, args, anchor);

				}, std::move(item), std::move(anchor));
			}
			else
			{
				resolve_result.register_error("Error finding runtime "s + whose);
				callback->write_complete(trans_id, 404, get_content_type()
					, create_error_response(1, item_result.errors_line()));
			}
		}
		else
		{
			resolve_result.register_error("Error resolving reference "s + whose);
			callback->write_complete(trans_id, 404, get_content_type()
				, create_error_response(1, resolve_result.errors_line()));

		}

	}
	else
	{
		callback->write_complete(trans_id, 400, get_content_type(), create_error_response(1, RX_INVALID_PATH));
	}
}

void std_enterprise_interface::begin_execute (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor)
{
	int result = 0;

	string_type path_val;
	if (!path.empty() && path[0] == '/')
		path_val = path.substr(1);
	else
		path_val = path;

	string_type whose;
	string_type item_path;
	rx_split_item_path(path_val, whose, item_path);
	if (!whose.empty())
	{
		rx_directory_resolver directories;
		directories.add_paths({ "/world" , "/" });

		auto resolve_result = api::ns::rx_resolve_reference(whose, directories);
		if (resolve_result)
		{
			auto item_result = model::algorithms::get_working_runtime(resolve_result.value());
			if (item_result)
			{
				auto item = item_result.move_value();
				auto item_executer = item->get_executer();

				uint64_t my_id = register_transaction(trans_id, callback, anchor);
				string_type my_data(data);

				rx_post_function_to(item_executer, rx_reference_ptr(),
					[this, trans_id = my_id, my_data = std::move(my_data), args, sub_item = std::move(item_path)](platform_item_ptr data, rx_reference_ptr anchor)
				{

					sync_execute(trans_id, std::move(data), sub_item, my_data, args, anchor);

				}, std::move(item), std::move(anchor));
			}
			else
			{
				resolve_result.register_error("Error finding runtime "s + whose);
				callback->execute_complete(trans_id, 404, get_content_type()
					, create_error_response(1, item_result.errors_line()));
			}
		}
		else
		{
			resolve_result.register_error("Error resolving reference "s + whose);
			callback->execute_complete(trans_id, 404, get_content_type()
				, create_error_response(1, resolve_result.errors_line()));

		}

	}
	else
	{
		callback->execute_complete(trans_id, 400, get_content_type(), create_error_response(1, RX_INVALID_PATH));
	}
}

void std_enterprise_interface::begin_query (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor)
{
	RX_ASSERT(false);// not supported yet!!!
}

void std_enterprise_interface::finish_read_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	enterprise_callback* callback = nullptr;
	uint64_t caller_id = 0;
	{
		std::scoped_lock _(transactions_lock_);
		auto it = transactions_.find(trans_id);
		if (it != transactions_.end())
		{
			callback = it->second.callback;
			caller_id = it->second.caller_id;
			transactions_.erase(it);
		}
	}
	if (callback)
		callback->read_complete(caller_id, result, cont_type, data);
}

void std_enterprise_interface::finish_write_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	enterprise_callback* callback = nullptr;
	uint64_t caller_id = 0;
	{
		std::scoped_lock _(transactions_lock_);
		auto it = transactions_.find(trans_id);
		if (it != transactions_.end())
		{
			callback = it->second.callback;
			caller_id = it->second.caller_id;
			transactions_.erase(it);
		}
	}
	if (callback)
		callback->write_complete(caller_id, result, cont_type, data);
}

void std_enterprise_interface::finish_execute_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	enterprise_callback* callback = nullptr;
	uint64_t caller_id = 0;
	{
		std::scoped_lock _(transactions_lock_);
		auto it = transactions_.find(trans_id);
		if (it != transactions_.end())
		{
			callback = it->second.callback;
			caller_id = it->second.caller_id;
			transactions_.erase(it);
		}
	}
	if (callback)
		callback->execute_complete(caller_id, result, cont_type, data);
}

void std_enterprise_interface::finish_query_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data)
{
	RX_ASSERT(false);// not supported yet!!!
}

uint64_t std_enterprise_interface::register_transaction (uint64_t trans_id, enterprise_callback* callback, rx_reference_ptr anchor)
{
	uint64_t new_id = g_last_id_++;
	std::scoped_lock _(transactions_lock_);
	trans_data_t data;
	data.callback = callback;
	data.caller_id = trans_id;
	data.started = rx_time::now();
	transactions_.emplace(new_id, std::move(data));
	return new_id;
}


} // namespace enterprise
} // namespace rx_internal



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	return "json";

#endif
