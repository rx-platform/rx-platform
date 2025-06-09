

/****************************************************************************
*
*  system\runtime\rx_runtime_logic.cpp
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
        ctx.path.push_to_path(one.name);

        ret = one.item->initialize_runtime(ctx);
        if (!ret)
            return ret;
        ret = one.program_ptr->initialize_runtime(ctx);
        if (!ret)
            return ret;
        ctx.path.pop_from_path();
        one.context = one.program_ptr->create_program_context(nullptr, ctx.context->get_security_guard(0));//sec warning
    }
    for (auto& one : runtime_methods_)
    {
        ctx.path.push_to_path(one.name);
        ret = one.initialize_runtime(ctx);
        if (!ret)
            return ret;
        ctx.path.pop_from_path();
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
        ret = one.deinitialize_runtime(ctx);
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
        ret = one.start_runtime(ctx);
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
        ret = one.stop_runtime(ctx);
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
        }
        
    }
    for (auto& one : runtime_methods_)
    {
        auto it = data.children.find(one.name);
        if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
        {
            one.fill_data(std::get< data::runtime_values_data>(it->second));
        }
    }
}

void logic_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    for (const auto& one : runtime_programs_)
    {
        data::runtime_values_data one_data;
        one.collect_data(one_data, type);
        if(!one_data.empty())
            data.add_child(one.name, std::move(one_data));
    }
    for (const auto& one : runtime_methods_)
    {
        data::runtime_values_data one_data;
        one.collect_data(one_data, type);
        if (!one_data.empty())
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
            one.item->collect_data(path, data, type);
            return true;
        }
    }
    if (!item_ptr)
    {
        for (const auto& one : runtime_methods_)
        {
            if (one.name == item)
            {
                one.item->collect_data(path, data, type);
                return true;
            }
        }
    }
    return "Invalid path";
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
    item->collect_data("", data, type);
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
      : context_(nullptr)
    , method_ptr(std::move(var))
    , item(std::move(rt))
    , inputs(prototype.inputs)
    , outputs(prototype.outputs)
{
    value.value.assign_static<int32_t>(0);
    pending_tasks_ = std::make_unique<std::map<runtime_transaction_id_t, std::pair<bool, std::unique_ptr<structure::execute_task> > > >();
}



void method_data::fill_data (const data::runtime_values_data& data)
{
    item->fill_data(data);
    auto child_it = data.children.find("In");
    if (child_it!= data.children.end()
        && std::holds_alternative<data::runtime_values_data>(child_it->second))
    {
        const auto& simple_child = std::get<data::runtime_values_data>(child_it->second);
        inputs.fill_data(simple_child);
    }
    child_it = data.children.find("Out");
    if (child_it != data.children.end()
        && std::holds_alternative<data::runtime_values_data>(child_it->second))
    {
        auto& simple_child = std::get<data::runtime_values_data>(child_it->second);
        outputs.fill_data(simple_child);
    }
}

void method_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    item->collect_data("", data, type);
    if (type != runtime_value_type::persistent_runtime_value)
    {
        data::runtime_values_data child_inputs;
        data::runtime_values_data child_outputs;
        inputs.collect_data("", child_inputs, type);
        outputs.collect_data("", child_outputs, type);
        data.add_child("In", std::move(child_inputs));
        data.add_child("Out", std::move(child_outputs));
    }
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

rx_result method_data::execute (context_execute_data&& data, std::unique_ptr<structure::execute_task> task, runtime_process_context* ctx)
{

    rx_result result;
    rx_simple_value to_send;
    bool named = false;

    if (std::holds_alternative<rx_simple_value>(data.data))
    {
        result = inputs.check_value(std::get<rx_simple_value>(data.data));
        if (result)
        {
            to_send = std::move(std::get<rx_simple_value>(data.data));
        }
    }
    else if (std::holds_alternative<data::runtime_values_data>(data.data))
    {
        structure::block_data args(inputs);
        args.fill_data(std::get<data::runtime_values_data>(data.data));
        result = args.collect_value(to_send, runtime_value_type::simple_runtime_value);
        named = true;
    }
    else
    {
        RX_ASSERT(false);
        return RX_INTERNAL_ERROR;
    }
    if (!result)
        return result;


    security::secured_scope _(data.identity);

    auto new_trans = rx_get_new_transaction_id();

    execute_data send_data;
    send_data.test = data.test;
    send_data.identity = data.identity;
    send_data.internal = data.internal;
    send_data.transaction_id = new_trans;
    send_data.value = std::move(to_send);

    pending_tasks_->emplace(new_trans, std::pair<bool, std::unique_ptr<structure::execute_task> >{ named, std::move(task) });

    if (rx_is_debug_instance())
    {
        std::ostringstream ss;
        ss << "Method "
            << full_path
            << " started execute request with id:"
            << data.transaction_id;
        RUNTIME_LOG_DEBUG("method_data", 500, ss.str());
    }
    result = method_ptr->execute(std::move(send_data), ctx);
    if (!result)
    {
        pending_tasks_->erase(new_trans);
    }
    else
    {
        int32_t val = 0;
        val = value.simple_get_value().extract_static<int32_t>(val);
        val++;
        value.value.assign_static(val);
        value.value.set_time(ctx->now());
        context_->method_changed(this);
    }

    return result;
}

void method_data::execution_complete (runtime_transaction_id_t id, rx_result&& result, values::rx_simple_value&& data)
{
    if (context_)
    {

        if (rx_is_debug_instance())
        {
            std::ostringstream ss;
            ss << "Method "
                << full_path
                << " execute completed for id, result:"
                << id << " - ";
            if (result)
                ss << "OK";
            else
                ss << result.errors_line();
            RUNTIME_LOG_DEBUG("method_data", 500, ss.str());
        }
        method_execute_result_data result_data;
        result_data.result = std::move(result);
        result_data.transaction_id = id;
        result_data.data = std::move(data);
        result_data.whose = this;
        context_->method_result_pending(std::move(result_data));
    }
}

void method_data::process_execute_result (runtime_transaction_id_t id, rx_result&& result, values::rx_simple_value&& data)
{

    int32_t val = 0;
    val = value.simple_get_value().extract_static<int32_t>(val);
    val--;
    value.value.assign_static(val);
    value.value.set_time(rx_time::now());
    context_->method_changed(this);

    auto it = pending_tasks_->find(id);
    if (it != pending_tasks_->end())
    {
        auto task = std::move(it->second);
        pending_tasks_->erase(it);

        if (task.second)
        {
            if (task.first)
            {
                data::runtime_values_data rt_data;
                if (result)
                {                    
                    rx_result temp_result = outputs.create_safe_runtime_data(data, rt_data);
                    if (!temp_result)
                    {
                        temp_result.register_error("Error converting output parameters!");
                        RUNTIME_LOG_WARNING("method_data", 300, temp_result.errors_line());
                        result = std::move(temp_result);
                    }
                }
                task.second->process_result(std::move(result), std::move(rt_data));

            }
            else
            {
                if (result)
                {
                    rx_result temp_result = outputs.check_value(data);
                    if (!temp_result)
                    {
                        temp_result.register_error("Error converting output parameters!");
                        RUNTIME_LOG_WARNING("method_data", 300, temp_result.errors_line());
                        result = std::move(temp_result);
                    }
                }

                task.second->process_result(std::move(result), std::move(data));
            }
        }
    }
}

rx_result method_data::initialize_runtime (runtime::runtime_init_context& ctx)
{

    // point_count!!!!
    //ctx.points_count += 2;
    ctx.structure.push_item(*item);
    ctx.method = this;
    method_ptr->container_ = this;

    auto ret = item->initialize_runtime(ctx);
    if (!ret)
        return ret;
    ret = inputs.initialize_runtime(ctx);
    if (!ret)
        return ret;
    ret = outputs.initialize_runtime(ctx);
    if (!ret)
        return ret;

    ret = method_ptr->initialize_runtime(ctx);
    if (!ret)
        return ret;


    if (rx_is_debug_instance())
        full_path = ctx.meta.get_full_path() + RX_OBJECT_DELIMETER + ctx.path.get_current_path();

    ctx.method = nullptr;
    ctx.structure.pop_item();

    return true;
}

rx_result method_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    auto result = method_ptr->deinitialize_runtime(ctx);
    if (result)
    {
        result = item->deinitialize_runtime(ctx);
        if (result)
        {
            result = inputs.deinitialize_runtime(ctx);
            if (result)
                result = outputs.deinitialize_runtime(ctx);
        }
    }
    method_ptr->container_ = nullptr;

    return result;
}

rx_result method_data::start_runtime (runtime::runtime_start_context& ctx)
{
    context_ = ctx.context;
    ctx.structure.push_item(*item);
    ctx.method = this;

    value.value.set_time(ctx.now());

    auto result = item->start_runtime(ctx);
    if (result)
    {
        if (result)
        {
            result = inputs.start_runtime(ctx);
            if (result)
            {
                result = outputs.start_runtime(ctx);
            }
        }
    }
    if(result)
        result = method_ptr->start_runtime(ctx);

    ctx.method = nullptr;
    ctx.structure.pop_item();
    return result;
}

rx_result method_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
    auto result = method_ptr->stop_runtime(ctx);
    if (result)
    {
        result = item->stop_runtime(ctx);
        if (result)
        {
            result = inputs.stop_runtime(ctx);
            if (result)
                result = outputs.stop_runtime(ctx);
        }
    }
    context_ = nullptr;
    return result;
}

data::runtime_data_model method_data::get_method_inputs ()
{
    return inputs.create_runtime_model();
}

data::runtime_data_model method_data::get_method_outputs ()
{
    return outputs.create_runtime_model();
}


} // namespace logic_blocks
} // namespace runtime
} // namespace rx_platform

