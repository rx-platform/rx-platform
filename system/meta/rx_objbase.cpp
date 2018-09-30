

/****************************************************************************
*
*  system\meta\rx_objbase.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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
#include "sys_internal/rx_internal_ns.h"
#include "lib/rx_ser_lib.h"
#include "system/json/rx_ser.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace objects {
const char* g_const_simple_class_name = "CONST_SIMPLE";

namespace object_types {

// Class rx_platform::objects::object_types::user_object 

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


// Class rx_platform::objects::object_types::server_object 

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


// Class rx_platform::objects::object_types::application_runtime 

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
	return (namespace_item_attributes)(namespace_item_application | namespace_item_domain | namespace_item_object | namespace_item_read_access | namespace_item_system);
}


// Class rx_platform::objects::object_types::domain_runtime 

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
	return (namespace_item_attributes)(namespace_item_domain | namespace_item_object | namespace_item_read_access | namespace_item_system);
}


// Class rx_platform::objects::object_types::port_runtime 

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


// Class rx_platform::objects::object_types::object_runtime 

string_type object_runtime::type_name = RX_CPP_OBJECT_TYPE_NAME;

object_runtime::object_runtime()
      : change_time_(rx_time::now())
	, runtime_item_("_unnamed", rx_node_id::null_id, false)
{
}

object_runtime::object_runtime (const string_type& name, const rx_node_id& id, bool system)
      : change_time_(rx_time::now())
	, meta_data_(name,id,rx_node_id::null_id,system)
	, runtime_item_(name, id, system)
{
}


object_runtime::~object_runtime()
{
}



rx_value object_runtime::get_value (const string_type path) const
{
	if (path.empty())
	{
		rx_value ret;
		ret.assign_static(meta_data_.get_name(), get_modified_time());
		ret.adapt_quality_to_mode(mode_);
		return ret;
	}
	else
	{
		return runtime_item_.get_value(path);
	}
}

void object_runtime::turn_on ()
{
	if (mode_.turn_on())
		runtime_item_.object_state_changed(rx_time::now());
}

void object_runtime::turn_off ()
{
	if (mode_.turn_off())
		runtime_item_.object_state_changed(rx_time::now());
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
	// this static object improves performance its, created only once and it is empty
	rx_value temp;
	temp.assign_static(meta_data_.get_version(), meta_data_.get_modified_time());
	return temp;
}

namespace_item_attributes object_runtime::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_object| namespace_item_read_access);
}

void object_runtime::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

bool object_runtime::generate_json (std::ostream& def, std::ostream& err) const
{

	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	serialize_definition(writer, STREAMING_TYPE_OBJECT);

	writer.write_footer();

	string_type result;
	bool out = writer.get_string(result, true);

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return out;
}

bool object_runtime::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_data_.serialize_checkable_definition(stream, type))
		return false;

	if (!runtime_item_.serialize_definition(stream, type,get_modified_time(),mode_))
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
	if (!meta_data_.deserialize_checkable_definition(stream, type))
		return false;

	if (!runtime_item_.deserialize_definition(stream, type))
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
	return true;
}

bool object_runtime::is_browsable () const
{
	return true;
}

void object_runtime::get_content (server_items_type& sub_items, const string_type& pattern) const
{
	runtime_item_.get_sub_items(sub_items,pattern);
}

platform_item_ptr object_runtime::get_item_ptr ()
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

rx_time object_runtime::get_created_time () const
{
	return meta_data_.get_created_time();
}

rx_time object_runtime::get_modified_time () const
{
	return meta_data_.get_modified_time();
}

string_type object_runtime::get_name () const
{
	return meta_data_.get_name();
}

size_t object_runtime::get_size () const
{
	return sizeof(*this);
}

blocks::complex_runtime_item_ptr object_runtime::get_complex_item ()
{
	return &runtime_item_;
}


} // namespace object_types
} // namespace objects
} // namespace rx_platform

