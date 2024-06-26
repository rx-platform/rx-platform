

/****************************************************************************
*
*  lib\rx_meta_data.cpp
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


// rx_meta_data
#include "lib/rx_meta_data.h"



namespace rx {
rx_uuid running_at_local = rx_uuid();
rx_uuid running_shared = rx_uuid_t{ 0xffffffff, 0xffff, 0xffff, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };

// Class rx::meta_data 

meta_data::meta_data()
      : version(RX_INITIAL_ITEM_VERSION)
	, attributes(namespace_item_full_type_access)
{
}



rx_result meta_data::serialize_meta_data (base_meta_writer& stream, uint8_t type, rx_item_type object_type) const
{
	if (!stream.start_object("meta"))
		return stream.get_error();
	if (stream.is_string_based())
	{
		if (!stream.write_string("type", rx_item_type_name(object_type).c_str()))
			return stream.get_error();
	}
	else
	{
		if (!stream.write_byte("type", object_type))
			return stream.get_error();
	}
	if (!stream.write_id("nodeId", id))
		return stream.get_error();
	if (!stream.write_string("name", name.c_str()))
		return stream.get_error();
	if (!stream.write_byte("attrs", (uint8_t)attributes))
		return stream.get_error();
	if (stream.get_version() >= RX_PARENT_REF_VERSION)
	{
		if (!stream.write_item_reference("super", parent))
			return stream.get_error();
	}
	else
	{// old version <= RX_FIRST_SERIZALIZE_VERSION
		if (parent.is_node_id())
		{
			if (!stream.write_id("superId", parent.get_node_id()))
				return stream.get_error();
		}
		else
		{
			if (!stream.write_id("superId", rx_node_id()))
				return stream.get_error();
		}
	}
	if (!stream.write_time("created", created_time.c_data()))
		return stream.get_error();
	if (!stream.write_time("modified", modified_time.c_data()))
		return stream.get_error();
	if (!stream.write_version("ver", version))
		return stream.get_error();
	if (!stream.write_string("path", path.c_str()))
		return stream.get_error();
	if (stream.get_version() >= RX_RUN_AT_LEVEL_VERSION)
	{
		if (!stream.write_string("runat", run_at.c_str()))
			return stream.get_error();
	}
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

rx_result meta_data::deserialize_meta_data (base_meta_reader& stream, uint8_t type, rx_item_type& object_type)
{
	if (!stream.start_object("meta"))
		return stream.get_error();
	if (stream.is_string_based())
	{
		string_type temp;
		if (!stream.read_string("type", temp))
			return stream.get_error();
		object_type = rx_parse_type_name(temp);
		if (object_type >= rx_item_type::rx_first_invalid)
			return temp + " is invalid type name";
	}
	else
	{
		uint8_t temp;
		if (!stream.read_byte("type", temp))
			return stream.get_error();
		if (temp >= rx_item_type::rx_first_invalid)
			return "Invalid type";
		object_type = (rx_item_type)temp;
	}
	if (!stream.read_id("nodeId", id))
		return stream.get_error();
	if (!stream.read_string("name", name))
		return stream.get_error();
	uint8_t temp_byte;
	if (!stream.read_byte("attrs", temp_byte))
		return stream.get_error();
	attributes = (namespace_item_attributes)temp_byte;
	if (stream.get_version() >= RX_PARENT_REF_VERSION)
	{
		if (!stream.read_item_reference("super", parent))
			return stream.get_error();
	}
	else
	{// old version <= RX_FIRST_SERIZALIZE_VERSION
		rx_node_id parent_id;
		if (!stream.read_id("superId", parent_id))
			return stream.get_error();
		parent = parent_id;
	}
	rx_time_struct_t temp;
	if (!stream.read_time("created", temp))
		return stream.get_error();
	created_time = temp;
	if (!stream.read_time("modified", temp))
		return stream.get_error();
	modified_time = temp;
	if (!stream.read_version("ver", version))
		return stream.get_error();
	if (!stream.read_string("path", path))
		return stream.get_error();
	if (stream.get_version() >= RX_RUN_AT_LEVEL_VERSION)
	{
		if (!stream.read_string("runat", run_at))
			return stream.get_error();
	}
	
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

rx_result meta_data::check_in (base_meta_reader& stream)
{
	return false;
}

rx_result meta_data::check_out (base_meta_writer& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_CHECKOUT, 0))
		return stream.get_error();

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, STREAMING_TYPE_CHECKOUT);
*/

	if (!stream.write_footer())
		return stream.get_error();

	return true;
}

bool meta_data::is_system () const
{
	return attributes & namespace_item_attributes::namespace_item_system_mask;
}

values::rx_value meta_data::get_value () const
{
	values::rx_value temp;
	temp.assign_static(version, modified_time);
	return temp;
}

string_type meta_data::get_full_path () const
{
	if (path.empty())
	{
		return RX_DIR_DELIMETER_STR + name;
	}
	else if (*path.rbegin() == RX_DIR_DELIMETER)
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
	if (path.empty())
	{
		buffer = RX_DIR_DELIMETER_STR + name;
	}
	else if (*path.rbegin() == RX_DIR_DELIMETER)
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
	RX_ASSERT(false);
	return rx_item_reference();
	//string_type my_path = get_full_path();
	//if (my_path.empty())
	//	return rx_item_reference();
	//rx_directory_ptr my_directory = rx_gate::instance().get_directory(path);
	//size_t best_idx = 0;
	//string_array best_splitted;
	//size_t current_idx;
	//string_type path;
	//string_array my_spplitted, splitted;
	//rx_split_string(path, my_spplitted, RX_DIR_DELIMETER);
	//for (const auto& one_path : dirs)
	//{
	//	auto one_directory = my_directory->get_sub_directory(one_path);
	//	if (one_directory)
	//	{
	//		path.clear();
	//		one_directory->fill_path(path);
	//		splitted.clear();
	//		rx_split_string(path, splitted, RX_DIR_DELIMETER);
	//		current_idx = 0;
	//		auto my_it = my_spplitted.begin();
	//		auto his_it = splitted.begin();
	//		while (my_it != my_spplitted.end() && his_it != splitted.end())
	//		{
	//			if (*my_it != *his_it)
	//				break;
	//			current_idx++;
	//			my_it++;
	//			his_it++;
	//		}
	//		if (current_idx > best_idx)
	//		{
	//			best_idx = current_idx;
	//			best_splitted = splitted;
	//		}
	//	}
	//}
	//if (best_idx > 0)
	//{
	//	auto my_it = my_spplitted.begin();
	//	auto his_it = best_splitted.begin();
	//	// skip the common parts
	//	while (my_it != my_spplitted.end() && his_it != best_splitted.end())
	//	{
	//		if (*my_it != *his_it)
	//			break;
	//		my_it++;
	//		his_it++;
	//	}
	//	string_type relative_path(RX_DIR_DELIMETER_STR);
	//	for (; his_it != best_splitted.end(); his_it++)
	//	{
	//		relative_path += ".." RX_DIR_DELIMETER_STR;
	//	}
	//	for (; my_it != my_spplitted.begin(); my_it++)
	//	{
	//		relative_path += *my_it;
	//		relative_path += RX_DIR_DELIMETER;
	//	}
	//	relative_path += name;

	//	return rx_item_reference(relative_path);
	//}
	//else
	//{
	//	return rx_item_reference(my_path);
	//}
}

void meta_data::increment_version (bool full_ver)
{
	if (full_ver)
		version = ((version >> 16) + 1 ) << 16;
	else
		version++;
}

void meta_data::set_id (const rx_node_id& id)
{
	this->id = id;
}

void meta_data::set_parent (const rx_node_id& id)
{
	this->parent = id;
}

void meta_data::set_name (const string_type& str)
{
	this->name = str;
}

void meta_data::set_path (const string_type& str)
{
	this->path = str;
}

void meta_data::set_created_time (rx_time tm)
{
	this->created_time = tm;
}

void meta_data::set_modified_time (rx_time tm)
{
	this->modified_time = tm;
}

void meta_data::set_attributes (namespace_item_attributes val)
{
	this->attributes = val;
}

void meta_data::set_version (uint32_t val)
{
	this->version = version;
}

void meta_data::set_run_at (const string_type& run)
{
}

meta_data create_meta_for_new(const meta_data& proto)
{
	meta_data ret_data = proto;
	rx_time now(rx_time::now());
	ret_data.created_time = now;
	ret_data.modified_time = now;
	if(!proto.created_time.is_null())
		ret_data.created_time = proto.created_time;
	if (!proto.modified_time.is_null())
		ret_data.modified_time = proto.modified_time;
	ret_data.version = RX_INITIAL_ITEM_VERSION;
	if (ret_data.version < proto.version)
		ret_data.version = proto.version;
	if (proto.id.is_null())
		ret_data.id = rx_node_id::generate_new();

	return ret_data;
}


bool rx_is_runtime(rx_item_type type)
{
	switch (type)
	{
	case rx_item_type::rx_directory:
		return false;

	case rx_item_type::rx_application:
		return true;
	case rx_item_type::rx_application_type:
		return false;
	case rx_item_type::rx_domain:
		return true;
	case rx_item_type::rx_domain_type:
		return false;
	case rx_item_type::rx_object:
		return true;
	case rx_item_type::rx_object_type:
		return false;
	case rx_item_type::rx_port:
		return true;
	case rx_item_type::rx_port_type:
		return false;

	case rx_item_type::rx_struct_type:
	case rx_item_type::rx_variable_type:
	case rx_item_type::rx_source_type:
	case rx_item_type::rx_filter_type:
	case rx_item_type::rx_event_type:
	case rx_item_type::rx_mapper_type:
		return false;

	case rx_item_type::rx_display_type:
		return false;
	case rx_item_type::rx_program_type:
		return false;
	case rx_item_type::rx_method_type:
		return false;

	case rx_item_type::rx_relation_type:
		return false;

	default:
		return false;
	}
}
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

	case rx_item_type::rx_display_type:
		return RX_CPP_DISPLAY_CLASS_TYPE_NAME;

	case rx_item_type::rx_program_type:
		return RX_CPP_PROGRAM_CLASS_TYPE_NAME;
	case rx_item_type::rx_method_type:
		return RX_CPP_METHOD_CLASS_TYPE_NAME;
	case rx_item_type::rx_data_type:
		return RX_CPP_DATA_CLASS_TYPE_NAME;

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
		case struct_array_attribute_type:
			return RX_CPP_STRUCT_TYPE_NAME "[]";
		case variable_attribute_type:
		case variable_data_attribute_type:
			return RX_CPP_VARIABLE_TYPE_NAME;
		case variable_array_attribute_type:
		case variable_data_array_attribute_type:
			return RX_CPP_VARIABLE_TYPE_NAME "[]";
		case source_attribute_type:
			return RX_CPP_SOURCE_TYPE_NAME;
		case filter_attribute_type:
			return RX_CPP_FILTER_TYPE_NAME;
		case event_attribute_type:
			return RX_CPP_EVENT_TYPE_NAME;
		case mapper_attribute_type:
			return RX_CPP_MAPPER_TYPE_NAME;
		case const_attribute_type:
		case const_data_attribute_type:
			return RX_CONST_VALUE_TYPE_NAME;
		case const_data_array_attribute_type:
		case const_array_attribute_type:
			return RX_CONST_VALUE_TYPE_NAME "[]";
		case value_attribute_type:
		case value_data_attribute_type:
			return RX_VALUE_TYPE_NAME;
		case value_array_attribute_type:
		case value_data_array_attribute_type:
			return RX_VALUE_TYPE_NAME "[]";
		case relation_attribute_type:
			return RX_CPP_RELATION_TYPE_NAME;
		case relation_target_attribute_type:
			return RX_CPP_RELATION_TARGET_TYPE_NAME;
		case method_attribute_type:
			return RX_CPP_METHOD_TYPE_NAME;
		case program_attribute_type:
			return RX_CPP_PROGRAM_TYPE_NAME;
		case data_attribute_type:
			return RX_CPP_DATA_TYPE_NAME;
		case data_array_attribute_type:
			return RX_CPP_DATA_TYPE_NAME "[]";
		case display_attribute_type:
			return RX_CPP_DISPLAY_TYPE_NAME;
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

	else if (name == RX_CPP_DISPLAY_CLASS_TYPE_NAME)
		return rx_item_type::rx_display_type;

	else if (name == RX_CPP_PROGRAM_CLASS_TYPE_NAME)
		return rx_item_type::rx_program_type;
	else if (name == RX_CPP_METHOD_CLASS_TYPE_NAME)
		return rx_item_type::rx_method_type;

	else if(name==RX_CPP_DATA_CLASS_TYPE_NAME)
		return rx_item_type::rx_data_type;

	else if (name == RX_CPP_RELATION_CLASS_TYPE_NAME)
		return rx_item_type::rx_relation_type;

	return rx_item_type::rx_invalid_type;
}
} // namespace rx

