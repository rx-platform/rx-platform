

/****************************************************************************
*
*  system\runtime\rx_blocks.h
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


#ifndef rx_blocks_h
#define rx_blocks_h 1



// rx_ptr
#include "lib/rx_ptr.h"

#include "system/server/rx_ns.h"
#include "system/callbacks/rx_callback.h"
#include "system/runtime/rx_runtime_helpers.h"
#include "system/runtime/rx_rt_struct.h"
using namespace rx_platform::ns;
using namespace rx::values;
using rx_platform::runtime::write_data;


namespace rx_platform {

namespace runtime {

namespace blocks {





class filter_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
filter runtime. basic implementation of an filter runtime");

	DECLARE_REFERENCE_PTR(filter_runtime);
    friend class structure::filter_data;

  public:
      filter_runtime();

      virtual ~filter_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_filter (runtime::runtime_init_context& ctx);

      virtual rx_result start_filter (runtime::runtime_start_context& ctx);

      virtual rx_result stop_filter (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_filter (runtime::runtime_deinit_context& ctx);


      static string_type type_name;


  protected:

      rx_result get_value (runtime_handle_t handle, values::rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, values::rx_simple_value&& val);

      template<typename valT>
      valT get_binded_as(runtime_handle_t handle, const valT& default_value) const
      {
          values::rx_simple_value temp_val;
          auto result = get_value(handle, temp_val);
          if (result)
          {
              return temp_val.extract_static<valT>(default_value);
          }
          return default_value;
      }
      template<typename valT>
      void set_binded_as(runtime_handle_t handle, valT&& value)
      {
          values::rx_simple_value temp_val;
          temp_val.assign_static<valT>(std::forward<valT>(value));
          auto result = set_value(handle, std::move(temp_val));
      }
  private:

      virtual bool supports_input () const;

      virtual bool supports_output () const;

      virtual rx_result filter_input (rx_value& val);

      virtual rx_result filter_output (rx_simple_value& val);



      structure::filter_data* container_;


};






class source_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
source runtime. basic implementation of an source runtime");

	DECLARE_REFERENCE_PTR(source_runtime);
    friend class structure::source_data;

  public:
      source_runtime();

      virtual ~source_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_source (runtime::runtime_init_context& ctx);

      virtual rx_result start_source (runtime::runtime_start_context& ctx);

      virtual rx_result stop_source (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_source (runtime::runtime_deinit_context& ctx);

      bool is_input () const;

      bool is_output () const;

      threads::job_thread* get_jobs_queue ();

      void add_periodic_job (jobs::periodic_job::smart_ptr job);


      static string_type type_name;

      template<typename funcT, typename... Args>
      rx_timer_ptr create_timer_function(funcT&& func, Args&&... args)
      {
          auto job = rx_create_timer_job<funcT, Args...>()(smart_this(), std::forward<funcT>(func), std::forward<Args>(args)...);
          add_periodic_job(job);
          return job;
      }
  protected:

      rx_result source_value_changed (rx_value&& val);

      void source_result_received (rx_result&& result, runtime_transaction_id_t id);

      std::vector<rx_value> get_sourced_values (runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path) const;


      rx_value_t get_value_type () const;

      template<typename T>
      typename std::vector<T> get_sourced_values_as(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path, const T& default_value)
      {
          using ret_data_t = typename std::vector<T>;

          ret_data_t ret;
          std::vector<rx_value> raw_values = get_sourced_values(ctx, id, path);
          if (!raw_values.empty())
          {
              for (const auto& raw : raw_values)
              {
                  ret.emplace_back(raw.extract_static<T>(default_value));
              }
          }
          return ret;
      }
      template<typename T>
      T get_sourced_cumulative(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path, const T& default_value)
      {
          T ret(default_value);
          auto vals = get_sourced_values_as<T>(ctx, id, path, default_value);
          for (const auto& one : vals)
          {
              ret += one;
          }
          return ret;
      }
      string_type get_sourced_first_string(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path)
      {
          string_type def;
          auto vals = get_sourced_values_as(ctx, id, path, def);
          for (const auto& one : vals)
              if (!one.empty())
                  return one;
          return def;
      }
      string_type get_sourced_path_string(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path, char delim)
      {
          string_type def;
          auto vals = get_sourced_values_as(ctx, id, path, def);
          std::ostringstream stream;
          bool had_delim = true;
          for (const auto& one : vals)
          {
              if (!one.empty())
              {
                  if (!had_delim)
                      stream << delim;
                  stream << one;
                  had_delim = (*one.rbegin() == delim);
              }
          }
          return stream.str();
      }
  private:

      virtual bool supports_input () const;

      virtual bool supports_output () const;

      virtual rx_result source_write (write_data&& data, runtime_process_context* ctx);



      structure::source_data* container_;

      rx_value_t value_type_;


};






class struct_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
struct runtime. basic implementation of an struct runtime");

	DECLARE_REFERENCE_PTR(struct_runtime);
    friend class structure::struct_data;

  public:
      struct_runtime();

      virtual ~struct_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_struct (runtime::runtime_init_context& ctx);

      virtual rx_result start_struct (runtime::runtime_start_context& ctx);

      virtual rx_result stop_struct (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_struct (runtime::runtime_deinit_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class variable_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
variable runtime. basic implementation of an variable runtime");

	DECLARE_REFERENCE_PTR(variable_runtime);
    friend class structure::variable_data;

  public:
      variable_runtime();

      virtual ~variable_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_variable (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_variable (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_variable (runtime::runtime_start_context& ctx);

      virtual rx_result stop_variable (runtime::runtime_stop_context& ctx);


      static string_type type_name;


  protected:

      void process_variable (runtime_process_context* ctx);

      void variable_result_pending (runtime_process_context* ctx, rx_result&& result, runtime_transaction_id_t id);


  private:

      virtual rx_value get_variable_input (runtime_process_context* ctx, runtime_sources_type& sources);

      virtual rx_result variable_write (write_data&& data, runtime_process_context* ctx, runtime_sources_type& sources);



      structure::variable_data* container_;


};






class event_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
event runtime. basic implementation of an event runtime");

	DECLARE_REFERENCE_PTR(event_runtime);
    friend class structure::event_data;

  public:
      event_runtime();

      virtual ~event_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_event (runtime::runtime_init_context& ctx);

      virtual rx_result start_event (runtime::runtime_start_context& ctx);

      virtual rx_result stop_event (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_event (runtime::runtime_deinit_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class mapper_start_context 
{

  public:

      rx_value& init_value ();


  protected:

  private:


      rx_value* initial_value_;


};






class mapper_stop_context 
{

  public:

  protected:

  private:


};







class mapper_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
mapper runtime. basic implementation of an mapper runtime");

	DECLARE_REFERENCE_PTR(mapper_runtime);
    friend class structure::mapper_data;

  public:
      mapper_runtime();

      virtual ~mapper_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      virtual rx_result start_mapper (runtime::runtime_start_context& ctx);

      virtual rx_result stop_mapper (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_mapper (runtime::runtime_deinit_context& ctx);

      bool can_read () const;

      bool can_write () const;

      threads::job_thread* get_jobs_queue ();


      static string_type type_name;


  protected:

      void mapper_write_pending (write_data&& data);

      void map_current_value () const;

      std::vector<rx_value> get_mapped_values (runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path) const;


      rx_value_t get_value_type () const;

      template<typename T>
      typename std::vector<T> get_mapped_values_as(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path, const T& default_value)
      {
          using ret_data_t = typename std::vector<T>;

          ret_data_t ret;
          std::vector<rx_value> raw_values = get_mapped_values(ctx, id, path);
          if (!raw_values.empty())
          {
              for (const auto& raw : raw_values)
              {
                  ret.emplace_back(raw.extract_static<T>(default_value));
              }
          }
          return ret;
      }

      template<typename T>
      T get_mapped_cumulative(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path, const T& default_value)
      {
          T ret(default_value);
          auto vals = get_mapped_values_as<T>(ctx, id, path, default_value);
          for (const auto& one : vals)
          {
              ret += one;
          }
          return ret;
      }
      string_type get_mapped_first_string(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path)
      {
          string_type def;
          auto vals = get_mapped_values_as(ctx, id, path, def);
          for (const auto& one : vals)
              if (!one.empty())
                  return one;
          return def;
      }
      string_type get_mapped_path_string(runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path, char delim)
      {
          string_type def;
          auto vals = get_mapped_values_as(ctx, id, path, def);
          std::ostringstream stream;
          bool had_delim = true;
          for (const auto& one : vals)
          {
              if (!one.empty())
              {
                  if (!had_delim)
                      stream << delim;
                  stream << one;
                  had_delim = (*one.rbegin() == delim);
              }
          }
          return stream.str();
      }
  private:

      virtual bool supports_read () const;

      virtual bool supports_write () const;

      virtual void mapped_value_changed (rx_value&& val);

      virtual void mapper_result_received (rx_result&& result, runtime_transaction_id_t id);



      structure::mapper_data* container_;

      rx_value_t value_type_;


};


} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
