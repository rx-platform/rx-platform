

/****************************************************************************
*
*  runtime_internal\rx_relations_runtime.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_subscription
#include "runtime_internal/rx_subscription.h"
// rx_relations_runtime
#include "runtime_internal/rx_relations_runtime.h"

#include "system/server/rx_platform_item.h"
using namespace rx_platform::runtime;


namespace rx_internal {

namespace sys_runtime {

namespace relations_runtime {

// Class rx_internal::sys_runtime::relations_runtime::local_relation_connector 

local_relation_connector::local_relation_connector (platform_item_ptr&& item)
      : item_ptr_(std::move(item))
{
}



rx_result local_relation_connector::read_tag (runtime_handle_t item, tags_callback_ptr monitor, runtime_process_context* ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result local_relation_connector::write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, runtime_process_context* ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result_with<runtime_handle_t> local_relation_connector::connect_tag (const string_type& path, tags_callback_ptr monitor, runtime_process_context* ctx)
{
    string_array paths{ path };
    auto result = item_ptr_->connect_items(paths, monitor);
    return std::move(result[0]);
}

rx_result local_relation_connector::disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result local_relation_connector::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
    auto result = item_ptr_->browse(prefix, path, filter, items);
    return result;
}


// Class rx_internal::sys_runtime::relations_runtime::remote_relation_connector 


rx_result remote_relation_connector::read_tag (runtime_handle_t item, tags_callback_ptr monitor, runtime_process_context* ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result remote_relation_connector::write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, runtime_process_context* ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result_with<runtime_handle_t> remote_relation_connector::connect_tag (const string_type& path, tags_callback_ptr monitor, runtime_process_context* ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result remote_relation_connector::disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result remote_relation_connector::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
    return RX_NOT_IMPLEMENTED;
}


} // namespace relations_runtime
} // namespace sys_runtime
} // namespace rx_internal

