

/****************************************************************************
*
*  system\meta\rx_obj_types.cpp
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


// rx_meta_algorithm
#include "system/meta/rx_meta_algorithm.h"
// rx_obj_types
#include "system/meta/rx_obj_types.h"

#include "rx_configuration.h"
#include "system/runtime/rx_objbase.h"
#include "sys_internal/rx_internal_ns.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "system/runtime/rx_io_relations.h"
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
  return std::make_unique<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

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

bool application_type::check_type (type_check_context& ctx)
{
    return object_types_algorithm<application_type>::check_object_type(*this, ctx);
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
  return std::make_unique<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

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

rx_result object_type::construct (rx_object_ptr what, construct_context& ctx) const
{
	auto result = object_types_algorithm<object_type>::construct_object(*this, what, ctx);
	
	return result;
}

platform_item_ptr object_type::get_item_ptr () const
{
  return std::make_unique<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

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



rx_result object_data_type::resolve (rx_directory_ptr dir)
{
	// nothing yet to resolve here
	return true;
}

bool object_data_type::check_type (type_check_context& ctx)
{
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
  return std::make_unique<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

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


// Class rx_platform::meta::object_types::relation_attribute 

relation_attribute::relation_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
{
}

relation_attribute::relation_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
{
}



rx_result relation_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return relation_blocks_algorithm::serialize_relation_attribute(*this, stream);
}

rx_result relation_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return relation_blocks_algorithm::deserialize_relation_attribute(*this, stream);
}

rx_result relation_attribute::check (type_check_context& ctx)
{
	return true;
}

rx_result_with<runtime::relation_runtime_ptr> relation_attribute::construct (construct_context& ctx) const
{
	return meta_algorithm::relation_blocks_algorithm::construct_relation_attribute(*this, ctx);	
}


// Class rx_platform::meta::object_types::relation_type 

rx_item_type relation_type::type_id = rx_item_type::rx_relation_type;

relation_type::relation_type()
      : hierarchical_(false)
{
}

relation_type::relation_type (const object_type_creation_data& data)
      : hierarchical_(false)
	, meta_info_(data.name, data.id, data.base_id, data.attributes, data.path)
{
}



platform_item_ptr relation_type::get_item_ptr () const
{
  return std::make_unique<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_result relation_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return object_types_algorithm<relation_type>::serialize_object_type(*this, stream, type);
}

rx_result relation_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return object_types_algorithm<relation_type>::deserialize_object_type(*this, stream, type);
}

meta_data& relation_type::meta_info ()
{
  return meta_info_;

}

rx_result_with<runtime::relation_runtime_ptr> relation_type::construct (runtime::relation_runtime_ptr what, construct_context& ctx) const
{
	return RX_NOT_IMPLEMENTED;
}

bool relation_type::check_type (type_check_context& ctx)
{
	return object_types_algorithm<relation_type>::check_object_type(*this, ctx);
}

void relation_type::set_runtime_data (runtime_data_prototype& prototype, RTypePtr where)
{
}

void relation_type::set_instance_data (instance_data_t&& data, RTypePtr where)
{
}


const meta_data& relation_type::meta_info () const
{
  return meta_info_;
}

const relation_data_type& relation_type::complex_data () const
{
  return complex_data_;
}


// Class rx_platform::meta::object_types::relation_data_type 


rx::data::runtime_values_data& relation_data_type::get_overrides () const
{
	static rx::data::runtime_values_data dummy;
	return dummy;
}


} // namespace object_types
} // namespace meta
} // namespace rx_platform

