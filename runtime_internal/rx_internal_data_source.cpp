

/****************************************************************************
*
*  runtime_internal\rx_internal_data_source.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_internal_data_source
#include "runtime_internal/rx_internal_data_source.h"

#include "sys_internal/rx_async_functions.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_internal {

namespace sys_runtime {

namespace data_source {

// Class rx_internal::sys_runtime::data_source::internal_data_source 

internal_data_source::internal_data_source (const string_type& path)
      : next_subscription_id_(0)
{
}



void internal_data_source::add_item (const string_type& path, uint32_t rate, value_handle_extended& handle, rx_mode_type mode)
{
	auto it = rate_subscriptions_.find({ rate, mode.raw_format });
	if (it == rate_subscriptions_.end())
	{
		auto subs = std::make_unique<internal_data_subscription>(handle, my_controler, mode);
		uint16_t subs_handle = next_subscription_id_++;
		internal_data_subscription& subscription(*subs);
		rate_subscriptions_.emplace(std::pair<uint32_t, uint32_t>(rate, mode.raw_format), rate_subscription_data{ *subs, subs_handle });
		subscriptions_.emplace(subs_handle, subscription_data{ std::move(subs), subs_handle });
		handle.subscription = subs_handle;
		subscription.add_item(path, handle);
	}
	else
	{
		handle.subscription = it->second.handle;
		it->second.subscription.get().add_item(path, handle);
	}
}

void internal_data_source::write_item (const value_handle_extended& handle, rx_simple_value val, runtime_transaction_id_t id)
{
	auto it = subscriptions_.find(handle.subscription);
	if (it != subscriptions_.end())
	{
		it->second.subscription->write_item(handle, std::move(val), id);
	}
}

void internal_data_source::execute_item (const value_handle_extended& handle, values::rx_simple_value data, runtime_transaction_id_t id)
{
	auto it = subscriptions_.find(handle.subscription);
	if (it != subscriptions_.end())
	{
		it->second.subscription->execute_item(handle, std::move(data), id);
	}
}

void internal_data_source::execute_item (const value_handle_extended& handle, data::runtime_values_data data, runtime_transaction_id_t id)
{
	auto it = subscriptions_.find(handle.subscription);
	if (it != subscriptions_.end())
	{
		it->second.subscription->execute_item(handle, std::move(data), id);
	}
}

void internal_data_source::remove_item (const value_handle_extended& handle)
{
	auto it = subscriptions_.find(handle.subscription);
	if (it != subscriptions_.end())
	{
		it->second.subscription->remove_item(handle);
	}
}

bool internal_data_source::is_empty () const
{
	return false;
}


// Class rx_internal::sys_runtime::data_source::internal_data_subscription 

internal_data_subscription::internal_data_subscription (value_handle_extended handles, data_controler* controler, rx_mode_type mode)
      : controler_(controler),
        handles_(handles)
{
	my_subscription_ = rx_create_reference<subscriptions::rx_subscription>(this, mode);
	my_subscription_->activate();
}



void internal_data_subscription::add_item (const string_type& path, value_handle_extended& handle)
{
	std::vector<rx_result_with<runtime_handle_t> > results;
	string_array items{path};
	my_subscription_->connect_items(items, results);
	if (!results.empty() && results[0] && results[0].value())
		handle.item = results[0].value();
}

void internal_data_subscription::remove_item (const value_handle_extended& handle)
{
	std::vector<runtime_handle_t> handles{ handle.item };
	std::vector<rx_result> results;
	my_subscription_->disconnect_items(handles, results);
}

void internal_data_subscription::items_changed (const std::vector<update_item>& items)
{
	std::vector<std::pair<value_handle_type, rx_value> > values;
	values.reserve(items.size());
	for (const auto& one : items)
	{
		// we can reuse this one in a loop because we are changing only item handle value and reusing rest
		handles_.item = one.handle;
		values.emplace_back(handles_.make_handle(), one.value);
	}
	controler_->items_changed(values);
}

void internal_data_subscription::write_item (const value_handle_extended& handle, rx_simple_value val, runtime_transaction_id_t id)
{
	std::vector<std::pair<runtime_handle_t, rx_simple_value> > temp{ {handle.item, std::move(val)} };
	std::vector<rx_result> results;
	auto result = my_subscription_->write_items(id, std::move(temp), results);
	if (id != 0)
	{
		if (!result)
		{
			controler_->result_received(handle.make_handle(), std::move(result), id);
		}
		else
		{
			RX_ASSERT(results.size() == 1);
			if (!results.empty())
			{
				if(!results[0])
					controler_->result_received(handle.make_handle(), std::move(results[0]), id);
			}
			else
			{
				controler_->result_received(handle.make_handle(), "Unexpected error", id);
			}
		}
	}
}

void internal_data_subscription::write_completed (runtime_transaction_id_t transaction_id, std::vector<write_result_item> results)
{
	for (auto& one : results)
	{
		// we can reuse this one in a loop because we are changing only item handle value and reusing rest
		handles_.item = one.handle;
		controler_->result_received(handles_.make_handle(), std::move(one.result), transaction_id);
	}
}

void internal_data_subscription::execute_item (const value_handle_extended& handle, values::rx_simple_value data, runtime_transaction_id_t id)
{
	
	auto result = my_subscription_->execute_item(id, handle.item, std::move(data));
	if (id != 0)
	{
		if (!result)
		{
			controler_->execute_result_received(handle.make_handle(), std::move(result), values::rx_simple_value(), id);
		}
	}
}

void internal_data_subscription::execute_item (const value_handle_extended& handle, data::runtime_values_data data, runtime_transaction_id_t id)
{
	auto result = my_subscription_->execute_item(id, handle.item, std::move(data));
	if (id != 0)
	{
		if (!result)
		{
			controler_->execute_result_received(handle.make_handle(), std::move(result), data::runtime_values_data(), id);
		}
	}
}

void internal_data_subscription::execute_completed (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, values::rx_simple_value data)
{
	// we can reuse this one in a loop because we are changing only item handle value and reusing rest
	handles_.item = item;
	controler_->execute_result_received(handles_.make_handle(), std::move(result), std::move(data), transaction_id);
}

bool internal_data_subscription::is_empty () const
{
	return false;
}

void internal_data_subscription::execute_completed (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, data::runtime_values_data data)
{
	handles_.item = item;
	controler_->execute_result_received(handles_.make_handle(), std::move(result), std::move(data), transaction_id);
}


} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal

