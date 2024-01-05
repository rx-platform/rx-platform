

/****************************************************************************
*
*  system\runtime\rx_rt_item_types.h
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


#ifndef rx_rt_item_types_h
#define rx_rt_item_types_h 1


#include "lib/rx_rt_data.h"

// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"



namespace rx_platform {

namespace runtime {

namespace structure {






template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, class blocks_type, class variable_blocks_type, uint_fast8_t type_id>
class runtime_data : public runtime_item  
{
public:
    static constexpr uint_fast8_t get_runtime_data_type()
    {
        // missing static assert here!!!!
        return type_id;
    }
    static constexpr bool has_variables()
    {
        return variables_type::can_insert();
    }
    static constexpr bool has_structs()
    {
        return structs_type::can_insert();
    }
    static constexpr bool has_sources()
    {
        return sources_type::can_insert();
    }
    static constexpr bool has_mappers()
    {
        return mappers_type::can_insert();
    }
    static constexpr bool has_filters()
    {
        return filters_type::can_insert();
    }
    static constexpr bool has_events()
    {
        return events_type::can_insert();
    }
    static constexpr bool has_block_data()
    {
        return blocks_type::can_insert();
    }
    static constexpr bool has_variable_blocks_data()
    {
        return variable_blocks_type::can_insert();
    }
    typedef const_size_vector<array_wrapper<const_value_data> > const_values_type;
    typedef const_size_vector< array_wrapper<value_data> > values_type;


    typedef const_size_vector<index_data> items_type;

  public:
      ~runtime_data();


      rx_result collect_data (string_view_type path, data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      void object_state_changed (runtime_process_context* ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref, bool is_var);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      runtime_filters_type& get_filters ();

      runtime_sources_type& get_sources ();

      runtime_mappers_type& get_mappers ();

      runtime_events_type& get_events ();

      runtime_variables_type& get_variables ();

      runtime_structs_type& get_structs ();

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      bool is_this_yours (string_view_type path) const;


      variables_type variables;

      structs_type structs;

      mappers_type mappers;

      sources_type sources;

      events_type events;

      filters_type filters;

      const_values_type const_values;

      values_type values;

      blocks_type blocks;

      variable_blocks_type variable_blocks;


      items_type items;


  protected:

  private:


};

// empty item data
// only values and const values are there
typedef runtime_data<
	empty<array_wrapper<variable_data> >,
	empty<array_wrapper<struct_data> >,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>, 0x00> empty_item_data;

// variables item data
// only variables are there
typedef runtime_data<
	has<array_wrapper<variable_data> >,
	empty<array_wrapper<struct_data> >,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>, 0x01> variables_item_data;


// structs item data
// only structs are there
typedef runtime_data<
	empty<array_wrapper<variable_data> >,
	has<array_wrapper<struct_data> >,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>, 0x02> structs_item_data;


// basic item data
// structs and variables are there
typedef runtime_data<
	has<array_wrapper<variable_data> >,
	has<array_wrapper<struct_data> >,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>, 0x03> basic_item_data;


// basic item data
// mixed empty<->has
typedef runtime_data<
	has<array_wrapper<variable_data> >,
	empty<array_wrapper<struct_data> >,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>, 0x15> mixed_item_data;


// full item data
// all the sub items are there
typedef runtime_data<
	has<array_wrapper<variable_data> >,
	has<array_wrapper<struct_data> >,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>, 0x3f> full_item_data;

//*********************************************************************************************************
//*                      CODE GENERATED BY GenerateRuntimeItemBoringStuff tool!!!                         *
//*********************************************************************************************************
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x00> runtime_data_type00;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x01> runtime_data_type01;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x02> runtime_data_type02;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x03> runtime_data_type03;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x04> runtime_data_type04;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x05> runtime_data_type05;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x06> runtime_data_type06;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x07> runtime_data_type07;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x08> runtime_data_type08;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x09> runtime_data_type09;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x0a> runtime_data_type0a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x0b> runtime_data_type0b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x0c> runtime_data_type0c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x0d> runtime_data_type0d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x0e> runtime_data_type0e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x0f> runtime_data_type0f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x10> runtime_data_type10;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x11> runtime_data_type11;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x12> runtime_data_type12;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x13> runtime_data_type13;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x14> runtime_data_type14;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x15> runtime_data_type15;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x16> runtime_data_type16;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x17> runtime_data_type17;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x18> runtime_data_type18;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x19> runtime_data_type19;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x1a> runtime_data_type1a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x1b> runtime_data_type1b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x1c> runtime_data_type1c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x1d> runtime_data_type1d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x1e> runtime_data_type1e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x1f> runtime_data_type1f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x20> runtime_data_type20;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x21> runtime_data_type21;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x22> runtime_data_type22;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x23> runtime_data_type23;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x24> runtime_data_type24;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x25> runtime_data_type25;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x26> runtime_data_type26;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x27> runtime_data_type27;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x28> runtime_data_type28;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x29> runtime_data_type29;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x2a> runtime_data_type2a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x2b> runtime_data_type2b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x2c> runtime_data_type2c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x2d> runtime_data_type2d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x2e> runtime_data_type2e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x2f> runtime_data_type2f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x30> runtime_data_type30;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x31> runtime_data_type31;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x32> runtime_data_type32;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x33> runtime_data_type33;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x34> runtime_data_type34;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x35> runtime_data_type35;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x36> runtime_data_type36;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x37> runtime_data_type37;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x38> runtime_data_type38;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x39> runtime_data_type39;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x3a> runtime_data_type3a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x3b> runtime_data_type3b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x3c> runtime_data_type3c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x3d> runtime_data_type3d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x3e> runtime_data_type3e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x3f> runtime_data_type3f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x40> runtime_data_type40;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x41> runtime_data_type41;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x42> runtime_data_type42;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x43> runtime_data_type43;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x44> runtime_data_type44;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x45> runtime_data_type45;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x46> runtime_data_type46;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x47> runtime_data_type47;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x48> runtime_data_type48;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x49> runtime_data_type49;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x4a> runtime_data_type4a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x4b> runtime_data_type4b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x4c> runtime_data_type4c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x4d> runtime_data_type4d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x4e> runtime_data_type4e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x4f> runtime_data_type4f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x50> runtime_data_type50;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x51> runtime_data_type51;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x52> runtime_data_type52;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x53> runtime_data_type53;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x54> runtime_data_type54;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x55> runtime_data_type55;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x56> runtime_data_type56;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x57> runtime_data_type57;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x58> runtime_data_type58;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x59> runtime_data_type59;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x5a> runtime_data_type5a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x5b> runtime_data_type5b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x5c> runtime_data_type5c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x5d> runtime_data_type5d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x5e> runtime_data_type5e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x5f> runtime_data_type5f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x60> runtime_data_type60;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x61> runtime_data_type61;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x62> runtime_data_type62;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x63> runtime_data_type63;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x64> runtime_data_type64;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x65> runtime_data_type65;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x66> runtime_data_type66;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x67> runtime_data_type67;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x68> runtime_data_type68;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x69> runtime_data_type69;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x6a> runtime_data_type6a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x6b> runtime_data_type6b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x6c> runtime_data_type6c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x6d> runtime_data_type6d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x6e> runtime_data_type6e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x6f> runtime_data_type6f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x70> runtime_data_type70;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x71> runtime_data_type71;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x72> runtime_data_type72;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x73> runtime_data_type73;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x74> runtime_data_type74;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x75> runtime_data_type75;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x76> runtime_data_type76;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x77> runtime_data_type77;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x78> runtime_data_type78;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x79> runtime_data_type79;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x7a> runtime_data_type7a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x7b> runtime_data_type7b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x7c> runtime_data_type7c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x7d> runtime_data_type7d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x7e> runtime_data_type7e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	empty<array_wrapper<variable_block_data>>,
	0x7f> runtime_data_type7f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x80> runtime_data_type80;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x81> runtime_data_type81;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x82> runtime_data_type82;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x83> runtime_data_type83;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x84> runtime_data_type84;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x85> runtime_data_type85;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x86> runtime_data_type86;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x87> runtime_data_type87;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x88> runtime_data_type88;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x89> runtime_data_type89;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x8a> runtime_data_type8a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x8b> runtime_data_type8b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x8c> runtime_data_type8c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x8d> runtime_data_type8d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x8e> runtime_data_type8e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x8f> runtime_data_type8f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x90> runtime_data_type90;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x91> runtime_data_type91;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x92> runtime_data_type92;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x93> runtime_data_type93;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x94> runtime_data_type94;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x95> runtime_data_type95;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x96> runtime_data_type96;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x97> runtime_data_type97;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x98> runtime_data_type98;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x99> runtime_data_type99;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x9a> runtime_data_type9a;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x9b> runtime_data_type9b;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x9c> runtime_data_type9c;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x9d> runtime_data_type9d;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x9e> runtime_data_type9e;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0x9f> runtime_data_type9f;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa0> runtime_data_typea0;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa1> runtime_data_typea1;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa2> runtime_data_typea2;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa3> runtime_data_typea3;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa4> runtime_data_typea4;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa5> runtime_data_typea5;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa6> runtime_data_typea6;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa7> runtime_data_typea7;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa8> runtime_data_typea8;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xa9> runtime_data_typea9;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xaa> runtime_data_typeaa;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xab> runtime_data_typeab;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xac> runtime_data_typeac;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xad> runtime_data_typead;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xae> runtime_data_typeae;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xaf> runtime_data_typeaf;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb0> runtime_data_typeb0;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb1> runtime_data_typeb1;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb2> runtime_data_typeb2;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb3> runtime_data_typeb3;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb4> runtime_data_typeb4;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb5> runtime_data_typeb5;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb6> runtime_data_typeb6;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb7> runtime_data_typeb7;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb8> runtime_data_typeb8;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xb9> runtime_data_typeb9;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xba> runtime_data_typeba;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xbb> runtime_data_typebb;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xbc> runtime_data_typebc;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xbd> runtime_data_typebd;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xbe> runtime_data_typebe;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	empty<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xbf> runtime_data_typebf;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc0> runtime_data_typec0;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc1> runtime_data_typec1;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc2> runtime_data_typec2;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc3> runtime_data_typec3;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc4> runtime_data_typec4;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc5> runtime_data_typec5;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc6> runtime_data_typec6;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc7> runtime_data_typec7;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc8> runtime_data_typec8;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xc9> runtime_data_typec9;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xca> runtime_data_typeca;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xcb> runtime_data_typecb;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xcc> runtime_data_typecc;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xcd> runtime_data_typecd;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xce> runtime_data_typece;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xcf> runtime_data_typecf;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd0> runtime_data_typed0;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd1> runtime_data_typed1;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd2> runtime_data_typed2;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd3> runtime_data_typed3;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd4> runtime_data_typed4;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd5> runtime_data_typed5;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd6> runtime_data_typed6;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd7> runtime_data_typed7;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd8> runtime_data_typed8;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xd9> runtime_data_typed9;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xda> runtime_data_typeda;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xdb> runtime_data_typedb;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xdc> runtime_data_typedc;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xdd> runtime_data_typedd;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xde> runtime_data_typede;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	empty<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xdf> runtime_data_typedf;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe0> runtime_data_typee0;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe1> runtime_data_typee1;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe2> runtime_data_typee2;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe3> runtime_data_typee3;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe4> runtime_data_typee4;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe5> runtime_data_typee5;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe6> runtime_data_typee6;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe7> runtime_data_typee7;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe8> runtime_data_typee8;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xe9> runtime_data_typee9;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xea> runtime_data_typeea;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xeb> runtime_data_typeeb;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xec> runtime_data_typeec;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xed> runtime_data_typeed;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xee> runtime_data_typeee;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	empty<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xef> runtime_data_typeef;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf0> runtime_data_typef0;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf1> runtime_data_typef1;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf2> runtime_data_typef2;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf3> runtime_data_typef3;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf4> runtime_data_typef4;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf5> runtime_data_typef5;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf6> runtime_data_typef6;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf7> runtime_data_typef7;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf8> runtime_data_typef8;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xf9> runtime_data_typef9;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xfa> runtime_data_typefa;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	empty<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xfb> runtime_data_typefb;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xfc> runtime_data_typefc;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	empty<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xfd> runtime_data_typefd;
typedef runtime_data<
	empty<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xfe> runtime_data_typefe;
typedef runtime_data<
	has<array_wrapper<variable_data>>,
	has<array_wrapper<struct_data>>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>,
	has<array_wrapper<value_block_data>>,
	has<array_wrapper<variable_block_data>>,
	0xff> runtime_data_typeff;
//*********************************************************************************************************



} // namespace structure
} // namespace runtime
} // namespace rx_platform



#endif
