

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
#include "system/server/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "sys_internal/rx_internal_ns.h"


namespace sys_runtime {

namespace subscriptions {

// Class sys_runtime::subscriptions::rx_subscription

rx_subscription::rx_subscription (rx_subscription_callback* callback)
      : callback_(callback),
        active_(false)
{
	target_ = rx_thread_context();
}



rx_result rx_subscription::connect_items (const string_array& paths, std::vector<rx_result_with<runtime_handle_t> >& result)
{

	// OutputDebugStringA("****************Something to connect\r\n");
	for (const auto& one : paths)
	{
		auto it_inv = inverse_tags_.find(one);
		if (it_inv == inverse_tags_.end())
		{
			auto new_id = platform_runtime_manager::get_new_handle();
			rx_subscription_tag tag;
			tag.path = one;
			tag.ref_count = 1;
			tag.sampling_rate = 0;
			tag.target_handle = 0;
			tags_.emplace(new_id, std::move(tag));
			inverse_tags_.emplace(one, new_id);

			string_type object_path;
			string_type item_path;
			rx_split_item_path(one, object_path, item_path);
			auto it = to_connect_.find(object_path);
			if (it != to_connect_.end())
			{
				it->second.items.emplace(new_id, item_path);
			}
			else
			{
				runtime_data temp;
				temp.items.emplace(new_id, connect_data(item_path));
				to_connect_.emplace(object_path, std::move(temp));
			}

			result.emplace_back(new_id);
		}
		else
		{
			auto it_tags = tags_.find(it_inv->second);
			if (it_tags != tags_.end())
			{
				it_tags->second.ref_count++;
				result.emplace_back(it_tags->first);
				if (it_tags->second.item_ptr)
				{
					api::rx_context ctx;
					ctx.object = smart_this();
					it_tags->second.item_ptr->read_items({ it_tags->second.target_handle }, smart_this(), ctx);
				}
			}
			else
			{
				RX_ASSERT(false);
				result.emplace_back("Internal error!!!");
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

rx_result rx_subscription::process_connections ()
{
	if (!active_)
		return false;

	//if (!to_connect_.empty())
	//{
	//	// query for object items from paths
	//	if (query_names_.empty())
	//	{
	//		for (auto& one : to_connect_)
	//		{
	//			if (!one.second.item.get_name().empty() && !one.second.querying && !one.second.items.empty())
	//			{
	//				query_names_.emplace_back(one.first);
	//				one.second.querying = true;
	//			}
	//		}
	//		if (!query_names_.empty())
	//		{

	//			// OutputDebugStringA("****************Something to query\r\n");
	//			api::rx_context ctx;
	//			ctx.object = smart_this();
	//			auto result = api::ns::rx_list_runtime_from_path(query_names_, ctx);

	//			size_t idx = 0;
	//			RX_ASSERT(result.size() == query_names_.size());
	//			for (const auto& one : result)
	//			{
	//				auto it = to_connect_.find(query_names_[idx]);
	//				if (it != to_connect_.end())
	//				{
	//					it->second.querying = false;
	//					if (one)
	//					{
	//						it->second.item = one.value();
	//					}
	//				}
	//				idx++;
	//			}
	//			query_names_.clear();
	//		}
	//		if (attempts_.empty())
	//		{
	//			// OutputDebugStringA("****************Something to connect attempts empty\r\n");
	//			for (auto& one : to_connect_)
	//			{
	//				if (one.second.item && !one.second.querying && !one.second.items.empty())
	//				{
	//					std::vector<runtime_handle_t> items;
	//					string_array paths;

	//					for (auto& item : one.second.items)
	//					{
	//						items.emplace_back(item.first);
	//						paths.emplace_back(item.second.local_path);
	//					}
	//					if (!items.empty())
	//					{

	//						// OutputDebugStringA("****************Something to connect calling\r\n");
	//						one.second.querying = true;
	//						attempts_.emplace(one.first, std::move(items));

	//						string_type object_path(one.first);
	//						api::rx_context ctx;
	//						ctx.object = smart_this();
	//						auto& item_ptr = one.second.item;
	//						item_ptr->connect_items(paths, [this, object_path] (std::vector<rx_result_with<runtime_handle_t> > result)
	//							{
	//								auto attempts_it = attempts_.find(object_path);
	//								if (attempts_it != attempts_.end())
	//								{
	//									auto to_connect_it = to_connect_.find(object_path);
	//									if (to_connect_it != to_connect_.end())
	//									{
	//										to_connect_it->second.querying = false;
	//										size_t result_size = result.size();
	//										if (attempts_it->second.size() == result_size)
	//										{
	//											for (size_t idx = 0; idx < result_size; idx++)
	//											{
	//												auto& one_result = result[idx];
	//												if (one_result)
	//												{
	//													runtime_handle_t local_handle = attempts_it->second[idx];
	//													auto temp_it = to_connect_it->second.items.find(local_handle);
	//													if (temp_it != to_connect_it->second.items.end())
	//													{
	//														runtime_handle_t remote_handle = one_result.value();
	//														to_connect_it->second.items.erase(temp_it);
	//														auto tags_it = tags_.find(local_handle);
	//														if (tags_it != tags_.end())
	//														{
	//															tags_it->second.target_handle = remote_handle;
	//															tags_it->second.item_ptr = item_ptr;
	//															handles_[remote_handle] = local_handle;
	//														}
	//													}
	//													else
	//														RX_ASSERT(false);
	//												}
	//											}
	//										}
	//										else
	//											RX_ASSERT(false);
	//										if (to_connect_it->second.items.empty())
	//										{
	//											to_connect_.erase(to_connect_it);
	//										}
	//									}
	//									attempts_.erase(attempts_it);
	//								}
	//								else
	//									RX_ASSERT(false);
	//							}
	//							, smart_this(), ctx);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	return true;
}

void rx_subscription::activate ()
{
	if (!active_)
	{
		active_ = true;
		std::function<bool(int)> func = [this] (int) -> bool { return process_connections();  };
		rx_create_periodic_function<smart_ptr, int>(100u, func, smart_this(), 5);
	}
}

void rx_subscription::deactivate ()
{
	if (active_)
	{
		active_ = false;
	}
}

void rx_subscription::items_changed (const std::vector<update_item>& items)
{
	if (!items.empty())
	{
		std::vector<update_item> next;
		next.reserve(items.size());
		for (const auto& one : items)
		{
			auto it = handles_.find(one.handle);
			if (it != handles_.end())
				next.emplace_back(update_item{ it->second, one.value });
		}
		callback_->items_changed(next);
	}
}

void rx_subscription::transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items)
{
}

rx_thread_handle_t rx_subscription::get_target ()
{
	return target_;
}


// Class sys_runtime::subscriptions::rx_subscription_tag


// Class sys_runtime::subscriptions::rx_subscription_callback

rx_subscription_callback::~rx_subscription_callback()
{
}



} // namespace subscriptions
} // namespace sys_runtime

