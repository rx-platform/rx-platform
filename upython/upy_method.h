

/****************************************************************************
*
*  upython\upy_method.h
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


#ifndef upy_method_h
#define upy_method_h 1


#ifdef UPYTHON_SUPPORT

// rx_logic
#include "system/logic/rx_logic.h"
// upy_internal
#include "upython/upy_internal.h"



#include "system/runtime/rx_value_templates.h"
using runtime::local_value;


namespace rx_platform {

namespace python {





class upy_method_execution_context : public logic::method_execution_context  
{

  public:
      upy_method_execution_context (execute_data data, const string_type& func_name);


      string_type get_eval_code () const;

      void set_data (data::runtime_values_data data);


      const string_type& get_func_name () const;

      runtime_transaction_id_t get_id () const
      {
        return id_;
      }



  protected:

  private:


      string_type func_name_;

      runtime_transaction_id_t id_;

      data::runtime_values_data vals_;


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

      virtual logic::method_execution_context* create_execution_context (execute_data data);

      virtual rx_result execute (data::runtime_values_data args, logic::method_execution_context* context);


  protected:

  private:


      upy_module_ptr my_module_;


      string_type script_buffer_;

      local_value<string_type> script_;


};


} // namespace python
} // namespace rx_platform

#endif


#endif
