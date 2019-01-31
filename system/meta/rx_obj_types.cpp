

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
	static bool serialize_object_class(const objectT& whose, base_meta_writer& stream, uint8_t type, const string_type object_type)
	{
		if (!whose.meta_data_.serialize_checkable_definition(stream, type, object_type))
			return false;
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data_.serialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data_.serialize_mapped_definition(stream, type))
			return false;
		if (!whose.object_data_.serialize_object_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}

	template<class objectT>
	static bool deserialize_object_class(objectT& whose, base_meta_reader& stream, uint8_t type)
	{
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data_.deserialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data_.deserialize_mapped_definition(stream, type))
			return false;
		if (!whose.object_data_.deserialize_object_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}

	template<class objectT>
	static void construct_object_runtime(objectT& whose, typename objectT::RTypePtr what, construct_context& ctx)
	{
		whose.complex_data_.construct(ctx);
		whose.mapping_data_.construct(whose.complex_data_.get_names_cache(), ctx);
		whose.object_data_.construct(what, ctx);
	}

};

// Class rx_platform::meta::object_defs::application_type 

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



void application_type::construct (runtime::rx_application_ptr& what, construct_context& ctx)
{
	obj_meta_helpers::construct_object_runtime(*this, what, ctx);
}

platform_item_ptr application_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool application_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type, type_name))
		return false;
	return true;
}

bool application_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
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


// Class rx_platform::meta::object_defs::domain_type 

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



void domain_type::construct (runtime::rx_domain_ptr what, construct_context& ctx)
{
	obj_meta_helpers::construct_object_runtime(*this, what, ctx);
}

platform_item_ptr domain_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool domain_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type, type_name))
		return false;
	return true;
}

bool domain_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
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


// Class rx_platform::meta::object_defs::object_type 

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

void object_type::construct (runtime::object_runtime_ptr what, construct_context& ctx)
{
	obj_meta_helpers::construct_object_runtime(*this, what, ctx);
}

platform_item_ptr object_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool object_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type, type_name))
		return false;
	return true;
}

bool object_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
}

checkable_data& object_type::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& object_type::complex_data ()
{
  return complex_data_;

}

void object_type::set_object_runtime_data (def_blocks::runtime_data_prototype& prototype, RTypePtr where)
{
	where->item_ = std::move(create_runtime_data(prototype));
}

def_blocks::mapped_data_type& object_type::mapping_data ()
{
  return mapping_data_;

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

void object_data_type::construct (runtime::object_runtime_ptr what, construct_context& ctx)
{
}


// Class rx_platform::meta::object_defs::port_type 

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



void port_type::construct (runtime::rx_port_ptr what, construct_context& ctx)
{
	complex_data_.construct(ctx);
	object_data_.construct(what, ctx);
}

platform_item_ptr port_type::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool port_type::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!obj_meta_helpers::serialize_object_class(*this, stream, type, type_name))
		return false;
	return true;
}

bool port_type::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!obj_meta_helpers::deserialize_object_class(*this, stream, type))
		return false;
	return true;
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


// Class rx_platform::meta::object_defs::object_type_creation_data 


} // namespace object_defs
} // namespace meta
} // namespace rx_platform

