

/****************************************************************************
*
*  system\meta\rx_meta_algorithm.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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
// rx_meta_algorithm
#include "system/meta/rx_meta_algorithm.h"

#include "api/rx_meta_api.h"
#include "rx_def_blocks.h"
#include "rx_types.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "runtime_internal/rx_runtime_algorithms.h"
#include "rx_meta_attr_algorithm.h"


namespace rx_platform {

namespace meta {

namespace meta_algorithm {

// Parameterized Class rx_platform::meta::meta_algorithm::basic_types_algorithm 


template <class typeT>
rx_result basic_types_algorithm<typeT>::serialize_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info.serialize_meta_data(stream, type, typeT::type_id))
		return false;
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
rx_result basic_types_algorithm<typeT>::deserialize_type (typeT& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
bool basic_types_algorithm<typeT>::check_type (typeT& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	return ret;
}

template <class typeT>
rx_result basic_types_algorithm<typeT>::construct (const typeT& whose, construct_context& ctx)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	return ret;
}

// Template specialization for variable_type and struct_type
template <>
rx_result basic_types_algorithm<variable_type>::serialize_type(const variable_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info.serialize_meta_data(stream, type, variable_type::type_id))
		return false;
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = variable_data_algorithm::serialize_complex_attribute(whose.variable_data, stream);
	if (!ret)
		return ret;
	ret = mapped_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}
template <>
rx_result basic_types_algorithm<struct_type>::serialize_type(const struct_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info.serialize_meta_data(stream, type, struct_type::type_id))
		return false;
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapped_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<source_type>::serialize_type(const source_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info.serialize_meta_data(stream, type, source_type::type_id))
		return false;
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::serialize_complex_attribute(whose.filter_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<mapper_type>::serialize_type(const mapper_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info.serialize_meta_data(stream, type, mapper_type::type_id))
		return false;
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::serialize_complex_attribute(whose.filter_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<variable_type>::deserialize_type(variable_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = variable_data_algorithm::deserialize_complex_attribute(whose.variable_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	ret = mapped_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<struct_type>::deserialize_type(struct_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapped_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<source_type>::deserialize_type(source_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::deserialize_complex_attribute(whose.filter_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}

template <>
rx_result basic_types_algorithm<mapper_type>::deserialize_type(mapper_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::deserialize_complex_attribute(whose.filter_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return false;
	return true;
}


template <>
bool basic_types_algorithm<variable_type>::check_type(variable_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && variable_data_algorithm::check_complex_attribute(whose.variable_data, ctx);
	ret = ret && mapped_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
	return ret;
}
template <>
bool basic_types_algorithm<struct_type>::check_type(struct_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && mapped_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
	return ret;
}

template <>
bool basic_types_algorithm<source_type>::check_type(source_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && filtered_data_algorithm::check_complex_attribute(whose.filter_data, ctx);
	return ret;
}
template <>
bool basic_types_algorithm<mapper_type>::check_type(mapper_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && filtered_data_algorithm::check_complex_attribute(whose.filter_data, ctx);
	return ret;
}


template <>
rx_result basic_types_algorithm<variable_type>::construct(const variable_type& whose, construct_context& ctx)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret = variable_data_algorithm::construct_complex_attribute(whose.variable_data, whose.complex_data.get_names_cache() , ctx);
		if (ret)
			ret = mapped_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);
	}
	return ret;
}

template <>
rx_result basic_types_algorithm<struct_type>::construct(const struct_type& whose, construct_context& ctx)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
		ret = mapped_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);

	return ret;
}


template <>
rx_result basic_types_algorithm<source_type>::construct(const source_type& whose, construct_context& ctx)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret = filtered_data_algorithm::construct_complex_attribute(whose.filter_data, whose.complex_data.get_names_cache(), ctx);
	}
	return ret;
}
template <>
rx_result basic_types_algorithm<mapper_type>::construct(const mapper_type& whose, construct_context& ctx)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret = filtered_data_algorithm::construct_complex_attribute(whose.filter_data, whose.complex_data.get_names_cache(), ctx);
	}
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
rx_result object_types_algorithm<typeT>::serialize_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info.serialize_meta_data(stream, type, typeT::type_id))
		return false;
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapped_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
	if (!ret)
		return ret;
	//////////////////////////////////
	// object data
	if (!stream.write_bool("constructable", whose.object_data.is_constructable))
		return false;
	if (!stream.start_array("relations", whose.object_data.relations_.size()))
		return false;
	for (const auto& one : whose.object_data.relations_)
	{
		if (!stream.start_object("item"))
			return false;
		
		ret = relation_blocks_algorithm::serialize_relation_attribute(one, stream);
		if (!ret)
			return ret;

		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;
	if (!stream.start_array("programs", whose.object_data.programs_.size()))
		return false;
	for (const auto& one : whose.object_data.programs_)
	{
		if (!one->save_program(stream, type))
			return false;
	}
	if (!stream.end_array())
		return false;
	// end object data
	//////////////////////////////////
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
rx_result object_types_algorithm<typeT>::deserialize_type (typeT& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapped_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	// object data
	if (!stream.read_bool("constructable", whose.object_data.is_constructable))
		return false;
	if (!stream.start_array("relations"))
		return false;

	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return false;

		relation_attribute one;
		if (!relation_blocks_algorithm::deserialize_relation_attribute(one, stream))
			return false;
		whose.object_data.relations_.emplace_back(one);

		if (!stream.end_object())
			return false;
	}
	return true;
	// end object data
	if (!stream.end_object())
		return false;
	return true;
}

template <class typeT>
bool object_types_algorithm<typeT>::check_type (typeT& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && mapped_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
	for (auto& one : whose.object_data.relations_)
	{
		ret = ret && meta_algorithm::relation_blocks_algorithm::check_relation_attribute(one, ctx);
	}
	return ret;
}

template <class typeT>
rx_result object_types_algorithm<typeT>::construct_runtime (const typeT& whose, typename typeT::RTypePtr what, construct_context& ctx)
{
	ctx.get_directories().add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret  = mapped_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);
		if (ret)
		{
			for (const auto& one : whose.object_data.relations_)
			{
				auto data = rx_create_reference<runtime::relations::relation_data>();
				ret = meta_algorithm::relation_blocks_algorithm::construct_relation_attribute(one, *data, what, ctx);
				if (ret)
				{
					what->relations_.source_relations_.emplace_back(std::move(data));
				}
			}
		}
	}
	return ret;
}



template class object_types_algorithm<object_types::object_type>;
template class object_types_algorithm<object_types::port_type>;
template class object_types_algorithm<object_types::application_type>;
template class object_types_algorithm<object_types::domain_type>;
// Class rx_platform::meta::meta_algorithm::relation_blocks_algorithm 


rx_result relation_blocks_algorithm::serialize_relation_attribute (const object_types::relation_attribute& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name))
		return false;
	if (!stream.write_item_reference("relation", whose.relation_type))
		return false;
	if (!stream.write_item_reference("target", whose.target))
		return false;
	return true;
}

rx_result relation_blocks_algorithm::deserialize_relation_attribute (object_types::relation_attribute& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name))
		return false;
	if (!stream.read_item_reference("relation", whose.relation_type))
		return false;
	if (!stream.read_item_reference("target", whose.target))
		return false;
	return true;
}

bool relation_blocks_algorithm::check_relation_attribute (object_types::relation_attribute& whose, type_check_context& ctx)
{
	rx_node_id target_id;
	auto resolve_result = rx_internal::model::algorithms::resolve_relation_reference(whose.target, ctx.get_directories());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target_id = resolve_result.value();
	auto target = rx_internal::model::platform_types_manager::instance().get_relations_repository().get_type_definition(target_id);
	if (!target)
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(relation_type::type_id)
			<< " in attribute "
			<< whose.name;

		ctx.add_error(ss.str(), RX_ITEM_NOT_FOUND, rx_medium_severity, target.errors());
	}
	return ctx.is_check_ok();
}

rx_result relation_blocks_algorithm::construct_relation_attribute (const object_types::relation_attribute& whose, runtime::relations::relation_data& data, rx_reference_ptr ref_ptr, construct_context& ctx)
{
	auto resolve_result = rx_internal::model::algorithms::resolve_relation_reference(whose.relation_type, ctx.get_directories());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve relation type");
		return ret.errors();
	}
	auto relation_type_id = resolve_result.value();
	resolve_result = rx_internal::model::algorithms::resolve_reference(whose.target, ctx.get_directories());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve relation target type");
		return ret.errors();
	}
	auto target_base_id = resolve_result.value();

	auto ret_val = rx_internal::model::platform_types_manager::instance().get_relations_repository().create_runtime(relation_type_id, ctx.rt_name(), data, ctx.get_directories());
	if (ret_val)
	{
		data.target_relation_name = replace_in_string(data.target_relation_name, RX_MACRO_SYMBOL_STR "name" RX_MACRO_SYMBOL_STR, ctx.rt_name());
		data.name = whose.name;
		data.target_base_id = target_base_id;
		rx_timed_value str_val;
		str_val.assign_static<string_type>("", ctx.now);
		data.value.value = str_val;
		data.implementation_ = ret_val.value();
		// this is something i still don't know about
		// it will have to wait
		//!!!!!
		/*rx_timed_value val;
		val.assign_static<string_type>("", ctx.now);
		ctx.runtime_data.add_value(whose.name, val);*/
		return true;
	}
	else
	{
		return ret_val.errors();
	}
}


// Class rx_platform::meta::meta_algorithm::relation_type_algorithm 


rx_result relation_type_algorithm::serialize_type (const relation_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (!whose.meta_info.serialize_meta_data(stream, type, relation_type::type_id))
		return false;
	if (!stream.start_object("def"))
		return false;

	if (!stream.write_bool("sealed", whose.relation_data.sealed_type))
		return false;
	if (!stream.write_bool("abstract", whose.relation_data.abstract_type))
		return false;
	if (!stream.write_bool("hierarchical", whose.relation_data.hierarchical))
		return false;
	if (!stream.write_bool("symmetrical", whose.relation_data.symmetrical))
		return false;
	if (!stream.write_string("inverse", whose.relation_data.inverse_name))
		return false;
	if (!stream.write_bool("dynamic", whose.relation_data.dynamic))
		return false;

	if (!stream.end_object())
		return false;
	return true;
}

rx_result relation_type_algorithm::deserialize_type (relation_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;

	if (!stream.read_bool("sealed", whose.relation_data.sealed_type))
		return false;
	if (!stream.read_bool("abstract", whose.relation_data.abstract_type))
		return false;
	if (!stream.read_bool("hierarchical", whose.relation_data.hierarchical))
		return false;
	if (!stream.read_bool("symmetrical", whose.relation_data.symmetrical))
		return false;
	if (!stream.read_string("inverse", whose.relation_data.inverse_name))
		return false;
	if (!stream.read_bool("dynamic", whose.relation_data.dynamic))
		return false;

	if (!stream.end_object())
		return false;
	return true;
}

bool relation_type_algorithm::check_type (relation_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	// just check inverse_name is symmetrical
	if (!whose.relation_data.symmetrical && whose.relation_data.inverse_name.empty())
		ctx.add_error("no inverse name provided for non-symmetrical relation", RX_NO_INVERSE_NAME_FOUND, rx_tolerable_severity);
	return true;
}

rx_result relation_type_algorithm::construct_runtime (const relation_type& whose, relation_type::RTypePtr what, construct_context& ctx)
{
	return true;
}


} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform

