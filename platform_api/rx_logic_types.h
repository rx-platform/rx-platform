

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_logic_types.h
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
//
//template <> struct convert_type_helper <int>
//{
//    static int from_arg(const rx_simple_value& what)
//    {
//        return what.extract_static<int>(0);
//    }
//
//    static rx_simple_value to_result(int r)
//    {
//        rx_simple_value ret;
//        ret.assign_static(r);
//        return std::to_string(r);
//    }
//
//};

template<typename... Args>
class rx_simple_method : public rx_method
{

    template < typename Type>
    Type from_arg(const rx_simple_value& what)
    {
        if constexpr (std::is_same_v<Type, string_type>)
        {
            return what.extract_static<std::string>("");
        }
        else
        {
            return what.extract_static<Type>(0);
        }
    }

    template < typename Type>
    rx_simple_value to_result(Type what)
    {
        rx_simple_value ret;
        ret.assign_static(std::move(what));
        return ret;
    }
public:
    rx_simple_method()
    {
    }
    template<std::size_t... I>
    void invoke_helper(runtime_transaction_id_t id, std::vector<rx_simple_value> inputs,
        std::index_sequence<I...>)
    {
        execute(id, from_arg<Args>(std::move(inputs.at(I)))...);
    }
    virtual void execute(runtime_transaction_id_t id, Args... args) = 0;
    rx_result call_value(runtime_transaction_id_t id, rx_simple_value args)
    {
        if (args.is_struct())
        {
            std::vector<rx_simple_value> inputs;
            inputs.reserve(args.struct_size());
            for (size_t i = 0; i < args.struct_size(); i++)
            {
                inputs.push_back(args[(int)i]);
            }
            invoke_helper(id, std::move(inputs), std::index_sequence_for<Args...>{});
            return true;
        }
        else
        {
            return RX_INVALID_ARGUMENT;
        }
    }
    template<typename... Results>
    void send_result(runtime_transaction_id_t id, Results... args)
    {
        rx_simple_value result = rx_create_value_static(std::forward<Results>(args)...);
        execute_result_received(true, id, std::move(result));
    }
    rx_result method_execute(runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx)
    {
        auto result = call_value(id, std::move(val));
        return result;
    }
};






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
