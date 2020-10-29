

/****************************************************************************
*
*  system\runtime\rx_relation_impl.cpp
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


// rx_relation_impl
#include "system/runtime/rx_relation_impl.h"

#include "model/rx_model_algorithms.h"
#include "runtime_internal/rx_runtime_internal.h"
using namespace rx_internal::sys_runtime;


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

rx_result relation_runtime::initialize_relation (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result relation_runtime::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result relation_runtime::start_relation (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result relation_runtime::stop_relation (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_result_with<platform_item_ptr> relation_runtime::resolve_runtime_sync (const rx_node_id& id)
{
	auto item_ptr = platform_runtime_manager::instance().get_cache().get_item(id);
	if (!item_ptr)
		return "Not a running instance!";
	return item_ptr;
}

void relation_runtime::relation_connected ()
{
}

void relation_runtime::relation_disconnected ()
{
}

rx_item_reference relation_runtime::get_implicit_reference (const meta::meta_data& info)
{
	return rx_item_reference::null_ref;
}

relation_runtime::smart_ptr relation_runtime::make_target_relation ()
{
	return rx_create_reference< relation_runtime>();
}


} // namespace relations
} // namespace runtime
} // namespace rx_platform



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	return false;

#endif
