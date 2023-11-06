

/****************************************************************************
*
*  system\runtime\rx_rt_item_types.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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




// rx_rt_item_types
#include "system/runtime/rx_rt_item_types.h"

#include "lib/rx_ser_json.h"
#include "rx_rt_struct.h"
#include "rx_blocks.h"
#include "rx_objbase.h"
#include "rx_rt_item_types.h"


namespace rx_platform {

namespace runtime {

namespace structure {

namespace
{

runtime_filters_type g_empty_filters;
runtime_sources_type g_empty_sources;
runtime_mappers_type g_empty_mappers;
runtime_events_type g_empty_events;
runtime_structs_type g_empty_structs;
runtime_variables_type g_empty_variables;

}



void collect_data_const(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<const_value_data>& item);
void collect_data_value(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper <value_data>& item);
void collect_data_variable(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<variable_data>& item);
template<typename T>
void collect_data_arrayed(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<T>& item);
template<typename T>
void collect_data_plain(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const T& item);

void fill_data_const(const string_type& name, const data::runtime_values_data& data, array_wrapper<const_value_data>& item);
void fill_data_value(const string_type& name, const data::runtime_values_data& data, array_wrapper<value_data>& item);
void fill_data_variable(const string_type& name, const data::runtime_values_data& data, array_wrapper<variable_data>& item);
template<typename T>
void fill_data_arrayed(const string_type& name, const data::runtime_values_data& data, array_wrapper<T>& item);
template<typename T>
void fill_data_plain(const string_type& name, const data::runtime_values_data& data, T& item);

void collect_value_const(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<const_value_data>& item);
void collect_value_value(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper <value_data>& item);
void collect_value_variable(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper <variable_data>& item);
template<typename T>
void collect_value_arrayed(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<T>& item);
template<typename T>
void collect_value_plain(std::vector<values::rx_simple_value>& data, runtime_value_type type, const T& item);

string_view_type extract_index(string_view_type name, int& idx);
members_index_type internal_get_index(string_view_type name, int& idx, const const_size_vector<index_data>& items);
members_index_type internal_split_get_index(string_view_type path, string_view_type& mine, string_view_type& bellow, int& idx, const const_size_vector<index_data>& items);
bool is_value_index(members_index_type idx);
bool is_complex_index(members_index_type idx);

rx_result get_value_const(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<const_value_data>& item);
rx_result get_value_value(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<value_data>& item);
rx_result get_value_variable(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<variable_data>& item);
template<typename T>
rx_result get_value_arrayed(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<T>& item);
template<typename T>
rx_result get_value_plain(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const T& item);


rx_result get_local_value_const(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<const_value_data>& item);
rx_result get_local_value_value(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<value_data>& item);
rx_result get_local_value_variable(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<variable_data>& item);
template<typename T>
rx_result get_local_value_arrayed(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<T>& item);
template<typename T>
rx_result get_local_value_plain(int array_idx, string_view_type bellow, rx_simple_value& val, const T& item);


rx_result get_value_ref_const(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<const_value_data>& item);
rx_result get_value_ref_value(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<value_data>& item);
rx_result get_value_ref_variable(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<variable_data>& item);
template<typename T>
rx_result get_value_ref_arrayed(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<T>& item);
template<typename T>
rx_result get_value_ref_plain(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, T& item);

template<typename T>
void object_state_changed_arrayed(runtime_process_context* ctx, const_size_vector<array_wrapper<T> >& items);
template<typename T>
void object_state_changed_plain(runtime_process_context* ctx, const_size_vector<T>& items);

template<typename T>
rx_result initialize_runtime_arrayed(const string_type name, runtime::runtime_init_context& ctx, array_wrapper<T>& item);
template<typename T>
rx_result initialize_runtime_plain(const string_type name, runtime::runtime_init_context& ctx, T& item);

template<typename T>
rx_result deinitialize_runtime_arrayed(runtime::runtime_deinit_context& ctx, array_wrapper<T>& item);
template<typename T>
rx_result deinitialize_runtime_plain(runtime::runtime_deinit_context& ctx, T& item);

template<typename T>
rx_result start_runtime_arrayed(const string_type name, runtime::runtime_start_context& ctx, array_wrapper<T>& item);
template<typename T>
rx_result start_runtime_plain(const string_type name, runtime::runtime_start_context& ctx, T& item);

template<typename T>
rx_result stop_runtime_arrayed(runtime::runtime_stop_context& ctx, array_wrapper<T>& item);
template<typename T>
rx_result stop_runtime_plain(runtime::runtime_stop_context& ctx, T& item);

rx_result browse_items_const(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<const_value_data>& item);
rx_result browse_items_value(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<value_data>& item);
rx_result browse_items_variable(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<variable_data>& item);
template<typename T>
rx_result browse_items_arrayed(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<T>& item);
rx_result browse_items_blocks(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<value_block_data>& item);
template<typename T>
rx_result browse_items_plain(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const T& item);



const runtime_item* get_child_item_variable(int array_idx, string_view_type path, const array_wrapper<variable_data>& item);
template<typename T>
const runtime_item* get_child_item_arrayed(int array_idx, string_view_type path, const array_wrapper<T>& item);
template<typename T>
const runtime_item* get_child_item_plain(int array_idx, string_view_type path, const T& item);


// Parameterized Class rx_platform::runtime::structure::runtime_data 

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::~runtime_data()
{
}



template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	for (const auto& one : items)
	{
		if (!one.name.empty() && one.name[0] == '_' && type == runtime_value_type::simple_runtime_value)
			continue;
		uint_fast16_t index = one.index & rt_type_mask;
		switch (index)
		{
		case rt_const_index_type:
			// const value
			collect_data_const(one.name, data, type, const_values[(one.index >> rt_type_shift)]);
			break;
		case rt_value_index_type:
			// simple value
			collect_data_value(one.name, data, type, values[(one.index >> rt_type_shift)]);
			break;
		default:
			{
				if constexpr (has_variables())
				{
					if (index == rt_variable_index_type)
					{
						collect_data_variable(one.name, data, type, variables.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_structs())
				{
					if (index == rt_struct_index_type)
					{
						collect_data_arrayed(one.name, data, type, structs.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_sources())
				{
					if (index == rt_source_index_type)
					{
						collect_data_plain(one.name, data, type, sources.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_mappers())
				{
					if (index == rt_mapper_index_type)
					{
						collect_data_plain(one.name, data, type, mappers.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_filters())
				{
					if (index == rt_filter_index_type)
					{
						collect_data_plain(one.name, data, type, filters.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_events())
				{
					if (index == rt_event_index_type)
					{
						collect_data_plain(one.name, data, type, events.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_block_data())
				{
					if (index == rt_value_data_index_type)
					{
						collect_data_arrayed(one.name, data, type, blocks.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_variable_blocks_data())
				{
					if (index == rt_variable_data_index_type)
					{
						collect_data_arrayed(one.name, data, type, variable_blocks.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
			}
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::fill_data (const data::runtime_values_data& data)
{
	for (auto& one : items)
	{
		uint_fast16_t index = one.index & rt_type_mask;
		switch (index)
		{
		case rt_const_index_type:
			{// const value
				fill_data_const(one.name, data, const_values[(one.index >> rt_type_shift)]);
			}
			break;
		case rt_value_index_type:
			{// value
				fill_data_value(one.name, data, values[(one.index >> rt_type_shift)]);
			}
			break;
		default:
			{
				if constexpr (has_variables())
				{
					if (index == rt_variable_index_type)
					{
						fill_data_variable(one.name, data, variables.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_structs())
				{
					if (index == rt_struct_index_type)
					{
						fill_data_arrayed<struct_data>(one.name, data, structs.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_sources())
				{
					if (index == rt_source_index_type)
					{
						fill_data_plain<source_data>(one.name, data, sources.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_mappers())
				{
					if (index == rt_mapper_index_type)
					{
						fill_data_plain<mapper_data>(one.name, data, mappers.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_filters())
				{
					if (index == rt_filter_index_type)
					{
						fill_data_plain<filter_data>(one.name, data, filters.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_events())
				{
					if (index == rt_event_index_type)
					{
						fill_data_plain<event_data>(one.name, data, events.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_block_data())
				{
					if (index == rt_value_data_index_type)
					{
						fill_data_arrayed<value_block_data>(one.name, data, blocks.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_variable_blocks_data())
				{
					if (index == rt_variable_data_index_type)
					{
						fill_data_arrayed<variable_block_data>(one.name, data, variable_blocks.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
			}
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	std::vector<rx_simple_value> my_values;
	for (const auto& one : items)
	{
		if (!one.name.empty() && one.name[0] == '_' && type == runtime_value_type::simple_runtime_value)
			continue;

		uint_fast16_t index = one.index & rt_type_mask;
		switch (index)
		{
		case rt_const_index_type:
			// const value
			collect_value_const(my_values, type, const_values[(one.index >> rt_type_shift)]);
			break;
		case rt_value_index_type:
			// simple value
			collect_value_value(my_values, type, values[(one.index >> rt_type_shift)]);
			break;
		default:
			{
				if constexpr (has_variables())
				{
					if (index == rt_variable_index_type)
					{
						collect_value_variable(my_values, type, variables.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}				
				if constexpr (has_structs())
				{
					if (index == rt_struct_index_type)
					{
						collect_value_arrayed(my_values, type, structs.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				
				if constexpr (has_sources())
				{
					if (index == rt_source_index_type)
					{
						collect_value_plain(my_values, type, sources.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_mappers())
				{
					if (index == rt_mapper_index_type)
					{
						collect_value_plain(my_values, type, mappers.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_filters())
				{
					if (index == rt_filter_index_type)
					{
						collect_value_plain(my_values, type, filters.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_events())
				{
					if (index == rt_event_index_type)
					{
						collect_value_plain(my_values, type, events.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_block_data())
				{
					if (index == rt_value_data_index_type)
					{
						collect_value_arrayed(my_values, type, blocks.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
				if constexpr (has_variable_blocks_data())
				{
					if (index == rt_variable_data_index_type)
					{
						collect_value_arrayed(my_values, type, variable_blocks.collection[(one.index >> rt_type_shift)]);
						continue;
					}
				}
			}
		}
	}
	if (!my_values.empty())
		data.assign_static(my_values);
	return true;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::fill_value (const values::rx_simple_value& data)
{
	return RX_NOT_SUPPORTED;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{// our value
		rx_simple_value temp_val;
		rx_result result = collect_value(temp_val, runtime_value_type::simple_runtime_value);
		if (!result)
		{
			return result;
		}
		val = rx_value(std::move(temp_val), rx_time::now());
		val.set_good_locally();
		val = ctx->adapt_value(val);
		return true;
	}
	else
	{
		string_view_type mine;
		string_view_type bellow;
		int array_idx = -1;
		auto idx = internal_split_get_index(path, mine, bellow, array_idx, items);
		if (idx)
		{
			uint_fast16_t index = idx & rt_type_mask;
			switch (index)
			{
			case rt_const_index_type:
				return get_value_const(array_idx, bellow, val, ctx, const_values[(idx >> rt_type_shift)]);
			case rt_value_index_type:
				return get_value_value(array_idx, bellow, val, ctx, values[(idx >> rt_type_shift)]);
			default:
				if constexpr (has_variables())
				{
					if(index == rt_variable_index_type)
						return get_value_variable(array_idx, bellow, val, ctx, variables.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_structs())
				{
					if (index == rt_struct_index_type)
						return get_value_arrayed<struct_data>(array_idx, bellow, val, ctx, structs.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_sources())
				{
					if (index == rt_source_index_type)
						return get_value_plain<source_data>(array_idx, bellow, val, ctx, sources.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_mappers())
				{
					if (index == rt_mapper_index_type)
						return get_value_plain<mapper_data>(array_idx, bellow, val, ctx, mappers.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_filters())
				{
					if (index == rt_filter_index_type)
						return get_value_plain<filter_data>(array_idx, bellow, val, ctx, filters.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_events())
				{
					if (index == rt_event_index_type)
						return get_value_plain<event_data>(array_idx, bellow, val, ctx, events.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_block_data())
				{
					if (index == rt_value_data_index_type)
						return get_value_arrayed<value_block_data>(array_idx, bellow, val, ctx, blocks.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_variable_blocks_data())
				{
					if (index == rt_variable_data_index_type)
						return get_value_arrayed<variable_block_data>(array_idx, bellow, val, ctx, variable_blocks.collection[(idx >> rt_type_shift)]);
				}
			}
		}
	}
	return RX_INVALID_PATH;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::object_state_changed (runtime_process_context* ctx)
{
	if constexpr (has_structs())
	{
		object_state_changed_arrayed(ctx, structs.collection);
	}
	if constexpr (has_variables())
	{
		object_state_changed_arrayed(ctx, variables.collection);
	}
	if constexpr (has_sources())
	{
		object_state_changed_plain(ctx, sources.collection);
	}
	if constexpr (has_mappers())
	{
		object_state_changed_plain(ctx, mappers.collection);
	}
	if constexpr (has_events())
	{
		object_state_changed_plain(ctx, events.collection);
	}
	if constexpr (has_filters())
	{
		object_state_changed_plain(ctx, filters.collection);
	}
	
	object_state_changed_arrayed(ctx, values);

	if constexpr (has_block_data())
	{
		object_state_changed_arrayed(ctx, blocks.collection);
	}
	if constexpr (has_variable_blocks_data())
	{
		object_state_changed_arrayed(ctx, variable_blocks.collection);
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::initialize_runtime (runtime::runtime_init_context& ctx)
{

	rx_result ret(true);
	if constexpr (has_mappers())
	{
		for (auto& one : mappers.collection)
		{
			ctx.mappers.push_mapper(one.mapper_id, &one);
		}
	}
	if constexpr (has_sources())
	{
		for (auto& one : sources.collection)
		{
			ctx.sources.push_source(one.source_id, &one);
		}
	}
	for (auto& one : items)
	{
		if ((one.index & rt_type_mask) == rt_const_index_type || (one.index & rt_type_mask) == rt_value_index_type)
			continue;
		if constexpr (has_variables())
		{
			if((one.index & rt_type_mask) == rt_variable_index_type)
			{
				ret = initialize_runtime_arrayed(one.name, ctx, variables.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_structs())
		{
			if ((one.index & rt_type_mask) == rt_struct_index_type)
			{
				ret = initialize_runtime_arrayed(one.name, ctx, structs.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_sources())
		{
			if ((one.index & rt_type_mask) == rt_source_index_type)
			{
				ret = initialize_runtime_plain(one.name, ctx, sources.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_mappers())
		{
			if ((one.index & rt_type_mask) == rt_mapper_index_type)
			{
				ret = initialize_runtime_plain(one.name, ctx, mappers.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_filters())
		{
			if ((one.index & rt_type_mask) == rt_filter_index_type)
			{
				ret = initialize_runtime_plain(one.name, ctx, filters.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_events())
		{
			if ((one.index & rt_type_mask) == rt_event_index_type)
			{
				ret = initialize_runtime_plain(one.name, ctx, events.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}

		if constexpr (has_block_data())
		{
			if ((one.index & rt_type_mask) == rt_value_data_index_type)
			{
				ret = initialize_runtime_arrayed(one.name, ctx, blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_variable_blocks_data())
		{
			if ((one.index & rt_type_mask) == rt_variable_data_index_type)
			{
				ret = initialize_runtime_arrayed(one.name, ctx, variable_blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
	}
	if constexpr (has_mappers())
	{
		for (auto& one : mappers.collection)
		{
			ctx.mappers.pop_mapper(one.mapper_id);
		}
	}
	if constexpr (has_sources())
	{
		for (auto& one : sources.collection)
		{
			ctx.sources.pop_source(one.source_id);
		}
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	rx_result ret(true);
	for (auto& one : items)
	{
		if ((one.index & rt_type_mask) == rt_const_index_type || (one.index & rt_type_mask) == rt_value_index_type)
			continue;
		if constexpr (has_variables())
		{
			if ((one.index & rt_type_mask) == rt_variable_index_type)
			{
				ret = deinitialize_runtime_arrayed(ctx, variables.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_structs())
		{
			if ((one.index & rt_type_mask) == rt_struct_index_type)
			{
				ret = deinitialize_runtime_arrayed(ctx, structs.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_sources())
		{
			if ((one.index & rt_type_mask) == rt_source_index_type)
			{
				ret = deinitialize_runtime_plain(ctx, sources.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_mappers())
		{
			if ((one.index & rt_type_mask) == rt_mapper_index_type)
			{
				ret = deinitialize_runtime_plain(ctx, mappers.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_filters())
		{
			if ((one.index & rt_type_mask) == rt_filter_index_type)
			{
				ret = deinitialize_runtime_plain(ctx, filters.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_events())
		{
			if ((one.index & rt_type_mask) == rt_event_index_type)
			{
				ret = deinitialize_runtime_plain(ctx, events.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}

		if constexpr (has_block_data())
		{
			if ((one.index & rt_type_mask) == rt_value_data_index_type)
			{
				ret = deinitialize_runtime_arrayed(ctx, blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_variable_blocks_data())
		{
			if ((one.index & rt_type_mask) == rt_variable_data_index_type)
			{
				ret = deinitialize_runtime_arrayed(ctx, variable_blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::start_runtime (runtime::runtime_start_context& ctx)
{
	rx_result ret(true);
	for (auto& one : items)
	{
		if ((one.index & rt_type_mask) == rt_const_index_type || (one.index & rt_type_mask) == rt_value_index_type)
			continue;
		if constexpr (has_variables())
		{
			if ((one.index & rt_type_mask) == rt_variable_index_type)
			{
				ret = start_runtime_arrayed(one.name, ctx, variables.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_structs())
		{
			if ((one.index & rt_type_mask) == rt_struct_index_type)
			{
				ret = start_runtime_arrayed(one.name, ctx, structs.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_sources())
		{
			if ((one.index & rt_type_mask) == rt_source_index_type)
			{
				ret = start_runtime_plain(one.name, ctx, sources.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_mappers())
		{
			if ((one.index & rt_type_mask) == rt_mapper_index_type)
			{
				ret = start_runtime_plain(one.name, ctx, mappers.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_filters())
		{
			if ((one.index & rt_type_mask) == rt_filter_index_type)
			{
				ret = start_runtime_plain(one.name, ctx, filters.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_events())
		{
			if ((one.index & rt_type_mask) == rt_event_index_type)
			{
				ret = start_runtime_plain(one.name, ctx, events.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}

		if constexpr (has_block_data())
		{
			if ((one.index & rt_type_mask) == rt_value_data_index_type)
			{
				ret = start_runtime_arrayed(one.name, ctx, blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_variable_blocks_data())
		{
			if ((one.index & rt_type_mask) == rt_variable_data_index_type)
			{
				ret = start_runtime_arrayed(one.name, ctx, variable_blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::stop_runtime (runtime::runtime_stop_context& ctx)
{
	rx_result ret(true);
	for (auto& one : items)
	{
		if ((one.index & rt_type_mask) == rt_const_index_type || (one.index & rt_type_mask) == rt_value_index_type)
			continue;
		if constexpr (has_variables())
		{
			if ((one.index & rt_type_mask) == rt_variable_index_type)
			{
				ret = stop_runtime_arrayed(ctx, variables.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_structs())
		{
			if ((one.index & rt_type_mask) == rt_struct_index_type)
			{
				ret = stop_runtime_arrayed(ctx, structs.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_sources())
		{
			if ((one.index & rt_type_mask) == rt_source_index_type)
			{
				ret = stop_runtime_plain(ctx, sources.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_mappers())
		{
			if ((one.index & rt_type_mask) == rt_mapper_index_type)
			{
				ret = stop_runtime_plain(ctx, mappers.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_filters())
		{
			if ((one.index & rt_type_mask) == rt_filter_index_type)
			{
				ret = stop_runtime_plain(ctx, filters.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_events())
		{
			if ((one.index & rt_type_mask) == rt_event_index_type)
			{
				ret = stop_runtime_plain(ctx, events.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}

		if constexpr (has_block_data())
		{
			if ((one.index & rt_type_mask) == rt_value_data_index_type)
			{
				ret = stop_runtime_arrayed(ctx, blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
		if constexpr (has_variable_blocks_data())
		{
			if ((one.index & rt_type_mask) == rt_variable_data_index_type)
			{
				ret = stop_runtime_arrayed(ctx, variable_blocks.collection[one.index >> rt_type_shift]);
				if (!ret)
					return ret;
				continue;
			}
		}
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_value_ref (string_view_type path, rt_value_ref& ref, bool is_var)
{


	//////////////////////////////////////////////////////////////////
	//
	if (path.empty())
	{// our value
		return RX_INVALID_PATH;
	}
	else
	{
		string_view_type mine;
		string_view_type bellow;
		int array_idx = -1;
		auto idx = internal_split_get_index(path, mine, bellow, array_idx, items);
		if (idx)
		{
			uint_fast16_t index = idx & rt_type_mask;
			switch (index)
			{
			case rt_const_index_type:
				return get_value_ref_const(array_idx, bellow, path, ref, is_var, const_values[(idx >> rt_type_shift)]);
			case rt_value_index_type:
				return get_value_ref_value(array_idx, bellow, path, ref, is_var, values[(idx >> rt_type_shift)]);
			default:
				if constexpr (has_variables())
				{
					if (index == rt_variable_index_type)
						return get_value_ref_variable(array_idx, bellow, path, ref, is_var, variables.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_structs())
				{
					if (index == rt_struct_index_type)
						return get_value_ref_arrayed<struct_data>(array_idx, bellow, path, ref, is_var, structs.collection[(idx >> rt_type_shift)]);
				}				
				if constexpr (has_sources())
				{
					if (index == rt_source_index_type)
						return get_value_ref_plain<source_data>(array_idx, bellow, path, ref, is_var, sources.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_mappers())
				{
					if (index == rt_mapper_index_type)
						return get_value_ref_plain<mapper_data>(array_idx, bellow, path, ref, is_var, mappers.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_filters())
				{
					if (index == rt_filter_index_type)
						return get_value_ref_plain<filter_data>(array_idx, bellow, path, ref, is_var, filters.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_events())
				{
					if (index == rt_event_index_type)
						return get_value_ref_plain<event_data>(array_idx, bellow, path, ref, is_var, events.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_block_data())
				{
					if (index == rt_value_data_index_type)
						return get_value_ref_arrayed<value_block_data>(array_idx, bellow, path, ref, is_var, blocks.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_variable_blocks_data())
				{
					if (index == rt_variable_data_index_type)
						return get_value_ref_arrayed<variable_block_data>(array_idx, bellow, path, ref, is_var, variable_blocks.collection[(idx >> rt_type_shift)]);
				}
			}
		}
	}
	return RX_INVALID_PATH;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{



	if (path.empty())
	{
		for (const auto& one : this->items)
		{
			switch ((one.index & rt_type_mask))
			{
			case rt_const_index_type:
				browse_items_const(true, one.name, -1, prefix, path, filter, items, ctx, const_values[(one.index >> rt_type_shift)]);
				break;
			case rt_value_index_type:
				browse_items_value(true, one.name, -1, prefix, path, filter, items, ctx, values[(one.index >> rt_type_shift)]);
				break;
			default:
				{
					if constexpr (has_variables())
					{
						if ((one.index & rt_type_mask) == rt_variable_index_type)
						{
							browse_items_variable(true, one.name, -1, prefix, path, filter, items, ctx, variables.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
					if constexpr (has_structs())
					{
						if ((one.index & rt_type_mask) == rt_struct_index_type)
						{
							browse_items_arrayed<struct_data>(true, one.name, -1, prefix, path, filter, items, ctx, structs.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
					if constexpr (has_sources())
					{
						if ((one.index & rt_type_mask) == rt_source_index_type)
						{
							browse_items_plain<source_data>(true, one.name, -1, prefix, path, filter, items, ctx, sources.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
					if constexpr (has_mappers())
					{
						if ((one.index & rt_type_mask) == rt_mapper_index_type)
						{
							browse_items_plain<mapper_data>(true, one.name, -1, prefix, path, filter, items, ctx, mappers.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
					if constexpr (has_filters())
					{
						if ((one.index & rt_type_mask) == rt_filter_index_type)
						{
							browse_items_plain<filter_data>(true, one.name, -1, prefix, path, filter, items, ctx, filters.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
					if constexpr (has_events())
					{
						if ((one.index & rt_type_mask) == rt_event_index_type)
						{
							browse_items_plain<event_data>(true, one.name, -1, prefix, path, filter, items, ctx, events.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
					if constexpr (has_block_data())
					{
						if ((one.index & rt_type_mask) == rt_value_data_index_type)
						{
							browse_items_blocks(true, one.name, -1, prefix, path, filter, items, ctx, blocks.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
					if constexpr (has_variable_blocks_data())
					{
						if ((one.index & rt_type_mask) == rt_variable_data_index_type)
						{
							browse_items_arrayed<variable_block_data>(true, one.name, -1, prefix, path, filter, items, ctx, variable_blocks.collection[(one.index >> rt_type_shift)]);
							continue;
						}
					}
				}
			}
		}
		return true;
	}
	else
	{
		string_type bellow;
		string_type mine;
		string_type prefix_bellow;

		string_view_type mine_view;
		string_view_type bellow_view;
		int array_idx = -1;
		auto item_idx = internal_split_get_index(string_view_type(path), mine_view, bellow_view, array_idx, this->items);
		if (item_idx)
		{
			bellow = bellow_view;
			mine = mine_view;
			prefix_bellow = prefix.empty() ? mine : prefix + RX_OBJECT_DELIMETER + mine;

			switch ((item_idx & rt_type_mask))
			{
			case rt_const_index_type:
				return browse_items_const(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, const_values[(item_idx >> rt_type_shift)]);
			case rt_value_index_type:
				return browse_items_value(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, values[(item_idx >> rt_type_shift)]);
			default:
				{
					if constexpr (has_variables())
					{
						if ((item_idx & rt_type_mask) == rt_variable_index_type)
							return browse_items_variable(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, variables.collection[(item_idx >> rt_type_shift)]);
					}
					if constexpr (has_structs())
					{
						if ((item_idx & rt_type_mask) == rt_struct_index_type)
							return browse_items_arrayed<struct_data>(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, structs.collection[(item_idx >> rt_type_shift)]);
					}
					if constexpr (has_sources())
					{
						if ((item_idx & rt_type_mask) == rt_source_index_type)
							return browse_items_plain<source_data>(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, sources.collection[(item_idx >> rt_type_shift)]);
					}
					if constexpr (has_mappers())
					{
						if ((item_idx & rt_type_mask) == rt_mapper_index_type)
							return browse_items_plain<mapper_data>(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, mappers.collection[(item_idx >> rt_type_shift)]);
					}
					if constexpr (has_filters())
					{
						if ((item_idx & rt_type_mask) == rt_filter_index_type)
							return browse_items_plain<filter_data>(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, filters.collection[(item_idx >> rt_type_shift)]);
					}
					if constexpr (has_events())
					{
						if ((item_idx & rt_type_mask) == rt_event_index_type)
							return browse_items_plain<event_data>(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, events.collection[(item_idx >> rt_type_shift)]);
					}
					if constexpr (has_block_data())
					{
						if ((item_idx & rt_type_mask) == rt_value_data_index_type)
							return browse_items_blocks(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, blocks.collection[(item_idx >> rt_type_shift)]);
					}
					if constexpr (has_variable_blocks_data())
					{
						if ((item_idx & rt_type_mask) == rt_variable_data_index_type)
							return browse_items_arrayed<variable_block_data>(false, mine, array_idx, prefix_bellow, bellow, filter, items, ctx, variable_blocks.collection[(item_idx >> rt_type_shift)]);
					}
				}
			}
		}
		return RX_INVALID_PATH;
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
const runtime_item* runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_child_item (string_view_type path) const
{


	  //////////////////////////////////////////////////////////////////
	  //
	if (path.empty())
	{// our value
		return this;
	}
	else
	{
		string_view_type mine;
		string_view_type bellow;
		int array_idx = -1;
		auto idx = internal_split_get_index(path, mine, bellow, array_idx, items);
		if (idx)
		{
			uint_fast16_t index = idx & rt_type_mask;
			if constexpr (has_variables())
			{
				if (index == rt_variable_index_type)
					return get_child_item_variable(array_idx, bellow, variables.collection[(idx >> rt_type_shift)]);
			}
			if constexpr (has_structs())
			{
				if (index == rt_struct_index_type)
					return get_child_item_arrayed<struct_data>(array_idx, bellow, structs.collection[(idx >> rt_type_shift)]);
			}
			if constexpr (has_sources())
			{
				if (index == rt_source_index_type)
					return get_child_item_plain<source_data>(array_idx, bellow, sources.collection[(idx >> rt_type_shift)]);
			}
			if constexpr (has_mappers())
			{
				if (index == rt_mapper_index_type)
					return get_child_item_plain<mapper_data>(array_idx, bellow, mappers.collection[(idx >> rt_type_shift)]);
			}
			if constexpr (has_filters())
			{
				if (index == rt_filter_index_type)
					return get_child_item_plain<filter_data>(array_idx, bellow, filters.collection[(idx >> rt_type_shift)]);
			}
			if constexpr (has_events())
			{
				if (index == rt_event_index_type)
					return get_child_item_plain<event_data>(array_idx, bellow, events.collection[(idx >> rt_type_shift)]);
			}
			if constexpr (has_block_data())
			{
				if (index == rt_value_data_index_type)
					return get_child_item_arrayed<value_block_data>(array_idx, bellow, blocks.collection[(idx >> rt_type_shift)]);
			}
			if constexpr (has_variable_blocks_data())
			{
				if (index == rt_variable_data_index_type)
					return get_child_item_arrayed<variable_block_data>(array_idx, bellow, variable_blocks.collection[(idx >> rt_type_shift)]);

			}
		}
	}
	return nullptr;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
runtime_filters_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_filters ()
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
runtime_sources_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_sources ()
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
runtime_mappers_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_mappers ()
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
runtime_events_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_events ()
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
runtime_variables_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_variables ()
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
runtime_structs_type& runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_structs ()
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::get_local_value (string_view_type path, rx_simple_value& val) const
{
	if (path.empty())
	{// our value
		rx_simple_value temp_val;
		rx_result result = collect_value(temp_val, runtime_value_type::simple_runtime_value);
		if (!result)
		{
			return result;
		}
		val = std::move(temp_val);
		return true;
	}
	else
	{
		string_view_type mine;
		string_view_type bellow;
		int array_idx = -1;
		auto idx = internal_split_get_index(path, mine, bellow, array_idx, items);
		if (idx)
		{
			uint_fast16_t index = idx & rt_type_mask;
			switch (index)
			{
			case rt_const_index_type:
				return get_local_value_const(array_idx, bellow, val, const_values[(idx >> rt_type_shift)]);
			case rt_value_index_type:
				return get_local_value_value(array_idx, bellow, val, values[(idx >> rt_type_shift)]);
			default:
				if constexpr (has_variables())
				{
					if (index == rt_variable_index_type)
						return get_local_value_variable(array_idx, bellow, val, variables.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_structs())
				{
					if (index == rt_struct_index_type)
						return get_local_value_arrayed<struct_data>(array_idx, bellow, val, structs.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_sources())
				{
					if (index == rt_source_index_type)
						return get_local_value_plain<source_data>(array_idx, bellow, val, sources.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_mappers())
				{
					if (index == rt_mapper_index_type)
						return get_local_value_plain<mapper_data>(array_idx, bellow, val, mappers.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_filters())
				{
					if (index == rt_filter_index_type)
						return get_local_value_plain<filter_data>(array_idx, bellow, val, filters.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_events())
				{
					if (index == rt_event_index_type)
						return get_local_value_plain<event_data>(array_idx, bellow, val, events.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_block_data())
				{
					if (index == rt_value_data_index_type)
						return get_local_value_arrayed<value_block_data>(array_idx, bellow, val, blocks.collection[(idx >> rt_type_shift)]);
				}
				if constexpr (has_variable_blocks_data())
				{
					if (index == rt_variable_data_index_type)
						return get_local_value_arrayed<variable_block_data>(array_idx, bellow, val, variable_blocks.collection[(idx >> rt_type_shift)]);
				}
			}
		}
	}
	return RX_INVALID_PATH;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::is_this_yours (string_view_type path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	if (idx == string_type::npos)
	{
		int array_idx;
		return internal_get_index(path, array_idx, this->items) != 0;
	}
	else
	{
		int array_idx;
		return internal_get_index(path.substr(0, idx), array_idx, items) != 0;
	}
	return false;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::read_struct (string_view_type path, read_struct_data data) const
{
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,blocks_type,variable_blocks_type,type_id>::write_struct (string_view_type path, write_struct_data data)
{
}


} // namespace structure
} // namespace runtime
} // namespace rx_platform

