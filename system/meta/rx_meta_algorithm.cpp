

/****************************************************************************
*
*  system\meta\rx_meta_algorithm.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"

#include "system/meta/rx_meta_attr_algorithm.h"

// rx_types
#include "system/meta/rx_types.h"
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
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace meta {

namespace meta_algorithm {

// Class rx_platform::meta::meta_algorithm::data_types_algorithm 


rx_result data_types_algorithm::serialize_type (const data_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		auto result = whose.meta_info.serialize_meta_data(stream, type, data_type::type_id);
		if (!result)
			return result;
	}
	if (!stream.start_object("def"))
		return stream.get_error();

	if (!stream.start_array("items", whose.complex_data.names_cache_.size()))
		return stream.get_error();

	for (const auto& one : whose.complex_data.names_cache_)
	{
		switch (one.second & data_type_def::type_mask)
		{
		case data_type_def::simple_values_mask:
			{
				if (!stream.start_object("item"))
					return stream.get_error();

				if (!stream.write_bool("simple", true))
					return stream.get_error();

				auto result = whose.complex_data.values_[one.second & data_type_def::index_mask].serialize_definition(stream);
				if (!result)
					return result;

				if (!stream.end_object())
					return stream.get_error();
			}
			break;
		case data_type_def::child_values_mask:
			{
				if (!stream.start_object("item"))
					return stream.get_error();

				if (!stream.write_bool("simple", false))
					return stream.get_error();

				auto result = data_blocks_algorithm::serialize_data_attribute(
					whose.complex_data.children_[one.second & data_type_def::index_mask], stream);

				if (!result)
					return result;

				if (!stream.end_object())
					return stream.get_error();
			}
			break;
		}
	}

	if (!stream.end_array())
		return stream.get_error();

	if (!stream.write_init_values("overrides", whose.complex_data.overrides_))
		return stream.get_error();

	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.complex_data.description_.c_str()))
			return stream.get_error();
	}

	if (!stream.end_object())
		return stream.get_error();
	return true;
}

rx_result data_types_algorithm::deserialize_type (data_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();

	if (!stream.start_array("items"))
		return stream.get_error();

	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		bool is_value;
		if (!stream.read_bool("simple", is_value))
			return stream.get_error();

		if (is_value)
		{// simple value
			data_value_def val;
			if (!val.deserialize_definition(stream))
				return stream.get_error();
			auto ret = whose.complex_data.check_name(val.get_name()
				, (static_cast<int>(whose.complex_data.values_.size() | data_type_def::simple_values_mask)));
			if (ret)
			{
				whose.complex_data.values_.emplace_back(std::move(val));
			}
			else
				return ret;
		}
		else
		{// complex value
			data_attribute attr;
			if (!data_blocks_algorithm::deserialize_data_attribute(attr, stream))
				return stream.get_error();
			auto ret = whose.complex_data.check_name(attr.get_name()
				, (static_cast<int>(whose.complex_data.children_.size() | data_type_def::child_values_mask)));
			if (ret)
			{
				whose.complex_data.children_.emplace_back(std::move(attr));
			}
			else
				return ret;
		}

		if (!stream.end_object())
			return stream.get_error();
	}

	if (!stream.read_init_values("overrides", whose.complex_data.overrides_))
		return stream.get_error();

	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.complex_data.description_))
			return stream.get_error();
	}

	if (!stream.end_object())
		return stream.get_error();
	return true;
}

bool data_types_algorithm::check_type (data_type& whose, type_check_context& ctx)
{
	std::set<string_type> names_hash;
	for (auto& one : whose.complex_data.children_)
	{
		if (names_hash.find(one.get_name()) != names_hash.end())
		{
			rx_result res(one.get_name() + " is duplicated name.");
			ctx.add_error(res.errors()[0], RX_DUPLICATED_NAME, rx_medium_severity, res);
		}
		else
		{
			names_hash.emplace(one.get_name());
		}
		data_blocks_algorithm::check_data_attribute(one, ctx);
	}
	for (auto& one : whose.complex_data.values_)
	{
		if (names_hash.find(one.get_name()) != names_hash.end())
		{
			rx_result res(one.get_name() + " is duplicated name.");
			ctx.add_error(res.errors()[0], RX_DUPLICATED_NAME, rx_medium_severity, res);
		}
		else
		{
			names_hash.emplace(one.get_name());
		}
	}
	return true;
}

rx_result data_types_algorithm::construct_runtime (const data_type& whose, runtime::structure::block_data& what, construct_context& ctx)
{
	rx_result ret = true;
	// first pass, create structure
	size_t children_size = 0;
	size_t values_size = 0;
	size_t items_size = 0;
	// first pass find the sizes

	std::vector<runtime::structure::index_data> items;
	std::vector<runtime::structure::array_wrapper<runtime::structure::const_value_data> > values;
	std::vector<runtime::structure::array_wrapper<runtime::structure::block_data> > children;

	for (const auto& one : whose.complex_data.names_cache_)
	{
		switch (one.second & data_type_def::type_mask)
		{
		case data_type_def::simple_values_mask:
			{
				if (whose.complex_data.values_[one.second & complex_data_type::index_mask].get_array_size() < 0)
				{
					if (check_name(one.first, items))
					{
						members_index_type new_idx = static_cast<members_index_type>(values.size());
						runtime::structure::const_value_data temp;
						temp.value = std::move(whose.complex_data.values_[one.second & complex_data_type::index_mask].get_value());
						values.emplace_back(std::move(temp));
						items.push_back({ one.first, (new_idx << rt_type_shift) | rt_const_index_type });
					}
				}
				else if (whose.complex_data.values_[one.second & complex_data_type::index_mask].get_array_size() == 0)
				{
					if (check_name(one.first, items))
					{
						members_index_type new_idx = static_cast<members_index_type>(values.size());
						runtime::structure::const_value_data temp_const;
						temp_const.value = std::move(whose.complex_data.values_[one.second & complex_data_type::index_mask].get_value());
						runtime::structure::array_wrapper<runtime::structure::const_value_data> temp;
						temp.declare_null_array(std::move(temp_const));
						values.emplace_back(std::move(temp));
						items.push_back({ one.first, (new_idx << rt_type_shift) | rt_const_index_type });
					}
				}
				else
				{
					auto size = whose.complex_data.children_[one.second & complex_data_type::index_mask].get_array_size();
					std::vector<runtime::structure::const_value_data> child_data(size);

					if (check_name(one.first, items))
					{
						for (int i = 0; i < size; i++)
						{
							runtime::structure::const_value_data one_temp;
							one_temp.value = std::move(whose.complex_data.values_[one.second & complex_data_type::index_mask].get_value());
							child_data.push_back(std::move(one_temp));
						}
						members_index_type new_idx = static_cast<members_index_type>(values.size());
						values.emplace_back(std::move(child_data));
						items.push_back({ one.first, (new_idx << rt_type_shift) | rt_const_index_type });
					}
				}
			}
			break;
		case data_type_def::child_values_mask:
			{
				if (whose.complex_data.children_[one.second & complex_data_type::index_mask].get_array_size() < 0)
				{
					block_data child_data;
					ret = data_blocks_algorithm::construct_data_attribute(
						whose.complex_data.children_[one.second & complex_data_type::index_mask], child_data, ctx);
					if (!ret)
						return ret;
					if (check_name(one.first, items))
					{
						members_index_type new_idx = static_cast<members_index_type>(children.size());
						children.emplace_back(std::move(child_data));
						items.push_back({ one.first, (new_idx << rt_type_shift) | rt_data_index_type });
					}
				}
				else if (whose.complex_data.children_[one.second & complex_data_type::index_mask].get_array_size() == 0)
				{
					block_data child_data;
					ret = data_blocks_algorithm::construct_data_attribute(
						whose.complex_data.children_[one.second & complex_data_type::index_mask], child_data, ctx);
					if (!ret)
						return ret;
					if (check_name(one.first, items))
					{
						members_index_type new_idx = static_cast<members_index_type>(children.size());
						runtime::structure::array_wrapper<runtime::structure::block_data> temp;
						temp.declare_null_array(std::move(child_data));
						children.emplace_back(std::move(temp));
						items.push_back({ one.first, (new_idx << rt_type_shift) | rt_data_index_type });
					}
				}
				else
				{
					auto size = whose.complex_data.children_[one.second & complex_data_type::index_mask].get_array_size();
					std::vector<block_data> child_data(size);
					for (auto i = 0; i < size; i++)
					{
						ret = data_blocks_algorithm::construct_data_attribute(
							whose.complex_data.children_[one.second & complex_data_type::index_mask], child_data[i], ctx);
						if (!ret)
							return ret;
					}
					if (check_name(one.first, items))
					{
						members_index_type new_idx = static_cast<members_index_type>(children.size());
						children.emplace_back(std::move(child_data));
						items.push_back({ one.first, (new_idx << rt_type_shift) | rt_data_index_type });
					}
				}
			}
			break;
		}
	}
	what.items = const_size_vector<runtime::structure::index_data>(std::move(items));
	what.children = const_size_vector<runtime::structure::array_wrapper<runtime::structure::block_data> >(std::move(children));
	what.values = const_size_vector<runtime::structure::array_wrapper<runtime::structure::const_value_data>>(std::move(values));
	return ret;
}

bool data_types_algorithm::check_name (const string_type& name, std::vector<runtime::structure::index_data>& items)
{
	for (const auto& one : items)
	{
		if (one.name == name)
			return false;
	}
	return true;
}


// Parameterized Class rx_platform::meta::meta_algorithm::basic_types_algorithm 


template <class typeT>
rx_result basic_types_algorithm<typeT>::serialize_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, typeT::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <class typeT>
rx_result basic_types_algorithm<typeT>::deserialize_type (typeT& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
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
rx_result basic_types_algorithm<typeT>::construct (const typeT& whose, construct_context& ctx, typename typeT::RDataType& prototype)
{
	ctx.get_directories().add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	return ret;
}

template <class typeT>
rx_result basic_types_algorithm<typeT>::get_depends (const typeT& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	return ret;
}

// Template specialization for variable_type and struct_type
template <>
rx_result basic_types_algorithm<variable_type>::serialize_type(const variable_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, variable_type::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = variable_data_algorithm::serialize_complex_attribute(whose.variable_data, stream);
	if (!ret)
		return ret;
	ret = mapsrc_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}
template <>
rx_result basic_types_algorithm<struct_type>::serialize_type(const struct_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, struct_type::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapsrc_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <>
rx_result basic_types_algorithm<source_type>::serialize_type(const source_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, source_type::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::serialize_complex_attribute(whose.filter_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <>
rx_result basic_types_algorithm<mapper_type>::serialize_type(const mapper_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, mapper_type::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::serialize_complex_attribute(whose.filter_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}


template <>
rx_result basic_types_algorithm<event_type>::serialize_type(const event_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, event_type::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
	{
		if (!stream.write_item_reference("args", whose.arguments))
			return stream.get_error();
	}
	if (stream.get_version() >= RX_METHOD_MAPPERS_VERSION)
	{
		ret = mapped_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
		if (!ret)
			return ret;
	}
	if (!stream.end_object())
		return stream.get_error();
	return true;
}



template <>
rx_result basic_types_algorithm<method_type>::serialize_type(const method_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, method_type::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
	{
		if (!stream.write_item_reference("in", whose.inputs))
			return stream.get_error();
		if (!stream.write_item_reference("out", whose.outputs))
			return stream.get_error();
	}
	if (stream.get_version() >= RX_METHOD_MAPPERS_VERSION)
	{
		ret = mapped_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
		if (!ret)
			return ret;
	}
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <>
rx_result basic_types_algorithm<variable_type>::deserialize_type(variable_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = variable_data_algorithm::deserialize_complex_attribute(whose.variable_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	ret = mapsrc_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <>
rx_result basic_types_algorithm<struct_type>::deserialize_type(struct_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapsrc_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <>
rx_result basic_types_algorithm<source_type>::deserialize_type(source_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::deserialize_complex_attribute(whose.filter_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <>
rx_result basic_types_algorithm<mapper_type>::deserialize_type(mapper_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = filtered_data_algorithm::deserialize_complex_attribute(whose.filter_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}


template <>
rx_result basic_types_algorithm<event_type>::deserialize_type(event_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
	{
		if (!stream.read_item_reference("args", whose.arguments))
			return stream.get_error();
	}
	if (stream.get_version() >= RX_METHOD_MAPPERS_VERSION)
	{
		ret = mapped_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream, whose.complex_data);
		if (!ret)
			return ret;
	}
	if (!stream.end_object())
		return stream.get_error();
	return true;
}



template <>
rx_result basic_types_algorithm<method_type>::deserialize_type(method_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
	{
		if (!stream.read_item_reference("in", whose.inputs))
			return stream.get_error();
		if (!stream.read_item_reference("out", whose.outputs))
			return stream.get_error();
	}
	if (stream.get_version() >= RX_METHOD_MAPPERS_VERSION)
	{
		ret = mapped_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream,whose.complex_data);
		if (!ret)
			return ret;
	}
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <>
bool basic_types_algorithm<variable_type>::check_type(variable_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && variable_data_algorithm::check_complex_attribute(whose.variable_data, ctx);
	ret = ret && mapsrc_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
	return ret;
}
template <>
bool basic_types_algorithm<struct_type>::check_type(struct_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && mapsrc_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
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
bool basic_types_algorithm<event_type>::check_type(event_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	if (!whose.arguments.is_null())
	{
		auto result = data_blocks_algorithm::check_data_reference(whose.arguments, ctx.get_directories());
		if (!result)
		{
			ctx.add_error("Unable to resolve arguments data type.", RX_ITEM_NOT_FOUND, rx_medium_severity, result.errors());
		}
		ret = ret && result;
		ret = ret && mapped_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
	}
	return ret;
}


template <>
bool basic_types_algorithm<method_type>::check_type(method_type& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	if (!whose.inputs.is_null())
	{
		auto result = data_blocks_algorithm::check_data_reference(whose.inputs, ctx.get_directories());
		if (!result)
		{
			ctx.add_error("Unable to resolve inputs data type.", RX_ITEM_NOT_FOUND, rx_medium_severity, result.errors());
		}
		ret = ret && result;
		result = data_blocks_algorithm::check_data_reference(whose.outputs, ctx.get_directories());
		if (!result)
		{
			ctx.add_error("Unable to resolve outputs data type.", RX_ITEM_NOT_FOUND, rx_medium_severity, result.errors());
		}
		ret = ret && result;
		ret = ret && mapped_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
	}
	return ret;
}



template <>
rx_result basic_types_algorithm<variable_type>::construct(const variable_type& whose, construct_context& ctx, variable_type::RDataType& prototype)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret = variable_data_algorithm::construct_complex_attribute(whose.variable_data, whose.complex_data.get_names_cache() , ctx);
		if (ret)
			ret = mapsrc_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);
	}
	return ret;
}

template <>
rx_result basic_types_algorithm<struct_type>::construct(const struct_type& whose, construct_context& ctx, struct_type::RDataType& prototype)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
		ret = mapsrc_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);

	return ret;
}


template <>
rx_result basic_types_algorithm<source_type>::construct(const source_type& whose, construct_context& ctx, source_type::RDataType& prototype)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret = filtered_data_algorithm::construct_complex_attribute(whose.filter_data, whose.complex_data.get_names_cache(), ctx);
		if (ret)
		{
		}
	}
	return ret;
}
template <>
rx_result basic_types_algorithm<mapper_type>::construct(const mapper_type& whose, construct_context& ctx, mapper_type::RDataType& prototype)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret = filtered_data_algorithm::construct_complex_attribute(whose.filter_data, whose.complex_data.get_names_cache(), ctx);
	}
	return ret;
}

template <>
rx_result basic_types_algorithm<event_type>::construct(const event_type& whose, construct_context& ctx, event_type::RDataType& prototype)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		auto ret = mapped_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);
		if (ret)
		{
			if (prototype.arguments.values.empty() && prototype.arguments.children.empty() && !whose.arguments.is_null())
			{
				block_data data_proto;
				data_attribute attr;
				if (whose.arguments.is_node_id())
					attr = std::move(data_attribute("Args", whose.arguments.get_node_id()));
				else
					attr = std::move(data_attribute("Args", whose.arguments.get_path()));

				ret = data_blocks_algorithm::construct_data_attribute(attr, data_proto, ctx);
				if (ret)
					prototype.arguments = std::move(data_proto);
			}
		}
	}
	return ret;
}


template <>
rx_result basic_types_algorithm<method_type>::construct(const method_type& whose, construct_context& ctx, method_type::RDataType& prototype)
{
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		auto ret = mapped_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);
		if (ret)
		{
			if (prototype.inputs.values.empty() && prototype.inputs.children.empty() && !whose.inputs.is_null())
			{
				block_data data_proto;
				data_attribute attr;
				if (whose.inputs.is_node_id())
					attr = std::move(data_attribute("In", whose.inputs.get_node_id()));
				else
					attr = std::move(data_attribute("In", whose.inputs.get_path()));

				ret = data_blocks_algorithm::construct_data_attribute(attr, data_proto, ctx);
				if (ret)
					prototype.inputs = std::move(data_proto);
			}
			if (prototype.outputs.values.empty() && prototype.outputs.children.empty() && !whose.outputs.is_null())
			{
				block_data data_proto;
				data_attribute attr;
				if (whose.outputs.is_node_id())
					attr = std::move(data_attribute("Out", whose.outputs.get_node_id()));
				else
					attr = std::move(data_attribute("Out", whose.outputs.get_path()));

				ret = data_blocks_algorithm::construct_data_attribute(attr, data_proto, ctx);
				if (ret)
					prototype.outputs = std::move(data_proto);
			}
		}
	}
	return ret;
}


template <>
rx_result basic_types_algorithm<variable_type>::get_depends(const variable_type& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	if (ret)
	{
		ret = variable_data_algorithm::get_depends(whose.variable_data, ctx);
		if(ret)
			ret = mapsrc_data_algorithm::get_depends(whose.mapping_data, ctx);
	}
	return ret;
}
template <>
rx_result basic_types_algorithm<struct_type>::get_depends(const struct_type& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	if (ret)
		ret = mapsrc_data_algorithm::get_depends(whose.mapping_data, ctx);
	return ret;
}

template <>
rx_result basic_types_algorithm<source_type>::get_depends(const source_type& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	if (ret)
		ret = filtered_data_algorithm::get_depends(whose.filter_data, ctx);
	return ret;
}
template <>
rx_result basic_types_algorithm<mapper_type>::get_depends(const mapper_type& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	if (ret)
		ret = filtered_data_algorithm::get_depends(whose.filter_data, ctx);
	return ret;
}


template <>
rx_result basic_types_algorithm<event_type>::get_depends(const event_type& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	if (ret)
	{
		if (ret && !whose.arguments.is_null())
		{
			auto res = rx_internal::model::algorithms::resolve_reference(whose.arguments, ctx.directories);
			if (res)
				ctx.cache.emplace(res.move_value());

		}
		ret = mapped_data_algorithm::get_depends(whose.mapping_data, ctx);
	}
	return ret;
}



template <>
rx_result basic_types_algorithm<method_type>::get_depends(const method_type& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	if (ret)
	{
		if (!whose.inputs.is_null())
		{
			auto res = rx_internal::model::algorithms::resolve_reference(whose.inputs, ctx.directories);
			if (res)
				ctx.cache.emplace(res.move_value());
		}
		if (!whose.outputs.is_null())
		{
			auto res = rx_internal::model::algorithms::resolve_reference(whose.outputs, ctx.directories);
			if (res)
				ctx.cache.emplace(res.move_value());
		}
		ret = mapped_data_algorithm::get_depends(whose.mapping_data, ctx);

	}
	return ret;
}



template class basic_types_algorithm<basic_types::struct_type>;
template class basic_types_algorithm<basic_types::variable_type>;
template class basic_types_algorithm<basic_types::source_type>;
template class basic_types_algorithm<basic_types::mapper_type>;
template class basic_types_algorithm<basic_types::filter_type>;
template class basic_types_algorithm<basic_types::event_type>;
template class basic_types_algorithm<basic_types::method_type>;
template class basic_types_algorithm<basic_types::program_type>;
template class basic_types_algorithm<basic_types::display_type>;
// Parameterized Class rx_platform::meta::meta_algorithm::object_types_algorithm 


template <class typeT>
rx_result object_types_algorithm<typeT>::serialize_type (const typeT& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, typeT::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::serialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapsrc_data_algorithm::serialize_complex_attribute(whose.mapping_data, stream);
	if (!ret)
		return ret;
	ret = object_data_algorithm<typeT>::serialize_object_data(whose.object_data, stream);
	if (!ret)
		return ret;
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <class typeT>
rx_result object_types_algorithm<typeT>::deserialize_type (typeT& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();
	auto ret = complex_data_algorithm::deserialize_complex_attribute(whose.complex_data, stream);
	if (!ret)
		return ret;
	ret = mapsrc_data_algorithm::deserialize_complex_attribute(whose.mapping_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	// object data
	ret = object_data_algorithm<typeT>::deserialize_object_data(whose.object_data, stream, whose.complex_data);
	if (!ret)
		return ret;
	// end object data
	if (!stream.end_object())
		return stream.get_error();
	return true;
}

template <class typeT>
bool object_types_algorithm<typeT>::check_type (typeT& whose, type_check_context& ctx)
{
	type_check_source _(whose.meta_info.get_full_path(), &ctx);
	auto ret = complex_data_algorithm::check_complex_attribute(whose.complex_data, ctx);
	ret = ret && mapsrc_data_algorithm::check_complex_attribute(whose.mapping_data, ctx);
	ret = ret && object_data_algorithm<typeT>::check_object_data(whose.object_data, ctx);
	return ret;
}

template <class typeT>
rx_result object_types_algorithm<typeT>::construct_runtime (const typeT& whose, typename typeT::RTypePtr what, construct_context& ctx)
{
	ctx.get_directories().add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::construct_complex_attribute(whose.complex_data, ctx);
	if (ret)
	{
		ret  = mapsrc_data_algorithm::construct_complex_attribute(whose.mapping_data, whose.complex_data.get_names_cache(), ctx);
		if (ret)
		{
			ret = object_data_algorithm<typeT>::construct_object_data(whose.object_data, what, whose.complex_data.get_names_cache(), ctx);
		}
	}
	return ret;
}

template <class typeT>
rx_result object_types_algorithm<typeT>::get_depends (const typeT& whose, dependencies_context& ctx)
{
	ctx.directories.add_paths({ whose.meta_info.path });
	auto ret = complex_data_algorithm::get_depends(whose.complex_data, ctx);
	if (ret)
	{
		ret = mapsrc_data_algorithm::get_depends(whose.mapping_data,ctx);
		if (ret)
		{
			ret = object_data_algorithm<typeT>::get_depends(whose.object_data, ctx);
		}
	}
	return ret;
}



template class object_types_algorithm<object_types::object_type>;
template class object_types_algorithm<object_types::port_type>;
template class object_types_algorithm<object_types::application_type>;
template class object_types_algorithm<object_types::domain_type>;
// Class rx_platform::meta::meta_algorithm::relation_type_algorithm 


rx_result relation_type_algorithm::serialize_type (const relation_type& whose, base_meta_writer& stream, uint8_t type)
{
	if (type != STREAMING_TYPE_MESSAGE)
	{
		if (!whose.meta_info.serialize_meta_data(stream, type, relation_type::type_id))
			return stream.get_error();
	}
	if (!stream.start_object("def"))
		return stream.get_error();

	if (!stream.write_bool("sealed", whose.relation_data.sealed_type))
		return stream.get_error();
	if (!stream.write_bool("abstract", whose.relation_data.abstract_type))
		return stream.get_error();
	if (!stream.write_bool("hierarchical", whose.relation_data.hierarchical))
		return stream.get_error();
	if (!stream.write_bool("symmetrical", whose.relation_data.symmetrical))
		return stream.get_error();
	if (!stream.write_string("inverse", whose.relation_data.inverse_name.c_str()))
		return stream.get_error();
	if (!stream.write_bool("dynamic", whose.relation_data.dynamic))
		return stream.get_error();

	if (stream.get_version() >= RX_RELATION_TARGET_VERSION)
	{
		if (!stream.write_item_reference("target", whose.relation_data.target))
			return stream.get_error();
	}

	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.relation_data.description.c_str()))
			return stream.get_error();
	}

	if (!stream.end_object())
		return stream.get_error();
	return true;
}

rx_result relation_type_algorithm::deserialize_type (relation_type& whose, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return stream.get_error();

	if (!stream.read_bool("sealed", whose.relation_data.sealed_type))
		return stream.get_error();
	if (!stream.read_bool("abstract", whose.relation_data.abstract_type))
		return stream.get_error();
	if (!stream.read_bool("hierarchical", whose.relation_data.hierarchical))
		return stream.get_error();
	if (!stream.read_bool("symmetrical", whose.relation_data.symmetrical))
		return stream.get_error();
	if (!stream.read_string("inverse", whose.relation_data.inverse_name))
		return stream.get_error();
	if (!stream.read_bool("dynamic", whose.relation_data.dynamic))
		return stream.get_error();

	if (stream.get_version() >= RX_RELATION_TARGET_VERSION)
	{
		if (!stream.read_item_reference("target", whose.relation_data.target))
			return stream.get_error();
	}
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.relation_data.description))
			return stream.get_error();
	}


	if (!stream.end_object())
		return stream.get_error();
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


// Parameterized Class rx_platform::meta::meta_algorithm::object_data_algorithm 


template <class typeT>
rx_result object_data_algorithm<typeT>::serialize_object_data (const object_types::object_data_type& whose, base_meta_writer& stream)
{
	if (!stream.write_bool("constructable", whose.is_constructable))
		return stream.get_error();
	if (!stream.start_array("relations", whose.relations_.size()))
		return stream.get_error();
	for (const auto& one : whose.relations_)
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		auto ret = relation_attribute::AlgorithmType::serialize_relation_attribute(one, stream);
		if (!ret)
			return ret;

		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();

	if (!stream.start_array("programs", whose.programs_.size()))
		return stream.get_error();
	for (const auto& one : whose.programs_)
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		auto one_result = program_attribute::AlgorithmType::serialize_complex_attribute(one, stream);
		if(!one_result)
			return one_result;

		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();

	if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
	{
		if (!stream.start_array("methods", whose.methods_.size()))
			return stream.get_error();
		for (const auto& one : whose.methods_)
		{
			if (!stream.start_object("item"))
				return stream.get_error();

			auto one_result = method_attribute::AlgorithmType::serialize_complex_attribute(one, stream);
			if (!one_result)
				return one_result;

			if (!stream.end_object())
				return stream.get_error();
		}
		if (!stream.end_array())
			return stream.get_error();

		if (!stream.start_array("displays", whose.displays_.size()))
			return stream.get_error();
		for (const auto& one : whose.displays_)
		{
			if (!stream.start_object("item"))
				return stream.get_error();

			auto one_result = display_attribute::AlgorithmType::serialize_complex_attribute(one, stream);
			if (!one_result)
				return one_result;

			if (!stream.end_object())
				return stream.get_error();
		}
		if (!stream.end_array())
			return stream.get_error();
	}
	return true;
}

template <class typeT>
rx_result object_data_algorithm<typeT>::deserialize_object_data (object_types::object_data_type& whose, base_meta_reader& stream, complex_data_type& complex_data)
{
	if (!stream.read_bool("constructable", whose.is_constructable))
		return stream.get_error();
	if (!stream.start_array("relations"))
		return stream.get_error();

	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		relation_attribute one;
		auto one_result = relation_attribute::AlgorithmType::deserialize_relation_attribute(one, stream);
		if (!one_result)
			return one_result;
		auto ret = complex_data.check_name(one.name, (static_cast<int>(whose.relations_.size() | complex_data_type::relations_mask)));
		if (ret)
		{
			whose.relations_.emplace_back(std::move(one));
		}
		else
			return ret;

		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.start_array("programs"))
		return stream.get_error();
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		program_attribute temp;
		auto one_result = program_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream);
		if (!one_result)
			return one_result;
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(whose.programs_.size() | complex_data_type::programs_mask)));
		if (ret)
		{
			whose.programs_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return stream.get_error();
	}

	if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
	{
		if (!stream.start_array("methods"))
			return stream.get_error();
		while (!stream.array_end())
		{
			if (!stream.start_object("item"))
				return stream.get_error();

			method_attribute temp;
			auto one_result = method_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream);
			if (!one_result)
				return one_result;
			auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(whose.methods_.size() | complex_data_type::methods_mask)));
			if (ret)
			{
				whose.methods_.emplace_back(std::move(temp));
			}
			else
				return ret;

			if (!stream.end_object())
				return stream.get_error();
		}

		if (!stream.start_array("displays"))
			return stream.get_error();
		while (!stream.array_end())
		{
			if (!stream.start_object("item"))
				return stream.get_error();

			display_attribute temp;
			auto one_result = display_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream);
			if (!one_result)
				return one_result;
			auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(whose.displays_.size() | complex_data_type::displays_mask)));
			if (ret)
			{
				whose.displays_.emplace_back(std::move(temp));
			}
			else
				return ret;

			if (!stream.end_object())
				return stream.get_error();
		}
	}
	return true;
}

template <class typeT>
bool object_data_algorithm<typeT>::check_object_data (object_types::object_data_type& whose, type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : whose.relations_)
		ret = ret && relation_attribute::AlgorithmType::check_relation_attribute(one, ctx);
	for (auto& one : whose.programs_)
		ret = ret && program_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	for (auto& one : whose.methods_)
		ret = ret && method_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	for (auto& one : whose.displays_)
		ret = ret && display_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	return ret;
}

template <class typeT>
rx_result object_data_algorithm<typeT>::construct_object_data (const object_types::object_data_type& whose, typename typeT::RTypePtr what, const names_cahce_type& names, construct_context& ctx)
{
	rx_result ret = true;
	for (const auto& one : names)
	{
		switch (one.second & complex_data_type::type_mask)
		{
			// relations
		case complex_data_type::relations_mask:
			{
				auto data = rx_create_reference<runtime::relations::relation_data>();
				rx_result ret = relation_attribute::AlgorithmType::construct_relation_attribute(
					whose.relations_[one.second & complex_data_type::index_mask], *data, ctx);
				if (!ret)
				{
					ret.register_error("Unable to create relation "s + one.first + "!");
					return ret;
				}
				what->relations_.source_relations_.emplace_back(std::move(data));
				break;
			}
		case complex_data_type::methods_mask:
			{
				rx_result ret = method_attribute::AlgorithmType::construct_complex_attribute(
					whose.methods_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create method "s + one.first + "!");
					return ret;
				}
				//what->logic_.runtime_methods_.emplcae_back(std::move(data))
				break;
			}
		case complex_data_type::programs_mask:
			{
				rx_result ret = program_attribute::AlgorithmType::construct_complex_attribute(
					whose.programs_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create program "s + one.first + "!");
					return ret;
				}
				break;
			}
		case complex_data_type::displays_mask:
			{
				rx_result ret = display_attribute::AlgorithmType::construct_complex_attribute(
					whose.displays_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create display "s + one.first + "!");
					return ret;
				}
				break;
			}
		}
	}
	return true;
}

template <class typeT>
rx_result object_data_algorithm<typeT>::get_depends (const object_types::object_data_type& whose, dependencies_context& ctx)
{
	for (const auto& one : whose.methods_)
	{
		auto ret = rx_internal::model::algorithms::resolve_reference(one.get_target(), ctx.directories);
		if (!ret)
		{
			ret.register_error("Unable to resolve method "s + one.get_target().to_string() + "!");
			return ret.errors();
		}
		ctx.cache.emplace(ret.move_value());
	}
	for (const auto& one : whose.displays_)
	{
		auto ret = rx_internal::model::algorithms::resolve_reference(one.get_target(), ctx.directories);
		if (!ret)
		{
			ret.register_error("Unable to resolve display "s + one.get_target().to_string() + "!");
			return ret.errors();
		}
		ctx.cache.emplace(ret.move_value());
	}
	for (const auto& one : whose.programs_)
	{
		auto ret = rx_internal::model::algorithms::resolve_reference(one.get_target(), ctx.directories);
		if (!ret)
		{
			ret.register_error("Unable to resolve program "s + one.get_target().to_string() + "!");
			return ret.errors();
		}
		ctx.cache.emplace(ret.move_value());
	}
	return true;
}


// Class rx_platform::meta::meta_algorithm::relation_blocks_algorithm 


rx_result relation_blocks_algorithm::serialize_relation_attribute (const object_types::relation_attribute& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name.c_str()))
		return stream.get_error();
	if (!stream.write_item_reference("relation", whose.relation_type))
		return stream.get_error();
	if (!stream.write_item_reference("target", whose.target))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description.c_str()))
			return stream.get_error();
	}
	return true;
}

rx_result relation_blocks_algorithm::deserialize_relation_attribute (object_types::relation_attribute& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name))
		return stream.get_error();
	if (!stream.read_item_reference("relation", whose.relation_type))
		return stream.get_error();
	if (!stream.read_item_reference("target", whose.target))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description))
			return stream.get_error();
	}
	return true;
}

bool relation_blocks_algorithm::check_relation_attribute (object_types::relation_attribute& whose, type_check_context& ctx)
{
	rx_node_id target_id;
	auto resolve_result = rx_internal::model::algorithms::resolve_relation_reference(whose.relation_type, ctx.get_directories());
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

rx_result relation_blocks_algorithm::construct_relation_attribute (const object_types::relation_attribute& whose, runtime::relations::relation_data& data, construct_context& ctx)
{
	auto resolve_result = rx_internal::model::algorithms::resolve_relation_reference(whose.relation_type, ctx.get_directories());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve relation type");
		return ret.errors();
	}
	rx_node_id relation_type_id = resolve_result.value();
	rx_node_id target_base_id;
	if (!whose.target.is_null())
	{
		resolve_result = rx_internal::model::algorithms::resolve_reference(whose.target, ctx.get_directories());
		if (!resolve_result)
		{
			rx_result ret(resolve_result.errors());
			ret.register_error("Unable to resolve relation target type");
			return ret.errors();
		}
		target_base_id = resolve_result.value();
	}

	auto ret_val = rx_internal::model::platform_types_manager::instance().get_relations_repository().create_runtime(relation_type_id, ctx.rt_name(), data, ctx.get_directories());
	if (ret_val)
	{
		data.target_relation_name = replace_in_string(data.target_relation_name, RX_MACRO_SYMBOL_STR "name" RX_MACRO_SYMBOL_STR, ctx.rt_name());
		data.name = whose.name;
		if (!target_base_id.is_null())
		{
			if (data.target_base_id.is_null() ||  rx_internal::model::algorithms::is_derived_from(target_base_id, data.target_base_id))
				data.target_base_id = target_base_id;
			else
				return "Wrong relation target type!";
		}
		rx_timed_value str_val;
		str_val.assign_static("", ctx.now);
		data.value.value = str_val;
		data.implementation_ = ret_val.value().ptr;
		// this is something i still don't know about
		// it will have to wait
		//!!!!!
		/*rx_timed_value val;
		val.assign_static<string_type>("", ctx.now());
		ctx.runtime_data.add_value(whose.name, val);*/
		rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().add_functions(data.meta_info().id, ret_val.value().register_f, ret_val.value().unregister_f);
		return true;
	}
	else
	{
		return ret_val.errors();
	}
}


} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform

