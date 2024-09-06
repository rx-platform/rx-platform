

/****************************************************************************
*
*  upython\upy_method.h
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


#ifndef upy_method_h
#define upy_method_h 1


#ifdef UPYTHON_SUPPORT

// upy_internal
#include "upython/upy_internal.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_logic
#include "system/logic/rx_logic.h"

namespace rx_platform {
namespace python {
class upy_method;

} // namespace python
} // namespace rx_platform


#include "system/runtime/rx_value_templates.h"
#include "system/runtime/rx_process_context.h"
#include "system/runtime/rx_runtime_helpers.h"
using rx_platform::runtime::context_execute_data;
using rx_platform::runtime::local_value;


namespace rx_platform {

namespace python {





class upy_method_execution_context 
{

  public:
      upy_method_execution_context (rx_platform::runtime::execute_data data, const string_type& func_name, rx_reference<upy_method> method);


      string_type get_eval_code () const;

      void execution_complete (rx_result result);

      void execution_complete (values::rx_simple_value data);

      void execution_complete (data::runtime_values_data data);


      const string_type& get_func_name () const;

      runtime_transaction_id_t get_id () const
      {
        return id_;
      }



  protected:

  private:


      runtime::execute_data data_;

      rx_reference<upy_method> method_;


      string_type func_name_;

      runtime_transaction_id_t id_;


};







class upy_method : public logic::method_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
MicroPython method script. Currently testing on Win32 only.");

    DECLARE_REFERENCE_PTR(upy_method)

  public:
      upy_method();

      ~upy_method();


      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result execute (rx_platform::runtime::execute_data data, runtime::runtime_process_context* ctx);

      void send_execute_result (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id);

      void send_execute_result (data::runtime_values_data out_val, rx_result&& result, runtime_transaction_id_t id);

      void read (const string_type& path, mp_obj_t iter, mp_obj_t func);

      virtual string_type get_code () = 0;

      virtual string_type get_function () = 0;

      virtual string_type get_module () = 0;


  protected:

      void reset_module ();


  private:


      upy_module_ptr my_module_;


      string_type script_buffer_;

      local_value<string_type> script_;

      data::runtime_data_model inputs_;

      data::runtime_data_model outputs_;

      platform_item_ptr item_;


};






class upy_module_method : public upy_method  
{

  public:
      upy_module_method();

      ~upy_module_method();


      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      string_type get_code ();

      string_type get_function ();

      string_type get_module ();


  protected:

  private:


      local_value<string_type> module_;

      local_value<string_type> function_;


};






class upy_script_method : public upy_method  
{

  public:
      upy_script_method();

      ~upy_script_method();


      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      string_type get_code ();

      string_type get_function ();

      string_type get_module ();


  protected:

  private:


      local_value<string_type> script_;


};


} // namespace python
} // namespace rx_platform

#endif


#endif
