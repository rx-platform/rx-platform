

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


namespace server {

namespace objects {
const char* g_const_simple_class_name = "CONST_SIMPLE";

// Class server::objects::server_object 

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


// Class server::objects::complex_runtime_item 

complex_runtime_item::complex_runtime_item (object_runtime_ptr my_object)
      : m_my_object(my_object)
{
}


complex_runtime_item::~complex_runtime_item()
{
}



dword complex_runtime_item::register_const_value (const string_type& name, const_value_item& val)
{
	auto it = m_names_cache.find(name);
	if (it == m_names_cache.end())
	{
		m_const_values.emplace_back(&val);
		dword idx = (dword)(m_const_values.size() - 1);
		m_names_cache.emplace(name, idx | RT_CONST_IDX_MASK);
		return RX_OK;
	}
	return RX_ERROR;
}

rx_value complex_runtime_item::get_value (const string_type path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto it = m_names_cache.find(mine);
		if (it != m_names_cache.end())
		{// found it
			if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return m_sub_items[(it->second&RT_INDEX_MASK)]->get_value(bellow);
			}
			else
			{// this here should never happend
				RX_ASSERT(false);
			}

		}
	}
	else// its' ours
	{
		auto it = m_names_cache.find(path);
		if (it != m_names_cache.end())
		{// search const values first
			if ((it->second&RT_CONST_IDX_MASK) == RT_CONST_IDX_MASK)
			{// const value
				RX_ASSERT((it->second&RT_INDEX_MASK) < m_const_values.size());
				rx_value ret;
				m_const_values[(it->second&RT_INDEX_MASK)]->get_value(ret,m_my_object->get_modified_time(), m_my_object->get_mode());
				return ret;
			}
			else if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return m_sub_items[(it->second&RT_INDEX_MASK)]->get_value(nullptr);
			}
		}
	}

	rx_value ret;
	ret.set_time(rx_time::now());
	ret.set_quality(RX_BAD_QUALITY_CONFIG_ERROR);
	return ret;

}

dword complex_runtime_item::set_hosting_object (object_runtime_ptr obj)
{
	RX_ASSERT(!m_my_object);
	if (m_my_object)
		return RX_ERROR;

	m_my_object = obj;
	return RX_OK;
}

object_runtime_ptr& complex_runtime_item::get_hosting_object ()
{
	return m_my_object;
}

void complex_runtime_item::object_state_changed (const rx_time& now)
{
	for (auto& one : m_names_cache)
	{
		dword type = (one.second& RT_TYPE_MASK);
		switch (type)
		{
		case RT_CONST_IDX_MASK:
			{
				if ((one.second&RT_CALLBACK_MASK)!=0)
				{// has callback so fire all og it quality changed
					rx_value val;
					m_const_values[(one.second&RT_INDEX_MASK)]->get_value(val, now, get_hosting_object()->get_mode());
					(*(m_const_values_callbacks[RT_CALLBACK_INDEX(one.second)]))(val, 0);
				}

			}
			break;
		case RT_VALUE_IDX_MASK:
			{

			}
			break;
		case RT_COMPLEX_IDX_MASK:
			{
				m_sub_items[(one.second&RT_INDEX_MASK)]->object_state_changed(now);
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
		auto it = m_names_cache.find(mine);
		if (it != m_names_cache.end())
		{// found it
			if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return m_sub_items[(it->second&RT_INDEX_MASK)]->get_callback(bellow,val);
			}
			else
			{// this here should never happend
				RX_ASSERT(false);
			}

		}
	}
	else// its' ours
	{
		auto it = m_names_cache.find(path);
		if (it != m_names_cache.end())
		{// search const values first
			if ((it->second&RT_CONST_IDX_MASK) == RT_CONST_IDX_MASK)
			{// const value
				RX_ASSERT((it->second&RT_INDEX_MASK) < m_const_values.size());
				if (it->second&RT_CALLBACK_MASK)
				{// we have callback mask so return it
					dword idx = RT_CALLBACK_INDEX(it->second);
					m_const_values[(it->second&RT_INDEX_MASK)]->get_value(val, get_hosting_object()->get_modified_time(), get_hosting_object()->get_mode());
					return m_const_values_callbacks[idx];
				}
				else
				{// create new callback
					value_callback_t *ret = new value_callback_t;
					m_const_values_callbacks.emplace_back(ret);
					it->second = ((it->second) | (((dword)m_const_values_callbacks.size()) << 16));
					m_const_values[(it->second&RT_INDEX_MASK)]->get_value(val, get_hosting_object()->get_modified_time(), get_hosting_object()->get_mode());
					return ret;

				}
			}

		}
	}
	return nullptr;
}

dword complex_runtime_item::register_sub_item (const string_type& name, complex_runtime_item::smart_ptr val)
{
	auto it = m_names_cache.find(name);
	if (it == m_names_cache.end())
	{
		m_sub_items.emplace_back(val);
		dword idx = (dword)(m_sub_items.size() - 1);
		m_names_cache.emplace(name, idx | RT_COMPLEX_IDX_MASK);
		val->set_hosting_object(m_my_object);
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

bool complex_runtime_item::serialize_definition (base_meta_writter& stream, byte type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_array("Items", m_names_cache.size()))
		return false;
	for (const auto& one : m_names_cache)
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
				m_const_values[one.second&RT_INDEX_MASK]->serialize_definition(stream, type, ts, mode);
				if (!stream.end_object())//Val
					return false;
				if (!stream.end_object())//Item
					return false;
			}
			break;
		case RT_VALUE_IDX_MASK:
			{// simple value
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

bool complex_runtime_item::deserialize_definition (base_meta_reader& stream, byte type)
{
	return false;
}


// Class server::objects::object_runtime 

string_type object_runtime::type_name = "OBJECT";

object_runtime::object_runtime()
      : m_created_time(rx_time::now()),
        m_modified_time(rx_time::now())
{
}

object_runtime::object_runtime (const string_type& name, const rx_node_id& id, bool system)
      : m_created_time(rx_time::now()),
        m_modified_time(rx_time::now())
	, object_runtime_t(name,id,rx_node_id::null_id,system)
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
		ret.set_time(m_created_time);
		ret.adapt_quality_to_mode(m_mode);
		return ret;
	}
	else
	{
		return m_complex_item->get_value(path);
	}
}

void object_runtime::turn_on ()
{
	if (m_mode.turn_on())
		m_complex_item->object_state_changed(rx_time::now());
}

void object_runtime::turn_off ()
{
	if (m_mode.turn_off())
		m_complex_item->object_state_changed(rx_time::now());
}

void object_runtime::set_blocked ()
{
	m_mode.set_blocked();
}

void object_runtime::set_test ()
{
	m_mode.set_test();
}

void object_runtime::get_value (values::rx_value& val) const
{
	// this static object improves performance its, created only once and it is emtyy
	static const string_type empty;
	val = get_value(empty);
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

	server::serialization::json_writter writter;

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

bool object_runtime::serialize_definition (base_meta_writter& stream, byte type) const
{
	if (!checkable_type::serialize_definition(stream, type))
		return false;

	if (!m_complex_item->serialize_definition(stream, type,m_modified_time,m_mode))
		return false;

	return true;
}

bool object_runtime::deserialize_definition (base_meta_reader& stream, byte type)
{
	if (!checkable_type::deserialize_definition(stream, type))
		return false;

	if (!m_complex_item->deserialize_definition(stream, type))
		return false;

	return true;
}

dword object_runtime::register_const_value (const string_type& name, const_value_item& val)
{
	return m_complex_item->register_const_value(name, val);
}

bool object_runtime::init_object ()
{
	m_complex_item = complex_runtime_item_ptr(smart_this());
	return true;
}


// Class server::objects::value_item 

const dword value_item::m_type_id = RT_TYPE_ID_VALUE;

value_item::value_item()
{
}


value_item::~value_item()
{
}



bool value_item::serialize_definition (base_meta_writter& stream, byte type) const
{
	return true;
}

bool value_item::deserialize_definition (base_meta_reader& stream, byte type)
{
	return true;
}


// Class server::objects::variable_runtime 

variable_runtime::variable_runtime (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


variable_runtime::~variable_runtime()
{
}



// Class server::objects::filter_runtime 

filter_runtime::filter_runtime (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


filter_runtime::~filter_runtime()
{
}



// Class server::objects::source 

source::source (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


source::~source()
{
}



// Class server::objects::mapper 

mapper::mapper (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


mapper::~mapper()
{
}



// Class server::objects::domain_runtime 

string_type domain_runtime::type_name = "DOMAIN";

domain_runtime::domain_runtime()
{
	m_my_domain = smart_this();
}

domain_runtime::domain_runtime (const string_type& name, const rx_node_id& id, bool system)
	: object_runtime(name,id,system)
{
	m_my_domain = smart_this();
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
	return (namespace_item_attributes)(namespace_item_domain | namespace_item_read_access | namespace_item_system);
}


// Class server::objects::application_runtime 

string_type application_runtime::type_name = "APPLICATION";

application_runtime::application_runtime()
{
	m_my_application = smart_this();
	m_my_domain = smart_this();
}

application_runtime::application_runtime (const string_type& name, const rx_node_id& id, bool system)
	: domain_runtime(name,id,system)
{
	m_my_application = smart_this();
	m_my_domain = smart_this();
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


// Class server::objects::struct_runtime 

struct_runtime::struct_runtime (object_runtime_ptr my_object)
	: complex_runtime_item(my_object)
{
}


struct_runtime::~struct_runtime()
{
}



// Class server::objects::const_value_item 

const dword const_value_item::m_type_id = RT_TYPE_ID_CONST_VALUE;

const_value_item::const_value_item()
{
}


const_value_item::~const_value_item()
{
}



bool const_value_item::serialize_definition (base_meta_writter& stream, byte type, const rx_time& ts, const rx_mode_type& mode) const
{
	rx_value val;
	get_value(val, ts, mode);
	if (!val.serialize_value(stream))
		return false;
	return true;
}

bool const_value_item::deserialize_definition (base_meta_reader& stream, byte type)
{
	return true;
}


// Class server::objects::port_runtime 

string_type port_runtime::type_name = "PORT";

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


// Class server::objects::user_object 

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
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_write_access | namespace_item_execute | namespace_item_object);
}


} // namespace objects
} // namespace server

