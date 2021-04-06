

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_runtime_helpers_h
#define rx_runtime_helpers_h 1


#include "lib/rx_values.h"
using namespace rx::values;
/////////////////////////////////////////////////////////////
// logging macros for console library
#define RUNTIME_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Run",src,lvl,(msg))
#define RUNTIME_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Run",src,lvl,(msg))
#define RUNTIME_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Run",src,lvl,msg)
#define RUNTIME_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Run",src,lvl,(msg))
#define RUNTIME_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Run",src,lvl,(msg))
#define RUNTIME_LOG_TRACE(src,lvl,msg) RX_TRACE("Run",src,lvl,(msg))


namespace rx_platform {
namespace ns {
class rx_directory_resolver;
} // namespace ns

namespace runtime {
namespace structure {
class runtime_item;
class mapper_data;
class variable_data;
} // namespace structure

namespace algorithms {
template <class typeT> class runtime_holder;
} // namespace algorithms

namespace operational {
class binded_tags;
} // namespace operational

class runtime_process_context;

} // namespace runtime
} // namespace rx_platform


#include "lib/rx_const_size_vector.h"


namespace rx_platform {

namespace logic
{
class program_runtime;
class method_runtime;
}
typedef rx_reference<logic::program_runtime> program_runtime_ptr;
typedef rx_reference<logic::method_runtime> method_runtime_ptr;

namespace displays
{
class display_runtime;
}
typedef rx_reference<displays::display_runtime> display_runtime_ptr;

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
template <class typeT>
class meta_blocks_algorithm;
class relation_blocks_algorithm;
class complex_data_algorithm;
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
class runtime_holder_algorithms;
template <class typeT>
class runtime_scan_algorithms;
}
}
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
namespace blocks
{
class variable_runtime;
class struct_runtime;
class source_runtime;
class mapper_runtime;
class filter_runtime;
class event_runtime;
}
namespace relations
{
class relation_data;
class relation_runtime;
}


typedef rx::pointers::reference<blocks::struct_runtime> struct_runtime_ptr;
typedef rx::pointers::reference<blocks::variable_runtime> variable_runtime_ptr;
typedef rx::pointers::reference<blocks::source_runtime> source_runtime_ptr;
typedef rx::pointers::reference<blocks::mapper_runtime> mapper_runtime_ptr;
typedef rx::pointers::reference<blocks::filter_runtime> filter_runtime_ptr;
typedef rx::pointers::reference<blocks::event_runtime> event_runtime_ptr;
typedef rx::pointers::reference<relations::relation_runtime> relation_runtime_ptr;

namespace operational
{
class rx_tags_callback;
typedef rx_reference<rx_tags_callback> tags_callback_ptr;
}
using operational::tags_callback_ptr;
namespace structure {
class const_value_data;
class value_data;
class struct_data;
class variable_data;
class event_data;
class filter_data;
class source_data;
class mapper_data;
class write_context;
} // namespace structure


typedef rx::const_size_vector<structure::variable_data> runtime_variables_type;
typedef rx::const_size_vector<structure::struct_data> runtime_structs_type;
typedef rx::const_size_vector<structure::event_data> runtime_events_type;
typedef rx::const_size_vector<structure::filter_data> runtime_filters_type;
typedef rx::const_size_vector<structure::source_data> runtime_sources_type; 
typedef rx::const_size_vector<structure::mapper_data> runtime_mappers_type;

union rt_value_ref_union
{
	structure::const_value_data* const_value;
	structure::value_data* value;
	structure::variable_data* variable;
    relations::relation_data* relation;
};
enum class rt_value_ref_type
{
	rt_null = 0,
	rt_const_value = 1,
	rt_value = 2,
	rt_variable = 3,
    rt_relation = 4/// !!!!!!/// CHECK switches
};
struct rt_value_ref
{
	rt_value_ref_type ref_type;
	rt_value_ref_union ref_value_ptr;
};

typedef std::unique_ptr<structure::runtime_item> rx_runtime_item_ptr;






class io_capabilities 
{

  public:

      void set_input (bool val);

      void set_output (bool val);

      bool get_input () const;

      bool get_output () const;


  protected:

  private:


      std::bitset<2> settings_;


};







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
	typedef std::stack<structure::variable_data*, std::vector<structure::variable_data*> > variables_type;

  public:

      void push_variable (structure::variable_data* what);

      void pop_variable ();

      structure::variable_data* get_current_variable () const;


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






struct runtime_start_context 
{

      runtime_start_context (structure::runtime_item& root, runtime_process_context* context, ns::rx_directory_resolver* directories);


      runtime_handle_t connect (const string_type& path, uint32_t rate, std::function<void(const values::rx_value&)> callback);


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      runtime_process_context *context;


      ns::rx_directory_resolver* directories;

      rx_time now;

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







class mappers_stack 
{
    typedef std::map<rx_node_id, std::vector<structure::mapper_data*> > mappers_type;

  public:

      void push_mapper (const rx_node_id& id, structure::mapper_data* what);

      void pop_mapper (const rx_node_id& id);

      std::vector<rx_value> get_mapped_values (const rx_node_id& id, const string_type& path);


  protected:

  private:


      mappers_type mappers_;


};


typedef std::map<string_type, runtime_handle_t> binded_tags_type;





struct runtime_init_context 
{

      runtime_init_context (structure::runtime_item& root, const meta::meta_data& meta, runtime_process_context* context, operational::binded_tags* binded, ns::rx_directory_resolver* directories);


      runtime_handle_t get_new_handle ();

      rx_result_with<runtime_handle_t> bind_item (const string_type& path);

      rx_result set_item (const string_type& path, rx_simple_value&& value);


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      runtime_process_context *context;

      operational::binded_tags *tags;

      mappers_stack mappers;


      const meta::meta_data& meta;

      binded_tags_type binded_tags;

      ns::rx_directory_resolver* directories;

      rx_time now;

      rx_reference_ptr anchor;

  public:
      template<typename T>
      rx_result set_item_static(const string_type& path, T&& value)
      {
          rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = set_item(path, std::move(temp));

          return result;
      }
  protected:

  private:


      runtime_handle_t next_handle_;


};


} // namespace runtime
} // namespace rx_platform



#endif
