

/****************************************************************************
*
*  system\meta\rx_obj_classes.cpp
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


#include "pch.h"


// rx_obj_classes
#include "system/meta/rx_obj_classes.h"

#include "rx_configuration.h"
#include "system/meta/rx_objbase.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace meta {

namespace object_defs {
class obj_meta_helpers
{
public:
	template<class objectT>
	static bool serialize_object_class(const objectT& whose, base_meta_writer& stream, uint8_t type)
	{
		if (!whose.meta_data_.serialize_checkable_definition(stream, type))
			return false;
		if (!whose.complex_data_.serialize_complex_definition(stream, type))
			return false;
		return true;
	}

	template<class objectT>
	static bool deserialize_object_class(objectT& whose, base_meta_reader& stream, uint8_t type)
	{
		return false;
	}

};

// Class rx_platform::meta::object_defs::application_class 

string_type application_class::type_name = RX_CPP_APPLICATION_CLASS_TYPE_NAME;

application_class::application_class()
{
}

application_class::application_class (const string_type& name, const rx_node_id& id, bool system)
{
}


application_class::~application_class()
{
}



void application_class::construct (objects::object_runtime_ptr what)
{
}

void application_class::construct (complex_runtime_ptr what)
{
}

platform_item_ptr application_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool application_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return obj_meta_helpers::serialize_object_class<application_class>(*this, stream, type);
}

bool application_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return obj_meta_helpers::deserialize_object_class<application_class>(*this, stream, type);
}

checkable_data& application_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& application_class::complex_data ()
{
  return complex_data_;

}


const object_data_type& application_class::object_data () const
{
  return object_data_;
}

const checkable_data& application_class::meta_data () const
{
  return meta_data_;
}

const def_blocks::complex_data_type& application_class::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& application_class::mapping_data () const
{
  return mapping_data_;
}


// Class rx_platform::meta::object_defs::domain_class 

string_type domain_class::type_name = RX_CPP_DOMAIN_CLASS_TYPE_NAME;

domain_class::domain_class()
{
}

domain_class::domain_class (const string_type& name, const rx_node_id& id, bool system)
	: meta_data_(name,id,system)
{
}


domain_class::~domain_class()
{
}



void domain_class::construct (objects::object_runtime_ptr what)
{
}

void domain_class::construct (complex_runtime_ptr what)
{
}

platform_item_ptr domain_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool domain_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return obj_meta_helpers::serialize_object_class<domain_class>(*this, stream, type);
}

bool domain_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return obj_meta_helpers::deserialize_object_class<domain_class>(*this, stream, type);
}

checkable_data& domain_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& domain_class::complex_data ()
{
  return complex_data_;

}


const object_data_type& domain_class::object_data () const
{
  return object_data_;
}

const checkable_data& domain_class::meta_data () const
{
  return meta_data_;
}

const def_blocks::complex_data_type& domain_class::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& domain_class::mapping_data () const
{
  return mapping_data_;
}


// Class rx_platform::meta::object_defs::object_class 

string_type object_class::type_name = RX_CPP_OBJECT_CLASS_TYPE_NAME;

object_class::object_class()
{
}

object_class::object_class (const string_type& name, const rx_node_id& id, bool system)
	: meta_data_(name, id, system)
{
}


object_class::~object_class()
{
}



void object_class::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

namespace_item_attributes object_class::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_class | namespace_item_attributes::namespace_item_execute_access | namespace_item_attributes::namespace_item_read_access | (meta_data_.get_system() ?  namespace_item_attributes::namespace_item_system : namespace_item_attributes::namespace_item_null) );
}

void object_class::construct (objects::object_runtime_ptr what)
{
	complex_data_.construct(what->get_complex_item());
	object_data_.construct(what);
}

platform_item_ptr object_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool object_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return obj_meta_helpers::serialize_object_class<object_class>(*this, stream, type);
}

bool object_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return obj_meta_helpers::deserialize_object_class<object_class>(*this, stream, type);
}

objects::object_runtime_ptr object_class::create_runtime_ptr ()
{
	return rx_create_reference<objects::object_types::object_runtime>();
}

checkable_data& object_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& object_class::complex_data ()
{
  return complex_data_;

}


const object_data_type& object_class::object_data () const
{
  return object_data_;
}

const checkable_data& object_class::meta_data () const
{
  return meta_data_;
}

const def_blocks::complex_data_type& object_class::complex_data () const
{
  return complex_data_;
}


// Class rx_platform::meta::object_defs::object_data_type 

object_data_type::object_data_type()
{
}

object_data_type::object_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



bool object_data_type::serialize_object_definition (base_meta_writer& stream, uint8_t type) const
{
	
	return true;
}

bool object_data_type::deserialize_object_definition (base_meta_reader& stream, uint8_t type)
{
	
	return true;
}

void object_data_type::construct (objects::object_runtime_ptr what)
{
}


// Class rx_platform::meta::object_defs::port_class 

string_type port_class::type_name = RX_CPP_PORT_CLASS_TYPE_NAME;

port_class::port_class()
{
}

port_class::port_class (const string_type& name, const rx_node_id& id, bool system)
{
}


port_class::~port_class()
{
}



void port_class::construct (objects::object_runtime_ptr what)
{
}

void port_class::construct (complex_runtime_ptr what)
{
}

platform_item_ptr port_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool port_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return obj_meta_helpers::serialize_object_class<port_class>(*this, stream, type);
}

bool port_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return obj_meta_helpers::deserialize_object_class<port_class>(*this, stream, type);
}

checkable_data& port_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& port_class::complex_data ()
{
  return complex_data_;

}


const object_data_type& port_class::object_data () const
{
  return object_data_;
}

const checkable_data& port_class::meta_data () const
{
  return meta_data_;
}

const def_blocks::complex_data_type& port_class::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& port_class::mapping_data () const
{
  return mapping_data_;
}


} // namespace object_defs
} // namespace meta
} // namespace rx_platform

