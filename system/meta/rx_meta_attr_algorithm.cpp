

/****************************************************************************
*
*  system\meta\rx_meta_attr_algorithm.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_meta_attr_algorithm
#include "system/meta/rx_meta_attr_algorithm.h"

#include "model/rx_model_algorithms.h"
#include "system/runtime/rx_runtime_logic.h"
#include "system/runtime/rx_display_blocks.h"


namespace rx_platform {

namespace meta {

namespace meta_algorithm {
namespace
{
rx_subitem_type parse_subitem_type(const string_type& name)
{
	if (name == RX_CONST_VALUE_TYPE_NAME)
		return rx_subitem_type::rx_const_value_subitem;
	else if (name == RX_VALUE_TYPE_NAME)
		return rx_subitem_type::rx_value_subitem;
	else if (name == RX_CPP_STRUCT_TYPE_NAME)
		return rx_subitem_type::rx_struct_subitem;
	else if (name == RX_CPP_VARIABLE_TYPE_NAME)
		return rx_subitem_type::rx_variable_subitem;
	else if (name == RX_CPP_EVENT_TYPE_NAME)
		return rx_subitem_type::rx_event_subitem;
	else
		return rx_subitem_type::rx_invalid_subitem;
}
string_type get_subitem_type_name(rx_subitem_type type)
{
	switch (type)
	{
	case rx_subitem_type::rx_const_value_subitem:
		return RX_CONST_VALUE_TYPE_NAME;
	case rx_subitem_type::rx_value_subitem:
		return RX_VALUE_TYPE_NAME;
	case rx_subitem_type::rx_struct_subitem:
		return RX_CPP_STRUCT_TYPE_NAME;
	case rx_subitem_type::rx_variable_subitem:
		return RX_CPP_VARIABLE_TYPE_NAME;
	case rx_subitem_type::rx_event_subitem:
		return RX_CPP_EVENT_TYPE_NAME;
	default:
		return string_type();
	}
}
}


// Variable Attribute is a special case!!!
template<>
rx_result meta_blocks_algorithm<def_blocks::variable_attribute>::serialize_complex_attribute(const def_blocks::variable_attribute& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name_.c_str()))
		return stream.get_error();
	if (!stream.write_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.write_bool("ro", whose.read_only_))
		return stream.get_error();
	if (!whose.storage_.serialize("value", stream))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::variable_attribute>::deserialize_complex_attribute(def_blocks::variable_attribute& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name_))
		return stream.get_error();
	if (!stream.read_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.read_bool("ro", whose.read_only_))
		return stream.get_error();
	if (!whose.storage_.deserialize("value", stream))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::variable_attribute>::construct_complex_attribute(const def_blocks::variable_attribute& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<def_blocks::variable_attribute::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<def_blocks::variable_attribute::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	if (temp)
	{
		temp.value().value = whose.get_value(ctx.now);
		return ctx.runtime_data().add_variable(whose.name_, std::move(temp.value()), target);
	}
	else
	{
		return temp.errors();
	}
}

template<>
rx_result meta_blocks_algorithm<def_blocks::source_attribute>::serialize_complex_attribute(const def_blocks::source_attribute& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name_.c_str()))
		return stream.get_error();
	if (!stream.write_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.write_bool("input", whose.io_.input))
		return stream.get_error();
	if (!stream.write_bool("output", whose.io_.output))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::source_attribute>::deserialize_complex_attribute(def_blocks::source_attribute& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name_))
		return stream.get_error();
	if (!stream.read_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.read_bool("input", whose.io_.input))
		return stream.get_error();
	if (!stream.read_bool("output", whose.io_.output))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::source_attribute>::construct_complex_attribute(const def_blocks::source_attribute& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<def_blocks::source_attribute::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<def_blocks::source_attribute::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	if (temp)
	{
		temp.value().io_.set_input(whose.io_.input);
		temp.value().io_.set_output(whose.io_.output);
		temp.value().source_id = target;
		return ctx.runtime_data().add(whose.name_, std::move(temp.value()), target);
	}
	else
	{
		return temp.errors();
	}
}


template<>
rx_result meta_blocks_algorithm<def_blocks::mapper_attribute>::serialize_complex_attribute(const def_blocks::mapper_attribute& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name_.c_str()))
		return stream.get_error();
	if (!stream.write_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.write_bool("read", whose.io_.input))
		return stream.get_error();
	if (!stream.write_bool("write", whose.io_.output))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::mapper_attribute>::deserialize_complex_attribute(def_blocks::mapper_attribute& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name_))
		return stream.get_error();
	if (!stream.read_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.read_bool("read", whose.io_.input))
		return stream.get_error();
	if (!stream.read_bool("write", whose.io_.output))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::mapper_attribute>::construct_complex_attribute(const def_blocks::mapper_attribute& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<def_blocks::mapper_attribute::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		META_LOG_WARNING("meta_blocks_algorithm", 900, ret.errors_line());
		ctx.register_warining({ ret.errors_line() });
		return true;
	}
	target = resolve_result.value();
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<def_blocks::mapper_attribute::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	if (temp)
	{
		temp.value().io_.set_input(whose.io_.input);
		temp.value().io_.set_output(whose.io_.output);
		temp.value().mapper_id = target;
		return ctx.runtime_data().add(whose.name_, std::move(temp.value()), target);
	}
	else
	{
		META_LOG_WARNING("meta_blocks_algorithm", 900, temp.errors_line());
		ctx.register_warining({ temp.errors_line() });
		return true;
	}
}


template<>
rx_result meta_blocks_algorithm<def_blocks::filter_attribute>::serialize_complex_attribute(const def_blocks::filter_attribute& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name_.c_str()))
		return stream.get_error();
	if (!stream.write_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.write_bool("input", whose.io_.input))
		return stream.get_error();
	if (!stream.write_bool("output", whose.io_.output))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::filter_attribute>::deserialize_complex_attribute(def_blocks::filter_attribute& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name_))
		return stream.get_error();
	if (!stream.read_item_reference("target", whose.target_))
		return stream.get_error();
	if (!stream.read_bool("input", whose.io_.input))
		return stream.get_error();
	if (!stream.read_bool("output", whose.io_.output))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}
template<>
rx_result meta_blocks_algorithm<def_blocks::filter_attribute>::construct_complex_attribute(const def_blocks::filter_attribute& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<def_blocks::filter_attribute::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<def_blocks::filter_attribute::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	if (temp)
	{
		temp.value().io_.set_input(whose.io_.input);
		temp.value().io_.set_output(whose.io_.output);
		return ctx.runtime_data().add(whose.name_, std::move(temp.value()), target);
	}
	else
	{
		return temp.errors();
	}
}

template<>
rx_result meta_blocks_algorithm<def_blocks::program_attribute>::construct_complex_attribute(const def_blocks::program_attribute& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<def_blocks::filter_attribute::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	ctx.start_program(whose.get_name());
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<def_blocks::program_attribute::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	ctx.end_program();
	if (temp)
	{
		return true;
	}
	else
	{
		return temp.errors();
	}
}


template<>
rx_result meta_blocks_algorithm<def_blocks::display_attribute>::construct_complex_attribute(const def_blocks::display_attribute& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<def_blocks::filter_attribute::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	ctx.start_program(whose.get_name());
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<def_blocks::program_attribute::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	ctx.end_program();
	if (temp)
	{
		return true;
	}
	else
	{
		return temp.errors();
	}
}

// Class rx_platform::meta::meta_algorithm::variable_data_algorithm 


rx_result variable_data_algorithm::serialize_complex_attribute (const variable_data_type& whose, base_meta_writer& stream)
{
	if (!stream.start_array("filters", whose.filters_.size()))
		return stream.get_error();
	for (const auto& one : whose.filters_)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		if (!filter_attribute::AlgorithmType::serialize_complex_attribute(one, stream))
			return stream.get_error();
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();


	return true;
}

rx_result variable_data_algorithm::deserialize_complex_attribute (variable_data_type& whose, base_meta_reader& stream, complex_data_type& complex_data)
{
	if (!stream.start_array("filters"))
		return stream.get_error();
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		filter_attribute temp;
		if (!filter_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream))
			return stream.get_error();
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(whose.filters_.size() | complex_data_type::filters_mask)));
		if (ret)
		{
			whose.filters_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return stream.get_error();
	}
	return true;
}

bool variable_data_algorithm::check_complex_attribute (variable_data_type& whose, type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : whose.filters_)
		ret &= filter_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	return ret;
}

rx_result variable_data_algorithm::construct_complex_attribute (const variable_data_type& whose, const names_cahce_type& names, construct_context& ctx)
{
	for (const auto& one : names)
	{
		switch (one.second & complex_data_type::type_mask)
		{
			// filters
		case complex_data_type::filters_mask:
			{
				rx_result ret = filter_attribute::AlgorithmType::construct_complex_attribute(whose.filters_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create filter "s + one.first + "!");
					return ret;
				}
				break;
			}
		}
	}
	return true;
}


// Class rx_platform::meta::meta_algorithm::complex_data_algorithm 


rx_result complex_data_algorithm::serialize_complex_attribute (const complex_data_type& whose, base_meta_writer& stream)
{

	if (!stream.write_bool("sealed", whose.is_sealed))
		return stream.get_error();

	if (!stream.write_bool("abstract", whose.is_abstract))
		return stream.get_error();

	if (!stream.start_array("items", whose.names_cache_.size()))
		return stream.get_error();
	for (const auto& one : whose.names_cache_)
	{
		switch (one.second & complex_data_type::type_mask)
		{
		case complex_data_type::structs_mask:
			{

				if (!stream.start_object("item"))
					return stream.get_error();
				if (stream.is_string_based())
				{
					if (!stream.write_string("type", get_subitem_type_name(rx_subitem_type::rx_struct_subitem)))
						return stream.get_error();
				}
				else
				{
					if (!stream.write_byte("type", rx_subitem_type::rx_struct_subitem))
						return stream.get_error();
				}
				if (!struct_attribute::AlgorithmType::serialize_complex_attribute(whose.structs_[one.second & complex_data_type::index_mask], stream))
					return stream.get_error();
				if (!stream.end_object())
					return stream.get_error();
			}
			break;
		case complex_data_type::variables_mask:
			{

				if (!stream.start_object("item"))
					return stream.get_error();
				if (stream.is_string_based())
				{
					if (!stream.write_string("type", get_subitem_type_name(rx_subitem_type::rx_variable_subitem)))
						return stream.get_error();
				}
				else
				{
					if (!stream.write_byte("type", rx_subitem_type::rx_variable_subitem))
						return stream.get_error();
				}
				if (!variable_attribute::AlgorithmType::serialize_complex_attribute(whose.variables_[one.second & complex_data_type::index_mask], stream))
					return stream.get_error();
				if (!stream.end_object())
					return stream.get_error();
			}
			break;
		case complex_data_type::events_mask:
			{

				if (!stream.start_object("item"))
					return stream.get_error();
				if (stream.is_string_based())
				{
					if (!stream.write_string("type", get_subitem_type_name(rx_subitem_type::rx_event_subitem)))
						return stream.get_error();
				}
				else
				{
					if (!stream.write_byte("type", rx_subitem_type::rx_event_subitem))
						return stream.get_error();
				}
				if (!event_blocks_algorithm::serialize_complex_attribute(whose.events_[one.second & complex_data_type::index_mask], stream))
					return stream.get_error();
				if (!stream.end_object())
					return stream.get_error();
			}
			break;
		case complex_data_type::simple_values_mask:
			{
				if (!stream.start_object("item"))
					return stream.get_error();
				if (stream.is_string_based())
				{
					if (!stream.write_string("type", get_subitem_type_name(rx_subitem_type::rx_value_subitem)))
						return stream.get_error();
				}
				else
				{
					if (!stream.write_byte("type", rx_subitem_type::rx_value_subitem))
						return stream.get_error();
				}
				if (!whose.simple_values_[one.second & complex_data_type::index_mask].serialize_definition(stream))
					return stream.get_error();
				if (!stream.end_object())
					return stream.get_error();
			}
			break;
		case complex_data_type::complex_data_type::const_values_mask:
			{
				if (!stream.start_object("item"))
					return stream.get_error();
				if (stream.is_string_based())
				{
					if (!stream.write_string("type", get_subitem_type_name(rx_subitem_type::rx_const_value_subitem)))
						return stream.get_error();
				}
				else
				{
					if (!stream.write_byte("type", rx_subitem_type::rx_const_value_subitem))
						return stream.get_error();
				}
				if (!whose.const_values_[one.second & complex_data_type::index_mask].serialize_definition(stream))
					return stream.get_error();
				if (!stream.end_object())
					return stream.get_error();
			}
			break;

		}
	}
	if (!stream.end_array())
		return stream.get_error();

	if (!stream.write_init_values("overrides", whose.overrides_))
		return stream.get_error();

	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description_))
			return stream.get_error();
	}

	return true;
}

rx_result complex_data_algorithm::deserialize_complex_attribute (complex_data_type& whose, base_meta_reader& stream)
{
	if (!stream.read_bool("sealed", whose.is_sealed))
		return stream.get_error();

	if (!stream.read_bool("abstract", whose.is_abstract))
		return stream.get_error();

	if (!stream.start_array("items"))
		return stream.get_error();

	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		rx_subitem_type item_type = rx_subitem_type::rx_invalid_subitem;
		if (stream.is_string_based())
		{
			string_type temp;
			if (!stream.read_string("type", temp))
				return stream.get_error();
			item_type = parse_subitem_type(temp);
			if (item_type >= rx_subitem_type::rx_first_invalid_subitem)
				return temp + " is invalid item type";
		}
		else
		{
			uint8_t temp;
			if (!stream.read_byte("type", temp))
				return stream.get_error();
			if (item_type >= rx_subitem_type::rx_first_invalid_subitem)
				return "Invalid item type";
			item_type = (rx_subitem_type)temp;
		}
		switch (item_type)
		{
		case rx_subitem_type::rx_const_value_subitem:
			{
				const_value_def temp;
				if (!temp.deserialize_definition(stream))
					return stream.get_error();
				auto ret = whose.check_name(temp.get_name(), (static_cast<int>(whose.const_values_.size() | complex_data_type::complex_data_type::const_values_mask)));
				if (ret)
				{
					whose.const_values_.emplace_back(std::move(temp));
				}
				else
					return ret;
			}
			break;
		case rx_subitem_type::rx_value_subitem:
			{
				simple_value_def temp;
				if (!temp.deserialize_definition(stream))
					return stream.get_error();
				auto ret = whose.check_name(temp.get_name(), (static_cast<int>(whose.simple_values_.size() | complex_data_type::simple_values_mask)));
				if (ret)
				{
					whose.simple_values_.emplace_back(std::move(temp));
				}
				else
					return ret;
			}
			break;
		case rx_subitem_type::rx_struct_subitem:
			{
				meta::def_blocks::struct_attribute temp;
				if (!struct_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream))
					return stream.get_error();
				auto ret = whose.check_name(temp.get_name(), (static_cast<int>(whose.structs_.size() | complex_data_type::structs_mask)));
				if (ret)
				{
					whose.structs_.emplace_back(std::move(temp));
				}
				else
					return ret;
			}
			break;
		case rx_subitem_type::rx_variable_subitem:
			{
				meta::def_blocks::variable_attribute temp;
				if (!variable_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream))
					return stream.get_error();
				auto ret = whose.check_name(temp.get_name(), (static_cast<int>(whose.variables_.size() | complex_data_type::variables_mask)));
				if (ret)
				{
					whose.variables_.emplace_back(std::move(temp));
				}
				else
					return ret;

			}
			break;
		case rx_subitem_type::rx_event_subitem:
			{
				meta::def_blocks::event_attribute temp;
				if (!event_blocks_algorithm::deserialize_complex_attribute(temp, stream))
					return stream.get_error();
				auto ret = whose.check_name(temp.get_name(), (static_cast<int>(whose.events_.size() | complex_data_type::events_mask)));
				if (ret)
				{
					whose.events_.emplace_back(std::move(temp));
				}
				else
					return ret;

			}
			break;
		default:
			return get_subitem_type_name(item_type) + "is unknown type!";
		}

		if (!stream.end_object())
			return stream.get_error();
	}

	if (!stream.read_init_values("overrides", whose.overrides_))
		return stream.get_error();

	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description_))
			return stream.get_error();
	}

	return true;//!!!! NOT DONE
}

bool complex_data_algorithm::check_complex_attribute (complex_data_type& whose, type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : whose.structs_)
		ret &= struct_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	for (auto& one : whose.variables_)
		ret &= variable_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	for (auto& one : whose.events_)
		ret &= event_blocks_algorithm::check_complex_attribute(one, ctx);
	return ret;
}

rx_result complex_data_algorithm::construct_complex_attribute (const complex_data_type& whose, construct_context& ctx)
{
	for (const auto& one : whose.names_cache_)
	{
		switch (one.second & complex_data_type::type_mask)
		{
			// constant values
		case complex_data_type::const_values_mask:
			{
				rx_result ret = ctx.runtime_data().add_const_value(
					one.first,
					whose.const_values_[one.second & complex_data_type::index_mask].get_value());
				if (!ret)
				{
					ret.register_error("Unable to add const value "s + one.first + "!");
					return ret;
				}
			}
			break;
			// simple values
		case complex_data_type::simple_values_mask:
			{
				rx_result ret = ctx.runtime_data().add_value(
					one.first,
					whose.simple_values_[one.second & complex_data_type::index_mask].get_value(ctx.now),
					whose.simple_values_[one.second & complex_data_type::index_mask].get_read_only(),
					whose.simple_values_[one.second & complex_data_type::index_mask].get_persistent());
				if (!ret)
				{
					ret.register_error("Unable to add simple value "s + one.first + "!");
					return ret;
				}
			}
			break;
			// structures
		case complex_data_type::structs_mask:
			{
				rx_result ret = struct_attribute::AlgorithmType::construct_complex_attribute(whose.structs_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create struct "s + one.first + "!");
					return ret;
				}
				break;
			}
			// variables
		case complex_data_type::variables_mask:
			{
				rx_result ret = variable_attribute::AlgorithmType::construct_complex_attribute(whose.variables_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create variable "s + one.first + "!");
					return ret;
				}
				break;
			}
			// events
		case complex_data_type::events_mask:
			{
				rx_result ret = event_attribute::AlgorithmType::construct_complex_attribute(whose.events_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create event "s + one.first + "!");
					return ret;
				}
				break;
			}
		}
	}
	return true;
}


// Class rx_platform::meta::meta_algorithm::filtered_data_algorithm 


rx_result filtered_data_algorithm::serialize_complex_attribute (const filtered_data_type& whose, base_meta_writer& stream)
{
	if (!stream.start_array("filters", whose.filters_.size()))
		return stream.get_error();
	for (const auto& one : whose.filters_)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		if (!filter_attribute::AlgorithmType::serialize_complex_attribute(one, stream))
			return stream.get_error();
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();

	return true;
}

rx_result filtered_data_algorithm::deserialize_complex_attribute (filtered_data_type& whose, base_meta_reader& stream, complex_data_type& complex_data)
{
	if (!stream.start_array("filters"))
		return stream.get_error();
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		filter_attribute temp;
		if (!filter_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream))
			return stream.get_error();
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(whose.filters_.size() | complex_data_type::filters_mask)));
		if (ret)
		{
			whose.filters_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return stream.get_error();
	}
	return true;
}

bool filtered_data_algorithm::check_complex_attribute (filtered_data_type& whose, type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : whose.filters_)
		ret &= filter_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	return ret;
}

rx_result filtered_data_algorithm::construct_complex_attribute (const filtered_data_type& whose, const names_cahce_type& names, construct_context& ctx)
{
	for (const auto& one : names)
	{
		switch (one.second & complex_data_type::type_mask)
		{
			// filters
		case complex_data_type::filters_mask:
			{
				rx_result ret = filter_attribute::AlgorithmType::construct_complex_attribute(whose.filters_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create filter "s + one.first + "!");
					return ret;
				}
				break;
			}
		}
	}
	return true;
}


// Class rx_platform::meta::meta_algorithm::mapped_data_algorithm 


rx_result mapped_data_algorithm::serialize_complex_attribute (const mapped_data_type& whose, base_meta_writer& stream)
{
	if (!stream.start_array("sources", whose.sources_.size()))
		return stream.get_error();
	for (const auto& one : whose.sources_)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		if (!source_attribute::AlgorithmType::serialize_complex_attribute(one, stream))
			return stream.get_error();
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	if (!stream.start_array("mappers", whose.mappers_.size()))
		return stream.get_error();
	for (const auto& one : whose.mappers_)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		if (!mapper_attribute::AlgorithmType::serialize_complex_attribute(one, stream))
			return stream.get_error();
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result mapped_data_algorithm::deserialize_complex_attribute (mapped_data_type& whose, base_meta_reader& stream, complex_data_type& complex_data)
{
	if (!stream.start_array("sources"))
		return stream.get_error();
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		source_attribute temp;
		if (!source_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream))
			return stream.get_error();
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(whose.sources_.size() | complex_data_type::sources_mask)));
		if (ret)
		{
			whose.sources_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.start_array("mappers"))
		return stream.get_error();
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		mapper_attribute temp;
		if (!mapper_attribute::AlgorithmType::deserialize_complex_attribute(temp, stream))
			return stream.get_error();
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(whose.mappers_.size() | complex_data_type::mappings_mask)));
		if (ret)
		{
			whose.mappers_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return stream.get_error();
	}
	return true;
}

bool mapped_data_algorithm::check_complex_attribute (mapped_data_type& whose, type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : whose.sources_)
		ret &= source_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	for (auto& one : whose.mappers_)
		ret &= mapper_attribute::AlgorithmType::check_complex_attribute(one, ctx);
	return ret;
}

rx_result mapped_data_algorithm::construct_complex_attribute (const mapped_data_type& whose, const names_cahce_type& names, construct_context& ctx)
{
	for (const auto& one : names)
	{
		switch (one.second & complex_data_type::type_mask)
		{
			// sources
		case complex_data_type::sources_mask:
			{
				rx_result ret = source_attribute::AlgorithmType::construct_complex_attribute(whose.sources_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create source "s + one.first + "!");
					return ret;
				}
				break;
			}
			// mappers
		case complex_data_type::mappings_mask:
			{
				rx_result ret = mapper_attribute::AlgorithmType::construct_complex_attribute(whose.mappers_[one.second & complex_data_type::index_mask], ctx);
				if (!ret)
				{
					ret.register_error("Unable to create mapper "s + one.first + "!");
					return ret;
				}
				break;
			}
		}
	}
	return true;
}


// Class rx_platform::meta::meta_algorithm::data_blocks_algorithm 


rx_result data_blocks_algorithm::serialize_data_attribute (const def_blocks::data_attribute& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name_))
		return stream.get_error();
	if (!stream.write_item_reference("target", whose.target_))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}

rx_result data_blocks_algorithm::deserialize_data_attribute (def_blocks::data_attribute& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name_))
		return stream.get_error();
	if (!stream.read_item_reference("target", whose.target_))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}

bool data_blocks_algorithm::check_data_attribute (def_blocks::data_attribute& whose, type_check_context& ctx)
{
	auto result = check_data_reference(whose.target_, ctx.get_directories());
	if (!result)
	{
		ctx.add_error("Unable to resolve attribute", RX_ITEM_NOT_FOUND, rx_medium_severity, result.errors());
		return true;
	}
	else
		return ctx.is_check_ok();
}

rx_result data_blocks_algorithm::construct_data_attribute (const def_blocks::data_attribute& whose, data_blocks_prototype& data, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_data_type_reference(whose.target_, ctx.get_directories());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	auto temp = rx_internal::model::platform_types_manager::instance().get_data_types_repository().create_data_type(target, whose.name_, ctx, ctx.get_directories());
	if (temp)
	{
		data = temp.move_value();
		return true;
	}
	else
	{
		return temp.errors();
	}
}

rx_result data_blocks_algorithm::check_data_reference (const rx_item_reference& ref, ns::rx_directory_resolver& dirs)
{
	rx_node_id target_id;
	auto resolve_result = rx_internal::model::algorithms::resolve_data_type_reference(ref, dirs);
	if (!resolve_result)
		return resolve_result.errors();

	target_id = resolve_result.value();
	auto target = rx_internal::model::platform_types_manager::instance().get_data_types_repository().get_type_definition(target_id);
	if (!target)
		return target.errors();
	else
		return true;
}


// Class rx_platform::meta::meta_algorithm::event_blocks_algorithm 


rx_result event_blocks_algorithm::serialize_complex_attribute (const def_blocks::event_attribute& whose, base_meta_writer& stream)
{

	if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
	{
		if (!stream.write_item_reference("args", whose.arguments_))
			return stream.get_error();
	}
	auto ret = meta_blocks_algorithm<event_attribute>::serialize_complex_attribute(whose, stream);
	return ret;
}

rx_result event_blocks_algorithm::deserialize_complex_attribute (def_blocks::event_attribute& whose, base_meta_reader& stream)
{
	auto ret = meta_blocks_algorithm<event_attribute>::deserialize_complex_attribute(whose, stream);
	if (ret)
	{
		if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
		{
			if (!stream.read_item_reference("args", whose.arguments_))
				return stream.get_error();
		}
	}
	return ret;
}

bool event_blocks_algorithm::check_complex_attribute (def_blocks::event_attribute& whose, type_check_context& ctx)
{
	auto ret = meta_blocks_algorithm<event_attribute>::check_complex_attribute(whose, ctx);
	if (ret)
	{
		if (!whose.arguments_.is_null())
		{
			auto result = data_blocks_algorithm::check_data_reference(whose.arguments_, ctx.get_directories());
			if (!result)
			{
				ctx.add_error("Unable to resolve arguments data type.", RX_ITEM_NOT_FOUND, rx_medium_severity, result.errors());
				ret = ctx.is_check_ok();
			}
		}
	}
	return ret;
}

rx_result event_blocks_algorithm::construct_complex_attribute (const def_blocks::event_attribute& whose, construct_context& ctx)
{
	auto ret = meta_blocks_algorithm<event_attribute>::construct_complex_attribute(whose, ctx);
	if (ret)
	{

	}
	return ret;
}


// Parameterized Class rx_platform::meta::meta_algorithm::meta_blocks_algorithm 


template <class typeT>
rx_result meta_blocks_algorithm<typeT>::serialize_complex_attribute (const typeT& whose, base_meta_writer& stream)
{
	if (!stream.write_string("name", whose.name_))
		return stream.get_error();
	if (!stream.write_item_reference("target", whose.target_))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}

template <class typeT>
rx_result meta_blocks_algorithm<typeT>::deserialize_complex_attribute (typeT& whose, base_meta_reader& stream)
{
	if (!stream.read_string("name", whose.name_))
		return stream.get_error();
	if (!stream.read_item_reference("target", whose.target_))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", whose.description_))
			return stream.get_error();
	}
	return true;
}

template <class typeT>
bool meta_blocks_algorithm<typeT>::check_complex_attribute (typeT& whose, type_check_context& ctx)
{
	rx_node_id target_id;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<typename typeT::TargetType>());
	if (!resolve_result)
	{
		ctx.add_error("Unable to resolve attribute", RX_ITEM_NOT_FOUND, rx_medium_severity, resolve_result.errors());
		return true;
	}
	target_id = resolve_result.value();
	auto target = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<typename typeT::TargetType>().get_type_definition(target_id);
	if (!target)
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(typeT::TargetType::type_id)
			<< " in attribute "
			<< whose.name_;

		ctx.add_error(ss.str(), RX_ITEM_NOT_FOUND, rx_medium_severity, target.errors());
	}
	return ctx.is_check_ok();
}

template <class typeT>
rx_result meta_blocks_algorithm<typeT>::construct_complex_attribute (const typeT& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<typename typeT::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<typename typeT::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	if (temp)
	{
		return ctx.runtime_data().add(whose.name_, std::move(temp.value()), target);
	}
	else
	{
		return temp.errors();
	}
}


// explicit templates initialization
template class meta_blocks_algorithm<variable_attribute>;
template class meta_blocks_algorithm<struct_attribute>;
template class meta_blocks_algorithm<filter_attribute>;
template class meta_blocks_algorithm<source_attribute>;
template class meta_blocks_algorithm<mapper_attribute>;
template class meta_blocks_algorithm<program_attribute>;
template class meta_blocks_algorithm<display_attribute>;
// Class rx_platform::meta::meta_algorithm::method_blocks_algorithm 


rx_result method_blocks_algorithm::serialize_complex_attribute (const def_blocks::method_attribute& whose, base_meta_writer& stream)
{
	auto ret = meta_blocks_algorithm<method_attribute>::serialize_complex_attribute(whose, stream);
	if (ret)
	{
		if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
		{
			if (!stream.write_item_reference("inputs", whose.inputs_))
				return stream.get_error();
			if (!stream.write_item_reference("outputs", whose.outputs_))
				return stream.get_error();
		}
	}
	return ret;
}

rx_result method_blocks_algorithm::deserialize_complex_attribute (def_blocks::method_attribute& whose, base_meta_reader& stream)
{
	auto ret = meta_blocks_algorithm<method_attribute>::deserialize_complex_attribute(whose, stream);
	if (ret)
	{
		if (stream.get_version() >= RX_EVENT_METHOD_DATA_VERSION)
		{
			if (!stream.read_item_reference("inputs", whose.inputs_))
				return stream.get_error();
			if (!stream.read_item_reference("outputs", whose.outputs_))
				return stream.get_error();
		}
	}
	return ret;
}

bool method_blocks_algorithm::check_complex_attribute (def_blocks::method_attribute& whose, type_check_context& ctx)
{
	auto ret = meta_blocks_algorithm<method_attribute>::check_complex_attribute(whose, ctx);
	if (ret)
	{
		if (!whose.inputs_.is_null())
		{
			auto result = data_blocks_algorithm::check_data_reference(whose.inputs_, ctx.get_directories());
			if (!result)
			{
				ctx.add_error("Unable to resolve inputs data type.", RX_ITEM_NOT_FOUND, rx_medium_severity, result.errors());
				ret = ctx.is_check_ok();
			}
		}
		if (ret)
		{
			if (!whose.outputs_.is_null())
			{
				auto result = data_blocks_algorithm::check_data_reference(whose.outputs_, ctx.get_directories());
				if (!result)
				{
					ctx.add_error("Unable to resolve outputs data type.", RX_ITEM_NOT_FOUND, rx_medium_severity, result.errors());
					ret = ctx.is_check_ok();
				}
			}
		}
	}
	return ret;
}

rx_result method_blocks_algorithm::construct_complex_attribute (const def_blocks::method_attribute& whose, construct_context& ctx)
{
	rx_node_id target;
	auto resolve_result = rx_internal::model::algorithms::resolve_simple_type_reference(whose.target_, ctx.get_directories(), tl::type2type<def_blocks::filter_attribute::TargetType>());
	if (!resolve_result)
	{
		rx_result ret(resolve_result.errors());
		ret.register_error("Unable to resolve attribute");
		return ret;
	}
	target = resolve_result.value();
	ctx.start_method(whose.get_name());
	auto temp = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<def_blocks::method_attribute::TargetType>().create_simple_runtime(target, whose.name_, ctx);
	ctx.end_method();
	if (temp)
	{
		return true;
	}
	else
	{
		return temp.errors();
	}
}


} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform

