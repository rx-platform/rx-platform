

/****************************************************************************
*
*  system\runtime\rx_rt_struct.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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

#include "system/server/rx_server.h"

// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"

#include "system/runtime/rx_display_blocks.h"
#include "system/runtime/rx_runtime_logic.h"
#include "rx_library.h"
#include "rx_blocks.h"
#include "rx_objbase.h"
#include "rx_relations.h"
#include "lib/rx_ser_json.h"
#include "runtime_internal/rx_runtime_internal.h"

namespace rx_platform
{
rx_security_handle_t rx_security_context();
}


namespace rx_platform {

namespace runtime {

namespace structure {


template class runtime_data<
	has<array_wrapper<variable_data> >,
	has<array_wrapper<struct_data> >,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3f>;

template class runtime_data<
	empty<array_wrapper<variable_data> >,
	empty<array_wrapper<struct_data> >,
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
			if (type != runtime_value_type::persistent_runtime_value)
			{
				if (!const_values[(one.index >> rt_type_shift)].is_array())
				{
					data.add_value(one.name, const_values[(one.index >> rt_type_shift)].get_item()->value);
				}
				else
				{
					std::vector<rx_simple_value> temp_vals;
					temp_vals.reserve(const_values[(one.index >> rt_type_shift)].get_size());
					for (int i = 0; i < const_values[(one.index >> rt_type_shift)].get_size(); i++)
					{
						temp_vals.push_back(const_values[(one.index >> rt_type_shift)].get_item(i)->value);
					}
					data.add_value(one.name, std::move(temp_vals));
				}
			}
			break;
		case rt_value_index_type:
			// simple value
			if (!values[(one.index >> rt_type_shift)].is_array())
			{
				if (type != runtime_value_type::persistent_runtime_value
					|| values[(one.index >> rt_type_shift)].get_item()->value_opt[runtime::structure::value_opt_persistent])
					data.add_value(one.name, values[(one.index >> rt_type_shift)].get_item()->value.to_simple());
			}
			else
			{
				std::vector<rx_simple_value> temp_vals;
				temp_vals.reserve(values[(one.index >> rt_type_shift)].get_size());
				for (int i = 0; i < values[(one.index >> rt_type_shift)].get_size(); i++)
				{
					if (type != runtime_value_type::persistent_runtime_value
						|| values[(one.index >> rt_type_shift)].get_item(i)->value_opt[runtime::structure::value_opt_persistent])
						temp_vals.push_back(values[(one.index >> rt_type_shift)].get_item(i)->value.to_simple());
				}
				data.add_value(one.name, std::move(temp_vals));
			}
			break;
		case rt_variable_index_type:
			if constexpr (has_variables())
			{
				if (!variables.collection[(one.index >> rt_type_shift)].is_array())
				{
					if (type == runtime_value_type::simple_runtime_value)
					{
						data.add_value(one.name, variables.collection[(one.index >> rt_type_shift)].get_item()->value.to_simple());
					}
					else
					{
						data::runtime_values_data child_data;
						variables.collection[(one.index >> rt_type_shift)].get_item()->collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
					}
				}
				else
				{
					if (type == runtime_value_type::simple_runtime_value)
					{
						std::vector<rx_simple_value> temp_vals;
						temp_vals.reserve(variables.collection[(one.index >> rt_type_shift)].get_size());
						for (int i = 0; i < variables.collection[(one.index >> rt_type_shift)].get_size(); i++)
						{
							temp_vals.push_back(variables.collection[(one.index >> rt_type_shift)].get_item(i)->value.to_simple());
						}
						data.add_value(one.name, std::move(temp_vals));
					}
					else
					{
						std::vector<data::runtime_values_data> childs;
						childs.reserve(variables.collection[(one.index >> rt_type_shift)].get_size());
						for (int i = 0; i < variables.collection[(one.index >> rt_type_shift)].get_size(); i++)
						{
							data::runtime_values_data child_data;
							variables.collection[(one.index >> rt_type_shift)].get_item(i)->collect_data(child_data, type);
							if (!child_data.empty())
								childs.push_back(std::move(child_data));
						}
						if(!childs.empty())
							data.add_array_child(one.name, std::move(childs));
					}
				}
			}
			break;
		case rt_struct_index_type:
			{
				if constexpr (has_structs())
				{
					if (!structs.collection[(one.index >> rt_type_shift)].is_array())
					{
						data::runtime_values_data child_data;
						structs.collection[(one.index >> rt_type_shift)].get_item()->collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
					}
					else
					{
						std::vector<data::runtime_values_data> childs;
						childs.reserve(structs.collection[(one.index >> rt_type_shift)].get_size());
						for (int i = 0; i < structs.collection[(one.index >> rt_type_shift)].get_size(); i++)
						{
							data::runtime_values_data child_data;
							structs.collection[(one.index >> rt_type_shift)].get_item(i)->collect_data(child_data, type);
							if (!child_data.empty())
								childs.push_back(std::move(child_data));
						}
						if (!childs.empty())
							data.add_array_child(one.name, std::move(childs));
					}

				}
			}
			break;
		case rt_source_index_type:
			{
				if constexpr (has_sources())
				{
					if ((type & runtime_value_type::sources_runtime_value) == runtime_value_type::sources_runtime_value)
					{
						data::runtime_values_data child_data;
						sources.collection[(one.index >> rt_type_shift)].collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
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
						data::runtime_values_data child_data;
						mappers.collection[(one.index >> rt_type_shift)].collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
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
						data::runtime_values_data child_data;
						filters.collection[(one.index >> rt_type_shift)].collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
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
						data::runtime_values_data child_data;
						events.collection[(one.index >> rt_type_shift)].collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
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
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::fill_data (const data::runtime_values_data& data)
{
	for (auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			{// const value
				if (!const_values[(one.index >> rt_type_shift)].is_array())
				{
					auto val = data.get_value(one.name);
					if (!val.is_null())
					{
						const_values[one.index >> rt_type_shift].get_item()->set_value(std::move(val));
					}
				}
				else
				{
					std::vector<rx_simple_value> vals;
					if (data.get_array_value(one.name, vals))
					{
						size_t dim = std::min<size_t>(vals.size(), const_values[(one.index >> rt_type_shift)].get_size());
						for (size_t i = 0; i < dim; i++)
						{
							const_values[one.index >> rt_type_shift].get_item((int)i)->set_value(std::move(vals[i]));
						}
					}
				}
			}
			break;
		case rt_value_index_type:
			{// value
				if (!values[(one.index >> rt_type_shift)].is_array())
				{
					auto val = data.get_value(one.name);
					if (!val.is_null())
					{
						values[one.index >> rt_type_shift].get_item()->set_value(std::move(val), rx_time::now());
					}
				}
				else
				{
					std::vector<rx_simple_value> vals;
					if (data.get_array_value(one.name, vals))
					{
						rx_time now = rx_time::now();
						size_t dim = std::min<size_t>(vals.size(), values[(one.index >> rt_type_shift)].get_size());
						for (size_t i = 0; i < dim; i++)
						{
							values[one.index >> rt_type_shift].get_item((int)i)->set_value(std::move(vals[i]), now);
						}
					}
				}
			}
			break;
		case rt_variable_index_type:
			{// variable
				// check for simple value first
				if constexpr (has_variables())
				{
					if (!variables.collection[(one.index >> rt_type_shift)].is_array())
					{
						auto val = data.get_value(one.name);
						if (!val.is_null())
						{
							variables.collection[one.index >> rt_type_shift].get_item()->set_value(std::move(val));
						}
						auto it = data.children.find(one.name);
						if (it != data.children.end())
						{
							if (std::holds_alternative<data::runtime_values_data>(it->second))
							{
								variables.collection[one.index >> rt_type_shift].get_item()->fill_data(
									std::get< data::runtime_values_data>(it->second));
							}
						}
					}
					else
					{
						std::vector<rx_simple_value> vals;
						if (data.get_array_value(one.name, vals))
						{
							size_t dim = std::min<size_t>(vals.size(), variables.collection[(one.index >> rt_type_shift)].get_size());
							for (size_t i = 0; i < dim; i++)
							{
								variables.collection[one.index >> rt_type_shift].get_item((int)i)->set_value(std::move(vals[i]));
							}
						}
						auto it = data.children.find(one.name);
						if (it != data.children.end())
						{
							if (std::holds_alternative<std::vector<data::runtime_values_data> >(it->second))
							{
								auto& childs = std::get<std::vector<data::runtime_values_data>>(it->second);
								size_t dim = std::min<size_t>(childs.size(), variables.collection[(one.index >> rt_type_shift)].get_size());
								for (size_t i = 0; i < dim; i++)
								{
									variables.collection[one.index >> rt_type_shift].get_item((int)i)->fill_data(childs[i]);
								}
							}
						}
					}
				}
			}
			break;
		case rt_struct_index_type:
			{// struct
				if constexpr (has_structs())
				{
					if (!structs.collection[(one.index >> rt_type_shift)].is_array())
					{
						auto it = data.children.find(one.name);
						if (it != data.children.end())
						{
							if (std::holds_alternative<data::runtime_values_data>(it->second))
							{
								structs.collection[one.index >> rt_type_shift].get_item()->fill_data(
									std::get< data::runtime_values_data>(it->second));
							}
						}
					}
					else
					{
						auto it = data.children.find(one.name);
						if (it != data.children.end())
						{
							if (std::holds_alternative<std::vector<data::runtime_values_data> >(it->second))
							{
								auto& childs = std::get<std::vector<data::runtime_values_data>>(it->second);
								size_t dim = std::min<size_t>(childs.size(), structs.collection[(one.index >> rt_type_shift)].get_size());
								for (size_t i = 0; i < dim; i++)
								{
									structs.collection[one.index >> rt_type_shift].get_item((int)i)->fill_data(childs[i]);
								}
							}
						}
					}
				}
			}
			break;
		case rt_source_index_type:
			{// source
				if constexpr (has_sources())
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
					{
						sources.collection[one.index >> rt_type_shift].fill_data(std::get<data::runtime_values_data>(it->second));
					}
				}
			}
			break;
		case rt_mapper_index_type:
			{// mapper
				if constexpr (has_mappers())
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
					{
						mappers.collection[one.index >> rt_type_shift].fill_data(std::get<data::runtime_values_data>(it->second));
					}
				}
			}
			break;
		case rt_filter_index_type:
			{// filter
				if constexpr (has_filters())
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
					{
						filters.collection[one.index >> rt_type_shift].fill_data(std::get<data::runtime_values_data>(it->second));
					}
				}
			}
			break;
		case rt_event_index_type:
			{// event
				if constexpr (has_events())
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
					{
						events.collection[one.index >> rt_type_shift].fill_data(std::get<data::runtime_values_data>(it->second));
					}
				}
			}
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{// our value
		rx_result result;
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
			writer.write_footer();
			string_type temp_str(writer.get_string());
			if (!temp_str.empty())
			{
				val.assign_static(temp_str.c_str(), ctx->now);
				result = true;
			}
			else
			{
				result = "Retrieving string from JSON stream";
			}
		}
		return result;
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
			int array_idx = -1;
			auto idx = internal_get_index(mine, array_idx);
			if (idx && is_complex_index(idx))
			{
				if (array_idx < 0)
				{
					switch (idx & rt_type_mask)
					{
					case rt_variable_index_type:
						if (variables.collection[(idx >> rt_type_shift)].is_array())
							return "Expected array index!";
						else
							return variables.collection[idx >> rt_type_shift].get_item()->get_value(bellow, val, ctx);
					case rt_struct_index_type:
						if (structs.collection[(idx >> rt_type_shift)].is_array())
							return "Expected array index!";
						else
							return structs.collection[idx >> rt_type_shift].get_item()->get_value(bellow, val, ctx);
					case rt_source_index_type:
						return sources.collection[idx >> rt_type_shift].get_value(bellow, val, ctx);
					case rt_mapper_index_type:
						return mappers.collection[idx >> rt_type_shift].get_value(bellow, val, ctx);
					case rt_filter_index_type:
						return filters.collection[idx >> rt_type_shift].get_value(bellow, val, ctx);
					case rt_event_index_type:
						return events.collection[idx >> rt_type_shift].get_value(bellow, val, ctx);
					default:
						RX_ASSERT(false);
					}
				}
				else
				{
					switch (idx & rt_type_mask)
					{
					case rt_variable_index_type:
						if (!variables.collection[(idx >> rt_type_shift)].is_array())
							return "Not an array!";
						else if (array_idx < variables.collection[idx >> rt_type_shift].get_size())
							return variables.collection[idx >> rt_type_shift].get_item(array_idx)->get_value(bellow, val, ctx);
						else
							return "Out of bounds!";
					case rt_struct_index_type:
						if (!structs.collection[(idx >> rt_type_shift)].is_array())
							return "Not an array!";
						else if (array_idx < structs.collection[idx >> rt_type_shift].get_size())
							return structs.collection[idx >> rt_type_shift].get_item(array_idx)->get_value(bellow, val, ctx);
						else
							return "Out of bounds!";
					case rt_source_index_type:
					case rt_mapper_index_type:
					case rt_filter_index_type:
					case rt_event_index_type:
						return "Not an array!";
					default:
						RX_ASSERT(false);
					}
				}
			}
		}
		else// its' ours
		{
			int array_idx = -1;
			auto idx = internal_get_index(path, array_idx);
			if (idx && is_value_index(idx))
			{
				switch (idx & rt_type_mask)
				{
				case rt_const_index_type:
					if (array_idx < 0)
					{
						if (!const_values[(idx >> rt_type_shift)].is_array())
						{
							val = const_values[idx >> rt_type_shift].get_item()->get_value(ctx);
							return true;
						}
						else
						{
							rx_simple_value temp_val;
							uint32_t len;
							auto size = const_values[idx >> rt_type_shift].get_size();
							RX_ASSERT(size >= 0);
							if (size >= 0)
							{
								len = (uint32_t)size;
								temp_val.assign_static(len);
								val = ctx->adapt_value(temp_val);
								return true;
							}
						}
					}
					else
					{
						if (!const_values[(idx >> rt_type_shift)].is_array())
						{
							return "Not an array!";
						}
						else if(array_idx< const_values[(idx >> rt_type_shift)].get_size())
						{
							val = const_values[idx >> rt_type_shift].get_item(array_idx)->get_value(ctx);
							return true;
						}
						else
						{
							return "Out of bounds!";
						}
					}
				case rt_value_index_type:
					if (array_idx < 0)
					{
						if (!values[(idx >> rt_type_shift)].is_array())
						{
							val = values[idx >> rt_type_shift].get_item()->get_value(ctx);
							return true;
						}
						else
						{
							rx_simple_value temp_val;
							uint32_t len;
							auto size = values[idx >> rt_type_shift].get_size();
							RX_ASSERT(size >= 0);
							if (size >= 0)
							{
								len = (uint32_t)size;
								temp_val.assign_static(len);
								val = ctx->adapt_value(temp_val);
								return true;
							}
						}
					}
					else
					{
						if (!values[(idx >> rt_type_shift)].is_array())
						{
							return "Not an array!";
						}
						else if (array_idx < values[(idx >> rt_type_shift)].get_size())
						{
							val = values[idx >> rt_type_shift].get_item(array_idx)->get_value(ctx);
							return true;
						}
						else
						{
							return "Out of bounds!";
						}
					}
					return true;
				case rt_variable_index_type:
					if (array_idx < 0)
					{
						if (!variables.collection[(idx >> rt_type_shift)].is_array())
						{
							val = variables.collection[idx >> rt_type_shift].get_item()->get_value(ctx);
							return true;
						}
						else
						{
							rx_simple_value temp_val;
							uint32_t len;
							auto size = variables.collection[idx >> rt_type_shift].get_size();
							RX_ASSERT(size >= 0);
							if (size >= 0)
							{
								len = (uint32_t)size;
								temp_val.assign_static(len);
								val = ctx->adapt_value(temp_val);
								return true;
							}
						}
					}
					else
					{
						if (!variables.collection[(idx >> rt_type_shift)].is_array())
						{
							return "Not an array!";
						}
						else if (array_idx < variables.collection[(idx >> rt_type_shift)].get_size())
						{
							val = variables.collection[idx >> rt_type_shift].get_item(array_idx)->get_value(ctx);
							return true;
						}
						else
						{
							return "Out of bounds!";
						}
					}
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
	if constexpr (has_structs())
	{
		for (auto& one : structs.collection)
		{
			if (one.is_array())
			{
				for(int i=0; i<one.get_size(); i++)
					one.get_item(i)->object_state_changed(ctx);
			}
			else
			{
				one.get_item()->object_state_changed(ctx);
			}
		}
	}
	if constexpr (has_variables())
	{
		for (auto& one : variables.collection)
		{
			if (one.is_array())
			{
				for (int i = 0; i < one.get_size(); i++)
					one.get_item(i)->object_state_changed(ctx);
			}
			else
			{
				one.get_item()->object_state_changed(ctx);
			}
		}
	}
	if constexpr (has_sources())
	{
		for (auto& one : sources.collection)
			one.object_state_changed(ctx);
	}
	if constexpr (has_mappers())
	{
		for (auto& one : mappers.collection)
			one.object_state_changed(ctx);
	}
	if constexpr (has_events())
	{
		for (auto& one : events.collection)
			one.object_state_changed(ctx);
	}
	if constexpr (has_filters())
	{
		for (auto& one : filters.collection)
			one.object_state_changed(ctx);
	}
	for (auto& one : values)
	{
		if (one.is_array())
		{
			for (int i = 0; i < one.get_size(); i++)
				one.get_item(i)->object_state_changed(ctx);
		}
		else
		{
			one.get_item()->object_state_changed(ctx);
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::initialize_runtime (runtime::runtime_init_context& ctx)
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
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			if constexpr (has_variables())
			{
				if (variables.collection[one.index >> rt_type_shift].is_array())
				{
					char buff[0x100];
					for (int i = 0; i < variables.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						sprintf(buff, "%s[%d]", one.name.c_str(), i);
						ctx.path.push_to_path(buff);
						ret = variables.collection[one.index >> rt_type_shift].get_item(i)->initialize_runtime(ctx);
						if (!ret)
							break;
						ctx.path.pop_from_path();
					}
				}
				else
				{
					ctx.path.push_to_path(one.name);
					ret = variables.collection[one.index >> rt_type_shift].get_item()->initialize_runtime(ctx);
					ctx.path.pop_from_path();
				}
			}
			break;
		case rt_struct_index_type:
			// struct
			if constexpr (has_structs())
			{
				if (structs.collection[one.index >> rt_type_shift].is_array())
				{
					char buff[0x100];
					for (int i = 0; i < structs.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						sprintf(buff, "%s[%d]", one.name.c_str(), i);
						ctx.path.push_to_path(buff);
						ret = structs.collection[one.index >> rt_type_shift].get_item(i)->initialize_runtime(ctx);
						if (!ret)
							break;
						ctx.path.pop_from_path();
					}
				}
				else
				{
					ctx.path.push_to_path(one.name);
					ret = structs.collection[one.index >> rt_type_shift].get_item()->initialize_runtime(ctx);
					ctx.path.pop_from_path();
				}
			}
			break;
		case rt_source_index_type:
			// source
			if constexpr (has_sources())
			{
				ctx.path.push_to_path(one.name);
				ret = sources.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
				ctx.path.pop_from_path();
			}
			break;
		case rt_mapper_index_type:
			// mapper
			if constexpr (has_mappers())
			{
				ctx.path.push_to_path(one.name);
				ret = mappers.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
				ctx.path.pop_from_path();
			}
			break;
		case rt_filter_index_type:
			// filter
			if constexpr (has_filters())
			{
				ctx.path.push_to_path(one.name);
				ret = filters.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
				ctx.path.pop_from_path();
			}
			break;
		case rt_event_index_type:
			// event
			if constexpr (has_events())
			{
				ctx.path.push_to_path(one.name);
				ret = events.collection[one.index >> rt_type_shift].initialize_runtime(ctx);
				ctx.path.pop_from_path();
			}
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
		}
		if (!ret)
			break;
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

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	rx_result ret(true);
	for (auto& one : items)
	{
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			if constexpr (has_variables())
			{
				if (variables.collection[one.index >> rt_type_shift].is_array())
				{
					for (int i = 0; i < variables.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						ret = variables.collection[one.index >> rt_type_shift].get_item(i)->deinitialize_runtime(ctx);
						if (!ret)
							break;
					}
				}
				else
				{
					ret = variables.collection[one.index >> rt_type_shift].get_item()->deinitialize_runtime(ctx);
				}
			}
			break;
		case rt_struct_index_type:
			// struct
			if constexpr (has_structs())
			{
				if (structs.collection[one.index >> rt_type_shift].is_array())
				{
					for (int i = 0; i < structs.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						ret = structs.collection[one.index >> rt_type_shift].get_item(i)->deinitialize_runtime(ctx);
						if (!ret)
							break;
					}
				}
				else
				{
					ret = structs.collection[one.index >> rt_type_shift].get_item()->deinitialize_runtime(ctx);
				}
			}
			break;
		case rt_source_index_type:
			// source
			if constexpr (has_sources())
				ret = sources.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_mapper_index_type:
			// mapper
			if constexpr (has_mappers())
				ret = mappers.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_filter_index_type:
			// filter
			if constexpr (has_filters())
				ret = filters.collection[one.index >> rt_type_shift].deinitialize_runtime(ctx);
			break;
		case rt_event_index_type:
			// event
			if constexpr (has_events())
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
	for (auto& one : items)
	{
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			if constexpr (has_variables())
			{
				if (variables.collection[one.index >> rt_type_shift].is_array())
				{
					char buff[0x100];
					for (int i = 0; i < variables.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						sprintf(buff, "%s[%d]", one.name.c_str(), i);
						ctx.path.push_to_path(buff);
						ret = variables.collection[one.index >> rt_type_shift].get_item(i)->start_runtime(ctx);
						if (!ret)
							break;
						ctx.path.pop_from_path();
					}
				}
				else
				{
					ctx.path.push_to_path(one.name);
					ret = variables.collection[one.index >> rt_type_shift].get_item()->start_runtime(ctx);
					ctx.path.pop_from_path();
				}
			}
			break;
			break;
		case rt_struct_index_type:
			// struct
			if constexpr (has_structs())
			{
				if (structs.collection[one.index >> rt_type_shift].is_array())
				{
					char buff[0x100];
					for (int i = 0; i < structs.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						sprintf(buff, "%s[%d]", one.name.c_str(), i);
						ctx.path.push_to_path(buff);
						ret = structs.collection[one.index >> rt_type_shift].get_item(i)->start_runtime(ctx);
						if (!ret)
							break;
						ctx.path.pop_from_path();
					}
				}
				else
				{
					ctx.path.push_to_path(one.name);
					ret = structs.collection[one.index >> rt_type_shift].get_item()->start_runtime(ctx);
					ctx.path.pop_from_path();
				}
			}
			break;
		case rt_source_index_type:
			// source
			if constexpr (has_sources())
			{
				ctx.path.push_to_path(one.name);
				ret = sources.collection[one.index >> rt_type_shift].start_runtime(ctx);
				ctx.path.pop_from_path();
			}
			break;
		case rt_mapper_index_type:
			// mapper
			if constexpr (has_mappers())
			{
				ctx.path.push_to_path(one.name);
				ret = mappers.collection[one.index >> rt_type_shift].start_runtime(ctx);
				ctx.path.pop_from_path();
			}
			break;
		case rt_filter_index_type:
			// filter
			ctx.path.push_to_path(one.name);
			ret = filters.collection[one.index >> rt_type_shift].start_runtime(ctx);
			ctx.path.pop_from_path();
			break;
		case rt_event_index_type:
			// event
			if constexpr (has_events())
			{
				ctx.path.push_to_path(one.name);
				ret = events.collection[one.index >> rt_type_shift].start_runtime(ctx);
				ctx.path.pop_from_path();
			}
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
	for (auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
		case rt_value_index_type:
			break;
		case rt_variable_index_type:
			// variable
			if constexpr (has_variables())
			{
				if (variables.collection[one.index >> rt_type_shift].is_array())
				{
					for (int i = 0; i < variables.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						ret = variables.collection[one.index >> rt_type_shift].get_item(i)->stop_runtime(ctx);
						if (!ret)
							break;
					}
				}
				else
				{
					ret = variables.collection[one.index >> rt_type_shift].get_item()->stop_runtime(ctx);
				}
			}
			break;
		case rt_struct_index_type:
			// struct
			if constexpr (has_structs())
			{
				if (structs.collection[one.index >> rt_type_shift].is_array())
				{
					for (int i = 0; i < structs.collection[one.index >> rt_type_shift].get_size(); i++)
					{
						ret = structs.collection[one.index >> rt_type_shift].get_item(i)->stop_runtime(ctx);
						if (!ret)
							break;
					}
				}
				else
				{
					ret = structs.collection[one.index >> rt_type_shift].get_item()->stop_runtime(ctx);
				}
			}
			break;
		case rt_source_index_type:
			// source
			if constexpr (has_sources())
				ret = sources.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_mapper_index_type:
			// mapper
			if constexpr (has_mappers())
				ret = mappers.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_filter_index_type:
			// filter
			if constexpr (has_filters())
				ret = filters.collection[one.index >> rt_type_shift].stop_runtime(ctx);
			break;
		case rt_event_index_type:
			// event
			if constexpr (has_events())
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
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_value_ref (string_view_type path, rt_value_ref& ref, bool is_var)
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_view_type mine;
	if (idx != string_view_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_view_type bellow = path.substr(idx + 1);
		int array_idx = -1;
		auto idx = internal_get_index(mine, array_idx);
		if (idx && is_complex_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_variable_index_type:
				if (array_idx < 0)
				{
					if (!variables.collection[idx >> rt_type_shift].is_array())
						return variables.collection[idx >> rt_type_shift].get_item()->get_value_ref(bellow, ref);
					else
						return "Invalid array index";
				}
				else
				{
					if (!variables.collection[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < variables.collection[idx >> rt_type_shift].get_size())
						return variables.collection[idx >> rt_type_shift].get_item(array_idx)->get_value_ref(bellow, ref);
					else
						return "Out of bounds!";
				}
			case rt_struct_index_type:
				if (array_idx < 0)
				{
					if (!structs.collection[idx >> rt_type_shift].is_array())
						return structs.collection[idx >> rt_type_shift].get_item()->get_value_ref(bellow, ref);
					else
						return "Invalid array index";
				}
				else
				{
					if (!structs.collection[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < structs.collection[idx >> rt_type_shift].get_size())
						return structs.collection[idx >> rt_type_shift].get_item(array_idx)->get_value_ref(bellow, ref);
					else
						return "Out of bounds!";
				}
			case rt_source_index_type:
				if (array_idx >= 0)
					return "Not an array!";
				return sources.collection[idx >> rt_type_shift].get_value_ref(bellow, ref);
			case rt_mapper_index_type:
				if (array_idx >= 0)
					return "Not an array!";
				return mappers.collection[idx >> rt_type_shift].get_value_ref(bellow, ref);
			case rt_filter_index_type:
				if (array_idx >= 0)
					return "Not an array!";
				return filters.collection[idx >> rt_type_shift].get_value_ref(bellow, ref);
			case rt_event_index_type:
				if (array_idx >= 0)
					return "Not an array!";
				return events.collection[idx >> rt_type_shift].get_value_ref(bellow, ref);
			default:
				RX_ASSERT(false);
			}
		}
	}
	else// its' ours
	{
		int array_idx = -1;
		auto idx = internal_get_index(path, array_idx);
		if (idx && is_value_index(idx))
		{
			switch (idx&rt_type_mask)
			{
			case rt_const_index_type:
				if (array_idx < 0)
				{
					ref.ref_type = rt_value_ref_type::rt_const_value;
					if (!const_values[idx >> rt_type_shift].is_array())
						ref.ref_value_ptr.const_value = const_values[idx >> rt_type_shift].get_item();
					else
						return "Invalid array index";
				}
				else
				{
					ref.ref_type = rt_value_ref_type::rt_const_value;
					if (!const_values[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < const_values[idx >> rt_type_shift].get_size())
						ref.ref_value_ptr.const_value = const_values[idx >> rt_type_shift].get_item(array_idx);
					else
						return "Out of bounds!";
				}
				return true;
			case rt_value_index_type:
				if (array_idx < 0)
				{
					ref.ref_type = rt_value_ref_type::rt_value;
					if (!values[idx >> rt_type_shift].is_array())
						ref.ref_value_ptr.value = values[idx >> rt_type_shift].get_item();
					else
						return "Invalid array index";
				}
				else
				{
					ref.ref_type = rt_value_ref_type::rt_value;
					if (!values[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < values[idx >> rt_type_shift].get_size())
						ref.ref_value_ptr.value = values[idx >> rt_type_shift].get_item(array_idx);
					else
						return "Out of bounds!";
				}
				return true;
			case rt_variable_index_type:
				if (array_idx < 0)
				{
					ref.ref_type = rt_value_ref_type::rt_variable;
					if (!variables.collection[idx >> rt_type_shift].is_array())
						ref.ref_value_ptr.variable = variables.collection[idx >> rt_type_shift].get_item();
					else
						return "Invalid array index";
				}
				else
				{
					ref.ref_type = rt_value_ref_type::rt_variable;
					if (!variables.collection[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < variables.collection[idx >> rt_type_shift].get_size())
						ref.ref_value_ptr.variable = variables.collection[idx >> rt_type_shift].get_item(array_idx);
					else
						return "Out of bounds!";
				}
				return true;
			case rt_source_index_type:
				if (array_idx >= 0)
					return "Not an array!";
				if(!is_var)
					return string_type(path) + " not valid!";
				ref.ref_type = rt_value_ref_type::rt_full_value;
				ref.ref_value_ptr.full_value = &sources.collection[idx >> rt_type_shift].input_value;
				return true;
			case rt_mapper_index_type:
				if (array_idx >= 0)
					return "Not an array!";
				if (!is_var)
					return string_type(path) + " not valid!";
				ref.ref_type = rt_value_ref_type::rt_full_value;
				ref.ref_value_ptr.full_value = &mappers.collection[idx >> rt_type_shift].mapped_value;
				return true;
			default:
				RX_ASSERT(false);// has to be because of is_value_index;
			}
		}
	}
	return string_type(path) + " not found!";
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_result runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		for (const auto& one : this->items)
		{
			runtime_item_attribute one_attr;
			switch (one.index & rt_type_mask)
			{
			case rt_const_index_type:
				one_attr.name = one.name;
				if (!const_values[(one.index >> rt_type_shift)].is_array())
				{
					one_attr.value = const_values[(one.index >> rt_type_shift)].get_item()->get_value(ctx);
					one_attr.type = rx_attribute_type::const_attribute_type;
				}
				else
				{
					one_attr.type = rx_attribute_type::const_array_attribute_type;
				}
				break;
			case rt_value_index_type:
				one_attr.name = one.name;
				if (!values[(one.index >> rt_type_shift)].is_array())
				{
					one_attr.value = values[(one.index >> rt_type_shift)].get_item()->get_value(ctx);
					one_attr.type = rx_attribute_type::value_attribute_type;
				}
				else
				{
					one_attr.type = rx_attribute_type::value_array_attribute_type;
				}
				break;
			case rt_variable_index_type:
				one_attr.name = one.name;
				if (!variables.collection[(one.index >> rt_type_shift)].is_array())
				{
					one_attr.value = variables.collection[(one.index >> rt_type_shift)].get_item()->get_value(ctx);
					one_attr.type = rx_attribute_type::variable_attribute_type;
				}
				else
				{
					one_attr.type = rx_attribute_type::variable_array_attribute_type;
				}
				break;
			case rt_struct_index_type:
				one_attr.name = one.name;
				if (!structs.collection[(one.index >> rt_type_shift)].is_array())
				{
					one_attr.type = rx_attribute_type::struct_attribute_type;
				}
				else
				{
					one_attr.type = rx_attribute_type::struct_array_attribute_type;
				}
				break;
			case rt_source_index_type:
				one_attr.name = one.name;
				if constexpr (has_sources())
					one_attr.value = sources.collection[(one.index >> rt_type_shift)].get_value(ctx);
				one_attr.type = rx_attribute_type::source_attribute_type;
				break;
			case rt_mapper_index_type:
				one_attr.name = one.name;
				if constexpr (has_mappers())
					one_attr.value = mappers.collection[(one.index >> rt_type_shift)].get_value(ctx);
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
				RX_ASSERT(false);
				one_attr.type = rx_attribute_type::invalid_attribute_type_type;
			}
			if (one_attr.type != rx_attribute_type::invalid_attribute_type_type)
			{
				if(prefix.empty())
					one_attr.full_path = one.name;
				else
					one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one.name;

				items.emplace_back(std::move(one_attr));
			}
		}
		return true;
	}
	else
	{
		size_t idx = path.find(RX_OBJECT_DELIMETER);
		string_type mine;
		string_type bellow;
		if (idx != string_type::npos)
		{// bellow us, split it
			mine = path.substr(0, idx);
			bellow = path.substr(idx + 1);
		}
		else
		{
			mine = path;
		}
		string_type prefix_bellow = prefix.empty() ? mine : prefix + RX_OBJECT_DELIMETER + mine;
		int array_idx = -1;
		auto item_idx = internal_get_index(mine, array_idx);
		if (item_idx)
		{
			if (array_idx < 0)
			{
				switch (item_idx & rt_type_mask)
				{
				case rt_variable_index_type:
					if (variables.collection[(item_idx >> rt_type_shift)].is_array())
					{
						if (bellow.empty())
						{
							for (int i = 0; i < variables.collection[(item_idx >> rt_type_shift)].get_size(); i++)
							{
								char temp_buff[0x20];
								runtime_item_attribute one_attr;
								sprintf(temp_buff, "[%d]", i);
								one_attr.name = mine + temp_buff;
								one_attr.type = rx_attribute_type::variable_attribute_type;
								one_attr.value = variables.collection[(item_idx >> rt_type_shift)].get_item(i)->get_value(ctx);
								if (prefix.empty())
									one_attr.full_path = one_attr.name;
								else
									one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
								items.emplace_back(std::move(one_attr));
							}
							return true;
						}
					}
					else
					{
						return variables.collection[item_idx >> rt_type_shift].get_item()->browse_items(prefix_bellow, bellow, filter, items, ctx);
					}
					break;
				case rt_struct_index_type:
					if (structs.collection[(item_idx >> rt_type_shift)].is_array())
					{
						if (bellow.empty())
						{
							for (int i = 0; i < structs.collection[(item_idx >> rt_type_shift)].get_size(); i++)
							{
								char temp_buff[0x20];
								runtime_item_attribute one_attr;
								sprintf(temp_buff, "[%d]", i);
								one_attr.name = mine + temp_buff;
								one_attr.type = rx_attribute_type::struct_attribute_type;
								if (prefix.empty())
									one_attr.full_path = one_attr.name;
								else
									one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
								items.emplace_back(std::move(one_attr));
							}
							return true;
						}
					}
					else
					{
						return structs.collection[item_idx >> rt_type_shift].get_item()->browse_items(prefix_bellow, bellow, filter, items, ctx);
					}
					break;
				case rt_source_index_type:
					return sources.collection[item_idx >> rt_type_shift].browse_items(prefix_bellow, bellow, filter, items, ctx);
				case rt_mapper_index_type:
					return mappers.collection[item_idx >> rt_type_shift].browse_items(prefix_bellow, bellow, filter, items, ctx);
				case rt_filter_index_type:
					return filters.collection[item_idx >> rt_type_shift].browse_items(prefix_bellow, bellow, filter, items, ctx);
				case rt_event_index_type:
					return events.collection[item_idx >> rt_type_shift].browse_items(prefix_bellow, bellow, filter, items, ctx);
				case rt_const_index_type:
					if (const_values[(item_idx >> rt_type_shift)].is_array() && bellow.empty())
					{
						for (int i = 0; i < const_values[(item_idx >> rt_type_shift)].get_size(); i++)
						{
							char temp_buff[0x20];
							runtime_item_attribute one_attr;
							sprintf(temp_buff, "[%d]", i);
							one_attr.name = mine + temp_buff;
							one_attr.type = rx_attribute_type::const_attribute_type;
							one_attr.value = const_values[(item_idx >> rt_type_shift)].get_item(i)->get_value(ctx);
							if (prefix.empty())
								one_attr.full_path = one_attr.name;
							else
								one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
							items.emplace_back(std::move(one_attr));
						}
						return true;
					}
					break;
				case rt_value_index_type:
					if (values[(item_idx >> rt_type_shift)].is_array() && bellow.empty())
					{
						for (int i = 0; i < values[(item_idx >> rt_type_shift)].get_size(); i++)
						{
							char temp_buff[0x20];
							runtime_item_attribute one_attr;
							sprintf(temp_buff, "[%d]", i);
							one_attr.name = mine + temp_buff;
							one_attr.type = rx_attribute_type::value_attribute_type;
							one_attr.value = values[(item_idx >> rt_type_shift)].get_item(i)->get_value(ctx);
							if (prefix.empty())
								one_attr.full_path = one_attr.name;
							else
								one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
							items.emplace_back(std::move(one_attr));
						}
						return true;
					}
					break;
				}
			}
			else
			{
				runtime_item_attribute one_attr;
				switch (item_idx & rt_type_mask)
				{
				case rt_variable_index_type:
					if (variables.collection[(item_idx >> rt_type_shift)].is_array()
						&& array_idx< variables.collection[(item_idx >> rt_type_shift)].get_size())
					{
						return variables.collection[item_idx >> rt_type_shift].get_item(array_idx)->browse_items(prefix_bellow, bellow, filter, items, ctx);
					}
					break;
				case rt_struct_index_type:
					if (structs.collection[(item_idx >> rt_type_shift)].is_array()
						&& array_idx < structs.collection[(item_idx >> rt_type_shift)].get_size())
					{
						return structs.collection[item_idx >> rt_type_shift].get_item(array_idx)->browse_items(prefix_bellow, bellow, filter, items, ctx);
					}
					break;
				default:
					one_attr.type = rx_attribute_type::invalid_attribute_type_type;
				}
				if (one_attr.type != rx_attribute_type::invalid_attribute_type_type)
				{
					if (prefix_bellow.empty())
						one_attr.full_path = one_attr.name;
					else
						one_attr.full_path = prefix_bellow + RX_OBJECT_DELIMETER + one_attr.name;

					items.emplace_back(std::move(one_attr));
				}
			}
		}
		return prefix + path + " not found!";
	}

}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
const runtime_item* runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_child_item (string_view_type path) const
{
	if (path.empty())
		return this;
	const runtime_item* ret = nullptr;
	// find our part
	string_view_type mine;
	string_view_type rest;
	auto in = path.find(RX_OBJECT_DELIMETER);
	if (in == string_view_type::npos)
	{
		mine = path;
	}
	else
	{
		mine = path.substr(0, in);
		rest = path.substr(in + 1);
	}
	int array_idx = -1;
	mine = extract_index(mine, array_idx);
	// now compare it
	for (const auto& one : this->items)
	{
		if (is_complex_index(one.index) && one.name == mine)
		{
			if (array_idx < 0)
			{
				switch (one.index & rt_type_mask)
				{
				case rt_variable_index_type:
					if(!variables.collection[one.index >> rt_type_shift].is_array())
						ret = variables.collection[one.index >> rt_type_shift].get_item()->item.get();
					break;
				case rt_struct_index_type:
					if (!structs.collection[one.index >> rt_type_shift].is_array())
						ret = structs.collection[one.index >> rt_type_shift].get_item()->item.get();
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
			else
			{
				switch (one.index & rt_type_mask)
				{
				case rt_variable_index_type:
					if (variables.collection[one.index >> rt_type_shift].is_array() && array_idx < variables.collection[one.index >> rt_type_shift].get_size())
						ret = variables.collection[one.index >> rt_type_shift].get_item(array_idx)->item.get();
					break;
				case rt_struct_index_type:
					if (structs.collection[one.index >> rt_type_shift].is_array() && array_idx < structs.collection[one.index >> rt_type_shift].get_size())
						ret = structs.collection[one.index >> rt_type_shift].get_item(array_idx)->item.get();
					break;
				}
			}
		}
		if (ret)
			break;
	}
	if (ret && !rest.empty())
	{
		// this is not all go bellow
		return ret->get_child_item(rest);
	}
	else
	{
		// this is our's return it
		return ret;
	}
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
		int array_idx = -1;
		auto idx = internal_get_index(mine, array_idx);
		if (idx && is_complex_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_variable_index_type:
				if (array_idx < 0 && !variables.collection[idx >> rt_type_shift].is_array())
				{
					return variables.collection[idx >> rt_type_shift].get_item()->get_local_value(bellow, val);
				}
				else if (array_idx >= 0 && array_idx < variables.collection[idx >> rt_type_shift].get_size())
				{
					return variables.collection[idx >> rt_type_shift].get_item(array_idx)->get_local_value(bellow, val);
				}
				else
				{
					return "Index out of bounds";
				}
			case rt_struct_index_type:
				if (array_idx < 0 && !structs.collection[idx >> rt_type_shift].is_array())
				{
					return structs.collection[idx >> rt_type_shift].get_item()->get_local_value(bellow, val);
				}
				else if (array_idx >= 0 && array_idx < structs.collection[idx >> rt_type_shift].get_size())
				{
					return structs.collection[idx >> rt_type_shift].get_item(array_idx)->get_local_value(bellow, val);
				}
				else
				{
					return "Index out of bounds";
				}
			case rt_source_index_type:
				return sources.collection[idx >> rt_type_shift].get_local_value(bellow, val);
			case rt_mapper_index_type:
				return mappers.collection[idx >> rt_type_shift].get_local_value(bellow, val);
			case rt_filter_index_type:
				return filters.collection[idx >> rt_type_shift].get_local_value(bellow, val);
			case rt_event_index_type:
				return events.collection[idx >> rt_type_shift].get_local_value(bellow, val);
			default:
				RX_ASSERT(false);
			}
		}
	}
	else// its' ours
	{
		int array_idx = -1;
		auto idx = internal_get_index(path, array_idx);
		if (idx && is_value_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_const_index_type:
				if (!const_values[idx >> rt_type_shift].is_array())
				{
					val = const_values[idx >> rt_type_shift].get_item()->simple_get_value();
					return true;
				}
				else if (array_idx < const_values[idx >> rt_type_shift].get_size())
				{
					val = const_values[idx >> rt_type_shift].get_item(array_idx)->simple_get_value();
					return true;
				}
				else
				{
					return "Index out of bounds";
				}
			case rt_value_index_type:
				if (!values[idx >> rt_type_shift].is_array())
				{
					val = values[idx >> rt_type_shift].get_item()->simple_get_value();
					return true;
				}
				else if (array_idx < values[idx >> rt_type_shift].get_size())
				{
					val = values[idx >> rt_type_shift].get_item(array_idx)->simple_get_value();
					return true;
				}
				else
				{
					return "Index out of bounds";
				}
			case rt_variable_index_type:
				if constexpr (has_variables())
				{
					if (!variables.collection[idx >> rt_type_shift].is_array())
					{
						val = variables.collection[idx >> rt_type_shift].get_item()->simple_get_value();
						return true;
					}
					else if (array_idx < variables.collection[idx >> rt_type_shift].get_size())
					{
						val = variables.collection[idx >> rt_type_shift].get_item(array_idx)->simple_get_value();
						return true;
					}
					else
					{
						return "Index out of bounds";
					}
				}
				break;
			default:
				RX_ASSERT(false);
			}
		}
	}
	return path + " not found!";
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
string_view_type runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::extract_index (string_view_type name, int& idx) const
{
	string_view_type ret;
	if (name.empty())
		return string_view_type();
	if (*name.rbegin() == ']')
	{
		size_t str_idx = name.rfind('[');
		if (str_idx != string_type::npos && str_idx < name.size() - 2)
		{
			uint16_t unsigned_idx;
			string_view_type number = name.substr(str_idx + 1, name.size() - str_idx /*We know this because of the upper condition*/ - 2);
			auto  result = std::from_chars(number.data(), number.data() + number.size(), unsigned_idx);
			if (result.ec != std::errc())
				return ret;//error
			idx = unsigned_idx;
			ret = name.substr(0, str_idx);
		}
		else
		{
			return string_view_type();
		}
	}
	else
	{
		idx = -1;
		ret = name;
	}
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
members_index_type runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::internal_get_index (string_view_type name, int& idx) const
{
	int item_idx = -1;
	name = extract_index(name, item_idx);
	if (name.empty())
		return 0;
	for (const auto& one : items)
	{
		if (one.name == name)
		{
			idx = item_idx;
			return one.index;
		}
	}
	return 0;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::is_value_index (members_index_type idx) const
{
	auto temp = idx & rt_type_mask;
	return temp == rt_const_index_type || temp == rt_value_index_type || temp == rt_variable_index_type
		|| temp == rt_source_index_type || temp == rt_mapper_index_type;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::is_complex_index (members_index_type idx) const
{
	auto temp = idx & rt_type_mask;
	return temp != rt_const_index_type && temp != rt_value_index_type && temp;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::is_this_yours (string_view_type path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	if (idx == string_type::npos)
	{
		int array_idx;
		return internal_get_index(path, array_idx) != 0;
	}
	else
	{
		int array_idx;
		return internal_get_index(path.substr(0, idx), array_idx) != 0;
	}
	return false;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::read_struct (string_view_type path, read_struct_data data) const
{
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::write_struct (string_view_type path, write_struct_data data)
{
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
	pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, write_task*> >();
}

variable_data::variable_data (runtime_item::smart_ptr&& rt, variable_runtime_ptr&& var, const variable_data& prototype)
	: item(std::move(rt))
	, variable_ptr(std::move(var))
{
	pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, write_task*> >();
}



void variable_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	if (type != runtime_value_type::persistent_runtime_value
		|| value_opt[runtime::structure::value_opt_persistent])
		data.add_value(RX_DEFAULT_VARIABLE_NAME, value.to_simple());
	item->collect_data(data, type);
}

void variable_data::fill_data (const data::runtime_values_data& data)
{
	auto val = data.get_value(RX_DEFAULT_VARIABLE_NAME);
	if (!val.is_null())
	{
		rx_value_t my_type = value.get_type();
		value = rx_value(val, rx_time::now());
		value.convert_to(my_type);
	}
	item->fill_data(data);
}

rx_value variable_data::get_value (runtime_process_context* ctx) const
{
	if (ctx)
		return ctx->adapt_value(value);
	else
		return value;
}

void variable_data::set_value (rx_simple_value&& val)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_value(std::move(val), rx_time::now());
	}
}

rx_result variable_data::write_value (write_data&& data, write_task* task, runtime_process_context* ctx)
{
	if (!data.value.convert_to(value.get_type()))
		return RX_INVALID_CONVERSION;
	auto& filters = item->get_filters();
	rx_result result;
	if (!filters.empty())
	{
		for (auto& filter : filters)
		{
			if (filter.is_output()
				&& !(result = filter.filter_output(data.value)))
				break;
		}
		if (!result)
		{
			result.register_error("Unable to filter write value.");
			return result;
		}
	}
	auto& sources = item->get_sources();
	auto new_trans = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
	pending_tasks_->emplace(new_trans, task);
	data.transaction_id = new_trans;
	result = variable_ptr->variable_write(std::move(data), ctx, sources);
	if (!result)
		pending_tasks_->erase(new_trans);

	return result;
}

rx_result variable_data::execute (execute_data&& data, execute_task* task, runtime_process_context* ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result variable_data::initialize_runtime (runtime::runtime_init_context& ctx)
{

	variable_ptr->container_ = this;
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

	auto prepared_value = variable_ptr->get_variable_input(ctx, sources);
	if (prepared_value.get_time().is_null())
		return;
	if (prepared_value.is_null())
	{
		rx_value temp_val(value);
		temp_val.set_quality(prepared_value.get_quality());
		temp_val.set_time(prepared_value.get_time());
		prepared_value = std::move(temp_val);
	}
	if (prepared_value.convert_to(value.get_type()) &&
		ctx->get_mode().can_callculate(prepared_value))
	{
		rx_result result;
		auto& filters = item->get_filters();
		if (!filters.empty())
		{
			for (auto& filter : filters)
			{
				if (filter.is_input())
				{
					result = filter.filter_input(prepared_value);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter read value.");
			}
		}
	}
	else
	{


	}
	if (!value.compare(prepared_value, time_compare_type::skip))
	{
		// value has changed
		value = prepared_value;

		// send subscription update
		ctx->variable_value_changed(this, prepared_value);
		// send update to mappers


		ctx->get_mode().can_callculate(prepared_value);
		{
			auto& mappers = item->get_mappers();
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
			it->second->process_result(std::move(result));
		pending_tasks_->erase(it);
	}
}

rx_result variable_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		val = get_value(ctx);
		return true;
	}
	else
	{
		return item->get_value(path, val, ctx);
	}
}

rx_result variable_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		ref.ref_type = rt_value_ref_type::rt_variable;
		ref.ref_value_ptr.variable = this;
		return true;
	}
	else
	{
		return item->get_value_ref(path, ref, true);
	}
}

rx_result variable_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* variable_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result variable_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

void variable_data::variable_result_pending (runtime_process_context* ctx, rx_result&& result, runtime_transaction_id_t id)
{
	if (ctx)
	{
		write_result_struct<structure::variable_data> data;
		data.whose = this;
		data.transaction_id = id;
		data.result = std::move(result);
		ctx->variable_result_pending(std::move(data));
	}
}

void variable_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}

rx_simple_value variable_data::simple_get_value () const
{
	return value.to_simple();
}


// Class rx_platform::runtime::structure::struct_data 

string_type struct_data::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_data::struct_data()
{
}

struct_data::struct_data (runtime_item::smart_ptr&& rt, struct_runtime_ptr&& var, const struct_data& prototype)
	: item(std::move(rt))
	, struct_ptr(std::move(var))
{
}



void struct_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void struct_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
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

rx_result struct_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	return item->get_value(path, val, ctx);
}

rx_result struct_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	return item->get_value_ref(path, ref, false);
}

rx_result struct_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* struct_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result struct_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

void struct_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}


// Class rx_platform::runtime::structure::mapper_data 

string_type mapper_data::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_data::mapper_data()
      : my_variable_(nullptr),
        context_(nullptr)
{
}

mapper_data::mapper_data (runtime_item::smart_ptr&& rt, mapper_runtime_ptr&& var, const mapper_data& prototype)
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

void mapper_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}

rx_result mapper_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	rx_value_t val_type=ctx.structure.get_current_item().get_local_as<rx_value_t>("ValueType", RX_NULL_TYPE);
	if(val_type!= RX_NULL_TYPE)
		mapped_value.value.convert_to(val_type);
	my_variable_ = ctx.variables.get_current_variable();
	if (mapped_value.value.get_type() == RX_NULL_TYPE && my_variable_)
		mapped_value.value.convert_to(my_variable_->value.get_type());
	mapper_ptr->value_type_ = mapped_value.value.get_type();
	mapper_ptr->container_ = this;
	ctx.mappers.push_mapper(mapper_id, this);
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = mapper_ptr->initialize_mapper(ctx);
	ctx.structure.pop_item();
	ctx.mappers.pop_mapper(mapper_id);
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
		auto value_type = mapped_value.value.get_type();
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
		rx_simple_value prepared_value = value.to_simple();
		auto quality = value.get_quality();
		auto& filters = item->get_filters();
		if (!filters.empty() && !value.is_null())
		{
			for (auto& filter : filters)
			{
				if (filter.is_output())
				{
					result = filter.filter_output(prepared_value);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter input value.");
				quality = RX_BAD_QUALITY_FAILURE;
			}
			if (!prepared_value.convert_to(mapper_ptr->value_type_))
				quality = RX_BAD_QUALITY_TYPE_MISMATCH;
			rx_value filtered_value = rx_value(std::move(prepared_value), value.get_time());
			filtered_value.set_quality(quality);
			mapped_value.set_value(filtered_value, context_);
			mapper_ptr->mapped_value_changed(std::move(filtered_value), context_);
		}
		else
		{
			if (!value.convert_to(mapper_ptr->value_type_))
				value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
			mapped_value.set_value(value, context_);
			mapper_ptr->mapped_value_changed(std::move(value), context_);
		}
	}
}

void mapper_data::process_write (write_data&& data)
{
	if (my_variable_)
	{
		auto trans_id = data.transaction_id;
		rx_value prepared_value = rx_value(std::move(data.value), rx_time());
		if (!prepared_value.convert_to(my_variable_->value.get_type()))
		{
			mapper_ptr->mapper_result_received(RX_INVALID_CONVERSION, trans_id, context_);
		}
		else
		{
			rx_result result;
			auto& filters = item->get_filters();
			if (!filters.empty())
			{
				for (auto& filter : filters)
				{
					if (filter.is_input())
					{
						result = filter.filter_input(prepared_value);
						if (!result)
							break;
					}
				}
				if (!result)
				{
					result.register_error("Unable to filter output value.");
				}
			}
			if (!result)
			{
				mapper_ptr->mapper_result_received(std::move(result), trans_id, context_);
			}
			else
			{
				auto task = new mapper_write_task(this, trans_id);
				data.value = prepared_value.to_simple();
				result = my_variable_->write_value(std::move(data), task, context_);
				if (!result)
				{
					mapper_ptr->mapper_result_received(std::move(result), trans_id, context_);
				}
			}
		}
	}
}

void mapper_data::mapper_write_pending (write_data&& data)
{
	write_data_struct<mapper_data> reg_data;
	reg_data.whose = this;
	if (data.identity == 0)
	{
		data.identity = rx_security_context();
	}
	reg_data.data = std::move(data);

	if (context_)
		context_->mapper_write_pending(std::move(reg_data));
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
		mapper_ptr->mapper_result_received(std::move(result), id, context_);
	}
}

rx_result mapper_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		val = get_value(ctx);
		return true;
	}
	else
	{
		return item->get_value(path, val, ctx);
	}
}

rx_result mapper_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		ref.ref_type = rt_value_ref_type::rt_full_value;
		ref.ref_value_ptr.full_value = &mapped_value;
		return true;
	}
	else
	{
		return item->get_value_ref(path, ref, false);
	}
}

rx_result mapper_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* mapper_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result mapper_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

rx_value mapper_data::get_value (runtime_process_context* ctx) const
{
	return mapped_value.get_value(ctx);
}

void mapper_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}


// Class rx_platform::runtime::structure::source_data 

string_type source_data::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_data::source_data()
      : my_variable_(nullptr),
        context_(nullptr)
{
}

source_data::source_data (runtime_item::smart_ptr&& rt, source_runtime_ptr&& var, const source_data& prototype)
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

void source_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}

rx_result source_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	rx_value_t val_type = ctx.structure.get_current_item().get_local_as<rx_value_t>("ValueType", RX_NULL_TYPE);
	if (val_type != RX_NULL_TYPE)
		input_value.value.convert_to(val_type);
	my_variable_ = ctx.variables.get_current_variable();
	if (input_value.value.get_type() == RX_NULL_TYPE && my_variable_)
		input_value.value.convert_to(my_variable_->value.get_type());
	source_ptr->value_type_ = input_value.value.get_type();
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
		auto value_type = input_value.value.get_type();
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

rx_result source_data::write_value (write_data&& data)
{
	if (!context_ || !source_ptr)
		return RX_ERROR_STOPPED;
	else if (!source_ptr->is_output())
		return RX_NOT_SUPPORTED;

	write_data_struct<source_data> reg_data;
	reg_data.whose = this;
	reg_data.data = std::move(data);

	context_->source_write_pending(std::move(reg_data));
	return true;
}

void source_data::process_update (values::rx_value&& value)
{
	RX_ASSERT(source_ptr);
	if (source_ptr && context_ && my_variable_)
	{
		if (value != current_value_)
		{
			input_value.set_value(value, context_);
			rx_result result;
			auto& filters = item->get_filters();
			if (!filters.empty())
			{
				for (auto& filter : filters)
				{
					if (filter.is_input())
					{
						result = filter.filter_input(value);
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

void source_data::process_write (write_data&& data)
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
					result = filter.filter_output(data.value);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter output value.");
			}
		}
		if (result)
		{
			result = source_ptr->source_write(std::move(data), context_);
		}
		if (!result)
			source_result_pending(std::move(result), data.transaction_id);
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

rx_result source_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		val = get_value(ctx);
		return true;
	}
	else
	{
		return item->get_value(path, val, ctx);
	}
}

rx_result source_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		ref.ref_type = rt_value_ref_type::rt_full_value;
		ref.ref_value_ptr.full_value = &input_value;
		return true;
	}
	else
	{
		return item->get_value_ref(path, ref, false);
	}
}

rx_result source_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* source_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result source_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

rx_value source_data::get_value (runtime_process_context* ctx) const
{
	return input_value.get_value(ctx);
}

void source_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}


// Class rx_platform::runtime::structure::event_data 

string_type event_data::type_name = RX_CPP_EVENT_TYPE_NAME;

event_data::event_data()
{
}

event_data::event_data (runtime_item::smart_ptr&& rt, event_runtime_ptr&& var, const event_data& prototype)
	: item(std::move(rt))
	, event_ptr(std::move(var))
	, arguments(std::move(prototype.arguments))
{
}



void event_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void event_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
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
	{
		result = item->deinitialize_runtime(ctx);
	}
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

rx_result event_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		return RX_INVALID_PATH;
	}
	else
	{
		auto idx = path.find(RX_OBJECT_DELIMETER);
		string_type sub_path;
		string_type rest_path;
		if (idx != string_type::npos)
		{
			sub_path = path.substr(0, idx);
			rest_path = path.substr(idx + 1);
		}
		else
		{
			sub_path = path;
		}
		if (sub_path == "Args")
		{
			return arguments.get_value( rest_path, val, ctx);
		}
		else
		{
			return item->get_value(path, val, ctx);
		}
	}
}

rx_result event_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		return RX_INVALID_PATH;
	}
	else
	{
		auto idx = path.find(RX_OBJECT_DELIMETER);
		string_view_type sub_path;
		string_view_type rest_path;
		if (idx != string_view_type::npos)
		{
			sub_path = path.substr(0, idx);
			rest_path = path.substr(idx + 1);
		}
		else
		{
			sub_path = path;
		}
		if (sub_path == "Args")
		{
			return arguments.get_value_ref(rest_path, ref, false);
		}
		else
		{
			return item->get_value_ref(path, ref, false);
		}
	}
}

rx_result event_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		runtime_item_attribute args_attr;
		args_attr.name = "Args";
		args_attr.type = rx_attribute_type::data_attribute_type;
		args_attr.full_path = prefix.empty() ? args_attr.name : prefix+RX_OBJECT_DELIMETER + args_attr.name;
		items.emplace_back(std::move(args_attr));
		return item->browse_items(prefix, path, filter, items, ctx);
	}
	else
	{
		auto idx = path.find(RX_OBJECT_DELIMETER);
		string_type sub_path;
		string_type rest_path;
		if (idx != string_type::npos)
		{
			sub_path = path.substr(0, idx);
			rest_path = path.substr(idx + 1);
		}
		else
		{
			sub_path = path;
		}
		if (sub_path == "Args")
		{
			string_type bellow_prefix = prefix.empty() ? sub_path : prefix + RX_OBJECT_DELIMETER + sub_path;
			return arguments.browse_items(bellow_prefix, rest_path, filter, items, ctx);
		}
		else
		{
			return item->browse_items(prefix, path, filter, items, ctx);
		}
	}
}

const runtime_item* event_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result event_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

void event_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}


// Class rx_platform::runtime::structure::filter_data 

string_type filter_data::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_data::filter_data()
      : context_(nullptr)
{
}

filter_data::filter_data (runtime_item::smart_ptr&& rt, filter_runtime_ptr&& var, const filter_data& prototype)
      : context_(nullptr)
	, item(std::move(rt))
	, filter_ptr(std::move(var))
{
}



void filter_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void filter_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}

rx_result filter_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	filter_ptr->container_ = this;
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
	{
		result = filter_ptr->initialize_filter(ctx);
	}
	ctx.structure.pop_item();
	return result;
}

rx_result filter_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = filter_ptr->deinitialize_filter(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	filter_ptr->container_ = nullptr;
	return result;
}

rx_result filter_data::start_runtime (runtime::runtime_start_context& ctx)
{
	context_ = ctx.context;
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
	context_ = nullptr;
	return result;
}

void filter_data::process_runtime (runtime_process_context* ctx)
{
}

rx_result filter_data::filter_output (rx_simple_value& val)
{
	return filter_ptr ? filter_ptr->filter_output(val) : RX_ERROR_STOPPED;
}

rx_result filter_data::filter_input (rx_value& val)
{
	return filter_ptr ? filter_ptr->filter_input(val) : RX_ERROR_STOPPED;
}

bool filter_data::is_input () const
{
	return filter_ptr ? filter_ptr->supports_input() && io_.get_input() : false;
}

bool filter_data::is_output () const
{
	return filter_ptr ? filter_ptr->supports_output() && io_.get_output() : false;
}

rx_result filter_data::get_value (runtime_handle_t handle, values::rx_simple_value& val) const
{
	if (context_)
	{
		return context_->get_value(handle, val);
	}
	else
	{
		RX_ASSERT(false);
		return "Context not binded!";
	}
}

rx_result filter_data::set_value (runtime_handle_t handle, values::rx_simple_value&& val)
{
	if (context_)
	{
		return context_->set_value(handle, std::move(val));
	}
	else
	{
		RX_ASSERT(false);
		return "Context not binded!";
	}
}

rx_result filter_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	return item->get_value(path, val, ctx);
}

rx_result filter_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	return item->get_value_ref(path, ref, false);
}

rx_result filter_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* filter_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result filter_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

rx_result filter_data::filter_changed ()
{
	return RX_NOT_IMPLEMENTED;
}

void filter_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
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
			return RX_INVALID_CONVERSION;
		}
	}
}


// Class rx_platform::runtime::structure::value_data 

string_type value_data::type_name = RX_VALUE_TYPE_NAME;


rx_value value_data::get_value (runtime_process_context* ctx) const
{
	if (value_opt[opt_state_ignorant])
	{
		static rx_mode_type on_mode;

		rx_value ret(value);
		value.get_value(ret, ctx->get_mode_time(), on_mode);
		return ret;
	}
	else
	{
		return ctx->adapt_value(value);
	}
}

void value_data::set_value (rx_simple_value&& val, const rx_time& time)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_timed_value(std::move(val), time);
	}
}

void value_data::object_state_changed (runtime_process_context* ctx)
{
	if (ctx->get_mode_time() > value.get_time())
		value.set_time(ctx->get_mode_time());
}

rx_result value_data::write_value (write_data&& data, runtime_process_context* ctx)
{
	if (value_opt[runtime::structure::value_opt_readonly] && !data.internal)
		return "Access Denied!";
	if (data.value.convert_to(value.get_type()))
	{
		value = rx_timed_value(std::move(data.value), rx_time::now());
		if (value_opt[runtime::structure::value_opt_persistent])
			ctx->runtime_dirty();
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

rx_result value_data::simple_set_value (rx_simple_value&& val, runtime_process_context* ctx)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_timed_value(std::move(val), rx_time::now());
		if (value_opt[runtime::structure::value_opt_persistent])
			ctx->runtime_dirty();
		return true;
	}
	else
	{
		return RX_INVALID_CONVERSION;
	}
}


// Class rx_platform::runtime::structure::runtime_item 


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
		value = rx_timed_value(std::move(val), time);
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
		return RX_INVALID_CONVERSION;
	return true;
}


// Class rx_platform::runtime::structure::write_task 

write_task::~write_task()
{
}



// Class rx_platform::runtime::structure::mapper_write_task 

mapper_write_task::mapper_write_task (mapper_data* my_mapper, runtime_transaction_id_t trans_id)
      : my_mapper_(my_mapper),
        transaction_id_(trans_id)
{
}



void mapper_write_task::process_result (rx_result&& result)
{
	my_mapper_->process_write_result(std::move(result), transaction_id_);
}


// Class rx_platform::runtime::structure::block_data 


void block_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	for (const auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
			case rt_const_index_type:
				// simple value
				{
					auto& this_val = values[(one.index >> rt_type_shift)];
					if (this_val.is_array())
					{
						std::vector<rx_simple_value> vals;
						for (int i = 0; i < this_val.get_size(); i++)
							vals.push_back(this_val.get_item(i)->value);
						data.add_value(one.name, std::move(vals));
					}
					else
					{
						data.add_value(one.name, this_val.get_item()->value);
					}
				}
				break;
			case rt_data_index_type:
				{
					auto& this_val = children[(one.index >> rt_type_shift)];
					if (this_val.is_array())
					{
						std::vector<data::runtime_values_data> childs;
						for (int i = 0; i < this_val.get_size(); i++)
						{
							data::runtime_values_data child_data;
							this_val.get_item(i)->collect_data(child_data, type);
							if (!child_data.empty())
								childs.push_back(std::move(child_data));
						}
						data.add_array_child(one.name, std::move(childs));
					}
					else
					{
						data::runtime_values_data child_data;
						this_val.get_item()->collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
					}
				}
				break;
			default:
				RX_ASSERT(false);
		}
	}
}

void block_data::fill_data (const data::runtime_values_data& data)
{
	for (auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			{// simple value
				if (!values[one.index >> rt_type_shift].is_array())
				{
					auto val = data.get_value(one.name);
					if (!val.is_null())
					{
						values[one.index >> rt_type_shift].get_item()->set_value(std::move(val));
					}
				}
				else
				{
					std::vector<rx_simple_value> vals;
					if (data.get_array_value(one.name, vals))
					{
						size_t dim = std::min<size_t>(vals.size(), values[(one.index >> rt_type_shift)].get_size());
						for (size_t i = 0; i < dim; i++)
						{
							values[one.index >> rt_type_shift].get_item((int)i)->set_value(std::move(vals[i]));
						}
					}
				}
			}
			break;
		case rt_data_index_type:
			{// child value
				if (!children[(one.index >> rt_type_shift)].is_array())
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end())
					{
						if (std::holds_alternative<data::runtime_values_data>(it->second))
						{
							children[one.index >> rt_type_shift].get_item()->fill_data(
								std::get< data::runtime_values_data>(it->second));
						}
					}
				}
				else
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end())
					{
						if (std::holds_alternative<std::vector<data::runtime_values_data> >(it->second))
						{
							auto& childs = std::get<std::vector<data::runtime_values_data>>(it->second);
							size_t dim = std::min<size_t>(childs.size(), children[(one.index >> rt_type_shift)].get_size());
							for (size_t i = 0; i < dim; i++)
							{
								children[one.index >> rt_type_shift].get_item((int)i)->fill_data(childs[i]);
							}
						}
					}
				}
			}
			break;
		default:
			RX_ASSERT(false);
		}
	}
}

rx_result block_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{// our value
		rx_result result;
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
			writer.write_footer();
			string_type temp_str(writer.get_string());
			if (!temp_str.empty())
			{
				val.assign_static(temp_str.c_str(), ctx->now);
				result = true;
			}
			else
			{
				result = "Retrieving string from JSON stream";
			}
		}
		return result;
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
			int array_idx = -1;
			auto idx = internal_get_index(mine, array_idx);
			if (idx && is_complex_index(idx))
			{
				switch (idx & rt_type_mask)
				{
				case rt_data_index_type:
					if (array_idx < 0)
					{
						if (!children[idx >> rt_type_shift].is_array())
							return children[idx >> rt_type_shift].get_item()->get_value(bellow, val, ctx);
						else
							return "Not an array!";
					}
					else
					{
						if (!values[idx >> rt_type_shift].is_array())
							return "Not an array";
						else if (array_idx < values[idx >> rt_type_shift].get_size())
							return children[idx >> rt_type_shift].get_item(array_idx)->get_value(bellow, val, ctx);
						else
							return "Out of bounds!";
					}
				default:
					RX_ASSERT(false);
				}
			}
		}
		else// its' ours
		{
			int array_idx = -1;
			auto idx = internal_get_index(path, array_idx);
			if (idx && is_value_index(idx))
			{
				switch (idx & rt_type_mask)
				{
				case rt_const_index_type:
					if (array_idx < 0)
					{
						if (!values[idx >> rt_type_shift].is_array())
						{
							val = values[idx >> rt_type_shift].get_item()->get_value(ctx);
							return true;
						}
						else
						{
							return "Not an array!";
						}

					}
					else
					{
						if (!values[idx >> rt_type_shift].is_array())
						{
							return "Not an array";
						}
						else if (array_idx < values[idx >> rt_type_shift].get_size())
						{
							val = values[idx >> rt_type_shift].get_item(array_idx)->get_value(ctx);
							return true;
						}
						else
						{
							return "Out of bounds!";
						}
					}
				default:
						RX_ASSERT(false);
				}
			}
		}
		return mine + " not found!";
	}
}

void block_data::object_state_changed (runtime_process_context* ctx)
{
	// nothing to do for data
}

rx_result block_data::get_value_ref (string_view_type path, rt_value_ref& ref, bool is_var)
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_view_type mine;
	if (idx != string_view_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_view_type bellow = path.substr(idx + 1);
		int array_idx = -1;
		auto idx = internal_get_index(mine, array_idx);
		if (idx && is_complex_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_const_index_type:
				if (array_idx < 0)
				{
					if (!children[idx >> rt_type_shift].is_array())
						return children[idx >> rt_type_shift].get_item()->get_value_ref(bellow, ref, false);
					else
						return "Invalid array index";
				}
				else
				{
					if (!children[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < children[idx >> rt_type_shift].get_size())
						return children[idx >> rt_type_shift].get_item(array_idx)->get_value_ref(bellow, ref, false);
					else
						return "Out of bounds!";
				}
			default:
				RX_ASSERT(false);
			}
		}
	}
	else// its' ours
	{
		int array_idx = -1;
		auto idx = internal_get_index(path, array_idx);
		if (idx && is_value_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_const_index_type:
				if (array_idx < 0)
				{
					ref.ref_type = rt_value_ref_type::rt_const_value;
					if (!values[idx >> rt_type_shift].is_array())
						ref.ref_value_ptr.const_value = values[idx >> rt_type_shift].get_item();
					else
						return "Invalid array index";
				}
				else
				{
					ref.ref_type = rt_value_ref_type::rt_const_value;
					if (!values[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < values[idx >> rt_type_shift].get_size())
						ref.ref_value_ptr.const_value = values[idx >> rt_type_shift].get_item(array_idx);
					else
						return "Out of bounds!";
				}
				return true;
			default:
				RX_ASSERT(false);// has to be because of is_value_index;
			}
		}
	}
	return string_type(path) + " not found!";
}

rx_result block_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		for (const auto& one : this->items)
		{
			runtime_item_attribute one_attr;
			switch (one.index & rt_type_mask)
			{
			case rt_const_index_type:
				one_attr.name = one.name;
				if (!values[(one.index >> rt_type_shift)].is_array())
				{
					one_attr.value = values[(one.index >> rt_type_shift)].get_item()->get_value(ctx);
					one_attr.type = rx_attribute_type::const_attribute_type;
				}
				else
				{
					one_attr.type = rx_attribute_type::const_array_attribute_type;
				}
				break;
			case rt_data_index_type:
				one_attr.name = one.name;
				if (!values[(one.index >> rt_type_shift)].is_array())
					one_attr.type = rx_attribute_type::data_attribute_type;
				else
					one_attr.type = rx_attribute_type::data_array_attribute_type;
				break;
			default:
				RX_ASSERT(false);
				one_attr.type = rx_attribute_type::invalid_attribute_type_type;
			}
			if (one_attr.type != rx_attribute_type::invalid_attribute_type_type)
			{
				one_attr.full_path = prefix + path + RX_OBJECT_DELIMETER + one.name;
				items.emplace_back(std::move(one_attr));
			}
		}
		return true;
	}
	else
	{
		size_t idx = path.find(RX_OBJECT_DELIMETER);
		string_type mine;
		string_type bellow;
		if (idx != string_type::npos)
		{// bellow us, split it
			mine = path.substr(0, idx);
			bellow = path.substr(idx + 1);
		}
		else
		{
			mine = path;
		}
		int array_idx = -1;
		auto item_idx = internal_get_index(mine, array_idx);
		if (item_idx)
		{
			string_type prefix_bellow = prefix.empty() ? mine : prefix + RX_OBJECT_DELIMETER + mine;
			switch (item_idx & rt_type_mask)
			{
			case rt_data_index_type:
				if (children[(item_idx >> rt_type_shift)].is_array())
				{
					if (bellow.empty())
					{// browse array
						for (int i = 0; i < children[(item_idx >> rt_type_shift)].get_size(); i++)
						{
							char temp_buff[0x20];
							runtime_item_attribute one_attr;
							sprintf(temp_buff, "[%d]", i);
							one_attr.name = mine + temp_buff;
							one_attr.type = rx_attribute_type::data_attribute_type;
							if (prefix.empty())
								one_attr.full_path = one_attr.name;
							else
								one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
							items.emplace_back(std::move(one_attr));
						}
					}
					else
					{// browse bellow
						return children[item_idx >> rt_type_shift].get_item(array_idx)->browse_items(prefix_bellow, bellow, filter, items, ctx);
					}
				}
				else
				{
					return children[item_idx >> rt_type_shift].get_item(0)->browse_items(prefix_bellow, bellow, filter, items, ctx);
				}
			case rt_const_index_type:
				if (values[(item_idx >> rt_type_shift)].is_array())
				{
					for (int i = 0; i < values[(item_idx >> rt_type_shift)].get_size(); i++)
					{
						char temp_buff[0x20];
						runtime_item_attribute one_attr;
						sprintf(temp_buff, "[%d]", i);
						one_attr.name = mine + temp_buff;
						one_attr.type = rx_attribute_type::const_attribute_type;
						one_attr.value = values[(item_idx >> rt_type_shift)].get_item(i)->get_value(ctx);
						if (prefix.empty())
							one_attr.full_path = one_attr.name;
						else
							one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
						items.emplace_back(std::move(one_attr));
					}
					return true;
				}
				break;
			default:
				RX_ASSERT(false);

			}
		}
		return prefix + path + " not found!";
	}
}

const runtime_item* block_data::get_child_item (string_view_type path) const
{
	if (path.empty())
		return this;

	const runtime_item* ret = nullptr;
	string_view_type mine;
	string_view_type rest;
	auto in = path.find(RX_OBJECT_DELIMETER);
	if (in == string_view_type::npos)
	{
		mine = path;
	}
	else
	{
		mine = path.substr(0, in);
		rest = path.substr(in + 1);
	}
	int array_idx = -1;
	mine = extract_index(mine, array_idx);
	// now compare it
	for (const auto& one : this->items)
	{
		if (is_complex_index(one.index) && one.name == mine)
		{
			switch (one.index & rt_type_mask)
			{
				case rt_data_index_type:
					ret = children[one.index >> rt_type_shift].get_item();
					break;
				default:
					RX_ASSERT(false);
			}
		}
		if (ret)
			break;
	}
	if (ret && !rest.empty())
	{
		// this is not all go bellow
		return ret->get_child_item(rest);
	}
	else
	{
		// this is our's return it
		return ret;
	}
}

rx_result block_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;// nothing to do for data
}

rx_result block_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;// nothing to do for data
}

rx_result block_data::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;// nothing to do for data
}

rx_result block_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;// nothing to do for data
}

runtime_filters_type& block_data::get_filters ()
{
	return g_empty_filters;
}

runtime_sources_type& block_data::get_sources ()
{
	return g_empty_sources;
}

runtime_mappers_type& block_data::get_mappers ()
{
	return g_empty_mappers;
}

runtime_events_type& block_data::get_events ()
{
	return g_empty_events;
}

runtime_variables_type& block_data::get_variables ()
{
	return g_empty_variables;
}

runtime_structs_type& block_data::get_structs ()
{
	return g_empty_structs;
}

rx_result block_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		int array_idx = -1;
		auto idx = internal_get_index(mine, array_idx);
		if (idx && is_complex_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_data_index_type:
				if (array_idx < 0 && !children[idx >> rt_type_shift].is_array())
				{
					return children[idx >> rt_type_shift].get_item()->get_local_value(bellow, val);
				}
				else if (array_idx >= 0 && array_idx < children[idx >> rt_type_shift].get_size())
				{
					return children[idx >> rt_type_shift].get_item(array_idx)->get_local_value(bellow, val);
				}
				else
				{
					return "Index out of bounds";
				}

			default:
				RX_ASSERT(false);
			}
		}
	}
	else// its' ours
	{
		int array_idx = -1;
		auto idx = internal_get_index(path, array_idx);
		if (idx && is_value_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_const_index_type:
				if (array_idx < 0 && !values[idx >> rt_type_shift].is_array())
				{
					val = values[idx >> rt_type_shift].get_item()->simple_get_value();
					return true;
				}
				else if (array_idx >=0 && array_idx < values[idx >> rt_type_shift].get_size())
				{
					val = values[idx >> rt_type_shift].get_item(array_idx)->simple_get_value();
					return true;
				}
				else
				{
					return "Index out of bounds";
				}
			default:
				RX_ASSERT(false);
			}
		}
	}
	return path + " not found!";
}

string_view_type block_data::extract_index (string_view_type name, int& idx) const
{
	string_view_type ret;
	if (name.empty())
		return string_view_type();
	if (*name.rbegin() == ']')
	{
		size_t str_idx = name.rfind('[');
		if (str_idx != string_type::npos && str_idx < name.size() - 2)
		{
			uint16_t unsigned_idx;
			string_view_type number = name.substr(str_idx + 1, name.size() - str_idx /*We know this because of the upper condition*/ - 2);
			auto  result = std::from_chars(number.data(), number.data() + number.size(), unsigned_idx);
			if (result.ec != std::errc())
				return ret;//error
			idx = unsigned_idx;
			ret = name.substr(0, str_idx);
		}
		else
		{
			return string_view_type();
		}
	}
	else
	{
		idx = -1;
		ret = name;
	}
	return ret;
}

members_index_type block_data::internal_get_index (string_view_type name, int& idx) const
{
	int item_idx = -1;
	name = extract_index(name, item_idx);
	if (name.empty())
		return 0;
	for (const auto& one : items)
	{
		if (one.name == name)
		{
			idx = item_idx;
			return one.index;
		}
	}
	return 0;
}

bool block_data::is_value_index (members_index_type idx) const
{
	return (idx & rt_type_mask) == rt_const_index_type;
}

bool block_data::is_complex_index (members_index_type idx) const
{
	return (idx & rt_type_mask) == rt_data_index_type;
}

bool block_data::is_this_yours (string_view_type path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	int array_idx = -1;
	if (idx == string_type::npos)
	{
		return internal_get_index(path, array_idx) != 0;
	}
	else
	{
		return internal_get_index(path.substr(0, idx), array_idx) != 0;
	}
	return false;
}

void block_data::read_struct (string_view_type path, read_struct_data data) const
{
}

void block_data::write_struct (string_view_type path, write_struct_data data)
{
}

rx_result block_data::create_safe_runtime_data (const data::runtime_values_data& in, data::runtime_values_data& out)
{
	for (const auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			{
				if (!values[(one.index >> rt_type_shift)].is_array())
				{
					auto val = in.get_value(one.name);
					if (!val.is_null())
					{
						return "Missing argument "s + one.name;
					}
					string_type temp_name = one.name;

					auto val_type = values[(one.index >> rt_type_shift)].get_item()->value.get_type();
					if (!val.convert_to(val_type))
					{
						return RX_INVALID_CONVERSION " for "s + one.name;
					}
					out.add_value(one.name, std::move(val));
				}
				else
				{
					return RX_NOT_IMPLEMENTED;
				}
			}
			break;
		case rt_data_index_type:
			return RX_NOT_IMPLEMENTED;
		default:
			RX_ASSERT(false);
			return RX_INTERNAL_ERROR;
		}
	}
	return true;
}


// Class rx_platform::runtime::structure::full_value_data 


rx_value full_value_data::get_value (runtime_process_context* ctx) const
{
	if (ctx)
		return ctx->adapt_value(value);
	else
		return value;
}

void full_value_data::set_value (const rx_value& val, runtime_process_context* ctx)
{
	rx_value temp(val);
	if (temp.convert_to(value.get_type()))
	{
		value = std::move(temp);
		ctx->full_value_changed(this);
	}
}

rx_simple_value full_value_data::simple_get_value () const
{
	return value.to_simple();
}


// Class rx_platform::runtime::structure::execute_task 

execute_task::~execute_task()
{
}



// Parameterized Class rx_platform::runtime::structure::array_wrapper 


} // namespace structure
} // namespace runtime
} // namespace rx_platform

