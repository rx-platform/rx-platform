

/****************************************************************************
*
*  runtime_internal\rx_runtime_internal.cpp
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


// rx_resolvers
#include "system/runtime/rx_resolvers.h"
// rx_runtime_internal
#include "runtime_internal/rx_runtime_internal.h"

#include "sys_internal/rx_internal_ns.h"
#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_runtime_helpers.h"
#include "runtime_internal/rx_data_source.h"
#include "system/server/rx_platform_item.h"
#include "rx_simulation.h"
#include "rx_filters.h"
#include "rx_variables.h"
#include "rx_runtime_relations.h"
#include "sys_internal/rx_async_functions.h"


namespace rx_platform
{
locks::slim_lock* g_runtime_lock = nullptr;
}


namespace rx_internal {

namespace sys_runtime {
template<>
rx_object_ptr get_runtime_impl<meta::object_types::object_type>(runtime_cache* whose, const rx_node_id& id)
{
	return whose->get_object(id);
}
template<>
rx_domain_ptr get_runtime_impl<meta::object_types::domain_type>(runtime_cache* whose, const rx_node_id& id)
{
	return whose->get_domain(id);
}
template<>
rx_application_ptr get_runtime_impl<meta::object_types::application_type>(runtime_cache* whose, const rx_node_id& id)
{
	return whose->get_application(id);
}
template<>
rx_port_ptr get_runtime_impl<meta::object_types::port_type>(runtime_cache* whose, const rx_node_id& id)
{
	return whose->get_port(id);
}
namespace {
platform_runtime_manager* g_instance = nullptr;
}

// Class rx_internal::sys_runtime::platform_runtime_manager 

platform_runtime_manager::platform_runtime_manager()
      : first_cpu_(0),
        last_cpu_(0)
{
}



platform_runtime_manager& platform_runtime_manager::instance ()
{
	if (g_instance == nullptr)
		g_instance = new platform_runtime_manager();
	return *g_instance;
}

rx_thread_handle_t platform_runtime_manager::resolve_app_processor (const application_instance_data& data)
{
	rx_thread_handle_t this_cpu = data.get_data().processor;
	if (this_cpu < 0)
		this_cpu = resolve_processor_auto();
	this_cpu = this_cpu % cpu_coverage_.size();
	cpu_coverage_[this_cpu]++;
	return this_cpu;
}

rx_thread_handle_t platform_runtime_manager::resolve_domain_processor (const domain_instance_data& data)
{
	rx_thread_handle_t this_cpu = data.get_data().processor;
	if (this_cpu < 0)
	{
		// handle get application and get his
		this_cpu = resolve_processor_auto();
	}
	else
	{
		this_cpu = this_cpu % cpu_coverage_.size();
	}
	cpu_coverage_[this_cpu]++;
	return this_cpu;
}

void platform_runtime_manager::remove_one (rx_thread_handle_t from_where)
{
	// TOTO CPU!!!!
	//cpu_coverage_[from_where- first_cpu_]--;
}

rx_result platform_runtime_manager::initialize (hosting::rx_platform_host* host, runtime_data_t& data)
{
	size_t cpu_count = 1;
	char buff[0x40];
	rx_collect_processor_info(buff, sizeof(buff), &cpu_count);
	std::ostringstream ss;
	ss << "Detected "
		<< cpu_count
		<< (rx_big_endian ? " Big-endian" : " Little-endian")
		<< " CPU Cores "
		<< " [PID:"
		<< rx_pid
		<< "]";
	RUNTIME_LOG_INFO("platform_runtime_manager", 900, ss.str());

	if (cpu_count > (size_t)data.io_pool_size)
	{
		first_cpu_ = data.io_pool_size;
	}
	else
	{
		first_cpu_ = 0;
	}
	last_cpu_ = first_cpu_ + data.workers_pool_size;
	cpu_coverage_.assign(last_cpu_ - first_cpu_, 0);
	rx_result result = data_source::data_source_factory::instance().register_internal_sources();
	if (result)
	{
		result = simulation::register_simulation_constructors();
		result = relations_runtime::register_internal_relations_constructors();
		result = filters::register_filter_constructors();
		result = variables::register_variables_constructors();
	}
	return result;
}

rx_thread_handle_t platform_runtime_manager::resolve_processor_auto ()
{
	int min = std::numeric_limits<int>::max();
	rx_thread_handle_t ret = 0;
	for (size_t i = 0; i<cpu_coverage_.size(); i++)
	{
		if (cpu_coverage_[i] < min)
		{
			min = cpu_coverage_[i];
			ret = i;
		}
	}
	return ret;
}

void platform_runtime_manager::get_applications (api::query_result& result, const string_type& path)
{
	//we are heavy and we're doing it fast
	result.items.reserve(applications_.size());

	string_type buffer;

	for (const auto& one : applications_)
	{
		if (!path.empty())
		{
			one.second->meta_info().get_full_path_with_buffer(buffer);
			if (path.size() <= buffer.size())
			{
				size_t size = std::min(path.size(), buffer.size());
				// sorry guys i have to do it, i know I'm waisting a byte
				if (0 == memcmp(buffer.c_str(), path.c_str(), size))
				{
					result.items.emplace_back(api::query_result_detail{ rx_application, one.second->meta_info() });
				}
			}
		}
		else // path.empty()
		{
			result.items.emplace_back(api::query_result_detail{ rx_application, one.second->meta_info() });
		}
	}
}

runtime_handle_t platform_runtime_manager::get_new_handle ()
{
	static std::atomic<runtime_handle_t> g_handle(1);
	runtime_handle_t ret = g_handle++;
	if (!ret)// avoid zero
		return get_new_handle();
	else
		return ret;
}

runtime_transaction_id_t platform_runtime_manager::get_new_transaction_id ()
{
	static std::atomic<runtime_transaction_id_t> g_trans_id(1);
	runtime_transaction_id_t ret = g_trans_id++;
	if (!ret)// avoid zero
		return get_new_handle();
	else
		return ret;
}

void platform_runtime_manager::stop_all ()
{
	std::map<rx_thread_handle_t, std::vector<rx_application_ptr> > apps;
	std::map<rx_thread_handle_t, std::vector<rx_domain_ptr> > domains;
	for (auto& one : applications_)
	{
		apps[one.first].push_back(one.second);
		std::vector<rx_domain_ptr> temp = one.second->get_instance_data().get_domains();
		for (auto& one_dom : temp)
		{
			domains[one_dom->get_executer()].push_back(one_dom);
		}
	}

	locks::event sync_event(false);
	locks::event* psync_event = &sync_event;
	rx_reference_ptr dummy_ptr;

	for (auto& one : domains)
	{
		std::function<void(std::vector<rx_domain_ptr>, locks::event*)> func = [](std::vector<rx_domain_ptr> domains, locks::event* psync_event)
			{
				algorithms::shutdown_algorithms::stop_domains(std::move(domains));
				psync_event->set();
			};
		rx_post_function_to(one.first, dummy_ptr, std::move(func), std::move(one.second), std::move(psync_event));
		sync_event.wait_handle();
	}

	for (auto& one : apps)
	{
		std::function<void(std::vector<rx_application_ptr>, locks::event*)> func = [](std::vector<rx_application_ptr> apps, locks::event* psync_event)
			{
				algorithms::shutdown_algorithms::stop_applications(std::move(apps));
				psync_event->set();
			};
		rx_post_function_to(one.first, dummy_ptr, std::move(func), std::move(one.second), std::move(psync_event));
		sync_event.wait_handle();
	}
}

void platform_runtime_manager::deinitialize ()
{
	data_source::data_source_factory::instance().deinitialize();
	delete this;
}


// Class rx_internal::sys_runtime::runtime_cache 

runtime_cache::runtime_cache()
{
	g_runtime_lock = &lock_;
}



void runtime_cache::add_to_cache (platform_item_ptr&& item, collected_subscribers_type& subscribers)
{
	static std::function<void(const rx_node_id&)> g_dummy_f;

	string_type name = item->get_name();
	string_type path = item->meta_info().get_full_path();
	rx_node_id id = item->meta_info().id;

	auto it_path = path_cache_.find(path);
	if (it_path != path_cache_.end())
	{
		return;
	}
	auto it_id = id_cache_.find(id);
	if (it_id != id_cache_.end())
	{
		if (it_id->second.item)
		{
			RX_ASSERT(false);
			return;
		}
		if (it_id->second.register_f)
			it_id->second.register_f(item->meta_info().id);
		it_id->second.item = item->clone();
	}
	else
	{
		id_cache_.emplace(std::move(id), runtime_id_data{ item->clone(), g_dummy_f, g_dummy_f });
	}
	path_cache_.emplace(std::move(path), std::move(item));

	collect_subscribers(id, name, subscribers);
}

std::vector<platform_item_ptr> runtime_cache::get_items (const string_array& paths)
{
	std::vector<platform_item_ptr> ret;
	ret.reserve(paths.size());
	locks::auto_lock_t _(&lock_);
	for (const auto& path : paths)
	{
		auto it = path_cache_.find(path);
		if (it != path_cache_.end())
			ret.emplace_back(it->second->clone());
		else
			ret.emplace_back(platform_item_ptr());
	}
	return ret;
}

platform_item_ptr runtime_cache::get_item (const rx_node_id& id)
{
	locks::auto_lock_t _(&lock_);
	auto it = id_cache_.find(id);
	if (it != id_cache_.end() &&  it->second.item)
		return it->second.item->clone();
	else
		return platform_item_ptr();
}

void runtime_cache::remove_from_cache (platform_item_ptr&& item)
{
	using subs_list_t = std::vector< std::pair<runtime::resolvers::runtime_subscriber*, rx_reference_ptr> >;
	std::map<rx_thread_handle_t, subs_list_t> to_send;
	string_type name = item->get_name();
	rx_node_id id = item->meta_info().id;
	{
		locks::auto_lock_t _(&lock_);
		switch (item->get_type_id())
		{
		case rx_item_type::rx_application:
			applications_cache_.erase(id);
			break;
		case rx_item_type::rx_domain:
			domains_cache_.erase(id);
			break;
		case rx_item_type::rx_port:
			ports_cache_.erase(id);
			break;
		case rx_item_type::rx_object:
			objects_cache_.erase(id);
			break;
		case rx_item_type::rx_relation:
			relations_cache_.erase(id);
			break;
		default:
			RX_ASSERT(false);
		}
		auto it = path_cache_.find(item->meta_info().get_full_path());
		if (it != path_cache_.end())
			path_cache_.erase(it);
		auto it_id = id_cache_.find(item->meta_info().id);
		if (it_id != id_cache_.end())
		{
			if (it_id->second.deleter_f)
				it_id->second.deleter_f(item->meta_info().id);
			id_cache_.erase(it_id);
		}
		collect_subscribers(id, name, to_send);
	}
	if (!to_send.empty())
	{
		for (auto& for_one : to_send)
		{
			if (!for_one.second.empty())
			{
				auto ref = for_one.second.begin()->second;
				rx_post_function_to(for_one.first, ref,
					[id](subs_list_t data)
					{
                        for(auto& one : data)
                        {
							one.first->runtime_destroyed(id);
                        }
					}, std::move(for_one.second));
			}
		}
	}
}

void runtime_cache::add_functions (const rx_node_id& id, const std::function<void(const rx_node_id&)>& register_f, const std::function<void(const rx_node_id&)>& deleter_f)
{
	locks::auto_lock_t _(&lock_);
	auto it_id = id_cache_.find(id);
	if (it_id != id_cache_.end())
	{
		RX_ASSERT(false);
	}
	else
	{
		id_cache_.emplace(id, runtime_id_data{ platform_item_ptr(), register_f, deleter_f });
	}
}

void runtime_cache::remove_functions (const rx_node_id& id)
{
	locks::auto_lock_t _(&lock_);
	auto it_id = id_cache_.find(id);
	if (it_id != id_cache_.end())
	{
		id_cache_.erase(it_id);
	}
	else
	{
		RX_ASSERT(false);
	}
}

void runtime_cache::unregister_subscriber (const rx_item_reference& ref, runtime::resolvers::runtime_subscriber* whose)
{
	string_type name;
	if (!ref.is_node_id())
	{
		size_t idx = ref.get_path().rfind(RX_DIR_DELIMETER);
		if (idx != string_type::npos)
			name = ref.get_path().substr(idx + 1);
		else
			name = ref.get_path();
	}
	rx_node_id id;
	platform_item_ptr item;
	{
		locks::auto_lock_t _(&lock_);
		if (name.empty())
		{
			if (ref.is_node_id())
			{
				id = ref.get_node_id();
				auto it_subs = id_subscribers_.find(id);
				if (it_subs != id_subscribers_.end())
				{
					auto result = it_subs->second.erase(whose);
					RX_ASSERT(result);// has to be here
				}
				else
				{
					RX_ASSERT(false);// has to be here
				}
			}
		}
		else
		{
			auto it_subs = path_subscribers_.find(name);
			if (it_subs != path_subscribers_.end())
			{
				auto result = it_subs->second.erase(whose);
				RX_ASSERT(result);// has to be here
			}
			else
			{
				RX_ASSERT(false);// has to be here
			}
		}
	}
}

void runtime_cache::register_subscriber (const rx_item_reference& ref, runtime::resolvers::runtime_subscriber* whose)
{
	auto executer = rx_thread_context();
	string_type name;
	if (!ref.is_node_id())
	{
		size_t idx = ref.get_path().rfind(RX_DIR_DELIMETER);
		if (idx != string_type::npos)
			name = ref.get_path().substr(idx + 1);
		else
			name = ref.get_path();
	}
	rx_node_id id;
	platform_item_ptr item;
	{
		locks::auto_lock_t _(&lock_);
		if (name.empty())
		{
			if (ref.is_node_id())
			{
				id = ref.get_node_id();
				auto it_subs = id_subscribers_.find(id);
				if (it_subs != id_subscribers_.end())
				{
					it_subs->second.emplace(whose, subscriber_data{ executer, name, id });
				}
				else
				{
					id_subscribers_type::mapped_type subs;
					subs.emplace(whose, subscriber_data{ executer, name, id });
					id_subscribers_.emplace(id, std::move(subs));
				}
				auto it = id_cache_.find(id);
				if (it != id_cache_.end() && it->second.item)
					item = it->second.item->clone();
			}
		}
		else
		{
			auto it_subs = path_subscribers_.find(name);
			if (it_subs != path_subscribers_.end())
			{
				it_subs->second.emplace(whose, subscriber_data{ executer, name, id });
			}
			else
			{
				id_subscribers_type::mapped_type subs;
				subs.emplace(whose, subscriber_data{ executer, name, id });
				path_subscribers_.emplace(std::move(name), std::move(subs));
			}
		}
	}
	rx_post_function_to(executer, whose->get_reference(), [whose](platform_item_ptr item)
		{
			whose->runtime_appeared(std::move(item));
		}
		, item ? std::move(item) : platform_item_ptr());
}

void runtime_cache::collect_subscribers (const rx_node_id& id, const string_type& name, std::map<rx_thread_handle_t, subs_list_t>& to_send)
{
	auto it_id_subs = id_subscribers_.find(id);
	if (it_id_subs != id_subscribers_.end())
	{
		for (auto& one : it_id_subs->second)
		{
			to_send[one.second.target].emplace_back(one.first, one.first->get_reference());
		}
	}
	auto it_name_subs = path_subscribers_.find(name);
	if (it_name_subs != path_subscribers_.end())
	{
		for (auto& one : it_name_subs->second)
		{
			to_send[one.second.target].emplace_back(one.first, one.first->get_reference());
		}
	}
}

rx_object_ptr runtime_cache::get_object (const rx_node_id& id)
{
	locks::auto_lock_t _(&lock_);
	auto it = objects_cache_.find(id);
	if (it != objects_cache_.end())
		return it->second;
	else
		return rx_object_ptr::null_ptr;
}

rx_application_ptr runtime_cache::get_application (const rx_node_id& id)
{
	locks::auto_lock_t _(&lock_);
	auto it = applications_cache_.find(id);
	if (it != applications_cache_.end())
		return it->second;
	else
		return rx_application_ptr::null_ptr;
}

rx_domain_ptr runtime_cache::get_domain (const rx_node_id& id)
{
	locks::auto_lock_t _(&lock_);
	auto it = domains_cache_.find(id);
	if (it != domains_cache_.end())
		return it->second;
	else
		return rx_domain_ptr::null_ptr;
}

rx_port_ptr runtime_cache::get_port (const rx_node_id& id)
{
	locks::auto_lock_t _(&lock_);
	auto it = ports_cache_.find(id);
	if (it != ports_cache_.end())
		return it->second;
	else
		return rx_port_ptr::null_ptr;
}

void runtime_cache::add_to_cache (rx_object_ptr item)
{
	collected_subscribers_type subscribers;
	{
		locks::auto_lock_t _(&lock_);
		add_to_cache(item->get_item_ptr(), subscribers);
		objects_cache_.emplace(item->meta_info().id, item);
	}
	post_appeared(subscribers);
}

void runtime_cache::add_to_cache (rx_domain_ptr item)
{
	collected_subscribers_type subscribers;
	{
		locks::auto_lock_t _(&lock_);
		add_to_cache(item->get_item_ptr(), subscribers);
		domains_cache_.emplace(item->meta_info().id, item);
	}
	post_appeared(subscribers);
}

void runtime_cache::add_to_cache (rx_port_ptr item)
{
	collected_subscribers_type subscribers;
	{
		locks::auto_lock_t _(&lock_);
		add_to_cache(item->get_item_ptr(), subscribers);
		ports_cache_.emplace(item->meta_info().id, item);
	}
	post_appeared(subscribers);
}

void runtime_cache::add_to_cache (rx_application_ptr item)
{
	collected_subscribers_type subscribers;
	{
		locks::auto_lock_t _(&lock_);
		add_to_cache(item->get_item_ptr(), subscribers);
		applications_cache_.emplace(item->meta_info().id, item);
	}
	post_appeared(subscribers);
}

void runtime_cache::post_appeared (collected_subscribers_type& subscribers)
{
	if (!subscribers.empty())
	{
		for (auto& for_one : subscribers)
		{
			if (!for_one.second.empty())
			{
				auto ref = for_one.second.begin()->second;
				rx_post_function_to(for_one.first, ref,
					[](subs_list_t data)
				{
					for (auto& one : data)
					{
						one.first->runtime_appeared(platform_item_ptr());
					}
				}, std::move(for_one.second));
			}
		}
	}
}

rx_result runtime_cache::add_target_relation (const rx_node_id& id, relations::relation_data::smart_ptr data)
{
	rx_result result;
	lock_.lock();
	auto it = id_cache_.find(id);
	if (it != id_cache_.end() && it->second.item)
	{
		switch (it->second.item->get_type_id())
		{
		case rx_item_type::rx_application:
			{
				auto rt_it = applications_cache_.find(id);
				if (rt_it != applications_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->add_target_relation(data);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		case rx_item_type::rx_domain:
			{
				auto rt_it = domains_cache_.find(id);
				if (rt_it != domains_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->add_target_relation(data);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		case rx_item_type::rx_port:
			{
				auto rt_it = ports_cache_.find(id);
				if (rt_it != ports_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->add_target_relation(data);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		case rx_item_type::rx_object:
			{
				auto rt_it = objects_cache_.find(id);
				if (rt_it != objects_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->add_target_relation(data);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		default:
			lock_.unlock();
			result = "Not found in cache!";
			RX_ASSERT(false);
		}
	}
	else
	{
		lock_.unlock();
		result = "Not found in cache!";
	}
	return result;
}

rx_result runtime_cache::remove_target_relation (const rx_node_id& id, const string_type& name)
{
	rx_result result;
	lock_.lock();
	auto it = id_cache_.find(id);
	if (it != id_cache_.end() && it->second.item)
	{
		switch (it->second.item->get_type_id())
		{
		case rx_item_type::rx_application:
			{
				auto rt_it = applications_cache_.find(id);
				if (rt_it != applications_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->remove_target_relation(name);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		case rx_item_type::rx_domain:
			{
				auto rt_it = domains_cache_.find(id);
				if (rt_it != domains_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->remove_target_relation(name);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		case rx_item_type::rx_port:
			{
				auto rt_it = ports_cache_.find(id);
				if (rt_it != ports_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->remove_target_relation(name);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		case rx_item_type::rx_object:
			{
				auto rt_it = objects_cache_.find(id);
				if (rt_it != objects_cache_.end())
				{
					auto rt_ptr = rt_it->second;
					lock_.unlock();
					result = rt_ptr->remove_target_relation(name);
				}
				else
				{
					lock_.unlock();
					result = "Critical stuff!";
					RX_ASSERT(false);
				}
			}
			break;
		default:
			lock_.unlock();
			result = "Not found in cache!";
			RX_ASSERT(false);
		}
	}
	else
	{
		lock_.unlock();
		// not found in cache, so it is lost anyway
		// I'll just return true!!!

		result = true;
		//result = "Not found in cache!"; // old stuff, seems not right
	}
	return result;
}

std::vector<platform_item_ptr> runtime_cache::get_items (const rx_node_ids& ids)
{
	std::vector<platform_item_ptr> ret;
	ret.reserve(ids.size());
	locks::auto_lock_t _(&lock_);
	for (const auto& id : ids)
	{
		auto it = id_cache_.find(id);
		if (it != id_cache_.end())
			ret.emplace_back(it->second.item->clone());
		else
			ret.emplace_back(platform_item_ptr());
	}
	return ret;
}

void runtime_cache::cleanup_cache ()
{
	applications_cache_.clear();
	domains_cache_.clear();
	objects_cache_.clear();
	ports_cache_.clear();

	id_cache_.clear();
	path_cache_.clear();

	id_subscribers_.clear();
	path_subscribers_.clear();
}

rx_relation_ptr runtime_cache::get_relation (const rx_node_id& id)
{
	locks::auto_lock_t _(&lock_);
	auto it = relations_cache_.find(id);
	if (it != relations_cache_.end())
		return it->second;
	else
		return rx_relation_ptr::null_ptr;
}

void runtime_cache::add_to_cache (rx_relation_ptr item)
{
	collected_subscribers_type subscribers;
	{
		locks::auto_lock_t _(&lock_);
		add_to_cache(item->get_item_ptr(), subscribers);
		relations_cache_.emplace(item->meta_info().id, item);
	}
	post_appeared(subscribers);
}


} // namespace sys_runtime
} // namespace rx_internal

