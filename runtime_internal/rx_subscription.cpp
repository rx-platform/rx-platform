

/****************************************************************************
*
*  runtime_internal\rx_subscription.cpp
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

#include "rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "system/server/rx_platform_item.h"


namespace rx_internal {

namespace sys_runtime {

namespace subscriptions {

// Class rx_internal::sys_runtime::subscriptions::rx_subscription 

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
		///!!!!!WARNING THIS IS A MANUAL LOCK FUNCTION
		///!!!!!IT HAS BEEN DONE ON PURPOSE
		///!!!!!JUST BE CAREFUL
		items_lock_.lock();
		auto it_inv = tag_paths_.find(one);
		if (it_inv == tag_paths_.end())
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

				to_process_.emplace(it->second);
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

				to_retrieve_.emplace(idx);
				to_process_.emplace(idx);
			}
			if (new_id)
			{
				tag_paths_.emplace(one, new_id);
				inverse_tags_.emplace(new_id, one);
			}
			items_lock_.unlock();

			if (new_id)
			{
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
				auto it = inverse_tags_.find(one);
				if (it != inverse_tags_.end())
				{
					tag_paths_.erase(it->second);
					inverse_tags_.erase(it);
				}
				// inverse_tags_
				to_process_.emplace(one >> 16);
				results.emplace_back(true);
			}
			else
			{
				results.emplace_back("Invalid handle value!");
			}
		}
		else
		{
			results.emplace_back("Invalid handle value!");
		}
	}
	return true;
}

void rx_subscription::process_subscription (bool posted)
{
	rx_time now(rx_time::now());
	auto current_context = rx_thread_context();
	bool synchronized = (target_ == current_context);
	items_lock_.lock();
	if (!to_retrieve_.empty())
	{
		// we are multi-threaded nay way so use the host's thread if needed!!!
		// retrieve unknown items
		std::vector<string_type> to_query;
		for (auto idx : to_retrieve_)
		{
			auto& one = connections_[idx];
			if (!one.connected && !one.item && one.last_checked + 1000 < now)
			{
				one.last_checked = now;
				to_query.emplace_back(one.path);
			}
		}
		// query for unknown items all at once to be faster
		if (!to_query.empty())
		{
			auto items_result = platform_runtime_manager::instance().get_cache().get_items(to_query);
			RX_ASSERT(items_result.size() == to_query.size());
			size_t count = to_query.size();

			for (size_t i = 0; i < count; i++)
			{
				auto connect_it = connection_paths_.find(to_query[i]);
				if (items_result[i])
				{
					RX_ASSERT(connect_it != connection_paths_.end());
					RX_ASSERT(connect_it->second < connections_.size() && !connections_[connect_it->second].item);

					connections_[connect_it->second].item = std::move(items_result[i]);
					to_process_.emplace(connect_it->second);
					to_retrieve_.erase(connect_it->second);
				}
				else
				{
					// delete processing just in case we're dead
					to_process_.erase(connect_it->second);
				}
			}
		}
	}
	if (!to_process_.empty())
	{
		// go through and connect or execute in other thread
		std::set<rx_thread_handle_t> to_send;
		auto ts = rx_time::now();

		auto it_process = to_process_.begin();
		while (it_process != to_process_.end())
		{
			if (connections_[*it_process].item)
			{
				auto executer = connections_[*it_process].item->get_executer();
				if (executer == current_context)
				{
					connections_[*it_process].process_connection(ts, smart_this());
					it_process = to_process_.erase(it_process);
				}
				else
				{
					it_process++;
					if (!posted && to_send.count(executer) == 0)
					{
						to_send.emplace(executer);
					}
				}
			}
			else
			{
				it_process++;
			}
		}
		items_lock_.unlock();
		if (!to_send.empty())
		{
			for (auto one : to_send)
			{
				rx_post_function_to(one, smart_this(), [](smart_ptr whose) {
                        whose->process_subscription(true);
                    }, smart_this());
			}
		}
	}
	else
	{
		items_lock_.unlock();
	}
	if (synchronized)
	{
		rx_subscription_callback* callback = nullptr;
		std::vector<update_item> to_send;

		items_lock_.lock();
		callback = callback_;
		if (!pending_updates_.empty())
		{
			to_send = pending_updates_;
			pending_updates_.clear();
		}
		items_lock_.unlock();
		if (callback)
			callback->items_changed(to_send);
	}
}

void rx_subscription::activate ()
{
	if (!active_)
	{
		active_ = true;
		timer_ = rx_create_periodic_function(100u, smart_this()
			, [this]() { process_subscription(); });
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
		std::vector<runtime_connection_data*> found_dead;
        for (const auto& one : items)
        {
			auto it = handles_.find(one.handle);
			if (it != handles_.end() && !it->second.empty())
			{
				for (auto&& handle : it->second)
					pending_updates_.emplace_back(update_item{ std::forward<decltype(handle)>(handle), one.value });

				if (one.value.is_dead())
				{
					auto conn = get_connection(it->second.front());
					if (conn)
					{
						if (conn->connection_dead())
						{
							to_retrieve_.emplace(it->second.front() >> 16);
							to_process_.emplace(it->second.front() >> 16);
							handles_.erase(it);
						}
					}
				}
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
	subscription_write_transaction transaction;
	transaction.client_handle = transaction_id;
	runtime_transaction_id_t master_id = platform_runtime_manager::get_new_transaction_id();
	rx_result total_result(true);

	result.reserve(values.size());
	items_lock_.lock();
	for (auto&& one : values)
	{
		auto connection = get_connection(one.first);
		auto tag = get_tag(one.first);
		if (connection && tag)
		{
			RX_ASSERT(connection->item);
			auto single_trans_id = transaction.add_write(master_id, transactions_map_, one.first);
			rx_thread_handle_t target = connection->item->get_executer();
			auto it = pending_writes_.find(target);
			if (it != pending_writes_.end())
			{
				it->second.emplace_back(pending_write_data{ single_trans_id , one.first, std::move(one.second)});
			}
			else
			{
				auto ret_val = pending_writes_.emplace(target, pending_writes_type::mapped_type());
				if (ret_val.second)
				{
					ret_val.first->second.emplace_back(pending_write_data{ single_trans_id , one.first, std::move(one.second) });
				}
			}
			result.emplace_back(true);
		}
		else
		{
			string_type err_msg = "Invalid item handle";
			result.emplace_back(err_msg);
		}
	}
	if (!transaction.is_done())
	{
		write_transactions_.emplace(master_id, std::move(transaction));
		RX_ASSERT(!pending_writes_.empty());
		if (!pending_writes_.empty())
		{
			for (auto one : pending_writes_)
			{
				rx_post_function_to(one.first, smart_this(), [this]()
					{
						process_writes();
					});
			}
		}
	}
	else
	{
		total_result = "Invalid item handles";
	}
	items_lock_.unlock();

	return total_result;
}

void rx_subscription::process_writes ()
{
	std::map<size_t, std::map<runtime_transaction_id_t, std::vector<std::pair<runtime_handle_t, rx_simple_value> > > > writes;
	locks::auto_lock_t<decltype(items_lock_)> _(&items_lock_);
	auto thread_it = pending_writes_.find(rx_thread_context());
	if (thread_it!=pending_writes_.end())
	{
		if (!thread_it->second.empty())
		{
			for (auto&& one : thread_it->second)
			{
				size_t idx = ((one.handle >> 16) & 0xffff);
				if (idx >= connections_.size() || connections_[idx].path.empty())
					continue;
				auto tag = get_tag(one.handle);
				if (tag)
				{
					auto it = writes.find(idx);
					if (it != writes.end())
					{
						RX_ASSERT(connections_[idx].item);
						auto it_trans = it->second.find(one.trans_id);
						if (it_trans != it->second.end())
						{
							it_trans->second.emplace_back(tag->target_handle, std::move(one.value));
						}
						else
						{
							std::vector<std::pair<runtime_handle_t, rx_simple_value> > new_vector;
							new_vector.emplace_back(tag->target_handle, std::move(one.value));
							it->second.emplace(one.trans_id, std::move(new_vector));
						}
					}
					else
					{
						std::map<runtime_transaction_id_t, std::vector<std::pair<runtime_handle_t, rx_simple_value> > > new_map;
						std::vector<std::pair<runtime_handle_t, rx_simple_value> > new_vector;
						new_vector.emplace_back(tag->target_handle, std::move(one.value));
						new_map.emplace(one.trans_id, std::move(new_vector));
						writes.emplace(idx, std::move(new_map));
					}
				}
				else
				{
					write_complete(one.trans_id, one.handle, rx_result("Invalid item handle."));
				}
			}
		}
		thread_it->second.clear();
		for (auto& conns_it : writes)
		{
			auto& connection = connections_[conns_it.first];
			for (auto&& write : conns_it.second)
			{
				auto write_result = connection.item->write_items(write.first, std::move(write.second), smart_this());
			}
		}
	}
}

void rx_subscription::write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result)
{
	locks::auto_lock_t<decltype(items_lock_)> _(&items_lock_);
	auto map_it = transactions_map_.find(transaction_id);
	if (map_it != transactions_map_.end())
	{
		auto trans_it = write_transactions_.find(map_it->second);
		if (trans_it != write_transactions_.end())
		{
			auto end = trans_it->second.write_done(transaction_id, std::move(result), transactions_map_);
			if (end)
			{
				bool first_one = pending_write_results_.empty();
				auto results = trans_it->second.get_results();
				pending_write_results_.emplace_back(trans_it->second.client_handle, std::move(results));
				write_transactions_.erase(trans_it);
				if (first_one)
				{
					rx_post_function_to(target_, smart_this(), [](smart_ptr whose) {
						whose->process_results();
						}, smart_this());
				}
			}
		}
	}
}

void rx_subscription::process_results ()
{
	rx_subscription_callback* callback = nullptr;
	pending_write_results_type write_results;

	items_lock_.lock();
	callback = callback_;
	if (!pending_write_results_.empty())
	{
		write_results = std::move(pending_write_results_);
		pending_write_results_.clear();
	}
	items_lock_.unlock();
	if (callback && !write_results.empty())
	{
		for (auto&& one : write_results)
		{
			callback->write_completed(one.first, std::move(one.second));
		}
	}
}


// Class rx_internal::sys_runtime::subscriptions::rx_subscription_tag 


// Class rx_internal::sys_runtime::subscriptions::rx_subscription_callback 

rx_subscription_callback::~rx_subscription_callback()
{
}



// Class rx_internal::sys_runtime::subscriptions::runtime_connection_data 


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
			if (tags_[idx].target_handle)
			{
				tags_[idx].mine_handle = 0;
			}
			else
			{
				tags_[idx].path.clear();
				empty_slots_.emplace_back(idx);
			}
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
		connect_indexes.emplace_back(idx);
		return ret;
	}
	else
	{
		auto idx = empty_slots_.back();
		empty_slots_.pop_back();
		runtime_handle_t ret = (((runtime_handle_t)idx + 1) | connection_handle);
		tag.mine_handle = ret;
		RX_ASSERT(tags_[idx].path.empty());
		tags_[idx] = std::move(tag);
		connect_indexes.emplace_back(idx);
		return ret;
	}
}

bool runtime_connection_data::process_connection (const rx_time& ts, rx_subscription_ptr whose)
{
	if (!connecting && !connected && item)
	{
		connect_indexes.clear();
		to_connect.clear();
		std::vector<runtime_handle_t> to_disconnect;
		std::vector<size_t> disconnect_indexes;
		size_t count = tags_.size();
		for (size_t idx=0; idx<count; idx++)
		{
			if (tags_[idx].path.empty())
				continue;
			if (tags_[idx].target_handle == 0)
			{
				to_connect.emplace_back(tags_[idx].path);
				connect_indexes.emplace_back(idx);
			}
			else if (tags_[idx].target_handle != 0 && tags_[idx].mine_handle == 0)
			{
				to_disconnect.emplace_back(tags_[idx].target_handle);
				disconnect_indexes.emplace_back(idx);
			}
		}
		if (!to_disconnect.empty())
		{
			auto result = item->disconnect_items(to_disconnect, whose);
			count = result.size();
			for (size_t idx = 0; idx < count; idx++)
			{
				if (result[idx])
				{
					tags_[disconnect_indexes[idx]].path.clear();
					empty_slots_.emplace_back(disconnect_indexes[idx]);
				}
			}
		}
		if (!to_connect.empty())
		{
			RX_ASSERT(to_connect.size() == connect_indexes.size());
			auto result = item->connect_items(to_connect, whose);
			RX_ASSERT(result.size() == to_connect.size());
			count = result.size();
			bool had_good = false;
			bool all_good = true;
			for (size_t idx = 0; idx < count; idx++)
			{
				if (result[idx])
				{
					had_good = true;
					tags_[connect_indexes[idx]].target_handle = result[idx].value();
					whose->handles_[result[idx].value()].emplace_back(tags_[connect_indexes[idx]].mine_handle);
				}
				else
				{
					all_good = false;
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
			if (!had_good)
				connection_dead();

			return all_good;
		}
	}
	return false;
}

bool runtime_connection_data::connection_dead ()
{
	if (!item)
		return false;// probably already done!!!
	connected = false;
	connecting = false;
	item.reset();
	for (auto& one : tags_)
	{
		one.target_handle = 0;
	}
	return true;
}


// Class rx_internal::sys_runtime::subscriptions::subscription_write_transaction 


runtime_transaction_id_t subscription_write_transaction::add_write (runtime_transaction_id_t master_id, transactions_map_type& map, runtime_handle_t handle)
{
	auto trans_id = platform_runtime_manager::get_new_transaction_id();
	writes_.emplace(trans_id, single_write_data{ handle, writes_.size(), rx_result() });
	pending_count_++;
	map.emplace(trans_id, master_id);
	return trans_id;
}

bool subscription_write_transaction::write_done (runtime_transaction_id_t trans_id, rx_result&& result, transactions_map_type& map)
{
	auto it = writes_.find(trans_id);
	if (it != writes_.end())
	{
		it->second.result = std::move(result);
		map.erase(trans_id);
		pending_count_--;
	}
	else
	{
		RX_ASSERT(false);
	}
	return is_done();
}

bool subscription_write_transaction::is_done () const
{
	return pending_count_ <= 0;
}

std::vector<std::pair<runtime_handle_t, rx_result> > subscription_write_transaction::get_results ()
{
	std::vector<std::pair<runtime_handle_t, rx_result> > ret;
	size_t ret_size = writes_.size();
	ret.resize(ret_size);
	for (auto&& one : writes_)
	{
		RX_ASSERT(one.second.transaction_index < ret_size);
		if (one.second.transaction_index < ret_size)
		{
			ret[one.second.transaction_index] = { one.second.handle, std::move(one.second.result) };
		}
	}
	return ret;
}


} // namespace subscriptions
} // namespace sys_runtime
} // namespace rx_internal

