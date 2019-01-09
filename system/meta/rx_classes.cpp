

/****************************************************************************
*
*  system\meta\rx_classes.cpp
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

#include "rx_configuration.h"

// rx_classes
#include "system/meta/rx_classes.h"

#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace meta {

namespace basic_defs {

namespace_item_attributes create_attributes_for_basic_types_from_flags(bool system)
{
	if (system)
	{
		return namespace_item_system_access;
	}
	else
	{
		return namespace_item_full_access;
	}
}
class meta_helpers
{
public:
	template<class complexT>
	static bool serialize_simple_class(const complexT& whose, base_meta_writer& stream, uint8_t type)
	{
		if (!whose.meta_data().serialize_checkable_definition(stream, type))
			return false;
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data().serialize_complex_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}

	template<class complexT>
	static bool deserialize_simple_class(complexT& whose, base_meta_reader& stream, uint8_t type)
	{
		if (!whose.meta_data().deserialize_checkable_definition(stream, type))
			return false;
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data().deserialize_complex_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}


	template<class complexT>
	static bool serialize_struct_class(const complexT& whose, base_meta_writer& stream, uint8_t type)
	{
		if (!whose.meta_data().serialize_checkable_definition(stream, type))
			return false;
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data().serialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data().serialize_mapped_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}

	template<class complexT>
	static bool deserialize_struct_class(complexT& whose, base_meta_reader& stream, uint8_t type)
	{
		if (!whose.meta_data().deserialize_checkable_definition(stream, type))
			return false;
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data().deserialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data().deserialize_mapped_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}

	template<class complexT>
	static bool serialize_variable_class(const complexT& whose, base_meta_writer& stream, uint8_t type)
	{
		if (!whose.meta_data().serialize_checkable_definition(stream, type))
			return false;
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data().serialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data().serialize_mapped_definition(stream, type))
			return false;
		if (!whose.variable_data().serialize_variable_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}

	template<class complexT>
	static bool deserialize_variable_class(complexT& whose, base_meta_reader& stream, uint8_t type)
	{
		if (!whose.meta_data().deserialize_checkable_definition(stream, type))
			return false;
		if (!stream.start_object("Def"))
			return false;
		if (!whose.complex_data().deserialize_complex_definition(stream, type))
			return false;
		if (!whose.mapping_data().deserialize_mapped_definition(stream, type))
			return false;
		if (!whose.variable_data().deserialize_variable_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
		return true;
	}
};

// Class rx_platform::meta::basic_defs::event_class 

string_type event_class::type_name = RX_CPP_EVENT_CLASS_TYPE_NAME;

event_class::event_class()
{
}

event_class::event_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system)
	: meta_data_(name, id, base_id, create_attributes_for_basic_types_from_flags(system))
{
}



platform_item_ptr event_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool event_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_simple_class(*this, stream, type))
		return false;
	return true;
}

bool event_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_simple_class(*this, stream, type))
		return false;
	return true;
}

checkable_data& event_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& event_class::complex_data ()
{
  return complex_data_;

}

event_class::RTypePtr event_class::create_runtime_ptr ()
{
	return std::make_unique<RType>();
}

void event_class::construct (RType& what, construct_context& ctx)
{
	complex_data_.construct(what, ctx);
}


const def_blocks::complex_data_type& event_class::complex_data () const
{
  return complex_data_;
}

const checkable_data& event_class::meta_data () const
{
  return meta_data_;
}


// Class rx_platform::meta::basic_defs::filter_class 

string_type filter_class::type_name = RX_CPP_FILTER_CLASS_TYPE_NAME;

filter_class::filter_class()
{
}

filter_class::filter_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system)
	: meta_data_(name, id, base_id, create_attributes_for_basic_types_from_flags(system))
{
}



platform_item_ptr filter_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool filter_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_simple_class(*this, stream, type))
		return false;
	return true;
}

bool filter_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_simple_class(*this, stream, type))
		return false;
	return true;
}

checkable_data& filter_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& filter_class::complex_data ()
{
  return complex_data_;

}

filter_class::RTypePtr filter_class::create_runtime_ptr ()
{
	return rx_create_reference<runtime::blocks::filter_runtime>();
}

void filter_class::construct (RType& what, construct_context& ctx)
{
	complex_data_.construct(what, ctx);
}


const def_blocks::complex_data_type& filter_class::complex_data () const
{
  return complex_data_;
}

const checkable_data& filter_class::meta_data () const
{
  return meta_data_;
}


// Class rx_platform::meta::basic_defs::mapper_class 

string_type mapper_class::type_name = RX_CPP_MAPPER_CLASS_TYPE_NAME;

mapper_class::mapper_class()
{
}

mapper_class::mapper_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system)
	: meta_data_(name, id, base_id, create_attributes_for_basic_types_from_flags(system))
{
}



platform_item_ptr mapper_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool mapper_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_simple_class(*this, stream, type))
		return false;
	return true;
}

bool mapper_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_simple_class(*this, stream, type))
		return false;
	return true;
}

checkable_data& mapper_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& mapper_class::complex_data ()
{
  return complex_data_;

}

mapper_class::RTypePtr mapper_class::create_runtime_ptr ()
{
	return rx_create_reference<runtime::blocks::mapper_runtime>();
}

void mapper_class::construct (RType& what, construct_context& ctx)
{
}


const checkable_data& mapper_class::meta_data () const
{
  return meta_data_;
}

const def_blocks::complex_data_type& mapper_class::complex_data () const
{
  return complex_data_;
}


// Class rx_platform::meta::basic_defs::source_class 

string_type source_class::type_name = RX_CPP_SOURCE_CLASS_TYPE_NAME;

source_class::source_class()
{
}

source_class::source_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system)
	: meta_data_(name, id, base_id, create_attributes_for_basic_types_from_flags(system))
{
}



platform_item_ptr source_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool source_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_simple_class(*this, stream, type))
		return false;
	return true;
}

bool source_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_simple_class(*this, stream, type))
		return false;
	return true;
}

checkable_data& source_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& source_class::complex_data ()
{
  return complex_data_;

}

source_class::RTypePtr source_class::create_runtime_ptr ()
{
	return rx_create_reference<runtime::blocks::source_runtime>();
}

void source_class::construct (RType& what, construct_context& ctx)
{
}


const def_blocks::complex_data_type& source_class::complex_data () const
{
  return complex_data_;
}

const checkable_data& source_class::meta_data () const
{
  return meta_data_;
}


// Class rx_platform::meta::basic_defs::struct_class 

string_type struct_class::type_name = RX_CPP_STRUCT_CLASS_TYPE_NAME;

struct_class::struct_class()
{
}

struct_class::struct_class (const type_creation_data& data)
	: meta_data_(data.name, data.id, data.base_id, create_attributes_for_basic_types_from_flags(data.system))
{
}



void struct_class::construct (RType& what, construct_context& ctx)
{
	complex_data_.construct(what, ctx);
	mapping_data_.construct(what, complex_data_.get_names_cache(), ctx);
}

platform_item_ptr struct_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool struct_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_struct_class(*this, stream, type))
		return false;
	return true;
}

bool struct_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_struct_class(*this, stream, type))
		return false;
	return true;
}

struct_class::RTypePtr struct_class::create_runtime_ptr ()
{
	return rx_create_reference<runtime::blocks::struct_runtime>();
}

checkable_data& struct_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& struct_class::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& struct_class::mapping_data ()
{
  return mapping_data_;

}


const def_blocks::complex_data_type& struct_class::complex_data () const
{
  return complex_data_;
}

const def_blocks::mapped_data_type& struct_class::mapping_data () const
{
  return mapping_data_;
}

const checkable_data& struct_class::meta_data () const
{
  return meta_data_;
}


// Class rx_platform::meta::basic_defs::variable_class 

string_type variable_class::type_name = RX_CPP_VARIABLE_CLASS_TYPE_NAME;

variable_class::variable_class()
{
}

variable_class::variable_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system)
	: meta_data_(name, id, base_id, create_attributes_for_basic_types_from_flags(system))
{
}



void variable_class::construct (RType& what, construct_context& ctx)
{
}

bool variable_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_variable_class(*this, stream, type))
		return false;
	return true;
}

bool variable_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_variable_class(*this, stream, type))
		return false;
	return true;
}

platform_item_ptr variable_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_value variable_class::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_data_.get_version(), meta_data_.get_created_time());
	return temp;
}

checkable_data& variable_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& variable_class::complex_data ()
{
  return complex_data_;

}

def_blocks::mapped_data_type& variable_class::mapping_data ()
{
  return mapping_data_;

}

def_blocks::variable_data_type& variable_class::variable_data ()
{
  return variable_data_;

}

variable_class::RTypePtr variable_class::create_runtime_ptr ()
{
	return rx_create_reference<runtime::blocks::variable_runtime>();
}


const checkable_data& variable_class::meta_data () const
{
  return meta_data_;
}

const def_blocks::variable_data_type& variable_class::variable_data () const
{
  return variable_data_;
}

const def_blocks::mapped_data_type& variable_class::mapping_data () const
{
  return mapping_data_;
}

const def_blocks::complex_data_type& variable_class::complex_data () const
{
  return complex_data_;
}


// Class rx_platform::meta::basic_defs::type_creation_data 


} // namespace basic_defs
} // namespace meta
} // namespace rx_platform

//#define RX_TEMPLATE_INST(typeArg, brwArg) \
//template class rx_platform::meta::checkable_type<typeArg, brwArg>;\
//
//RX_TEMPLATE_INST(rx_platform::meta::object_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::domain_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::application_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::port_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::struct_class, false);
//
//
//template class rx_platform::meta::variable_data_type<rx_platform::meta::complex_data_type>;
//RX_TEMPLATE_INST(rx_platform::meta::variable_class, false);
//
//#define RX_TEMPLATE_INST_SIMPLE(typeArg, brwArg) \
//template class rx_platform::meta::checkable_type<typeArg, brwArg>;\
//
//RX_TEMPLATE_INST_SIMPLE(rx_platform::runtime::object_runtime, true);
//RX_TEMPLATE_INST_SIMPLE(rx_platform::logic::program_runtime, false);

