

/****************************************************************************
*
*  system\runtime\rx_rt_struct.cpp
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


// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"

#include "rx_blocks.h"


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


// Parameterized Class rx_platform::runtime::structure::empty 


// Parameterized Class rx_platform::runtime::structure::has 


// Class rx_platform::runtime::structure::variable_data 

string_type variable_data::type_name = RX_CPP_VARIABLE_TYPE_NAME;


void variable_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void variable_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}


// Class rx_platform::runtime::structure::struct_data 

string_type struct_data::type_name = RX_CPP_STRUCT_TYPE_NAME;


void struct_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void struct_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}


// Class rx_platform::runtime::structure::mapper_data 

string_type mapper_data::type_name = RX_CPP_MAPPER_TYPE_NAME;


void mapper_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void mapper_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}


// Class rx_platform::runtime::structure::source_data 

string_type source_data::type_name = RX_CPP_SOURCE_TYPE_NAME;


void source_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void source_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}


// Class rx_platform::runtime::structure::event_data 

string_type event_data::type_name = RX_CPP_EVENT_TYPE_NAME;


void event_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void event_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}


// Class rx_platform::runtime::structure::filter_data 

string_type filter_data::type_name = RX_CPP_FILTER_TYPE_NAME;


void filter_data::collect_data (data::runtime_values_data& data) const
{
	item->collect_data(data);
}

void filter_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}


// Class rx_platform::runtime::structure::const_value_data 

string_type const_value_data::type_name = RX_CONST_VALUE_TYPE_NAME;


rx_value const_value_data::get_value (const hosting_object_data& state) const
{
	rx_value ret;
	value.get_value(ret, state.time, state.mode);
	return ret;
}


// Class rx_platform::runtime::structure::value_data 

string_type value_data::type_name = RX_VALUE_TYPE_NAME;


rx_value value_data::get_value (const hosting_object_data& state) const
{
	rx_value ret;
	value.get_value(ret, state.time, state.mode);
	return ret;
}


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
		default:
			RX_ASSERT(false);
		}
	}
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::fill_data (const data::runtime_values_data& data)
{
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
rx_value runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::get_value (const hosting_object_data& state, const string_type& path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto idx = internal_get_index(mine);
		if (idx && !is_value_index(idx))
		{
			switch (idx&rt_type_mask)
			{
			case rt_variable_index_type:
				return variables.collection[idx >> rt_type_shift].item->get_value(state, bellow);
			case rt_struct_index_type:
				return structs.collection[idx >> rt_type_shift].item->get_value(state, bellow);
			case rt_source_index_type:
				return sources.collection[idx >> rt_type_shift].item->get_value(state, bellow);
			case rt_mapper_index_type:
				return mappers.collection[idx >> rt_type_shift].item->get_value(state, bellow);
			case rt_filter_index_type:
				return filters.collection[idx >> rt_type_shift].item->get_value(state, bellow);
			case rt_event_index_type:
				return events.collection[idx >> rt_type_shift].item->get_value(state, bellow);
			}
		}
	}
	else// its' ours
	{
		auto idx = internal_get_index(mine);
		if (idx && is_value_index(idx))
		{
			switch (idx&rt_type_mask)
			{
			case rt_const_index_type:
				return const_values[idx >> rt_type_shift].get_value(state);
			case rt_value_index_type:
				return values[idx >> rt_type_shift].get_value(state);
			}
		}
	}

	rx_value ret;
	ret.set_time(rx_time::now());
	ret.set_quality(RX_BAD_QUALITY_CONFIG_ERROR);
	return ret;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
void runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::object_state_changed (const hosting_object_data& state)
{
	for (auto& one : variables.collection)
		one.item->object_state_changed(state);
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
	return temp == rt_const_index_type || temp == rt_value_index_type;
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

// Class rx_platform::runtime::structure::hosting_object_data 


// Class rx_platform::runtime::structure::runtime_item 


} // namespace structure
} // namespace runtime
} // namespace rx_platform

