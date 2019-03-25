

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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_meta_algorithm
#include "system/meta/rx_meta_algorithm.h"

#include "api/rx_meta_api.h"
#include "rx_def_blocks.h"
#include "rx_types.h"
#include "model/rx_meta_internals.h"


namespace rx_platform {

namespace meta {

namespace meta_algorithm {

// Parameterized Class rx_platform::meta::meta_algorithm::meta_blocks_algorithm 


template <class typeT>
rx_result meta_blocks_algorithm<typeT>::serialize_complex_attribute (const typeT& whose, base_meta_writer& stream)
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
rx_result meta_blocks_algorithm<typeT>::deserialize_complex_attribute (typeT& whose, base_meta_reader& stream)
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
			<< " in attribute "
			<< whose.name_;
		
		ctx.add_error(ss.str());
	}
	return ctx.is_check_ok();
}

template <class typeT>
rx_result meta_blocks_algorithm<typeT>::construct_complex_attribute (const typeT& whose, construct_context& ctx)
{
	auto temp = model::platform_types_manager::instance().get_simple_type_cache<typename typeT::TargetType>().create_simple_runtime(whose.target_id_);
	if (temp)
	{
		ctx.runtime_data.add(whose.name_, std::move(temp.value()));
		return true;
	}
	else
	{
		return temp.errors();
	}
}

template <class typeT>
rx_result meta_blocks_algorithm<typeT>::resolve_complex_attribute (typeT& whose, rx_directory_ptr dir)
{
	if (!whose.target_id_.is_null())
		return true;// already resolved
	if (whose.target_name_.empty())
		return "Unable to resolve empty target name";
	auto item = dir->get_sub_item(whose.target_name_);
	if (!item)
	{
		return whose.target_name_ + " does not exists!";
	}
	auto id = item->meta_info().get_id();
	if (id.is_null())
	{// TODO error, item does not have id
		return whose.target_name_ + " does not have valid id!";
	}
	auto ret = model::platform_types_manager::instance().internal_get_simple_type_cache<typename typeT::TargetType>().type_exists(id);
	if (!ret)
	{// type does not exist
		return ret;
	}
	// everything is good we resolved it
	whose.target_id_ = std::move(id);
	return true;
}

// Variable Attribute is a special case!!!
template<>
rx_result meta_blocks_algorithm<def_blocks::variable_attribute>::serialize_complex_attribute(const def_blocks::variable_attribute& whose, base_meta_writer& stream)
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
rx_result meta_blocks_algorithm<def_blocks::variable_attribute>::deserialize_complex_attribute(def_blocks::variable_attribute& whose, base_meta_reader& stream)
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
rx_result meta_blocks_algorithm<def_blocks::variable_attribute>::construct_complex_attribute(const def_blocks::variable_attribute& whose, construct_context& ctx)
{
	auto temp = model::platform_types_manager::instance().get_simple_type_cache<def_blocks::variable_attribute::TargetType>().create_simple_runtime(whose.target_id_);
	if (temp)
	{
		temp.value().set_value(whose.get_value(ctx.now));
		ctx.runtime_data.add_variable(whose.name_, std::move(temp.value()), whose.get_value(ctx.now));
		return true;
	}
	else
	{
		return temp.errors();
	}
}
// explicit templates initialization
template class meta_blocks_algorithm<variable_attribute>;
template class meta_blocks_algorithm<struct_attribute>;
template class meta_blocks_algorithm<event_attribute>;
template class meta_blocks_algorithm<filter_attribute>;
template class meta_blocks_algorithm<source_attribute>;
template class meta_blocks_algorithm<mapper_attribute>;
// Parameterized Class rx_platform::meta::meta_algorithm::basic_types_algorithm 


template <class typeT>
rx_result basic_types_algorithm<typeT>::serialize_basic_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info_.serialize_meta_data(stream, type, typeT::type_name))
		return false;
	if (!stream.start_object("Def"))
		return false;
	auto ret = whose.complex_data_.serialize_complex_definition(stream, type);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
rx_result basic_types_algorithm<typeT>::deserialize_basic_type (typeT& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("Def"))
		return false;
	auto ret = whose.complex_data_.deserialize_complex_definition(stream, type);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
bool basic_types_algorithm<typeT>::check_basic_type (typeT& whose, type_check_context& ctx)
{
	auto ret = whose.complex_data_.check_type(ctx);
	return ret;
}

template <class typeT>
rx_result basic_types_algorithm<typeT>::construct_basic_type (const typeT& whose, construct_context& ctx)
{
	return whose.complex_data_.construct(ctx);
}

template <class typeT>
rx_result basic_types_algorithm<typeT>::resolve_basic_type (typeT& whose, rx_directory_ptr dir)
{
	auto ret = whose.complex_data_.resolve(dir);
	return ret;
	
	
}

// Template specialization for variable_type and struct_type
template <>
rx_result basic_types_algorithm<variable_type>::serialize_basic_type(const variable_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info_.serialize_meta_data(stream, type, variable_type::type_name))
		return false;
	if (!stream.start_object("Def"))
		return false;
	auto ret = whose.complex_data_.serialize_complex_definition(stream, type);
	if (!ret)
		return ret;
	ret = whose.variable_data_.serialize_variable_definition(stream, type);
	if (!ret)
		return ret;
	ret = whose.mapping_data_.serialize_mapped_definition(stream, type);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}
template <>
rx_result basic_types_algorithm<struct_type>::serialize_basic_type(const struct_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info_.serialize_meta_data(stream, type, struct_type::type_name))
		return false;
	if (!stream.start_object("Def"))
		return false;
	auto ret = whose.complex_data_.serialize_complex_definition(stream, type);
	if (!ret)
		return ret;
	ret = whose.mapping_data_.serialize_mapped_definition(stream, type);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<variable_type>::deserialize_basic_type(variable_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("Def"))
		return false;
	auto ret = whose.complex_data_.deserialize_complex_definition(stream, type);
	if (!ret)
		return ret;
	ret = whose.variable_data_.deserialize_variable_definition(stream, type, whose.complex_data_);
	if (!ret)
		return ret;
	ret = whose.mapping_data_.deserialize_mapped_definition(stream, type, whose.complex_data_);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<struct_type>::deserialize_basic_type(struct_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("Def"))
		return false;
	auto ret = whose.complex_data_.deserialize_complex_definition(stream, type);
	if (!ret)
		return ret;
	ret = whose.mapping_data_.deserialize_mapped_definition(stream, type, whose.complex_data_);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}


template <>
bool basic_types_algorithm<variable_type>::check_basic_type(variable_type& whose, type_check_context& ctx)
{
	auto ret = whose.complex_data_.check_type(ctx);
	ret = ret && whose.variable_data_.check_type(ctx);
	ret = ret && whose.mapping_data_.check_type(ctx);
	return ret;
}
template <>
bool basic_types_algorithm<struct_type>::check_basic_type(struct_type& whose, type_check_context& ctx)
{
	auto ret = whose.complex_data_.check_type(ctx);
	ret = ret && whose.mapping_data_.check_type(ctx);
	return ret;
}


template <>
rx_result basic_types_algorithm<variable_type>::construct_basic_type(const variable_type& whose, construct_context& ctx)
{
	auto ret = whose.complex_data_.construct(ctx);
	if (ret)
	{
		ret = whose.variable_data_.construct(whose.complex_data_.get_names_cache() , ctx);
		if (ret)
			ret = whose.mapping_data_.construct(whose.complex_data_.get_names_cache(), ctx);
	}
	return ret;
}

template <>
rx_result basic_types_algorithm<struct_type>::construct_basic_type(const struct_type& whose, construct_context& ctx)
{
	auto ret = whose.complex_data_.construct(ctx);	
	if (ret)
		ret = whose.mapping_data_.construct(whose.complex_data_.get_names_cache(), ctx);

	return ret;
}


template <>
rx_result basic_types_algorithm<variable_type>::resolve_basic_type(variable_type& whose, rx_directory_ptr dir)
{
	auto ret = whose.complex_data_.resolve(dir);
	if (ret)
	{
		ret = whose.variable_data_.resolve(dir);
		if (ret)
			ret = whose.mapping_data_.resolve(dir);
	}
	return ret;
}


template <>
rx_result basic_types_algorithm<struct_type>::resolve_basic_type(struct_type& whose, rx_directory_ptr dir)
{
	auto ret = whose.complex_data_.resolve(dir);
	if (ret)
		ret = whose.mapping_data_.resolve(dir);
	return ret;
}

template class basic_types_algorithm<basic_types::struct_type>;
template class basic_types_algorithm<basic_types::variable_type>;
template class basic_types_algorithm<basic_types::source_type>;
template class basic_types_algorithm<basic_types::mapper_type>;
template class basic_types_algorithm<basic_types::filter_type>;
template class basic_types_algorithm<basic_types::event_type>;
// Parameterized Class rx_platform::meta::meta_algorithm::object_types_algorithm 


template <class typeT>
rx_result object_types_algorithm<typeT>::serialize_object_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info_.serialize_meta_data(stream, type, typeT::type_name))
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
rx_result object_types_algorithm<typeT>::deserialize_object_type (typeT& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("Def"))
		return false;
	if (!whose.complex_data_.deserialize_complex_definition(stream, type))
		return false;
	if (!whose.mapping_data_.deserialize_mapped_definition(stream, type, whose.complex_data_))
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
rx_result object_types_algorithm<typeT>::construct_object (const typeT& whose, typename typeT::RTypePtr what, construct_context& ctx)
{
	auto ret = whose.complex_data_.construct(ctx);
	if (ret)
	{
		ret = whose.mapping_data_.construct(whose.complex_data_.get_names_cache(), ctx);
		if (ret)
			ret = whose.object_data_.construct(what, ctx);
	}
	return ret;
}

template <class typeT>
rx_result object_types_algorithm<typeT>::resolve_object_type (typeT& whose, rx_directory_ptr dir)
{
	auto ret = whose.complex_data_.resolve(dir);
	if (ret)
	{
		ret = whose.mapping_data_.resolve(dir);
		if (ret)
			ret = whose.object_data_.resolve(dir);
	}
	return ret;
}

template class object_types_algorithm<object_types::object_type>;
template class object_types_algorithm<object_types::port_type>;
template class object_types_algorithm<object_types::application_type>;
template class object_types_algorithm<object_types::domain_type>;
} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform

