

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.h
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


#ifndef rx_runtime_helpers_h
#define rx_runtime_helpers_h 1


#include "system/rx_platform_typedefs.h"
#include "lib/rx_values.h"
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

namespace rx_internal
{
namespace sys_runtime
{
namespace data_source
{
class callback_value_point;
}
}
}




namespace rx_platform {
namespace runtime {
namespace algorithms {
template <class typeT> class runtime_holder;
} // namespace algorithms

namespace relations {
class relations_holder;
} // namespace relations

class relation_subscriber;
class runtime_process_context;
namespace tag_blocks {
class binded_tags;
} // namespace tag_blocks
} // namespace runtime

namespace ns {
class rx_directory_resolver;
} // namespace ns

namespace runtime {
namespace structure {
class mapper_data;
class variable_data;
class runtime_item;
class source_data;

} // namespace structure
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {
namespace structure
{
class block_data;
}

struct update_item
{
    runtime_handle_t handle;
    rx_value value;
};


struct simple_event_item
{    
    rx_simple_value value;
    string_type event_id;
    rx_time ts;
};


struct data_event_item
{
    data::runtime_values_data data;
    string_type event_id;
    rx_time ts;
};


struct write_result_item
{
    runtime_handle_t handle;
    rx_result result;
    uint32_t signal_level;
};
struct write_result_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    rx_result result;
    uint32_t signal_level;
};

struct write_tag_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    bool test;
    std::variant<values::rx_simple_value, data::runtime_values_data> data;
    tags_callback_ptr callback;
    rx_security_handle_t identity;
};

struct execute_result_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    rx_result result;
    std::variant<values::rx_simple_value, data::runtime_values_data> data;
    uint32_t signal_level;
};

struct execute_tag_data
{
    runtime_transaction_id_t transaction_id;
    runtime_handle_t item;
    bool test;
    std::variant<values::rx_simple_value, data::runtime_values_data> data;
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

      void set_complex (bool val);

      bool get_complex () const;

      void set_in_method (bool val);

      bool get_in_method () const;

      void set_in_event (bool val);

      bool get_in_event () const;

      void set_in_source (bool val);

      bool get_in_source () const;

      void set_in_mapper (bool val);

      bool get_in_mapper () const;

      void set_is_root (bool val);

      bool get_is_root () const;


  protected:

  private:


      std::bitset<16> settings_;


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
    typedef std::map<string_type, data::runtime_data_model> data_types_type;

  public:
      runtime_structure_resolver (structure::runtime_item& root);


      void push_item (structure::runtime_item& item);

      void pop_item ();

      structure::runtime_item& get_current_item ();

      structure::runtime_item& get_root ();

      data::runtime_data_model get_data_type (const string_type& path);

      void register_data_type (const string_type& path, data::runtime_data_model data);


  protected:

  private:


      runtime_items_type items_;

      std::reference_wrapper<structure::runtime_item> root_;

      data_types_type data_types_;


};


typedef std::variant<structure::variable_data*, structure::variable_block_data*> variable_stack_entry;





class variables_stack 
{
	typedef std::stack<variable_stack_entry, std::vector<variable_stack_entry> > variables_type;

  public:

      void push_variable (structure::variable_data* what);

      void push_variable (structure::variable_block_data* what);

      void pop_variable ();

      variable_stack_entry get_current_variable () const;


  protected:

  private:


      variables_type variables_;


};






struct runtime_deinit_context 
{

      runtime_deinit_context (const meta_data& meta_data);


      variables_stack variables;


      const meta_data& meta;

  public:

  protected:

  private:


};


typedef std::vector<std::pair<structure::const_value_data*, tag_blocks::binded_callback_t> > const_callbacks_type;
struct pending_data_t
{
    string_type path;
    uint32_t rate;
    std::unique_ptr<rx_internal::sys_runtime::data_source::callback_value_point> value_pt;
};
typedef std::map<runtime_handle_t, pending_data_t> pending_connections_type;

struct status_detail_t
{
    string_type item;
    uint32_t state;
    string_type msg;
};
typedef std::vector<status_detail_t> status_details_type;
struct  status_data_type
{
    rx_mode_type mode;
    status_details_type details;
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

      rx_value& get_current_variable_value ();

      rx_time now ();


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      runtime_process_context *context;

      tag_blocks::binded_tags *tags;


      ns::rx_directory_resolver* directories;

      bool simulation;

      threads::job_thread* queue;

      const_callbacks_type const_callbacks;

      logic_blocks::method_data *method;

      pending_connections_type pending_connections;

      status_data_type status_data;

      std::function<platform_item_ptr()> get_platform_item;

  public:
      runtime_start_context(const runtime_start_context&) = delete;
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

      runtime_stop_context (const meta_data& meta_data, runtime_process_context* context);


      variables_stack variables;

      runtime_process_context *context;


      const meta_data& meta;

  public:

  protected:

  private:


};







class mappers_stack 
{
    typedef std::map<rx_node_id, std::vector<structure::mapper_data*> > mappers_type;

  public:
      mappers_stack();


      void push_mapper (const rx_node_id& id, structure::mapper_data* what);

      void pop_mapper (const rx_node_id& id);

      std::vector<rx_simple_value> get_mapping_values (const rx_node_id& id, const string_type& path);


      structure::mapper_data* mapper;


  protected:

  private:


      mappers_type mappers_;


};







class sources_stack 
{
    typedef std::map<rx_node_id, std::vector<structure::source_data*> > sources_type;

  public:
      sources_stack();


      void push_source (const rx_node_id& id, structure::source_data* what);

      void pop_source (const rx_node_id& id);

      std::vector<rx_simple_value> get_source_values (const rx_node_id& id, const string_type& path);


      structure::source_data* source;


  protected:

  private:


      sources_type sources_;


};


typedef std::map<string_type, runtime_handle_t> binded_tags_type;





struct runtime_init_context 
{

      runtime_init_context (structure::runtime_item& root, const meta_data& meta, runtime_process_context* context, tag_blocks::binded_tags* binded, ns::rx_directory_resolver* directories, rx_item_type type);


      runtime_handle_t get_new_handle ();

      rx_result_with<runtime_handle_t> bind_item (const string_type& path, tag_blocks::binded_callback_t callback);

      rx_result_with<runtime_handle_t> bind_item (const string_type& path, tag_blocks::binded_callback_t callback, tag_blocks::write_callback_t write_callback);

      rx_result set_item (const string_type& path, rx_simple_value&& value);

      rx_result get_item (const string_type& path, rx_simple_value& val);

      rx_result_with<runtime_handle_t> connect_item (const string_type& path, uint32_t rate, tag_blocks::binded_callback_t callback, tag_blocks::binded_write_result_callback_t write_callback, tag_blocks::binded_execute_result_callback_t execute_callback);

      rx_time now ();

      data::runtime_data_model get_data_model (string_view_type path);


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      runtime_process_context *context;

      tag_blocks::binded_tags *tags;

      mappers_stack mappers;

      sources_stack sources;


      const meta_data& meta;

      binded_tags_type binded_tags;

      ns::rx_directory_resolver* directories;

      rx_reference_ptr anchor;

      rx_item_type item_type;

      const_callbacks_type const_callbacks;

      logic_blocks::method_data *method;

      structure::event_data *event;

      pending_connections_type pending_connections;

      status_data_type status_data;

      std::unique_ptr<structure::block_data> model;

      size_t points_count;

  public:
      runtime_init_context(const runtime_init_context&) = delete;
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
