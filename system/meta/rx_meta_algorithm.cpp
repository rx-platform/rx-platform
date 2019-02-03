

/****************************************************************************
*
*  system\meta\rx_meta_algorithm.cpp
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


// rx_meta_algorithm
#include "system/meta/rx_meta_algorithm.h"

#include "model/rx_meta.h"
#include "rx_def_blocks.h"


namespace rx_platform {

namespace meta {

namespace meta_algorithm {

// Parameterized Class rx_platform::meta::meta_algorithm::meta_blocks_algorithm 


template <class typeT>
bool meta_blocks_algorithm<typeT>::serialize_complex_attribute (const typeT& whose, base_meta_writer& stream)
{
	if (!stream.start_object(typeT::TargetType::type_name.c_str()))
		return false;
	if (!stream.write_id("Id", whose.target_id_))
		return false;
	if (!stream.write_string("Name", whose.name_.c_str()))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
bool meta_blocks_algorithm<typeT>::deserialize_complex_attribute (typeT& whose, base_meta_reader& stream)
{
	if (!stream.start_object(typeT::TargetType::type_name.c_str()))
		return false;
	if (!stream.read_id("Id", whose.target_id_))
		return false;
	if (!stream.read_string("Name", whose.name_))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
bool meta_blocks_algorithm<typeT>::check_complex_attribute (typeT& whose, type_check_context& ctx)
{
	auto target = model::platform_types_manager::instance().get_simple_type_cache<typename typeT::TargetType>().get_type_definition(whose.target_id_);
	if (!target)
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< typeT::TargetType::type_name
			<< " in ";
		ctx.add_error(ss.str());
	}
	return ctx.is_check_ok();
}

template <class typeT>
void meta_blocks_algorithm<typeT>::construct_complex_attribute (const typeT& whose, construct_context& ctx)
{
	auto temp = model::platform_types_manager::instance().get_simple_type_cache<typename typeT::TargetType>().create_simple_runtime(whose.target_id_);
	ctx.runtime_data.add(whose.name_, std::move(temp));
}

// Variable Attribute is a special case!!!
template<>
bool meta_blocks_algorithm<def_blocks::variable_attribute>::serialize_complex_attribute(const def_blocks::variable_attribute& whose, base_meta_writer& stream)
{
	if (!stream.start_object(variable_attribute::TargetType::type_name.c_str()))
		return false;
	if (!stream.write_id("Id", whose.target_id_))
		return false;
	if (!stream.write_string("Name", whose.name_.c_str()))
		return false;
	if (!stream.write_bool("RO", whose.read_only_))
		return false;
	if (!whose.storage_.serialize(stream))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}
template<>
bool meta_blocks_algorithm<def_blocks::variable_attribute>::deserialize_complex_attribute(def_blocks::variable_attribute& whose, base_meta_reader& stream)
{
	if (!stream.start_object(variable_attribute::TargetType::type_name.c_str()))
		return false;
	if (!stream.read_id("Id", whose.target_id_))
		return false;
	if (!stream.read_string("Name", whose.name_))
		return false;
	if (!stream.read_bool("RO", whose.read_only_))
		return false;
	if (!whose.storage_.deserialize(stream))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}
template<>
bool meta_blocks_algorithm<def_blocks::variable_attribute>::check_complex_attribute(def_blocks::variable_attribute& whose, type_check_context& ctx)
{
	return true;
}
template<>
void meta_blocks_algorithm<def_blocks::variable_attribute>::construct_complex_attribute(const def_blocks::variable_attribute& whose, construct_context& ctx)
{
	auto temp = model::platform_types_manager::instance().get_simple_type_cache<def_blocks::variable_attribute::TargetType>().create_simple_runtime(whose.target_id_);
	temp.set_value(whose.get_value(ctx.now));
	ctx.runtime_data.add_variable(whose.name_, std::move(temp), whose.get_value(ctx.now));
}
// explicit templates initialization
template class meta_blocks_algorithm<struct_attribute>;
template class meta_blocks_algorithm<event_attribute>;
template class meta_blocks_algorithm<filter_attribute>;
template class meta_blocks_algorithm<source_attribute>;
template class meta_blocks_algorithm<mapper_attribute>;
// Parameterized Class rx_platform::meta::meta_algorithm::basic_types_algorithm 


template <class typeT>
bool basic_types_algorithm<typeT>::serialize_basic_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_data_.serialize_checkable_definition(stream, type, typeT::type_name))
		return false;
	if (!stream.start_object("Def"))
		return false;
	if (!whose.complex_data_.serialize_complex_definition(stream, type))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
bool basic_types_algorithm<typeT>::deserialize_basic_type (typeT& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("Def"))
		return false;
	if (!whose.complex_data_.deserialize_complex_definition(stream, type))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
bool basic_types_algorithm<typeT>::check_basic_type (typeT& whose, type_check_context& ctx)
{
	bool ret = whose.complex_data_.check_type(ctx);
	return ret;
}

template <class typeT>
void basic_types_algorithm<typeT>::construct_basic_type (const typeT& whose, construct_context& ctx)
{
	whose.complex_data_.construct(ctx);
}

// Struct Type is a special case!!!
template <>
bool basic_types_algorithm<basic_types::struct_type>::serialize_basic_type(const basic_types::struct_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_data_.serialize_checkable_definition(stream, type, basic_types::struct_type::type_name))
		return false;
	if (!stream.start_object("Def"))
		return false;
	if (!whose.complex_data_.serialize_complex_definition(stream, type))
		return false;
	if (!whose.mapping_data_.serialize_mapped_definition(stream, type))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
bool basic_types_algorithm<basic_types::struct_type>::deserialize_basic_type(basic_types::struct_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("Def"))
		return false;
	if (!whose.complex_data_.deserialize_complex_definition(stream, type))
		return false;
	if (!whose.mapping_data_.deserialize_mapped_definition(stream, type))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
bool basic_types_algorithm<basic_types::struct_type>::check_basic_type(basic_types::struct_type& whose, type_check_context& ctx)
{
	bool ret = whose.complex_data_.check_type(ctx);
	ret &= whose.mapping_data_.check_type(ctx);
	return ret;
}

template <>
void basic_types_algorithm<basic_types::struct_type>::construct_basic_type(const basic_types::struct_type& whose, construct_context& ctx)
{
	whose.complex_data_.construct(ctx);
	whose.mapping_data_.construct(whose.complex_data_.get_names_cache(), ctx);
}
// Variable Type is a special case!!!
template <>
bool basic_types_algorithm<basic_types::variable_type>::serialize_basic_type(const basic_types::variable_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_data_.serialize_checkable_definition(stream, type, basic_types::variable_type::type_name))
		return false;
	if (!stream.start_object("Def"))
		return false;
	if (!whose.complex_data_.serialize_complex_definition(stream, type))
		return false;
	if (!whose.mapping_data_.serialize_mapped_definition(stream, type))
		return false;
	if (!whose.variable_data_.serialize_variable_definition(stream, type))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
bool basic_types_algorithm<basic_types::variable_type>::deserialize_basic_type(basic_types::variable_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("Def"))
		return false;
	if (!whose.complex_data_.deserialize_complex_definition(stream, type))
		return false;
	if (!whose.mapping_data_.deserialize_mapped_definition(stream, type))
		return false;
	if (!whose.variable_data_.deserialize_variable_definition(stream, type))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
bool basic_types_algorithm<basic_types::variable_type>::check_basic_type(basic_types::variable_type& whose, type_check_context& ctx)
{
	bool ret = whose.complex_data_.check_type(ctx);
	ret &= whose.mapping_data_.check_type(ctx);
	ret &= whose.variable_data_.check_type(ctx);
	return ret;
}

template <>
void basic_types_algorithm<basic_types::variable_type>::construct_basic_type(const basic_types::variable_type& whose, construct_context& ctx)
{
	whose.complex_data_.construct(ctx);
	whose.mapping_data_.construct(whose.complex_data_.get_names_cache(), ctx);
	whose.variable_data_.construct(whose.complex_data_.get_names_cache(), ctx);
}
template class basic_types_algorithm<basic_types::struct_type>;
template class basic_types_algorithm<basic_types::variable_type>;
template class basic_types_algorithm<basic_types::source_type>;
template class basic_types_algorithm<basic_types::mapper_type>;
template class basic_types_algorithm<basic_types::filter_type>;
template class basic_types_algorithm<basic_types::event_type>;
// Parameterized Class rx_platform::meta::meta_algorithm::object_types_algorithm 


template <class typeT>
bool object_types_algorithm<typeT>::serialize_object_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_data_.serialize_checkable_definition(stream, type, typeT::type_name))
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

template <class typeT>
bool object_types_algorithm<typeT>::deserialize_object_type (typeT& whose, base_meta_reader& stream, uint8_t type)
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

template <class typeT>
bool object_types_algorithm<typeT>::check_object_type (typeT& whose, type_check_context& ctx)
{
	bool ret = whose.complex_data_.check_type(ctx);
	ret &= whose.mapping_data_.check_type(ctx);
	ret &= whose.object_data_.check_type(ctx);
	return ret;
}

template <class typeT>
void object_types_algorithm<typeT>::construct_object (const typeT& whose, typename typeT::RTypePtr what, construct_context& ctx)
{
	whose.complex_data_.construct(ctx);
	whose.mapping_data_.construct(whose.complex_data_.get_names_cache(), ctx);
	whose.object_data_.construct(what, ctx);
}

template class object_types_algorithm<object_types::object_type>;
template class object_types_algorithm<object_types::port_type>;
template class object_types_algorithm<object_types::application_type>;
template class object_types_algorithm<object_types::domain_type>;
} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform

