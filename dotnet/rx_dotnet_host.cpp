

/****************************************************************************
*
*  dotnet\rx_dotnet_host.cpp
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


// rx_dotnet_host
#include "dotnet/rx_dotnet_host.h"

#include "dotnet/dotnet_api.h"
#include "dotnet/rx_dotnet.h"


namespace rx_internal {

namespace dotnet {
sys_handle_t create_dotnet_process(const rx_dotnet_config_data_t& config, pipe_server* pipes);
void rx_terminate_process(sys_handle_t process);


template<typename T>
void dotnet_wp_connection::send_message(T message, uint32_t trans_id)
{

    std::function<void(T)> send_message = [this, trans_id](T message)
        {
            auto buffer = get_buffer();
            dotnet_wp_message_header* phead = buffer.alloc_from_buffer<dotnet_wp_message_header>();
            phead->known = RX_DOTNET_KNOWN;
            phead->type = T::msg_id;
            phead->trans = trans_id;

            auto write_result = message.serialize(buffer);
            if (!write_result)
            {
                DOTNET_LOG_DEBUG("dotnet_wp_connection", 100, rx_create_string("Error serializing message:", write_result.errors_line()).c_str());

                release_buffer(std::move(buffer));
                return;
            }
            // now write the full size of the message to the header
            phead->size = (uint32_t)buffer.size;
            auto result = pipe_server_.write_pipe(&buffer);
            if (!result)
            {
                DOTNET_LOG_DEBUG("dotnet_wp_connection", 100, rx_create_string("Error sending message:", result.errors_line()).c_str());
            }
            release_buffer(std::move(buffer));
        };
    sender_.append(rx_platform::jobs::rx_create_job<decltype(send_message), T>()
        (rx_reference_ptr(), std::move(send_message), std::move(message)));


}

// Class rx_internal::dotnet::dotnet_host

dotnet_host::dotnet_host (const configuration_data_t& data, const rx_dotnet_config_data_t& init_config)
{
}


dotnet_host::~dotnet_host()
{
}



rx_result dotnet_host::initialize (const rx_dotnet_config_data_t& config)
{
    auto result =  wp_.initialize(this, config);
    if (result)
    {
		hosted_version_ = wp_.hosted_version;
		dotnet_host_version_ = wp_.dotnet_host_version;
    }
    return result;
}

rx_result dotnet_host::start ()
{
    wp_.start();
	return true;
}

void dotnet_host::deinitialize (rx_void_callback callback)
{
    wp_.deinitialize(std::move(callback), 60000);// ("", rx_result_callback(rx_reference_ptr(), std::move(local_callback), RX_DOMAIN_META), 60000);
}

rx_result dotnet_host::do_build (configuration_data_t& config, startup_create_data_type& data)
{
    return wp_.do_build("", 60000);
}

rx_result dotnet_host::do_reload (const string_type& lib)
{
	return wp_.do_reload(lib, 60000);
}

void dotnet_host::do_unload (const string_type& lib, rx_result_callback callback)
{
    return wp_.do_unload(lib, std::move(callback), 60000);
}

void dotnet_host::received_result_callback (uint32_t transId, rx_result result)
{
    std::scoped_lock lock(callbacks_lock_);
    auto it = result_callbacks_.find(transId);
    if (it != result_callbacks_.end())
    {
        auto cb = std::move(it->second);
        result_callbacks_.erase(it);
        cb(std::move(result));
    }
}

void dotnet_host::received_void_callback (uint32_t transId)
{
    std::scoped_lock lock(callbacks_lock_);
    auto it = void_callbacks_.find(transId);
    if (it != void_callbacks_.end())
    {
        auto cb = std::move(it->second);
        void_callbacks_.erase(it);
        cb();
    }
}

void dotnet_host::received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
    switch (type)
    {
        case rx_item_type::rx_object:
            {
                auto it = runtime_objects_.find(inst);
                if (it != runtime_objects_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
        case rx_item_type::rx_struct_type:
            {
                auto it = runtime_structs_.find(inst);
                if (it != runtime_structs_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
		case rx_item_type::rx_source_type:
            {
                auto it = runtime_sources_.find(inst);
                if (it != runtime_sources_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
			break;
        case rx_item_type::rx_mapper_type:
            {
                auto it = runtime_mappers_.find(inst);
                if (it != runtime_mappers_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
		case rx_item_type::rx_event_type:
            {
                auto it = runtime_events_.find(inst);
                if (it != runtime_events_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
        default:
            callback(rx_result("Type invalid to support write!"));
			break;
    }
}

void dotnet_host::bind_runtime (const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_object_runtime> ref)
{
    runtime_objects_.emplace(ptr, std::move(ref));
    wp_.bind_object(rx_item_type::rx_object, id, path.c_str(), type_id, ptr);
}

void dotnet_host::unbind_runtime (const rx_node_id& id, const rx_node_id& type_id, void* ptr)
{
    wp_.unbind_object(rx_item_type::rx_object, id, "", type_id, ptr);
	runtime_objects_.erase(ptr);
}

void dotnet_host::runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose)
{
	wp_.runtime_value_changed(idx, std::move(val), type, whose);
}

void dotnet_host::send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose)
{
	wp_.send_initial_values(std::move(names), std::move(val), type, whose);
}

rx_result dotnet_host::execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value)
{
    return wp_.execute_method(trans_id, whose, method, value);
}

void dotnet_host::received_execute_done (uint32_t transId, void* inst, const string_type& val, rx_result result)
{
    auto it = runtime_objects_.find(inst);
    if (it != runtime_objects_.end())
    {
        it->second->received_execute_done(transId, val, std::move(result));
    }
}

void dotnet_host::bind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_struct> ref)
{
    runtime_structs_.emplace(ptr, std::move(ref));
	wp_.bind_object(rx_item_type::rx_struct_type, id, path, type_id, ptr);
}

void dotnet_host::unbind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    wp_.unbind_object(rx_item_type::rx_struct_type, id, path, type_id, ptr);
    runtime_structs_.erase(ptr);
}

void dotnet_host::bind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_source> ref)
{
    runtime_sources_.emplace(ptr, std::move(ref));
    wp_.bind_object(rx_item_type::rx_source_type, id, path, type_id, ptr);
}

void dotnet_host::unbind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    wp_.unbind_object(rx_item_type::rx_source_type, id, path, type_id, ptr);
    runtime_sources_.erase(ptr);
}

void dotnet_host::bind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_mapper> ref)
{
    runtime_mappers_.emplace(ptr, std::move(ref));
    wp_.bind_object(rx_item_type::rx_mapper_type, id, path, type_id, ptr);
}

void dotnet_host::unbind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    wp_.unbind_object(rx_item_type::rx_mapper_type, id, path, type_id, ptr);
    runtime_mappers_.erase(ptr);
}

void dotnet_host::bind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_event> ref)
{
    runtime_events_.emplace(ptr, std::move(ref));
    wp_.bind_object(rx_item_type::rx_event_type, id, path, type_id, ptr);
}

void dotnet_host::unbind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    wp_.unbind_object(rx_item_type::rx_event_type, id, path, type_id, ptr);
    runtime_events_.erase(ptr);
}

rx_result dotnet_host::source_write (uint32_t trans_id, void* whose, typed_value_type val)
{
	return wp_.source_write(trans_id, whose, std::move(val));
}

void dotnet_host::received_source_change (void* inst, rx_value val)
{
    auto it = runtime_sources_.find(inst);
    if (it != runtime_sources_.end())
    {
        it->second->source_changed(std::move(val));
    }
}

void dotnet_host::received_source_write_done (uint32_t trans_id, void* inst, rx_result result)
{
    auto it = runtime_sources_.find(inst);
    if (it != runtime_sources_.end())
    {
        it->second->source_result(std::move(result), trans_id);
    }
}

void dotnet_host::received_fire_event (void* inst, string_type val, string_type queue, bool state, bool remove)
{
    auto it = runtime_events_.find(inst);
    if (it != runtime_events_.end())
    {
        it->second->fire_event(std::move(val), std::move(queue), state, remove);
    }
}

const string_type& dotnet_host::get_hosted_dotnet_version () const
{
	return hosted_version_;
}

const string_type& dotnet_host::get_dotnet_host_version () const
{
	return dotnet_host_version_;
}

void dotnet_host::stop ()
{
	wp_.stop();
}


// Class rx_internal::dotnet::dotnet_wp_connection

dotnet_wp_connection::dotnet_wp_connection()
      : my_host_(nullptr),
        process_(0),
        initialized_(false),
        free_border_(0x1000)
    , sender_("dotnet-wp-sender", RX_DOMAIN_DOTNET)
{
	memzero(&pipes_, sizeof(pipes_));
}


dotnet_wp_connection::~dotnet_wp_connection()
{
}



rx_result dotnet_wp_connection::initialize (dotnet_host* host, const rx_dotnet_config_data_t& config)
{
	my_host_ = host;
	pipe_server_.set_my_connection(this);
	sender_.start();

    if(RX_OK != rx_create_server_side_pipe(&pipes_, RX_DOTNET_PIPE_BUFFER_SIZE))
    {
        char buff[2028];
        rx_last_os_error("Failed to create server side pipe!", buff, sizeof(buff));
        return buff;
	}

    process_ = create_dotnet_process(config, &pipes_);
    if(process_ == 0)
    {
        char buff[2028];
        rx_last_os_error("Failed to create .NET process!", buff, sizeof(buff));
        rx_destry_server_side_pipe(&pipes_);
        pipe_server_.wait_handle();
        return buff;
	}
    rx_sync_server_pipe(&pipes_);

    pipe_server_.start_receive(&pipes_);



	auto ret = initialized_.wait_handle(RX_DOTNET_INITIALIZATION_TIMEOUT);
    if (ret == RX_WAIT_TIMEOUT)
    {
        rx_destry_server_side_pipe(&pipes_);
        rx_terminate_process(process_);
		return rx_result("Timeout while waiting for .NET process to initialize!");
    }
    if (!action_result_)
    {
        rx_destry_server_side_pipe(&pipes_);
        return std::move(action_result_);
    }



    return true;
}

void dotnet_wp_connection::bind_object (rx_item_type type, const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    dotnet_bind_runtime_message msg;
	msg.type = type;
	msg.id = id;
	msg.path = path;
	msg.type_id = type_id;
	msg.ptr = (intptr_t)ptr;
    send_message(msg, 0);
}

void dotnet_wp_connection::unbind_object (rx_item_type type, const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    dotnet_unbind_runtime_message msg;
    msg.type = type;
    msg.id = id;
    msg.path = path;
    msg.type_id = type_id;
    msg.ptr = (intptr_t)ptr;
    send_message(msg, 0);
}

void dotnet_wp_connection::runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose)
{
    dotnet_value_changed_message msg;
    msg.type = type;
	msg.idx = idx;
	msg.val = std::move(val);
	msg.whose = (intptr_t)whose;
    send_message(msg, 0);
}

void dotnet_wp_connection::send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose)
{
    dotnet_initial_values_message msg;
    msg.type = type;
    msg.names = std::move(names);
    msg.values = std::move(val);
    msg.whose = (intptr_t)whose;
    send_message(msg, 0);
}

rx_result dotnet_wp_connection::execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value)
{
    auto trans = std::make_shared<execute_method_wp_transaction>(trans_id, whose, method, value);
    auto result = trans->start(*this);
    if (!result)
        return result;

    pending_out_transactions_.emplace(trans->get_transaction_id(), trans);
    return true;
}

rx_result dotnet_wp_connection::source_write (uint32_t trans_id, void* whose, typed_value_type val)
{
	return RX_NOT_IMPLEMENTED;
}

void dotnet_wp_connection::initialize_completed (uint32_t trans_id, rx_result result, string_type hosted_version, string_type dotnet_host_version)
{
	action_result_ = std::move(result);
	this->hosted_version = std::move(hosted_version);
	this->dotnet_host_version = std::move(dotnet_host_version);
	initialized_.set();
}

rx_result dotnet_wp_connection::do_build (const string_type& lib, uint32_t timeout)
{

	auto trans = std::make_shared<build_types_wp_transaction>(lib, timeout);
    auto result = trans->start(*this);
    if(!result)
        return result;
    pending_out_transactions_.emplace(trans->get_transaction_id(), trans);
	result = trans->wait_result();
    return result;
}

rx_result dotnet_wp_connection::do_reload (const string_type& lib, uint32_t timeout)
{
    auto trans = std::make_shared<reload_wp_transaction>(lib, timeout);
    auto result = trans->start(*this);
    if (!result)
        return result;
    pending_out_transactions_.emplace(trans->get_transaction_id(), trans);
    result = trans->wait_result();
    return result;
}

void dotnet_wp_connection::do_unload (const string_type& lib, rx_result_callback callback, uint32_t timeout)
{
    auto trans = std::make_shared<unload_wp_transaction>(lib, std::move(callback), timeout);
    auto result = trans->start(*this);
    if(!result)
    {
        callback(std::move(result));
        return;
	}
    pending_out_transactions_.emplace(trans->get_transaction_id(), trans);
}

io::rx_io_buffer dotnet_wp_connection::get_buffer ()
{
    if (!free_buffers_.empty())
    {
        io::rx_io_buffer ret = std::move(free_buffers_.top());
        free_buffers_.pop();
        return ret;
    }
    else
    {
        io::rx_io_buffer new_buffer(0x10000, 0);
        return new_buffer;
    }
}

void dotnet_wp_connection::release_buffer (io::rx_io_buffer what)
{
    if (what.size <= free_border_)
    {
        what.reinit();
        free_buffers_.push(std::move(what));
    }
}

rx_result dotnet_wp_connection::received (io::rx_const_io_buffer& buffer)
{
    const dotnet_wp_message_header* phead = buffer.get_from_buffer<dotnet_wp_message_header>();
    RX_ASSERT(phead != nullptr && phead->known == RX_DOTNET_KNOWN);
    switch (phead->type)
    {
        case worker_initialized_message::msg_id:
            {
                worker_initialized_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing initialization result: ", result.errors_line().c_str());
                }
                initialize_completed(phead->trans, std::move(msg.action_result), std::move(msg.hosted_version), std::move(msg.dotnet_host_version));

                return true;
            }
            break;
        case write_log_message::msg_id:
            {
                write_log_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing log message: ", result.errors_line().c_str());
                }
                log::log_object::instance().log_event_fast((log::log_event_type)msg.type
                    , msg.library.c_str(), msg.source.c_str(), msg.level, msg.code.c_str()
                    , log_callback_func_t(), msg.message.c_str());
                return true;
            }
            break;
        case build_one_type_message::msg_id:
            {
                build_one_type_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing build type message: ", result.errors_line().c_str());
                }
                result = api::dotnetBuildType(msg.type
                    , msg.library.c_str()
                    , msg.id
                    , msg.parent
                    , msg.name
                    , msg.path
                    , msg.version
                    , msg.flags
                    , msg.def
                    , msg.ownType
                    , msg.conns
                    , msg.code_info);
                build_one_type_response response;
                response.action_result = std::move(result);
                send_message(std::move(response), phead->trans);
                return true;
            }
            break;
        case build_runtime_message::msg_id:
            {
                build_runtime_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing build runtime message: ", result.errors_line().c_str());
                }
                std::function<void(rx_result&&)> callback = [this, trans_id = phead->trans](rx_result&& result) mutable
                    {
                        build_runtime_response response;
                        response.action_result = std::move(result);
                        send_message(std::move(response), trans_id);
                    };
                api::dotnetBuildRuntime(msg.type
                    , msg.library.c_str()
                    , msg.id
                    , msg.parent
                    , msg.name
                    , msg.path
                    , msg.version
                    , msg.flags
                    , msg.def
					, std::move(callback)
                    );
                return true;
            }
            break;
        case delete_one_type_message::msg_id:
            {
                delete_one_type_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing build type message: ", result.errors_line().c_str());
                }
                result = api::dotnetDeleteType(msg.type
                    , msg.library.c_str()
                    , msg.id.c_ptr());
                wp_result_response response;
                response.action_result = std::move(result);
                send_message(std::move(response), phead->trans);
                return true;
            }
            break;

        case delete_one_runtime_message::msg_id:
            {
                delete_one_runtime_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing delete runtime message: ", result.errors_line().c_str());
                }
                std::function<void(rx_result&&)> callback = [this, trans_id = phead->trans](rx_result&& result) mutable
                    {
                        wp_result_response response;
                        response.action_result = std::move(result);
                        send_message(std::move(response), trans_id);
                    };
                api::dotnetDeleteRuntime(msg.type
                    , msg.library.c_str()
                    , msg.id.c_ptr()
                    , std::move(callback)
                );
                return true;
            }
            break;

        case wp_write_value_message::msg_id:
            {
                wp_write_value_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing write value message: ", result.errors_line().c_str());
                }
                std::function<void(rx_result&&)> callback = [this, trans_id = phead->trans](rx_result&& result) mutable
                    {
                        wp_result_response response;
                        response.action_result = std::move(result);
                        send_message(std::move(response), trans_id);
                    };
                api::dotnetWriteValue(msg.type
                    , phead->trans, (void*)msg.inst, msg.idx, msg.val.move()
                    , std::move(callback));
                return true;
            }
            break;
        case dotnet_fire_event_message::msg_id:
            {
                dotnet_fire_event_message msg = {  };
                auto result = msg.deserialize(buffer);
                if (!result)
                {
                    return rx_create_string("Error deserializing fire event message: ", result.errors_line().c_str());
                }
				dotnet_hosting::instance().received_fire_event((void*)msg.inst, std::move(msg.value), std::move(msg.queue), msg.state, msg.remove);
                return true;
				}
            break;
        default:
            {
                if(phead->trans && RX_IS_DOTNET_RESPONSE(phead->type))
                {
                    auto it = pending_out_transactions_.find(phead->trans);
                    if (it != pending_out_transactions_.end())
                    {
                        it->second->response(phead->type, buffer);
                        pending_out_transactions_.erase(it);
                        return true;
                    }
                    return rx_create_string("Unknown .NET process transaction id response: ", phead->trans);
                }
				string_type err_str = rx_create_string("Unknown message type received from .NET process: ", phead->type);
                return err_str;
            }
    }
}

void dotnet_wp_connection::pipe_broken (string_type msg)
{
    for (auto& trans : pending_out_transactions_)
    {
		trans.second->response_error(rx_result(msg));
    }
}

rx_result dotnet_wp_connection::start ()
{
    dotnet_start_message msg;
    send_message(msg, 0);
    return true;
}

void dotnet_wp_connection::deinitialize (rx_void_callback callback, uint32_t timeout)
{
    auto trans = std::make_shared<stop_wp_transaction>(std::move(callback), timeout);
    auto result = trans->start(*this);
    if (!result)
    {
        callback();
        return;
    }
    pending_out_transactions_.emplace(trans->get_transaction_id(), trans);
}

void dotnet_wp_connection::stop ()
{
	sender_.end();
	rx_destry_server_side_pipe(&pipes_);
	pipe_server_.wait_handle();
}


// Class rx_internal::dotnet::pipe_receiver

pipe_receiver::pipe_receiver()
      : my_connection_(nullptr),
        pipes_(nullptr),
        receive_buffer_(0x10000, 0)
	, rx_platform::threads::thread("dotnet-receiver", 0)
    , buffer_(RX_DOTNET_PIPE_BUFFER_SIZE)
{
}


pipe_receiver::~pipe_receiver()
{
}



void pipe_receiver::start_receive (pipe_server_t* pipes)
{
	pipes_ = pipes;

    auto proto_result = rx_init_packet_decoder(&packet_decoder_, &receive_buffer_, pipe_header_function, sizeof(dotnet_wp_message_header_t), &header_buffer_);

    if (proto_result != RX_PROTOCOL_OK)
    {
        //return rx_create_string("Error initializing packet decoder: ", rx_protocol_error_message(proto_result));
    }
    start();
}

uint32_t pipe_receiver::handler ()
{
	string_type error_msg = "Error in pipe receiver: ";
    while (true)
    {
        bool error = false;
        rx_const_packet_buffer buffer{};
        size_t size = buffer_.get_size();
        rx_result result;
        auto ret = rx_read_pipe_server(pipes_, buffer_.buffer(), &size);
        if (ret == RX_OK)
        {

            /*std::ostringstream ss;
            char buff[0x4];
            ss << "\r\nRead from pipe:";
            for (size_t i = 0; i < size && i < 0x100; i++)
            {
                if (i % 16 == 0)
                    ss << "\r\n";
                sprintf(buff, "%02X ", ((uint8_t*)buffer_.buffer())[i]);
                ss << buff;
            }
            ss << "\r\n";
            printf("%s", ss.str().c_str());*/

            rx_init_const_packet_buffer(&buffer, buffer_.buffer(), size);
        }
        else
        {
            result = rx_result::create_from_last_os_error("Error reading pipe");
        }
        if (!result)
        {
            DOTNET_LOG_ERROR("dotnet_wp_connection", 100, rx_create_string("Error reading from pipe", result.errors_line()).c_str());

            break;
        }
        rx_const_packet_buffer recv_buffer{};
        rx_init_const_packet_buffer(&recv_buffer, NULL, 0);
        while (!rx_buffer_eof(&buffer))
        {
            struct rx_const_packet_buffer_type recv_buffer {};
            ret = rx_decode_arrived(&packet_decoder_, &buffer, &recv_buffer);
            if (ret != RX_PROTOCOL_OK)
            {
                error_msg += ("Error decoding arrived packet:"s + std::string(rx_protocol_error_message(ret)));
                error = true;
                break;
            }
            if (!rx_buffer_eof(&recv_buffer))
            {// we received something
                io::rx_const_io_buffer buffer(&recv_buffer);
                auto result = my_connection_->received(buffer);
                if (!result)
                {
                    error_msg += ("Error processing received packet:"s + result.errors_line());
                    error = true;
                    break;
                }
                rx_init_const_packet_buffer(&recv_buffer, NULL, 0);
            }
        }
        if (error)
        {
            break;
        }

    }
	my_connection_->pipe_broken(std::move(error_msg));
    return 0;
}

rx_result pipe_receiver::write_pipe (const rx_packet_buffer* buffer)
{
    if (buffer == nullptr)
    {
        rx_write_pipe_server(pipes_, NULL, 0);
        return true;
    }
    /*std::ostringstream ss;
    char buff[0x4];
    ss << "\r\nWriting to pipe:";
    for(size_t i = 0; i < buffer->size && i < 0x100; i++)
    {
        if (i % 16 == 0)
            ss << "\r\n";
        sprintf(buff, "%02X ", ((uint8_t*)buffer->buffer_ptr)[i]);
        ss << buff;
	}
	ss << "\r\n";
	printf("%s", ss.str().c_str());*/
    auto ret = rx_write_pipe_server(pipes_, buffer->buffer_ptr, buffer->size);
    if (ret == RX_OK)
    {
        return true;
    }
    else
    {
        return rx_result::create_from_last_os_error("Error writing pipe");
    }
}


void pipe_receiver::set_my_connection (dotnet_wp_connection * value)
{
  my_connection_ = value;
}


// Class rx_internal::dotnet::dotnet_wp_transaction

dotnet_wp_transaction::dotnet_wp_transaction()
{
}


dotnet_wp_transaction::~dotnet_wp_transaction()
{
}



// Class rx_internal::dotnet::build_types_wp_transaction

build_types_wp_transaction::build_types_wp_transaction (const string_type& lib, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        timeout_(timeout),
        trans_done_(false),
        library_(lib)
{
}



rx_result build_types_wp_transaction::start (dotnet_wp_connection& connection) const
{
	build_types_message msg;
	msg.library = library_;
    connection.send_message(std::move(msg), trans_id_);
	return true;
}

rx_result build_types_wp_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
    if(msg_type != build_types_response::msg_id)
    {
        return rx_create_string("Invalid message type for build types response: ", msg_type);
	}
    build_types_response msg;
	auto result = msg.deserialize(buffer);
    if (!result)
    {
        result = rx_create_string("Error deserializing build types response: ", result.errors_line().c_str());
    }
	result_ = std::move(msg.action_result);
    trans_done_.set();
    return true;
}

void build_types_wp_transaction::response_error (rx_result result)
{
    result_ = std::move(result);
    trans_done_.set();
}

void build_types_wp_transaction::timeouted ()
{
	result_ = rx_result("Build types transaction timed out");
    trans_done_.set();
}

uint32_t build_types_wp_transaction::get_transaction_id () const
{
	return trans_id_;
}

rx_result build_types_wp_transaction::wait_result ()
{
	auto ret = trans_done_.wait_handle(timeout_ + 100);
    if (ret == RX_WAIT_TIMEOUT)
    {
		result_ = rx_result("Build types transaction timed out");
    }
	return std::move(result_);
}


// Class rx_internal::dotnet::execute_method_wp_transaction

execute_method_wp_transaction::execute_method_wp_transaction (uint32_t trans_id, void* whose, const string_type& method, const string_type& value)
      : trans_id_(trans_id),
        whose_(whose),
        method_(method),
        value_(value)
{
}



rx_result execute_method_wp_transaction::start (dotnet_wp_connection& connection) const
{
    dotnet_execute_method_message msg;
	msg.inst = (intptr_t)whose_;
	msg.method = method_;
	msg.value = value_;
    connection.send_message(std::move(msg), trans_id_);
    return true;
}

rx_result execute_method_wp_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
    if (msg_type != dotnet_execute_method_response::msg_id)
    {
        return rx_create_string("Invalid message type for execute method response: ", msg_type);
    }
    dotnet_execute_method_response msg;
    auto result = msg.deserialize(buffer);
    if (!result)
    {
        result = rx_create_string("Error deserializing execute method response: ", result.errors_line().c_str());
    }
	dotnet_hosting::instance().received_execute_done(trans_id_, (void*)whose_,std::move(msg.result_value), std::move(msg.action_result));
    return true;
}

void execute_method_wp_transaction::response_error (rx_result result)
{
    dotnet_hosting::instance().received_execute_done(trans_id_, (void*)whose_, "", std::move(result));
}

void execute_method_wp_transaction::timeouted ()
{
    dotnet_hosting::instance().received_execute_done(trans_id_, (void*)whose_, "", rx_result("Timeout occured"));
}

uint32_t execute_method_wp_transaction::get_transaction_id () const
{
    return trans_id_;
}


// Class rx_internal::dotnet::reload_wp_transaction

reload_wp_transaction::reload_wp_transaction (const string_type& lib, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        timeout_(timeout),
        trans_done_(false),
        library_(lib)
{
}



rx_result reload_wp_transaction::start (dotnet_wp_connection& connection) const
{
    reload_message msg;
    msg.library = library_;
    connection.send_message(std::move(msg), trans_id_);
    return true;
}

rx_result reload_wp_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
    if (msg_type != wp_result_response::msg_id)
    {
        return rx_create_string("Invalid message type for build types response: ", msg_type);
    }
    wp_result_response msg;
    auto result = msg.deserialize(buffer);
    if (!result)
    {
        result = rx_create_string("Error deserializing build types response: ", result.errors_line().c_str());
    }
    result_ = std::move(msg.action_result);
    trans_done_.set();
    return true;
}

void reload_wp_transaction::response_error (rx_result result)
{
    result_ = std::move(result);
    trans_done_.set();
}

void reload_wp_transaction::timeouted ()
{
    result_ = rx_result("Build types transaction timed out");
    trans_done_.set();
}

uint32_t reload_wp_transaction::get_transaction_id () const
{
    return trans_id_;
}

rx_result reload_wp_transaction::wait_result ()
{
    auto ret = trans_done_.wait_handle(timeout_ + 100);
    if (ret == RX_WAIT_TIMEOUT)
    {
        result_ = rx_result("Reload transaction timed out");
    }
    return std::move(result_);
}


// Class rx_internal::dotnet::unload_wp_transaction

unload_wp_transaction::unload_wp_transaction (const string_type& lib, rx_result_callback callback, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        timeout_(timeout),
        library_(lib),
        callback_(std::move(callback))
{
}



rx_result unload_wp_transaction::start (dotnet_wp_connection& connection) const
{
    unload_message msg;
    msg.library = library_;
    connection.send_message(std::move(msg), trans_id_);
    return true;
}

rx_result unload_wp_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
    if (msg_type != wp_result_response::msg_id)
    {
        return rx_create_string("Invalid message type for unload response: ", msg_type);
    }
    wp_result_response msg;
    auto result = msg.deserialize(buffer);
    if (!result)
    {
        return rx_create_string("Error deserializing unload response: ", result.errors_line().c_str());
    }
    result_ = std::move(msg.action_result);
	callback_(std::move(result_));
    return true;
}

void unload_wp_transaction::response_error (rx_result result)
{
    result_ = std::move(result);
    callback_(std::move(result_));
}

void unload_wp_transaction::timeouted ()
{
    result_ = "Timeout occurred!";
    callback_(std::move(result_));
}

uint32_t unload_wp_transaction::get_transaction_id () const
{
    return trans_id_;
}


// Class rx_internal::dotnet::stop_wp_transaction

stop_wp_transaction::stop_wp_transaction (rx_void_callback callback, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        timeout_(timeout),
        callback_(std::move(callback))
{
}



rx_result stop_wp_transaction::start (dotnet_wp_connection& connection) const
{
    dotnet_stop_message msg;
    connection.send_message(std::move(msg), trans_id_);
    return true;
}

rx_result stop_wp_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
    if (msg_type != wp_void_response::msg_id)
    {
        return rx_create_string("Invalid message type for unload response: ", msg_type);
    }
    wp_void_response msg;
    auto result = msg.deserialize(buffer);
    if (!result)
    {
        return rx_create_string("Error deserializing unload response: ", result.errors_line().c_str());
    }
    callback_();
    return true;
}

void stop_wp_transaction::response_error (rx_result result)
{
    callback_();
}

void stop_wp_transaction::timeouted ()
{
    callback_();
}

uint32_t stop_wp_transaction::get_transaction_id () const
{
	return trans_id_;
}


} // namespace dotnet
} // namespace rx_internal

