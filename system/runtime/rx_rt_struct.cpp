

/****************************************************************************
*
*  system\runtime\rx_rt_struct.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"

#include "rx_blocks.h"
#include "rx_objbase.h"
#include "rx_relations.h"
#include "system/serialization/rx_ser.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace runtime {

namespace structure {


template class runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3f>;

template class runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x00>;

namespace
{

runtime_filters_type g_empty_filters;
runtime_sources_type g_empty_sources;
runtime_mappers_type g_empty_mappers;
runtime_events_type g_empty_events;
runtime_structs_type g_empty_structs;
runtime_variables_type g_empty_variables;

}


// Parameterized Class rx_platform::runtime::structure::runtime_data 


template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	for (const auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			// const value
			data.add_value(one.name, const_values[(one.index >> rt_type_shift)].value);
			break;
		case rt_value_index_type:
			// simple value
			data.add_value(one.name, values[(one.index >> rt_type_shift)].value.to_simple());
			break;
		case rt_variable_index_type:
			if constexpr (has_variables())
			{
				if (type == runtime_value_type::simple_runtime_value)
				{
					data.add_value(one.name, variables.collection[(one.index >> rt_type_shift)].value.to_simple());
				}
				else
				{
					auto& result = data.add_child(one.name);
					variables.collection[(one.index >> rt_type_shift)].collect_data(result, type);
				}
			}
			break;
		case rt_struct_index_type:
			{
				if constexpr (has_structs())
				{
					auto& result = data.add_child(one.name);
					structs.collection[(one.index >> rt_type_shift)].collect_data(result, type);
				}
			}
			break;
		case rt_source_index_type:
			{
				if constexpr (has_sources())
				{
					if ((type & runtime_value_type::sources_runtime_value) == runtime_value_type::sources_runtime_value)
					{
						auto& result = data.add_child(one.name);
						sources.collection[(one.index >> rt_type_shift)].collect_data(result, type);
					}
				}
			}
			break;
		case rt_mapper_index_type:
			{
				if constexpr (has_mappers())
				{
					if ((type & runtime_value_type::mappers_runtime_value) == runtime_value_type::mappers_runtime_value)
					{
						auto& result = data.add_child(one.name);
						mappers.collection[(one.index >> rt_type_shift)].collect_data(result, type);
					}
				}
			}
			break;
		case rt_filter_index_type:
			{
				if constexpr (has_filters())
				{
					if ((type & runtime_value_type::filters_runtime_value) == runtime_value_type::filters_runtime_value)
					{
						auto& result = data.add_child(one.name);
						filters.collection[(one.index >> rt_type_shift)].collect_data(result, type);
					}
				}
			}
			break;
		case rt_event_index_type:
			{
				if constexpr (has_events())
				{
					if ((type & runtime_value_type::events_runtime_value) == runtime_value_type::events_runtime_value)
					{
						auto& result = data.add_child(one.name);
						events.collection[(one.index >> rt_type_shift)].collect_data(result, type);
					}
				}
			}
			break;
		default:
			RX_ASSERT(false);
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::fill_data (const data::runtime_values_data& data, fill_context& ctx)
{
	for (auto one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			{// const value
				auto it = data.values.find(one.name);
				if (it != data.values.end())
				{
					const_values[one.index >> rt_type_shift].set_value(rx_simple_value(it->second.value));
				}
			}
			break;
		case rt_value_index_type:
			{// value
				auto it = data.values.find(one.name);
				if (it != data.values.end())
				{
					values[one.index >> rt_type_shift].set_value(rx_simple_value(it->second.value), ctx.now);
				}
			}
			break;
		case rt_variable_index_type:
			{// variable
				// check for simple value first
				auto it_vals = data.values.find(one.name);
				if (it_vals != data.values.end())
				{
					variables.collection[one.index >> rt_type_shift].set_value(rx_simple_value(it_vals->second.value), ctx);
				}
				// now check for complex values
				auto it = data.children.find(one.name);
				if (it != data.children.end())
				{
					variables.collection[one.index >> rt_type_shift].fill_data(it->second, ctx);
				}
			}
			break;
		case rt_struct_index_type:
			{// struct
				auto it = data.children.find(one.name);
				if (it != data.children.end())
				{
					structs.collection[one.index >> rt_type_shift].fill_data(it->second, ctx);
				}
			}
			break;
		case rt_source_index_type:
			{// source
				auto it = data.children.find(one.name);
				if (it != data.children.end())
				{
					sources.collection[one.index >> rt_type_shift].fill_data(it->second, ctx);
				}
			}
			break;
		case rt_mapper_index_type:
			{// source
				auto it = data.children.find(one.name);
				if (it != data.children.end())
				{
					mappers.collection[one.index >> rt_type_shift].fill_data(it->second, ctx);
				}
			}
			break;
		case rt_filter_index_type:
			{// source
				auto it = data.children.find(one.name);
				if (it != data.children.end())
				{
					filters.collection[one.index >> rt_type_shift].fill_data(it->second, ctx);
				}
			}
			break;
		case rt_event_index_type:
			{// source
				auto it = data.children.find(one.name);
				if (it != data.children.end())
				{
					events.collection[one.index >> rt_type_shift].fill_data(it->second, ctx);
				}
			}
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{// our value
		rx_result result;
#ifdef RX_MIN_MEMORY
		if (!json_cache_.empty())
		{
			value.assign_static<string_type>(string_type(json_cache_), meta_info_.get_modified_time());
		}
		else
		{
			serialization::json_writer writer;
			writer.write_header(STREAMING_TYPE_MESSAGE, 0);
			result = serialize_value(writer, runtime_value_type::simple_runtime_value);
			if (result)
			{
#ifdef _DEBUG
				if (writer.get_string(const_cast<string_type&>(json_cache_), true))
				{
#else
				writer.get_string(const_cast<runtime_holder<typeT>*>(this)->json_cache_, false);
#endif
				value.assign_static<string_type>(string_type(json_cache_), meta_info_.get_modified_time());
				}
			}
		}
#else
		serialization::json_writer writer;
		writer.write_header(STREAMING_TYPE_MESSAGE, 0);
		data::runtime_values_data data;
		collect_data(data, runtime_value_type::simple_runtime_value);
		if (!writer.write_init_values(nullptr, data))
		{
			result = "Error writing values to the stream";
		}
		else
		{
			string_type temp_str;
			if (writer.get_string(const_cast<string_type&>(temp_str), true))
			{
				val.assign_static<string_type>(string_type(temp_str), ctx->now);
				result = true;
			}
			else
			{
				result = "Retrieving string from JSON stream";
			}
		}
		return result;
#endif
	}
	else
	{
		string_type mine;
		string_type bellow;
		size_t idx = path.find(RX_OBJECT_DELIMETER);
		if (idx != string_type::npos)
		{
			mine = path.substr(0, idx);
			bellow = path.substr(idx + 1);
		}
		else
		{
			mine = path;
			bellow = "";
		}
		auto& sub_item = get_sub_item(mine, false);
		if (sub_item)
		{
			return sub_item->get_value(bellow, val, ctx);
		}
		else// its' ours
		{
			auto idx = internal_get_index(path);
			if (idx && is_value_index(idx))
			{
				switch (idx & rt_type_mask)
				{
				case rt_const_index_type:
					val = const_values[idx >> rt_type_shift].get_value(ctx);
					return true;
				case rt_value_index_type:
					val = values[idx >> rt_type_shift].get_value(ctx);
					return true;
				case rt_variable_index_type:
					val = variables.collection[idx >> rt_type_shift].get_value(ctx);
					return true;
				}
			}
		}
		return mine + " not found!";
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::object_state_changed (runtime_process_context* ctx)
{
	if (has_structs())
	{
		for (auto& one : structs.collection)
			one.item->object_state_changed(ctx);
	}
	if (has_variables())
	{
		for (auto& one : variables.collection)
			one.item->object_state_changed(ctx);
	}
	for (auto& one : values)
		one.object_state_changed(ctx);
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
members_index_type runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::internal_get_index (const string_type& name) const
{
	for (const auto& one : items)
	{
		if (one.name == name)
			return one.index;
	}
	return 0;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::is_value_index (members_index_type idx) const
{
	auto temp = idx & rt_type_mask;
	return temp == rt_const_index_type || temp == rt_value_index_type || temp == rt_variable_index_type;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::is_complex_index (members_index_type idx) const
{
	auto temp = idx & rt_type_mask;
	return temp != rt_const_index_type && temp != rt_value_index_type && temp;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::initialize_runtime (runtime::runtime_init_context& ctx)
{
	rx_result ret(true);
	for (auto one : items)
	{
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			ctx.path.push_to_path(one.name);
			ret = variables.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_struct_index_type:
			// struct
			ctx.path.push_to_path(one.name);
			ret = structs.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_source_index_type:
			// source
			ctx.path.push_to_path(one.name);
			ret = sources.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_mapper_index_type:
			// mapper
			ctx.path.push_to_path(one.name);
			ret = mappers.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_filter_index_type:
			// filter
			ctx.path.push_to_path(one.name);
			ret = filters.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_event_index_type:
			// event
			ctx.path.push_to_path(one.name);
			ret = events.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
		}
		if (!ret)
			break;
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	rx_result ret(true);
	for (auto one : items)
	{
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			ret = variables.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_struct_index_type:
			// struct
			ret = structs.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_source_index_type:
			// source
			ret = sources.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_mapper_index_type:
			// mapper
			ret = mappers.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_filter_index_type:
			// filter
			ret = filters.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_event_index_type:
			// event
			ret = events.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
		}
		if (!ret)
			break;
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::start_runtime (runtime::runtime_start_context& ctx)
{
	rx_result ret(true);
	for (auto one : items)
	{
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			ctx.path.push_to_path(one.name);
			ret = variables.collection[one.index >> rt_type_shift].start_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_struct_index_type:
			// struct
			ctx.path.push_to_path(one.name);
			ret = structs.collection[one.index >> rt_type_shift].start_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_source_index_type:
			// source
			ctx.path.push_to_path(one.name);
			ret = sources.collection[one.index >> rt_type_shift].start_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_mapper_index_type:
			// mapper
			ctx.path.push_to_path(one.name);
			ret = mappers.collection[one.index >> rt_type_shift].start_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_filter_index_type:
			// filter
			ctx.path.push_to_path(one.name);
			ret = filters.collection[one.index >> rt_type_shift].start_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_event_index_type:
			// event
			ctx.path.push_to_path(one.name);
			ret = events.collection[one.index >> rt_type_shift].start_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
		}
		if (!ret)
			break;
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::stop_runtime (runtime::runtime_stop_context& ctx)
{
	rx_result ret(true);
	for (auto one : items)
	{
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			ret = variables.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_struct_index_type:
			// struct
			ret = structs.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_source_index_type:
			// source
			ret = sources.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_mapper_index_type:
			// mapper
			ret = mappers.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_filter_index_type:
			// filter
			ret = filters.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_event_index_type:
			// event
			ret = events.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
		}
		if (!ret)
			break;
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
const runtime_item::smart_ptr& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_sub_item (const string_type& path, bool include_vars) const
{
	static runtime_item::smart_ptr g_empty;

	auto idx = internal_get_index(path);
	if (idx && ((include_vars && is_complex_index(idx)) || (!include_vars && !is_value_index(idx))))
	{
		switch (idx&rt_type_mask)
		{
		case rt_variable_index_type:
			return variables.collection[idx >> rt_type_shift].item;
		case rt_struct_index_type:
			return structs.collection[idx >> rt_type_shift].item;
		case rt_source_index_type:
			return sources.collection[idx >> rt_type_shift].item;
		case rt_mapper_index_type:
			return mappers.collection[idx >> rt_type_shift].item;
		case rt_filter_index_type:
			return filters.collection[idx >> rt_type_shift].item;
		case rt_event_index_type:
			return events.collection[idx >> rt_type_shift].item;
		}
	}
	return g_empty;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
runtime_item::smart_ptr& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_sub_item (const string_type& path, bool include_vars)
{
	static runtime_item::smart_ptr g_empty;

	auto idx = internal_get_index(path);
	if (idx && is_complex_index(idx))
	{
		switch (idx&rt_type_mask)
		{
		case rt_variable_index_type:
			return variables.collection[idx >> rt_type_shift].item;
		case rt_struct_index_type:
			return structs.collection[idx >> rt_type_shift].item;
		case rt_source_index_type:
			return sources.collection[idx >> rt_type_shift].item;
		case rt_mapper_index_type:
			return mappers.collection[idx >> rt_type_shift].item;
		case rt_filter_index_type:
			return filters.collection[idx >> rt_type_shift].item;
		case rt_event_index_type:
			return events.collection[idx >> rt_type_shift].item;
		}
	}
	return g_empty;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_value_ref (const string_type& path, rt_value_ref& ref)
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto& sub_item = get_sub_item(mine);
		if (sub_item)
			return sub_item->get_value_ref(bellow, ref);
	}
	else// its' ours
	{
		auto idx = internal_get_index(path);
		if (idx && is_value_index(idx))
		{
			switch (idx&rt_type_mask)
			{
			case rt_const_index_type:
				ref.ref_type = rt_value_ref_type::rt_const_value;
				ref.ref_value_ptr.const_value = &const_values[idx >> rt_type_shift];
				return true;
			case rt_value_index_type:
				ref.ref_type = rt_value_ref_type::rt_value;
				ref.ref_value_ptr.value = &values[idx >> rt_type_shift];
				return true;
			case rt_variable_index_type:
				ref.ref_type = rt_value_ref_type::rt_variable;
				ref.ref_value_ptr.variable = &variables.collection[idx >> rt_type_shift];
				return true;
			default:
				RX_ASSERT(false);// has to be because of is_value_index;
			}
		}
	}
	return path + " not found!";
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::browse_items (const string_type& filter, const string_type& current_path, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	for (const auto& one : this->items)
	{
		runtime_item_attribute one_attr;
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
			one_attr.name = one.name;
			one_attr.value = const_values[(one.index >> rt_type_shift)].get_value(ctx);
			one_attr.type = rx_attribute_type::const_attribute_type;
			break;
		case rt_value_index_type:
			one_attr.name = one.name;
			one_attr.value = values[(one.index >> rt_type_shift)].get_value(ctx);
			one_attr.type = rx_attribute_type::value_attribute_type;
			break;
		case rt_variable_index_type:
			one_attr.name = one.name;
			if constexpr (has_variables())
				one_attr.value = variables.collection[(one.index >> rt_type_shift)].get_value(ctx);
			one_attr.type = rx_attribute_type::variable_attribute_type;
			break;
		case rt_struct_index_type:
			one_attr.name = one.name;
			one_attr.type = rx_attribute_type::struct_attribute_type;
			break;
		case rt_source_index_type:
			one_attr.name = one.name;
			one_attr.type = rx_attribute_type::source_attribute_type;
			break;
		case rt_mapper_index_type:
			one_attr.name = one.name;
			one_attr.type = rx_attribute_type::mapper_attribute_type;
			break;
		case rt_filter_index_type:
			one_attr.name = one.name;
			one_attr.type = rx_attribute_type::filter_attribute_type;
			break;
		case rt_event_index_type:
			one_attr.name = one.name;
			one_attr.type = rx_attribute_type::event_attribute_type;
			break;
		default:
			one_attr.type = rx_attribute_type::invalid_attribute_type_type;
		}
		if (one_attr.type != rx_attribute_type::invalid_attribute_type_type)
		{
			one_attr.full_path = current_path + one.name;
			items.emplace_back(std::move(one_attr));
		}
	}
	return true;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
const runtime_item* runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::faster_get_child_item (const char* path, size_t& idx) const
{
	const runtime_item* ret = nullptr;
	size_t start_idx = idx;
	// find our part
	while (path[idx] != RX_OBJECT_DELIMETER && path[idx] != '\0') idx++;
	// check for empty
	size_t name_size = idx - start_idx;
	if (name_size == 0)
		return nullptr;
	// now compare it
	for (const auto& one : this->items)
	{
		// hard-core memcmp ;)
		if (is_complex_index(one.index) && !one.name.empty() && name_size == one.name.size()
			&& memcmp(&path[start_idx], one.name.c_str(), name_size) == 0)
		{
			switch (one.index&rt_type_mask)
			{
			case rt_variable_index_type:
				ret = variables.collection[one.index >> rt_type_shift].item.get();
				break;
			case rt_struct_index_type:
				ret = structs.collection[one.index >> rt_type_shift].item.get();
				break;
			case rt_source_index_type:
				ret = sources.collection[one.index >> rt_type_shift].item.get();
				break;
			case rt_mapper_index_type:
				ret = mappers.collection[one.index >> rt_type_shift].item.get();
				break;
			case rt_filter_index_type:
				ret = filters.collection[one.index >> rt_type_shift].item.get();
				break;
			case rt_event_index_type:
				ret = events.collection[one.index >> rt_type_shift].item.get();
				break;
			}
		}
		if (ret)
			break;
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
runtime_filters_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_filters ()
{
	if constexpr (has_filters())
	{
		return filters.collection;
	}
	else
	{
		return g_empty_filters;
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
runtime_sources_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_sources ()
{
	if constexpr (has_sources())
	{
		return sources.collection;
	}
	else
	{
		return g_empty_sources;
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
runtime_mappers_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_mappers ()
{
	if constexpr (has_mappers())
	{
		return mappers.collection;
	}
	else
	{
		return g_empty_mappers;
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
runtime_events_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_events ()
{
	if constexpr (has_events())
	{
		return events.collection;
	}
	else
	{
		return g_empty_events;
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
runtime_variables_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_variables ()
{
	if constexpr (has_variables())
	{
		return variables.collection;
	}
	else
	{
		return g_empty_variables;
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
runtime_structs_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_structs ()
{
	if constexpr (has_structs())
	{
		return structs.collection;
	}
	else
	{
		return g_empty_structs;
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_local_value (const string_type& path, rx_simple_value& val) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto& sub_item = get_sub_item(mine);
		if (sub_item)
			return sub_item->get_local_value(bellow, val);
	}
	else// its' ours
	{
		auto idx = internal_get_index(path);
		if (idx && is_value_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_const_index_type:
				val = const_values[idx >> rt_type_shift].simple_get_value();
				return true;
			case rt_value_index_type:
				val = values[idx >> rt_type_shift].simple_get_value();
				return true;
			}
		}
	}
	return path + " not found!";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////


runtime_item::smart_ptr create_runtime_data(uint_fast8_t type_id)
{
	switch (type_id)
	{
	case 0x00:
		return std::make_unique<runtime_data_type00>();
	case 0x01:
		return std::make_unique<runtime_data_type01>();
	case 0x02:
		return std::make_unique<runtime_data_type02>();
	case 0x03:
		return std::make_unique<runtime_data_type03>();
	case 0x04:
		return std::make_unique<runtime_data_type04>();
	case 0x05:
		return std::make_unique<runtime_data_type05>();
	case 0x06:
		return std::make_unique<runtime_data_type06>();
	case 0x07:
		return std::make_unique<runtime_data_type07>();
	case 0x08:
		return std::make_unique<runtime_data_type08>();
	case 0x09:
		return std::make_unique<runtime_data_type09>();
	case 0x0a:
		return std::make_unique<runtime_data_type0a>();
	case 0x0b:
		return std::make_unique<runtime_data_type0b>();
	case 0x0c:
		return std::make_unique<runtime_data_type0c>();
	case 0x0d:
		return std::make_unique<runtime_data_type0d>();
	case 0x0e:
		return std::make_unique<runtime_data_type0e>();
	case 0x0f:
		return std::make_unique<runtime_data_type0f>();
	case 0x10:
		return std::make_unique<runtime_data_type10>();
	case 0x11:
		return std::make_unique<runtime_data_type11>();
	case 0x12:
		return std::make_unique<runtime_data_type12>();
	case 0x13:
		return std::make_unique<runtime_data_type13>();
	case 0x14:
		return std::make_unique<runtime_data_type14>();
	case 0x15:
		return std::make_unique<runtime_data_type15>();
	case 0x16:
		return std::make_unique<runtime_data_type16>();
	case 0x17:
		return std::make_unique<runtime_data_type17>();
	case 0x18:
		return std::make_unique<runtime_data_type18>();
	case 0x19:
		return std::make_unique<runtime_data_type19>();
	case 0x1a:
		return std::make_unique<runtime_data_type1a>();
	case 0x1b:
		return std::make_unique<runtime_data_type1b>();
	case 0x1c:
		return std::make_unique<runtime_data_type1c>();
	case 0x1d:
		return std::make_unique<runtime_data_type1d>();
	case 0x1e:
		return std::make_unique<runtime_data_type1e>();
	case 0x1f:
		return std::make_unique<runtime_data_type1f>();
	case 0x20:
		return std::make_unique<runtime_data_type20>();
	case 0x21:
		return std::make_unique<runtime_data_type21>();
	case 0x22:
		return std::make_unique<runtime_data_type22>();
	case 0x23:
		return std::make_unique<runtime_data_type23>();
	case 0x24:
		return std::make_unique<runtime_data_type24>();
	case 0x25:
		return std::make_unique<runtime_data_type25>();
	case 0x26:
		return std::make_unique<runtime_data_type26>();
	case 0x27:
		return std::make_unique<runtime_data_type27>();
	case 0x28:
		return std::make_unique<runtime_data_type28>();
	case 0x29:
		return std::make_unique<runtime_data_type29>();
	case 0x2a:
		return std::make_unique<runtime_data_type2a>();
	case 0x2b:
		return std::make_unique<runtime_data_type2b>();
	case 0x2c:
		return std::make_unique<runtime_data_type2c>();
	case 0x2d:
		return std::make_unique<runtime_data_type2d>();
	case 0x2e:
		return std::make_unique<runtime_data_type2e>();
	case 0x2f:
		return std::make_unique<runtime_data_type2f>();
	case 0x30:
		return std::make_unique<runtime_data_type30>();
	case 0x31:
		return std::make_unique<runtime_data_type31>();
	case 0x32:
		return std::make_unique<runtime_data_type32>();
	case 0x33:
		return std::make_unique<runtime_data_type33>();
	case 0x34:
		return std::make_unique<runtime_data_type34>();
	case 0x35:
		return std::make_unique<runtime_data_type35>();
	case 0x36:
		return std::make_unique<runtime_data_type36>();
	case 0x37:
		return std::make_unique<runtime_data_type37>();
	case 0x38:
		return std::make_unique<runtime_data_type38>();
	case 0x39:
		return std::make_unique<runtime_data_type39>();
	case 0x3a:
		return std::make_unique<runtime_data_type3a>();
	case 0x3b:
		return std::make_unique<runtime_data_type3b>();
	case 0x3c:
		return std::make_unique<runtime_data_type3c>();
	case 0x3d:
		return std::make_unique<runtime_data_type3d>();
	case 0x3e:
		return std::make_unique<runtime_data_type3e>();
	case 0x3f:
		return std::make_unique<runtime_data_type3f>();
	}
	return runtime_item::smart_ptr();
}

// Parameterized Class rx_platform::runtime::structure::empty 


// Parameterized Class rx_platform::runtime::structure::has 


// Class rx_platform::runtime::structure::variable_data 

string_type variable_data::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_data::variable_data()
{
	pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, variable_write_task*> >();
}

variable_data::variable_data (runtime_item::smart_ptr&& rt, variable_runtime_ptr&& var)
	: item(std::move(rt))
	, variable_ptr(std::move(var))
{
	pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, variable_write_task*> >();
}



void variable_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	data.add_value(RX_DEFAULT_VARIABLE_NAME, value.to_simple());
	item->collect_data(data, type);
}

void variable_data::fill_data (const data::runtime_values_data& data, fill_context& ctx)
{
	auto it = data.values.find(RX_DEFAULT_VARIABLE_NAME);
	if (it != data.values.end())
	{
		rx_value_t my_type = value.get_type();
		value = rx_value::from_simple(it->second.value, ctx.now);
		value.convert_to(my_type);
	}
	item->fill_data(data, ctx);
}

rx_value variable_data::get_value (runtime_process_context* ctx) const
{
	return ctx->adapt_value(value);
}

void variable_data::set_value (rx_simple_value&& val, fill_context& ctx)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_value::from_simple(std::move(val), ctx.now);
	}
}

rx_result variable_data::write_value (write_data&& data, variable_write_task* task, runtime_process_context* ctx)
{
	if (!data.value.convert_to(value.get_type()))
		return "Invalid conversion";
	auto& filters = item->get_filters();
	rx_result result;
	if (!filters.empty())
	{
		for (auto& filter : filters)
		{
			if (filter.is_output()
				|| !(result = filter.filter_output(data.value, ctx)))
				break;
		}
		if (!result)
		{
			result.register_error("Unable to filter write value.");
			return result;
		}
	}
	auto& sources = item->get_sources();
	if (!sources.empty())
	{
		auto new_trans = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
		pending_tasks_->emplace(new_trans, task);
		data.transaction_id = new_trans;
		result = variable_ptr->variable_write(std::move(data), ctx, item->get_sources());
		if (!result)
			pending_tasks_->erase(new_trans);
	}
	else
	{
		result = "No signal sources defined!";
	}
	return result;
}

rx_result variable_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	ctx.variables.push_variable(this);
	auto result = item->initialize_runtime(ctx);
	if(result)
		result = variable_ptr->initialize_variable(ctx);
	ctx.variables.pop_variable();
	ctx.structure.pop_item();
	return result;
}

rx_result variable_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	ctx.variables.push_variable(this);
	auto result = variable_ptr->deinitialize_variable(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	ctx.variables.pop_variable();
	return result;
}

rx_result variable_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	ctx.variables.push_variable(this);
	auto result = item->start_runtime(ctx);
	if (result)
		result = variable_ptr->start_variable(ctx);
	ctx.variables.pop_variable();
	ctx.structure.pop_item();
	if (result)
	{
		auto& mappers = item->get_mappers();
		if (!mappers.empty())
		{
			for (auto& one : mappers)
			{
				one.value_changed(rx_value(value));
			}
		}
	}
	return result;
}

rx_result variable_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	ctx.variables.push_variable(this);
	auto result = variable_ptr->stop_variable(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	ctx.variables.pop_variable();
	return result;
}

void variable_data::process_runtime (runtime_process_context* ctx)
{
	auto& sources = item->get_sources();
	if (sources.empty())
		return;
	auto prepared_value = variable_ptr->select_variable_input(ctx, sources);
	if (!prepared_value.convert_to(value.get_type()))
		return;

	rx_result result;
	auto& filters = item->get_filters();
	if (!filters.empty())
	{
		for (auto& filter : filters)
		{
			if (filter.is_input())
			{
				result = filter.filter_input(prepared_value, ctx);
				if (!result)
					break;
			}
		}
		if (!result)
		{
			result.register_error("Unable to filter read value.");
		}
	}
	if (value != prepared_value)
	{
		// value has changed
		value = prepared_value;
		// send subscription update
		ctx->variable_value_changed(this, prepared_value);
		// send update to mappers
		auto& mappers = item->get_mappers();
		if (!mappers.empty())
		{
			if (mappers.size() == 1)
			{
				mappers[0].value_changed(std::move(prepared_value));
			}
			else
			{
				for (auto& one : mappers)
				{
					one.value_changed(rx_value(prepared_value));
				}
			}
		}
	}
}

void variable_data::process_result (runtime_transaction_id_t id, rx_result&& result)
{
	auto it = pending_tasks_->find(id);
	if (it != pending_tasks_->end())
	{
		if(it->second)
			it->second->process_result(id, std::move(result));
		pending_tasks_->erase(it);
	}
}


// Class rx_platform::runtime::structure::struct_data 

string_type struct_data::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_data::struct_data()
{
}

struct_data::struct_data (runtime_item::smart_ptr&& rt, struct_runtime_ptr&& var)
	: item(std::move(rt))
	, struct_ptr(std::move(var))
{
}



void struct_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void struct_data::fill_data (const data::runtime_values_data& data, fill_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result struct_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = struct_ptr->initialize_struct(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result struct_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = struct_ptr->deinitialize_struct(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result struct_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = struct_ptr->start_struct(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result struct_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = struct_ptr->stop_struct(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}

void struct_data::process_runtime (runtime_process_context* ctx)
{
}


// Class rx_platform::runtime::structure::mapper_data 

string_type mapper_data::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_data::mapper_data()
      : my_variable_(nullptr),
        context_(nullptr)
{
}

mapper_data::mapper_data (runtime_item::smart_ptr&& rt, mapper_runtime_ptr&& var)
      : my_variable_(nullptr),
        context_(nullptr)
	, item(std::move(rt))
	, mapper_ptr(std::move(var))
{
}



void mapper_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void mapper_data::fill_data (const data::runtime_values_data& data, fill_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result mapper_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	my_variable_ = ctx.variables.get_current_variable();
	mapper_ptr->container_ = this;
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = mapper_ptr->initialize_mapper(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result mapper_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = mapper_ptr->deinitialize_mapper(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	mapper_ptr->container_ = nullptr;
	my_variable_ = nullptr;
	return result;
}

rx_result mapper_data::start_runtime (runtime::runtime_start_context& ctx)
{
	context_ = ctx.context;
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
	{
		auto value_type = ctx.structure.get_current_item().get_local_as<rx_value_t>("ValueType", RX_NULL_TYPE);
		if (value_type == RX_NULL_TYPE && my_variable_)
			value_type = my_variable_->value.get_type();
		mapper_ptr->value_type_ = value_type;
		result = mapper_ptr->start_mapper(ctx);
	}
	ctx.structure.pop_item();
	return result;
}

rx_result mapper_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = mapper_ptr->stop_mapper(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	context_ = nullptr;
	return result;
}

void mapper_data::process_update (values::rx_value&& value)
{
	if (mapper_ptr)
	{
		if (value.is_null() && my_variable_)
			value = my_variable_->get_value(context_);
		rx_result result;
		auto& filters = item->get_filters();
		if (!filters.empty())
		{
			for (auto& filter : filters)
			{
				if (filter.is_input())
				{
					result = filter.filter_input(value, context_);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter input value.");
			}
		}
		mapper_ptr->mapped_value_changed(std::move(value));
	}
}

void mapper_data::process_write (rx_simple_value&& val, runtime_transaction_id_t id)
{
	if (my_variable_)
	{
		rx_result result;
		auto& filters = item->get_filters();
		if (!filters.empty())
		{
			for (auto& filter : filters)
			{
				if (filter.is_output())
				{
					result = filter.filter_output(val, context_);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter output value.");
			}
		}
		write_data data;
		data.transaction_id = id;
		data.value = std::move(val);
		auto task = new mapper_write_task(this, id);
		result = my_variable_->write_value(std::move(data), task, context_);
	}
}

void mapper_data::mapper_write_pending (values::rx_simple_value&& value, runtime_transaction_id_t id)
{
	if (context_)
		context_->mapper_write_pending({ this, std::move(value), id });
}

rx_value mapper_data::get_mapped_value () const
{
	if (my_variable_)
	{
		return my_variable_->get_value(context_);
	}
	else
	{
		rx_value val;
		val.set_time(rx_time::now());
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
		return val;
	}
}

rx_result mapper_data::value_changed (rx_value&& val)
{
	if (context_)
		context_->mapper_update_pending({ this, std::move(val) });
	return true;
}

bool mapper_data::can_read () const
{
	return mapper_ptr ? mapper_ptr->supports_read() && io_.get_input() : false;
}

bool mapper_data::can_write () const
{
	return mapper_ptr ? mapper_ptr->supports_write() && io_.get_output() : false;
}

void mapper_data::process_write_result (rx_result&& result, runtime_transaction_id_t id)
{
	if (mapper_ptr)
	{
		mapper_ptr->mapper_result_received(std::move(result), id);
	}
}


// Class rx_platform::runtime::structure::source_data 

string_type source_data::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_data::source_data()
      : my_variable_(nullptr),
        context_(nullptr)
{
}

source_data::source_data (runtime_item::smart_ptr&& rt, source_runtime_ptr&& var)
      : my_variable_(nullptr),
        context_(nullptr)
	, item(std::move(rt))
	, source_ptr(std::move(var))
{
}



void source_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void source_data::fill_data (const data::runtime_values_data& data, fill_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result source_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	my_variable_ = ctx.variables.get_current_variable();
	source_ptr->container_ = this;
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = source_ptr->initialize_source(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result source_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = source_ptr->deinitialize_source(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	source_ptr->container_ = nullptr;
	my_variable_ = nullptr;
	return result;
}

rx_result source_data::start_runtime (runtime::runtime_start_context& ctx)
{
	context_ = ctx.context;
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
	{
		auto value_type = ctx.structure.get_current_item().get_local_as<rx_value_t>("ValueType", RX_NULL_TYPE);
		if (value_type == RX_NULL_TYPE && my_variable_)
			value_type = my_variable_->value.get_type();
		source_ptr->value_type_ = value_type;
		result = source_ptr->start_source(ctx);
	}
	ctx.structure.pop_item();
	return result;
}

rx_result source_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = source_ptr->stop_source(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	context_ = nullptr;
	return result;
}

rx_result source_data::write_value (structure::write_data&& data)
{
	if (!context_ || !source_ptr)
		return RX_ERROR_STOPPED;
	else if (!source_ptr->is_output())
		return RX_NOT_SUPPORTED;

	context_->source_write_pending({ this, std::move(data.value), data.transaction_id });
	return true;
}

void source_data::process_update (values::rx_value&& value)
{
	RX_ASSERT(source_ptr);
	if (source_ptr && context_ && my_variable_)
	{
		if (value != current_value_)
		{
			rx_result result;
			auto& filters = item->get_filters();
			if (!filters.empty())
			{
				for (auto& filter : filters)
				{
					if (filter.is_input())
					{
						result = filter.filter_input(value, context_);
						if (!result)
							break;
					}
				}
				if (!result)
				{
					result.register_error("Unable to filter input value.");
				}
			}
			current_value_ = value;
			context_->variable_pending(my_variable_);
		}
	}
}

void source_data::process_write (rx_simple_value&& val, runtime_transaction_id_t id)
{
	RX_ASSERT(source_ptr);
	if (source_ptr)
	{
		rx_result result;
		auto& filters = item->get_filters();
		if (!filters.empty())
		{
			for (auto& filter : filters)
			{
				if (filter.is_input())
				{
					result = filter.filter_output(val, context_);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter output value.");
			}
		}
		structure::write_data data;
		data.value = std::move(val);
		data.transaction_id = id;
		source_ptr->source_write(std::move(data), context_);
	}
}

void source_data::source_update_pending (values::rx_value&& value)
{
	if (context_)
		context_->source_update_pending({ this, std::move(value) });
}

bool source_data::is_input () const
{
	return source_ptr ? source_ptr->supports_input() && io_.get_input() : false;
}

bool source_data::is_output () const
{
	return source_ptr ? source_ptr->supports_output() && io_.get_output() : false;
}

const rx_value& source_data::get_current_value () const
{
	return current_value_;
}

void source_data::source_result_pending (rx_result&& result, runtime_transaction_id_t id)
{
	if (context_)
	{
		write_result_struct<structure::source_data> data;
		data.whose = this;
		data.transaction_id = id;
		data.result = std::move(result);
		context_->source_result_pending(std::move(data));
	}
}

void source_data::process_result (runtime_transaction_id_t id, rx_result&& result)
{
	my_variable_->process_result(id, std::move(result));
}


// Class rx_platform::runtime::structure::event_data 

string_type event_data::type_name = RX_CPP_EVENT_TYPE_NAME;

event_data::event_data()
{
}

event_data::event_data (runtime_item::smart_ptr&& rt, event_runtime_ptr&& var)
	: item(std::move(rt))
	, event_ptr(std::move(var))
{
}



void event_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void event_data::fill_data (const data::runtime_values_data& data, fill_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result event_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = event_ptr->initialize_event(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result event_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = event_ptr->deinitialize_event(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result event_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = event_ptr->start_event(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result event_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = event_ptr->stop_event(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}

void event_data::process_runtime (runtime_process_context* ctx)
{
}


// Class rx_platform::runtime::structure::filter_data 

string_type filter_data::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_data::filter_data()
{
}

filter_data::filter_data (runtime_item::smart_ptr&& rt, filter_runtime_ptr&& var)
	: item(std::move(rt))
	, filter_ptr(std::move(var))
{
}



void filter_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void filter_data::fill_data (const data::runtime_values_data& data, fill_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result filter_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = filter_ptr->initialize_filter(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result filter_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = filter_ptr->deinitialize_filter(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result filter_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = filter_ptr->start_filter(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result filter_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = filter_ptr->stop_filter(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}

void filter_data::process_runtime (runtime_process_context* ctx)
{
}

rx_result filter_data::filter_output (rx_simple_value& val, runtime_process_context* ctx)
{
	return filter_ptr ? filter_ptr->filter_output(val, ctx) : RX_ERROR_STOPPED;
}

rx_result filter_data::filter_input (rx_value& val, runtime_process_context* ctx)
{
	return filter_ptr ? filter_ptr->filter_input(val, ctx) : RX_ERROR_STOPPED;
}

bool filter_data::is_input () const
{
	return filter_ptr ? filter_ptr->supports_input() && io_.get_input() : false;
}

bool filter_data::is_output () const
{
	return filter_ptr ? filter_ptr->supports_output() && io_.get_output() : false;
}


// Class rx_platform::runtime::structure::const_value_data 

string_type const_value_data::type_name = RX_CONST_VALUE_TYPE_NAME;


rx_value const_value_data::get_value (runtime_process_context* ctx) const
{
	return ctx->adapt_value(value);
}

rx_simple_value const_value_data::simple_get_value () const
{
	return value;
}

rx_result const_value_data::set_value (rx_simple_value&& val)
{
	if (val.get_type() == value.get_type())
	{
		value = std::move(val);
		return true;
	}
	else
	{
		rx_simple_value temp(std::move(val));
		if (temp.convert_to(value.get_type()))
		{
			value = std::move(temp);
			return true;
		}
		else
		{
			return "Invalid conversion";
		}
	}
}


// Class rx_platform::runtime::structure::value_data 

string_type value_data::type_name = RX_VALUE_TYPE_NAME;


rx_value value_data::get_value (runtime_process_context* ctx) const
{
	return ctx->adapt_value(value);
}

void value_data::set_value (rx_simple_value&& val, const rx_time& time)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_timed_value::from_simple(std::move(val), time);
	}
}

void value_data::object_state_changed (runtime_process_context* ctx)
{
	if (ctx->get_mode_time() > value.get_time())
		value.set_time(ctx->get_mode_time());
}

rx_result value_data::write_value (write_data&& data)
{
	if (read_only && !data.is_internal())
		return "Access Denied!";
	if (data.value.convert_to(value.get_type()))
	{
		value = rx_timed_value::from_simple(std::move(data.value), rx_time::now());
		return true;
	}
	else
	{
		return "Conversion error!";
	}
}

rx_simple_value value_data::simple_get_value () const
{
	return value.to_simple();
}

rx_result value_data::simple_set_value (rx_simple_value&& val)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_timed_value::from_simple(std::move(val), rx_time::now());
		return true;
	}
	else
	{
		return RX_INVALID_CONVERSION;
	}
}


// Class rx_platform::runtime::structure::runtime_item 


const runtime_item* runtime_item::get_child_item (const string_type& path) const
{
	size_t idx = 0;
	// if we're empty?
	if (path.empty())
		return this;
	const char* c_path = path.c_str();
	const runtime_item* ret = this;
	size_t end = path.size();
	do
	{
		ret = ret->faster_get_child_item(c_path, idx);
		idx++;
	} while (ret && idx< end);

	return ret;
}


// Class rx_platform::runtime::structure::fill_context 

fill_context::fill_context (runtime_process_context* ctx)
	: now(rx_time::now())
{
}



// Class rx_platform::runtime::structure::write_data 


// Class rx_platform::runtime::structure::indirect_value_data 

string_type indirect_value_data::type_name = RX_INDIRECT_VALUE_TYPE_NAME;


rx_value indirect_value_data::get_value (runtime_process_context* ctx) const
{
	return ctx->adapt_value(value);
}

void indirect_value_data::set_value (rx_simple_value&& val, const rx_time& time)
{
	if (val.convert_to(value.get_type()))
	{
		default_value_ = val;
		value = rx_timed_value::from_simple(std::move(val), time);
	}
}

void indirect_value_data::object_state_changed (runtime_process_context* ctx)
{
	if (ctx->get_mode_time() > value.get_time())
		value.set_time(ctx->get_mode_time());
}

rx_result indirect_value_data::write_value (write_data&& data, runtime_process_context* ctx)
{
	if (!data.value.convert_to(value.get_type()))
		return "Invalid conversion";
	return true;
}


// Class rx_platform::runtime::structure::mapper_write_task 

mapper_write_task::mapper_write_task (mapper_data* my_mapper, runtime_transaction_id_t trans_id)
      : my_mapper_(my_mapper),
        transaction_id_(trans_id)
{
}



void mapper_write_task::process_result (runtime_transaction_id_t id, rx_result&& result)
{
	my_mapper_->process_write_result(std::move(result), transaction_id_);
}


// Class rx_platform::runtime::structure::variable_write_task 

variable_write_task::~variable_write_task()
{
}



} // namespace structure
} // namespace runtime
} // namespace rx_platform

