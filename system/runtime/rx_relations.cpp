

/****************************************************************************
*
*  system\runtime\rx_relations.cpp
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


// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"
// rx_relations
#include "system/runtime/rx_relations.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_internal_ns.h"
#include "rx_objbase.h"
#include "model/rx_model_algorithms.h"
#include "system/server/rx_async_functions.h"
#include "runtime_internal/rx_relations_runtime.h"
using namespace sys_runtime::relations_runtime;


namespace rx_platform {

namespace runtime {

namespace relations {

// Class rx_platform::runtime::relations::relation_runtime 

string_type relation_runtime::type_name = RX_CPP_RELATION_TYPE_NAME;

rx_item_type relation_runtime::type_id = rx_item_type::rx_relation;

relation_runtime::relation_runtime()
      : executer_(0),
        runtime_handle(0)
{
}

relation_runtime::relation_runtime (const string_type& name, const rx_node_id& id, bool system)
      : executer_(0),
        runtime_handle(0)
{
}


relation_runtime::~relation_runtime()
{
}



string_type relation_runtime::get_type_name () const
{
  return type_name;

}

rx_result relation_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result relation_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result relation_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	executer_ = rx_thread_context();
	try_resolve();
	return true;
}

rx_result relation_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	my_state_ = relation_state::relation_state_stopping;
	return true;
}

void relation_runtime::fill_data (const data::runtime_values_data& data)
{
	structure::init_context ctx;
	auto it = data.values.find(name);
	if (it != data.values.end())
	{
		target = it->second.value.get_storage().get_string_value();
	}
	rx_simple_value val;
	val.assign_static(target);
	value.simple_set_value(std::move(val));
}

void relation_runtime::collect_data (data::runtime_values_data& data) const
{
	rx_simple_value temp;
	temp.assign_static<string_type>(string_type(target));
	data.add_value(name, std::move(temp));
}

rx_result relation_runtime::read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relation_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return RX_NOT_IMPLEMENTED;
}

meta::meta_data& relation_runtime::meta_info ()
{
  return meta_info_;

}

void relation_runtime::try_resolve ()
{
	if (my_state_ != relation_state::relation_state_idle)
		return;// not in right state
	if (target.empty())
		return;// do nothing it's blank
	rx_directory_resolver dirs;
	dirs.add_paths({object_directory});
	auto resolve_result = model::algorithms::resolve_reference(target, dirs);
	if (!resolve_result)
	{
		RUNTIME_LOG_ERROR("relation_runtime", 100, "Unable to resolve relation reference to "s + target);
		return;
	}
	rx_node_id id = resolve_result.move_value();
	my_state_ = relation_state::relation_state_querying;
	std::function<rx_result_with<platform_item_ptr>()> func = [id]
	{
		auto result = model::algorithms::get_working_runtime_sync(id);
		return result;
	};
	rx_do_with_callback<rx_result_with<platform_item_ptr>, rx_reference_ptr>(
		func, RX_DOMAIN_META, [this](rx_result_with<platform_item_ptr>&& result)
		{
			if (my_state_ != relation_state::relation_state_querying)
				return;
			if (result)
			{
				auto&& item_ptr = result.move_value();
				if (item_ptr->get_executer() == executer_)
				{
					my_state_ = relation_state::relation_state_same_domain;
					connector_ = std::make_unique<local_relation_connector>(std::move(item_ptr));
					return;
				}
				else
				{
					RUNTIME_LOG_ERROR("relation_runtime", 999, "Not supported type, local domain for "s + target);
				}
			}
			my_state_ = relation_state::relation_state_idle;
		}, smart_this());
}

rx_result relation_runtime::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	if (connector_)
	{
		auto result = connector_->browse(prefix, path, filter, items);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result relation_runtime::read_tag (runtime_handle_t item, operational::tags_callback_ptr monitor, const structure::hosting_object_data& state)
{
	if (connector_)
	{
		auto result = connector_->read_tag(item, monitor, state);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result relation_runtime::write_tag (runtime_handle_t item, rx_simple_value&& value, operational::tags_callback_ptr monitor, const structure::hosting_object_data& state)
{
	if (connector_)
	{
		auto result = connector_->write_tag(item, std::move(value), monitor, state);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result_with<runtime_handle_t> relation_runtime::connect_tag (const string_type& path, tags_callback_ptr monitor, const structure::hosting_object_data& state)
{
	if (connector_)
	{
		auto result = connector_->connect_tag(path, monitor, state);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result relation_runtime::disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor)
{
	if (connector_)
	{
		auto result = connector_->disconnect_tag(handle, monitor);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}


// Class rx_platform::runtime::relations::relation_instance_data 


// Class rx_platform::runtime::relations::relation_connector 

relation_connector::~relation_connector()
{
}



} // namespace relations
} // namespace runtime
} // namespace rx_platform

