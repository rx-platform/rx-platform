

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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_obj_types
#include "system/meta/rx_obj_types.h"

#include "rx_configuration.h"
#include "system/runtime/rx_objbase.h"
#include "sys_internal/rx_internal_ns.h"
using namespace rx_platform::meta::meta_algorithm;


namespace rx_platform {

namespace meta {

namespace object_types {
namespace_item_attributes create_attributes_for_object_types_from_flags(const object_type_creation_data& data)
{
	if (data.system)
	{
		return namespace_item_system_access;
	}
	else
	{
		return namespace_item_full_access;
	}
}

// Class rx_platform::meta::object_types::application_type 

string_type application_type::type_name = RX_CPP_APPLICATION_CLASS_TYPE_NAME;

application_type::application_type()
{
}

application_type::application_type (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
{
}


application_type::~application_type()
{
}



void application_type::construct (runtime::rx_application_ptr& what, construct_context& ctx) const
{
	object_types_algorithm<application_type>::construct_object(*this, what, ctx);
}

platform_item_ptr application_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool application_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<application_type>::serialize_object_type(*this, stream, type);
}

bool application_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<application_type>::deserialize_object_type(*this, stream, type);
}

checkable_data& application_type::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& application_type::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& application_type::mapping_data ()
{
  return mapping_data_;

}

bool application_type::check_type (type_check_context& ctx)
{
	return object_types_algorithm<application_type>::check_object_type(*this, ctx);
}


const object_data_type& application_type::object_data () const
{
  return object_data_;
}

const checkable_data& application_type::meta_data () const
{
  return meta_data_;
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

string_type domain_type::type_name = RX_CPP_DOMAIN_CLASS_TYPE_NAME;

domain_type::domain_type()
{
}

domain_type::domain_type (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
{
}


domain_type::~domain_type()
{
}



void domain_type::construct (runtime::rx_domain_ptr what, construct_context& ctx) const
{
	object_types_algorithm<domain_type>::construct_object(*this, what, ctx);
}

platform_item_ptr domain_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool domain_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<domain_type>::serialize_object_type(*this, stream, type);
}

bool domain_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<domain_type>::deserialize_object_type(*this, stream, type);
}

checkable_data& domain_type::meta_data ()
{
  return meta_data_;

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


const object_data_type& domain_type::object_data () const
{
  return object_data_;
}

const checkable_data& domain_type::meta_data () const
{
  return meta_data_;
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

string_type object_type::type_name = RX_CPP_OBJECT_CLASS_TYPE_NAME;

object_type::object_type()
{
}

object_type::object_type (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
{
}


object_type::~object_type()
{
}



void object_type::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

void object_type::construct (runtime::object_runtime_ptr what, construct_context& ctx) const
{
	object_types_algorithm<object_type>::construct_object(*this, what, ctx);
}

platform_item_ptr object_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool object_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<object_type>::serialize_object_type(*this, stream, type);
}

bool object_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<object_type>::deserialize_object_type(*this, stream, type);
}

checkable_data& object_type::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& object_type::complex_data ()
{
  return complex_data_;

}

void object_type::set_object_runtime_data (runtime_data_prototype& prototype, RTypePtr where)
{
	where->item_ = std::move(create_runtime_data(prototype));
}

def_blocks::mapped_data_type& object_type::mapping_data ()
{
  return mapping_data_;

}

bool object_type::check_type (type_check_context& ctx)
{
	return object_types_algorithm<object_type>::check_object_type(*this, ctx);
}


const object_data_type& object_type::object_data () const
{
  return object_data_;
}

const checkable_data& object_type::meta_data () const
{
  return meta_data_;
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



bool object_data_type::serialize_object_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.write_bool("Constructible", constructible_))
		return false;
	return true;
}

bool object_data_type::deserialize_object_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_bool("Constructible", constructible_))
		return false;
	return true;
}

void object_data_type::construct (runtime::object_runtime_ptr what, construct_context& ctx) const
{
}

bool object_data_type::check_type (type_check_context& ctx)
{
	return true;
}


// Class rx_platform::meta::object_types::port_type 

string_type port_type::type_name = RX_CPP_PORT_CLASS_TYPE_NAME;

port_type::port_type()
{
}

port_type::port_type (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
{
}


port_type::~port_type()
{
}



void port_type::construct (runtime::rx_port_ptr what, construct_context& ctx) const
{
	object_types_algorithm<port_type>::construct_object(*this, what, ctx);
}

platform_item_ptr port_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool port_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<port_type>::serialize_object_type(*this, stream, type);
}

bool port_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<port_type>::deserialize_object_type(*this, stream, type);
}

checkable_data& port_type::meta_data ()
{
  return meta_data_;

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


const object_data_type& port_type::object_data () const
{
  return object_data_;
}

const checkable_data& port_type::meta_data () const
{
  return meta_data_;
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

