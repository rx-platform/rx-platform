

/****************************************************************************
*
*  C:\RX\Native\Source\platform_api\rx_runtime.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef rx_runtime_h
#define rx_runtime_h 1



// rx_ptr
#include "lib/rx_ptr.h"

#include "rx_abi.h"
#include "rx_general.h"
#include "lib/rx_values.h"
#include "lib/rx_rt_data.h"
#include "lib/rx_lock.h"

using namespace rx;
using namespace rx::values;
namespace rx_platform_api
{

class rx_source;
class rx_mapper;
class rx_filter;

class rx_struct;
class rx_variable;
class rx_event;

class rx_object;
class rx_domain;
class rx_application;

class rx_port;

class rx_method;
class rx_program;
class rx_display;

class rx_relation;
class rx_data_type;

}

extern "C"
{
    // forward declarations for friend stuff
    rx_result_struct c_init_source(rx_platform_api::rx_source* self, init_ctx_ptr ctx, uint8_t value_type);
    rx_result_struct c_init_mapper(rx_platform_api::rx_mapper* self, init_ctx_ptr ctx, uint8_t value_type);
    rx_result_struct c_init_filter(rx_platform_api::rx_filter* self, init_ctx_ptr ctx);
    rx_result_struct c_start_source(rx_platform_api::rx_source* self, start_ctx_ptr ctx);
    rx_result_struct c_start_mapper(rx_platform_api::rx_mapper* self, start_ctx_ptr ctx);
    rx_result_struct c_start_filter(rx_platform_api::rx_filter* self, start_ctx_ptr ctx);


    rx_result_struct c_init_struct(rx_platform_api::rx_struct* self, init_ctx_ptr ctx);
    rx_result_struct c_init_variable(rx_platform_api::rx_variable* self, init_ctx_ptr ctx);
    rx_result_struct c_init_event(rx_platform_api::rx_event* self, init_ctx_ptr ctx);
    rx_result_struct c_start_struct(rx_platform_api::rx_struct* self, start_ctx_ptr ctx);
    rx_result_struct c_start_variable(rx_platform_api::rx_variable* self, start_ctx_ptr ctx);
    rx_result_struct c_start_event(rx_platform_api::rx_event* self, start_ctx_ptr ctx);

    rx_result_struct c_init_object(rx_platform_api::rx_object* self, init_ctx_ptr ctx);
    rx_result_struct c_init_domain(rx_platform_api::rx_domain* self, init_ctx_ptr ctx);
    rx_result_struct c_init_application(rx_platform_api::rx_application* self, init_ctx_ptr ctx);
    rx_result_struct c_start_object(rx_platform_api::rx_object* self, start_ctx_ptr ctx);
    rx_result_struct c_start_domain(rx_platform_api::rx_domain* self, start_ctx_ptr ctx);
    rx_result_struct c_start_application(rx_platform_api::rx_application* self, start_ctx_ptr ctx);

    rx_result_struct c_init_port(rx_platform_api::rx_port* self, init_ctx_ptr ctx);
    rx_result_struct c_start_port(rx_platform_api::rx_port* self, start_ctx_ptr ctx);


    rx_result_struct c_deinit_object(rx_platform_api::rx_object* self);
    rx_result_struct c_deinit_domain(rx_platform_api::rx_domain* self);
    rx_result_struct c_deinit_application(rx_platform_api::rx_application* self);

    rx_result_struct c_deinit_port(rx_platform_api::rx_port* self);


    rx_result_struct c_init_relation(rx_platform_api::rx_relation* self, init_ctx_ptr ctx);
    rx_result_struct c_start_relation(rx_platform_api::rx_relation* self, start_ctx_ptr ctx, int is_target);

    rx_result_struct c_init_data_type(rx_platform_api::rx_data_type* self, init_ctx_ptr ctx, const struct bytes_value_struct_t* data);
    rx_result_struct c_start_data_type(rx_platform_api::rx_data_type* self, start_ctx_ptr ctx);


    rx_result_struct c_init_method(rx_platform_api::rx_method* self, init_ctx_ptr ctx);
    rx_result_struct c_init_program(rx_platform_api::rx_program* self, init_ctx_ptr ctx);
    rx_result_struct c_start_method(rx_platform_api::rx_method* self, start_ctx_ptr ctx);
    rx_result_struct c_start_program(rx_platform_api::rx_program* self, start_ctx_ptr ctx);
    rx_result_struct c_init_display(rx_platform_api::rx_display* self, init_ctx_ptr ctx);
    rx_result_struct c_start_display(rx_platform_api::rx_display* self, start_ctx_ptr ctx);

    rx_result_struct c_make_target_relation(rx_platform_api::rx_relation* self, struct plugin_relation_runtime_struct_t** target);
    rx_result_struct c_relation_connected(rx_platform_api::rx_relation* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);
    rx_result_struct c_relation_disconnected(rx_platform_api::rx_relation* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);
}


namespace rx_platform_api {

rx_result register_relation_runtime(const rx_node_id& id, rx_relation_constructor_t construct_func, rx_runtime_register_func_t reg_function, rx_runtime_unregister_func_t unreg_function);
template<class T>
rx_result register_relation_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_relation_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_relation_runtime(id, constr_lambda, rx_runtime_register_func_t(), rx_runtime_unregister_func_t());
}
template<class T>
rx_result register_monitored_relation_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_relation_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    rx_runtime_register_func_t reg_func = [](const rx_node_id_struct* id, lock_reference_struct* what)
    {
        using ptr_t = typename T::smart_ptr;
        rx_node_id reg_id(id);
        ptr_t ptr = ptr_t::create_from_pointer(reinterpret_cast<T*>(what->target));
        rx_runtime_manager_lock _;
        T::runtime_instances.emplace(reg_id, ptr);
    };
    rx_runtime_unregister_func_t unreg_func = [](const rx_node_id_struct* id)
    {
        rx_runtime_manager_lock _;
        T::runtime_instances.erase(rx_node_id(id));
    };
    return register_relation_runtime(id, constr_lambda, reg_func, unreg_func);
}


rx_result register_data_type_runtime(const rx_node_id& id, rx_data_type_constructor_t construct_func);
template<class T>
rx_result register_data_type_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_data_type_runtime_struct_t*
        {
            T* temp = new T;
            return &temp->impl_;
        };
    return register_data_type_runtime(id, constr_lambda);
}


template<typename T>
rx_result_with<typename T::smart_ptr> get_runtime_instance(const rx_node_id& id)
{
    auto* container = &T::runtime_instances;
    rx_runtime_manager_lock _;
    auto it = container->find(id);
    if (it == container->end())
    {
        std::ostringstream ss;
        ss << "Instance " << id.to_string()
            << " not registered.";
        return ss.str();
    }
    return it->second;
}






class rx_process_context 
{
    typedef void* implementation_t;

  public:
      rx_process_context();


      void bind (runtime_ctx_ptr ctx);

      bool is_binded () const;

      rx_result get_value (runtime_handle_t handle, values::rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, values::rx_simple_value&& val);

      rx_result write_connected (runtime_handle_t handle, values::rx_simple_value&& val, runtime_transaction_id_t trans_id);

      rx_result execute_connected (runtime_handle_t handle, values::rx_simple_value&& val, runtime_transaction_id_t trans_id);

      void set_async_pending (runtime_handle_t handle, values::rx_simple_value&& val);

      rx_process_context(const rx_process_context&) = delete;
      rx_process_context(rx_process_context&&) = delete;
      rx_process_context& operator=(const rx_process_context&) = delete;
      rx_process_context& operator=(rx_process_context&&) = delete;

      template<typename valT>
      valT get_binded_as(runtime_handle_t handle, const valT& default_value)
      {
          values::rx_simple_value temp_val;
          auto result = this->get_value(handle, temp_val);
          if (result)
          {
              return temp_val.extract_static(default_value);
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

      template<typename valT>
      void set_async_binded_as(runtime_handle_t handle, valT&& value)
      {
          rx_simple_value val;
          val.assign_static<valT>(std::forward<valT>(value));
          this->set_async_pending(handle, std::move(val));
      }
  protected:

  private:


      runtime_ctx_ptr impl_;


};







class rx_init_context 
{

  public:

      rx_result_with<runtime_handle_t> bind_item (const char* path, runtime_ctx_ptr* rt_ctx, bind_callback_data* callback);

      rx_result_with<runtime_handle_t> connect_item (const char* path, uint32_t rate, runtime_ctx_ptr* rt_ctx, connect_callback_data* callback);

      const char* get_current_path ();

      rx_result get_local_value (const string_type& path, values::rx_simple_value& val) const;

      rx_result set_local_value (const string_type& path, rx_simple_value&& value);

      std::vector<rx_simple_value> get_mapping_values (const rx_node_id& id, const string_type& path);

      std::vector<rx_simple_value> get_source_values (const rx_node_id& id, const string_type& path);

      rx_node_id get_node_id ();

      string_type get_path ();

      string_type get_name ();

      string_type get_full_path ();

      rx_result get_data_type (const string_type& path, data::runtime_data_model& data) const;

      template<typename T>
      T get_local_value_as(const string_type& path, const T& default_value)
      {
          values::rx_simple_value temp_val;
          auto result = get_local_value(path, temp_val);
          if (result)
          {
              return temp_val.extract_static(default_value);
          }
          return default_value;
      }
      template<typename T>
      rx_result set_local_value_as(const string_type& path, T&& value)
      {
          rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = set_local_value(path, std::move(temp));

          return result;
      }


      template<typename T>
      typename std::vector<T> get_mapping_values_as(const rx_node_id& id, const string_type& path, const T& default_value)
      {
          using ret_data_t = typename std::vector<T>;

          ret_data_t ret;
          std::vector<rx_simple_value> raw_values = get_mapping_values(id, path);
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
      T get_mapping_cumulative(const rx_node_id& id, const string_type& path, const T& default_value)
      {
          T ret(default_value);
          auto vals = get_mapping_values_as<T>(id, path, default_value);
          for (const auto& one : vals)
          {
              ret += one;
          }
          return ret;
      }
      string_type get_mapping_first_string(const rx_node_id& id, const string_type& path)
      {
          string_type def;
          auto vals = get_mapping_values_as(id, path, def);
          for (const auto& one : vals)
              if (!one.empty())
                  return one;
          return def;
      }
      string_type get_mapping_path_string(const rx_node_id& id, const string_type& path, char delim)
      {
          string_type def;
          auto vals = get_mapping_values_as(id, path, def);
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
      template<typename T>
      typename std::vector<T> get_source_values_as(const rx_node_id& id, const string_type& path, const T& default_value)
      {
          using ret_data_t = typename std::vector<T>;

          ret_data_t ret;
          std::vector<rx_simple_value> raw_values = get_source_values(id, path);
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
      T get_source_cumulative(const rx_node_id& id, const string_type& path, const T& default_value)
      {
          T ret(default_value);
          auto vals = get_source_values_as<T>(id, path, default_value);
          for (const auto& one : vals)
          {
              ret += one;
          }
          return ret;
      }
      string_type get_source_first_string(const rx_node_id& id, const string_type& path)
      {
          string_type def;
          auto vals = get_source_values_as(id, path, def);
          for (const auto& one : vals)
              if (!one.empty())
                  return one;
          return def;
      }
      string_type get_source_path_string(const rx_node_id& id, const string_type& path, char delim)
      {
          string_type def;
          auto vals = get_source_values_as(id, path, def);
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

      rx_init_context(const rx_init_context&) = delete;
      rx_init_context(rx_init_context&&) = delete;
      rx_init_context& operator=(const rx_init_context&) = delete;
      rx_init_context& operator=(rx_init_context&&) = delete;
  protected:

  private:
      rx_init_context (init_ctx_ptr ctx);



      init_ctx_ptr impl_;

      friend rx_result_struct (::c_init_source)(rx_platform_api::rx_source* self, init_ctx_ptr ctx, uint8_t value_type);
      friend rx_result_struct(::c_init_filter)(rx_platform_api::rx_filter* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_mapper)(rx_platform_api::rx_mapper* self, init_ctx_ptr ctx, uint8_t value_type);

      friend rx_result_struct(::c_init_struct)(rx_platform_api::rx_struct* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_variable)(rx_platform_api::rx_variable* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_event)(rx_platform_api::rx_event* self, init_ctx_ptr ctx);

      friend rx_result_struct(::c_init_object)(rx_platform_api::rx_object* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_domain)(rx_platform_api::rx_domain* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_application)(rx_platform_api::rx_application* self, init_ctx_ptr ctx);

      friend rx_result_struct(::c_init_port)(rx_platform_api::rx_port* self, init_ctx_ptr ctx);

      friend rx_result_struct(::c_init_relation)(rx_platform_api::rx_relation* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_data_type)(rx_platform_api::rx_data_type* self, init_ctx_ptr ctx, const struct bytes_value_struct_t* data);

      friend rx_result_struct(::c_init_method)(rx_platform_api::rx_method* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_program)(rx_platform_api::rx_program* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_display)(rx_platform_api::rx_display* self, init_ctx_ptr ctx);
};







class rx_start_context 
{

  public:

      const char* get_current_path ();

      rx_result get_local_value (const string_type& path, values::rx_simple_value& val) const;

      rx_result set_local_value (const string_type& path, rx_simple_value&& value);

      rx_result register_relation_subscriber (const string_type& name, relation_subscriber_data* callback);


      template<typename funcT>
      runtime_handle_t create_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(anchor_, std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_calc_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(anchor_, std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_CALC, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_io_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(anchor_, std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_IO, data, period);
      }
      template<typename T>
      T get_local_value_as(const string_type& path, const T& default_value)
      {
          values::rx_simple_value temp_val;
          auto result = get_local_value(path, temp_val);
          if (result)
          {
              return temp_val.extract_static(default_value);
          }
          return default_value;
      }

      template<typename T>
      rx_result set_local_value_as(const string_type& path, T&& value)
      {
          rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = set_local_value(path, std::move(temp));

          return result;
      }

      rx_start_context(const rx_start_context&) = delete;
      rx_start_context(rx_start_context&&) = delete;
      rx_start_context& operator=(const rx_start_context&) = delete;
      rx_start_context& operator=(rx_start_context&&) = delete;
  protected:

  private:
      rx_start_context (start_ctx_ptr ctx, rx_reference_ptr anchor);


      runtime_handle_t create_timer_internal (int type, callback_data::smart_ptr callback, uint32_t period);



      start_ctx_ptr impl_;

      rx_reference_ptr anchor_;

      friend rx_result_struct(::c_start_source)(rx_platform_api::rx_source* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_filter)(rx_platform_api::rx_filter* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_mapper)(rx_platform_api::rx_mapper* self, start_ctx_ptr ctx);

      friend rx_result_struct(::c_start_struct)(rx_platform_api::rx_struct* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_variable)(rx_platform_api::rx_variable* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_event)(rx_platform_api::rx_event* self, start_ctx_ptr ctx);

      friend rx_result_struct(::c_start_application)(rx_platform_api::rx_application* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_domain)(rx_platform_api::rx_domain* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_object)(rx_platform_api::rx_object* self, start_ctx_ptr ctx);

      friend rx_result_struct(::c_start_port)(rx_platform_api::rx_port* self, start_ctx_ptr ctx);

      friend rx_result_struct(::c_start_relation)(rx_platform_api::rx_relation* self, start_ctx_ptr ctx, int is_target);
      friend rx_result_struct(::c_start_data_type)(rx_platform_api::rx_data_type* self, start_ctx_ptr ctx);

      friend rx_result_struct(::c_start_method)(rx_platform_api::rx_method* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_program)(rx_platform_api::rx_program* self, start_ctx_ptr ctx);
      friend rx_result_struct(::c_start_display)(rx_platform_api::rx_display* self, start_ctx_ptr ctx);
};







class rx_runtime : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(rx_runtime);

  public:

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      runtime_transaction_id_t get_new_unique_id ();


  protected:

      rx_result post_job_internal (int type, callback_data::smart_ptr callback, uint32_t period);

      runtime_handle_t create_timer_internal (int type, callback_data::smart_ptr callback, uint32_t period);


  private:

      void bind_runtime (host_runtime_def_struct* impl, void* target);

      void release_runtime ();



      host_runtime_def_struct* impl_;

      void* target_;

      friend rx_result_struct (::c_init_source)(rx_platform_api::rx_source* self, init_ctx_ptr ctx, uint8_t value_type);
      friend rx_result_struct(::c_init_filter)(rx_platform_api::rx_filter* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_mapper)(rx_platform_api::rx_mapper* self, init_ctx_ptr ctx, uint8_t value_type);

      friend rx_result_struct(::c_init_struct)(rx_platform_api::rx_struct* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_variable)(rx_platform_api::rx_variable* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_event)(rx_platform_api::rx_event* self, init_ctx_ptr ctx);


      friend rx_result_struct(::c_init_object)(rx_platform_api::rx_object* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_domain)(rx_platform_api::rx_domain* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_application)(rx_platform_api::rx_application* self, init_ctx_ptr ctx);

      friend rx_result_struct(::c_init_port)(rx_platform_api::rx_port* self, init_ctx_ptr ctx);
      
      friend rx_result_struct (::c_deinit_object)(rx_platform_api::rx_object* self);
      friend rx_result_struct(::c_deinit_domain)(rx_platform_api::rx_domain* self);
      friend rx_result_struct(::c_deinit_application)(rx_platform_api::rx_application* self);

      friend rx_result_struct(::c_deinit_port)(rx_platform_api::rx_port* self);

      friend rx_result_struct(::c_init_relation)(rx_platform_api::rx_relation* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_data_type)(rx_platform_api::rx_data_type* self, init_ctx_ptr ctx, const struct bytes_value_struct_t* data);

      friend rx_result_struct(::c_init_method)(rx_platform_api::rx_method* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_program)(rx_platform_api::rx_program* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_init_display)(rx_platform_api::rx_display* self, init_ctx_ptr ctx);

      friend rx_result_struct(::c_make_target_relation)(rx_platform_api::rx_relation* self, struct plugin_relation_runtime_struct_t** target);

};






class rx_relation : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_relation);

  public:
      rx_relation();

      ~rx_relation();


      virtual rx_result initialize_relation (rx_init_context& ctx);

      virtual rx_result deinitialize_relation ();

      virtual rx_result start_relation (rx_start_context& ctx, bool is_target);

      virtual rx_result stop_relation (bool is_target);

      virtual rx_relation::smart_ptr make_target_relation ();

      static constexpr rx_item_type type_id = rx_item_type::rx_relation_type;

      template<class T>
      friend rx_result register_relation_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_relation)(rx_platform_api::rx_relation* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_relation)(rx_platform_api::rx_relation* self, start_ctx_ptr ctx, int is_target);

      friend rx_result_struct (::c_make_target_relation)(rx_platform_api::rx_relation* self, struct plugin_relation_runtime_struct_t** target);
      friend rx_result_struct(::c_relation_connected)(rx_platform_api::rx_relation* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);
      friend rx_result_struct(::c_relation_disconnected)(rx_platform_api::rx_relation* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);
  protected:

  private:

      virtual void relation_connected (rx_node_id from, rx_node_id to);

      virtual void relation_disconnected (rx_node_id from, rx_node_id to);



      plugin_relation_runtime_struct impl_;


};






class rx_data_type : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_data_type);

  public:
      rx_data_type();

      ~rx_data_type();


      virtual rx_result initialize_data_type (rx_init_context& ctx, const data::runtime_data_model& data);

      virtual rx_result deinitialize_data_type ();

      virtual rx_result start_data_type (rx_start_context& ctx);

      virtual rx_result stop_data_type ();

      static constexpr rx_item_type type_id = rx_item_type::rx_data_type;
  protected:

  private:


      plugin_data_type_runtime_struct impl_;

      template<class T>
      friend rx_result register_data_type_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_data_type)(rx_platform_api::rx_data_type* self, init_ctx_ptr ctx, const struct bytes_value_struct_t* data);
      friend rx_result_struct(::c_start_data_type)(rx_platform_api::rx_data_type* self, start_ctx_ptr ctx);
};






class rx_relation_subscriber 
{

  public:

      virtual void relation_connected (const char* name, const rx_node_id_struct* id) = 0;

      virtual void relation_disconnected (const char* name) = 0;


  protected:

  private:


};


} // namespace rx_platform_api

namespace rx_platform_api
{


template <typename typeT>
struct connected_value
{
    using callback_t = std::function<void(const typeT&, bool valid)>;
    callback_t callback_;
    typeT value_;
    bool valid_ = false;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;
    connect_callback_data_t callback_data_{ };
public:
    connected_value() = default;
    ~connected_value() = default;
    connected_value(const connected_value&) = delete;
    connected_value(connected_value&&) = delete;
    connected_value& operator=(const connected_value&) = delete;
    connected_value& operator=(connected_value&&) = delete;
    rx_result bind(const string_type& path, rx_init_context& ctx, callback_t callback = callback_t())
    {

        callback_data_.target = this;
        callback_data_.write_callback = nullptr;
        callback_data_.execute_callback = nullptr;
        callback_data_.callback = [](void* target, const struct full_value_type* val)
        {
            connected_value* self = (connected_value*)target;
            rx_value local_val(val);
            self->value_ = local_val.extract_static(self->value_);
            self->valid_ = local_val.is_good();
            if (self->callback_)
            {
                self->callback_(self->value_, self->valid_);
            }
        };

        callback_ = callback;
        runtime_ctx_ptr rt_ctx = 0;
        auto result = ctx.connect_item(path.c_str(), 200, &rt_ctx, &callback_data_);
        if (result)
        {
            ctx_.bind(rt_ctx);
            handle_ = result.move_value();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    connected_value(const typeT& right)
    {
        value_ = right;
    }
    connected_value(typeT&& right)
    {
        value_ = std::move(right);
    }
    connected_value& operator=(typeT right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val;
            temp_val.assign_static<typeT>(std::forward<typeT>(right));
            ctx_.write_connected(handle_, std::move(temp_val), 1);
        }
        return *this;
    }
    operator typeT() const
    {
        return value_;
    }
};


template <typename ...OutArgs>
struct connected_function
{
    template < typename Type>
    Type from_arg(const rx_simple_value& what)
    {
        return what.extract_static<Type>(0);
    }
    bool valid_ = false;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;
    connect_callback_data_t callback_data_{ };
    std::map<runtime_transaction_id_t, typename std::function<void(rx_result, OutArgs...)> > transactions_;
public:
    connected_function() = default;
    ~connected_function() = default;
    connected_function(const connected_function&) = delete;
    connected_function(connected_function&&) = default;
    connected_function& operator=(const connected_function&) = delete;
    connected_function& operator=(connected_function&&) = default;
    rx_result bind(const string_type& path, rx_init_context& ctx)
    {
        callback_data_.target = this;
        callback_data_.execute_callback = [](void* target, struct typed_value_type output, runtime_transaction_id_t trans_id, rx_result_struct result)
            {
                connected_function* self = (connected_function*)target;
                auto it = self->transactions_.find(trans_id);
                if (it != self->transactions_.end())
                {
                    self->callback_execute(std::move(it->second), rx_result(result), rx_simple_value(std::move(output)));
                    self->transactions_.erase(it);
                }
            };


        runtime_ctx_ptr rt_ctx = 0;
        auto result = ctx.connect_item(path.c_str(), 200, &rt_ctx, &callback_data_);
        if (result)
        {
            ctx_.bind(rt_ctx);
            handle_ = result.move_value();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    template<typename funcT, std::size_t... I>
    void invoke_helper(funcT callback, rx_result result, std::vector<rx_simple_value> inputs,
        std::index_sequence<I...>)
    {
        callback(std::move(result), from_arg<OutArgs>(std::move(inputs.at(I)))...);
    }
    template<typename funcT>
    rx_result callback_execute(funcT callback, rx_result result, rx_simple_value args)
    {
        if (args.is_struct())
        {
            std::vector<rx_simple_value> inputs;
            inputs.reserve(args.struct_size());
            for (size_t i = 0; i < args.struct_size(); i++)
            {
                inputs.push_back(args[(int)i]);
            }
            invoke_helper(std::move(callback), std::move(result), std::move(inputs), std::index_sequence_for<OutArgs...>{});
            return true;
        }
        else if (args.is_null())
        {
            callback(std::move(result), OutArgs()...);
            return true;
        }
        else
        {
            return RX_INVALID_ARGUMENT;
        }
    }
    template<typename funcT, typename ...InArgs>
    rx_result operator()(funcT callback, InArgs... in_args)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val = rx_create_value_static(std::forward<InArgs>(in_args)...);
            auto trans_id = rx_get_new_transaction_id();
            auto insert_ret = transactions_.emplace(trans_id, typename std::function<void(rx_result, OutArgs...)>(std::move(callback)));
            auto result = ctx_.execute_connected(handle_, std::move(temp_val), trans_id);
            if (!result)
            {
                transactions_.erase(insert_ret.first);
            }
            return result;
        }
        return RX_NOT_CONNECTED;
    }
};


template <typename typeT>
struct local_value
{
    using callback_t = std::function<void(const typeT&)>;
    callback_t callback_;
    typeT value_;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;
    bind_callback_data callback_data_;
public:
    local_value() = default;
    ~local_value() = default;
    local_value(const local_value&) = delete;
    local_value(local_value&&) = delete;
    local_value& operator=(const local_value&) = delete;
    local_value& operator=(local_value&&) = delete;
    rx_result bind(const string_type& path, rx_init_context& ctx, callback_t callback = callback_t())
    {

        callback_data_.target = this;
        callback_data_.callback = [](void* target, const struct full_value_type* val)
        {
            local_value* self = (local_value*)target;
            rx_value local_val(val);
            self->value_ = local_val.extract_static(self->value_);
            if (self->callback_)
            {
                self->callback_(self->value_);
            }
        };

        callback_ = callback;
        runtime_ctx_ptr rt_ctx = 0;
        auto result = ctx.bind_item(path.c_str(), &rt_ctx, &callback_data_);
        if (result)
        {
            ctx_.bind(rt_ctx);
            handle_ = result.move_value();
            value_ = ctx_.get_binded_as(handle_, value_);
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    local_value(const typeT& right)
    {
        value_ = right;
    }
    local_value(typeT&& right)
    {
        value_ = std::move(right);
    }
    local_value& operator=(typeT right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val;
            temp_val.assign_static<typeT>(std::forward<typeT>(right));
            ctx_.set_value(handle_, std::move(temp_val));
        }
        return *this;
    }
    operator typeT() const
    {
        return value_;
    }
};

struct local_complex_value
{
    using callback_t = std::function<void(const values::rx_simple_value&)>;
    callback_t callback_;
    values::rx_simple_value value_;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;
    bind_callback_data callback_data_;
public:
    local_complex_value() = default;
    ~local_complex_value() = default;
    local_complex_value(const local_complex_value&) = delete;
    local_complex_value(local_complex_value&&) = delete;
    local_complex_value& operator=(const local_complex_value&) = delete;
    local_complex_value& operator=(local_complex_value&&) = delete;
    rx_result bind(const string_type& path, rx_init_context& ctx, callback_t callback = callback_t());
    local_complex_value(const values::rx_simple_value& right);
    local_complex_value(values::rx_simple_value&& right);
    local_complex_value& operator=(values::rx_simple_value right);
    const values::rx_simple_value& value() const;
};


template <bool useIinit>
struct owned_complex_value
{
    values::rx_simple_value value_;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;
    bind_callback_data callback_data_;
    void internal_commit();
public:
    owned_complex_value() = default;
    ~owned_complex_value() = default;
    owned_complex_value(const owned_complex_value&) = delete;
    owned_complex_value(owned_complex_value&&) = delete;
    owned_complex_value& operator=(const owned_complex_value&) = delete;
    owned_complex_value& operator=(owned_complex_value&&) = delete;
    rx_result bind(const string_type& path, rx_init_context& ctx);
    owned_complex_value(const values::rx_simple_value& right);
    owned_complex_value(values::rx_simple_value&& right);
    owned_complex_value& operator=(values::rx_simple_value right);
    const values::rx_simple_value& value() const;
};
template <typename typeT, bool manual = false>
struct owned_value
{
    typeT val_;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;

    void internal_commit()
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            typeT temp(val_);
            ctx_.set_binded_as<typeT>(handle_, std::move(temp));
        }
    }
public:
    owned_value() = default;
    ~owned_value() = default;
    owned_value(const owned_value&) = default;
    owned_value(owned_value&&) = default;
    owned_value& operator=(const owned_value&) = default;
    owned_value& operator=(owned_value&&) = default;
    rx_result bind(const string_type& path, rx_init_context& ctx)
    {
        runtime_ctx_ptr rt_ctx = 0;
        auto result = ctx.bind_item(path.c_str(), &rt_ctx, nullptr);
        if (result)
        {
            ctx_.bind(rt_ctx);
            handle_ = result.move_value();
            if (handle_)
                internal_commit();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    owned_value(const typeT& right)
    {
        val_ = right;
    }
    owned_value(typeT&& right)
    {
        val_ = std::move(right);
    }
    owned_value& operator=(const typeT& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = right;
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    owned_value& operator=(typeT&& right)
    {
        if (ctx_ .is_binded()&& handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = std::move(right);
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    owned_value& operator+=(const typeT& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            val_ += right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    owned_value& operator-=(const typeT& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            val_ -= right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    operator typeT() const
    {
        return val_;
    }
    void commit()
    {
        if constexpr (manual)
        {
            internal_commit();
        }
        else
        {
            RX_ASSERT(false);
        }
    }
};


template <>
struct owned_value<values::rx_simple_value, false>
{
    values::rx_simple_value val_;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;

    void internal_commit()
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            values::rx_simple_value temp(val_);
            ctx_.set_value(handle_, std::move(temp));
        }
    }
public:
    owned_value() = default;
    ~owned_value() = default;
    owned_value(const owned_value&) = default;
    owned_value(owned_value&&) = default;
    owned_value& operator=(const owned_value&) = default;
    owned_value& operator=(owned_value&&) = default;
    rx_result bind(const string_type& path, rx_init_context& ctx)
    {
        runtime_ctx_ptr rt_ctx = 0;
        auto result = ctx.bind_item(path.c_str(), &rt_ctx, nullptr);
        if (result)
        {
            ctx_.bind(rt_ctx);
            handle_ = result.move_value();
            if (handle_)
                internal_commit();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    owned_value(const values::rx_simple_value& right)
    {
        val_ = right;
    }
    owned_value(values::rx_simple_value&& right)
    {
        val_ = std::move(right);
    }
    owned_value& operator=(const values::rx_simple_value& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = right;
                internal_commit();
            }
        }
        return *this;
    }
    owned_value& operator=(values::rx_simple_value&& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = std::move(right);
                internal_commit();
            }
        }
        return *this;
    }
    owned_value& operator+=(const values::rx_simple_value& right)
    {
        RX_ASSERT(false);
        return *this;
    }
    owned_value& operator-=(const values::rx_simple_value& right)
    {
        RX_ASSERT(false);
        return *this;
    }
    operator values::rx_simple_value() const
    {
        return val_;
    }
    void commit()
    {
        RX_ASSERT(false);
    }
};


template <typename typeT, bool manual = false>
struct async_owned_value
{
    typeT val_;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;

    void internal_commit()
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            typeT temp(val_);
            ctx_.set_async_binded_as<typeT>(handle_, std::move(temp));
        }
    }
public:
    async_owned_value() = default;
    ~async_owned_value() = default;
    async_owned_value(const async_owned_value&) = default;
    async_owned_value(async_owned_value&&) = default;
    async_owned_value& operator=(const async_owned_value&) = default;
    async_owned_value& operator=(async_owned_value&&) = default;
    rx_result bind(const string_type& path, rx_init_context& ctx)
    {
        runtime_ctx_ptr rt_ctx = 0;
        auto result = ctx.bind_item(path.c_str(), &rt_ctx, nullptr);
        if (result)
        {
            ctx_.bind(rt_ctx);
            handle_ = result.move_value();
            if (handle_)
                internal_commit();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    async_owned_value(const typeT& right)
    {
        val_ = right;
    }
    async_owned_value(typeT&& right)
    {
        val_ = std::move(right);
    }
    async_owned_value& operator=(const typeT& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = right;
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    async_owned_value& operator=(typeT&& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = std::move(right);
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    async_owned_value& operator+=(const typeT& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            val_ += right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    async_owned_value& operator-=(const typeT& right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            val_ -= right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    operator typeT() const
    {
        return val_;
    }
    void commit()
    {
        if constexpr (manual)
        {
            internal_commit();
        }
        else
        {
            RX_ASSERT(false);
        }
    }
};


template <typename typeT, typename lockT = locks::slim_lock>
struct async_local_value
{
    using callback_t = std::function<void(const typeT&)>;
    callback_t callback_;
    lockT lock_;
    typeT value_;
    runtime_handle_t handle_ = 0;
    rx_process_context ctx_;
    bind_callback_data callback_data_;
public:
    async_local_value() = default;
    ~async_local_value() = default;
    async_local_value(const async_local_value&) = delete;
    async_local_value(async_local_value&&) = default;
    async_local_value& operator=(const async_local_value&) = delete;
    async_local_value& operator=(async_local_value&&) = default;
    rx_result bind(const string_type& path, rx_init_context& ctx, callback_t callback = callback_t())
    {
        callback_data_.target = this;
        callback_data_.callback = [](void* target, const struct full_value_type* val)
        {
            async_local_value* self = (async_local_value*)target;
            rx_value local_val(val);
            {
                locks::auto_lock_t<lockT> _(&self->lock_);
                self->value_ = local_val.extract_static(self->value_);
            }
            if (self->callback_)
            {
                self->callback_(self->value_);
            }
        };

        callback_ = callback;
        runtime_ctx_ptr rt_ctx = 0;
        auto result = ctx.bind_item(path.c_str(), &rt_ctx, &callback_data_);
        if (result)
        {
            ctx_.bind(rt_ctx);
            handle_ = result.move_value();
            value_ = ctx_.get_binded_as(handle_, value_);
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    async_local_value(const typeT& right)
    {
        value_ = right;
    }
    async_local_value(typeT&& right)
    {
        value_ = std::move(right);
    }
    async_local_value& operator=(typeT right)
    {
        if (ctx_.is_binded() && handle_)// just in case both of them...
        {
            typeT temp(right);
            ctx_.set_async_binded_as<typeT>(handle_, std::move(temp));
        }
        return *this;
    }
    operator typeT() const
    {
        locks::const_auto_lock_t<lockT> _(&lock_);
        return value_;
    }
};
}


#endif
