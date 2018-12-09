

/****************************************************************************
*
*  system\meta\rx_classes.cpp
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

#include "rx_configuration.h"

// rx_blocks
#include "system/meta/rx_blocks.h"
// rx_objbase
#include "system/meta/rx_objbase.h"
// rx_classes
#include "system/meta/rx_classes.h"

#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace meta {

// Class rx_platform::meta::checkable_data 

checkable_data::checkable_data()
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
{
}

checkable_data::checkable_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system)
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
	, id_(id)
	, system_(system)
{
}



bool checkable_data::serialize_node (base_meta_writer& stream, uint8_t type, const rx_value_union& value) const
{
	if (!stream.write_header(type, 0))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, type);*/
	/*if(!ret)
		return false;
*/
	if (!stream.write_footer())
		return false;

	return true;
}

bool checkable_data::deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value)
{
	return false;
}

bool checkable_data::check_in (base_meta_reader& stream)
{
	return false;
}

bool checkable_data::check_out (base_meta_writer& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_CHECKOUT, 0))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, STREAMING_TYPE_CHECKOUT);
*/

	if (!stream.write_footer())
		return false;

	return true;
}

bool checkable_data::serialize_checkable_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.write_id("NodeId", id_))
		return false;
	if (!stream.write_bool("System", system_))
		return false;
	if (!stream.write_string("Name", name_.c_str()))
		return false;
	if (!stream.write_id("SuperId", parent_))
		return false;
	if (!stream.write_version("Ver", version_))
		return false;
	return true;
}

bool checkable_data::deserialize_checkable_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_id("NodeId", id_))
		return false;
	if (!stream.read_bool("System", system_))
		return false;
	return true;
	if (!stream.read_string("Name", name_))
		return false;
	return true;
	if (!stream.read_id("Parent", parent_))
		return false;
	if (!stream.read_version("Ver", version_))
		return false;
	return true;
}

values::rx_value checkable_data::get_value () const
{
	values::rx_value temp;
	temp.assign_static(version_, modified_time_);
	return temp;
}

void checkable_data::construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, bool system)
{
	name_ = name;
	id_ = id;
	parent_ = type_id;
}


namespace basic_defs {
	class meta_helpers
	{
	public:
		template<class complexT>
		static bool serialize_complex_class(const complexT& whose, base_meta_writer& stream, uint8_t type)
		{
			if (!whose.meta_data().serialize_checkable_definition(stream, type))
				return false;
			if (!whose.complex_data().serialize_complex_definition(stream, type))
				return false;
			return true;
		}

		template<class complexT>
		static bool deserialize_complex_class(complexT& whose, base_meta_reader& stream, uint8_t type)
		{
			return false;
		}


		template<class complexT>
		static bool serialize_mapped_class(const complexT& whose, base_meta_writer& stream, uint8_t type)
		{
			if (!serialize_complex_class(whose, stream, type))
				return false;
			if (!whose.mapping_data().serialize_mapped_definition(stream, type))
				return false;
			return true;
		}

		template<class complexT>
		static bool deserialize_mapped_class(complexT& whose, base_meta_reader& stream, uint8_t type)
		{
			return false;
		}

		template<class complexT>
		static bool serialize_variable_class(const complexT& whose, base_meta_writer& stream, uint8_t type)
		{
			if (!serialize_mapped_class(whose, stream, type))
				return false;
			if (!whose.variable_data().serialize_variable_definition(stream, type))
				return false;
			return true;
		}

		template<class complexT>
		static bool deserialize_variable_class(complexT& whose, base_meta_reader& stream, uint8_t type)
		{
			return false;
		}
	};

// Class rx_platform::meta::basic_defs::event_class 

string_type event_class::type_name = RX_CPP_EVENT_CLASS_TYPE_NAME;

event_class::event_class()
{
}

event_class::event_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system)
	: meta_data_(name, id, base_id, system)
{
}



platform_item_ptr event_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool event_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_complex_class(*this, stream, type))
		return false;
	return true;
}

bool event_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_complex_class(*this, stream, type))
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
	: meta_data_(name, id, base_id, system)
{
}



platform_item_ptr filter_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool filter_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_complex_class(*this, stream, type))
		return false;
	return true;
}

bool filter_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_complex_class(*this, stream, type))
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
	: meta_data_(name, id, base_id, system)
{
}



platform_item_ptr mapper_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool mapper_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_complex_class(*this, stream, type))
		return false;
	return true;
}

bool mapper_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_complex_class(*this, stream, type))
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
	: meta_data_(name, id, base_id, system)
{
}



platform_item_ptr source_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool source_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_complex_class(*this, stream, type))
		return false;
	return true;
}

bool source_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_helpers::deserialize_complex_class<source_class>(*this, stream, type);
}

checkable_data& source_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& source_class::complex_data ()
{
  return complex_data_;

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

struct_class::struct_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system)
	: meta_data_(name, id, base_id, system)
{
}



void struct_class::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

namespace_item_attributes struct_class::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_read_access | (meta_data_.get_system() ? namespace_item_attributes::namespace_item_system : namespace_item_attributes::namespace_item_null));
}

void struct_class::construct (struct_runtime_ptr what)
{
	complex_data_.construct(what);
}

platform_item_ptr struct_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

bool struct_class::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_helpers::serialize_mapped_class<struct_class>(*this, stream, type))
		return false;
	return true;
}

bool struct_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!meta_helpers::deserialize_mapped_class(*this, stream, type))
		return false;
	return true;
}

struct_runtime_ptr struct_class::create_runtime_ptr ()
{
	return new objects::blocks::struct_runtime;
}

checkable_data& struct_class::meta_data ()
{
  return meta_data_;

}

def_blocks::complex_data_type& struct_class::complex_data ()
{
  return complex_data_;

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
	: meta_data_(name, id, base_id, system)
{
}



void variable_class::construct (variable_runtime_ptr what)
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
//RX_TEMPLATE_INST_SIMPLE(rx_platform::objects::object_runtime, true);
//RX_TEMPLATE_INST_SIMPLE(rx_platform::logic::program_runtime, false);

