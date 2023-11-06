

/****************************************************************************
*
*  system\runtime\rx_runtime_logic.cpp
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


#include "pch.h"

#include "system/runtime/rx_runtime_helpers.h"

// rx_runtime_logic
#include "system/runtime/rx_runtime_logic.h"

#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace runtime {

namespace logic_blocks {

// Class rx_platform::runtime::logic_blocks::logic_holder 


rx_result logic_holder::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
    if (idx == string_type::npos)
    {
        for (auto& one : runtime_methods_)
        {
            if (one.name == path)
            {
                return one.get_value("", val, ctx);
            }
        }
    }
    else
    {
        string_type name = path.substr(0, idx);
        for (auto& one : runtime_programs_)
        {
            if (one.name == name)
            {
                return one.get_value(path.substr(idx + 1), val, ctx);
            }
        }
        for (auto& one : runtime_methods_)
        {
            if (one.name == name)
            {
                return one.get_value(path.substr(idx + 1), val, ctx);
            }
        }
    }
    return RX_INVALID_PATH;
}

rx_result logic_holder::initialize_logic (runtime::runtime_init_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->initialize_runtime(ctx);
        if (!ret)
            return ret;

        ret = one.program_ptr->initialize_runtime(ctx);
        if (!ret)
            return ret;

        one.context = one.program_ptr->create_program_context(nullptr, ctx.context->get_security_guard());
    }
    for (auto& one : runtime_methods_)
    {
        ctx.structure.push_item(*one.item);

        ret = one.item->initialize_runtime(ctx);
        if (!ret)
            return ret;
        
        ret = one.method_ptr->initialize_runtime(ctx);
        if (!ret)
            return ret;

        ctx.structure.pop_item();
    }
    return ret;
}

rx_result logic_holder::deinitialize_logic (runtime::runtime_deinit_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->deinitialize_runtime(ctx);
        if (ret)
            ret = one.program_ptr->deinitialize_runtime(ctx);
        if (!ret)
            return ret;
    }
    for (auto& one : runtime_methods_)
    {
        ret = one.item->deinitialize_runtime(ctx);
        if (ret)
            ret = one.method_ptr->deinitialize_runtime(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result logic_holder::start_logic (runtime::runtime_start_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->start_runtime(ctx);
        if (ret)
            ret = one.program_ptr->start_runtime(ctx);
        if (!ret)
            return ret;
    }
    for (auto& one : runtime_methods_)
    {
        ret = one.item->start_runtime(ctx);
        if (ret)
            ret = one.method_ptr->start_runtime(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result logic_holder::stop_logic (runtime::runtime_stop_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->stop_runtime(ctx);
        if (ret)
            ret = one.program_ptr->stop_runtime(ctx);
        if (!ret)
            return ret;
    }
    for (auto& one : runtime_methods_)
    {
        ret = one.item->stop_runtime(ctx);
        if (ret)
            ret = one.method_ptr->stop_runtime(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

void logic_holder::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
    for (auto& one : runtime_programs_)
    {
        auto it = data.children.find(one.name);
        if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
        {
            one.fill_data(std::get< data::runtime_values_data>(it->second));
            break;
        }
        
    }
    for (auto& one : runtime_methods_)
    {
        auto it = data.children.find(one.name);
        if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
        {
            one.fill_data(std::get< data::runtime_values_data>(it->second));
            break;
        }
    }
}

void logic_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    for (const auto& one : runtime_programs_)
    {
        data::runtime_values_data one_data;
        one.collect_data(one_data, type);
        data.add_child(one.name, std::move(one_data));
    }
    for (const auto& one : runtime_methods_)
    {
        data::runtime_values_data one_data;
        one.collect_data(one_data, type);
        data.add_child(one.name, std::move(one_data));
    }
}

rx_result logic_holder::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx)
{
    rx_result ret = true;
    
    if (path.empty())
    {
        for (auto& one : runtime_programs_)
        {
            runtime_item_attribute args_attr;
            args_attr.name = one.name;
            args_attr.type = rx_attribute_type::program_attribute_type;
            if(prefix.empty())
                args_attr.full_path = args_attr.name;
            else
                args_attr.full_path = prefix + RX_OBJECT_DELIMETER + args_attr.name;
            items.emplace_back(std::move(args_attr));
        }
        for (auto& one : runtime_methods_)
        {
            runtime_item_attribute args_attr;
            args_attr.name = one.name;
            args_attr.type = rx_attribute_type::method_attribute_type;
            args_attr.value =one.get_value(ctx);
            if (prefix.empty())
                args_attr.full_path = args_attr.name;
            else
                args_attr.full_path = prefix + RX_OBJECT_DELIMETER + args_attr.name;
            items.emplace_back(std::move(args_attr));
        }
    }
    else if (!runtime_programs_.empty() || !runtime_methods_.empty())
    {
        auto idx = path.find(RX_OBJECT_DELIMETER);
        string_type mine;
        string_type bellow;
        if (idx == string_type::npos)
        {
            mine = path;
        }
        else
        {
            mine = path.substr(0, idx);
            bellow = path.substr(idx + 1);
        }
        string_type new_prefix;
        if (prefix.empty())
            new_prefix = mine;
        else
            new_prefix = prefix + RX_OBJECT_DELIMETER + mine;
        for (auto& one : runtime_programs_)
        {
            if (one.name == mine)
            {
                ret = one.browse_items(new_prefix, bellow, filter, items, ctx);
                return ret;
            }
        }
        for (auto& one : runtime_methods_)
        {
            if (one.name == mine)
            {
                ret = one.browse_items(new_prefix, bellow, filter, items, ctx);
                return ret;
            }
        }
        ret = RX_INVALID_PATH;
    }
    return ret;
}

rx_result logic_holder::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_array("programs", runtime_programs_.size()))
        return stream.get_error();
    for (const auto& one : runtime_programs_)
    {
        ///!!!!! this is wrong stuff need to reconsider this serialization
        auto ret = one.program_ptr->save_program(stream, type);
        if (!ret)
            return ret;
    }
    if (!stream.end_array())
        return stream.get_error();
    
    return true;
}

rx_result logic_holder::deserialize (base_meta_reader& stream, uint8_t type)
{
    return true;
}

bool logic_holder::is_this_yours (string_view_type path) const
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
    if (idx == string_type::npos)
    {
        for (const auto& one : runtime_programs_)
        {
            if (one.name == path)
                return true;
        }
        for (const auto& one : runtime_methods_)
        {
            if (one.name == path)
                return true;
        }
    }
    else
    {
        string_view_type name = path.substr(0, idx);
        for (const auto& one : runtime_programs_)
        {
            if (one.name == name)
                return true;
        }
        for (const auto& one : runtime_methods_)
        {
            if (one.name == name)
                return true;
        }
    }
    return false;
}

void logic_holder::process_programs (runtime_process_context& ctx)
{
}

rx_result logic_holder::get_value_ref (string_view_type path, rt_value_ref& ref)
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
    if (idx == string_type::npos)
    {
        for (auto& one : runtime_methods_)
        {
            if (one.name == path)
            {
                ref.ref_type = rt_value_ref_type::rt_method;
                ref.ref_value_ptr.method = &one;
                return true;
            }
        }
    }
    else
    {
        string_view_type name = path.substr(0, idx);
        for (auto& one : runtime_programs_)
        {
            if (one.name == name)
            {
                return one.get_value_ref(path.substr(idx+1), ref);
            }
        }
        for (auto& one : runtime_methods_)
        {
            if (one.name == name)
            {
                return one.get_value_ref(path.substr(idx + 1), ref);
            }
        }
    }
    return RX_INVALID_PATH;
}

rx_result logic_holder::get_struct_value (string_view_type item, string_view_type path, data::runtime_values_data& data, runtime_value_type type, runtime_process_context* ctx) const
{
    const structure::runtime_item* item_ptr = nullptr;
    for (const auto& one : runtime_programs_)
    {
        if (one.name == item)
        {
            item_ptr = one.item->get_child_item(path);
            break;
        }
    }
    if (!item_ptr)
    {
        for (const auto& one : runtime_methods_)
        {
            if (one.name == item)
            {
                item_ptr = one.item->get_child_item(path);
            }
        }
    }
    if (item_ptr)
    {
        item_ptr->collect_data(data, type);
        return true;
    }
    else
    {
        return "Invalid path";
    }
}

void logic_holder::set_methods (std::vector<method_data> data)
{
    runtime_methods_ = const_size_vector<runtime::logic_blocks::method_data>(std::move(data));
}

void logic_holder::set_programs (std::vector<program_data> data)
{
    runtime_programs_ = const_size_vector<runtime::logic_blocks::program_data>(std::move(data));
}


// Class rx_platform::runtime::logic_blocks::program_data 

program_data::program_data (runtime_item_ptr&& rt, program_runtime_ptr&& var, const program_data& prototype)
    : program_ptr(std::move(var))
    , item(std::move(rt))
{
}



void program_data::fill_data (const data::runtime_values_data& data)
{
    item->fill_data(data);
}

void program_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    item->collect_data(data, type);
}

rx_result program_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx)
{
    return item->browse_items(prefix, path, filter, items, ctx);
}

rx_result program_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
    return item->get_value(path, val, ctx);
}

rx_result program_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
    return item->get_value_ref(path, ref, false);
}

rx_result program_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
    return item->get_local_value(path, val);
}

void program_data::process_program (runtime::runtime_process_context& ctx)
{
    program_ptr->process_program(context.get(), ctx);
}


// Class rx_platform::runtime::logic_blocks::method_data 

method_data::method_data (structure::runtime_item::smart_ptr&& rt, method_runtime_ptr&& var, method_data&& prototype)
    : method_ptr(std::move(var))
    , item(std::move(rt))
    , inputs(prototype.inputs)
    , outputs(prototype.outputs)
{
    value.value.assign_static<uint8_t>(0);
    pending_tasks_ = std::make_unique<std::map<runtime_transaction_id_t, structure::execute_task*> >();
}



void method_data::fill_data (const data::runtime_values_data& data)
{
    item->fill_data(data);
}

void method_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    item->collect_data(data, type);
}

rx_result method_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx)
{
    if (path.empty())
    {
        runtime_item_attribute args_attr;
        args_attr.name = "In";
        args_attr.type = rx_attribute_type::data_attribute_type;
        args_attr.full_path = prefix.empty() ? args_attr.name : prefix + RX_OBJECT_DELIMETER + args_attr.name;
        items.emplace_back(args_attr);

        args_attr.name = "Out";
        args_attr.type = rx_attribute_type::data_attribute_type;
        args_attr.full_path = prefix.empty() ? args_attr.name : prefix + RX_OBJECT_DELIMETER + args_attr.name;
        items.emplace_back(std::move(args_attr));

        return item->browse_items(prefix, path, filter, items, ctx);
    }
    else
    {
        auto idx = path.find(RX_OBJECT_DELIMETER);
        string_type sub_path;
        string_type rest_path;
        if (idx != string_type::npos)
        {
            sub_path = path.substr(0, idx);
            rest_path = path.substr(idx + 1);
        }
        else
        {
            sub_path = path;
        }
        if (sub_path == "In")
        {
            string_type bellow_prefix = prefix.empty() ? sub_path : prefix + RX_OBJECT_DELIMETER + sub_path;
            return inputs.browse_items(bellow_prefix, rest_path, filter, items, ctx);
        }
        else if (sub_path == "Out")
        {
            string_type bellow_prefix = prefix.empty() ? sub_path : prefix + RX_OBJECT_DELIMETER + sub_path;
            return outputs.browse_items(bellow_prefix, rest_path, filter, items, ctx);
        }
        else
        {
            return item->browse_items(prefix, path, filter, items, ctx);
        }
    }
}

rx_result method_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
    if (path.empty())
    {
        val = get_value(ctx);
        return true;
    }
    else
    {
        auto idx = path.find(RX_OBJECT_DELIMETER);
        string_type sub_path;
        string_type rest_path;
        if (idx != string_type::npos)
        {
            sub_path = path.substr(0, idx);
            rest_path = path.substr(idx + 1);
        }
        else
        {
            sub_path = path;
        }
        if (sub_path == "In")
        {
            return inputs.get_value(rest_path, val, ctx);
        }
        else if (sub_path == "Out")
        {
            return outputs.get_value(rest_path, val, ctx);
        }
        else
        {
            return item->get_value(path, val, ctx);
        }
    }
}

rx_result method_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
    if (path.empty())
    {
        return RX_INVALID_PATH;
    }
    else
    {
        auto idx = path.find(RX_OBJECT_DELIMETER);
        string_view_type sub_path;
        string_view_type rest_path;
        if (idx != string_view_type::npos)
        {
            sub_path = path.substr(0, idx);
            rest_path = path.substr(idx + 1);
        }
        else
        {
            sub_path = path;
        }
        if (sub_path == "In")
        {
            return inputs.get_value_ref(rest_path, ref, false);
        }
        else if (sub_path == "Out")
        {
            return outputs.get_value_ref(rest_path, ref, false);
        }
        else
        {
            return item->get_value_ref(path, ref, false);
        }
    }
}

rx_result method_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
    if (path.empty())
    {
        val = value.value.to_simple();
        return true;
    }
    else
    {
        return item->get_local_value(path, val);
    }
}

rx_value method_data::get_value (runtime_process_context* ctx) const
{
    if (ctx)
    {
        return ctx->adapt_value(value.value);
    }
    else
    {
        rx_value temp;
        value.value.get_value(temp, rx_time::null_time(), rx_mode_type());
        return temp;
    }
}

rx_result method_data::execute (execute_data&& data, structure::execute_task* task, runtime_process_context* ctx)
{

    data::runtime_values_data args;

    rx_result result = inputs.create_safe_runtime_data(data.data, args);
    if (!result)
        return result;


    security::secured_scope _(data.identity);

    auto new_trans = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
    pending_tasks_->emplace(new_trans, task);
    data.transaction_id = new_trans;
    auto context = method_ptr->create_execution_context(std::move(data), ctx->get_security_guard());


    if (context)
    {
        context->method_data_ = this;
        context->context_ = ctx;
        context->security_guard_ = ctx->get_security_guard();
        result = method_ptr->execute(std::move(args), context);
    }
    else
    {
        result = "Unexpected error, not execute context created!";
    }

    return result;
}

void method_data::process_execute_result (runtime_transaction_id_t id, rx_result&& result, data::runtime_values_data data)
{
    auto it = pending_tasks_->find(id);
    if (it != pending_tasks_->end())
    {
        auto task = it->second;
        pending_tasks_->erase(it);

        data::runtime_values_data out;
        if (result)
        {
            rx_result temp_result = outputs.create_safe_runtime_data(data, out);
            if (!temp_result)
            {
                temp_result.register_error("Error converting output parameters!");
                RUNTIME_LOG_WARNING("method_data", 300, temp_result.errors_line());
                result = std::move(temp_result);
            }
        }
        if (task)
            task->process_result(std::move(result), std::move(out));
    }
}


} // namespace logic_blocks
} // namespace runtime
} // namespace rx_platform

