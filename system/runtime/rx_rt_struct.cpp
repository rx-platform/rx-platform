

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


// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"

#include "rx_blocks.h"
#include "rx_objbase.h"
#include "rx_relations.h"


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


// Parameterized Class rx_platform::runtime::structure::runtime_data 


template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::collect_data (data::runtime_values_data& data) const
{
	for (const auto& one : items)
	{
		switch (one.index&rt_type_mask)
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
		{// simple value
			auto& result = data.add_child(one.name);
			variables.collection[(one.index >> rt_type_shift)].collect_data(result);
		}
		break;
		case rt_struct_index_type:
		{// simple value
			auto& result = data.add_child(one.name);
			structs.collection[(one.index >> rt_type_shift)].collect_data(result);
		}
		break;
		case rt_source_index_type:
		{// simple value
			auto& result = data.add_child(one.name);
			sources.collection[(one.index >> rt_type_shift)].collect_data(result);
		}
		break;
		case rt_mapper_index_type:
		{// simple value
			auto& result = data.add_child(one.name);
			mappers.collection[(one.index >> rt_type_shift)].collect_data(result);
		}
		break;
		case rt_filter_index_type:
		{// simple value
			auto& result = data.add_child(one.name);
			filters.collection[(one.index >> rt_type_shift)].collect_data(result);
		}
		break;
		case rt_event_index_type:
		{// simple value
			auto& result = data.add_child(one.name);
			events.collection[(one.index >> rt_type_shift)].collect_data(result);
		}
		break;
		default:
			RX_ASSERT(false);
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::fill_data (const data::runtime_values_data& data, init_context& ctx)
{
	for (auto one : items)
	{
		switch (one.index&rt_type_mask)
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
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_value (const hosting_object_data& state, const string_type& path, rx_value& val) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto& sub_item = get_sub_item(mine);
		if (sub_item)
			return sub_item->get_value(state, bellow, val);
	}
	else// its' ours
	{
		auto idx = internal_get_index(path);
		if (idx && is_value_index(idx))
		{
			switch (idx&rt_type_mask)
			{
			case rt_const_index_type:
				val = const_values[idx >> rt_type_shift].get_value(state);
				return true;
			case rt_value_index_type:
				val = values[idx >> rt_type_shift].get_value(state);
				return true;
			case rt_variable_index_type:
				val = variables.collection[idx >> rt_type_shift].get_value(state);
				return true;
			}
		}
	}
	return mine + " not found!";
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::object_state_changed (const hosting_object_data& state)
{
	if (has_structs())
	{
		for (auto& one : structs.collection)
			one.item->object_state_changed(state);
	}
	if (has_variables())
	{
		for (auto& one : variables.collection)
			one.item->object_state_changed(state);
	}
	for (auto& one : values)
		one.object_state_changed(state);
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
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::write_value (const string_type& path, rx_simple_value&& val, const write_context& ctx)
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto& sub_item = get_sub_item(mine);
		if (sub_item)
			return sub_item->write_value(bellow, std::move(val), ctx);
	}
	else// its' ours
	{
		auto idx = internal_get_index(path);
		if (idx && is_value_index(idx))
		{
			switch (idx&rt_type_mask)
			{
			case rt_const_index_type:
				return "Not valid for this";
				return true;
			case rt_value_index_type:
				return values[idx >> rt_type_shift].write_value(std::move(val), ctx);
			case rt_variable_index_type:
				return variables.collection[idx >> rt_type_shift].write_value(std::move(val), ctx);
			}
		}
	}
	return path + " not found!";
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
			switch (idx&rt_type_mask)
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
const runtime_item::smart_ptr& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_sub_item (const string_type& path) const
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
runtime_item::smart_ptr& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_sub_item (const string_type& path)
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
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::browse_items (const string_type& filter, const string_type& current_path, std::vector<runtime_item_attribute>& items) const
{
	for (const auto& one : this->items)
	{
		runtime_item_attribute one_attr;
		switch (one.index&rt_type_mask)
		{
		case rt_value_index_type:
			one_attr.name = one.name;
			one_attr.type = rx_attribute_type::value_attribute_type;
			break;
		case rt_const_index_type:
			one_attr.name = one.name;
			one_attr.type = rx_attribute_type::const_attribute_type;
			break;
		case rt_variable_index_type:
			one_attr.name = one.name;
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

variable_data::variable_data (runtime_item::smart_ptr&& rt, variable_runtime_ptr&& var)
	: item(std::move(rt))
	, variable_ptr(std::move(var))
{
}



void variable_data::collect_data (data::runtime_values_data& data) const
{
	data.add_value(RX_DEFAULT_VARIABLE_NAME, value.to_simple());
	item->collect_data(data);
}

void variable_data::fill_data (const data::runtime_values_data& data, init_context& ctx)
{
	auto it = data.values.find(RX_DEFAULT_VARIABLE_NAME);
	if (it != data.values.end())
	{
		value = rx_value::from_simple(it->second.value, ctx.now);
	}
	item->fill_data(data, ctx);
}

rx_value variable_data::get_value (const hosting_object_data& state) const
{
	return state.adapt_value(value);
}

void variable_data::set_value (rx_value&& value)
{
	this->value = std::move(value);
}

void variable_data::set_value (rx_simple_value&& val, const init_context& ctx)
{
	rx_simple_value temp(val);
	if (temp.convert_to(value.get_type()))
	{
		value = rx_value::from_simple(std::move(temp), ctx.now);
	}
}

rx_result variable_data::write_value (rx_simple_value&& val, const write_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result variable_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	ctx.variables.push_variable(variable_ptr);
	auto result = item->initialize_runtime(ctx);
	if(result)
		result = variable_ptr->initialize_runtime(ctx);
	ctx.variables.pop_variable();
	ctx.structure.pop_item();
	return result;
}

rx_result variable_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	ctx.variables.push_variable(variable_ptr);
	auto result = variable_ptr->deinitialize_runtime(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	ctx.variables.pop_variable();
	return result;
}

rx_result variable_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	ctx.variables.push_variable(variable_ptr);
	auto result = item->start_runtime(ctx);
	if (result)
		result = variable_ptr->start_runtime(ctx);
	ctx.variables.pop_variable();
	ctx.structure.pop_item();
	return result;
}

rx_result variable_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	ctx.variables.push_variable(variable_ptr);
	auto result = variable_ptr->stop_runtime(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	ctx.variables.pop_variable();
	return result;
}


// Class rx_platform::runtime::structure::struct_data 

string_type struct_data::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_data::struct_data (runtime_item::smart_ptr&& rt, struct_runtime_ptr&& var)
	: item(std::move(rt))
	, struct_ptr(std::move(var))
{
}



void struct_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void struct_data::fill_data (const data::runtime_values_data& data, init_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result struct_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = struct_ptr->initialize_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result struct_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = struct_ptr->deinitialize_runtime(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result struct_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = struct_ptr->start_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result struct_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = struct_ptr->stop_runtime(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}


// Class rx_platform::runtime::structure::mapper_data 

string_type mapper_data::type_name = RX_CPP_MAPPER_TYPE_NAME;


void mapper_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void mapper_data::fill_data (const data::runtime_values_data& data, init_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result mapper_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = mapper_ptr->initialize_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result mapper_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = mapper_ptr->deinitialize_runtime(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result mapper_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = mapper_ptr->start_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result mapper_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = mapper_ptr->stop_runtime(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}


// Class rx_platform::runtime::structure::source_data 

string_type source_data::type_name = RX_CPP_SOURCE_TYPE_NAME;


void source_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void source_data::fill_data (const data::runtime_values_data& data, init_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result source_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = source_ptr->initialize_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result source_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = source_ptr->deinitialize_runtime(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result source_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = source_ptr->start_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result source_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = source_ptr->stop_runtime(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}


// Class rx_platform::runtime::structure::event_data 

string_type event_data::type_name = RX_CPP_EVENT_TYPE_NAME;


void event_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void event_data::fill_data (const data::runtime_values_data& data, init_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result event_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = event_ptr->initialize_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result event_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = event_ptr->deinitialize_runtime(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result event_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = event_ptr->start_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result event_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = event_ptr->stop_runtime(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}


// Class rx_platform::runtime::structure::filter_data 

string_type filter_data::type_name = RX_CPP_FILTER_TYPE_NAME;


void filter_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void filter_data::fill_data (const data::runtime_values_data& data, init_context& ctx)
{
	item->fill_data(data, ctx);
}

rx_result filter_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = filter_ptr->initialize_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result filter_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = filter_ptr->deinitialize_runtime(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result filter_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = filter_ptr->start_runtime(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result filter_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = filter_ptr->stop_runtime(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}


// Class rx_platform::runtime::structure::const_value_data 

string_type const_value_data::type_name = RX_CONST_VALUE_TYPE_NAME;


rx_value const_value_data::get_value (const hosting_object_data& state) const
{
	rx_value ret;
	value.get_value(ret, state.time, state.mode);
	return ret;
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


rx_value value_data::get_value (const hosting_object_data& state) const
{
	rx_value ret;
	value.get_value(ret, state.time, state.mode);
	return ret;
}

void value_data::set_value (rx_simple_value&& val, const rx_time& time)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_timed_value::from_simple(std::move(val), time);
	}
}

void value_data::object_state_changed (const hosting_object_data& state)
{
	if (state.time > value.get_time())
		value.set_time(state.time);
}

rx_result value_data::write_value (rx_simple_value&& val, const write_context& ctx)
{
	if (read_only && !ctx.is_internal())
		return "Access Denied!";
	if (val.convert_to(value.get_type()))
	{
		value = rx_timed_value::from_simple(std::move(val), ctx.now);
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


// Class rx_platform::runtime::structure::hosting_object_data 

hosting_object_data::hosting_object_data (const std::vector<relation_runtime_ptr>& relations, algorithms::runtime_process_context* ctx)
      : context(ctx),
        relations_(relations)
{
}



rx_value hosting_object_data::adapt_value (const rx_value& from) const
{
	auto ret = rx_value(from);
	// TODO
	// Adapt value with hosting object data
	// currently doing nothing!!!
	return ret;
}

relation_runtime_ptr hosting_object_data::get_relation (const string_type& path) const
{
	for (auto& one : relations_)
	{
		if (one->name == path)
		{
			return one;
		}
	}
	return relation_runtime_ptr::null_ptr;
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


// Class rx_platform::runtime::structure::init_context 

init_context::init_context (const std::vector<relation_runtime_ptr>& relations, algorithms::runtime_process_context* ctx)
{
}



// Class rx_platform::runtime::structure::write_context 


write_context write_context::create_write_context (const structure::hosting_object_data& state, bool internal_write)
{
	write_context ret;
	ret.now = rx_time::now();
	ret.internal_ = internal_write;
	return ret;
}


} // namespace structure
} // namespace runtime
} // namespace rx_platform

