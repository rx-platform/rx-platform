

/****************************************************************************
*
*  interfaces\rx_file_ports.cpp
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

#define DMON_IMPL

#define DMON_ASSERT(e)   RX_ASSERT(e)

#include "third-party/dmon/dmon.h"

// rx_file_ports
#include "interfaces/rx_file_ports.h"

#include "rx_endpoints.h"
#include "system/server/rx_file_helpers.h"


namespace rx_internal {

namespace interfaces {

namespace file_endpoints {
namespace {

void watch_callback_internal(dmon_watch_id watch_id, dmon_action action, const char* rootdir,
    const char* filepath, const char* oldfilepath, void* user)
{
    file_endpoint* self = (file_endpoint*)user;
    self->watch_callback(watch_id.id, action, rootdir, filepath, oldfilepath);
}


}

// Class rx_internal::interfaces::file_endpoints::file_endpoint 

file_endpoint::file_endpoint()
      : my_port_(nullptr),
        watch_id_(0)
{
    ITF_LOG_DEBUG("ethernet_endpoint", 200, "Ethernet endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);

    mine_entry->send_function = &file_endpoint::send_function;
}


file_endpoint::~file_endpoint()
{
}



rx_protocol_result_t file_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_stack_endpoint* file_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* file_endpoint::get_port ()
{
    return my_port_;
}

rx_protocol_result_t file_endpoint::send_packet (send_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_result file_endpoint::open (const string_type& directory, const string_type& filter, security::security_context_ptr identity, file_port* port)
{
    if (watch_id_)
        return RX_ALREADY_CONNECTED;

    auto result = dmon_watch(directory.c_str(), watch_callback_internal, DMON_WATCHFLAGS_RECURSIVE, this);
    if (result.id == 0)
    {
        char buff[0x100];
        rx_last_os_error("Error connecting directory.", buff, sizeof(buff));
        return buff;
    }
    else
    {
        watch_id_ = result.id;
        my_port_ = port;
        return true;
    }
}

rx_result file_endpoint::close ()
{
    if (watch_id_ == 0)
        return RX_NOT_CONNECTED;

    dmon_unwatch({ watch_id_ });
    my_port_ = nullptr;
    watch_id_ = { 0 };
    return true;
}

bool file_endpoint::is_connected () const
{
    return watch_id_ != 0;
}

void file_endpoint::watch_callback (uint32_t watch_id, int action, const char* rootdir, const char* filepath, const char* oldfilepath)
{
    if (watch_id == watch_id_)
    {
        string_type path = rx_combine_paths(rootdir, filepath);

        if (file_filter_.empty() || rx_match_pattern(path.c_str(), file_filter_.c_str(), 0))
        {
            switch ((dmon_action_t)action)
            {
                case DMON_ACTION_MODIFY:
                case DMON_ACTION_CREATE:
                    {// added file to directory
                        byte_string data;
                        rx_source_file file;
                        if (file.open(path.c_str()))
                        {
                            if (file.read_data(data))
                            {
                                if (!data.empty() && my_port_)
                                {
                                    my_port_->send_io_function([port = my_port_](file_endpoint* ep, string_type name, byte_string data)
                                        {
                                            port->received_data(ep, std::move(name), std::move(data));

                                        }, this, string_type(filepath), std::move(data));
                                }
                            }
                            else
                            {
                                RX_ASSERT(false);
                            }
                        }
                        else
                        {
                            RX_ASSERT(false);
                        }
                    }
                    break;
                case DMON_ACTION_DELETE:
                    {// removed file from directory
                    }
                    break;
                case DMON_ACTION_MOVE:
                    {// renamed file in directory
                        printf("Renamed file from %s\r\n", oldfilepath);

                    }
                    break;
            }
        }

    }
    else
    {
        RX_ASSERT(false);
    }
    
}

void file_endpoint::received_data (string_type name, byte_string data)
{
    rx_const_packet_buffer buff{};
    rx_init_const_packet_buffer(&buff, &data[0], data.size());
    rx::io::string_address addr_from(name);
    auto recv_packet = rx_create_recv_packet(0, &buff, 0, 0);
    recv_packet.from_addr = &addr_from;
    auto result = rx_move_packet_up(&stack_endpoint_, recv_packet);
    if (result != RX_PROTOCOL_OK)
    {
        ITF_LOG_ERROR("file_endpoint", 100, "Error moving packet up:"s + rx_protocol_error_message(result));
    }
}


dmon_initializer::dmon_initializer()
{

    dmon_init();
}
dmon_initializer::~dmon_initializer()
{

    dmon_deinit();
}

// Class rx_internal::interfaces::file_endpoints::file_port 

file_port::file_port()
{
    static dmon_initializer initializer;
}


file_port::~file_port()
{
}



rx_result file_port::initialize_runtime (runtime::runtime_init_context& ctx)
{

    auto result = status.initialize(ctx);
    if (!result)
        return result;


    dir_path_ = ctx.get_item_static("Options.DirectoryPath", ""s);
    file_filter_.bind("Options.DirectoryPath", ctx, [](string_type val)
        {

        });

    return true;
}

rx_result file_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    endpoint_ = std::make_unique<file_endpoint>();
    auto sec_ctx = get_security_context();
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            file_endpoint* whose = reinterpret_cast<file_endpoint*>(entry->user_data);
            auto port = whose->get_port();
            if (port)
                port->disconnect_stack_endpoint(entry);
        };
    auto result = endpoint_->open(dir_path_, file_filter_, sec_ctx, this);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    result = add_stack_endpoint(endpoint_->get_stack_endpoint(), local_address, remote_address);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    return true;
}

rx_result_with<port_connect_result> file_port::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    endpoint_ = std::make_unique<file_endpoint>();
    auto sec_ctx = get_security_context();
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            file_endpoint* whose = reinterpret_cast<file_endpoint*>(entry->user_data);
            whose->get_port()->disconnect_stack_endpoint(entry);
        };
    auto result = endpoint_->open(dir_path_, file_filter_, sec_ctx, this);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    return port_connect_result(endpoint_->get_stack_endpoint(), endpoint_->is_connected());
}

rx_result file_port::stop_passive ()
{
    if (endpoint_)
        endpoint_->close();
    return true;
}

void file_port::received_data (file_endpoint* ep, string_type name, byte_string data)
{
    if (endpoint_.get() == ep)
    {
        endpoint_->received_data(std::move(name), std::move(data));
    }
}


} // namespace file_endpoints
} // namespace interfaces
} // namespace rx_internal

