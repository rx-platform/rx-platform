

/****************************************************************************
*
*  upython\upy_method.cpp
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


#include "pch.h"

#ifdef UPYTHON_SUPPORT

// upy_internal
#include "upython/upy_internal.h"
// upy_method
#include "upython/upy_method.h"

#include "upython.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace python {

// Class rx_platform::python::upy_method 

upy_method::upy_method()
{
}


upy_method::~upy_method()
{
}



rx_result upy_method::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = script_.bind(".Code", ctx, [this](const string_type&)
        {
            my_module_ = upy_module_ptr::null_ptr;
        });
    return result;
}

rx_result upy_method::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result upy_method::start_runtime (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result upy_method::stop_runtime (runtime::runtime_stop_context& ctx)
{
    return true;
}

logic::method_execution_context* upy_method::create_execution_context (context_execute_data data)
{
    return new upy_method_execution_context(data, "funkcija");
}

rx_result upy_method::execute (data::runtime_values_data args, logic::method_execution_context* context)
{
    upy_method_execution_context* upy_ctx = reinterpret_cast<upy_method_execution_context*>(context);
    if (!my_module_)
    {
        script_buffer_ = script_;
        if (script_buffer_.empty())
            return RX_INVALID_ARGUMENT " script is empty!";

        std::istringstream in(script_buffer_);
        std::ostringstream out;

    //    out << "global _task_dusan\n";
        out << "\n\nasync def funkcija(id";
        for (const auto& one : args.values)
            out << ", " << one.first;
        out << "):\n";
        char buff[0x100];
        while (in.getline(buff, sizeof(buff) / sizeof(buff[0])))
        {
            out << "\t" << buff << "\n";
        }
        out << "\t" << "rxplatform.execution_done(id)\n";
     //   out << "\t" << "rxplatform.execution_done(1)\n";
       // out << "def py_process(id):\n";
    //    out << "create_task(funkcija, 99)\n";
        out << "\n";
   //     out << "var = funkcija()\n";
   //     out << "_task_dusan.push(funkcija)\n";

        my_module_ = rx_create_reference<upy_module>(out.str(), "create_task(funkcija, 99)");
    }
    if (my_module_)
    {
        upy_ctx->set_data(std::move(args));
        my_module_->push_context(upy_ctx);
        upy_thread::instance().include(my_module_);
    }

    return true;
}


// Class rx_platform::python::upy_method_execution_context 

upy_method_execution_context::upy_method_execution_context (context_execute_data data, const string_type& func_name)
      : func_name_(func_name)
    , id_(data.transaction_id)
    , logic::method_execution_context(data)
{
}



string_type upy_method_execution_context::get_eval_code () const
{
    std::ostringstream ss;
    ss << "create_task(" << func_name_ << ", " << id_;
    for (const auto one : vals_.values)
    {
        ss << ", " << get_python_value(one.second);
    }
    ss << ")";
    return ss.str();
}

void upy_method_execution_context::set_data (data::runtime_values_data data)
{
    vals_ = std::move(data);
}


const string_type& upy_method_execution_context::get_func_name () const
{
  return func_name_;
}


} // namespace python
} // namespace rx_platform

#endif
