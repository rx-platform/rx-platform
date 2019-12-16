

/****************************************************************************
*
*  system\runtime\rx_port_types.cpp
*
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

#include "system/server/rx_async_functions.h"


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
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxPackets", ctx);
        if (bind_result)
            tx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");

        bind_result = ctx.tags->bind_item("Status.Connected", ctx);
        if (bind_result)
            connected_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.Connected");

        bind_result = ctx.tags->bind_item("Status.RxBytes", ctx);
        if (bind_result)
            rx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxBytes", ctx);
        if (bind_result)
            tx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");
    }
    return result;
}

void physical_port::update_received_counters (size_t count)
{
    rx_platform::rx_post_function<physical_port::smart_ptr>([count](physical_port::smart_ptr whose)
        {
            if (whose->rx_bytes_item_)
            {
                auto current = whose->get_runtime().get_binded_as<int64_t>(whose->rx_bytes_item_, 0);
                current += count;
                whose->get_runtime().set_binded_as<int64_t>(whose->rx_bytes_item_, std::move(current));
            }
            whose->update_received_packets(1);
        }, smart_this(), get_executer());
}

void physical_port::update_sent_counters (size_t count)
{
    rx_platform::rx_post_function<physical_port::smart_ptr>([count](physical_port::smart_ptr whose)
        {
            if (whose->tx_bytes_item_)
            {
                auto current = whose->get_runtime().get_binded_as<int64_t>(whose->tx_bytes_item_, 0);
                current += count;
                whose->get_runtime().set_binded_as<int64_t>(whose->tx_bytes_item_, std::move(current));
            }
            whose->update_sent_packets(1);
        }, smart_this(), get_executer());
}

void physical_port::update_received_packets (size_t count)
{
    if (rx_packets_item_)
    {
        auto current = get_runtime().get_binded_as<int64_t>(rx_packets_item_, 0);
        current += count;
        get_runtime().set_binded_as<int64_t>(rx_packets_item_, std::move(current));
    }
}

void physical_port::update_sent_packets (size_t count)
{
    if (tx_packets_item_)
    {
        auto current = get_runtime().get_binded_as<int64_t>(tx_packets_item_, 0);
        current += count;
        get_runtime().set_binded_as<int64_t>(tx_packets_item_, std::move(current));
    }
}

void physical_port::update_connected_status (bool status)
{
    rx_platform::rx_post_function<physical_port::smart_ptr>([status](physical_port::smart_ptr whose)
        {
            whose->get_runtime().set_binded_as(whose->connected_item_, status);
        }, smart_this(), get_executer());
}

bool physical_port::has_up_port () const
{
  return true;

}

bool physical_port::has_down_port () const
{
  return false;

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
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxPackets", ctx);
        if (bind_result)
            tx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");

        bind_result = ctx.tags->bind_item("Status.RxBytes", ctx);
        if (bind_result)
            rx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxBytes", ctx);
        if (bind_result)
            tx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");
    }
    return result;
}

void protocol_port::update_received_counters (size_t count)
{
    rx_platform::rx_post_function<protocol_port::smart_ptr>([count](protocol_port::smart_ptr whose)
        {
            if (whose->rx_bytes_item_)
            {
                auto current = whose->get_runtime().get_binded_as<int64_t>(whose->rx_bytes_item_, 0);
                current += count;
                whose->get_runtime().set_binded_as<int64_t>(whose->rx_bytes_item_, std::move(current));
            }
            whose->update_received_packets(1);
        }, smart_this(), get_executer());
}

void protocol_port::update_sent_counters (size_t count)
{
    rx_platform::rx_post_function<protocol_port::smart_ptr>([count](protocol_port::smart_ptr whose)
        {
            if (whose->tx_bytes_item_)
            {
                auto current = whose->get_runtime().get_binded_as<int64_t>(whose->tx_bytes_item_, 0);
                current += count;
                whose->get_runtime().set_binded_as<int64_t>(whose->tx_bytes_item_, std::move(current));
            }
            whose->update_sent_packets(1);
        }, smart_this(), get_executer());
}

void protocol_port::update_received_packets (size_t count)
{
    if (rx_packets_item_)
    {
        auto current = get_runtime().get_binded_as<int64_t>(rx_packets_item_, 0);
        current += count;
        get_runtime().set_binded_as<int64_t>(rx_packets_item_, std::move(current));
    }
}

void protocol_port::update_sent_packets (size_t count)
{
    if (tx_packets_item_)
    {
        auto current = get_runtime().get_binded_as<int64_t>(tx_packets_item_, 0);
        current += count;
        get_runtime().set_binded_as<int64_t>(tx_packets_item_, std::move(current));
    }
}

bool protocol_port::has_up_port () const
{
  return true;

}

bool protocol_port::has_down_port () const
{
  return true;

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
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxPackets", ctx);
        if (bind_result)
            tx_packets_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");

        bind_result = ctx.tags->bind_item("Status.RxBytes", ctx);
        if (bind_result)
            rx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

        bind_result = ctx.tags->bind_item("Status.TxBytes", ctx);
        if (bind_result)
            tx_bytes_item_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");
    }
    return result;
}

void transport_port::update_received_counters (size_t count)
{
    rx_platform::rx_post_function<transport_port::smart_ptr>([count](transport_port::smart_ptr whose)
        {
            if (whose->rx_bytes_item_)
            {
                auto current = whose->get_runtime().get_binded_as<int64_t>(whose->rx_bytes_item_, 0);
                current += count;
                whose->get_runtime().set_binded_as<int64_t>(whose->rx_bytes_item_, std::move(current));
            }
            whose->update_received_packets(1);
        }, smart_this(), get_executer());
}

void transport_port::update_sent_counters (size_t count)
{
    rx_platform::rx_post_function<transport_port::smart_ptr>([count](transport_port::smart_ptr whose)
        {
            if (whose->tx_bytes_item_)
            {
                auto current = whose->get_runtime().get_binded_as<int64_t>(whose->tx_bytes_item_, 0);
                current += count;
                whose->get_runtime().set_binded_as<int64_t>(whose->tx_bytes_item_, std::move(current));
            }
            whose->update_sent_packets(1);
        }, smart_this(), get_executer());
}

void transport_port::update_received_packets (size_t count)
{
    if (rx_packets_item_)
    {
        auto current = get_runtime().get_binded_as<int64_t>(rx_packets_item_, 0);
        current += count;
        get_runtime().set_binded_as<int64_t>(rx_packets_item_, std::move(current));
    }
}

void transport_port::update_sent_packets (size_t count)
{
    if (tx_packets_item_)
    {
        auto current = get_runtime().get_binded_as<int64_t>(tx_packets_item_, 0);
        current += count;
        get_runtime().set_binded_as<int64_t>(tx_packets_item_, std::move(current));
    }
}

bool transport_port::has_up_port () const
{
  return true;

}

bool transport_port::has_down_port () const
{
  return true;

}


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

