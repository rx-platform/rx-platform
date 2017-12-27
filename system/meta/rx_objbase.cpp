

/****************************************************************************
*
*  system\meta\rx_objbase.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"


// rx_objbase
#include "system/meta/rx_objbase.h"

#include "system/server/rx_server.h"

#include "lib/rx_ser_lib.h"
#include "system/json/rx_ser.h"


namespace rx_platform {

namespace objects {
const char* g_const_simple_class_name = "CONST_SIMPLE";

// Class rx_platform::objects::server_object 

server_object::server_object (const string_type& name, const rx_node_id& id)
	: object_runtime(name,id,true)
{
	init_object();
}


server_object::~server_object()
{
}



namespace_item_attributes server_object::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system | namespace_item_object);
}


// Class rx_platform::objects::complex_runtime_item 

complex_runtime_item::complex_runtime_item (object_runtime_ptr my_object)
      : my_object_(my_object)
{
}


complex_runtime_item::~complex_runtime_item()
{
}



rx_value complex_runtime_item::get_value (const string_type path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto it = names_cache_.find(mine);
		if (it != names_cache_.end())
		{// found it
			if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return sub_items_[(it->second&RT_INDEX_MASK)]->get_value(bellow);
			}
			else
			{// this here should never happend
				RX_ASSERT(false);
			}

		}
	}
	else// its' ours
	{
		auto it = names_cache_.find(path);
		if (it != names_cache_.end())
		{// search const values first
			if ((it->second&RT_CONST_IDX_MASK) == RT_CONST_IDX_MASK)
			{// const value
				RX_ASSERT((it->second&RT_INDEX_MASK) < const_values_.size());
				rx_value ret;
				const_values_[(it->second&RT_INDEX_MASK)]->get_value(ret,my_object_->get_modified_time(), my_object_->get_mode());
				return ret;
			}
			else if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return sub_items_[(it->second&RT_INDEX_MASK)]->get_value(nullptr);
			}
		}
	}

	rx_value ret;
	ret.set_time(rx_time::now());
	ret.set_quality(RX_BAD_QUALITY_CONFIG_ERROR);
	return ret;

}

uint32_t complex_runtime_item::set_hosting_object (object_runtime_ptr obj)
{
	RX_ASSERT(!my_object_);
	if (my_object_)
		return RX_ERROR;

	my_object_ = obj;
	return RX_OK;
}

object_runtime_ptr& complex_runtime_item::get_hosting_object ()
{
	return my_object_;
}

void complex_runtime_item::object_state_changed (const rx_time& now)
{
	for (auto& one : names_cache_)
	{
		uint32_t type = (one.second& RT_TYPE_MASK);
		switch (type)
		{
		case RT_CONST_IDX_MASK:
			{
				if ((one.second&RT_CALLBACK_MASK)!=0)
				{// has callback so fire all og it quality changed
					rx_value val;
					const_values_[(one.second&RT_INDEX_MASK)]->get_value(val, now, get_hosting_object()->get_mode());
					(*(const_values_callbacks_[RT_CALLBACK_INDEX(one.second)]))(val, 0);
				}

			}
			break;
		case RT_VALUE_IDX_MASK:
			{

			}
			break;
		case RT_COMPLEX_IDX_MASK:
			{
				sub_items_[(one.second&RT_INDEX_MASK)]->object_state_changed(now);
			}
			break;

		}
	}
}

value_callback_t* complex_runtime_item::get_callback (const string_type& path, rx_value& val)
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto it = names_cache_.find(mine);
		if (it != names_cache_.end())
		{// found it
			if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return sub_items_[(it->second&RT_INDEX_MASK)]->get_callback(bellow,val);
			}
			else
			{// this here should never happend
				RX_ASSERT(false);
			}

		}
	}
	else// its' ours
	{
		auto it = names_cache_.find(path);
		if (it != names_cache_.end())
		{// search const values first
			if ((it->second&RT_CONST_IDX_MASK) == RT_CONST_IDX_MASK)
			{// const value
				RX_ASSERT((it->second&RT_INDEX_MASK) < const_values_.size());
				if (it->second&RT_CALLBACK_MASK)
				{// we have callback mask so return it
					uint32_t idx = RT_CALLBACK_INDEX(it->second);
					const_values_[(it->second&RT_INDEX_MASK)]->get_value(val, get_hosting_object()->get_modified_time(), get_hosting_object()->get_mode());
					return const_values_callbacks_[idx];
				}
				else
				{// create new callback
					value_callback_t *ret = new value_callback_t;
					const_values_callbacks_.emplace_back(ret);
					it->second = ((it->second) | (((uint32_t)const_values_callbacks_.size()) << 16));
					const_values_[(it->second&RT_INDEX_MASK)]->get_value(val, get_hosting_object()->get_modified_time(), get_hosting_object()->get_mode());
					return ret;

				}
			}

		}
	}
	return nullptr;
}

uint32_t complex_runtime_item::register_sub_item (const string_type& name, complex_runtime_item::smart_ptr val)
{
	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		sub_items_.emplace_back(val);
		uint32_t idx = (uint32_t)(sub_items_.size() - 1);
		names_cache_.emplace(name, idx | RT_COMPLEX_IDX_MASK);
		val->set_hosting_object(my_object_);
		return RX_OK;
	}
	return RX_ERROR;
}

rx_value complex_runtime_item::get_value ()
{
	rx_value ret;
	ret.set_time(get_hosting_object()->get_created_time());
	ret.set_quality(RX_Q_TYPE_MISMATCH);
	return ret;
}

bool complex_runtime_item::serialize_definition (base_meta_writter& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_array("Items", names_cache_.size()))
		return false;
	for (const auto& one : names_cache_)
	{
		switch (one.second&RT_TYPE_MASK)
		{
		case RT_CONST_IDX_MASK:
			{// const value
				if (!stream.start_object("Item"))
					return false;
				if (!stream.write_string("Name", one.first.c_str()))
					return false;
				if (!stream.write_uint("ItemType", one.second&RT_TYPE_MASK))
					return false;
				if (!stream.start_object("Val"))
					return false;
				const_values_[one.second&RT_INDEX_MASK]->serialize_definition(stream, type, ts, mode);
				if (!stream.end_object())//Val
					return false;
				if (!stream.end_object())//Item
					return false;
			}
			break;
		case RT_VALUE_IDX_MASK:
			{// simple value
				if (!stream.start_object("Item"))
					return false;
				if (!stream.write_string("Name", one.first.c_str()))
					return false;
				if (!stream.write_uint("ItemType", one.second&RT_TYPE_MASK))
					return false;
				if (!stream.start_object("Val"))
					return false;
				values_[one.second&RT_INDEX_MASK]->serialize_definition(stream, type, mode);
				if (!stream.end_object())//Val
					return false;
				if (!stream.end_object())//Item
					return false;
			}
			break;
		case RT_COMPLEX_IDX_MASK:
			{// complex item
			}
			break;
		default:
			RX_ASSERT(false);
		}
	}
	if (!stream.end_array())
		return false;
	return true;
}

bool complex_runtime_item::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::objects::object_runtime 

string_type object_runtime::type_name = RX_CPP_OBJECT_TYPE_NAME;

object_runtime::object_runtime()
{
}

object_runtime::object_runtime (const string_type& name, const rx_node_id& id, bool system)
	: object_runtime_t(name,id,rx_node_id::null_id,system)
{
}


object_runtime::~object_runtime()
{
}



rx_value object_runtime::get_value (const string_type path) const
{
	if (path.empty())
	{
		rx_value ret(get_name());
		ret.set_time(get_modified_time());
		ret.adapt_quality_to_mode(mode_);
		return ret;
	}
	else
	{
		return complex_item_->get_value(path);
	}
}

void object_runtime::turn_on ()
{
	if (mode_.turn_on())
		complex_item_->object_state_changed(rx_time::now());
}

void object_runtime::turn_off ()
{
	if (mode_.turn_off())
		complex_item_->object_state_changed(rx_time::now());
}

void object_runtime::set_blocked ()
{
	mode_.set_blocked();
}

void object_runtime::set_test ()
{
	mode_.set_test();
}

values::rx_value object_runtime::get_value () const
{
	// this static object improves performance its, created only once and it is emtyy
	return rx_value(get_version(), get_modified_time());
}

namespace_item_attributes object_runtime::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_object| namespace_item_read_access);
}

void object_runtime::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

const string_type& object_runtime::get_item_name () const
{
	return get_name();
}

bool object_runtime::generate_json (std::ostream& def, std::ostream& err) const
{

	rx_platform::serialization::json_writter writter;

	writter.write_header(STREAMING_TYPE_OBJECT);

	serialize_definition(writter, STREAMING_TYPE_OBJECT);

	writter.write_footer();

	string_type result;
	bool out = writter.get_string(result, true);
	
	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return out;
}

bool object_runtime::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!checkable_type::serialize_definition(stream, type))
		return false;

	if (!complex_item_->serialize_definition(stream, type,get_modified_time(),mode_))
		return false;
	
	if (!stream.start_array("Programs", programs_.size()))
		return false;

	for (const auto& one : programs_)
	{
		if (!one->save_program(stream, type))
			return false;
	}

	if (!stream.end_array())
		return false;

	return true;
}

bool object_runtime::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!checkable_type::deserialize_definition(stream, type))
		return false;

	if (!complex_item_->deserialize_definition(stream, type))
		return false;

	if (!stream.start_array("Programs"))
		return false;

	while (!stream.array_end())
	{
		logic::ladder_program::smart_ptr one(pointers::_create_new);
		if (!one->load_program(stream, type))
			return false;
		programs_.push_back(one);
	}

	return true;
}

bool object_runtime::init_object ()
{
	complex_item_ = complex_runtime_item_ptr(smart_this());
	return true;
}

bool object_runtime::is_browsable () const
{
	return true;
}


// Class rx_platform::objects::value_item 

const uint32_t value_item::type_id_ = RT_TYPE_ID_VALUE;

value_item::value_item()
{
}


value_item::~value_item()
{
}



bool value_item::serialize_definition (base_meta_writter& stream, uint8_t type, const rx_mode_type& mode) const
{
	rx_value val;
	get_value(val, rx_time::now(), mode);
	if (!val.serialize_value(stream))
		return false;
	return true;
}

bool value_item::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}


// Class rx_platform::objects::variable_runtime 

variable_runtime::variable_runtime (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


variable_runtime::~variable_runtime()
{
}



// Class rx_platform::objects::filter_runtime 

filter_runtime::filter_runtime (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


filter_runtime::~filter_runtime()
{
}



// Class rx_platform::objects::source 

source::source (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


source::~source()
{
}



// Class rx_platform::objects::mapper 

mapper::mapper (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


mapper::~mapper()
{
}



// Class rx_platform::objects::domain_runtime 

string_type domain_runtime::type_name = RX_CPP_DOMAIN_TYPE_NAME;

domain_runtime::domain_runtime()
{
	my_domain_ = smart_this();
}

domain_runtime::domain_runtime (const string_type& name, const rx_node_id& id, bool system)
	: object_runtime(name,id,system)
{
	my_domain_ = smart_this();
}


domain_runtime::~domain_runtime()
{
}



string_type domain_runtime::get_type_name () const
{
  return type_name;

}

namespace_item_attributes domain_runtime::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_script | namespace_item_read_access | namespace_item_system);
}


// Class rx_platform::objects::application_runtime 

string_type application_runtime::type_name = RX_CPP_APPLICATION_TYPE_NAME;

application_runtime::application_runtime()
{
	my_application_ = smart_this();
	my_domain_ = smart_this();
}

application_runtime::application_runtime (const string_type& name, const rx_node_id& id, bool system)
	: domain_runtime(name,id,system)
{
	my_application_ = smart_this();
	my_domain_ = smart_this();
}


application_runtime::~application_runtime()
{
}



string_type application_runtime::get_type_name () const
{
  return type_name;

}

namespace_item_attributes application_runtime::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_application | namespace_item_read_access | namespace_item_system);
}


// Class rx_platform::objects::struct_runtime 

struct_runtime::struct_runtime (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


struct_runtime::~struct_runtime()
{
}



// Class rx_platform::objects::const_value_item 

const uint32_t const_value_item::type_id_ = RT_TYPE_ID_CONST_VALUE;

const_value_item::const_value_item()
{
}


const_value_item::~const_value_item()
{
}



bool const_value_item::serialize_definition (base_meta_writter& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	rx_value val;
	get_value(val, ts, mode);
	if (!val.serialize_value(stream))
		return false;
	return true;
}

bool const_value_item::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}


// Class rx_platform::objects::port_runtime 

string_type port_runtime::type_name = RX_CPP_PORT_TYPE_NAME;

port_runtime::port_runtime()
{
}

port_runtime::port_runtime (const string_type& name, const rx_node_id& id)
	: object_runtime(name,id,true)// every port is system objects
{
}


port_runtime::~port_runtime()
{
}



string_type port_runtime::get_type_name () const
{
  return type_name;

}

namespace_item_attributes port_runtime::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_write_access|namespace_item_system|namespace_item_port | namespace_item_read_access);
}


// Class rx_platform::objects::user_object 

user_object::user_object()
{
}

user_object::user_object (const string_type& name, const rx_node_id& id)
	: object_runtime(name, id, true)
{
	init_object();
}


user_object::~user_object()
{
}



namespace_item_attributes user_object::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_write_access | namespace_item_execute_access | namespace_item_object);
}


} // namespace objects
} // namespace rx_platform

