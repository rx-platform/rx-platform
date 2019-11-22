

/****************************************************************************
*
*  system\runtime\rx_relations.cpp
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


// rx_operational
#include "system/runtime/rx_operational.h"
// rx_relations
#include "system/runtime/rx_relations.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_internal_ns.h"
#include "rx_objbase.h"


namespace rx_platform {

namespace runtime {

namespace relations {

// Class rx_platform::runtime::relations::relation_runtime 

string_type relation_runtime::type_name = RX_CPP_RELATION_TYPE_NAME;

rx_item_type relation_runtime::type_id = rx_item_type::rx_relation;

relation_runtime::relation_runtime()
{
}

relation_runtime::relation_runtime (const string_type& name, const rx_node_id& id, bool system)
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
	return RX_NOT_IMPLEMENTED;
}

rx_result relation_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relation_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relation_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

void relation_runtime::fill_data (const data::runtime_values_data& data)
{
	structure::init_context ctx;
	auto it = data.values.find(name);
	if (it != data.values.end())
	{
		target = it->second.value.get_storage().get_string_value();
	}
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

rx_result relation_runtime::connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return objects::object_runtime_algorithms<meta::object_types::relation_type>::connect_items(paths, callback, monitor, ctx, this);
}

meta::meta_data& relation_runtime::meta_info ()
{
  return meta_info_;

}


// Class rx_platform::runtime::relations::relation_instance_data 


} // namespace relations
} // namespace runtime
} // namespace rx_platform

