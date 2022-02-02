

/****************************************************************************
*
*  runtime_internal\rx_data_source.cpp
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

// rx_data_source
#include "runtime_internal/rx_data_source.h"

#include "runtime_internal/rx_internal_data_source.h"
#include "sys_internal/rx_inf.h"
#include "platform_source.h"
#include "model/rx_meta_internals.h"


namespace rx_internal {

namespace sys_runtime {

namespace data_source {
value_handle_type value_handle_extended::make_handle() const
{
	return (((value_handle_type)source) << 48)
		| (((value_handle_type)subscription) << 32)
		| item;
}
value_handle_extended value_handle_extended::fill_from_handle(value_handle_type handle)
{
	value_handle_extended ret;
	ret.source = (handle >> 48);
	ret.subscription = ((handle >> 32) & 0xffff);
	ret.item = handle & 0xffffffff;
	return ret;
}

// Class rx_internal::sys_runtime::data_source::data_controler 

data_controler::data_controler (rx::threads::physical_job_thread* worker)
      : my_worker_(worker),
        next_source_id_(0)
{
}


data_controler::~data_controler()
{
}



void data_controler::register_value (value_handle_type handle, value_point_impl* whose)
{
	auto it = registered_values_.find(handle);
	if (it != registered_values_.end())
	{
		it->second.insert(whose);
	}
	else
	{
		registered_values_.emplace(handle, registered_objects_type{ whose });
	}
}

void data_controler::unregister_value (value_handle_type handle, value_point_impl* whose)
{
	auto it = registered_values_.find(handle);
	if (it != registered_values_.end())
	{
		it->second.erase(whose);
	}
	else
	{
		RX_ASSERT(false);
	}
}

value_handle_type data_controler::add_item (const string_type& path, uint32_t rate)
{
	string_type source;
	string_type concrete;

	auto idx = path.rfind(RX_SOURCE_DELIMETER);
	if (idx != string_type::npos)
	{
		source = path.substr(0, idx);
		concrete = path.substr(idx + 1);
	}
	else
	{
		source = "";
		concrete = path;
	}

	auto it = named_sources_.find(source);
	if (it != named_sources_.end())
	{
		value_handle_extended handle{ it->second.handle, 0, 0 };
		it->second.source.get().add_item(concrete, rate, handle);
		return handle.make_handle();
	}
	else
	{
		auto&& temp = data_source_factory::instance().create_data_source(source);
		if (temp)
		{
			temp.value()->my_controler = this;
			auto&& id = ++next_source_id_;
			auto result = sources_.emplace(id, source_data{ std::move(temp.value()), source });
			named_sources_.emplace(source, named_source_data{ *result.first->second.source, id });

			value_handle_extended handle{ id, 0, 0 };
			result.first->second.source->add_item(concrete, rate, handle);
			return handle.make_handle();
		}
	}
	return 0;
}

void data_controler::write_item (value_handle_type handle, rx_simple_value val, runtime_transaction_id_t id)
{
	auto ex_handle = value_handle_extended::fill_from_handle(handle);
	auto it = sources_.find(ex_handle.source);
	if (it != sources_.end())
	{
		it->second.source->write_item(ex_handle, std::move(val), id);
	}
	else
	{
		RX_ASSERT(false);
	}
}

void data_controler::execute_item (value_handle_type handle, data::runtime_values_data data, runtime_transaction_id_t id)
{
	auto ex_handle = value_handle_extended::fill_from_handle(handle);
	auto it = sources_.find(ex_handle.source);
	if (it != sources_.end())
	{
		it->second.source->execute_item(ex_handle, std::move(data), id);
	}
	else
	{
		RX_ASSERT(false);
	}
}

void data_controler::remove_item (value_handle_type handle)
{
	auto ex_handle = value_handle_extended::fill_from_handle(handle);
	auto it = sources_.find(ex_handle.source);
	if (it != sources_.end())
	{
		it->second.source->remove_item(ex_handle);
	}
	else
	{
		RX_ASSERT(false);
	}
}

data_controler* data_controler::get_controler ()
{
	return rx_internal::infrastructure::server_runtime::instance().get_data_controler(rx_thread_context());
}

void data_controler::items_changed (const std::vector<std::pair<value_handle_type, rx_value> >& values)
{
	changed_points_.clear();
	for (const auto& one : values)
	{
		auto it = registered_values_.find(one.first);
		if (it != registered_values_.end())
		{
			for (auto one_item : it->second)
			{
				one_item->value_changed(one.first, one.second);
				changed_points_.emplace_back(one_item);
			}
		}
	}
	if (!changed_points_.empty())
	{
		for (auto one : changed_points_)
			one->calculate(token_buffer_);
	}
}

void data_controler::result_received (value_handle_type handle, rx_result&& result, runtime_transaction_id_t id)
{
	if (id == 0)
		return;// nothing to do
	auto it = registered_values_.find(handle);
	if (it != registered_values_.end())
	{
		size_t count = it->second.size();
		if (count == 1)
		{// just move the result
			(*it->second.begin())->single_result_received(std::move(result), id);
		}
		else if (count > 0)
		{
			size_t last = count - 1;// it is more readable this way
			size_t counter = 0;
			bool done = false;
			for (auto itv = it->second.begin(); itv != it->second.end() && !done; itv++, counter++)
			{
				if(counter== last)
					done = (*itv)->single_result_received(std::move(result), id);
				else
					done = (*itv)->shared_result_received(result, id);
			}
		}
	}
}

void data_controler::execute_result_received (value_handle_type handle, rx_result&& result, data::runtime_values_data data, runtime_transaction_id_t id)
{
	if (id == 0)
		return;// nothing to do
	auto it = registered_values_.find(handle);
	if (it != registered_values_.end())
	{
		size_t count = it->second.size();
		if (count == 1)
		{// just move the result
			(*it->second.begin())->single_execute_result_received(std::move(result), data, id);
		}
		else if (count > 0)
		{
			size_t last = count - 1;// it is more readable this way
			size_t counter = 0;
			bool done = false;
			for (auto itv = it->second.begin(); itv != it->second.end() && !done; itv++, counter++)
			{
				if (counter == last)
					done = (*itv)->single_execute_result_received(std::move(result), data, id);
				else
					done = (*itv)->shared_execute_result_received(result, data, id);
			}
		}
	}
}


// Class rx_internal::sys_runtime::data_source::data_source 

data_source::~data_source()
{
}


namespace
{
data_source_factory* g_obj = nullptr;
}
// Class rx_internal::sys_runtime::data_source::data_source_factory 

data_source_factory::data_source_factory()
{
}



data_source_factory& data_source_factory::instance ()
{
	if (g_obj == nullptr)
		g_obj = new data_source_factory();
	return *g_obj;
}

void data_source_factory::register_data_source (const string_type& type, std::function<std::unique_ptr<data_source>(const string_type&)> creator)
{
	creators_.emplace(type, creator);
}

rx_result data_source_factory::register_internal_sources ()
{
	default_ = [] (const string_type& path){return std::make_unique<internal_data_source>(path); };
	register_data_source("rx", default_);
	auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<source_type>().register_constructor(
		RX_PLATFORM_SOURCE_TYPE_ID, [] {
			return rx_create_reference<platform_source>();
		});
	return true;
}

rx_result_with<std::unique_ptr<data_source> > data_source_factory::create_data_source (const string_type& source)
{
	auto idx = source.find("://");
	string_type type;
	string_type concrete;
	if (idx != string_type::npos)
	{
		type = source.substr(0, idx);
		concrete = source.substr(idx + 3);
	}
	else
	{
		concrete = source;
	}
	if (!type.empty())
	{
		auto it = creators_.find(type);
		if (it != creators_.end())
			return it->second(concrete);
		else
			return "Data Source not registered!";
	}
	return default_(concrete);
}

void data_source_factory::deinitialize ()
{
	if (g_obj)
		delete g_obj;
}


} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal

