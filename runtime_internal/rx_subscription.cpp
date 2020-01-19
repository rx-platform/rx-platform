

/****************************************************************************
*
*  runtime_internal\rx_subscription.cpp
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


// rx_subscription
#include "runtime_internal/rx_subscription.h"

#include "rx_runtime_internal.h"
#include "system/server/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "system/server/rx_platform_item.h"


namespace sys_runtime {

namespace subscriptions {

// Class sys_runtime::subscriptions::rx_subscription_tag 


// Class sys_runtime::subscriptions::rx_subscription_callback 

rx_subscription_callback::~rx_subscription_callback()
{
}



// Class sys_runtime::subscriptions::runtime_connection_data 


rx_subscription_tag* runtime_connection_data::get_tag (runtime_handle_t handle)
{
	size_t idx = (handle & 0xffff) - 1;
	if (idx < tags_.size() && !tags_[idx].path.empty())
	{
		return &tags_[idx];
	}
	else
	{
		return nullptr;
	}
}

bool runtime_connection_data::remove_tag (runtime_handle_t handle)
{
	// extract index from handle
	size_t idx = (handle & 0xffff) - 1;
	if (idx < tags_.size() && !tags_[idx].path.empty())
	{
		auto& tag = tags_[idx];
		tag.ref_count--;
		if (tag.ref_count < 1)
		{
			tags_[idx].path.clear();
			empty_slots_.emplace_back(idx);
		}
		return true;
	}
	else
	{
		return false;
	}
}

runtime_handle_t runtime_connection_data::add_tag (rx_subscription_tag&& tag, runtime_handle_t connection_handle)
{
	if (empty_slots_.empty())
	{// this is new one
		auto idx = tags_.size();
		runtime_handle_t ret = (((runtime_handle_t)idx + 1) | connection_handle);
		tag.mine_handle = ret;
		tags_.emplace_back(std::move(tag));
		to_connect_.emplace_back(idx);
		return ret;
	}
	else
	{
		auto idx = empty_slots_.back();
		runtime_handle_t ret = (((runtime_handle_t)idx + 1) | connection_handle);
		tag.mine_handle = ret;
		RX_ASSERT(tags_[idx].path.empty());
		tags_[idx] = std::move(tag);
		to_connect_.emplace_back(idx);
		return ret;
	}
}

bool runtime_connection_data::process_connection (const rx_time& ts, rx_subscription_ptr whose)
{
	if (!connecting && !connected && item)
	{
		std::vector<size_t> connect_indexes;
		string_array to_connect;
		size_t count = tags_.size();
		for (size_t idx=0; idx<count; idx++)
		{
			if (tags_[idx].target_handle == 0)
			{
				to_connect.emplace_back(tags_[idx].path);
				connect_indexes.emplace_back(idx);
			}
		}
		if (!to_connect.empty())
		{
			RX_ASSERT(to_connect.size() == connect_indexes.size());
			auto result = item->connect_items(to_connect, whose);
			RX_ASSERT(result.size() == to_connect.size());
			count = result.size();
			for (size_t idx = 0; idx < count; idx++)
			{
				if (result[idx])
				{
					tags_[connect_indexes[idx]].target_handle = result[idx].value();
					whose->handles_[result[idx].value()].emplace_back(tags_[connect_indexes[idx]].mine_handle);
				}
				else
				{
					std::ostringstream ss;
					ss << "Error connecting to "
						<< path
						<< RX_DIR_OBJECT_DELIMETER
						<< tags_[connect_indexes[idx]].path
						<< " :";
					for (const auto& one : result[idx].errors())
					{
						ss << one
							<< ", ";
					}
					RUNTIME_LOG_TRACE("runtime_connection_data", 10, ss.str());
				}
			}
		}
	}
	return false;
}


// Class sys_runtime::subscriptions::rx_subscription 

rx_subscription::rx_subscription (rx_subscription_callback* callback)
      : callback_(callback),
        active_(false),
        retrieve_items_(false),
        connect_items_(false)
{
	target_ = rx_thread_context();
}



rx_result rx_subscription::connect_items (const string_array& paths, std::vector<rx_result_with<runtime_handle_t> >& result)
{

	// OutputDebugStringA("****************Something to connect\r\n");
	for (const auto& one : paths)
	{
		///!!!!!WARNING THIS IS A MANUAL LOCK FUNCTION
		///!!!!!IT HAS BEEN DONE ON PURPOSE
		///!!!!!JUST BE CAREFUL
		items_lock_.lock();
		auto it_inv = inverse_tags_.find(one);
		if (it_inv == inverse_tags_.end())
		{
			runtime_handle_t new_id = 0;
			string_type object_path;
			string_type item_path;
			rx_split_item_path(one, object_path, item_path);
			auto it = connection_paths_.find(object_path);
			if (it != connection_paths_.end())
			{
				RX_ASSERT(it->second < connections_.size());
				// create runtime handle from connections size and connection tags size + 1 (to have zero as invalid)
				new_id = (runtime_handle_t)(it->second << 16);

				rx_subscription_tag tag;
				tag.path = item_path;
				tag.ref_count = 1;
				tag.sampling_rate = 0;
				tag.target_handle = 0;

				new_id = connections_[it->second].add_tag(std::move(tag), new_id);
			}
			else
			{
				size_t idx = connections_.size();
				// create runtime handle from connections size + 1 (to have zero as invalid)
				new_id = (runtime_handle_t)(idx << 16);

				runtime_connection_data temp;
				temp.path = object_path;
				temp.connected = false;
				temp.connecting = false;

				rx_subscription_tag tag;
				tag.path = item_path;
				tag.ref_count = 1;
				tag.sampling_rate = 0;
				tag.target_handle = 0;

				new_id = temp.add_tag(std::move(tag), new_id);

				connections_.emplace_back(std::move(temp));
				connection_paths_.emplace(object_path, idx);
				if (new_id)
					retrieve_items_ = true;
			}
			items_lock_.unlock();

			if (new_id)
			{
				connect_items_ = true;
				result.emplace_back(new_id);
			}
			else
			{
				result.emplace_back("Something really gone wrong here!!!");
			}
		}
		else
		{
			runtime_handle_t ret_handle = it_inv->second;
			auto tag = get_tag(it_inv->second);
			RX_ASSERT(tag != nullptr);
			auto connection = get_connection(ret_handle);
			RX_ASSERT(connection != nullptr);

			std::vector<runtime_handle_t> items;
			tag->ref_count++;
			result.emplace_back(ret_handle);
			if (connection->item && tag->target_handle)
			{
				std::vector<runtime_handle_t> items { tag->target_handle };
				api::rx_context ctx;
				ctx.object = smart_this();

				auto result = connection->item->read_items(items, smart_this(), ctx);
				if (!result)
				{
					return result;
				}
			}
			else
			{
			}
			items_lock_.unlock();

			result.emplace_back(ret_handle);
		}
	}
	return true;
}

rx_result rx_subscription::disconnect_items (const std::vector<runtime_handle_t>& items, std::vector<rx_result>& results)
{
	for (auto one : items)
	{
		locks::auto_lock_t<decltype(items_lock_)> _(&items_lock_);

		auto connection = get_connection(one);
		if(connection)
		{
			auto result = connection->remove_tag(one);
			if (result)
			{
				//!!!! TODO clean all stuff
				connect_items_ = true;
			}
			results.emplace_back(result);
		}
		else
		{
			results.emplace_back("Invalid handle value!");
		}
	}
	return true;
}

rx_result rx_subscription::process_subscription (bool force_connect)
{
	if (!active_)
		return false;

	///!!!!!WARNING THIS IS A MANUAL LOCK FUNCTION
	///!!!!!IT HAS BEEN DONE ON PURPOSE
	///!!!!!JUST BE CAREFUL

	if (retrieve_items_)
	{
		// we are multithreaded nay way so use the host's thread if needed!!!
		retrieve_items_ = false;
		items_lock_.lock();
		// retreive unknown items
		std::vector<string_type> to_query;
		for (auto& one : connections_)
		{
			if (!one.connected && !one.item)
			{
				to_query.emplace_back(one.path);
			}
		}
		items_lock_.unlock();
		// query for unknown items all at once to be faster
		if (!to_query.empty())
		{
			auto items_result = platform_runtime_manager::instance().get_cache().get_items(to_query);
			RX_ASSERT(items_result.size() == to_query.size());
			size_t count = to_query.size();

			locks::auto_lock_t<decltype(items_lock_)> _(&items_lock_);
			for (size_t i = 0; i < count; i++)
			{
				if (items_result[i])
				{
					auto connect_it = connection_paths_.find(to_query[i]);

					RX_ASSERT(connect_it != connection_paths_.end());
					RX_ASSERT(connect_it->second < connections_.size() && !connections_[connect_it->second].item);

					connections_[connect_it->second].item = std::move(items_result[i]);
					connect_items_ = true;

				}
			}
		}
	}

	if (connect_items_ || force_connect)
	{
		connect_items_ = false;
		// go thrugth and connect or execute in other thread
		std::set<rx_thread_handle_t> to_send;
		std::map<runtime_handle_t, runtime_handle_t> to_connect;
		auto ts = rx_time::now();
		items_lock_.lock();

		for (size_t idx = 0; idx < connections_.size(); idx++)
		{
			if (connections_[idx].item)
			{
				auto executer = connections_[idx].item->get_executer();
				if (executer == rx_thread_context())
				{
					connect_items_ = connections_[idx].process_connection(ts, smart_this());
				}
				else if (!force_connect && to_send.count(executer) == 0)
				{
					to_send.emplace(executer);
				}
			}
		}
		items_lock_.unlock();
		if (!to_send.empty())
		{
			std::function<void(smart_ptr)> func = [](smart_ptr whose) {
				whose->process_subscription(true);
			};
			for (auto one : to_send)
			{
				rx_post_function_to(one, func, smart_this(), smart_this());
			}
		}
	}
	if (target_ == rx_thread_context())
	{
		rx_subscription_callback* callback = nullptr;
		std::vector<update_item> to_send;
		items_lock_.lock();
		if (!pending_updates_.empty())
		{
			callback = callback_;
			to_send = pending_updates_;
			pending_updates_.clear();
		}
		items_lock_.unlock();
		if (callback)
			callback->items_changed(to_send);
	}
	return true;
}

void rx_subscription::activate ()
{
	if (!active_)
	{
		active_ = true;
		std::function<bool(int)> func = [this] (int) -> bool { return process_subscription();  };
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
        locks::auto_lock_t<decltype(items_lock_)> _(&items_lock_);
        for (const auto& one : items)
        {			
			auto it = handles_.find(one.handle);
			if (it != handles_.end())
			{
				for (auto&& handle : it->second)
					pending_updates_.emplace_back(update_item{ std::forward<decltype(handle)>(handle), one.value });
			}
        }
	}
}

void rx_subscription::transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items)
{
}

rx_thread_handle_t rx_subscription::get_target ()
{
	return target_;
}

runtime_connection_data* rx_subscription::get_connection (runtime_handle_t handle)
{
	size_t idx = ((handle >> 16) & 0xffff);
	if (idx < connections_.size() && !connections_[idx].path.empty())
	{
		return &connections_[idx];
	}
	else
	{
		return nullptr;
	}
}

rx_subscription_tag* rx_subscription::get_tag (runtime_handle_t handle)
{
	size_t idx = ((handle >> 16) & 0xffff);
	if (idx < connections_.size() && !connections_[idx].path.empty())
	{
		return connections_[idx].get_tag(handle);
	}
	else
	{
		return nullptr;
	}
}

rx_result rx_subscription::write_items (runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& result)
{
	result.reserve(values.size());
	items_lock_.lock();
	for (auto&& one : values)
	{
		auto connection = get_connection(one.first);
		auto tag = get_tag(one.first);
		if (connection && tag)
		{
			RX_ASSERT(connection->item);
			rx_thread_handle_t target = connection->item->get_executer();
			auto it = pending_writes_.find(target);
			if (it != pending_writes_.end())
			{
				it->second.emplace_back(std::move(one));
			}
			else
			{
				auto ret_val = pending_writes_.emplace(target, pending_writes_type::mapped_type());
				if (ret_val.second)
				{
					ret_val.first->second.emplace_back(std::move(one));
				}
			}
			result.emplace_back(true);
		}
		else
		{
			result.emplace_back("Invalid item handle");
		}

	}
	if (!pending_writes_.empty())
	{
		for (auto one : pending_writes_)
		{
			rx_post_function<smart_ptr>([](smart_ptr whose)
				{
					whose->process_writes();
				}, smart_this(), one.first);
		}
	}
	items_lock_.unlock();

	return true;
}

rx_result rx_subscription::process_writes (bool force_write)
{
	std::map<size_t, std::vector<std::pair<runtime_handle_t, rx_simple_value> > > writes;
	locks::auto_lock_t<decltype(items_lock_)> _(&items_lock_);
	auto thread_it = pending_writes_.find(rx_thread_context());
	if (thread_it!=pending_writes_.end())
	{
		if (!thread_it->second.empty())
		{
			for (auto&& one : thread_it->second)
			{
				size_t idx = ((one.first >> 16) & 0xffff);
				if (idx >= connections_.size() || connections_[idx].path.empty())
					continue;
				auto tag = get_tag(one.first);
				if (tag)
				{
					auto it = writes.find(idx);
					if (it != writes.end())
					{
						RX_ASSERT(connections_[idx].item);
						it->second.emplace_back(tag->target_handle, std::move(one.second));
					}
					else
					{
						std::vector<std::pair<runtime_handle_t, rx_simple_value> > new_vector;
						new_vector.emplace_back(tag->target_handle, std::move(one.second));
						writes.emplace(idx, std::move(new_vector));
					}
				}
			}
		}
		thread_it->second.clear();
		for (auto&& writes_it : writes)
		{
			auto& connection = connections_[writes_it.first];
			auto write_result = connection.item->write_items(55, writes_it.second, smart_this());
		}
	}
	return true;
}


} // namespace subscriptions
} // namespace sys_runtime
