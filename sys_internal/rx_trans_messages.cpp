

/****************************************************************************
*
*  sys_internal\rx_trans_messages.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_trans_messages
#include "sys_internal/rx_trans_messages.h"

#include "sys_internal/rx_internal_protocol.h"
#include "model/rx_model_dependencies.h"
#include "system/runtime/rx_runtime_holder.h"
#include "sys_internal/rx_namespace_algorithms.h"
#include "system/server/rx_directory_cache.h"


namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace set_messages {

template<typename T>
rx_result handle_path_id_stuff_templ(T& container, const std::map<string_type
	, std::pair<rx_node_id, rx_item_type> >& temp_map
	, const string_type path, size_t path_size, rx_item_type type)
{
	for (auto& one : container)
	{
		if (one->meta_info.path.empty())
		{
			one->meta_info.path = path;
		}
		else if (one->meta_info.path.size() < path_size || memcmp(path.c_str(), one->meta_info.path.c_str(), path_size) != 0)
		{
			return one->meta_info.get_full_path() + " is not located in folder " + path + ".";
		}

		if (one->meta_info.id.is_null())
		{
			auto it = temp_map.find(one->meta_info.get_full_path());
			if (it != temp_map.end() && it->second.second == type)
				one->meta_info.id = it->second.first;
		}
	}
	return true;
}

// Class rx_internal::rx_protocol::messages::set_messages::rx_update_directory_request_message 

string_type rx_update_directory_request_message::type_name = "updateDirReq";

uint16_t rx_update_directory_request_message::type_id = rx_update_directory_request_id;


rx_result rx_update_directory_request_message::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("path", path.c_str()))
		return stream.get_error();

	if (!stream.start_object("update"))
		return stream.get_error();

	if (!stream.write_uuid("checkout", update_data.checkout))
		return stream.get_error();

	if (!stream.write_bool("version", update_data.increment_version))
		return stream.get_error();

	if (!stream.write_bool("reinit", update_data.initialize_data))
		return stream.get_error();

	if (!stream.write_bool("release", update_data.release_forced))
		return stream.get_error();

	if (!stream.end_object())
		return stream.get_error();

	return true;
}

rx_result rx_update_directory_request_message::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("path", path))
		return stream.get_error();

	if (!stream.start_object("update"))
		return stream.get_error();

	rx_uuid_t temp;
	if (!stream.read_uuid("checkout", temp))
		return stream.get_error();
	update_data.checkout = temp;

	if (!stream.read_bool("version", update_data.increment_version))
		return stream.get_error();

	if (!stream.read_bool("reinit", update_data.initialize_data))
		return stream.get_error();

	if (!stream.read_bool("release", update_data.release_forced))
		return stream.get_error();

	if (!stream.end_object())
		return stream.get_error();

	auto result = content.deserialize("content", stream, STREAMING_TYPE_MESSAGE);
	if (!result)
		return result;

	return true;
}

message_ptr rx_update_directory_request_message::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	auto request_id = this->request_id;

	auto my_dir = ns::rx_directory_cache::instance().get_directory(ctx.active_path);
	if(!my_dir)
		return std::make_unique<error_message>("Invalid Path in context, something is really wrong here!!!"s, 99, request_id);

	auto dir = internal_ns::namespace_algorithms::get_or_create_direcotry(my_dir, path);

	auto brw_result = api::ns::rx_recursive_list_items(path, "", ctx);
	if (!brw_result)
	{
		return std::make_unique<error_message>(brw_result.errors(), 13, request_id);
	}
	
	auto result = handle_path_id_stuff(brw_result.value().items);
	if (!result)
	{
		return std::make_unique<error_message>(result.errors(), 14, request_id);
	}

	auto builder = rx_create_reference<rx_internal::model::transactions::local_dependecy_builder>();

	builder->add_config_part(content);

	for (auto& one : brw_result.value().items)
	{
		builder->add_query_result(api::query_result_detail(one.get_type(), one.get_meta()), true, false, false);
	}

	auto callback = rx_result_callback(ctx.object, [request_id, conn](rx_result&& result) mutable
		{
			if (result)
			{
				auto response = std::make_unique<rx_update_directory_response_message>();
				response->request_id = request_id;
				conn->data_processed(std::move(response));

			}
			else
			{
				auto ret_value = std::make_unique<error_message>(result, 14, request_id);
				conn->data_processed(std::move(ret_value));
			}

		});

	builder->apply_items(std::move(callback));	

	return message_ptr();
}

const string_type& rx_update_directory_request_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_update_directory_request_message::get_type_id ()
{
  return type_id;

}

rx_result rx_update_directory_request_message::handle_path_id_stuff (const ns::platform_items_type& items)
{
	// check for duplicates based on path!!!
	std::map<string_type, std::pair<rx_node_id, rx_item_type> > temp_map;
	for (auto& one : items)
		temp_map[one.get_meta().get_full_path()] = { one.get_meta().id, one.get_type() };

	size_t path_size = path.size();

	auto result = handle_path_id_stuff_templ(content.objects, temp_map, path, path_size, rx_object);
	if (!result)
		return result;
	result = handle_path_id_stuff_templ(content.domains, temp_map, path, path_size, rx_domain);
	if (!result)
		return result;
	result = handle_path_id_stuff_templ(content.ports, temp_map, path, path_size, rx_port);
	if (!result)
		return result;
	result = handle_path_id_stuff_templ(content.apps, temp_map, path, path_size, rx_application);
	if (!result)
		return result;

	return true;
}


// Class rx_internal::rx_protocol::messages::set_messages::rx_update_directory_response_message 

string_type rx_update_directory_response_message::type_name = "updateDirResp";

rx_message_type_t rx_update_directory_response_message::type_id = rx_update_directory_response_id;


rx_result rx_update_directory_response_message::serialize (base_meta_writer& stream) const
{
	return true;
}

rx_result rx_update_directory_response_message::deserialize (base_meta_reader& stream)
{
	return true;
}

const string_type& rx_update_directory_response_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_update_directory_response_message::get_type_id ()
{
  return type_id;

}


} // namespace set_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal

