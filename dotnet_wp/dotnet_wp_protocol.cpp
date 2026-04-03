

/****************************************************************************
*
*  dotnet_wp\dotnet_wp_protocol.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


// dotnet_wp_protocol
#include "dotnet_wp/dotnet_wp_protocol.h"


extern "C" {

	rx_protocol_result_t pipe_header_function(uint8_t* header, size_t* size)
	{
		struct dotnet_wp_message_header* head = (dotnet_wp_message_header*)header;
		if (head->known != RX_DOTNET_KNOWN)
		{
			return RX_PROTOCOL_PARSING_ERROR;
		}
		size_t ret = head->size;
		*size = ret;
		return RX_PROTOCOL_OK;
	}

} // extern "C"


namespace rx_internal {

namespace dotnet {

// Class rx_internal::dotnet::worker_initialized_message 


rx_result worker_initialized_message::serialize (io::rx_io_buffer& buffer) const
{
	rx_result result;
	if (action_result)
	{
		result = buffer.write_to_buffer(0);
		if (!result)
			return result;
	}
	else
	{
		result = buffer.write_to_buffer((uint32_t)action_result.errors().size());
		if (!result)
			return result;
		for(const auto& err : action_result.errors())
		{
			result = buffer.write_string(err);
			if (!result)
				return result;
		}
	}
	result = buffer.write_string(hosted_version.empty() ? "" : hosted_version.c_str());
	if (!result)
		return result;
	result = buffer.write_string(dotnet_host_version.empty() ? "" : dotnet_host_version.c_str());
	if (!result)
		return result;
	return true;
}

rx_result worker_initialized_message::deserialize (io::rx_const_io_buffer& buffer)
{
	uint32_t err_size = 0;
	string_type errMsg;
	auto result = buffer.read_from_buffer(err_size);
	if(!result)
		return result;
	
	for(uint32_t i = 0; i < err_size; ++i)
	{
		result = buffer.read_string(errMsg);
		if(!result)
			return result;
		action_result.register_error(errMsg);
	}
	result = buffer.read_string(hosted_version);
	if(!result)
		return result;
	result = buffer.read_string(dotnet_host_version);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::write_log_message 


rx_result write_log_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer(type);
	if (!result)
		return result;
	result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	result = buffer.write_string(source.empty() ? "" : source.c_str());
	if (!result)
		return result;
	result = buffer.write_to_buffer(level);
	if (!result)
		return result;
	result = buffer.write_string(code.empty() ? "" : code.c_str());
	if (!result)
		return result;
	result = buffer.write_string(message.empty() ? "" : message.c_str());
	if (!result)
		return result;

	return true;
}

rx_result write_log_message::deserialize (io::rx_const_io_buffer& buffer)
{
	auto result = buffer.read_from_buffer(type);
	if (!result)
		return result;
	result = buffer.read_string(library);
	if (!result)
		return result;
	result = buffer.read_string(source);
	if (!result)
		return result;
	result = buffer.read_from_buffer(level);
	if (!result)
		return result;
	result = buffer.read_string(code);
	if (!result)
		return result;
	result = buffer.read_string(message);
	if (!result)
		return result;

	return true;
}


// Class rx_internal::dotnet::build_types_message 


rx_result build_types_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	return true;
}

rx_result build_types_message::deserialize (io::rx_const_io_buffer& buffer)
{
	auto result = buffer.read_string(library);
	if (!result)
		return result;

	return true;
}


// Class rx_internal::dotnet::build_types_response 


rx_result build_types_response::serialize (io::rx_io_buffer& buffer) const
{
	rx_result result;
	if (action_result)
	{
		result = buffer.write_to_buffer(0);
		if (!result)
			return result;
	}
	else
	{
		result = buffer.write_to_buffer((uint32_t)action_result.errors().size());
		if (!result)
			return result;
		for (const auto& err : action_result.errors())
		{
			result = buffer.write_string(err);
			if (!result)
				return result;
		}
	}
	return true;
}

rx_result build_types_response::deserialize (io::rx_const_io_buffer& buffer)
{
	uint32_t err_size = 0;
	string_type errMsg;
	auto result = buffer.read_from_buffer(err_size);
	if (!result)
		return result;

	for (uint32_t i = 0; i < err_size; ++i)
	{
		result = buffer.read_string(errMsg);
		if (!result)
			return result;
		action_result.register_error(errMsg);
	}
	return true;
}


// Class rx_internal::dotnet::build_one_type_message 


rx_result build_one_type_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer((int)type);
	if (!result)
		return result;
	result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	result = buffer.write_node_id(id);
	if (!result)
		return result;
	result = buffer.write_node_id(parent);
	if (!result)
		return result;
	result = buffer.write_string(name.empty() ? "" : name.c_str());
	if (!result)
		return result;
	result = buffer.write_string(path.empty() ? "" : path.c_str());
	if (!result)
		return result;
	result = buffer.write_to_buffer(version);
	if (!result)
		return result;
	result = buffer.write_to_buffer(flags);
	if (!result)
		return result;
	result = buffer.write_string(def.empty() ? "" : def.c_str());
	if (!result)
		return result;
	result = buffer.write_to_buffer(ownType);
	if (!result)
		return result;
	result = buffer.write_string(conns.empty() ? "" : conns.c_str());
	if (!result)
		return result;
	result = buffer.write_string(code_info.empty() ? "" : code_info.c_str());
	if (!result)
		return result;
	return true;
}

rx_result build_one_type_message::deserialize (io::rx_const_io_buffer& buffer)
{
	int type_int = 0;
	auto result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_string(library);
	if (!result)
		return result;
	result = buffer.read_node_id(id);
	if (!result)
		return result;
	result = buffer.read_node_id(parent);
	if (!result)
		return result;
	result = buffer.read_string(name);
	if (!result)
		return result;
	result = buffer.read_string(path);
	if (!result)
		return result;
	result = buffer.read_from_buffer(version);
	if (!result)
		return result;
	result = buffer.read_from_buffer(flags);
	if (!result)
		return result;
	result = buffer.read_string(def);
	if (!result)
		return result;
	result = buffer.read_from_buffer(ownType);
	if (!result)
		return result;
	result = buffer.read_string(conns);
	if (!result)
		return result;
	result = buffer.read_string(code_info);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::build_one_type_response 


rx_result build_one_type_response::serialize (io::rx_io_buffer& buffer) const
{
	rx_result result;
	if (action_result)
	{
		result = buffer.write_to_buffer(0);
		if (!result)
			return result;
	}
	else
	{
		result = buffer.write_to_buffer((uint32_t)action_result.errors().size());
		if (!result)
			return result;
		for (const auto& err : action_result.errors())
		{
			result = buffer.write_string(err);
			if (!result)
				return result;
		}
	}
	return true;
}

rx_result build_one_type_response::deserialize (io::rx_const_io_buffer& buffer)
{
	uint32_t err_size = 0;
	string_type errMsg;
	auto result = buffer.read_from_buffer(err_size);
	if (!result)
		return result;

	for (uint32_t i = 0; i < err_size; ++i)
	{
		result = buffer.read_string(errMsg);
		if (!result)
			return result;
		action_result.register_error(errMsg);
	}
	return true;
}


// Class rx_internal::dotnet::dotnet_start_message 


rx_result dotnet_start_message::serialize (io::rx_io_buffer& buffer) const
{
	return true;
}

rx_result dotnet_start_message::deserialize (io::rx_const_io_buffer& buffer)
{
	return true;
}


// Class rx_internal::dotnet::build_runtime_message 


rx_result build_runtime_message::serialize (io::rx_io_buffer& buffer) const
{

	auto result = buffer.write_to_buffer((int)type);
	if (!result)
		return result;
	result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	result = buffer.write_node_id(id);
	if (!result)
		return result;
	result = buffer.write_node_id(parent);
	if (!result)
		return result;
	result = buffer.write_string(name.empty() ? "" : name.c_str());
	if (!result)
		return result;
	result = buffer.write_string(path.empty() ? "" : path.c_str());
	if (!result)
		return result;
	result = buffer.write_to_buffer(version);
	if (!result)
		return result;
	result = buffer.write_to_buffer(flags);
	if (!result)
		return result;
	result = buffer.write_string(def.empty() ? "" : def.c_str());
	if (!result)
		return result;
	result = buffer.write_to_buffer(instance);
	if (!result)
		return result;
	return true;
}

rx_result build_runtime_message::deserialize (io::rx_const_io_buffer& buffer)
{
	int type_int = 0;
	auto result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_string(library);
	if (!result)
		return result;
	result = buffer.read_node_id(id);
	if (!result)
		return result;
	result = buffer.read_node_id(parent);
	if (!result)
		return result;
	result = buffer.read_string(name);
	if (!result)
		return result;
	result = buffer.read_string(path);
	if (!result)
		return result;
	result = buffer.read_from_buffer(version);
	if (!result)
		return result;
	result = buffer.read_from_buffer(flags);
	if (!result)
		return result;
	result = buffer.read_string(def);
	if (!result)
		return result;
	result = buffer.read_from_buffer(instance);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::build_runtime_response 


rx_result build_runtime_response::serialize (io::rx_io_buffer& buffer) const
{
	rx_result result;
	if (action_result)
	{
		result = buffer.write_to_buffer(0);
		if (!result)
			return result;
	}
	else
	{
		result = buffer.write_to_buffer((uint32_t)action_result.errors().size());
		if (!result)
			return result;
		for (const auto& err : action_result.errors())
		{
			result = buffer.write_string(err);
			if (!result)
				return result;
		}
	}
	return true;
}

rx_result build_runtime_response::deserialize (io::rx_const_io_buffer& buffer)
{
	uint32_t err_size = 0;
	string_type errMsg;
	auto result = buffer.read_from_buffer(err_size);
	if (!result)
		return result;

	for (uint32_t i = 0; i < err_size; ++i)
	{
		result = buffer.read_string(errMsg);
		if (!result)
			return result;
		action_result.register_error(errMsg);
	}
	return true;
}


// Class rx_internal::dotnet::dotnet_binding_runtime_message 


rx_result dotnet_binding_runtime_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer((int)type);
	if (!result)
		return result;
	result = buffer.write_node_id(id);
	if (!result)
		return result;
	result = buffer.write_string(path.empty() ? "" : path.c_str());
	if (!result)
		return result;
	result = buffer.write_node_id(type_id);
	if (!result)
		return result;
	result = buffer.write_to_buffer(ptr);
	if (!result)
		return result;
	return true;
}

rx_result dotnet_binding_runtime_message::deserialize (io::rx_const_io_buffer& buffer)
{
	int type_int = 0;
	auto result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_node_id(id);
	if (!result)
		return result;
	result = buffer.read_string(path);
	if (!result)
		return result;
	result = buffer.read_node_id(type_id);
	if (!result)
		return result;
	result = buffer.read_from_buffer(ptr);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::dotnet_unbind_runtime_message 


// Class rx_internal::dotnet::dotnet_bind_runtime_message 


// Class rx_internal::dotnet::dotnet_value_changed_message 


rx_result dotnet_value_changed_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer(idx);
	if (!result)
		return result;
	result = buffer.write_value(val);
	if (!result)
		return result;
	result = buffer.write_to_buffer((int)type);
	if (!result)
		return result;
	result = buffer.write_to_buffer(whose);
	if (!result)
		return result;
	return true;
}

rx_result dotnet_value_changed_message::deserialize (io::rx_const_io_buffer& buffer)
{
	auto result = buffer.read_from_buffer(idx);
	if (!result)
		return result;
	result = buffer.read_value(val);
	if (!result)
		return result;
	int type_int = 0;
	result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_from_buffer(whose);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::dotnet_initial_values_message 


rx_result dotnet_initial_values_message::serialize (io::rx_io_buffer& buffer) const
{
	int count = (int)names.size();
	auto result = buffer.write_to_buffer(count);
	if (!result)
		return result;
	for (int i = 0; i < count; ++i)
	{
		result = buffer.write_string(names[i]);
		if (!result)
			return result;
	}
	count = (int)values.size();
	result = buffer.write_to_buffer(count);
	if (!result)
		return result;
	for (int i = 0; i < count; ++i)
	{
		result = buffer.write_value(values[i]);
		if (!result)
			return result;
	}
	int type_int = (int)type;
	result = buffer.write_to_buffer(type_int);
	if (!result)
		return result;
	result = buffer.write_to_buffer(whose);
	if (!result)
		return result;
	return true;
}

rx_result dotnet_initial_values_message::deserialize (io::rx_const_io_buffer& buffer)
{
	int count = 0;
	auto result = buffer.read_from_buffer(count);
	if(!result)
		return result;
	for (int i = 0; i < count; ++i)
	{
		string_type temp;
		result = buffer.read_string(temp);
		if (!result)
			return result;
		names.push_back(std::move(temp));
	}
	result = buffer.read_from_buffer(count);
	if (!result)
		return result;
	for (int i = 0; i < count; ++i)
	{
		rx_simple_value val;
		result = buffer.read_value(val);
		if (!result)
			return result;
		values.push_back(std::move(val));
	}
	int type_int = 0;
	result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_from_buffer(whose);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::wp_write_value_message 


rx_result wp_write_value_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer((int)type);
	if (!result)
		return result;
	result = buffer.write_to_buffer(inst);
	if (!result)
		return result;
	result = buffer.write_to_buffer(idx);
	if (!result)
		return result;
	result = buffer.write_value(val);
	if (!result)
		return result;
	return true;
}

rx_result wp_write_value_message::deserialize (io::rx_const_io_buffer& buffer)
{
	int type_int = 0;
	auto result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_from_buffer(inst);
	if (!result)
		return result;
	result = buffer.read_from_buffer(idx);
	if (!result)
		return result;
	result = buffer.read_value(val);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::wp_result_response 


rx_result wp_result_response::serialize (io::rx_io_buffer& buffer) const
{
	rx_result result;
	if (action_result)
	{
		result = buffer.write_to_buffer(0);
		if (!result)
			return result;
	}
	else
	{
		result = buffer.write_to_buffer((uint32_t)action_result.errors().size());
		if (!result)
			return result;
		for (const auto& err : action_result.errors())
		{
			result = buffer.write_string(err);
			if (!result)
				return result;
		}
	}
	return true;
}

rx_result wp_result_response::deserialize (io::rx_const_io_buffer& buffer)
{
	uint32_t err_size = 0;
	string_type errMsg;
	auto result = buffer.read_from_buffer(err_size);
	if (!result)
		return result;

	for (uint32_t i = 0; i < err_size; ++i)
	{
		result = buffer.read_string(errMsg);
		if (!result)
			return result;
		action_result.register_error(errMsg);
	}
	return true;
}


// Class rx_internal::dotnet::dotnet_execute_method_message 


rx_result dotnet_execute_method_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer(inst);
	if (!result)
		return result;
	result = buffer.write_string(method.empty() ? "" : method.c_str());
	if (!result)
		return result;
	result = buffer.write_string(value.empty() ? "" : value.c_str());
	if (!result)
		return result;

	return true;				
}

rx_result dotnet_execute_method_message::deserialize (io::rx_const_io_buffer& buffer)
{
	auto result = buffer.read_from_buffer(inst);
	if (!result)
		return result;
	result = buffer.read_string(method);
	if (!result)
		return result;
	result = buffer.read_string(value);
	if (!result)
		return result;

	return true;
}


// Class rx_internal::dotnet::dotnet_execute_method_response 


rx_result dotnet_execute_method_response::serialize (io::rx_io_buffer& buffer) const
{
	rx_result result;
	if (action_result)
	{
		result = buffer.write_to_buffer(0);
		if (!result)
			return result;
	}
	else
	{
		result = buffer.write_to_buffer((uint32_t)action_result.errors().size());
		if (!result)
			return result;
		for (const auto& err : action_result.errors())
		{
			result = buffer.write_string(err);
			if (!result)
				return result;
		}
	}
	result = buffer.write_string(result_value.empty() ? "" : result_value.c_str());
	if (!result)
		return result;
	return true;
}

rx_result dotnet_execute_method_response::deserialize (io::rx_const_io_buffer& buffer)
{
	uint32_t err_size = 0;
	string_type errMsg;
	auto result = buffer.read_from_buffer(err_size);
	if (!result)
		return result;

	for (uint32_t i = 0; i < err_size; ++i)
	{
		result = buffer.read_string(errMsg);
		if (!result)
			return result;
		action_result.register_error(errMsg);
	}
	result = buffer.read_string(result_value);
	if (!result)
		return result;

	return true;
}


// Class rx_internal::dotnet::delete_one_type_message 


rx_result delete_one_type_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer((int)type);
	if (!result)
		return result;
	result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	result = buffer.write_node_id(id);
	if (!result)
		return result;
	return true;
}

rx_result delete_one_type_message::deserialize (io::rx_const_io_buffer& buffer)
{
	int type_int = 0;
	auto result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_string(library);
	if (!result)
		return result;
	result = buffer.read_node_id(id);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::reload_message 


rx_result reload_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	return true;
}

rx_result reload_message::deserialize (io::rx_const_io_buffer& buffer)
{
	auto result = buffer.read_string(library);
	if (!result)
		return result;

	return true;
}


// Class rx_internal::dotnet::unload_message 


rx_result unload_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	return true;
}

rx_result unload_message::deserialize (io::rx_const_io_buffer& buffer)
{
	auto result = buffer.read_string(library);
	if (!result)
		return result;

	return true;
}


// Class rx_internal::dotnet::delete_one_runtime_message 


rx_result delete_one_runtime_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer((int)type);
	if (!result)
		return result;
	result = buffer.write_string(library.empty() ? "" : library.c_str());
	if (!result)
		return result;
	result = buffer.write_node_id(id);
	if (!result)
		return result;
	return true;
}

rx_result delete_one_runtime_message::deserialize (io::rx_const_io_buffer& buffer)
{
	int type_int = 0;
	auto result = buffer.read_from_buffer(type_int);
	if (!result)
		return result;
	type = (rx_item_type)type_int;
	result = buffer.read_string(library);
	if (!result)
		return result;
	result = buffer.read_node_id(id);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::dotnet_fire_event_message 


rx_result dotnet_fire_event_message::serialize (io::rx_io_buffer& buffer) const
{
	auto result = buffer.write_to_buffer(inst);
	if (!result)
		return result;
	result = buffer.write_string(value.empty() ? "" : value.c_str());
	if (!result)
		return result;
	result = buffer.write_string(queue.empty() ? "" : queue.c_str());
	if (!result)
		return result;
	result = buffer.write_to_buffer(state);
	if (!result)
		return result;
	result = buffer.write_to_buffer(remove);
	if (!result)
		return result;
	return true;
}

rx_result dotnet_fire_event_message::deserialize (io::rx_const_io_buffer& buffer)
{
	auto result = buffer.read_from_buffer(inst);
	if (!result)
		return result;
	result = buffer.read_string(value);
	if (!result)
		return result;
	result = buffer.read_string(queue);
	if (!result)
		return result;
	result = buffer.read_from_buffer(state);
	if (!result)
		return result;
	result = buffer.read_from_buffer(remove);
	if (!result)
		return result;
	return true;
}


// Class rx_internal::dotnet::dotnet_stop_message 


rx_result dotnet_stop_message::serialize (io::rx_io_buffer& buffer) const
{
	return true;
}

rx_result dotnet_stop_message::deserialize (io::rx_const_io_buffer& buffer)
{
	return true;
}


// Class rx_internal::dotnet::wp_void_response 


rx_result wp_void_response::serialize (io::rx_io_buffer& buffer) const
{
	return true;
}

rx_result wp_void_response::deserialize (io::rx_const_io_buffer& buffer)
{
	return true;
}


} // namespace dotnet
} // namespace rx_internal

