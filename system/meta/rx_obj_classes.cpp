

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

class obj_meta_helpers
{
public:
	template<class objectT>
	static bool serialize_object_class(const objectT& whose, base_meta_writer& stream, uint8_t type)
	{
		if (!whose.meta_data_.serialize_checkable_definition(stream, type))
			return false;
		if (!whose.object_data_.serialize_object_definition(stream, type))
			return false;
		if (!whose.complex_data_.serialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data_.serialize_mapped_definition(stream, type))
			return false;
		return true;
	}

	template<class objectT>
	static bool deserialize_object_class(objectT& whose, base_meta_reader& stream, uint8_t type)
	{
		if (!whose.meta_data_.deserialize_checkable_definition(stream, type))
			return false;
		if (!whose.object_data_.deserialize_object_definition(stream, type))
			return false;
		if (!whose.complex_data_.deserialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data_.deserialize_mapped_definition(stream, type))
			return false;
		return true;
	}

};

// Class rx_platform::meta::object_defs::application_class 

string_type application_class::type_name = RX_CPP_APPLICATION_CLASS_TYPE_NAME;

application_class::application_class()
{
}

application_class::application_class (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
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
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type))
		return false;
	return true;
}

bool application_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
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

domain_class::domain_class (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
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
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type))
		return false;
	return true;
}

bool domain_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
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

object_class::object_class (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
{
}


object_class::~object_class()
{
}



void object_class::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
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
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type))
		return false;
	return true;
}

bool object_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
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
	if (!stream.write_bool("Creatable", creatable_))
		return false;
	return true;
}

bool object_data_type::deserialize_object_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_bool("Creatable", creatable_))
		return false;
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

port_class::port_class (const object_type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_object_types_from_flags(data))
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
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type))
		return false;
	return true;
}

bool port_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
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


// Class rx_platform::meta::object_defs::object_type_creation_data 


} // namespace object_defs
} // namespace meta
} // namespace rx_platform

