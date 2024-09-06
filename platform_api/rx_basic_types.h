

/****************************************************************************
*
*  C:\RX\Native\Source\platform_api\rx_basic_types.h
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


#ifndef rx_basic_types_h
#define rx_basic_types_h 1



// rx_runtime
#include "rx_runtime.h"



namespace rx_platform_api {





class rx_struct : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_struct);

  public:
      rx_struct();

      ~rx_struct();


      virtual rx_result initialize_struct (rx_init_context& ctx);

      virtual rx_result start_struct (rx_start_context& ctx);

      virtual rx_result stop_struct ();

      virtual rx_result deinitialize_struct ();

      static constexpr rx_item_type type_id = rx_item_type::rx_struct_type;
  protected:

  private:


      plugin_struct_runtime_struct impl_;

      template<class T>
      friend rx_result register_struct_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_struct)(rx_platform_api::rx_struct* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_struct)(rx_platform_api::rx_struct* self, start_ctx_ptr ctx);
};

rx_result register_struct_runtime(const rx_node_id& id, rx_struct_constructor_t construct_func);
template<class T>
rx_result register_struct_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_struct_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_struct_runtime(id, constr_lambda);
}





class rx_variable : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_variable);

  public:
      rx_variable();

      ~rx_variable();


      virtual rx_result initialize_variable (rx_init_context& ctx);

      virtual rx_result start_variable (rx_start_context& ctx);

      virtual rx_result stop_variable ();

      virtual rx_result deinitialize_variable ();

      static constexpr rx_item_type type_id = rx_item_type::rx_variable_type;
  protected:

  private:


      plugin_variable_runtime_struct impl_;

      template<class T>
      friend rx_result register_variable_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_variable)(rx_platform_api::rx_variable* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_variable)(rx_platform_api::rx_variable* self, start_ctx_ptr ctx);
};

rx_result register_variable_runtime(const rx_node_id& id, rx_variable_constructor_t construct_func);
template<class T>
rx_result register_variable_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_variable_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_variable_runtime(id, constr_lambda);
}





class rx_event : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_event);

  public:
      rx_event();

      ~rx_event();


      virtual rx_result initialize_event (rx_init_context& ctx);

      virtual rx_result start_event (rx_start_context& ctx);

      virtual rx_result stop_event ();

      virtual rx_result deinitialize_event ();

      static constexpr rx_item_type type_id = rx_item_type::rx_event_type;
  protected:

      void event_fired (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_timed_value data, string_view_type queue, bool state, bool remove);

      data::runtime_data_model get_arguemnts ();


  private:


      plugin_event_runtime_struct impl_;

      template<class T>
      friend rx_result register_event_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_event)(rx_platform_api::rx_event* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_event)(rx_platform_api::rx_event* self, start_ctx_ptr ctx);
};

rx_result register_event_runtime(const rx_node_id& id, rx_event_constructor_t construct_func);
template<class T>
rx_result register_event_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_event_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_event_runtime(id, constr_lambda);
}


} // namespace rx_platform_api



#endif
