

/****************************************************************************
*
*  system\runtime\rx_port_helpers.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

// Class rx_platform::runtime::io_types::master_port_status 

master_port_status::master_port_status()
      : online_(false),
        read_time_(0),
        write_time_(0),
        max_read_time_(0),
        max_write_time_(0),
        read_count_(0),
        write_count_(0),
        failed_read_count_(0),
        failed_write_count_(0),
        queue_size_(0),
        received_(0),
        sent_(0)
{
}



rx_result master_port_status::initialize (runtime_init_context& ctx)
{
    rx_result result;

    auto one_result = received_.bind("Status.RxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.RxPackets:"s + one_result.errors_line());
    one_result = sent_.bind("Status.TxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.TxPackets:"s + one_result.errors_line());
    one_result = online_.bind("Status.Online", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.Online:"s + one_result.errors_line());
    one_result = read_count_.bind("Status.Reads", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.Reads:"s + one_result.errors_line());
    one_result = write_count_.bind("Status.Writes", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.Writes:"s + one_result.errors_line());
    one_result = failed_read_count_.bind("Status.FailedReads", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.FailedReads:"s + one_result.errors_line());
    one_result = failed_write_count_.bind("Status.FailedWrites", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.FailedWrites:"s + one_result.errors_line());

    one_result = read_time_.bind("Status.LastRead", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.LastRead:"s + one_result.errors_line());
    one_result = max_read_time_.bind("Status.MaxRead", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.MaxRead:"s + one_result.errors_line());
    one_result = write_time_.bind("Status.LastWrite", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.LastWrite:"s + one_result.errors_line());
    one_result = max_write_time_.bind("Status.MaxWrite", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.MaxWrite:"s + one_result.errors_line());


    return result;
}

void master_port_status::set_online ()
{
    online_ = true;
}

void master_port_status::set_offline ()
{
    online_ = false;
}

void master_port_status::write_done (uint64_t time, bool success)
{
    float this_time = (float)time / 1000.0f;
    write_time_ = this_time;
    if (max_write_time_ < this_time)
        max_write_time_ = this_time;
    if (!success)
    {
        failed_write_count_ += 1;
        online_ = false;
    }
    else
    {
        online_ = true;
    }
    write_count_ += 1;
}

void master_port_status::read_done (uint64_t time, bool success)
{
    float this_time = (float)time / 1000.0f;
    read_time_ = this_time;
    if (max_read_time_ < this_time)
        max_read_time_ = this_time;
    if (!success)
    {
        failed_read_count_ += 1;
        online_ = false;
    }
    else
    {
        online_ = true;
    }
    read_count_ += 1;
}

void master_port_status::received_packet ()
{
    received_ += 1;
}

void master_port_status::sent_packet ()
{
    sent_ += 1;
}


// Class rx_platform::runtime::io_types::simple_port_status 

simple_port_status::simple_port_status()
      : received_(0),
        sent_(0)
{
}



rx_result simple_port_status::initialize (runtime_init_context& ctx)
{
    rx_result result;

    auto one_result = received_.bind("Status.RxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.RxPackets:"s + one_result.errors_line());
    one_result = sent_.bind("Status.TxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.TxPackets:"s + one_result.errors_line());

    return result;
}

void simple_port_status::received_packet ()
{
    received_ += 1;
}

void simple_port_status::sent_packet ()
{
    sent_ += 1;
}


// Class rx_platform::runtime::io_types::external_port_status 

external_port_status::external_port_status()
      : received_bytes_(0),
        sent_bytes_(0),
        received_(0),
        sent_(0)
{
}



void external_port_status::received_packet (size_t bytes)
{
    received_bytes_ += bytes;
    received_ += 1;
}

void external_port_status::sent_packet (size_t bytes)
{
    sent_bytes_ += bytes;
    sent_ += 1;
}

rx_result external_port_status::initialize (runtime_init_context& ctx)
{
    rx_result result;

    auto one_result = received_.bind("Status.RxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.RxPackets:"s + one_result.errors_line());
    one_result = sent_.bind("Status.TxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.TxPackets:"s + one_result.errors_line()); 
    one_result = received_bytes_.bind("Status.RxBytes", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.RxBytes:"s + one_result.errors_line());
    one_result = sent_bytes_.bind("Status.TxBytes", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.TxBytes:"s + one_result.errors_line());

    return result;
}


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

