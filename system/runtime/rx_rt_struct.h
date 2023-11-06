

/****************************************************************************
*
*  system\runtime\rx_rt_struct.h
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


#ifndef rx_rt_struct_h
#define rx_rt_struct_h 1


#include "lib/rx_rt_data.h"

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
#include "system/server/rx_platform_item.h"
using namespace rx;
using namespace rx::values;


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
union variable_owner_ptr
{
    variable_data* variable_ptr;
    variable_block_data* block_ptr;
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
	  void copy_from(const std::vector<std::pair<rx_node_id, typeT> >& source)
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
	  void copy_from(std::vector<std::pair<rx_node_id, typeT> >&& source)
	  {
          std::vector<typeT> temp;
          temp.reserve(source.size());
          for (auto&& one : source)
              temp.push_back(std::move(one.second));
		  collection = collection_type(std::move(temp));
	  }
	  collection_type collection;
  protected:

  private:


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

      rx_result write_value (write_data&& data, runtime_process_context* ctx, bool& changed);

      rx_simple_value simple_get_value () const;

      rx_result simple_set_value (rx_simple_value&& val, runtime_process_context* ctx, bool& changed);

      rx_result check_set_value (runtime_process_context* ctx, bool internal, bool test);


      rx::values::rx_timed_value value;


      static string_type type_name;

      std::bitset<32> value_opt;


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

      virtual rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const = 0;

      virtual rx_result fill_value (const values::rx_simple_value& data) = 0;

      virtual rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const = 0;

      virtual void object_state_changed (runtime_process_context* ctx) = 0;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx) = 0;

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx) = 0;

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx) = 0;

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx) = 0;

      virtual rx_result get_value_ref (string_view_type path, rt_value_ref& ref, bool is_var) = 0;

      virtual rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const = 0;

      virtual const runtime_item* get_child_item (string_view_type path) const = 0;

      virtual runtime_filters_type& get_filters () = 0;

      virtual runtime_sources_type& get_sources () = 0;

      virtual runtime_mappers_type& get_mappers () = 0;

      virtual runtime_events_type& get_events () = 0;

      virtual runtime_variables_type& get_variables () = 0;

      virtual runtime_structs_type& get_structs () = 0;

      virtual rx_result get_local_value (string_view_type path, rx_simple_value& val) const = 0;

      virtual bool is_this_yours (string_view_type path) const = 0;

      virtual void read_struct (string_view_type path, read_struct_data data) const = 0;

      virtual void write_struct (string_view_type path, write_struct_data data) = 0;

	  template<typename T>
	  T get_local_as(const string_type& path, const T& default_value)
	  {
		  values::rx_simple_value temp_val;
		  auto result = get_local_value(path, temp_val);
		  if (result)
		  {
              return temp_val.extract_static(default_value);
		  }
		  return default_value;
	  }
      virtual ~runtime_item() = default;
  protected:

  private:


};







class struct_data 
{
  public:
      static constexpr bool has_own_value = false;
      static constexpr rx_attribute_type plain_attribute_type = rx_attribute_type::struct_attribute_type;
      static constexpr rx_attribute_type array_attribute_type = rx_attribute_type::struct_array_attribute_type;

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

      struct_data (runtime_item::smart_ptr&& rt, struct_runtime_ptr&& var, const struct_data& prototype);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_runtime (runtime_process_context* ctx);

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      void object_state_changed (runtime_process_context* ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      struct_runtime_ptr struct_ptr;

	  typedef std::unique_ptr<struct_data> smart_ptr;
  protected:

  private:


};







class filter_data 
{
public:
    static constexpr runtime_value_type runtime_type_ref = runtime_value_type::filters_runtime_value;
    static constexpr bool has_own_value = false;
    static constexpr rx_attribute_type plain_attribute_type = rx_attribute_type::filter_attribute_type;

	~filter_data() = default;
	filter_data(const filter_data&) = default;
	filter_data(filter_data&&) noexcept = default;
	filter_data& operator=(const filter_data&) = default;
	filter_data& operator=(filter_data&&) noexcept = default;
	operator bool() const
	{
		return filter_ptr;
	}
    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::filter_attribute>;

  public:
      filter_data();

      filter_data (runtime_item::smart_ptr&& rt, filter_runtime_ptr&& var, const filter_data& prototype);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

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

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      rx_result filter_changed ();

      void object_state_changed (runtime_process_context* ctx);


      runtime_item::smart_ptr item;

      static string_type type_name;

      filter_runtime_ptr filter_ptr;

	  typedef std::unique_ptr<filter_data> smart_ptr;
  protected:

  private:


      io_capabilities io_;

      runtime_process_context* context_;

      variable_owner_ptr my_variable_;


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






class write_task 
{

  public:
      virtual ~write_task();


      virtual void process_result (rx_result&& result) = 0;

      virtual runtime_transaction_id_t get_id () const = 0;


  protected:

  private:


};






class full_value_data 
{

  public:

      rx_value get_value (runtime_process_context* ctx) const;

      void set_value (const rx_value& val, runtime_process_context* ctx);

      rx_simple_value simple_get_value () const;


      rx::values::rx_value value;


  protected:

  private:


};







class mapper_data 
{
public:
    static constexpr runtime_value_type runtime_type_ref = runtime_value_type::mappers_runtime_value;
    static constexpr bool has_own_value = true;
    static constexpr rx_attribute_type plain_attribute_type = rx_attribute_type::mapper_attribute_type;

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

      mapper_data (runtime_item::smart_ptr&& rt, mapper_runtime_ptr&& var, const mapper_data& prototype);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

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

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      rx_value get_value (runtime_process_context* ctx) const;

      void object_state_changed (runtime_process_context* ctx);

      const rx_value& get_variable_value () const;


      full_value_data mapped_value;


      runtime_item::smart_ptr item;

      static string_type type_name;

      mapper_runtime_ptr mapper_ptr;

      rx_node_id mapper_id;

      string_type full_path;

	  typedef std::unique_ptr<mapper_data> smart_ptr;
  protected:

  private:


      runtime_process_context* context_;

      io_capabilities io_;

      variable_owner_ptr my_variable_;


};







class source_data 
{
  public:
    static constexpr runtime_value_type runtime_type_ref = runtime_value_type::sources_runtime_value;
    static constexpr bool has_own_value = true;
    static constexpr rx_attribute_type plain_attribute_type = rx_attribute_type::source_attribute_type;

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

      source_data (runtime_item::smart_ptr&& rt, source_runtime_ptr&& var, const source_data& prototype);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

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

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      rx_value get_value (runtime_process_context* ctx) const;

      void object_state_changed (runtime_process_context* ctx);

      const rx_value& get_variable_value () const;


      full_value_data input_value;


      runtime_item::smart_ptr item;

      static string_type type_name;

      source_runtime_ptr source_ptr;

      rx_node_id source_id;

      string_type full_path;

	  typedef std::unique_ptr<source_data> smart_ptr;
  protected:

  private:


      runtime_process_context* context_;

      rx_value current_value_;

      io_capabilities io_;

      variable_owner_ptr my_variable_;


};






class mapper_write_task : public write_task  
{

  public:
      mapper_write_task (mapper_data* my_mapper, runtime_transaction_id_t trans_id);


      void process_result (rx_result&& result);

      runtime_transaction_id_t get_id () const;


  protected:

  private:


      mapper_data *my_mapper_;


      runtime_transaction_id_t transaction_id_;


};






class execute_task 
{

  public:
      virtual ~execute_task();


      virtual void process_result (rx_result&& result, data::runtime_values_data&& data) = 0;


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
		  return (bool)variable_ptr;
	  }
      friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::variable_attribute>;
      friend class meta::basic_types::variable_type;
      typedef std::unique_ptr<std::map<runtime_transaction_id_t, write_task*> > pending_tasks_type;
      typedef std::unique_ptr<std::map<runtime_transaction_id_t, execute_task*> > pending_execution_taks_type;

  public:
      variable_data();

      variable_data (runtime_item::smart_ptr&& rt, variable_runtime_ptr&& var, const variable_data& prototype);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

      rx_value get_value (runtime_process_context* ctx) const;

      void set_value (rx_simple_value&& val);

      rx_result write_value (write_data&& data, write_task* task, runtime_process_context* ctx);

      rx_result execute (execute_data&& data, execute_task* task, runtime_process_context* ctx);

      rx_result internal_initialize_runtime (runtime::runtime_init_context& ctx, bool in_complex);

      rx_result internal_deinitialize_runtime (runtime::runtime_deinit_context& ctx, bool in_complex);

      rx_result internal_start_runtime (runtime::runtime_start_context& ctx, bool in_complex);

      rx_result internal_stop_runtime (runtime::runtime_stop_context& ctx, bool in_complex);

      void process_runtime (runtime_process_context* ctx);

      void process_result (runtime_transaction_id_t id, rx_result&& result);

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      void variable_result_pending (runtime_process_context* ctx, rx_result&& result, runtime_transaction_id_t id);

      void object_state_changed (runtime_process_context* ctx);

      rx_simple_value simple_get_value () const;

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      bool prepare_value (rx_value& prepared_value, runtime_process_context* ctx);

      void update_prepared (rx_value prepared_value, runtime_process_context* ctx);


      rx::values::rx_value value;


      runtime_item::smart_ptr item;

      static string_type type_name;

      variable_runtime_ptr variable_ptr;

      std::bitset<32> value_opt;

      string_type full_path;


  protected:

  private:


      pending_tasks_type pending_tasks_;

      pending_execution_taks_type pending_execution_taks_;


};






template <class typeT>
class array_wrapper 
{


    struct array_wrapper_union_t
    {
        constexpr static int empty_prototype_size = 0x10000;
        int size;
        union
        {
            typeT simple_item;
            typeT* array_item;
        };
        void destroy() noexcept;
        array_wrapper_union_t();
        void declare_null_array(typeT&& temp);
        array_wrapper_union_t(const array_wrapper_union_t& right);
        array_wrapper_union_t(array_wrapper_union_t&& right) noexcept;
        array_wrapper_union_t(typeT&& right) noexcept;
        array_wrapper_union_t& operator=(typeT&& right) noexcept;
        array_wrapper_union_t(std::initializer_list<typeT> init_list);
        array_wrapper_union_t& operator=(std::vector<typeT>&& right);
        explicit array_wrapper_union_t(std::vector<typeT>&& from);

        ~array_wrapper_union_t();
    };


    typedef typename std::vector<typeT> container_t;

  public:
      array_wrapper();
      array_wrapper(const array_wrapper& right) noexcept;
      array_wrapper(array_wrapper&& right) noexcept;
      array_wrapper(typeT&& right) noexcept;
      array_wrapper& operator=(typeT&& right) noexcept;
      array_wrapper& operator=(std::vector<typeT>&& right);

      explicit array_wrapper(std::vector<typeT>&& from);
      void declare_null_array(typeT&& temp);
      bool is_array() const;
      const typeT* get_item(int array_idx) const;
      typeT* get_item(int array_idx);
      const typeT* get_item() const;
      typeT* get_item();
      typeT* get_prototype();
      const typeT* get_prototype() const;
      int get_size() const;

  protected:

  private:


      array_wrapper_union_t data_;


};







class block_data : public runtime_item  
{
    typedef const_size_vector<array_wrapper<const_value_data> > values_type;
    typedef const_size_vector< array_wrapper<block_data> > children_type;

    typedef const_size_vector<index_data> items_type;
public:
    block_data() = default;
    ~block_data() = default;
    block_data(const block_data&) = default;
    block_data(block_data&&) noexcept = default;
    block_data& operator=(block_data&&) noexcept = default;
    block_data& operator=(const block_data&) = default;

  public:

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      void object_state_changed (runtime_process_context* ctx);

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref, bool is_var);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      runtime_filters_type& get_filters ();

      runtime_sources_type& get_sources ();

      runtime_mappers_type& get_mappers ();

      runtime_events_type& get_events ();

      runtime_variables_type& get_variables ();

      runtime_structs_type& get_structs ();

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      bool is_this_yours (string_view_type path) const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void write_struct (string_view_type path, write_struct_data data);

      rx_result create_safe_runtime_data (const data::runtime_values_data& in, data::runtime_values_data& out);


      values_type values;

      children_type children;


      items_type items;


  protected:

  private:

      string_view_type extract_index (string_view_type name, int& idx) const;

      members_index_type internal_get_index (string_view_type name, int& idx) const;

      bool is_value_index (members_index_type idx) const;

      bool is_complex_index (members_index_type idx) const;

      rx_result check_value (rx_value_t val_type, const rx_value_union& data);

      rx_result fill_value_internal (rx_value_t val_type, const rx_value_union& data);



};

struct block_data_result_t
{
    block_data runtime;
    bool success = false;
    operator bool() const
    {
        return success;
    }
};






class event_data 
{
public:
    static constexpr runtime_value_type runtime_type_ref = runtime_value_type::events_runtime_value;
    static constexpr bool has_own_value = false;
    static constexpr rx_attribute_type plain_attribute_type = rx_attribute_type::event_attribute_type;

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

      event_data (runtime_item::smart_ptr&& rt, event_runtime_ptr&& var, event_data&& prototype);


      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_runtime (runtime_process_context* ctx);

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      void object_state_changed (runtime_process_context* ctx);


      block_data arguments;


      runtime_item::smart_ptr item;

      static string_type type_name;

      event_runtime_ptr event_ptr;

	  typedef std::unique_ptr<event_data> smart_ptr;
  protected:

  private:


};







class block_data_references 
{
    typedef std::map<string_type, std::unique_ptr<value_data> > references_type;

  public:

      rx_result get_value_ref (block_data& data, string_view_type path, rt_value_ref& ref, rx_time ts);

      void block_data_changed (const block_data& data, runtime::runtime_process_context* ctx);


  protected:

  private:


      references_type references_;


};






class value_block_data 
{
public:
    static constexpr bool has_own_value = true;
    static constexpr rx_attribute_type plain_attribute_type = rx_attribute_type::value_data_attribute_type;
    static constexpr rx_attribute_type array_attribute_type = rx_attribute_type::value_data_array_attribute_type;

    ~value_block_data() = default;
    value_block_data() = default;
    value_block_data(const value_block_data&) = delete;
    value_block_data(value_block_data&&) noexcept = default;
    value_block_data& operator=(value_block_data&&) noexcept = default;
    value_block_data& operator=(const value_block_data&) = delete;


  public:

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      void object_state_changed (runtime_process_context* ctx);

      rx_value get_value (runtime_process_context* ctx) const;


      block_data block;

      value_data struct_value;


      static string_type type_name;

      rx_time timestamp;


  protected:

      rx_result do_write_callback (rx_simple_value& val, runtime::runtime_process_context* ctx);


  private:

      block_data_references* get_tag_references ();



      std::unique_ptr<block_data_references> tag_references_;


};






class variable_block_data_references 
{
    typedef std::map<string_type, std::unique_ptr<full_value_data> > references_type;

  public:

      rx_result get_value_ref (block_data& data, string_view_type path, rt_value_ref& ref, rx_time ts, uint32_t quality, uint32_t origin);

      void block_data_changed (const block_data& data, runtime::runtime_process_context* ctx, uint32_t quality, uint32_t origin);


  protected:

  private:


      references_type references_;


};






class variable_block_data 
{
public:
    static constexpr bool has_own_value = true;
    static constexpr rx_attribute_type plain_attribute_type = rx_attribute_type::variable_data_attribute_type;
    static constexpr rx_attribute_type array_attribute_type = rx_attribute_type::variable_data_array_attribute_type;


    ~variable_block_data() = default;
    variable_block_data() = default;
    variable_block_data(const variable_block_data&) = delete;
    variable_block_data(variable_block_data&&) noexcept = default;
    variable_block_data& operator=(variable_block_data&&) noexcept = default;
    variable_block_data& operator=(const variable_block_data&) = delete;


  public:

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      void fill_data (const data::runtime_values_data& data);

      rx_result collect_value (values::rx_simple_value& data, runtime_value_type type) const;

      rx_result fill_value (const values::rx_simple_value& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const;

      const runtime_item* get_child_item (string_view_type path) const;

      rx_result get_local_value (string_view_type path, rx_simple_value& val) const;

      rx_value get_value (runtime_process_context* ctx) const;

      rx_simple_value simple_get_value () const;

      rx_result set_value (rx_simple_value&& val);

      void object_state_changed (runtime_process_context* ctx);

      rx_result write_value (write_data&& data, write_task* task, runtime_process_context* ctx);

      void process_result (runtime_transaction_id_t id, rx_result&& result);

      void process_runtime (runtime_process_context* ctx);


      variable_data variable;

      block_data block;


      static string_type type_name;


  protected:

      rx_result do_write_callback (rx_simple_value& val, runtime::runtime_process_context* ctx);


  private:

      variable_block_data_references* get_tag_references ();



      std::unique_ptr<variable_block_data_references> tag_references_;


};


} // namespace structure
} // namespace runtime
} // namespace rx_platform

#include "system/runtime/rx_rt_array_wrapper_def.h"


#endif
