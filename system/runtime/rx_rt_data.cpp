

/****************************************************************************
*
*  system\runtime\rx_rt_data.cpp
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


// rx_rt_data
#include "system/runtime/rx_rt_data.h"



namespace rx_platform {

namespace runtime {

namespace blocks {

namespace data {


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


// Parameterized Class rx_platform::runtime::blocks::data::has 


// Parameterized Class rx_platform::runtime::blocks::data::empty 


// Parameterized Class rx_platform::runtime::blocks::data::runtime_data 


template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_array("Items", items.size()))
		return false;
	for (const auto& one : items)
	{
		switch (one.index&rt_type_mask)
		{
		case rt_const_index_type:
		{// const value
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", const_value_data::type_name.c_str()))
				return false;
			if (!stream.write_string("Name", one.name.c_str()))
				return false;
			if (!stream.start_object("Val"))
				return false;
			const_values[(one.index>>rt_type_shift)].value.serialize(stream);
			if (!stream.end_object())//Val
				return false;
			if (!stream.end_object())//Item
				return false;
		}
		break;
		case rt_value_index_type:
		{// simple value
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", value_data::type_name.c_str()))
				return false;
			if (!stream.write_string("Name", one.name.c_str()))
				return false;
			if (!stream.start_object("Val"))
				return false;
			values[(one.index >> rt_type_shift)].value.serialize(stream);
			if (!stream.end_object())//Val
				return false;
			if (!stream.end_object())//Item
				return false;
		}
		case rt_variable_index_type:
		{// simple value
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", variable_data::type_name.c_str()))
				return false;
			if (!stream.write_string("Name", one.name.c_str()))
				return false;
			if (!stream.start_object("Val"))
				return false;
			variables.collection[(one.index >> rt_type_shift)].serialize(stream, type, ts, mode);
			if (!stream.end_object())//Val
				return false;
			if (!stream.end_object())//Item
				return false;
		}
		case rt_struct_index_type:
		{// simple value
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", struct_data::type_name.c_str()))
				return false;
			if (!stream.write_string("Name", one.name.c_str()))
				return false;
			if (!stream.start_object("Val"))
				return false;
			structs.collection[(one.index >> rt_type_shift)].serialize(stream, type, ts, mode);
			if (!stream.end_object())//Val
				return false;
			if (!stream.end_object())//Item
				return false;
		}
		break;
		default:
			RX_ASSERT(false);
		}
	}
	if (!stream.end_array())
		return false;

	return true;
}

template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
bool runtime_data<variables_type,structs_type,sources_type,mappers_type,filters_type,events_type,type_id>::deserialize (base_meta_reader& stream, uint8_t type)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// exact constant-named runtime data types
typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x00> runtime_data_type00;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x01> runtime_data_type01;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x02> runtime_data_type02;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x03> runtime_data_type03;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x04> runtime_data_type04;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x05> runtime_data_type05;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x06> runtime_data_type06;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x07> runtime_data_type07;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x08> runtime_data_type08;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x09> runtime_data_type09;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x0a> runtime_data_type0a;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x0b> runtime_data_type0b;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x0c> runtime_data_type0c;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x0d> runtime_data_type0d;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x0e> runtime_data_type0e;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x0f> runtime_data_type0f;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x10> runtime_data_type10;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x11> runtime_data_type11;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x12> runtime_data_type12;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x13> runtime_data_type13;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x14> runtime_data_type14;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x15> runtime_data_type15;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x16> runtime_data_type16;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x17> runtime_data_type17;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x18> runtime_data_type18;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x19> runtime_data_type19;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x1a> runtime_data_type1a;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x1b> runtime_data_type1b;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x1c> runtime_data_type1c;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x1d> runtime_data_type1d;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x1e> runtime_data_type1e;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x1f> runtime_data_type1f;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x20> runtime_data_type20;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x21> runtime_data_type21;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x22> runtime_data_type22;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x23> runtime_data_type23;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x24> runtime_data_type24;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x25> runtime_data_type25;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x26> runtime_data_type26;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x27> runtime_data_type27;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x28> runtime_data_type28;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x29> runtime_data_type29;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x2a> runtime_data_type2a;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x2b> runtime_data_type2b;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x2c> runtime_data_type2c;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x2d> runtime_data_type2d;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x2e> runtime_data_type2e;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>, 0x2f> runtime_data_type2f;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x30> runtime_data_type30;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x31> runtime_data_type31;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x32> runtime_data_type32;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x33> runtime_data_type33;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x34> runtime_data_type34;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x35> runtime_data_type35;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x36> runtime_data_type36;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x37> runtime_data_type37;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x38> runtime_data_type38;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x39> runtime_data_type39;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3a> runtime_data_type3a;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3b> runtime_data_type3b;

typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3c> runtime_data_type3c;

typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3d> runtime_data_type3d;

typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3e> runtime_data_type3e;

typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3f> runtime_data_type3f;
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class runtime_data_type>
func::runtime_item::smart_ptr create_runtime_data_from_prototype(runtime_data_prototype& prototype)
{
	std::unique_ptr<runtime_data_type> ret = std::make_unique<runtime_data_type>();
	if (runtime_data_type::has_variables())
		ret->variables.copy_from(std::move(prototype.variables));
	if (runtime_data_type::has_structs())
		ret->structs.copy_from(std::move(prototype.structs));
	if (runtime_data_type::has_sources())
		ret->sources.copy_from(std::move(prototype.sources));
	if (runtime_data_type::has_mappers())
		ret->mappers.copy_from(std::move(prototype.mappers));
	if (runtime_data_type::has_filters())
		ret->filters.copy_from(std::move(prototype.filters));
	if (runtime_data_type::has_events())
		ret->events.copy_from(std::move(prototype.events));

	ret->values = const_size_vector<value_data>(std::move(prototype.values));
	ret->const_values = const_size_vector<const_value_data>(std::move(prototype.const_values));
	ret->items = const_size_vector<index_data>(std::move(prototype.items));

	return ret;
}


func::runtime_item::smart_ptr create_runtime_data(uint_fast8_t type_id)
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
	return func::runtime_item::smart_ptr();
}

func::runtime_item::smart_ptr create_runtime_data(runtime_data_prototype& prototype)
{
	uint_fast8_t effective_type = (prototype.variables.empty() ? rt_bit_none : rt_bit_has_variables)
		| (prototype.structs.empty() ? rt_bit_none : rt_bit_has_structs)
		| (prototype.sources.empty() ? rt_bit_none : rt_bit_has_sources)
		| (prototype.mappers.empty() ? rt_bit_none : rt_bit_has_mappers)
		| (prototype.filters.empty() ? rt_bit_none : rt_bit_has_filters)
		| (prototype.events.empty() ? rt_bit_none : rt_bit_has_events);

	switch (effective_type)
	{
	case 0x00:
		return create_runtime_data_from_prototype<runtime_data_type00>(prototype);
	case 0x01:
		return create_runtime_data_from_prototype<runtime_data_type01>(prototype);
	case 0x02:
		return create_runtime_data_from_prototype<runtime_data_type02>(prototype);
	case 0x03:
		return create_runtime_data_from_prototype<runtime_data_type03>(prototype);
	case 0x04:
		return create_runtime_data_from_prototype<runtime_data_type04>(prototype);
	case 0x05:
		return create_runtime_data_from_prototype<runtime_data_type05>(prototype);
	case 0x06:
		return create_runtime_data_from_prototype<runtime_data_type06>(prototype);
	case 0x07:
		return create_runtime_data_from_prototype<runtime_data_type07>(prototype);
	case 0x08:
		return create_runtime_data_from_prototype<runtime_data_type08>(prototype);
	case 0x09:
		return create_runtime_data_from_prototype<runtime_data_type09>(prototype);
	case 0x0a:
		return create_runtime_data_from_prototype<runtime_data_type0a>(prototype);
	case 0x0b:
		return create_runtime_data_from_prototype<runtime_data_type0b>(prototype);
	case 0x0c:
		return create_runtime_data_from_prototype<runtime_data_type0c>(prototype);
	case 0x0d:
		return create_runtime_data_from_prototype<runtime_data_type0d>(prototype);
	case 0x0e:
		return create_runtime_data_from_prototype<runtime_data_type0e>(prototype);
	case 0x0f:
		return create_runtime_data_from_prototype<runtime_data_type0f>(prototype);
	case 0x10:
		return create_runtime_data_from_prototype<runtime_data_type10>(prototype);
	case 0x11:
		return create_runtime_data_from_prototype<runtime_data_type11>(prototype);
	case 0x12:
		return create_runtime_data_from_prototype<runtime_data_type12>(prototype);
	case 0x13:
		return create_runtime_data_from_prototype<runtime_data_type13>(prototype);
	case 0x14:
		return create_runtime_data_from_prototype<runtime_data_type14>(prototype);
	case 0x15:
		return create_runtime_data_from_prototype<runtime_data_type15>(prototype);
	case 0x16:
		return create_runtime_data_from_prototype<runtime_data_type16>(prototype);
	case 0x17:
		return create_runtime_data_from_prototype<runtime_data_type17>(prototype);
	case 0x18:
		return create_runtime_data_from_prototype<runtime_data_type18>(prototype);
	case 0x19:
		return create_runtime_data_from_prototype<runtime_data_type19>(prototype);
	case 0x1a:
		return create_runtime_data_from_prototype<runtime_data_type1a>(prototype);
	case 0x1b:
		return create_runtime_data_from_prototype<runtime_data_type1b>(prototype);
	case 0x1c:
		return create_runtime_data_from_prototype<runtime_data_type1c>(prototype);
	case 0x1d:
		return create_runtime_data_from_prototype<runtime_data_type1d>(prototype);
	case 0x1e:
		return create_runtime_data_from_prototype<runtime_data_type1e>(prototype);
	case 0x1f:
		return create_runtime_data_from_prototype<runtime_data_type1f>(prototype);
	case 0x20:
		return create_runtime_data_from_prototype<runtime_data_type20>(prototype);
	case 0x21:
		return create_runtime_data_from_prototype<runtime_data_type21>(prototype);
	case 0x22:
		return create_runtime_data_from_prototype<runtime_data_type22>(prototype);
	case 0x23:
		return create_runtime_data_from_prototype<runtime_data_type23>(prototype);
	case 0x24:
		return create_runtime_data_from_prototype<runtime_data_type24>(prototype);
	case 0x25:
		return create_runtime_data_from_prototype<runtime_data_type25>(prototype);
	case 0x26:
		return create_runtime_data_from_prototype<runtime_data_type26>(prototype);
	case 0x27:
		return create_runtime_data_from_prototype<runtime_data_type27>(prototype);
	case 0x28:
		return create_runtime_data_from_prototype<runtime_data_type28>(prototype);
	case 0x29:
		return create_runtime_data_from_prototype<runtime_data_type29>(prototype);
	case 0x2a:
		return create_runtime_data_from_prototype<runtime_data_type2a>(prototype);
	case 0x2b:
		return create_runtime_data_from_prototype<runtime_data_type2b>(prototype);
	case 0x2c:
		return create_runtime_data_from_prototype<runtime_data_type2c>(prototype);
	case 0x2d:
		return create_runtime_data_from_prototype<runtime_data_type2d>(prototype);
	case 0x2e:
		return create_runtime_data_from_prototype<runtime_data_type2e>(prototype);
	case 0x2f:
		return create_runtime_data_from_prototype<runtime_data_type2f>(prototype);
	case 0x30:
		return create_runtime_data_from_prototype<runtime_data_type30>(prototype);
	case 0x31:
		return create_runtime_data_from_prototype<runtime_data_type31>(prototype);
	case 0x32:
		return create_runtime_data_from_prototype<runtime_data_type32>(prototype);
	case 0x33:
		return create_runtime_data_from_prototype<runtime_data_type33>(prototype);
	case 0x34:
		return create_runtime_data_from_prototype<runtime_data_type34>(prototype);
	case 0x35:
		return create_runtime_data_from_prototype<runtime_data_type35>(prototype);
	case 0x36:
		return create_runtime_data_from_prototype<runtime_data_type36>(prototype);
	case 0x37:
		return create_runtime_data_from_prototype<runtime_data_type37>(prototype);
	case 0x38:
		return create_runtime_data_from_prototype<runtime_data_type38>(prototype);
	case 0x39:
		return create_runtime_data_from_prototype<runtime_data_type39>(prototype);
	case 0x3a:
		return create_runtime_data_from_prototype<runtime_data_type3a>(prototype);
	case 0x3b:
		return create_runtime_data_from_prototype<runtime_data_type3b>(prototype);
	case 0x3c:
		return create_runtime_data_from_prototype<runtime_data_type3c>(prototype);
	case 0x3d:
		return create_runtime_data_from_prototype<runtime_data_type3d>(prototype);
	case 0x3e:
		return create_runtime_data_from_prototype<runtime_data_type3e>(prototype);
	case 0x3f:
		return create_runtime_data_from_prototype<runtime_data_type3f>(prototype);
	}
	return func::runtime_item::smart_ptr();

}
// Class rx_platform::runtime::blocks::data::variable_data 

string_type variable_data::type_name = RX_CPP_VARIABLE_TYPE_NAME;


bool variable_data::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!item->serialize(stream, type, ts, mode))
		return false;
	return true;
}

bool variable_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!item->deserialize(stream, type))
		return false;
	return true;
}


// Class rx_platform::runtime::blocks::data::struct_data 

string_type struct_data::type_name = RX_CPP_STRUCT_TYPE_NAME;


bool struct_data::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!item->serialize(stream, type, ts, mode))
		return false;
	return true;
}

bool struct_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!item->deserialize(stream, type))
		return false;
	return true;
}


// Class rx_platform::runtime::blocks::data::mapper_data 

string_type mapper_data::type_name = RX_CPP_MAPPER_TYPE_NAME;


bool mapper_data::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!item->serialize(stream, type, ts, mode))
		return false;
	return true;
}

bool mapper_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!item->deserialize(stream, type))
		return false;
	return true;
}


// Class rx_platform::runtime::blocks::data::source_data 

string_type source_data::type_name = RX_CPP_SOURCE_TYPE_NAME;


bool source_data::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!item->serialize(stream, type, ts, mode))
		return false;
	return true;
}

bool source_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!item->deserialize(stream, type))
		return false;
	return true;
}


// Class rx_platform::runtime::blocks::data::event_data 

string_type event_data::type_name = RX_CPP_EVENT_TYPE_NAME;


bool event_data::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!item->serialize(stream, type, ts, mode))
		return false;
	return true;
}

bool event_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!item->deserialize(stream, type))
		return false;
	return true;
}


// Class rx_platform::runtime::blocks::data::filter_data 

string_type filter_data::type_name = RX_CPP_FILTER_TYPE_NAME;


bool filter_data::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!item->serialize(stream, type, ts, mode))
		return false;
	return true;
}

bool filter_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!item->deserialize(stream, type))
		return false;
	return true;
}


// Class rx_platform::runtime::blocks::data::const_value_data 

string_type const_value_data::type_name = RX_CONST_VALUE_TYPE_NAME;


// Class rx_platform::runtime::blocks::data::value_data 

string_type value_data::type_name = RX_VALUE_TYPE_NAME;


// Class rx_platform::runtime::blocks::data::runtime_data_prototype 


void runtime_data_prototype::add_const_value (const string_type& name, rx_simple_value value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(const_values.size());
		const_values.push_back({ value });
		items.push_back({ name, (new_idx << rt_type_shift) | rt_const_index_type });
	}
}

void runtime_data_prototype::add_value (const string_type& name, rx_timed_value value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		values.push_back({ value });
		items.push_back({ name, (new_idx << rt_type_shift) | rt_value_index_type });
	}
}

bool runtime_data_prototype::check_name (const string_type& name) const
{
	for (const auto& one : items)
	{
		if (one.name == name)
			return false;
	}
	return true;
}

bool runtime_data_prototype::serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	return false;
}

bool runtime_data_prototype::deserialize (base_meta_reader& stream, uint8_t type)
{
	return false;
}

void runtime_data_prototype::add_sub_item (const string_type& name, mapper_data&& value)
{
}

void runtime_data_prototype::add_sub_item (const string_type& name, struct_data&& value)
{
}

void runtime_data_prototype::add_sub_item (const string_type& name, variable_data&& value)
{
}

void runtime_data_prototype::add_sub_item (const string_type& name, source_data::smart_ptr&& value)
{
}

void runtime_data_prototype::add_sub_item (const string_type& name, filter_data&& value)
{
}

void runtime_data_prototype::add_sub_item (const string_type& name, event_data&& value)
{
}


} // namespace data
} // namespace blocks
} // namespace runtime
} // namespace rx_platform

