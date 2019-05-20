

/****************************************************************************
*
*  system\meta\rx_obj_types.cpp
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


// rx_obj_types
#include "system/meta/rx_obj_types.h"

#include "rx_configuration.h"
#include "system/runtime/rx_objbase.h"
#include "sys_internal/rx_internal_ns.h"
#include "model/rx_meta_internals.h"
using namespace rx_platform::meta::meta_algorithm;


namespace rx_platform {

namespace meta {

namespace object_types {

// Class rx_platform::meta::object_types::application_type 

rx_item_type application_type::type_id = rx_item_type::rx_application_type;

application_type::application_type()
{
}

application_type::application_type (const object_type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}


application_type::~application_type()
{
}



rx_result application_type::construct (rx_application_ptr& what, construct_context& ctx) const
{
	return object_types_algorithm<application_type>::construct_object(*this, what, ctx);
}

platform_item_ptr application_type::get_item_ptr () const
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result application_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<application_type>::serialize_object_type(*this, stream, type);
}

rx_result application_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<application_type>::deserialize_object_type(*this, stream, type);
}

meta_data& application_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& application_type::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& application_type::mapping_data ()
{
  return mapping_data_;

}

rx_result application_type::check_type (type_check_context& ctx)
{
	return object_types_algorithm<application_type>::check_object_type(*this, ctx);
}

rx_result application_type::resolve (rx_directory_ptr dir)
{
	return object_types_algorithm<application_type>::resolve_object_type(*this, dir);
}

void application_type::set_runtime_data (runtime_data_prototype& prototype, RTypePtr where)
{
	where->runtime_.runtime.set_runtime_data(prototype);
}

void application_type::set_instance_data (instance_data_t&& data, RTypePtr where)
{
}


const object_data_type& application_type::object_data () const
{
  return object_data_;
}

const meta_data& application_type::meta_info () const
{
  return meta_info_;
}

const def_blocks::complex_data_type& application_type::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& application_type::mapping_data () const
{
  return mapping_data_;
}


// Class rx_platform::meta::object_types::domain_type 

rx_item_type domain_type::type_id = rx_item_type::rx_domain_type;

domain_type::domain_type()
{
}

domain_type::domain_type (const object_type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}


domain_type::~domain_type()
{
}



rx_result domain_type::construct (rx_domain_ptr what, construct_context& ctx) const
{
	auto result = object_types_algorithm<domain_type>::construct_object(*this, what, ctx);
	if (result)
	{
		//what->objects_
	}
	return result;
}

platform_item_ptr domain_type::get_item_ptr () const
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result domain_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<domain_type>::serialize_object_type(*this, stream, type);
}

rx_result domain_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<domain_type>::deserialize_object_type(*this, stream, type);
}

meta_data& domain_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& domain_type::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& domain_type::mapping_data ()
{
  return mapping_data_;

}

bool domain_type::check_type (type_check_context& ctx)
{
	return object_types_algorithm<domain_type>::check_object_type(*this, ctx);
}

rx_result domain_type::resolve (rx_directory_ptr dir)
{
	return object_types_algorithm<domain_type>::resolve_object_type(*this, dir);
}

void domain_type::set_runtime_data (runtime_data_prototype& prototype, RTypePtr where)
{
	where->runtime_.runtime.set_runtime_data(prototype);
}

void domain_type::set_instance_data (instance_data_t&& data, RTypePtr where)
{
}


const object_data_type& domain_type::object_data () const
{
  return object_data_;
}

const meta_data& domain_type::meta_info () const
{
  return meta_info_;
}

const def_blocks::complex_data_type& domain_type::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& domain_type::mapping_data () const
{
  return mapping_data_;
}


// Class rx_platform::meta::object_types::object_type 

rx_item_type object_type::type_id = rx_item_type::rx_object_type;

object_type::object_type()
{
}

object_type::object_type (const object_type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}


object_type::~object_type()
{
}



void object_type::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

rx_result object_type::construct (runtime::object_runtime_ptr what, construct_context& ctx) const
{
	auto result = object_types_algorithm<object_type>::construct_object(*this, what, ctx);
	if (result)
	{
		if (what->instance_data_.domain_id)
		{
			auto domain_ptr = model::platform_types_manager::instance().internal_get_type_cache<domain_type>().get_runtime(what->instance_data_.domain_id);
			domain_ptr->objects_.emplace_back(what);
			what->connect_domain(std::move(domain_ptr));
		}
		else
		{
			META_LOG_WARNING("object_type::construct", 900, "Domain Id is null, connecting object to unassigned domain.");
			auto domain_ptr = rx_gate::instance().get_manager().get_unassigned_domain();
			if(domain_ptr)
				domain_ptr->objects_.emplace_back(what);
			what->connect_domain(std::move(domain_ptr));
		}
		if (!what->my_domain_)
		{
			std::ostringstream message;
			message << "Unable to connect to domain " 
				<< what->instance_data_.domain_id ? what->instance_data_.domain_id.to_string() : RX_NULL_ITEM_NAME;
			result = rx_result(message.str());
		}
	}
	return result;
}

platform_item_ptr object_type::get_item_ptr () const
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result object_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<object_type>::serialize_object_type(*this, stream, type);
}

rx_result object_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<object_type>::deserialize_object_type(*this, stream, type);
}

meta_data& object_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& object_type::complex_data ()
{
  return complex_data_;

}

void object_type::set_runtime_data (runtime_data_prototype& prototype, RTypePtr where)
{
	where->runtime_.runtime.set_runtime_data(prototype);
}

def_blocks::mapped_data_type& object_type::mapping_data ()
{
  return mapping_data_;

}

bool object_type::check_type (type_check_context& ctx)
{
	return object_types_algorithm<object_type>::check_object_type(*this, ctx);
}

rx_result object_type::resolve (rx_directory_ptr dir)
{
	return object_types_algorithm<object_type>::resolve_object_type(*this, dir);
}

void object_type::set_instance_data (instance_data_t&& data, RTypePtr where)
{
}


const object_data_type& object_type::object_data () const
{
  return object_data_;
}

const meta_data& object_type::meta_info () const
{
  return meta_info_;
}

const def_blocks::complex_data_type& object_type::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& object_type::mapping_data () const
{
  return mapping_data_;
}


// Class rx_platform::meta::object_types::object_data_type 

object_data_type::object_data_type()
{
}

object_data_type::object_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



rx_result object_data_type::serialize_object_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.write_bool("constructable", constructable_))
		return false;
	return true;
}

rx_result object_data_type::deserialize_object_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_bool("constructable", constructable_))
		return false;
	return true;
}

rx_result object_data_type::construct (runtime::blocks::runtime_object& what, construct_context& ctx) const
{
	return true;
}

bool object_data_type::check_type (type_check_context& ctx)
{
	return true;
}

rx_result object_data_type::resolve (rx_directory_ptr dir)
{
	// nothing yet to resolve here
	return true;
}


// Class rx_platform::meta::object_types::port_type 

rx_item_type port_type::type_id = rx_item_type::rx_port_type;

port_type::port_type()
{
}

port_type::port_type (const object_type_creation_data& data)
	: meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}


port_type::~port_type()
{
}



rx_result port_type::construct (rx_port_ptr what, construct_context& ctx) const
{
	return object_types_algorithm<port_type>::construct_object(*this, what, ctx);
}

platform_item_ptr port_type::get_item_ptr () const
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result port_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<port_type>::serialize_object_type(*this, stream, type);
}

rx_result port_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<port_type>::deserialize_object_type(*this, stream, type);
}

meta_data& port_type::meta_info ()
{
  return meta_info_;

}

def_blocks::complex_data_type& port_type::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& port_type::mapping_data ()
{
  return mapping_data_;

}

bool port_type::check_type (type_check_context& ctx)
{
	return object_types_algorithm<port_type>::check_object_type(*this, ctx);
}

rx_result port_type::resolve (rx_directory_ptr dir)
{
	return object_types_algorithm<port_type>::resolve_object_type(*this, dir);
}

void port_type::set_runtime_data (runtime_data_prototype& prototype, RTypePtr where)
{
	where->runtime_.runtime.set_runtime_data(prototype);
}

void port_type::set_instance_data (instance_data_t&& data, RTypePtr where)
{
}


const object_data_type& port_type::object_data () const
{
  return object_data_;
}

const meta_data& port_type::meta_info () const
{
  return meta_info_;
}

const def_blocks::complex_data_type& port_type::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& port_type::mapping_data () const
{
  return mapping_data_;
}


} // namespace object_types
} // namespace meta
} // namespace rx_platform

