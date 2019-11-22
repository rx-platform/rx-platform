

/****************************************************************************
*
*  system\runtime\rx_rt_struct.h
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_rt_struct_h
#define rx_rt_struct_h 1



// rx_values
#include "lib/rx_values.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "lib/rx_rt_data.h"
#include "lib/rx_const_size_vector.h"
#include "rx_configuration.h"
#include "system/meta/rx_meta_data.h"
using namespace rx;
using namespace rx::values;
using rx_platform::runtime_item_attribute;


namespace rx_platform {
using runtime::blocks::runtime_holder;

namespace runtime {
namespace blocks
{
	class variable_runtime;
	class struct_runtime;
	class source_runtime;
	class mapper_runtime;
	class filter_runtime;
	class event_runtime;
}
namespace objects
{
	class object_runtime;
}
namespace relations
{

class relation_runtime;
}
typedef rx::pointers::reference<blocks::struct_runtime> struct_runtime_ptr;
typedef rx::pointers::reference<blocks::variable_runtime> variable_runtime_ptr;
typedef rx::pointers::reference<blocks::source_runtime> source_runtime_ptr;
typedef rx::pointers::reference<blocks::mapper_runtime> mapper_runtime_ptr;
typedef rx::pointers::reference<blocks::filter_runtime> filter_runtime_ptr;
typedef rx::pointers::reference<blocks::event_runtime> event_runtime_ptr;
typedef rx::pointers::reference<objects::object_runtime> object_runtime_ptr;
typedef rx::pointers::reference<relations::relation_runtime> relation_runtime_ptr;

namespace structure {

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

struct index_data
{
	string_type name;
	members_index_type index;
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






class hosting_object_data 
{

  public:

      rx_value adapt_value (const rx_value& from) const;


      rx_mode_type mode;

      rx_time time;

      const runtime_holder* object;


  protected:

  private:


};






class init_context 
{

  public:

      static init_context create_initialization_context (runtime_holder* whose);


      const string_type& get_current_path () const
      {
        return current_path_;
      }



      hosting_object_data object_data;


      rx_time now;


  protected:

  private:


      string_type current_path_;


};






class write_context 
{

  public:

      static write_context create_write_context (runtime_holder* whose);


      hosting_object_data object_data;


      rx_time now;


  protected:

  private:


};







class const_value_data 
{

  public:

      rx_value get_value (const hosting_object_data& state) const;

      rx_simple_value simple_get_value () const;

      rx_result set_value (rx_simple_value&& val);


      rx::values::rx_simple_value value;


      static string_type type_name;


  protected:

  private:


};







class value_data 
{

  public:

      rx_value get_value (const hosting_object_data& state) const;

      void set_value (rx_simple_value&& val, const init_context& ctx);

      void object_state_changed (const hosting_object_data& state);

      rx_result write_value (rx_simple_value&& val, const write_context& ctx);

      rx_simple_value simple_get_value () const;

      void simple_set_value (rx_simple_value&& val);


      rx::values::rx_timed_value value;


      static string_type type_name;


  protected:

  private:


};







class runtime_item 
{
  public:
	  typedef std::unique_ptr<runtime_item> smart_ptr;

  public:

      virtual void collect_data (data::runtime_values_data& data) const = 0;

      virtual void fill_data (const data::runtime_values_data& data, init_context& ctx) = 0;

      virtual rx_result get_value (const hosting_object_data& state, const string_type& path, rx_value& val) const = 0;

      virtual void object_state_changed (const hosting_object_data& state) = 0;

      virtual rx_result write_value (const string_type& path, rx_simple_value&& val, const write_context& ctx) = 0;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx) = 0;

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx) = 0;

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx) = 0;

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx) = 0;

      virtual rx_result get_local_value (const string_type& path, rx_simple_value& val) const = 0;

      virtual rx_result get_value_ref (const string_type& path, rt_value_ref& ref) = 0;

      const runtime_item* get_child_item (const string_type& path) const;

      virtual rx_result browse_items (const string_type& filter, const string_type& current_path, std::vector<runtime_item_attribute>& items) const = 0;

	  template<typename T>
	  T get_local_as(const string_type& path, const T& default_value)
	  {
		  values::rx_simple_value temp_val;
		  auto result = get_local_value(path, temp_val);
		  if (result)
		  {
			  return values::extract_value<T>(temp_val.get_storage(), default_value);
		  }
		  return default_value;
	  }
  protected:

      virtual const runtime_item* faster_get_child_item (const char* path, size_t& idx) const = 0;


  private:


};







class variable_data 
{
  public:
	  variable_data() = default;
	  ~variable_data() = default;
	  variable_data(const variable_data&) = delete;
	  variable_data(variable_data&&) noexcept = default;
	  variable_data& operator=(const variable_data&) = delete;
	  variable_data& operator=(variable_data&&) noexcept = default;
	  operator bool() const
	  {
		  return variable_ptr;
	  }

  public:
      variable_data (runtime_item::smart_ptr&& rt, variable_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data) const;

      void fill_data (const data::runtime_values_data& data, init_context& ctx);

      rx_value get_value (const hosting_object_data& state) const;

      void set_value (rx_value&& value);

      void set_value (rx_simple_value&& val, const init_context& ctx);

      rx_result write_value (rx_simple_value&& val, const write_context& ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      rx::values::rx_value value;


      runtime_item::smart_ptr item;

      static string_type type_name;

      variable_runtime_ptr variable_ptr;


  protected:

  private:


};







class struct_data 
{
  public:
	  struct_data() = default;
	  ~struct_data() = default;
	  struct_data(const struct_data&) = default;
	  struct_data(struct_data&&) noexcept = default;
	  struct_data& operator=(const struct_data&) = default;
	  struct_data& operator=(struct_data&&) noexcept = default;
	  operator bool() const
	  {
		  return struct_ptr;
	  }

  public:
      struct_data (runtime_item::smart_ptr&& rt, struct_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data) const;

      void fill_data (const data::runtime_values_data& data, init_context& ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      struct_runtime_ptr struct_ptr;

	  typedef std::unique_ptr<struct_data> smart_ptr;
  protected:

  private:


};







class mapper_data 
{
public:
	mapper_data() = default;
	~mapper_data() = default;
	mapper_data(const mapper_data&) = default;
	mapper_data(mapper_data&&) noexcept = default;
	mapper_data& operator=(const mapper_data&) = default;
	mapper_data& operator=(mapper_data&&) noexcept = default;
	operator bool() const
	{
		return mapper_ptr;
	}

  public:

      void collect_data (data::runtime_values_data& data) const;

      void fill_data (const data::runtime_values_data& data, init_context& ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      mapper_runtime_ptr mapper_ptr;

	  typedef std::unique_ptr<mapper_data> smart_ptr;
  protected:

  private:


};







class source_data 
{
  public:
	source_data() = default;
	~source_data() = default;
	source_data(const source_data&) = default;
	source_data(source_data&&) noexcept = default;
	source_data& operator=(const source_data&) = default;
	source_data& operator=(source_data&&) noexcept = default;
	operator bool() const
	{
		return source_ptr;
	}

  public:

      void collect_data (data::runtime_values_data& data) const;

      void fill_data (const data::runtime_values_data& data, init_context& ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      source_runtime_ptr source_ptr;

	  typedef std::unique_ptr<source_data> smart_ptr;
  protected:

  private:


};







class event_data 
{
public:
	event_data() = default;
	~event_data() = default;
	event_data(const event_data&) = default;
	event_data(event_data&&) noexcept = default;
	event_data& operator=(const event_data&) = default;
	event_data& operator=(event_data&&) noexcept = default;
	operator bool() const
	{
		return event_ptr;
	}

  public:

      void collect_data (data::runtime_values_data& data) const;

      void fill_data (const data::runtime_values_data& data, init_context& ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      event_runtime_ptr event_ptr;

	  typedef std::unique_ptr<event_data> smart_ptr;
  protected:

  private:


};







class filter_data 
{
public:
	filter_data() = default;
	~filter_data() = default;
	filter_data(const filter_data&) = default;
	filter_data(filter_data&&) noexcept = default;
	filter_data& operator=(const filter_data&) = default;
	filter_data& operator=(filter_data&&) noexcept = default;
	operator bool() const
	{
		return filter_ptr;
	}

  public:

      void collect_data (data::runtime_values_data& data) const;

      void fill_data (const data::runtime_values_data& data, init_context& ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      filter_runtime_ptr filter_ptr;

	  typedef std::unique_ptr<filter_data> smart_ptr;
  protected:

  private:


};







template <class variables_type, class structs_type, class sources_type, class mappers_type, class filters_type, class events_type, uint_fast8_t type_id>
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
	  typedef const_size_vector<const_value_data> const_values_type;
	  typedef const_size_vector<value_data> values_type;

	  
	  typedef const_size_vector<index_data> items_type;

  public:

      void collect_data (data::runtime_values_data& data) const;

      void fill_data (const data::runtime_values_data& data, init_context& ctx);

      rx_result get_value (const hosting_object_data& state, const string_type& path, rx_value& val) const;

      void object_state_changed (const hosting_object_data& state);

      rx_result write_value (const string_type& path, rx_simple_value&& val, const write_context& ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result get_local_value (const string_type& path, rx_simple_value& val) const;

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);

      rx_result browse_items (const string_type& filter, const string_type& current_path, std::vector<runtime_item_attribute>& items) const;


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

      const runtime_item* faster_get_child_item (const char* path, size_t& idx) const;


  private:

      members_index_type internal_get_index (const string_type& name) const;

      bool is_value_index (members_index_type idx) const;

      bool is_complex_index (members_index_type idx) const;

      const runtime_item::smart_ptr& get_sub_item (const string_type& path) const;

      runtime_item::smart_ptr& get_sub_item (const string_type& path);



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


// basic item data
// mixed empty<->has
typedef runtime_data<
	has<variable_data>,
	empty<struct_data>,
	has<source_data>,
	empty<mapper_data>,
	has<filter_data>,
	empty<event_data>, 0x15> mixed_item_data;


// full item data
// all the sub items are there
typedef runtime_data<
	has<variable_data>,
	has<struct_data>,
	has<source_data>,
	has<mapper_data>,
	has<filter_data>,
	has<event_data>, 0x3f> full_item_data;


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

// ugly function for creating data holder depending on runtime type
runtime_item::smart_ptr create_runtime_data(uint_fast8_t type_id);


} // namespace structure
} // namespace runtime
} // namespace rx_platform



#endif
