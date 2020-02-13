

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.h
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


#ifndef rx_runtime_helpers_h
#define rx_runtime_helpers_h 1


#include "lib/rx_values.h"
/////////////////////////////////////////////////////////////
// logging macros for console library
#define RUNTIME_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Run",src,lvl,msg)
#define RUNTIME_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Run",src,lvl,msg)
#define RUNTIME_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Run",src,lvl,msg)
#define RUNTIME_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Run",src,lvl,msg)
#define RUNTIME_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Run",src,lvl,msg)
#define RUNTIME_LOG_TRACE(src,lvl,msg) RX_TRACE("Run",src,lvl,msg)


namespace rx_platform {
namespace runtime {
namespace blocks {
class variable_runtime;
} // namespace blocks

namespace structure {
class runtime_item;
} // namespace structure

namespace operational {
class binded_tags;
class connected_tags;
} // namespace operational

namespace algorithms {
template <class typeT> class runtime_holder;

} // namespace algorithms
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace api
{
struct query_result;
}
namespace meta
{
class meta_data;
namespace object_types
{
class object_type;
class application_type;
class domain_type;
class port_type;
}
namespace meta_algorithm
{
template <class typeT>
class object_types_algorithm;
}
}
namespace runtime {
struct runtime_init_context;
namespace relations
{
class relation_runtime;
}
namespace items {
class port_runtime;
class object_runtime;
class application_runtime;
class domain_runtime;

} // namespace items
namespace algorithms
{
template <class typeT>
class runtime_holder;
template <class typeT>
class object_runtime_algorithms;
}
}
typedef rx_reference<pointers::reference_object> rx_reference_ptr;
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::domain_type> > rx_domain_ptr;
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::port_type> > rx_port_ptr;
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::object_type> > rx_object_ptr;
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::application_type> > rx_application_ptr;
typedef rx_reference<runtime::relations::relation_runtime> rx_relation_ptr;

typedef rx_reference<runtime::items::object_runtime> rx_object_impl_ptr;
typedef rx_reference<runtime::items::port_runtime> rx_port_impl_ptr;
typedef rx_reference<runtime::items::application_runtime> rx_application_impl_ptr;
typedef rx_reference<runtime::items::domain_runtime> rx_domain_impl_ptr;


typedef uint32_t runtime_handle_t;
typedef uint32_t runtime_transaction_id_t;

enum subscription_trigger_type
{
	subscription_trigger_periodic = 0,
	subscription_trigger_critical = 1,

	max_trigger_type = 1
};

namespace runtime {
namespace relations
{
class relation_runtime;
}
namespace operational
{
class rx_tags_callback;
typedef rx_reference<rx_tags_callback> tags_callback_ptr;
}
using operational::tags_callback_ptr;
namespace structure {
class const_value_data;
class value_data;
class variable_data;
class hosting_object_data;
} // namespace structure
union rt_value_ref_union
{
	structure::const_value_data* const_value;
	structure::value_data* value;
	structure::variable_data* variable;
    relations::relation_runtime* relation;
};
enum class rt_value_ref_type
{
	rt_null = 0,
	rt_const_value = 1,
	rt_value = 2,
	rt_variable = 3,
    rt_relation = 4/// !!!!!!/// CHECK shwitches
};
struct rt_value_ref
{
	rt_value_ref_type ref_type;
	rt_value_ref_union ref_value_ptr;
};

typedef rx_reference<blocks::variable_runtime> rx_variable_ptr;
typedef std::unique_ptr<structure::runtime_item> rx_runtime_item_ptr;

namespace algorithms {

enum runtime_process_step : int
{
	runtime_process_idle = 0,
	runtime_process_scheduled = 1,
    runtime_process_tag_writes = 2,
	runtime_process_tag_connections = 3,

    runtime_process_over = 4
};




class runtime_process_context 
{

  public:
      runtime_process_context (operational::binded_tags& binded, operational::connected_tags& tags);


      bool should_repeat () const;

      bool tag_updates_pending ();

      rx_result init_context ();

      bool tag_writes_pending ();

      bool should_process_tags ();

      bool should_process_writes ();

      rx_result get_value (runtime_handle_t handle, values::rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, values::rx_simple_value&& val);

      rx_result set_item (const string_type& path, values::rx_simple_value&& what, runtime_init_context& ctx);


      rx_time now;

      template<typename T>
      rx_result set_item_static(const string_type& path, T&& value, runtime_init_context& ctx)
      {
          values::rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = this->set_item(path, std::move(temp), ctx);

          return result;
      }
      template<typename valT>
      valT get_binded_as(runtime_handle_t handle, const valT& default_value)
      {
            values::rx_simple_value temp_val;
            auto result = this->get_value(handle, temp_val);
            if (result)
            {
                return values::extract_value<valT>(temp_val.get_storage(), default_value);
            }
          return default_value;
      }
      template<typename valT>
      void set_binded_as(runtime_handle_t handle, valT&& value)
      {
          values::rx_simple_value temp_val;
          temp_val.assign_static<valT>(std::forward<valT>(value));
          auto result = this->set_value(handle, std::move(temp_val));
      }
  protected:

  private:


      operational::connected_tags& tags_;

      operational::binded_tags& binded_;


      runtime_process_step current_step_;

      bool process_all_;

      bool process_tag_connections_;

      bool process_tag_writes_;

      structure::hosting_object_data* state_;


};


} // namespace algorithms






class runtime_path_resolver 
{

  public:

      void push_to_path (const string_type& name);

      void pop_from_path ();

      const string_type& get_current_path () const;

      string_type get_parent_path (size_t level) const;


  protected:

  private:


      string_type path_;


};







class runtime_structure_resolver 
{
	typedef std::stack<std::reference_wrapper<structure::runtime_item>, std::vector<std::reference_wrapper<structure::runtime_item> > > runtime_items_type;

  public:
      runtime_structure_resolver (structure::runtime_item& root);


      void push_item (structure::runtime_item& item);

      void pop_item ();

      structure::runtime_item& get_current_item ();

      structure::runtime_item& get_root ();


  protected:

  private:


      runtime_items_type items_;

      std::reference_wrapper<structure::runtime_item> root_;


};







class variables_stack 
{
	typedef std::stack<rx_variable_ptr, std::vector<rx_variable_ptr> > variables_type;

  public:

      void push_variable (rx_variable_ptr what);

      void pop_variable ();

      rx_variable_ptr get_current_variable () const;


  protected:

  private:


      variables_type variables_;


};






struct runtime_deinit_context 
{


      variables_stack variables;

  public:

  protected:

  private:


};


typedef std::map<string_type, runtime_handle_t> binded_tags_type;




struct runtime_init_context 
{

      runtime_init_context (structure::runtime_item& root, const meta::meta_data& meta, algorithms::runtime_process_context* context, operational::binded_tags* binded);


      runtime_handle_t get_new_handle ();


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      algorithms::runtime_process_context *context;

      operational::binded_tags *tags;


      const meta::meta_data& meta;

      binded_tags_type binded_tags;

  public:

  protected:

  private:


      runtime_handle_t next_handle_;


};






struct runtime_start_context 
{

      runtime_start_context (structure::runtime_item& root, algorithms::runtime_process_context* context);


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      algorithms::runtime_process_context *context;

  public:

  protected:

  private:


};






struct runtime_stop_context 
{


      variables_stack variables;

  public:

  protected:

  private:


};


} // namespace runtime
} // namespace rx_platform



#endif
