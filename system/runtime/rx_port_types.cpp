

/****************************************************************************
*
*  system\runtime\rx_port_types.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_port_types
#include "system/runtime/rx_port_types.h"

#include "sys_internal/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "rx_runtime_holder.h"


namespace rx_platform {

namespace runtime {

namespace io_types {

// Class rx_platform::runtime::io_types::physical_port 

physical_port::physical_port()
      : my_endpoints_(nullptr),
        rx_bytes_item_(0),
        tx_bytes_item_(0),
        rx_packets_item_(0),
        tx_packets_item_(0),
        connected_item_(0)
{
}



rx_result physical_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = port_runtime::initialize_runtime(ctx);
    if (result)
    {
        auto bind_result = ctx.tags->bind_item("Status.RxPackets", ctx);
        if (bind_result)
            rx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxPackets", ctx);
        if (bind_result)
            tx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.TxBytes");

        bind_result = ctx.tags->bind_item("Status.Connected", ctx);
        if (bind_result)
            connected_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.Connected");

        bind_result = ctx.tags->bind_item("Status.RxBytes", ctx);
        if (bind_result)
            rx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxBytes", ctx);
        if (bind_result)
            tx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.TxBytes");
    }
    return result;
}

void physical_port::update_received_counters (size_t count)
{
    rx_platform::rx_post_function_to(get_executer(), smart_this(), [count](physical_port::smart_ptr whose)
        {
            if (whose->rx_bytes_item_)
            {
                auto current = whose->get_binded_as<int64_t>(whose->rx_bytes_item_, 0);
                current += count;
                whose->set_binded_as<int64_t>(whose->rx_bytes_item_, std::move(current));
            }
            whose->update_received_packets(1);
        }, smart_this());
}

void physical_port::update_sent_counters (size_t count)
{
    rx_platform::rx_post_function_to(get_executer(), smart_this(), [count](physical_port::smart_ptr whose)
        {
            if (whose->tx_bytes_item_)
            {
                auto current = whose->get_binded_as<int64_t>(whose->tx_bytes_item_, 0);
                current += count;
                whose->set_binded_as<int64_t>(whose->tx_bytes_item_, std::move(current));
            }
            whose->update_sent_packets(1);
        }, smart_this());
}

void physical_port::update_received_packets (size_t count)
{
    if (rx_packets_item_)
    {
        auto current = get_binded_as<int64_t>(rx_packets_item_, 0);
        current += count;
        set_binded_as<int64_t>(rx_packets_item_, std::move(current));
    }
}

void physical_port::update_sent_packets (size_t count)
{
    if (tx_packets_item_)
    {
        auto current = get_binded_as<int64_t>(tx_packets_item_, 0);
        current += count;
        set_binded_as<int64_t>(tx_packets_item_, std::move(current));
    }
}

void physical_port::update_connected_status (bool status)
{
    rx_platform::rx_post_function_to(get_executer(), smart_this(), [status](physical_port::smart_ptr whose)
        {
            whose->set_binded_as(whose->connected_item_, status);
        }, smart_this());
}

void physical_port::structure_changed ()
{
}


// Class rx_platform::runtime::io_types::protocol_port 

protocol_port::protocol_port()
      : rx_bytes_item_(0),
        tx_bytes_item_(0),
        rx_packets_item_(0),
        tx_packets_item_(0)
{
}



rx_result protocol_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = port_runtime::initialize_runtime(ctx);
    if (result)
    {
        auto bind_result = ctx.tags->bind_item("Status.RxPackets", ctx);
        if (bind_result)
            rx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxPackets", ctx);
        if (bind_result)
            tx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.TxBytes");

        bind_result = ctx.tags->bind_item("Status.RxBytes", ctx);
        if (bind_result)
            rx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxBytes", ctx);
        if (bind_result)
            tx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.TxBytes");
    }
    return result;
}

void protocol_port::update_received_counters (size_t count)
{
    rx_platform::rx_post_function_to(get_executer(), smart_this(), [count](protocol_port::smart_ptr whose)
        {
            if (whose->rx_bytes_item_)
            {
                auto current = whose->get_binded_as<int64_t>(whose->rx_bytes_item_, 0);
                current += count;
                whose->set_binded_as<int64_t>(whose->rx_bytes_item_, std::move(current));
            }
            whose->update_received_packets(1);
        }, smart_this());
}

void protocol_port::update_sent_counters (size_t count)
{
    rx_platform::rx_post_function_to(get_executer(), smart_this(), [count](protocol_port::smart_ptr whose)
        {
            if (whose->tx_bytes_item_)
            {
                auto current = whose->get_binded_as<int64_t>(whose->tx_bytes_item_, 0);
                current += count;
                whose->set_binded_as<int64_t>(whose->tx_bytes_item_, std::move(current));
            }
            whose->update_sent_packets(1);
        }, smart_this());
}

void protocol_port::update_received_packets (size_t count)
{
    if (rx_packets_item_)
    {
        auto current = get_binded_as<int64_t>(rx_packets_item_, 0);
        current += count;
        set_binded_as<int64_t>(rx_packets_item_, std::move(current));
    }
}

void protocol_port::update_sent_packets (size_t count)
{
    if (tx_packets_item_)
    {
        auto current = get_binded_as<int64_t>(tx_packets_item_, 0);
        current += count;
        set_binded_as<int64_t>(tx_packets_item_, std::move(current));
    }
}


// Class rx_platform::runtime::io_types::transport_port 

transport_port::transport_port()
      : rx_bytes_item_(0),
        tx_bytes_item_(0),
        rx_packets_item_(0),
        tx_packets_item_(0)
{
}



rx_result transport_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = port_runtime::initialize_runtime(ctx);
    if (result)
    {
        auto bind_result = ctx.tags->bind_item("Status.RxPackets", ctx);
        if (bind_result)
            rx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxPackets", ctx);
        if (bind_result)
            tx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.TxBytes");

        bind_result = ctx.tags->bind_item("Status.RxBytes", ctx);
        if (bind_result)
            rx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxBytes", ctx);
        if (bind_result)
            tx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(ctx.meta.name, 200, "Unable to bind to value Status.TxBytes");
    }
    return result;
}

void transport_port::update_received_counters (size_t count)
{
    rx_platform::rx_post_function_to(get_executer(), smart_this(), [count](transport_port::smart_ptr whose)
        {
            if (whose->rx_bytes_item_)
            {
                auto current = whose->get_binded_as<int64_t>(whose->rx_bytes_item_, 0);
                current += count;
                whose->set_binded_as<int64_t>(whose->rx_bytes_item_, std::move(current));
            }
            whose->update_received_packets(1);
        }, smart_this());
}

void transport_port::update_sent_counters (size_t count)
{
    rx_platform::rx_post_function_to(get_executer(), smart_this(), [count](transport_port::smart_ptr whose)
        {
            if (whose->tx_bytes_item_)
            {
                auto current = whose->get_binded_as<int64_t>(whose->tx_bytes_item_, 0);
                current += count;
                whose->set_binded_as<int64_t>(whose->tx_bytes_item_, std::move(current));
            }
            whose->update_sent_packets(1);
        }, smart_this());
}

void transport_port::update_received_packets (size_t count)
{
    if (rx_packets_item_)
    {
        auto current = get_binded_as<int64_t>(rx_packets_item_, 0);
        current += count;
        set_binded_as<int64_t>(rx_packets_item_, std::move(current));
    }
}

void transport_port::update_sent_packets (size_t count)
{
    if (tx_packets_item_)
    {
        auto current = get_binded_as<int64_t>(tx_packets_item_, 0);
        current += count;
        set_binded_as<int64_t>(tx_packets_item_, std::move(current));
    }
}


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

