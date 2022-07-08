

/****************************************************************************
*
*  runtime_internal\platform_source.cpp
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

#include "system/runtime/rx_runtime_helpers.h"
#include "system/runtime/rx_process_context.h"
#include "system/runtime/rx_rt_struct.h"
using namespace rx_platform::runtime;

// platform_source
#include "runtime_internal/platform_source.h"



namespace rx_internal {

namespace sys_runtime {

namespace data_source {

// Class rx_internal::sys_runtime::data_source::platform_source 

platform_source::platform_source()
      : path_("")
    , point_(this)
{
}


platform_source::~platform_source()
{
}



rx_result platform_source::initialize_source (runtime::runtime_init_context& ctx)
{
    point_.set_context(ctx.context);
    auto result = path_.bind(".Path", ctx, [this](const string_type& val)
        {
            connect(val);
        });
    
    return true;
}

rx_result platform_source::start_source (runtime::runtime_start_context& ctx)
{
    auto result = connect(path_);
        return result;
    return true;
}

rx_result platform_source::stop_source (runtime::runtime_stop_context& ctx)
{
    disconnect();
	return true;
}

rx_result platform_source::connect (const string_type& path)
{
    point_.connect(path, 200);
    return true;
}

void platform_source::disconnect ()
{
    point_.disconnect();
}

rx_result platform_source::source_write (write_data&& data, runtime_process_context* ctx)
{
    point_.write(data.value, data.transaction_id);
    return true;
}

void platform_source::value_changed (rx_value&& val)
{
    source_value_changed(std::move(val));
}

void platform_source::result_received (rx_result&& result, runtime_transaction_id_t id)
{
    source_result_received(std::move(result), id);
}


// Class rx_internal::sys_runtime::data_source::platform_source_point 

platform_source_point::platform_source_point (platform_source* my_source)
      : my_source_(my_source)
{
}



void platform_source_point::value_changed (const rx_value& val)
{
    my_source_->value_changed(rx_value(val));
}

void platform_source_point::result_received (rx_result&& result, runtime_transaction_id_t id)
{
    my_source_->result_received(std::move(result), id);
}


} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal

