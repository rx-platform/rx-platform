

/****************************************************************************
*
*  runtime_internal\rx_variables.cpp
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

#include "system/runtime/rx_runtime_helpers.h"
#include "model/rx_meta_internals.h"
#include "system/runtime/rx_value_templates.h"
using namespace rx_platform::runtime;

// rx_variables
#include "runtime_internal/rx_variables.h"

#include "runtime_internal/rx_runtime_internal.h"


namespace rx_internal {

namespace sys_runtime {

namespace variables {
rx_result register_variables_constructors()
{
    // OBSOLETE for old school variables, permits variable base!!!
    auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
        RX_CLASS_VARIABLE_BASE_ID, [] {
            return rx_create_reference<simple_variable<true>>();
        });
    if (!result)
    {
        return result;
    }
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
		RX_SIMPLE_VARIABLE_TYPE_ID, [] {
			return rx_create_reference<simple_variable<>>();
		});
    if (!result)
    {
        return result;
    }
    result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
        RX_COMPLEX_INPUT_VARIABLE_TYPE_ID, [] {
            return rx_create_reference<complex_inputs_variable>();
        });
    if (!result)
    {
        return result;
    }
    result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
        RX_COMPLEX_OUTPUT_VARIABLE_TYPE_ID, [] {
            return rx_create_reference<complex_outputs_variable>();
        });
    if (!result)
    {
        return result;
    }
    result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
        RX_COMPLEX_IO_VARIABLE_TYPE_ID, [] {
            return rx_create_reference<complex_io_variable>();
        });
    if (!result)
    {
        return result;
    }
    result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
        RX_BRIDGE_VARIABLE_TYPE_ID, [] {
            return rx_create_reference<bridge_variable>();
        });
    if (!result)
    {
        return result;
    }
    result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
        RX_MUX_VARIABLE_TYPE_ID, [] {
            return rx_create_reference<multiplexer_variable>();
        });
    if (!result)
    {
        return result;
    }

	return true;
}
rx_value get_variable_complex_input(input_selection_type selection, runtime_process_context* ctx, std::vector<rx_value> sources)
{
    rx_time max_ts;
    rx_value bad_value;

    switch (selection)
    {
    case input_selection_type::first_good:
        {
            for (auto& one : sources)
            {
                if (ctx->get_mode().can_callculate(one))
                    return std::move(one);
                if (one.get_time() > max_ts)
                {
                    max_ts = one.get_time();
                    bad_value = std::move(one);
                }
            }
            return bad_value;
        }
        break;
    case input_selection_type::first_non_zero:
        {
            for (auto& one : sources)
            {
                if (ctx->get_mode().can_callculate(one))
                {
                    rx_value temp(one);
                    if (temp.convert_to(RX_BOOL_TYPE) && temp.get_bool())
                        return std::move(one);
                }
                else if (one.get_time() > max_ts)
                {
                    max_ts = one.get_time();
                    bad_value = std::move(one);
                }
            }
        }
        break;
    case input_selection_type::max_value:
        {
            bool first = true;
            double current_val;
            int selected_index = -1;
            int current_idx = 0;
            for (auto& one : sources)
            {
                if (ctx->get_mode().can_callculate(one) && one.is_numeric())
                {
                    double one_val = one.get_float();
                    if (first)
                    {
                        current_val = one_val;
                        selected_index = current_idx;
                        first = false;
                    }
                    else
                    {
                        if (current_val < one_val)
                        {
                            current_val = one_val;
                            selected_index = current_idx;
                        }
                    }
                }
                else if (one.get_time() > max_ts)
                {
                    max_ts = one.get_time();
                    bad_value = std::move(one);
                    if (ctx->get_mode().can_callculate(one))
                        bad_value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
                }
                current_idx++;
            }
            if (selected_index >= 0)
                return std::move(sources[selected_index]);
        }
        break;
    case input_selection_type::max_abs_value:
        {
            bool first = true;
            double current_val;
            int selected_index = -1;
            int current_idx = 0;
            for (auto& one : sources)
            {
                if (ctx->get_mode().can_callculate(one) && one.is_numeric())
                {
                    double one_val = abs(one.get_float());
                    if (first)
                    {
                        current_val = one_val;
                        selected_index = current_idx;
                        first = false;
                    }
                    else
                    {
                        if (current_val < one_val)
                        {
                            current_val = one_val;
                            selected_index = current_idx;
                        }
                    }
                }
                else if (one.get_time() > max_ts)
                {
                    max_ts = one.get_time();
                    bad_value = std::move(one);
                    if (ctx->get_mode().can_callculate(one))
                        bad_value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
                }
                current_idx++;
            }
            if (selected_index >= 0)
                return std::move(sources[selected_index]);
        }
        break;
    case input_selection_type::min_value:
        {
            bool first = true;
            double current_val;
            int selected_index = -1;
            int current_idx = 0;
            for (auto& one : sources)
            {
                if (ctx->get_mode().can_callculate(one) && one.is_numeric())
                {
                    double one_val = one.get_float();
                    if (first)
                    {
                        current_val = one_val;
                        selected_index = current_idx;
                        first = false;
                    }
                    else
                    {
                        if (current_val > one_val)
                        {
                            current_val = one_val;
                            selected_index = current_idx;
                        }
                    }
                }
                else if (one.get_time() > max_ts)
                {
                    max_ts = one.get_time();
                    bad_value = std::move(one);
                    if (ctx->get_mode().can_callculate(one))
                        bad_value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
                }
                current_idx++;
            }
            if (selected_index >= 0)
                return std::move(sources[selected_index]);
        }
    case input_selection_type::min_abs_value:
        {
            bool first = true;
            double current_val;
            int selected_index = -1;
            int current_idx = 0;
            for (auto& one : sources)
            {
                if (ctx->get_mode().can_callculate(one) && one.is_numeric())
                {
                    double one_val = abs(one.get_float());
                    if (first)
                    {
                        current_val = one_val;
                        selected_index = current_idx;
                        first = false;
                    }
                    else
                    {
                        if (current_val > one_val)
                        {
                            current_val = one_val;
                            selected_index = current_idx;
                        }
                    }
                }
                else if (one.get_time() > max_ts)
                {
                    max_ts = one.get_time();
                    bad_value = std::move(one);
                    if (ctx->get_mode().can_callculate(one))
                        bad_value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
                }
                current_idx++;
            }
            if (selected_index >= 0)
                return std::move(sources[selected_index]);
        }
        break;
    default:
        RX_ASSERT(false);
    }
    return bad_value;
}

// Parameterized Class rx_internal::sys_runtime::variables::simple_variable 

template <bool is_obsolite>
simple_variable<is_obsolite>::simple_variable()
{
}


template <bool is_obsolite>
simple_variable<is_obsolite>::~simple_variable()
{
}



template <bool is_obsolite>
rx_result simple_variable<is_obsolite>::initialize_variable (runtime::runtime_init_context& ctx)
{
    if constexpr (is_obsolite)
    {
        std::ostringstream ss;
        ss << "OBSOLETE!!! Variable "
            << ctx.path.get_current_path()
            << " is declares as VariableBase, please use SimpleVariable type!";
        RUNTIME_LOG_WARNING("simple_variable", 900, ss.str());
    }
    return true;
}

template <bool is_obsolite>
rx_value simple_variable<is_obsolite>::get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources)
{
    return get_variable_complex_input(input_selection_type::first_good, ctx, std::move(sources));
}

template <bool is_obsolite>
rx_result simple_variable<is_obsolite>::variable_write (write_data&& data, structure::write_task* task, runtime_process_context* ctx, runtime_sources_type& sources)
{
    rx_result ret = RX_NOT_SUPPORTED;
    if (sources.size() == 1)
    {
        if (sources[0].is_output())
        {
            auto new_trans = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
            write_task_data my_data;
            my_data.task = task;
            my_data.ref_count = 1;
            get_tasks().emplace(new_trans, std::move(my_data));
            data.transaction_id = new_trans;
            ret = sources[0].write_value(std::move(data));
            if (!ret)
                get_tasks().erase(new_trans);
        }
    }
    else if (!sources.empty())
    {
        auto new_trans = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
        write_task_data my_data;
        my_data.task = task;
        my_data.ref_count = 0;
        auto ins_result = get_tasks().emplace(new_trans, std::move(my_data));
        data.transaction_id = new_trans;
        bool success = false;

        for (auto& one : sources)
        {
            if (one.is_output())
            {
                ret = one.write_value(write_data(data));
                if (ret)
                {
                    ins_result.first->second.ref_count++;
                    success = true;
                }
            }
        }
        if (!success)
        {
            get_tasks().erase(new_trans);
        }
        else
        {
            ret = true;// at least one write succeeded
        }
    }
    return ret;
}

template <bool is_obsolite>
void simple_variable<is_obsolite>::process_result (runtime_transaction_id_t id, rx_result&& result)
{
    auto it = get_tasks().find(id);
    if (it != get_tasks().end())
    {
        it->second.ref_count--;
        if (result || it->second.ref_count <= 0)
        {
            if (it->second.task)
            {
                send_write_result(it->second.task, std::move(result));
            }
            get_tasks().erase(it);
        }
    }
}

template <bool is_obsolite>
std::map<runtime_transaction_id_t, typename simple_variable<is_obsolite>::write_task_data>& simple_variable<is_obsolite>::get_tasks ()
{
	if(!pending_tasks_)
		pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, write_task_data> >();
	return *pending_tasks_;
}


// Class rx_internal::sys_runtime::variables::complex_inputs_variable 

complex_inputs_variable::complex_inputs_variable()
      : inputs_selection_(input_selection_type::first_good)
{
}


complex_inputs_variable::~complex_inputs_variable()
{
}



rx_result complex_inputs_variable::initialize_variable (runtime::runtime_init_context& ctx)
{
    auto result = complex_inputs_base::initialize_variable(ctx);
    if (result)
    {
        uint8_t sel_type = ctx.get_item_static<uint8_t>(".InputsSelection");
        if (sel_type > (uint8_t)input_selection_type::min_abs_value)
        {
            std::ostringstream ss;
            ss << "Variable "
                << ctx.path.get_current_path()
                << " has invalid InputsSelection "
                << sel_type << ", falling back to first_good.";

            RUNTIME_LOG_WARNING("complex_inputs_variable", 500, ss.str());
        }
        else
        {
            inputs_selection_ = (input_selection_type)sel_type;
        }
    }
    return result;
}

rx_value complex_inputs_variable::get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources)
{
    return get_variable_complex_input(inputs_selection_, ctx, std::move(sources));
}


// Class rx_internal::sys_runtime::variables::multiplexer_variable 

multiplexer_variable::multiplexer_variable()
      : selector_(-1)
{
}


multiplexer_variable::~multiplexer_variable()
{
}



rx_result multiplexer_variable::initialize_variable (runtime::runtime_init_context& ctx)
{
    runtime::runtime_process_context* rt_ctx = ctx.context;
    auto result = selector_.bind(".Select", ctx, [this, rt_ctx](const uint8_t& val)
        {
            process_variable(rt_ctx);
        });

    return result;
}

rx_value multiplexer_variable::get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources)
{
    int8_t select = selector_;
    if (select < 0 || select>=(int)sources.size())
    {
        return rx_value();
    }
    return std::move(sources[select]);
}

rx_result multiplexer_variable::variable_write (write_data&& data, structure::write_task* task, runtime_process_context* ctx, runtime_sources_type& sources)
{
    int8_t select = selector_;
    if (select < 0 || select >= (int)sources.size())
    {
        return "Invalid Selector Value";
    }
    auto new_trans = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
    auto ins_result = get_tasks().emplace(new_trans, task);
    data.transaction_id = new_trans;
    for (auto& one : sources)
    {
        if (one.is_output())
        {
            if (select == 0)
            {
                auto ret = one.write_value(write_data(data));
                if (!ret)
                {
                    get_tasks().erase(new_trans);
                }
                return ret;
            }
            else
            {
                select--;
            }
        }
    }
    return "Invalid Selector Value";
}

void multiplexer_variable::process_result (runtime_transaction_id_t id, rx_result&& result)
{
    auto it = get_tasks().find(id);
    if (it != get_tasks().end())
    {
        if (it->second)
        {
            send_write_result(it->second, std::move(result));
        }
        get_tasks().erase(it);
    }
}

std::map<runtime_transaction_id_t, structure::write_task*>& multiplexer_variable::get_tasks ()
{
    if (!pending_tasks_)
        pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, structure::write_task*> >();
    return *pending_tasks_;
}


// Class rx_internal::sys_runtime::variables::complex_outputs_variable 

complex_outputs_variable::complex_outputs_variable()
{
}


complex_outputs_variable::~complex_outputs_variable()
{
}



rx_result complex_outputs_variable::initialize_variable (runtime::runtime_init_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_value complex_outputs_variable::get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources)
{
    return get_variable_complex_input(input_selection_type::first_good, ctx, std::move(sources));
}

rx_result complex_outputs_variable::variable_write (write_data&& data, structure::write_task* task, runtime_process_context* ctx, runtime_sources_type& sources)
{
    return RX_NOT_IMPLEMENTED;
}

void complex_outputs_variable::process_result (runtime_transaction_id_t id, rx_result&& result)
{
}

std::map<runtime_transaction_id_t, typename complex_outputs_variable::write_task_data>& complex_outputs_variable::get_tasks ()
{
    if (!pending_tasks_)
        pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, write_task_data> >();
    return *pending_tasks_;
}


// Class rx_internal::sys_runtime::variables::complex_io_variable 

complex_io_variable::complex_io_variable()
      : inputs_selection_(input_selection_type::first_good)
{
}


complex_io_variable::~complex_io_variable()
{
}



rx_result complex_io_variable::initialize_variable (runtime::runtime_init_context& ctx)
{
    auto result = complex_outputs_variable::initialize_variable(ctx);
    if (result)
    {
        uint8_t sel_type = ctx.get_item_static<uint8_t>(".InputsSelection");
        if (sel_type > (uint8_t)input_selection_type::min_abs_value)
        {
            std::ostringstream ss;
            ss << "Variable "
                << ctx.path.get_current_path()
                << " has invalid InputsSelection "
                << sel_type << ", falling back to first_good.";

            RUNTIME_LOG_WARNING("complex_inputs_variable", 500, ss.str());
        }
        else
        {
            inputs_selection_ = (input_selection_type)sel_type;
        }
    }
    return result;
}

rx_value complex_io_variable::get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources)
{
    return get_variable_complex_input(inputs_selection_, ctx, std::move(sources));
}


// Class rx_internal::sys_runtime::variables::bridge_variable 

bridge_variable::bridge_variable()
{
}


bridge_variable::~bridge_variable()
{
}



rx_result bridge_variable::initialize_variable (runtime::runtime_init_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}


} // namespace variables
} // namespace sys_runtime
} // namespace rx_internal

