

/****************************************************************************
*
*  system\meta\rx_meta_data.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_meta_support
#include "system/meta/rx_meta_support.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"

#include "system/meta/rx_obj_types.h"
#include "system/server/rx_server.h"
#include "api/rx_platform_api.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {
string_type rx_item_type_name(rx_item_type type)
{
	switch (type)
	{
	case rx_item_type::rx_directory:
		return RX_CPP_DIRECORY_TYPE_NAME;

	case rx_item_type::rx_application:
		return RX_CPP_APPLICATION_TYPE_NAME;
	case rx_item_type::rx_application_type:
		return RX_CPP_APPLICATION_CLASS_TYPE_NAME;
	case rx_item_type::rx_domain:
		return RX_CPP_DOMAIN_TYPE_NAME;
	case rx_item_type::rx_domain_type:
		return RX_CPP_DOMAIN_CLASS_TYPE_NAME;
	case rx_item_type::rx_object:
		return RX_CPP_OBJECT_TYPE_NAME;
	case rx_item_type::rx_object_type:
		return RX_CPP_OBJECT_CLASS_TYPE_NAME;
	case rx_item_type::rx_port:
		return RX_CPP_PORT_TYPE_NAME;
	case rx_item_type::rx_port_type:
		return RX_CPP_PORT_CLASS_TYPE_NAME;

	case rx_item_type::rx_struct_type:
		return RX_CPP_STRUCT_CLASS_TYPE_NAME;
	case rx_item_type::rx_variable_type:
		return RX_CPP_VARIABLE_CLASS_TYPE_NAME;
	case rx_item_type::rx_source_type:
		return RX_CPP_SOURCE_CLASS_TYPE_NAME;
	case rx_item_type::rx_filter_type:
		return RX_CPP_FILTER_CLASS_TYPE_NAME;
	case rx_item_type::rx_event_type:
		return RX_CPP_EVENT_CLASS_TYPE_NAME;
	case rx_item_type::rx_mapper_type:
		return RX_CPP_MAPPER_CLASS_TYPE_NAME;

	case rx_item_type::rx_program:
		return RX_CPP_PROGRAM_TYPE_NAME;
	case rx_item_type::rx_method:
		return RX_CPP_METHOD_TYPE_NAME;

	case rx_item_type::rx_relation_type:
		return RX_CPP_RELATION_CLASS_TYPE_NAME;

	default:
		return string_type();
	}
}

string_type rx_runtime_attribute_type_name(rx_attribute_type type)
{
	switch (type)
	{
		case struct_attribute_type:
			return RX_CPP_STRUCT_TYPE_NAME;
		case variable_attribute_type:
			return RX_CPP_VARIABLE_TYPE_NAME;
		case source_attribute_type:
			return RX_CPP_SOURCE_TYPE_NAME;
		case filter_attribute_type:
			return RX_CPP_FILTER_TYPE_NAME;
		case event_attribute_type:
			return RX_CPP_EVENT_TYPE_NAME;
		case mapper_attribute_type:
			return RX_CPP_MAPPER_TYPE_NAME;
		case const_attribute_type:
			return RX_CONST_VALUE_TYPE_NAME;
		case value_attribute_type:
			return RX_VALUE_TYPE_NAME;
		case relation_attribute_type:
			return RX_CPP_RELATION_TYPE_NAME;
		case relation_target_attribute_type:
			return RX_CPP_RELATION_TARGET_TYPE_NAME;
		default:
			return "Unknown attribute type!!!";
	}
}
rx_item_type rx_parse_type_name(const string_type name)
{
	// TODO might do this faster with hash stuff
	if (name == RX_CPP_DIRECORY_TYPE_NAME)
		return rx_item_type::rx_directory;
	else if (name == RX_CPP_APPLICATION_TYPE_NAME)
		return rx_item_type::rx_application;
	else if (name == RX_CPP_APPLICATION_CLASS_TYPE_NAME)
		return rx_item_type::rx_application_type;
	else if (name == RX_CPP_DOMAIN_TYPE_NAME)
		return rx_item_type::rx_domain;
	else if (name == RX_CPP_DOMAIN_CLASS_TYPE_NAME)
		return rx_item_type::rx_domain_type;
	else if (name == RX_CPP_OBJECT_TYPE_NAME)
		return rx_item_type::rx_object;
	else if (name == RX_CPP_OBJECT_CLASS_TYPE_NAME)
		return rx_item_type::rx_object_type;
	else if (name == RX_CPP_PORT_TYPE_NAME)
		return rx_item_type::rx_port;
	else if (name == RX_CPP_PORT_CLASS_TYPE_NAME)
		return rx_item_type::rx_port_type;

	else if (name == RX_CPP_STRUCT_CLASS_TYPE_NAME)
		return rx_item_type::rx_struct_type;
	else if (name == RX_CPP_VARIABLE_CLASS_TYPE_NAME)
		return rx_item_type::rx_variable_type;
	else if (name == RX_CPP_SOURCE_CLASS_TYPE_NAME)
		return rx_item_type::rx_source_type;
	else if (name == RX_CPP_FILTER_CLASS_TYPE_NAME)
		return rx_item_type::rx_filter_type;
	else if (name == RX_CPP_EVENT_CLASS_TYPE_NAME)
		return rx_item_type::rx_event_type;
	else if (name == RX_CPP_MAPPER_CLASS_TYPE_NAME)
		return rx_item_type::rx_mapper_type;

	else if (name == RX_CPP_PROGRAM_TYPE_NAME)
		return rx_item_type::rx_program;
	else if (name == RX_CPP_METHOD_TYPE_NAME)
		return rx_item_type::rx_method;

	else if (name == RX_CPP_RELATION_CLASS_TYPE_NAME)
		return rx_item_type::rx_relation_type;

	return rx_item_type::rx_invalid_type;
}


namespace meta {
/*
template<typename T>
rx_result_with<typename T::RTypePtr> deserialize_runtime(const meta_data& meta, base_meta_reader& stream, uint8_t type)
{
	using runtime_ptr = typename T::RTypePtr;

	runtime_ptr ret;
	auto result = ret->deserialize(stream, type);
	if (result)
	{
		ret->meta_info() = meta;
	}
	return ret;
}*/

// Class rx_platform::meta::meta_data 

meta_data::meta_data()
      : version(RX_INITIAL_ITEM_VERSION)
	, attributes(namespace_item_full_type_access)
{
}

meta_data::meta_data (const object_type_creation_data& type_data)
      : version(RX_INITIAL_ITEM_VERSION)
	, id(type_data.id)
	, name(type_data.name)
	, path(type_data.path)
	, parent(type_data.base_id)
	, created_time(rx_time::now())
	, modified_time(created_time)
	, attributes(type_data.attributes)
{
}

meta_data::meta_data (const type_creation_data& type_data)
      : version(RX_INITIAL_ITEM_VERSION)
	, id(type_data.id)
	, name(type_data.name)
	, path(type_data.path)
	, parent(type_data.base_id)
	, created_time(rx_time::now())
	, modified_time(created_time)
	, attributes(type_data.attributes)
{
}



rx_result meta_data::serialize_meta_data (base_meta_writer& stream, uint8_t type, rx_item_type object_type) const
{
	if (!stream.start_object("meta"))
		return false;
	if (stream.is_string_based())
	{
		if (!stream.write_string("type", rx_item_type_name(object_type)))
			return false;
	}
	else
	{
		if (!stream.write_byte("type", object_type))
			return false;
	}
	if (!stream.write_id("nodeId", id))
		return false;
	if (!stream.write_string("name", name.c_str()))
		return false;
	if (!stream.write_byte("attrs", (uint8_t)attributes))
		return false;
	if (!stream.write_id("superId", parent))
		return false;
	if (!stream.write_time("created", created_time))
		return false;
	if (!stream.write_time("modified", modified_time))
		return false;
	if (!stream.write_version("ver", version))
		return false;
	if (!stream.write_string("path", path))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

rx_result meta_data::deserialize_meta_data (base_meta_reader& stream, uint8_t type, rx_item_type& object_type)
{
	if (!stream.start_object("meta"))
		return false;
	if (stream.is_string_based())
	{
		string_type temp;
		if (!stream.read_string("type", temp))
			return false;
		object_type = rx_parse_type_name(temp);
		if (object_type >= rx_item_type::rx_first_invalid)
			return temp + " is invalid type name";
	}
	else
	{
		uint8_t temp;
		if (!stream.read_byte("type", temp))
			return false;
		if (temp >= rx_item_type::rx_first_invalid)
			return "Invalid type";
		object_type = (rx_item_type)temp;
	}
	if (!stream.read_id("nodeId", id))
		return false;
	if (!stream.read_string("name", name))
		return false;
	uint8_t temp_byte;
	if (!stream.read_byte("attrs", temp_byte))
		return false;
	attributes = (namespace_item_attributes)temp_byte;
	if (!stream.read_id("superId", parent))
		return false;
	if (!stream.read_time("created", created_time))
		return false;
	if (!stream.read_time("modified", modified_time))
		return false;
	if (!stream.read_string("path", path))
		return false;
	if (!stream.read_version("ver", version))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

rx_result meta_data::check_in (base_meta_reader& stream)
{
	return false;
}

rx_result meta_data::check_out (base_meta_writer& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_CHECKOUT, 0))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, STREAMING_TYPE_CHECKOUT);
*/

	if (!stream.write_footer())
		return false;

	return true;
}

bool meta_data::is_system () const
{
	return attributes & namespace_item_attributes::namespace_item_system_mask;
}

rx_result_with<rx_storage_ptr> meta_data::resolve_storage () const
{
	auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(path);
	if (dir)
	{
		return dir->resolve_storage();
	}
	else
		return "Unable to locate item's directory!";
}

values::rx_value meta_data::get_value () const
{
	values::rx_value temp;
	temp.assign_static(version, modified_time);
	return temp;
}

string_type meta_data::get_full_path () const
{
	if (!path.empty() && *path.rbegin() == RX_DIR_DELIMETER)
	{
		string_type ret(path);
		ret += name;
		return ret;

	}
	else
	{
		string_type ret(path);
		ret.reserve(ret.size() + name.size() + 1);
		ret += RX_DIR_DELIMETER;
		ret += name;
		return ret;
	}
}

void meta_data::get_full_path_with_buffer (string_type& buffer) const
{
	if (!path.empty() && *path.rbegin() == RX_DIR_DELIMETER)
	{
		buffer = path;
		buffer += name;

	}
	else
	{
		buffer = path;
		// we are not doing any memory stuff it's not our buffer
		//path.reserve(ret.size() + name_.size() + 1);
		buffer += RX_DIR_DELIMETER;
		buffer += name;
	}
}

rx_item_reference meta_data::create_item_reference ()
{
	if (id.is_null() && !path.empty())
		return rx_item_reference(get_full_path());
	else
		return rx_item_reference(id);
}

rx_item_reference meta_data::create_weak_item_reference (const string_array& dirs)
{
	string_type my_path = get_full_path();
	if (my_path.empty())
		return rx_item_reference();
	rx_directory_ptr my_directory = rx_gate::instance().get_root_directory()->get_sub_directory(path);
	size_t best_idx = 0;
	string_array best_splitted;
	size_t current_idx;
	string_type path;
	string_array my_spplitted, splitted;
	rx_split_string(path, my_spplitted, RX_DIR_DELIMETER);
	for (const auto& one_path : dirs)
	{
		auto one_directory = my_directory->get_sub_directory(one_path);
		if (one_directory)
		{
			path.clear();
			one_directory->fill_path(path);
			splitted.clear();
			rx_split_string(path, splitted, RX_DIR_DELIMETER);
			current_idx = 0;
			auto my_it = my_spplitted.begin();
			auto his_it = splitted.begin();
			while (my_it != my_spplitted.end() && his_it != splitted.end())
			{
				if (*my_it != *his_it)
					break;
				current_idx++;
				my_it++;
				his_it++;
			}
			if (current_idx > best_idx)
			{
				best_idx = current_idx;
				best_splitted = splitted;
			}
		}
	}
	if (best_idx > 0)
	{
		auto my_it = my_spplitted.begin();
		auto his_it = best_splitted.begin();
		// skip the common parts
		while (my_it != my_spplitted.end() && his_it != best_splitted.end())
		{
			if (*my_it != *his_it)
				break;
			my_it++;
			his_it++;
		}
		string_type relative_path(RX_DIR_DELIMETER_STR);
		for (; his_it != best_splitted.end(); his_it++)
		{
			relative_path += ".." RX_DIR_DELIMETER_STR;
		}
		for (; my_it != my_spplitted.begin(); my_it++)
		{
			relative_path += *my_it;
			relative_path += RX_DIR_DELIMETER;
		}
		relative_path += name;

		return rx_item_reference(relative_path);
	}
	else
	{
		return rx_item_reference(my_path);
	}
}

void meta_data::increment_version (bool full_ver)
{
	version++;
}

meta_data create_meta_for_new(const meta_data& proto)
{
	meta_data ret_data = proto;
	rx_time now(rx_time::now());
	ret_data.created_time = now;
	ret_data.modified_time = now;
	if(!proto.created_time.is_null())
		ret_data.created_time = proto.created_time;
	ret_data.version = RX_INITIAL_ITEM_VERSION;
	if (ret_data.version < proto.version)
		ret_data.version = proto.version;
	if (proto.id.is_null())
		ret_data.id = rx_node_id::generate_new();

	return ret_data;
}
} // namespace meta
} // namespace rx_platform

