

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
    inputs_ = get_method_inputs();
    outputs_ = get_method_outputs();
   
    return true;
}

rx_result upy_method::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result upy_method::start_runtime (runtime::runtime_start_context& ctx)
{
    item_ = ctx.get_platform_item();
    return true;
}

rx_result upy_method::stop_runtime (runtime::runtime_stop_context& ctx)
{
    item_.reset();
    return true;
}

rx_result upy_method::execute (rx_platform::runtime::execute_data data, runtime::runtime_process_context* ctx)
{
    
    auto upy_ctx = std::make_unique<upy_method_execution_context>(data, "funkcija", smart_this());
    if (!my_module_)
    {
        string_type temp_str = get_code();
        string_type func_str = get_function();
        string_type module_str = get_module();
        std::ostringstream out;

        if (!temp_str.empty())
        {
            script_buffer_ = temp_str;
            if (script_buffer_.empty())
                return RX_INVALID_ARGUMENT " script is empty!";

            std::istringstream in(script_buffer_);

            out << "\n\nasync def funkcija_exec(ctx";
            for (const auto& one : inputs_.elements)
            {
                out << ", ";
                out << one.name;
            }
            out << "):\n";
            char buff[0x100];
            while (in.getline(buff, sizeof(buff) / sizeof(buff[0])))
            {
                out << "\t" << buff << "\n";
            }
            out << "\n";

            func_str = "funkcija_exec";
        }
        else if (!module_str.empty())
        {
            out << "import rxplatform\n";
        }
        
        out << "\n\nasync def funkcija(ctx";
        for (const auto& one : inputs_.elements)
            out << ", " << one.name;
        out << "):\n";
        out << "\ttry:\n";
        //out << "\t\tprint(\"Pozvao\")\n";
        out << "\t\tresult = await " << func_str << "(ctx";
        for (const auto& one : inputs_.elements)
        {
            out << ", " << one.name;
            
        }
        out << ")\n";
        //out << "\t\tprint(\"gotovo\")\n";
        out << "\t\trxplatform.execution_done(ctx, result)\n";
        out << "\texcept Exception as ex:\n";
        out << "\t\trxplatform.execution_done(ctx, ex)\n";
        out << "\n";

        string_type temp = out.str();
        my_module_ = rx_create_reference<upy_module>(std::move(temp), "create_task(funkcija, 99)", item_->clone());
    }
    if (my_module_)
    {
        my_module_->push_context(std::move(upy_ctx));
        upy_thread::instance().include(my_module_);
    }

    return true;
}

void upy_method::send_execute_result (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id)
{
    execute_result_received(std::move(out_val), std::move(result), id);
}

void upy_method::send_execute_result (data::runtime_values_data out_val, rx_result&& result, runtime_transaction_id_t id)
{
    if (!result)
    {
        execute_result_received(rx_simple_value(), std::move(result), id);
    }
    else
    {
        rx_simple_value val;
        result = outputs_.fill_simple_value(val, out_val);
        if (!result)
        {
            result.register_error("Error converting value!");
            execute_result_received(rx_simple_value(), std::move(result), id);
        }
        else
        {
            execute_result_received(std::move(val), std::move(result), id);
        }
    }
}

void upy_method::read (const string_type& path, mp_obj_t iter, mp_obj_t func)
{
}

void upy_method::reset_module ()
{
    my_module_ = upy_module_ptr::null_ptr;
}


// Class rx_platform::python::upy_method_execution_context 

upy_method_execution_context::upy_method_execution_context (rx_platform::runtime::execute_data data, const string_type& func_name, rx_reference<upy_method> method)
      : data_(data),
        method_(method),
        func_name_(func_name)
    , id_(data.transaction_id)
{
}



string_type upy_method_execution_context::get_eval_code () const
{
    
    int struct_size = (int)data_.value.struct_size();
    std::ostringstream ss;
    ss << "create_task(" << func_name_ << ", " << id_;
    for (int i=0 ; i< struct_size; i++)
    {
        ss << ", " << get_python_value(data_.value[i]);
    }
    ss << ")";
    return ss.str();
}

void upy_method_execution_context::execution_complete (rx_result result)
{
    method_->send_execute_result(values::rx_simple_value(), std::move(result), id_);
}

void upy_method_execution_context::execution_complete (values::rx_simple_value data)
{
    method_->send_execute_result(std::move(data), true, id_);
}

void upy_method_execution_context::execution_complete (data::runtime_values_data data)
{
    method_->send_execute_result(std::move(data), true, id_);
}


const string_type& upy_method_execution_context::get_func_name () const
{
  return func_name_;
}


// Class rx_platform::python::upy_module_method 

upy_module_method::upy_module_method()
{
}


upy_module_method::~upy_module_method()
{
}



rx_result upy_module_method::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = upy_method::initialize_runtime(ctx);
    if (!result)
        return result;

    result = function_.bind(".Function", ctx, [this](const string_type& val)
        {
            reset_module();
        });
    if (!result)
        return result;

    result = module_.bind(".Module", ctx, [this](const string_type& val)
        {
            reset_module();
        });
    if (!result)
        return result;

    return true;
}

string_type upy_module_method::get_code ()
{
    return "";
}

string_type upy_module_method::get_function ()
{
    return function_;
}

string_type upy_module_method::get_module ()
{
    return module_;
}


// Class rx_platform::python::upy_script_method 

upy_script_method::upy_script_method()
{
}


upy_script_method::~upy_script_method()
{
}



rx_result upy_script_method::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = upy_method::initialize_runtime(ctx);
    if (!result)
        return result;

    result = script_.bind(".Code", ctx, [this](const string_type& val)
        {
            reset_module();
        });
    if (!result)
        return result;

    return true;
}

string_type upy_script_method::get_code ()
{
    return script_;
}

string_type upy_script_method::get_function ()
{
    return "";
}

string_type upy_script_method::get_module ()
{
    return "";
}


} // namespace python
} // namespace rx_platform

#endif
