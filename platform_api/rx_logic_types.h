

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_logic_types.h
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


#ifndef rx_logic_types_h
#define rx_logic_types_h 1



// rx_runtime
#include "rx_runtime.h"



namespace rx_platform_api {





class rx_method : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_method);

  public:
      rx_method();

      ~rx_method();


      virtual rx_result initialize_method (rx_init_context& ctx);

      virtual rx_result start_method (rx_start_context& ctx);

      virtual rx_result stop_method ();

      virtual rx_result deinitialize_method ();

      virtual rx_result method_execute (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx) = 0;

      static constexpr rx_item_type type_id = rx_item_type::rx_method_type;
  protected:

      void execute_result_received (rx_result&& result, runtime_transaction_id_t id, rx_simple_value val);


  private:


      plugin_method_runtime_struct impl_;

      template<class T>
      friend rx_result register_method_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_method)(rx_platform_api::rx_method* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_method)(rx_platform_api::rx_method* self, start_ctx_ptr ctx);
};

rx_result register_method_runtime(const rx_node_id& id, rx_method_constructor_t construct_func);
template<class T>
rx_result register_method_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_method_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_method_runtime(id, constr_lambda);
}






class rx_program : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_program);

  public:
      rx_program();

      ~rx_program();


      virtual rx_result initialize_program (rx_init_context& ctx);

      virtual rx_result start_program (rx_start_context& ctx);

      virtual rx_result stop_program ();

      virtual rx_result deinitialize_program ();

      static constexpr rx_item_type type_id = rx_item_type::rx_program_type;
  protected:

  private:


      plugin_program_runtime_struct impl_;

      template<class T>
      friend rx_result register_program_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_program)(rx_platform_api::rx_program* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_program)(rx_platform_api::rx_program* self, start_ctx_ptr ctx);
};

rx_result register_program_runtime(const rx_node_id& id, rx_program_constructor_t construct_func);
template<class T>
rx_result register_program_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_program_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_program_runtime(id, constr_lambda);
}






class rx_display : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_display);

  public:
      rx_display();

      ~rx_display();


      virtual rx_result initialize_display (rx_init_context& ctx);

      virtual rx_result start_display (rx_start_context& ctx);

      virtual rx_result stop_display ();

      virtual rx_result deinitialize_display ();

      static constexpr rx_item_type type_id = rx_item_type::rx_display_type;
  protected:

  private:


      plugin_display_runtime_struct impl_;

      template<class T>
      friend rx_result register_display_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_display)(rx_platform_api::rx_display* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_display)(rx_platform_api::rx_display* self, start_ctx_ptr ctx);
};

rx_result register_display_runtime(const rx_node_id& id, rx_display_constructor_t construct_func);
template<class T>
rx_result register_display_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_display_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_display_runtime(id, constr_lambda);
}


} // namespace rx_platform_api



#endif
