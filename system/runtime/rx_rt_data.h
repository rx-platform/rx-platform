

/****************************************************************************
*
*  system\runtime\rx_rt_data.h
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


#ifndef rx_rt_data_h
#define rx_rt_data_h 1



// rx_rt_func
#include "system/runtime/rx_rt_func.h"
// rx_values
#include "lib/rx_values.h"

#include "lib/rx_const_size_vector.h"
#include "rx_configuration.h"


namespace rx_platform {

namespace runtime {

namespace blocks {

namespace data {

// typedefs for masks
constexpr uint_fast8_t rt_const_index_type		= 0x01;
constexpr uint_fast8_t rt_value_index_type		= 0x02;
constexpr uint_fast8_t rt_variable_index_type	= 0x03;
constexpr uint_fast8_t rt_struct_index_type		= 0x04;
constexpr uint_fast8_t rt_source_index_type		= 0x05;
constexpr uint_fast8_t rt_mapper_index_type		= 0x06;
constexpr uint_fast8_t rt_filter_index_type		= 0x07;
constexpr uint_fast8_t rt_event_index_type		= 0x08;

constexpr uint_fast8_t rt_bit_none			= 0x00;
constexpr uint_fast8_t rt_bit_has_variables	= 0x01;
constexpr uint_fast8_t rt_bit_has_structs	= 0x02;
constexpr uint_fast8_t rt_bit_has_sources	= 0x04;
constexpr uint_fast8_t rt_bit_has_mappers	= 0x08;
constexpr uint_fast8_t rt_bit_has_filters	= 0x10;
constexpr uint_fast8_t rt_bit_has_events	= 0x20;

typedef uint_fast16_t members_index_type;

constexpr members_index_type rt_type_shift = 8;
constexpr members_index_type rt_type_mask = 0xff;







template <class typeT>
class has 
{

  public:
	  static constexpr bool can_insert()
	  {
		  return true;
	  }	  
	  typedef const_size_vector<typeT> collection_type;
	  void copy_from(std::vector<typeT>&& source)
	  {
		  collection = collection_type(std::move(source));
	  }
	  collection_type collection;
  protected:

  private:


};






template <class typeT>
class empty 
{

  public:
	  static constexpr bool can_insert()
	  {
		  return false;
	  }
	  typedef std::array<typeT, 0> collection_type;
	  collection_type collection;
	  void copy_from(const std::vector<typeT>& source)
	  {
		 
	  }
  protected:

  private:


};






class variable_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      func::runtime_item::smart_ptr item;

      static string_type type_name;

	  typedef std::unique_ptr<variable_data> smart_ptr;
  protected:

  private:


};






class struct_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      func::runtime_item::smart_ptr item;

      static string_type type_name;

	  typedef std::unique_ptr<struct_data> smart_ptr;
  protected:

  private:


};






class mapper_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      func::runtime_item::smart_ptr item;

      static string_type type_name;

	  typedef std::unique_ptr<mapper_data> smart_ptr;
  protected:

  private:


};






class source_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      func::runtime_item::smart_ptr item;

      static string_type type_name;

	  typedef std::unique_ptr<source_data> smart_ptr;
  protected:

  private:


};






class event_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      func::runtime_item::smart_ptr item;

      static string_type type_name;

	  typedef std::unique_ptr<event_data> smart_ptr;
  protected:

  private:


};






class filter_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      func::runtime_item::smart_ptr item;

      static string_type type_name;

	  typedef std::unique_ptr<filter_data> smart_ptr;
  protected:

  private:


};






class const_value_data 
{

  public:

      rx::values::rx_simple_value value;


      static string_type type_name;


  protected:

  private:


};






class value_data 
{

  public:

      rx::values::rx_timed_value value;


      static string_type type_name;


  protected:

  private:


};


struct index_data
{
	string_type name;
	members_index_type index;
};




class runtime_data_prototype : public func::runtime_item  
{
	typedef std::vector<const_value_data> const_values_type;
	typedef std::vector<value_data> values_type;
	typedef std::vector<variable_data> variables_type;
	typedef std::vector<struct_data> structs_type;
	typedef std::vector<source_data> sources_type;
	typedef std::vector<mapper_data> mappers_type;
	typedef std::vector<filter_data> filters_type;
	typedef std::vector<event_data> events_type;

	typedef std::vector<index_data> items_type;

  public:

      void add_const_value (const string_type& name, rx_simple_value value);

      void add_value (const string_type& name, rx_timed_value value);

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      void add_sub_item (const string_type& name, mapper_data&& value);

      void add_sub_item (const string_type& name, struct_data&& value);

      void add_sub_item (const string_type& name, variable_data&& value);

      void add_sub_item (const string_type& name, source_data::smart_ptr&& value);

      void add_sub_item (const string_type& name, filter_data&& value);

      void add_sub_item (const string_type& name, event_data&& value);


      items_type items;

      const_values_type const_values;

      values_type values;

      variables_type variables;

      structs_type structs;

      sources_type sources;

      mappers_type mappers;

      filters_type filters;

      events_type events;


  protected:

  private:

      bool check_name (const string_type& name) const;



};







template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
class runtime_data : public func::runtime_item  
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
	  typedef const_size_vector<const_value_data> const_values_type;
	  typedef const_size_vector<value_data> values_type;

	  
	  typedef const_size_vector<index_data> items_type;

  public:

      bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      variables_type variables;

      structs_type structs;

      mappers_type mappers;

      sources_type sources;

      events_type events;

      filters_type filters;

      const_values_type const_values;

      values_type values;


      items_type items;


  protected:

  private:


};

// empty item data
// only values and const values are there
typedef runtime_data<
	empty<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x00> empty_item_data;

// variables item data
// only variables are there
typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x01> variables_item_data;


// structs item data
// only structs are there
typedef runtime_data<
	empty<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x02> structs_item_data;


// basic item data
// structs and variables are there
typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	empty<source_data>,
	empty<mapper_data>,
	empty<filter_data>,
	empty<event_data>, 0x03> basic_item_data;

// full item data
// all the sub items are there
typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3f> full_item_data;

// ugly function for creating data holder depending on runtime type
func::runtime_item::smart_ptr create_runtime_data(uint_fast8_t type_id);

func::runtime_item::smart_ptr create_runtime_data(runtime_data_prototype& prototype);


} // namespace data
} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
