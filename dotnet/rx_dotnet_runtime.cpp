

/****************************************************************************
*
*  dotnet\rx_dotnet_runtime.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


// rx_dotnet_runtime
#include "dotnet/rx_dotnet_runtime.h"

#include "runtime_internal/rx_runtime_internal.h"
#include "lib/rx_ser_json.h"
#include "rx_dotnet.h"
#include "sys_internal/rx_async_functions.h"

extern "C"
{
    rx_result_struct c_dotnet_object_post_job(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        if (whose == nullptr)
        {
            return rx_result(RX_ERROR_STOPPED).move();
        }
        rx_internal::dotnet::dotnet_object_runtime* self = (rx_internal::dotnet::dotnet_object_runtime*)whose;
        switch (type)
        {
            case RX_JOB_REGULAR:
                if (period == 0)
                {
                    self->post_own_job(job);
                    return rx_result(true).move();
                }
            case RX_JOB_SLOW:
                //return rx_platform::extern_timers::instance().create_calc_timer(job, period, self->get_jobs_queue());
            case RX_JOB_IO:
                //return rx_platform::extern_timers::instance().create_timer(job, period, rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_IO));
            default:
                return rx_result(RX_NOT_SUPPORTED).move();
        }

    }

    host_object_def_struct _g_dotnet_object_def_
    {
        {
            c_dotnet_object_post_job
            , nullptr
            , nullptr
            , nullptr
            , nullptr
        }
    };
} // extern "C"


namespace rx_internal {

namespace dotnet {
void init_connection_data(const string_type& input, string_array& init_data, string_array& conn_data)
{
    string_type dyn_conns;
    string_type init_conns;

    auto idxs = input.find('|');
    if (idxs != string_type::npos)
    {
        dyn_conns = input.substr(0, idxs);
        idxs++;
        init_conns = input.substr(idxs);
    }
    else
    {
        dyn_conns = input;
    }

    size_t idx1 = 0;
    size_t idx2 = 0;
    if (!dyn_conns.empty())
    {
        idx2 = dyn_conns.find(';');
        while (idx2 != string_type::npos)
        {
            conn_data.push_back(dyn_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = dyn_conns.find(';', idx1);
        }
        conn_data.push_back(dyn_conns.substr(idx1));
    }
    if (!init_conns.empty())
    {
        idx1 = 0;
        idx2 = init_conns.find(';');
        while (idx2 != string_type::npos)
        {
            init_data.push_back(init_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = init_conns.find(';', idx1);
        }
        init_data.push_back(init_conns.substr(idx1));
    }
}

// Class rx_internal::dotnet::dotnet_method 

dotnet_method::dotnet_method()
{
}


dotnet_method::~dotnet_method()
{
}



rx_result dotnet_method::initialize_runtime (runtime::runtime_init_context& ctx)
{
    inputs_ = get_method_inputs();
    outputs_ = get_method_outputs();

	method_name_ = ctx.method->name;

    std::scoped_lock _(rx_internal::dotnet::dotnet_object_runtime::instances_lock);
    auto it = rx_internal::dotnet::dotnet_object_runtime::runtime_instances.find(ctx.meta.id);
    if (it != rx_internal::dotnet::dotnet_object_runtime::runtime_instances.end())
    {
		my_object_ = it->second;
    }
    return true;
}

rx_result dotnet_method::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result dotnet_method::start_runtime (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result dotnet_method::stop_runtime (runtime::runtime_stop_context& ctx)
{
	my_object_ = dotnet_object_runtime::smart_ptr::null_ptr;
    return true;
}

rx_result dotnet_method::execute (execute_data data, runtime::runtime_process_context* ctx)
{
    if (my_object_)
    {
        string_type values = "{ }";
        if (!data.value.is_null())
        {
            data::runtime_values_data rt;
            auto result = inputs_.fill_runtime_value(rt, data.value);
            if (result)
            {
                serialization::json_writer writer;
                writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                if (writer.write_init_values(nullptr, rt))
                {
                    writer.write_footer();
					values = writer.get_string();
                }
            }
            else
            {
                return result;
            }
        }
        return my_object_->execute_method(data.transaction_id, method_name_, values, smart_this());
    }
    return RX_NOT_CONNECTED;
}

void dotnet_method::send_execute_result (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id)
{
    execute_result_received(std::move(out_val), std::move(result), id);
}

void dotnet_method::send_execute_result (data::runtime_values_data out_val, rx_result&& result, runtime_transaction_id_t id)
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

void dotnet_method::received_execute_done (uint32_t transId, const string_type& val, rx_result result)
{
	serialization::json_reader reader;
    if (result)
    {
        if (reader.parse_data(val))
        {
            data::runtime_values_data rt;
            if (reader.read_init_values(nullptr, rt))
            {
                send_execute_result(std::move(rt), std::move(result), transId);
            }
        }
    }
    else
    {
		send_execute_result(rx_simple_value(), std::move(result), transId);
    }
}


// Class rx_internal::dotnet::dotnet_object_runtime 

std::map<rx_node_id, dotnet_object_runtime::smart_ptr> dotnet_object_runtime::runtime_instances;

locks::slim_lock dotnet_object_runtime::instances_lock;

dotnet_object_runtime::dotnet_object_runtime (string_type conn_data, rx_node_id type_id, dotnet_code_info code_info)
      : type_id_(type_id),
        code_info_(code_info)
{
    DOTNET_LOG_DEBUG("dotnet_object_runtime", 100, "dotnet proxy object created.");
    string_type dyn_conns;
    string_type init_conns;
    string_type rel_conns;
    string_type method_conns;

    auto idxs = conn_data.find('|');
    if (idxs != string_type::npos)
    {
        dyn_conns = conn_data.substr(0, idxs);
		idxs++;
        auto idxe = conn_data.find('|', idxs);
        if (idxe != string_type::npos)
        {
            init_conns = conn_data.substr(idxs, idxe - idxs);
            auto idxm = conn_data.find('|', idxe + 1);
            if (idxm != string_type::npos)
            {
                rel_conns = conn_data.substr(idxe + 1, idxm - idxe - 1);
                method_conns = conn_data.substr(idxm + 1);
            }
            else
            {
                rel_conns = conn_data.substr(idxe + 1);
            }
        }
        else
        {
            init_conns = conn_data.substr(idxs);
		}
    }
    else
    {
        dyn_conns = conn_data;
    }

    size_t idx1 = 0;
    size_t idx2 = 0;
    if (!dyn_conns.empty())
    {
        idx2 = dyn_conns.find(';');
        while (idx2 != string_type::npos)
        {
            conn_data_.push_back(dyn_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = dyn_conns.find(';', idx1);
        }
        conn_data_.push_back(dyn_conns.substr(idx1));
    }
    if (!init_conns.empty())
    {
        idx1 = 0;
        idx2 = init_conns.find(';');
        while (idx2 != string_type::npos)
        {
            init_data_.push_back(init_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = init_conns.find(';', idx1);
        }
        init_data_.push_back(init_conns.substr(idx1));
    }
    if (!rel_conns.empty())
    {
        idx1 = 0;
        idx2 = rel_conns.find(';');
        while (idx2 != string_type::npos)
        {
            rel_data_.push_back(rel_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = rel_conns.find(';', idx1);
        }
        rel_data_.push_back(rel_conns.substr(idx1));
    }
    if (!method_conns.empty())
    {
        idx1 = 0;
        idx2 = method_conns.find(';');
        while (idx2 != string_type::npos)
        {
            method_data_.push_back(method_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = method_conns.find(';', idx1);
        }
        method_data_.push_back(method_conns.substr(idx1));
    }
    my_relations_ = std::make_unique<std::map<string_type, rx_reference<dotnet_relation> >>();
}


dotnet_object_runtime::~dotnet_object_runtime()
{
    DOTNET_LOG_DEBUG("dotnet_object_runtime", 100, "dotnet proxy object destroyed.");
}



rx_result dotnet_object_runtime::initialize_runtime (runtime_init_context& ctx)
{
	rx::serialization::json_writer writer;
	writer.write_header(STREAMING_TYPE_MESSAGE, 0);
	writer.write_init_values(nullptr, ctx.init_data);
	writer.write_footer();
	string_type init = writer.get_string();
    dotnet_hosting::instance().bind_runtime(ctx.meta.id, ctx.meta.get_full_path(), type_id_, this, smart_this(), init);

    int idx = 0;

    if(!conn_data_.empty())
    {
        binded_values_.reserve(conn_data_.size());
        for (const auto& conn : conn_data_)
        {
			auto temp = std::make_unique<binded_value_data>();
            temp->model = ctx.get_data_model("."s + conn);
            if(!temp->model.elements.empty())
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_object_runtime", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
			}
            else
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_object_runtime", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
            }
            binded_values_.push_back(std::move(temp));
			idx++;
        }
	}
    if(!rel_data_.empty())
    {
        relation_values_.reserve(rel_data_.size());
        for (const auto& rel : rel_data_)
        {
			auto rel_it = my_relations_->find(rel);
            if (rel_it != my_relations_->end())
            {
				rel_it->second->set_index(idx);
            }
            auto temp = std::make_unique<runtime::async_complex_value>();
            auto result = temp->bind(rel, ctx);
            if (!result)
            {
                DOTNET_LOG_ERROR("dotnet_object_runtime", 900
                    , rx_create_string("Failed to bind to ", rel, result.errors_line().c_str()));
            }
            relation_values_.push_back(std::move(temp));
            idx++;
        }
	}

    if (!method_data_.empty())
    {
        method_values_.reserve(method_data_.size());
        for (const auto& method : method_data_)
        {
            auto temp = std::make_unique<execute_value_data>();
            temp->model = ctx.get_data_model("."s + method + RX_OBJECT_DELIMETER + "Out");

            auto result = temp->value.bind(method, ctx);
            if (!result)
            {
                DOTNET_LOG_ERROR("dotnet_object_runtime", 900
                    , rx_create_string("Failed to bind to ", method, result.errors_line().c_str()));
            }
            method_values_.push_back(std::move(temp));
            idx++;
        }
    }
    my_relations_.reset();
    DOTNET_LOG_DEBUG("dotnet_object_runtime", 100, "dotnet proxy object initialized.");
    return true;
}

rx_result dotnet_object_runtime::start_runtime (runtime_start_context& ctx)
{
    std::vector<rx_simple_value> initial_values;
	string_array init_names_array;
    if (!init_data_.empty() || !conn_data_.empty())
    {

		initial_values.reserve(init_data_.size() + conn_data_.size());
		init_names_array.reserve(init_data_.size() + conn_data_.size());

        for(const auto& one : init_data_)
        {
            rx_simple_value temp;
            auto result = ctx.get_item(one, temp);
            if (!result)
            {
                temp = rx_simple_value();
            }
			initial_values.push_back(std::move(temp));
			init_names_array.push_back(one);
		}
        size_t idx = 0;
        for (const auto& one : conn_data_)
        {
            rx_simple_value temp;
            auto result = ctx.get_item(one, temp);
            if (!result)
            {
                temp = rx_simple_value();
            }
            if (binded_values_[idx])
            {
                if(!binded_values_[idx]->model.elements.empty())
                {
                    rx_simple_value ser_val;
                    data::runtime_values_data rt;
                    auto result = binded_values_[idx]->model.fill_runtime_value(rt, temp);
                    if (result)
                    {
                        serialization::json_writer writer;
                        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                        if (writer.write_init_values(nullptr, rt))
                        {
                            writer.write_footer();
							string_type str_val = writer.get_string();
				             ser_val.assign_static(str_val);
							temp = std::move(ser_val);
                        }
                    }
                }
				else
                {
                    temp = binded_values_[idx]->value.value();
                }
            }
            initial_values.push_back(std::move(temp));
            init_names_array.push_back(one);
            idx++;
        }
    }
    dotnet_hosting::instance().send_initial_values(std::move(init_names_array), std::move(initial_values), rx_item_type::rx_object, this);

    DOTNET_LOG_DEBUG("dotnet_object_runtime", 100, "dotnet proxy object started.");
    return true;
}

rx_result dotnet_object_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
    dotnet_hosting::instance().unbind_runtime(ctx.meta.id, type_id_, this);
    DOTNET_LOG_DEBUG("dotnet_object_runtime", 100, "dotnet proxy object deinitialized.");
    return true;
}

rx_result dotnet_object_runtime::stop_runtime (runtime_stop_context& ctx)
{
    DOTNET_LOG_DEBUG("dotnet_object_runtime", 100, "dotnet proxy object stopped.");
    return true;
}

void dotnet_object_runtime::fill_code_info (std::ostream& info, const string_type& name) const
{
    rx_time_struct ret;
    create_module_compile_time(__DATE__, __TIME__, &ret);
    info << "CODE INFO" << "\r\n";
    info << "--------------------------------------------------------------------------------" << "\r\n";
    info << "name       : " << name << "\r\n";
    info << "subsystem  : " << "dotnet" << "\r\n";
    info << "--------------------------------------------------------------------------------" << "\r\n";
    info << "assembly   : " << code_info_.assembly_name << "\r\n";
    info << "file       : " << code_info_.file_name << "\r\n";
    info << "class name : " << code_info_.class_name << "\r\n";
}

void dotnet_object_runtime::post_own_job (plugin_job_struct* what)
{
    RX_ASSERT(false);
    //auto job = rx_create_reference<extern_job>(what);
    //get_context()->own_pending(job);
}

void dotnet_object_runtime::received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
    /*DOTNET_LOG_DEBUG("dotnet_object_runtime", 100
		, rx_create_string("Received write value, transId: ", transId, ", value: ", val.to_string().c_str()));*/

    if(idx < binded_values_.size())
    {        
		auto callback_ptr = std::make_shared<rx_result_callback>(smart_this(), std::move(callback), get_executer());
        binded_values_[idx]->value.commit(std::move(val), [trans_id = transId, callback_ptr = callback_ptr]
        (runtime_transaction_id_t , rx_result result) mutable
            {
                (*callback_ptr)(std::move(result));
             /*   DOTNET_LOG_DEBUG("dotnet_object_runtime", 100
                    , rx_create_string("Received write result for transId: ", trans_id, "."));*/

            });
    }
    else if (idx < binded_values_.size() + relation_values_.size())
    {
        size_t rel_idx = idx - binded_values_.size();
        auto callback_ptr = std::make_shared<rx_result_callback>(smart_this(), std::move(callback), get_executer());
		relation_values_[rel_idx]->commit(std::move(val), [trans_id = transId, callback_ptr = callback_ptr]
        (runtime_transaction_id_t, rx_result result) mutable
            {
                (*callback_ptr)(std::move(result));
      /*          DOTNET_LOG_DEBUG("dotnet_object_runtime", 100
                    , rx_create_string("Received write result for transId: ", trans_id, "."));*/

            });
    }
    else
    {
        callback(rx_result(RX_INVALID_ARGUMENT));
    }
}

void dotnet_object_runtime::received_execute_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_simple_value&&, rx_result&&)> callback)
{
    DOTNET_LOG_DEBUG("dotnet_object_runtime", 100
        , rx_create_string("Received execute value, transId: ", transId, ", value: ", val.to_string().c_str()));

    rx_post_function_to(get_executer(), smart_this(), [this, transId, idx, val = std::move(val), callback = std::move(callback)]()
        {
            if (idx >= binded_values_.size() + relation_values_.size())
            {
                size_t met_idx = idx - binded_values_.size() - relation_values_.size();
                if (met_idx < method_values_.size())
                {
					auto model = method_values_[met_idx]->model;
                    auto callback_ptr = std::make_shared<rx_platform::callback::rx_any_callback<rx_simple_value&&, rx_result&&>>(smart_this(), std::move(callback), get_executer());
                    method_values_[met_idx]->value.execute(std::move(val), [model = std::move(model), trans_id = transId, callback_ptr = callback_ptr]
                    (runtime_transaction_id_t, rx_result result, rx_simple_value val) mutable
                        {
                            if (result)
                            {
                                data::runtime_values_data rt;
                                result = model.fill_runtime_value(rt, val);
                                if(result)
                                 {
                                    serialization::json_writer writer;
                                    writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                                    if (writer.write_init_values(nullptr, rt))
                                    {
                                        writer.write_footer();
                                        val.assign_static(writer.get_string());
                                    }
                                    else
                                    {
                                        result = ("Failed to serialize output value!");
                                        val = rx_simple_value();
                                    }
                                }
                                else
                                {
                                    result = ("Failed to convert output value!");
                                    val = rx_simple_value();
								}
                            }
                            (*callback_ptr)(std::move(val), std::move(result));
                            DOTNET_LOG_DEBUG("dotnet_object_runtime", 100
                                , rx_create_string("Received execute result for transId: ", trans_id, "."));

                        });
                }
                else
                {
                    callback(rx_simple_value(), rx_result(RX_INVALID_ARGUMENT ". No method under index"));
                }
            }
            else
            {
                callback(rx_simple_value(), rx_result(RX_INVALID_ARGUMENT ". Index out of range"));
            }
        });
}

void dotnet_object_runtime::binded_value_changed (int idx, const values::rx_simple_value& val)
{
    if (binded_values_.size() > idx)
    {
        if (!binded_values_[idx]->model.elements.empty())
        {
            rx_simple_value ser_val;
            data::runtime_values_data rt;
            auto result = binded_values_[idx]->model.fill_runtime_value(rt, val);
            if (result)
            {
                serialization::json_writer writer;
                writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                if (writer.write_init_values(nullptr, rt))
                {
                    writer.write_footer();
                    ser_val.assign_static(writer.get_string());
                }
            }
            dotnet_hosting::instance().runtime_value_changed(idx, std::move(ser_val), rx_item_type::rx_object, this);
        }
        else
        {
            dotnet_hosting::instance().runtime_value_changed(idx, rx_simple_value(val), rx_item_type::rx_object, this);
        }
    }
}

rx_result dotnet_object_runtime::execute_method (uint32_t trans_id, const string_type& method, const string_type& value, rx_reference<dotnet_method> whose)
{
    {
		std::scoped_lock _(execute_transactions_lock_);
		execute_transactions_.emplace(trans_id, execute_transaction_data{ std::move(whose) });
    }
    auto result = dotnet_hosting::instance().execute_method(trans_id, this, method, value);
    if (!result)
    {
		std::scoped_lock _(execute_transactions_lock_);
        execute_transactions_.erase(trans_id);
    }
	return result;
}

void dotnet_object_runtime::received_execute_done (uint32_t transId, const string_type& val, rx_result result)
{
    std::scoped_lock _(execute_transactions_lock_);
	auto it = execute_transactions_.find(transId);
    if (it != execute_transactions_.end())
    {
        auto method = it->second.method;
        execute_transactions_.erase(it);
        if (method)
        {
			method->received_execute_done(transId, val, std::move(result));
        }
    }
}

void dotnet_object_runtime::register_relation (const string_type& name, rx_reference<dotnet_relation> who)
{
    if(my_relations_)
    {
        (*my_relations_)[name] = std::move(who);
	}
}


// Class rx_internal::dotnet::dotnet_relation 

dotnet_relation::dotnet_relation()
      : index_(0x8000)
{
}


dotnet_relation::~dotnet_relation()
{
}



rx_result dotnet_relation::initialize_relation (runtime::runtime_init_context& ctx)
{
    relation_name_ = ctx.path.get_current_path();

    std::scoped_lock _(rx_internal::dotnet::dotnet_object_runtime::instances_lock);
    auto it = rx_internal::dotnet::dotnet_object_runtime::runtime_instances.find(ctx.meta.id);
    if (it != rx_internal::dotnet::dotnet_object_runtime::runtime_instances.end())
    {
        my_object_ = it->second;
		my_object_->register_relation(relation_name_, smart_this());
    }
    return true;
}

rx_result dotnet_relation::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result dotnet_relation::start_relation (runtime::runtime_start_context& ctx, bool is_target)
{
    return true;
}

rx_result dotnet_relation::stop_relation (runtime::runtime_stop_context& ctx, bool is_target)
{
    my_object_ = dotnet_object_runtime::smart_ptr::null_ptr;
    return true;
}

rx_result_with<platform_item_ptr> dotnet_relation::resolve_runtime_sync (const rx_node_id& id)
{
    auto port_ptr = rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().get_object(id);
    if (!port_ptr)
    {
        return "Item not registered!";
    }
    else
    {

        {
            std::scoped_lock lock(rx_internal::dotnet::dotnet_object_runtime::instances_lock);
            auto it = rx_internal::dotnet::dotnet_object_runtime::runtime_instances.find(id);
            if (it != rx_internal::dotnet::dotnet_object_runtime::runtime_instances.end())
            {
                other_object_ = it->second;
            }
        }
        if (other_object_)
        {
            auto ret = port_ptr->get_item_ptr();
            return ret;
        }
        else
        {
			return "Item is not a .NET object!";
        }
    }
}

void dotnet_relation::relation_connected ()
{
    if(my_object_ && other_object_ && index_ < 0x8000)
    {
        uint64_t other = (uint64_t)other_object_.unsafe_ptr();
		rx_simple_value val;
		val.assign_static(other);
        dotnet_hosting::instance().runtime_value_changed(index_, std::move(val), rx_item_type::rx_object, my_object_.unsafe_ptr());
	}
}

void dotnet_relation::relation_disconnected ()
{
    if (my_object_ && other_object_ && index_ < 0x8000)
    {
        other_object_ = dotnet_object_runtime::smart_ptr::null_ptr;
        uint64_t other = 0;
        rx_simple_value val;
        val.assign_static(other);
        dotnet_hosting::instance().runtime_value_changed(index_, std::move(val), rx_item_type::rx_object, my_object_.unsafe_ptr());
    }
}

void dotnet_relation::set_index (size_t index)
{
	index_ = index;
}


// Class rx_internal::dotnet::dotnet_event 

dotnet_event::dotnet_event (string_type conn_data, rx_node_id type_id)
      : conn_data_(conn_data),
        type_id_(type_id)
{
}


dotnet_event::~dotnet_event()
{
}



rx_result dotnet_event::initialize_event (runtime::runtime_init_context& ctx)
{
    object_id_ = ctx.meta.id;
    path_ = ctx.path.get_current_path();
    string_type init;
	const auto& init_rt = ctx.init_data.get_child(ctx.path.get_current_path());
    if (!init_rt.empty())
    {
        rx::serialization::json_writer writer;
        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
        writer.write_init_values(nullptr, init_rt);
        writer.write_footer();
        init = writer.get_string();
    }
    dotnet_hosting::instance().bind_event(object_id_, path_, type_id_, this, smart_this(), init);
    string_type dyn_conns;
    auto idxs = conn_data_.find('|');
    if (idxs != string_type::npos)
    {
        dyn_conns = conn_data_.substr(0, idxs);
    }
    else
    {
        dyn_conns = conn_data_;
    }

    string_array binding_conns;
    size_t idx1 = 0;
    size_t idx2 = dyn_conns.find(';');
    while (idx2 != string_type::npos)
    {
        binding_conns.push_back(dyn_conns.substr(idx1, idx2 - idx1));
        idx1 = idx2 + 1;
        idx2 = dyn_conns.find(';', idx1);
    }
    binding_conns.push_back(dyn_conns.substr(idx1));
    if (!binding_conns.empty())
    {
        binded_values_.reserve(binding_conns.size());
        int idx = 0;
        string_type conn;
        for (const auto& conn1 : binding_conns)
        {
            if (conn1.empty())
                continue;
            conn = "."s + conn1;
            auto temp = std::make_unique<binded_value_data>();
            temp->model = ctx.get_data_model(conn);
            if (!temp->model.elements.empty())
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_event", 900
                        , rx_create_string("Failed to bind event to ", conn, result.errors_line().c_str()));
                }
            }
            else
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_event", 900
                        , rx_create_string("Failed to bind event to ", conn, result.errors_line().c_str()));
                }
            }
            binded_values_.push_back(std::move(temp));
            idx++;
        }
    }
    DOTNET_LOG_DEBUG("dotnet_event", 100, "dotnet proxy event initialized.");
	return true;
}

rx_result dotnet_event::start_event (runtime::runtime_start_context& ctx)
{
    string_type stat_conns;
    auto idxs = conn_data_.find('|');
    if (idxs != string_type::npos)
    {
        stat_conns = conn_data_.substr(idxs + 1);
    }
    else
    {
        stat_conns = "";
    }
    string_array binding_conns;
    std::vector<rx_simple_value> initial_values;
    if (!stat_conns.empty())
    {
        size_t idx1 = 0;
        size_t idx2 = stat_conns.find(';');
        while (idx2 != string_type::npos)
        {
            binding_conns.push_back(stat_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = stat_conns.find(';', idx1);
        }
        binding_conns.push_back(stat_conns.substr(idx1));

        initial_values.reserve(binding_conns.size());

        for (const auto& one : binding_conns)
        {
            rx_simple_value temp;
            auto result = ctx.get_item("."s + one, temp);
            if (!result)
            {
                temp = rx_simple_value();
            }
            initial_values.push_back(std::move(temp));
        }
    }
    dotnet_hosting::instance().send_initial_values(std::move(binding_conns), std::move(initial_values), rx_item_type::rx_event_type, this);

    DOTNET_LOG_DEBUG("dotnet_event", 100, "dotnet proxy event started.");
	return true;
}

rx_result dotnet_event::stop_event (runtime::runtime_stop_context& ctx)
{
    DOTNET_LOG_DEBUG("dotnet_event", 100, "dotnet proxy event initialized.");
	return true;
}

rx_result dotnet_event::deinitialize_event (runtime::runtime_deinit_context& ctx)
{
	dotnet_hosting::instance().unbind_event(object_id_, path_, type_id_, this);
	return true;
}

void dotnet_event::fill_code_info (std::ostream& info, const string_type& name) const
{
}

void dotnet_event::received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
    /*DOTNET_LOG_DEBUG("dotnet_event", 100
        , rx_create_string("Received write value, transId: ", transId, ", value: ", val.to_string().c_str()));*/

    if (idx < binded_values_.size())
    {
        auto callback_ptr = std::make_shared<rx_result_callback>(smart_this(), std::move(callback));
        binded_values_[idx]->value.commit(rx_simple_value(val), [trans_id = transId, callback_ptr = callback_ptr]
        (runtime_transaction_id_t, rx_result result) mutable
            {
                (*callback_ptr)(std::move(result));
               /* DOTNET_LOG_DEBUG("dotnet_object_runtime", 100
                    , rx_create_string("Received write result for transId: ", trans_id, "."));*/

            });
    }
    else
    {
        callback(rx_result(RX_INVALID_ARGUMENT).move());
    }
}

void dotnet_event::binded_value_changed (int idx, const values::rx_simple_value& val)
{
    if (binded_values_.size() > idx)
    {
        if (!binded_values_[idx]->model.elements.empty())
        {
            rx_simple_value ser_val;
            data::runtime_values_data rt;
            auto result = binded_values_[idx]->model.fill_runtime_value(rt, val);
            if (result)
            {
                serialization::json_writer writer;
                writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                if (writer.write_init_values(nullptr, rt))
                {
                    writer.write_footer();
                    ser_val.assign_static(writer.get_string());
                }
            }
            dotnet_hosting::instance().runtime_value_changed(idx, std::move(ser_val), rx_item_type::rx_event_type, this);
        }
        else
        {
            dotnet_hosting::instance().runtime_value_changed(idx, rx_simple_value(val), rx_item_type::rx_event_type, this);
        }
    }
}

void dotnet_event::fire_event (string_type data, string_type queue, bool state, bool remove)
{
    data::runtime_values_data event_data;

    serialization::json_reader reader;
    if (reader.parse_data(data))
    {
        if (reader.read_init_values(nullptr, event_data))
        {
            event_fired(0, std::move(event_data), std::move(queue), state, remove);
        }
    }
}


// Class rx_internal::dotnet::dotnet_mapper 

dotnet_mapper::dotnet_mapper (string_type conn_data, rx_node_id type_id)
      : conn_data_(conn_data),
        type_id_(type_id)
{
}


dotnet_mapper::~dotnet_mapper()
{
}



rx_result dotnet_mapper::initialize_mapper (runtime::runtime_init_context& ctx)
{
    object_id_ = ctx.meta.id;
    path_ = ctx.path.get_current_path();
    string_type init;
    const auto& init_rt = ctx.init_data.get_child(ctx.path.get_current_path());
    if (!init_rt.empty())
    {
        rx::serialization::json_writer writer;
        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
        writer.write_init_values(nullptr, init_rt);
        writer.write_footer();
        init = writer.get_string();
    }
    dotnet_hosting::instance().bind_mapper(object_id_, path_, type_id_, this, smart_this(), init);
    string_type dyn_conns;
    auto idxs = conn_data_.find('|');
    if (idxs != string_type::npos)
    {
        dyn_conns = conn_data_.substr(0, idxs);
    }
    else
    {
        dyn_conns = conn_data_;
    }

    string_array binding_conns;
    size_t idx1 = 0;
    size_t idx2 = dyn_conns.find(';');
    while (idx2 != string_type::npos)
    {
        binding_conns.push_back(dyn_conns.substr(idx1, idx2 - idx1));
        idx1 = idx2 + 1;
        idx2 = dyn_conns.find(';', idx1);
    }
    binding_conns.push_back(dyn_conns.substr(idx1));
    if (!binding_conns.empty())
    {
        binded_values_.reserve(binding_conns.size());
        int idx = 0;
        string_type conn;
        for (const auto& conn1 : binding_conns)
        {
            conn = "."s + conn1;
            auto temp = std::make_unique<binded_value_data>();
            temp->model = ctx.get_data_model(conn);
            if (!temp->model.elements.empty())
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_mapper", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
            }
            else
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_mapper", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
            }
            binded_values_.push_back(std::move(temp));
            idx++;
        }
    }
    DOTNET_LOG_DEBUG("dotnet_mapper", 100, "dotnet proxy mapper initialized.");
    return true;
}

rx_result dotnet_mapper::start_mapper (runtime::runtime_start_context& ctx)
{
    string_type stat_conns;
    auto idxs = conn_data_.find('|');
    if (idxs != string_type::npos)
    {
        stat_conns = conn_data_.substr(idxs + 1);
    }
    else
    {
        stat_conns = "";
    }
    if (!stat_conns.empty())
    {
        string_array binding_conns;
        size_t idx1 = 0;
        size_t idx2 = stat_conns.find(';');
        while (idx2 != string_type::npos)
        {
            binding_conns.push_back(stat_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = stat_conns.find(';', idx1);
        }
        binding_conns.push_back(stat_conns.substr(idx1));

        std::vector<rx_simple_value> initial_values;
        initial_values.reserve(binding_conns.size());

        for (const auto& one : binding_conns)
        {
            rx_simple_value temp;
            auto result = ctx.get_item("."s + one, temp);
            if (!result)
            {
                temp = rx_simple_value();
            }
            initial_values.push_back(std::move(temp));
        }
        dotnet_hosting::instance().send_initial_values(std::move(binding_conns), std::move(initial_values), rx_item_type::rx_mapper_type, this);
    }
    DOTNET_LOG_DEBUG("dotnet_mapper", 100, "dotnet proxy mapper started.");
    return true;
}

rx_result dotnet_mapper::stop_mapper (runtime::runtime_stop_context& ctx)
{
    DOTNET_LOG_DEBUG("dotnet_mapper", 100, "dotnet proxy mapper stopped.");
    return true;
}

rx_result dotnet_mapper::deinitialize_mapper (runtime::runtime_deinit_context& ctx)
{
    dotnet_hosting::instance().unbind_mapper(object_id_, path_, type_id_, this);
    DOTNET_LOG_DEBUG("dotnet_mapper", 100, "dotnet proxy mapper deinitialized.");
	return true;
}

void dotnet_mapper::fill_code_info (std::ostream& info, const string_type& name) const
{
}

void dotnet_mapper::received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
    //DOTNET_LOG_DEBUG("dotnet_mapper", 100
    //    , rx_create_string("Received write value, transId: ", transId, ", value: ", val.to_string().c_str()));

    if (idx < binded_values_.size())
    {
        auto callback_ptr = std::make_shared<rx_result_callback>(smart_this(), std::move(callback));
        binded_values_[idx]->value.commit(rx_simple_value(val), [trans_id = transId, callback_ptr = callback_ptr]
        (runtime_transaction_id_t, rx_result result) mutable
            {
               (*callback_ptr)(std::move(result));
               /*  DOTNET_LOG_DEBUG("dotnet_object_runtime", 100
                    , rx_create_string("Received write result for transId: ", trans_id, "."));*/

            });
    }
    else
    {
        callback(rx_result(RX_INVALID_ARGUMENT).move());
    }
}

void dotnet_mapper::binded_value_changed (int idx, const values::rx_simple_value& val)
{
    if (binded_values_.size() > idx)
    {
        if (!binded_values_[idx]->model.elements.empty())
        {
            rx_simple_value ser_val;
            data::runtime_values_data rt;
            auto result = binded_values_[idx]->model.fill_runtime_value(rt, val);
            if (result)
            {
                serialization::json_writer writer;
                writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                if (writer.write_init_values(nullptr, rt))
                {
                    writer.write_footer();
                    ser_val.assign_static(writer.get_string());
                }
            }
            dotnet_hosting::instance().runtime_value_changed(idx, std::move(ser_val), rx_item_type::rx_mapper_type, this);
        }
        else
        {
            dotnet_hosting::instance().runtime_value_changed(idx, rx_simple_value(val), rx_item_type::rx_mapper_type, this);
        }
    }
}


// Class rx_internal::dotnet::dotnet_source 

dotnet_source::dotnet_source (string_type conn_data, rx_node_id type_id)
      : conn_data_(conn_data),
        type_id_(type_id)
{
}


dotnet_source::~dotnet_source()
{
}



rx_result dotnet_source::initialize_source (runtime::runtime_init_context& ctx)
{
    object_id_ = ctx.meta.id;
    path_ = ctx.path.get_current_path(); rx::serialization::json_writer writer;
    string_type init;
    const auto& init_rt = ctx.init_data.get_child(ctx.path.get_current_path());
    if (!init_rt.empty())
    {
        rx::serialization::json_writer writer;
        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
        writer.write_init_values(nullptr, init_rt);
        writer.write_footer();
        init = writer.get_string();
    }
	dotnet_hosting::instance().bind_source(object_id_, path_, type_id_, this, smart_this(), init);
    string_type dyn_conns;
    auto idxs = conn_data_.find('|');
    if (idxs != string_type::npos)
    {
        dyn_conns = conn_data_.substr(0, idxs);
    }
    else
    {
        dyn_conns = conn_data_;
    }

    string_array binding_conns;
    size_t idx1 = 0;
    size_t idx2 = dyn_conns.find(';');
    while (idx2 != string_type::npos)
    {
        binding_conns.push_back(dyn_conns.substr(idx1, idx2 - idx1));
        idx1 = idx2 + 1;
        idx2 = dyn_conns.find(';', idx1);
    }
    binding_conns.push_back(dyn_conns.substr(idx1));
    if (!binding_conns.empty())
    {
        binded_values_.reserve(binding_conns.size());
        int idx = 0;
        string_type conn;
        for (const auto& conn1 : binding_conns)
        {
            conn = "."s + conn1;
            auto temp = std::make_unique<binded_value_data>();
            temp->model = ctx.get_data_model(conn);
            if (!temp->model.elements.empty())
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_source", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
            }
            else
            {
                auto result = temp->value.bind(conn, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_source", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
            }
            binded_values_.push_back(std::move(temp));
            idx++;
        }
    }
    DOTNET_LOG_DEBUG("dotnet_source", 100, "dotnet proxy source initialized.");
    return true;
}

rx_result dotnet_source::start_source (runtime::runtime_start_context& ctx)
{
    string_type stat_conns;
    auto idxs = conn_data_.find('|');
    if (idxs != string_type::npos)
    {
        stat_conns = conn_data_.substr(idxs + 1);
    }
    else
    {
        stat_conns = "";
    }
    if (!stat_conns.empty())
    {
        string_array binding_conns;
        size_t idx1 = 0;
        size_t idx2 = stat_conns.find(';');
        while (idx2 != string_type::npos)
        {
            binding_conns.push_back(stat_conns.substr(idx1, idx2 - idx1));
            idx1 = idx2 + 1;
            idx2 = stat_conns.find(';', idx1);
        }
        binding_conns.push_back(stat_conns.substr(idx1));

        std::vector<rx_simple_value> initial_values;
        initial_values.reserve(binding_conns.size());

        for (const auto& one : binding_conns)
        {
            rx_simple_value temp;
            auto result = ctx.get_item("."s + one, temp);
            if (!result)
            {
                temp = rx_simple_value();
            }
            initial_values.push_back(std::move(temp));
        }
        dotnet_hosting::instance().send_initial_values(std::move(binding_conns), std::move(initial_values), rx_item_type::rx_source_type, this);
    }
    DOTNET_LOG_DEBUG("dotnet_source", 100, "dotnet proxy source started.");
    return true;
}

rx_result dotnet_source::stop_source (runtime::runtime_stop_context& ctx)
{
    DOTNET_LOG_DEBUG("dotnet_source", 100, "dotnet proxy source stopped.");
    return true;
}

rx_result dotnet_source::deinitialize_source (runtime::runtime_deinit_context& ctx)
{
  dotnet_hosting::instance().unbind_source(object_id_, path_, type_id_, this);
  DOTNET_LOG_DEBUG("dotnet_source", 100, "dotnet proxy source deinitialized.");
  return true;
}

void dotnet_source::fill_code_info (std::ostream& info, const string_type& name) const
{
}

void dotnet_source::received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
  /*  DOTNET_LOG_DEBUG("dotnet_source", 100
       , rx_create_string("Received write value, transId: ", transId, ", value: ", val.to_string().c_str()));
 */
    if (idx < binded_values_.size())
    {
        auto callback_ptr = std::make_shared<rx_result_callback>(smart_this(), std::move(callback));
        binded_values_[idx]->value.commit(rx_simple_value(val), [trans_id = transId, callback_ptr = callback_ptr]
        (runtime_transaction_id_t, rx_result result) mutable
            {
                (*callback_ptr)(std::move(result));
               /* DOTNET_LOG_DEBUG("dotnet_source", 100
                    , rx_create_string("Received write result for transId: ", trans_id, "."));*/

            });
    }
    else
    {
        callback(rx_result(RX_INVALID_ARGUMENT).move());
    }
}

void dotnet_source::binded_value_changed (int idx, const values::rx_simple_value& val)
{
    if (binded_values_.size() > idx)
    {
        if (!binded_values_[idx]->model.elements.empty())
        {
            rx_simple_value ser_val;
            data::runtime_values_data rt;
            auto result = binded_values_[idx]->model.fill_runtime_value(rt, val);
            if (result)
            {
                serialization::json_writer writer;
                writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                if (writer.write_init_values(nullptr, rt))
                {
                    writer.write_footer();
                    ser_val.assign_static(writer.get_string());
                }
            }
            dotnet_hosting::instance().runtime_value_changed(idx, std::move(ser_val), rx_item_type::rx_source_type, this);
        }
        else
        {
            dotnet_hosting::instance().runtime_value_changed(idx, rx_simple_value(val), rx_item_type::rx_source_type, this);
        }
    }
}

void dotnet_source::source_changed (rx_value&& val)
{
	source_value_changed(std::move(val));
}

void dotnet_source::source_result (rx_result&& result, runtime_transaction_id_t id)
{
	source_result_received(std::move(result), id);
}

void dotnet_source::source_execute_result (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id)
{
	source_execute_result_received(std::move(out_val), std::move(result), id);
}

rx_result dotnet_source::source_write (write_data&& data, runtime_process_context* ctx)
{
	return dotnet_hosting::instance().source_write(data.transaction_id, this, data.value.move());
}

rx_result dotnet_source::source_execute (execute_data&& data, runtime_process_context* ctx)
{
    return RX_NOT_IMPLEMENTED;
}


// Class rx_internal::dotnet::dotnet_struct 

dotnet_struct::dotnet_struct (string_type conn_data, rx_node_id type_id)
      : type_id_(type_id)
{
	init_connection_data(conn_data, init_data_, conn_data_);
}


dotnet_struct::~dotnet_struct()
{
}



rx_result dotnet_struct::initialize_struct (runtime::runtime_init_context& ctx)
{
	object_id_ = ctx.meta.id;
	path_ = ctx.path.get_current_path();
    string_type init;
    const auto& init_rt = ctx.init_data.get_child(ctx.path.get_current_path());
    if (!init_rt.empty())
    {
        rx::serialization::json_writer writer;
        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
        writer.write_init_values(nullptr, init_rt);
        writer.write_footer();
        init = writer.get_string();
    }
    dotnet_hosting::instance().bind_struct(object_id_, path_, type_id_, this, smart_this(), init);
    int idx = 0;

    if (!conn_data_.empty())
    {
        binded_values_.reserve(conn_data_.size());
        for (const auto& conn : conn_data_)
        {
			string_type conn_path = "."s + conn;
            auto temp = std::make_unique<binded_value_data>();
            temp->model = ctx.get_data_model(conn_path);
            if (!temp->model.elements.empty())
            {
                auto result = temp->value.bind(conn_path, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_struct", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
            }
            else
            {
                auto result = temp->value.bind(conn_path, ctx, [idx, this](const values::rx_simple_value& val)
                    {
                        binded_value_changed(idx, val);
                    });
                if (!result)
                {
                    DOTNET_LOG_ERROR("dotnet_struct", 900
                        , rx_create_string("Failed to bind to ", conn, result.errors_line().c_str()));
                }
            }
            binded_values_.push_back(std::move(temp));
            idx++;
        }
    }
    DOTNET_LOG_DEBUG("dotnet_struct", 100, "dotnet proxy struct initialized.");
    return true;
}

rx_result dotnet_struct::start_struct (runtime::runtime_start_context& ctx)
{
    std::vector<rx_simple_value> initial_values;
    string_array init_names_array;
    if (!init_data_.empty() || !conn_data_.empty())
    {

        initial_values.reserve(init_data_.size() + conn_data_.size());
        init_names_array.reserve(init_data_.size() + conn_data_.size());

        for (const auto& one : init_data_)
        {
            rx_simple_value temp;
            auto result = ctx.get_item(one, temp);
            if (!result)
            {
                temp = rx_simple_value();
            }
            initial_values.push_back(std::move(temp));
            init_names_array.push_back(one);
        }
        size_t idx = 0;
        for (const auto& one : conn_data_)
        {
            rx_simple_value temp;
            temp = binded_values_[idx]->value.value();
            if (binded_values_[idx])
            {
                if (!binded_values_[idx]->model.elements.empty())
                {
                    rx_simple_value ser_val;
                    data::runtime_values_data rt;
                    auto result = binded_values_[idx]->model.fill_runtime_value(rt, temp);
                    if (result)
                    {
                        serialization::json_writer writer;
                        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                        if (writer.write_init_values(nullptr, rt))
                        {
                            writer.write_footer();
                            string_type str_val = writer.get_string();
                            ser_val.assign_static(str_val);
                            temp = std::move(ser_val);
                        }
                    }
                }
            }
            initial_values.push_back(std::move(temp));
            init_names_array.push_back(one);
            idx++;
        }
    }
    dotnet_hosting::instance().send_initial_values(std::move(init_names_array), std::move(initial_values), rx_item_type::rx_struct_type, this);
    
    DOTNET_LOG_DEBUG("dotnet_struct", 100, "dotnet proxy struct started.");
    return true;
}

rx_result dotnet_struct::stop_struct (runtime::runtime_stop_context& ctx)
{
    DOTNET_LOG_DEBUG("dotnet_struct", 100, "dotnet proxy struct stopped.");
    return true;
}

rx_result dotnet_struct::deinitialize_struct (runtime::runtime_deinit_context& ctx)
{
    dotnet_hosting::instance().unbind_struct(object_id_, path_, type_id_, this);

    DOTNET_LOG_DEBUG("dotnet_struct", 100, "dotnet proxy struct de-initialized.");
    return true;
}

void dotnet_struct::fill_code_info (std::ostream& info, const string_type& name) const
{
}

void dotnet_struct::received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
  /*  DOTNET_LOG_DEBUG("dotnet_struct", 100
        , rx_create_string("Received write value, transId: ", transId, ", value: ", val.to_string().c_str()));*/

    if (idx < binded_values_.size())
    {
        auto callback_ptr = std::make_shared<rx_result_callback>(smart_this(), std::move(callback));
        binded_values_[idx]->value.commit(std::move(val), [trans_id = transId, callback_ptr = callback_ptr]
        (runtime_transaction_id_t, rx_result result) mutable
            {
                (*callback_ptr)(std::move(result));
             /*   DOTNET_LOG_DEBUG("dotnet_struct", 100
                    , rx_create_string("Received write result for transId: ", trans_id, "."));*/

            });
    }
    else
    {
        callback(rx_result(RX_INVALID_ARGUMENT).move());
    }
}

void dotnet_struct::binded_value_changed (int idx, const values::rx_simple_value& val)
{
    if (binded_values_.size() > idx)
    {
        if (!binded_values_[idx]->model.elements.empty())
        {
            rx_simple_value ser_val;
            data::runtime_values_data rt;
            auto result = binded_values_[idx]->model.fill_runtime_value(rt, val);
            if (result)
            {
                serialization::json_writer writer;
                writer.write_header(STREAMING_TYPE_MESSAGE, 0);
                if (writer.write_init_values(nullptr, rt))
                {
                    writer.write_footer();
                    ser_val.assign_static(writer.get_string());
                }
            }
            dotnet_hosting::instance().runtime_value_changed(idx, std::move(ser_val), rx_item_type::rx_struct_type, this);
        }
        else
        {
            dotnet_hosting::instance().runtime_value_changed(idx, rx_simple_value(val), rx_item_type::rx_struct_type, this);
        }
    }
}


// Class rx_internal::dotnet::dotnet_appliacation 

dotnet_appliacation::dotnet_appliacation()
{
}


dotnet_appliacation::~dotnet_appliacation()
{
}



rx_result dotnet_appliacation::initialize_runtime (runtime_init_context& ctx)
{
    return true;
}

rx_result dotnet_appliacation::deinitialize_runtime (runtime_deinit_context& ctx)
{
    return true;
}

rx_result dotnet_appliacation::start_runtime (runtime_start_context& ctx)
{
    return true;
}

rx_result dotnet_appliacation::stop_runtime (runtime_stop_context& ctx)
{
    return true;
}


// Class rx_internal::dotnet::dotnet_domain 

dotnet_domain::dotnet_domain()
{
}


dotnet_domain::~dotnet_domain()
{
}



rx_result dotnet_domain::initialize_runtime (runtime_init_context& ctx)
{
    return true;
}

rx_result dotnet_domain::deinitialize_runtime (runtime_deinit_context& ctx)
{
    return true;
}

rx_result dotnet_domain::start_runtime (runtime_start_context& ctx)
{
    return true;
}

rx_result dotnet_domain::stop_runtime (runtime_stop_context& ctx)
{
    return true;
}


// Class rx_internal::dotnet::dotnet_monitored_struct 

dotnet_monitored_struct::dotnet_monitored_struct (string_type conn_data, rx_node_id type_id)
      : type_id_(type_id)
{
}


dotnet_monitored_struct::~dotnet_monitored_struct()
{
}



rx_result dotnet_monitored_struct::initialize_struct (runtime::runtime_init_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result dotnet_monitored_struct::start_struct (runtime::runtime_start_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result dotnet_monitored_struct::stop_struct (runtime::runtime_stop_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result dotnet_monitored_struct::deinitialize_struct (runtime::runtime_deinit_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

void dotnet_monitored_struct::received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
}

void dotnet_monitored_struct::binded_value_changed (int idx, const values::rx_simple_value& val)
{
}


// Class rx_internal::dotnet::dotnet_monitored_object 

dotnet_monitored_object::dotnet_monitored_object (string_type conn_data, rx_node_id type_id)
      : type_id_(type_id)
{
}


dotnet_monitored_object::~dotnet_monitored_object()
{
}



rx_result dotnet_monitored_object::initialize_object (runtime::runtime_init_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result dotnet_monitored_object::start_object (runtime::runtime_start_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result dotnet_monitored_object::stop_object (runtime::runtime_stop_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result dotnet_monitored_object::deinitialize_object (runtime::runtime_deinit_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

void dotnet_monitored_object::received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
}

void dotnet_monitored_object::binded_value_changed (int idx, const values::rx_simple_value& val)
{
}


} // namespace dotnet
} // namespace rx_internal

