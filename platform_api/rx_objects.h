

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_objects.h
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


#ifndef rx_objects_h
#define rx_objects_h 1



// rx_runtime
#include "rx_runtime.h"



namespace rx_platform_api {





class rx_object : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_object);

  public:
      rx_object();

      ~rx_object();


      virtual rx_result initialize_object (rx_init_context& ctx);

      virtual rx_result start_object (rx_start_context& ctx);

      virtual rx_result stop_object ();

      virtual rx_result deinitialize_object ();

      static constexpr rx_item_type type_id = rx_item_type::rx_object_type;
      static constexpr rx_item_type runtime_type_id = rx_item_type::rx_object;

      template<typename funcT>
      runtime_handle_t post_job(funcT func, uint32_t period = 0)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return post_job_internal(RX_JOB_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t post_slow_job(funcT func, uint32_t period = 0)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return post_job_internal(RX_JOB_SLOW, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_calc_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_CALC, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_io_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_IO, data, period);
      }
  protected:

  private:


      plugin_object_runtime_struct impl_;

      template<class T>
      friend rx_result register_object_runtime(const rx_node_id& id);
      template<class T>
      friend rx_result register_singleton_runtime(const rx_node_id& id);
      template<class T>
      friend rx_result register_monitored_object_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_object)(rx_platform_api::rx_object* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_object)(rx_platform_api::rx_object* self, start_ctx_ptr ctx);
};

rx_result register_object_runtime(const rx_node_id& id, rx_object_constructor_t construct_func, rx_runtime_register_func_t reg_function, rx_runtime_unregister_func_t unreg_function);
template<class T>
rx_result register_object_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_object_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_object_runtime(id, constr_lambda, rx_runtime_register_func_t(), rx_runtime_unregister_func_t());
}

template<class T>
rx_result register_singleton_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_object_runtime_struct_t*
    {
        return &T::instance()->impl_;
    };
    return register_object_runtime(id, constr_lambda, rx_runtime_register_func_t(), rx_runtime_unregister_func_t());
}

template<class T>
rx_result register_monitored_object_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_object_runtime_struct_t*
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
    return register_object_runtime(id, constr_lambda, reg_func, unreg_func);
}





class rx_application : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_application);

  public:
      rx_application();

      ~rx_application();


      virtual rx_result initialize_application (rx_init_context& ctx);

      virtual rx_result start_application (rx_start_context& ctx);

      virtual rx_result stop_application ();

      virtual rx_result deinitialize_application ();

      static constexpr rx_item_type type_id = rx_item_type::rx_application_type;
      static constexpr rx_item_type runtime_type_id = rx_item_type::rx_application;

      template<typename funcT>
      runtime_handle_t post_job(funcT func, uint32_t period = 0)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return post_job_internal(RX_JOB_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t post_slow_job(funcT func, uint32_t period = 0)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return post_job_internal(RX_JOB_SLOW, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_calc_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_CALC, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_io_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_IO, data, period);
      }
  protected:

  private:


      plugin_application_runtime_struct impl_;

      template<class T>
      friend rx_result register_application_runtime(const rx_node_id& id);
      template<class T>
      friend rx_result register_monitored_application_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_application)(rx_platform_api::rx_application* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_application)(rx_platform_api::rx_application* self, start_ctx_ptr ctx);
};

rx_result register_application_runtime(const rx_node_id& id, rx_application_constructor_t construct_func, rx_runtime_register_func_t reg_function, rx_runtime_unregister_func_t unreg_function);
template<class T>
rx_result register_application_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_application_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_application_runtime(id, constr_lambda, rx_runtime_register_func_t(), rx_runtime_unregister_func_t());
}
template<class T>
rx_result register_monitored_application_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_application_runtime_struct_t*
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
    return register_application_runtime(id, constr_lambda, reg_func, unreg_func);
}





class rx_domain : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_domain);

  public:
      rx_domain();

      ~rx_domain();


      virtual rx_result initialize_domain (rx_init_context& ctx);

      virtual rx_result start_domain (rx_start_context& ctx);

      virtual rx_result stop_domain ();

      virtual rx_result deinitialize_domain ();


      static constexpr rx_item_type type_id = rx_item_type::rx_domain_type;
      static constexpr rx_item_type runtime_type_id = rx_item_type::rx_domain;

      template<typename funcT>
      runtime_handle_t post_job(funcT func, uint32_t period = 0)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return post_job_internal(RX_JOB_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t post_slow_job(funcT func, uint32_t period = 0)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return post_job_internal(RX_JOB_SLOW, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_calc_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_CALC, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_io_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_IO, data, period);
      }
  protected:

  private:


      plugin_domain_runtime_struct impl_;

      template<class T>
      friend rx_result register_domain_runtime(const rx_node_id& id);
      template<class T>
      friend rx_result register_monitored_domain_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_domain)(rx_platform_api::rx_domain* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_domain)(rx_platform_api::rx_domain* self, start_ctx_ptr ctx);
};

rx_result register_domain_runtime(const rx_node_id& id, rx_domain_constructor_t construct_func, rx_runtime_register_func_t reg_function, rx_runtime_unregister_func_t unreg_function);
template<class T>
rx_result register_domain_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_domain_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_domain_runtime(id, constr_lambda, rx_runtime_register_func_t(), rx_runtime_unregister_func_t());
}
template<class T>
rx_result register_monitored_domain_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_domain_runtime_struct_t*
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
    return register_domain_runtime(id, constr_lambda, reg_func, unreg_func);
}

} // namespace rx_platform_api



#endif
