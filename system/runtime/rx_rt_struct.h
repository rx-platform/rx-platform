

/****************************************************************************
*
*  system\runtime\rx_rt_struct.h
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


#ifndef rx_rt_struct_h
#define rx_rt_struct_h 1



// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
// rx_values
#include "lib/rx_values.h"

namespace rx_platform {
namespace runtime {
namespace structure {
class filter_data;
class source_data;

} // namespace structure
} // namespace runtime
} // namespace rx_platform


#include "lib/rx_rt_data.h"
#include "lib/rx_const_size_vector.h"
#include "rx_configuration.h"
#include "system/meta/rx_meta_data.h"
#include "system/server/rx_platform_item.h"
using namespace rx;
using namespace rx::values;
using rx_platform::runtime_item_attribute;


namespace rx_platform {

namespace meta
{
class runtime_data_prototype;

namespace def_blocks
{
class complex_data_type;
class source_attribute;
class mapper_attribute;
class filter_attribute;
class event_attribute;
class variable_attribute;
}
namespace basic_types
{
class struct_type;
class variable_type;
class source_type;
class mapper_type;
class filter_type;
class event_type;
}
namespace object_types
{
class object_data_type;
class relation_attribute;
}
}

namespace runtime {

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






class fill_context 
{

  public:
      fill_context (runtime_process_context* ctx);


      const string_type& get_current_path () const
      {
        return current_path_;
      }



      runtime_process_context *context;


      rx_time now;


  protected:

  private:


      string_type current_path_;


};







class const_value_data 
{

  public:

      rx_value get_value (runtime_process_context* ctx) const;

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

      rx_value get_value (runtime_process_context* ctx) const;

      void set_value (rx_simple_value&& val, const rx_time& time);

      void object_state_changed (runtime_process_context* ctx);

      rx_result write_value (write_data&& data);

      rx_simple_value simple_get_value () const;

      rx_result simple_set_value (rx_simple_value&& val);


      rx::values::rx_timed_value value;


      static string_type type_name;

      bool read_only;


  protected:

  private:


};







class runtime_item 
{
  public:
	  typedef std::unique_ptr<runtime_item> smart_ptr;

  public:

      virtual void collect_data (data::runtime_values_data& data, runtime_value_type type) const = 0;

      virtual void fill_data (const data::runtime_values_data& data) = 0;

      virtual rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const = 0;

      virtual void object_state_changed (runtime_process_context* ctx) = 0;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx) = 0;

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx) = 0;

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx) = 0;

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx) = 0;

      virtual rx_result get_value_ref (const string_type& path, rt_value_ref& ref) = 0;

      const runtime_item* get_child_item (const string_type& path) const;

      virtual rx_result browse_items (const string_type& filter, const string_type& current_path, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const = 0;

      virtual runtime_filters_type& get_filters () = 0;

      virtual runtime_sources_type& get_sources () = 0;

      virtual runtime_mappers_type& get_mappers () = 0;

      virtual runtime_events_type& get_events () = 0;

      virtual runtime_variables_type& get_variables () = 0;

      virtual runtime_structs_type& get_structs () = 0;

      virtual rx_result get_local_value (const string_type& path, rx_simple_value& val) const = 0;

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







class struct_data 
{
  public:
	  ~struct_data() = default;
	  struct_data(const struct_data&) = delete;
	  struct_data(struct_data&&) noexcept = default;
	  struct_data& operator=(const struct_data&) = delete;
	  struct_data& operator=(struct_data&&) noexcept = default;
	  operator bool() const
	  {
		  return struct_ptr;
	  }

  public:
      struct_data();

      struct_data (runtime_item::smart_ptr&& rt, struct_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_runtime (runtime_process_context* ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      struct_runtime_ptr struct_ptr;

	  typedef std::unique_ptr<struct_data> smart_ptr;
  protected:

  private:


};







class event_data 
{
public:
	~event_data() = default;
	event_data(const event_data&) = delete;
	event_data(event_data&&) noexcept = default;
	event_data& operator=(const event_data&) = delete;
	event_data& operator=(event_data&&) noexcept = default;
	operator bool() const
	{
		return event_ptr;
	}
    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::event_attribute>;

  public:
      event_data();

      event_data (runtime_item::smart_ptr&& rt, event_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_runtime (runtime_process_context* ctx);


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
	~filter_data() = default;
	filter_data(const filter_data&) = delete;
	filter_data(filter_data&&) noexcept = default;
	filter_data& operator=(const filter_data&) = delete;
	filter_data& operator=(filter_data&&) noexcept = default;
	operator bool() const
	{
		return filter_ptr;
	}
    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::filter_attribute>;

  public:
      filter_data();

      filter_data (runtime_item::smart_ptr&& rt, filter_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_runtime (runtime_process_context* ctx);

      rx_result filter_output (rx_simple_value& val);

      rx_result filter_input (rx_value& val);

      bool is_input () const;

      bool is_output () const;

      rx_result get_value (runtime_handle_t handle, values::rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, values::rx_simple_value&& val);


      runtime_item::smart_ptr item;

      static string_type type_name;

      filter_runtime_ptr filter_ptr;

	  typedef std::unique_ptr<filter_data> smart_ptr;
  protected:

  private:


      io_capabilities io_;

      runtime_process_context* context_;


};






class indirect_value_data 
{

  public:

      rx_value get_value (runtime_process_context* ctx) const;

      void set_value (rx_simple_value&& val, const rx_time& time);

      void object_state_changed (runtime_process_context* ctx);

      rx_result write_value (write_data&& data, runtime_process_context* ctx);


      rx::values::rx_timed_value value;


      static string_type type_name;

      bool read_only;


  protected:

  private:


      rx::values::rx_simple_value default_value_;


};






class variable_write_task 
{

  public:
      virtual ~variable_write_task();


      virtual void process_result (runtime_transaction_id_t id, rx_result&& result) = 0;


  protected:

  private:


};







class variable_data 
{
  public:
	  ~variable_data() = default;
	  variable_data(const variable_data&) = delete;
	  variable_data(variable_data&&) noexcept = default;
	  variable_data& operator=(const variable_data&) = delete;
	  variable_data& operator=(variable_data&&) noexcept = default;
	  operator bool() const
	  {
		  return variable_ptr;
	  }
      friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::variable_attribute>;
      friend class meta::basic_types::variable_type;
      typedef std::unique_ptr<std::map<runtime_transaction_id_t, variable_write_task*> > pending_tasks_type;

  public:
      variable_data();

      variable_data (runtime_item::smart_ptr&& rt, variable_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_value get_value (runtime_process_context* ctx) const;

      void set_value (rx_simple_value&& val);

      rx_result write_value (write_data&& data, variable_write_task* task, runtime_process_context* ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_runtime (runtime_process_context* ctx);

      void process_result (runtime_transaction_id_t id, rx_result&& result);


      rx::values::rx_value value;


      runtime_item::smart_ptr item;

      static string_type type_name;

      variable_runtime_ptr variable_ptr;


  protected:

  private:


      pending_tasks_type pending_tasks_;


};







class mapper_data 
{
public:
	~mapper_data() = default;
	mapper_data(const mapper_data&) = delete;
	mapper_data(mapper_data&&) noexcept = default;
	mapper_data& operator=(const mapper_data&) = delete;
	mapper_data& operator=(mapper_data&&) noexcept = default;
	operator bool() const
	{
		return mapper_ptr;
	}
    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::mapper_attribute>;
    friend class meta::def_blocks::complex_data_type;
    friend class meta::basic_types::mapper_type;

  public:
      mapper_data();

      mapper_data (runtime_item::smart_ptr&& rt, mapper_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_update (values::rx_value&& value);

      void process_write (write_data&& data);

      void mapper_write_pending (write_data&& data);

      rx_value get_mapped_value () const;

      rx_result value_changed (rx_value&& val);

      bool can_read () const;

      bool can_write () const;

      void process_write_result (rx_result&& result, runtime_transaction_id_t id);


      runtime_item::smart_ptr item;

      static string_type type_name;

      mapper_runtime_ptr mapper_ptr;

      rx_node_id mapper_id;

	  typedef std::unique_ptr<mapper_data> smart_ptr;
  protected:

  private:


      variable_data *my_variable_;


      runtime_process_context* context_;

      io_capabilities io_;


};







class source_data 
{
  public:
	~source_data() = default;
	source_data(const source_data&) = delete;
	source_data(source_data&&) noexcept = default;
	source_data& operator=(const source_data&) = delete;
	source_data& operator=(source_data&&) noexcept = default;
	operator bool() const
	{
		return source_ptr;
	}
    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::source_attribute>;
    friend class meta::def_blocks::complex_data_type;
    friend class meta::basic_types::source_type;

  public:
      source_data();

      source_data (runtime_item::smart_ptr&& rt, source_runtime_ptr&& var);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result write_value (write_data&& data);

      void process_update (values::rx_value&& value);

      void process_write (write_data&& data);

      void source_update_pending (values::rx_value&& value);

      bool is_input () const;

      bool is_output () const;

      const rx_value& get_current_value () const;

      void source_result_pending (rx_result&& result, runtime_transaction_id_t id);

      void process_result (runtime_transaction_id_t id, rx_result&& result);

      threads::job_thread* get_jobs_queue ();

      void add_periodic_job (jobs::periodic_job::smart_ptr job);


      runtime_item::smart_ptr item;

      static string_type type_name;

      source_runtime_ptr source_ptr;

	  typedef std::unique_ptr<source_data> smart_ptr;
  protected:

  private:


      variable_data *my_variable_;


      runtime_process_context* context_;

      rx_value current_value_;

      io_capabilities io_;


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
      typedef const_size_vector<indirect_value_data> indirect_values_type;

	  
	  typedef const_size_vector<index_data> items_type;

  public:

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      void object_state_changed (runtime_process_context* ctx);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);

      rx_result browse_items (const string_type& filter, const string_type& current_path, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      runtime_filters_type& get_filters ();

      runtime_sources_type& get_sources ();

      runtime_mappers_type& get_mappers ();

      runtime_events_type& get_events ();

      runtime_variables_type& get_variables ();

      runtime_structs_type& get_structs ();

      rx_result get_local_value (const string_type& path, rx_simple_value& val) const;


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

      const runtime_item::smart_ptr& get_sub_item (const string_type& path, bool include_vars = true) const;

      runtime_item::smart_ptr& get_sub_item (const string_type& path, bool include_vars = true);



      indirect_values_type indirect_values_;


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






class mapper_write_task : public variable_write_task  
{

  public:
      mapper_write_task (mapper_data* my_mapper, runtime_transaction_id_t trans_id);


      void process_result (runtime_transaction_id_t id, rx_result&& result);


  protected:

  private:


      mapper_data *my_mapper_;


      runtime_transaction_id_t transaction_id_;


};


} // namespace structure
} // namespace runtime
} // namespace rx_platform



#endif
