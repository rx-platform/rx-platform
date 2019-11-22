

/****************************************************************************
*
*  system\meta\rx_meta_data.cpp
*
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
// rx_meta_data
#include "system/meta/rx_meta_data.h"

#include "system/meta/rx_obj_types.h"
#include "system/server/rx_server.h"
#include "api/rx_platform_api.h"


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
}

// Class rx_platform::meta::meta_data 

meta_data::meta_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, namespace_item_attributes attrs, const string_type& path, rx_time now)
      : version_(RX_INITIAL_ITEM_VERSION)
	, name_(name)
	, id_(id)
	, parent_(parent)
	, path_(path)
	, created_time_(now)
	, modified_time_(now)
	, attributes_(attrs)
	, state_(rx_item_state::rx_item_state_unknown)
{
	if (id_.is_null())
		id_ = rx_node_id(rx_uuid::create_new().uuid());
}

meta_data::meta_data (namespace_item_attributes attrs, rx_time now)
      : version_(RX_INITIAL_ITEM_VERSION)
	, created_time_(now)
	, modified_time_(now)
	, attributes_(attrs)
	, state_(rx_item_state::rx_item_state_unknown)
{
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
	if (!stream.write_id("nodeId", id_))
		return false;
	if (!stream.write_string("name", name_.c_str()))
		return false;
	if (!stream.write_byte("attrs", (uint8_t)attributes_))
		return false;
	if (!stream.write_id("superId", parent_))
		return false;
	if (!stream.write_time("created", created_time_))
		return false;
	if (!stream.write_time("modified", modified_time_))
		return false;
	if (!stream.write_version("ver", version_))
		return false;
	if (!stream.write_string("path", path_))
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
	if (!stream.read_id("nodeId", id_))
		return false;
	if (!stream.read_string("name", name_))
		return false;
	uint8_t temp_byte;
	if (!stream.read_byte("attrs", temp_byte))
		return false;
	attributes_ = (namespace_item_attributes)temp_byte;
	if (!stream.read_id("superId", parent_))
		return false;
	if (!stream.read_time("created", created_time_))
		return false;
	if (!stream.read_time("modified", modified_time_))
		return false;
	if (!stream.read_string("path", path_))
		return false;
	if (!stream.read_version("ver", version_))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

values::rx_value meta_data::get_value () const
{
	values::rx_value temp;
	temp.assign_static(version_, modified_time_);
	return temp;
}

void meta_data::construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, namespace_item_attributes attributes, const string_type& path)
{
	name_ = name;
	id_ = id;
	parent_ = type_id;
	path_ = path;
	attributes_ = attributes;
}

bool meta_data::get_system () const
{
	return (attributes_&namespace_item_system) != namespace_item_null;
}

rx_result_with<platform_item_ptr> meta_data::deserialize_runtime_item (base_meta_reader& stream, uint8_t type)
{
	meta::meta_data meta;
	rx_item_type type_id;
	if (!meta.deserialize_meta_data(stream, type, type_id))
		return "Error deserialize meta data!";

	switch (type_id)
	{
		case rx_item_type::rx_object:
		{
			auto result = deserialize_runtime<object_types::object_type>(meta, stream, type);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
		case rx_item_type::rx_port:
		{
			auto result = deserialize_runtime<object_types::port_type>(meta, stream, type);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
		case rx_item_type::rx_domain:
		{
			auto result = deserialize_runtime<object_types::domain_type>(meta, stream, type);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
		case rx_item_type::rx_application:
		{
			auto result = deserialize_runtime<object_types::application_type>(meta, stream, type);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
		default:
			return rx_item_type_name(type_id) + " is unknown type name";
	}
	return "Undefined error";
}

void meta_data::resolve ()
{
	// resolve storage type by attributes
	modified_time_ = rx_time::now();
	// now handle null id
	if (id_.is_null())
	{
		id_ = rx_node_id::generate_new(RX_USER_NAMESPACE);
	}
}

void meta_data::set_path (const string_type& path)
{
	path_ = path;
}

string_type meta_data::get_full_path () const
{
	if (!path_.empty() && *path_.rbegin() == RX_DIR_DELIMETER)
	{
		string_type ret(path_);
		ret += name_;
		return ret;

	}
	else
	{
		string_type ret(path_);
		ret.reserve(ret.size() + name_.size() + 1);
		ret += RX_DIR_DELIMETER;
		ret += name_;
		return ret;
	}
}

bool meta_data::is_system () const
{
	return attributes_ & namespace_item_attributes::namespace_item_system_mask;
}

rx_result_with<rx_storage_ptr> meta_data::resolve_storage () const
{
	auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(path_);
	if (dir)
	{
		return dir->resolve_storage();
	}
	else
		return "Unable to locate item's directory!";
}

void meta_data::increment_version (bool full_ver)
{
	version_++;
}

rx_platform::meta::item_reference meta_data::create_item_reference ()
{
	if (id_.is_null() && !path_.empty())
		return item_reference(get_full_path());
	else
		return item_reference(id_);
}

rx_platform::meta::item_reference meta_data::create_weak_item_reference (const string_array& dirs)
{
	string_type my_path = get_full_path();
	if (my_path.empty())
		return item_reference();
	rx_directory_ptr my_directory = rx_gate::instance().get_root_directory()->get_sub_directory(path_);
	size_t best_idx = 0;
	string_array best_splitted;
	size_t current_idx;
	string_type path;
	string_array my_spplitted, splitted;
	rx_split_string(path_, my_spplitted, RX_DIR_DELIMETER);
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
		relative_path += name_;

		return item_reference(relative_path);
	}
	else
	{
		return item_reference(my_path);
	}
}


// Class rx_platform::meta::item_reference 

item_reference::item_reference()
{
	is_id_ = true;
	new(&id_) rx_node_id();
}

item_reference::item_reference(const item_reference &right)
{
	if (right.is_id_)
	{
		new(&id_) rx_node_id(right.id_);
	}
	else
	{
		new(&path_) string_type(right.path_);
	}
	is_id_ = right.is_id_;
}

item_reference::item_reference (const rx_node_id& right)
{
	is_id_ = true;
	new(&id_) rx_node_id(right);
}

item_reference::item_reference (const string_type& right)
{
	is_id_ = false;
	new(&path_) string_type(right);
}

item_reference::item_reference (const char* right)
{
	is_id_ = false;
	new(&path_) string_type(right);
}

item_reference::item_reference (const rx_simple_value& right)
{
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(right.get_storage().get_id_value());
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(right.get_storage().get_string_value());
	}
}


item_reference::~item_reference()
{
	clear_content();
}


item_reference & item_reference::operator=(const item_reference &right)
{
	clear_content();
	if (right.is_id_)
	{
		new(&id_) rx_node_id(right.id_);
	}
	else
	{
		new(&path_) string_type(right.path_);
	}
	is_id_ = right.is_id_;
	return *this;
}



rx_result item_reference::serialize_reference (const char* name, base_meta_writer& stream) const
{
	if (!stream.start_object(name))
		return false;
	if (!stream.write_bool("is_id", is_id_))
		return false;
	if (is_id_)
	{
		if (!stream.write_id("id", id_))
			return false;
	}
	else
	{
		if (!stream.write_string("path", path_))
			return false;
	}
	if (!stream.end_object())
		return false;
	return true;
}

rx_result item_reference::deserialize_reference (const char* name, base_meta_reader& stream)
{
	clear_content();
	if (!stream.start_object(name))
		return false;
	if (!stream.read_bool("is_id", is_id_))
		return false;
	if (is_id_)
	{
		new(&id_) rx_node_id();
		if (!stream.read_id("id", id_))
			return false;
	}
	else
	{
		new(&path_) string_type();
		if (!stream.read_string("path", path_))
			return false;
	}
	if (!stream.end_object())
		return false;
	return true;
}

bool item_reference::is_null () const
{
	if (is_id_)
	{
		return id_.is_null();
	}
	else
	{
		return path_.empty();
	}
}

void item_reference::clear_content ()
{
	if (is_id_)
	{
		id_.~rx_node_id();
	}
	else
	{
		path_.~string_type();
	}
}

item_reference& item_reference::operator = (const rx_node_id& right)
{
	clear_content();
	is_id_ = true;
	new(&id_) rx_node_id(right);
	return *this;
}

item_reference& item_reference::operator = (const string_type& right)
{
	clear_content();
	is_id_ = false;
	new(&path_) string_type(right);
	return *this;
}

bool item_reference::is_node_id () const
{
	return is_id_;
}

const string_type& item_reference::get_path () const
{
	if (is_id_)
		throw std::invalid_argument("Target is referenced by id!");
	else
		return path_;
}

const rx_node_id& item_reference::get_node_id () const
{
	if (!is_id_)
		throw std::invalid_argument("Target is referenced by path!");
	else
		return id_;
}

rx_simple_value item_reference::to_value () const
{
	values::rx_simple_value temp;
	if (is_id_)
	{
		temp.assign_static<rx_node_id>(rx_node_id(id_));
	}
	else
	{
		temp.assign_static<string_type>(string_type(path_));
	}
	return temp;
}

item_reference& item_reference::operator = (const rx_simple_value& right)
{
	clear_content();
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(right.get_storage().get_id_value());
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(right.get_storage().get_string_value());
	}
	return *this;
}


item_reference::item_reference(item_reference&& right) noexcept
{
	if (right.is_id_)
	{
		new(&id_) rx_node_id(std::move(right.id_));
	}
	else
	{
		new(&path_) string_type(std::move(right.path_));
	}
	is_id_ = right.is_id_;

}
item_reference& item_reference::operator=(item_reference&& right) noexcept
{
	clear_content();
	if (right.is_id_)
	{
		new(&id_) rx_node_id(std::move(right.id_));
	}
	else
	{
		new(&path_) string_type(std::move(right.path_));
	}
	is_id_ = right.is_id_;
	return *this;
}

item_reference::item_reference(rx_node_id&& right) noexcept
{
	is_id_ = true;
	new(&id_) rx_node_id(std::move(right));
}
item_reference::item_reference(string_type&& right) noexcept
{
	is_id_ = false;
	new(&path_) string_type(std::move(right));
}

item_reference::item_reference(rx_simple_value&& right) noexcept
{
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(std::move(right.get_storage().get_id_value()));
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(std::move(right.get_storage().get_string_value()));
	}
}

item_reference& item_reference::operator= (rx_node_id&& right) noexcept
{
	clear_content();
	is_id_ = true;
	new(&id_) rx_node_id(std::move(right));
	return *this;

}
item_reference& item_reference::operator= (string_type&& right) noexcept
{
	clear_content();
	is_id_ = false;
	new(&path_) string_type(std::move(right));
	return *this;
}
item_reference& item_reference::operator= (rx_simple_value&& right) noexcept
{
	clear_content();
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(std::move(right.get_storage().get_id_value()));
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(std::move(right.get_storage().get_string_value()));
	}
	return *this;
}
} // namespace meta
} // namespace rx_platform

