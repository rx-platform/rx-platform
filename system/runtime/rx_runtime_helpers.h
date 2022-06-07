

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.h
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


#ifndef rx_runtime_helpers_h
#define rx_runtime_helpers_h 1


#include "lib/rx_values.h"
#include "system/storage_base/rx_storage.h"
#include "system/server/rx_log_macros.h"
#include "system/threads/rx_job.h"
#include "platform_api/rx_abi.h"
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
class variable_data;
class runtime_item;
class source_data;
class mapper_data;
} // namespace structure

namespace algorithms {
template <class typeT> class runtime_holder;
} // namespace algorithms

class relation_subscriber;
class runtime_process_context;
namespace relations {
class relations_holder;
} // namespace relations

namespace tag_blocks {
class binded_tags;

} // namespace tag_blocks
} // namespace runtime
} // namespace rx_platform


#include "lib/rx_const_size_vector.h"


namespace rx_platform {
namespace threads
{
class job_thread;
}
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
class basic_types_algorithm;
template <class typeT>
class object_data_algorithm;
template <class typeT>
class meta_blocks_algorithm;
class relation_blocks_algorithm;
class complex_data_algorithm;
}
}
namespace runtime {

struct write_data;
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
class runtime_relation_algorithms;
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


enum subscription_trigger_type
{
	subscription_trigger_periodic = 0,
	subscription_trigger_critical = 1,

	max_trigger_type = 1
};
enum class runtime_status_type
{
    info = 0,
    warning = 1,
    error = 2
};
struct runtime_status_record
{
    string_type message;
};
struct runtime_status_data
{
    runtime_status_type type;
    string_type path;
    runtime_status_record data;
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
class relation_value_data;
class relation_runtime;
}


typedef rx::pointers::reference<blocks::struct_runtime> struct_runtime_ptr;
typedef rx::pointers::reference<blocks::variable_runtime> variable_runtime_ptr;
typedef rx::pointers::reference<blocks::source_runtime> source_runtime_ptr;
typedef rx::pointers::reference<blocks::mapper_runtime> mapper_runtime_ptr;
typedef rx::pointers::reference<blocks::filter_runtime> filter_runtime_ptr;
typedef rx::pointers::reference<blocks::event_runtime> event_runtime_ptr;
typedef rx::pointers::reference<relations::relation_runtime> relation_runtime_ptr;

namespace tag_blocks
{
class rx_tags_callback;
typedef rx_reference<rx_tags_callback> tags_callback_ptr;
typedef std::function<void(const rx_value&)> binded_callback_t;
}
using tag_blocks::tags_callback_ptr;
namespace structure {
template<typename typeT>
class array_wrapper;
class const_value_data;
class value_data;
class full_value_data;
class struct_data;
class variable_data;
class event_data;
class filter_data;
class source_data;
class mapper_data;
class write_context;
} // namespace structure

namespace logic_blocks
{
class method_data;
class program_data;
}


typedef rx::const_size_vector<structure::array_wrapper<structure::variable_data> > runtime_variables_type;
typedef rx::const_size_vector< structure::array_wrapper<structure::struct_data> > runtime_structs_type;
typedef rx::const_size_vector<structure::event_data> runtime_events_type;
typedef rx::const_size_vector<structure::filter_data> runtime_filters_type;
typedef rx::const_size_vector<structure::source_data> runtime_sources_type;
typedef rx::const_size_vector<structure::mapper_data> runtime_mappers_type;

union rt_value_ref_union
{
	structure::const_value_data* const_value;
	structure::value_data* value;
    structure::full_value_data* full_value;
	structure::variable_data* variable;
    logic_blocks::method_data* method;
    relations::relation_data* relation;
    relations::relation_value_data* relation_value;
};
enum class rt_value_ref_type
{
	rt_null = 0,
	rt_const_value = 1,
	rt_value = 2,
    rt_full_value = 3,
	rt_variable = 4,
    rt_method = 5,
    rt_relation = 6,
    rt_relation_value = 7
};
struct rt_value_ref
{
	rt_value_ref_type ref_type;
	rt_value_ref_union ref_value_ptr;
};

typedef std::unique_ptr<structure::runtime_item> rx_runtime_item_ptr;



struct update_item
{
    runtime_handle_t handle;
    rx_value value;
};

struct write_result_item
{
    runtime_handle_t handle;
    rx_result result;
};
struct write_result_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    rx_result result;
};

struct write_tag_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    rx_simple_value value;
    tags_callback_ptr callback;
};

struct execute_result_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    rx_result result;
    data::runtime_values_data data;
};

struct execute_tag_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    data::runtime_values_data data;
    tags_callback_ptr callback;
    rx_security_handle_t identity;
};






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

      runtime_deinit_context (const meta::meta_data& meta_data);


      variables_stack variables;


      const meta::meta_data& meta;

  public:

  protected:

  private:


};







struct runtime_start_context 
{

      runtime_start_context (structure::runtime_item& root, runtime_process_context* context, tag_blocks::binded_tags* binded, ns::rx_directory_resolver* directories, relations::relations_holder* relations, threads::job_thread* jobs_queue);


      runtime_handle_t connect (const string_type& path, uint32_t rate, std::function<void(const values::rx_value&)> callback);

      rx_result register_relation_subscriber (const string_type& name, relation_subscriber* who);

      rx_result register_extern_relation_subscriber (const string_type& name, relation_subscriber_data* who);

      rx_result set_item (const string_type& path, rx_simple_value&& value);

      rx_result get_item (const string_type& path, rx_simple_value& val);

      void add_periodic_job (jobs::periodic_job::smart_ptr job);

      void add_calc_periodic_job (jobs::periodic_job::smart_ptr job);

      void add_io_periodic_job (jobs::periodic_job::smart_ptr job);


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      runtime_process_context *context;

      tag_blocks::binded_tags *tags;


      ns::rx_directory_resolver* directories;

      rx_time now;

      bool simulation;

      threads::job_thread* queue;

  public:
      template<typename funcT, typename... Args>
      rx_timer_ptr create_timer_function(rx_reference_ptr anchor, funcT&& func, Args&&... args)
      {
          auto job = rx_create_timer_job<funcT, Args...>()(anchor, std::forward<funcT>(func), std::forward<Args>(args)...);
          add_periodic_job(job);
          return job;
      }
      template<typename funcT, typename... Args>
      rx_timer_ptr create_callculation_timer_function(rx_reference_ptr anchor, funcT&& func, Args&&... args)
      {
          auto job = rx_create_timer_job<funcT, Args...>()(anchor, std::forward<funcT>(func), std::forward<Args>(args)...);
          add_calc_periodic_job(job);
          return job;
      }
      template<typename funcT, typename... Args>
      rx_timer_ptr create_io_timer_function(rx_reference_ptr anchor, funcT&& func, Args&&... args)
      {
          auto job = rx_create_timer_job<funcT, Args...>()(anchor, std::forward<funcT>(func), std::forward<Args>(args)...);
          add_io_periodic_job(job);
          return job;
      }
      template<typename T>
      rx_result set_item_static(const string_type& path, T&& value)
      {
          rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = set_item(path, std::move(temp));

          return result;
      }
      template<typename T>
      T get_item_static(const string_type& path, const T& def = T(0))
      {
          rx_simple_value temp;
          auto result = get_item(path, temp);
          if (result)
          {
              return temp.extract_static<T>(def);
          }
          else
          {
              return def;
          }
      }
  protected:

  private:


      relations::relations_holder *relations_;


};






struct runtime_stop_context 
{

      runtime_stop_context (const meta::meta_data& meta_data, runtime_process_context* context);


      variables_stack variables;

      runtime_process_context *context;


      const meta::meta_data& meta;

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

      std::vector<rx_simple_value> get_mapping_values (const rx_node_id& id, const string_type& path);


  protected:

  private:


      mappers_type mappers_;


};







class sources_stack 
{
    typedef std::map<rx_node_id, std::vector<structure::source_data*> > sources_type;

  public:

      void push_source (const rx_node_id& id, structure::source_data* what);

      void pop_source (const rx_node_id& id);

      std::vector<rx_simple_value> get_source_values (const rx_node_id& id, const string_type& path);


  protected:

  private:


      sources_type sources_;


};


typedef std::map<string_type, runtime_handle_t> binded_tags_type;





struct runtime_init_context 
{

      runtime_init_context (structure::runtime_item& root, const meta::meta_data& meta, runtime_process_context* context, tag_blocks::binded_tags* binded, ns::rx_directory_resolver* directories, rx_item_type type);


      runtime_handle_t get_new_handle ();

      rx_result_with<runtime_handle_t> bind_item (const string_type& path, tag_blocks::binded_callback_t callback);

      rx_result set_item (const string_type& path, rx_simple_value&& value);

      rx_result get_item (const string_type& path, rx_simple_value& val);


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      runtime_process_context *context;

      tag_blocks::binded_tags *tags;

      mappers_stack mappers;

      sources_stack sources;


      const meta::meta_data& meta;

      binded_tags_type binded_tags;

      ns::rx_directory_resolver* directories;

      rx_time now;

      rx_reference_ptr anchor;

      rx_item_type item_type;

  public:
      template<typename T>
      rx_result set_item_static(const string_type& path, T&& value)
      {
          rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = set_item(path, std::move(temp));

          return result;
      }
      template<typename T>
      T get_item_static(const string_type& path, const T& def = T(0))
      {
          rx_simple_value temp;
          auto result = get_item(path, temp);
          if (result)
          {
              return temp.extract_static<T>(def);
          }
          else
          {
              return def;
          }
      }
  protected:

  private:


      runtime_handle_t next_handle_;


};


} // namespace runtime
} // namespace rx_platform



#endif
