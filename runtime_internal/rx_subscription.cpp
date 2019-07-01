

/****************************************************************************
*
*  runtime_internal\rx_subscription.cpp
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


// rx_subscription
#include "runtime_internal/rx_subscription.h"

#include "rx_runtime_internal.h"


namespace sys_runtime {

namespace subscriptions {

// Class sys_runtime::subscriptions::rx_subscription 

rx_subscription::rx_subscription (rx_subscription_callback* callback)
      : callback_(callback)
{
}



rx_result rx_subscription::connect_items (const std::vector<std::pair<string_type, runtime_handle_t> >& paths, std::vector<rx_result_with<runtime_handle_t> >& results)
{
	for (const auto& one : paths)
	{
		auto it_inv = inverse_tags_.find(one.first);
		if (it_inv == inverse_tags_.end())
		{
			auto new_id = platform_runtime_manager::get_new_handle();
			rx_subscription_tag tag;
			tag.path = one.first;
			tag.ref_count = 1;
			tag.sampling_rate = 0;
			tag.target_handle = 0;
			tags_.emplace(new_id, std::move(tag));
			inverse_tags_.emplace(one.first, new_id);

			results.emplace_back(new_id);
		}
		else
		{
			auto it_tags = tags_.find(it_inv->second);
			if (it_tags != tags_.end())
			{
				it_tags->second.ref_count++;
			}
		}
	}
	return true;
}

rx_result rx_subscription::disconnect_items (const std::vector<runtime_handle_t>& items, std::vector<rx_result>& results)
{
	for (auto one : items)
	{
		auto it_tags = tags_.find(one);
		if (it_tags != tags_.end())
		{
			it_tags->second.ref_count--;
			if (it_tags->second.ref_count < 1)
			{// remove it
				inverse_tags_.erase(it_tags->second.path);
			}
			results.emplace_back(true);
		}
		else
		{
			results.emplace_back("Invalid handle value!");
		}
	}
	return true;
}


// Class sys_runtime::subscriptions::rx_subscription_tag 


// Class sys_runtime::subscriptions::rx_subscription_callback 


} // namespace subscriptions
} // namespace sys_runtime

