

/****************************************************************************
*
*  api\rx_meta_api.cpp
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

#include "rx_platform.h"

// rx_meta_api
#include "api/rx_meta_api.h"

#include "system/meta/rx_meta_data.h"
#include "model/rx_meta_internals.h"

namespace rx_platform
{
namespace api
{
namespace meta
{

rx_result rx_delete_object(const string_type& name
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	model::platform_types_manager::instance().delete_runtime<object_type, pointers::reference_object::smart_ptr>(
		name, ctx.directory, callback, ctx.object);
	return true;
}

rx_result rx_create_object(const string_type& name, const string_type& type_name, data::runtime_values_data* init_data
	, ns::namespace_item_attributes attributes, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx)
{
	data::runtime_values_data* ptr_copy = nullptr;
	if(init_data)// copy values to resolve lifetime
		ptr_copy = new data::runtime_values_data(std::move(*init_data));
	model::platform_types_manager::instance().create_runtime<object_type, pointers::reference_object::smart_ptr>(
		name, type_name, ptr_copy, ctx.directory, attributes, callback, ctx.object);
	return true;
}

rx_result rx_create_port(const string_type& name, const string_type& type_name, data::runtime_values_data* init_data
	, ns::namespace_item_attributes attributes, std::function<void(rx_result_with<rx_port_ptr>&&)> callback, rx_context ctx)
{
	data::runtime_values_data* ptr_copy = nullptr;
	if (init_data)// copy values to resolve lifetime
		ptr_copy = new data::runtime_values_data(std::move(*init_data));
	model::platform_types_manager::instance().create_runtime<port_type, pointers::reference_object::smart_ptr>(
		name, type_name, ptr_copy, ctx.directory, attributes, callback, ctx.object);
	return true;
}

rx_result rx_create_prototype(const string_type& name, const rx_node_id& instance_id, const string_type& type_name
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx)
{
	model::platform_types_manager::instance().create_prototype<object_type, pointers::reference_object::smart_ptr>(
		name, instance_id, type_name, ctx.directory, ns::namespace_item_attributes::namespace_item_null, callback, ctx.object);
	return true;
}

rx_result rx_create_object_type(const string_type& name
	, const string_type& base_name, rx_object_type_ptr prototype, ns::namespace_item_attributes attributes
	, std::function<void(rx_result_with<rx_object_type_ptr>&&)> callback, rx_context ctx)
{
	model::platform_types_manager::instance().create_type<object_type, pointers::reference_object::smart_ptr>(
		name, base_name, prototype, ctx.directory, attributes| ns::namespace_item_attributes::namespace_item_full_type_access, callback, ctx.object);
	return true;
}
rx_result recursive_save_directory(rx_directory_ptr dir)
{
	platform_directories_type dirs;
	platform_items_type items;
	dir->get_content(dirs, items, "");
	for (auto& dir : dirs)
	{
		auto result = recursive_save_directory(dir);
		if (!result)
			return result;
	}
	for (auto& item : items)
	{
		auto result = item->save();
		if (!result)
			return result;
	}
	return true;
}
// save functionalities
rx_result save_item_helper(const string_type& name, rx_directory_ptr dir)
{
	// check to see if we are to save whole directory?
	auto dir_ptr = dir->get_sub_directory(name);
	if (dir_ptr)
	{// this is a directory do recursive save!!!
		return recursive_save_directory(dir_ptr);
	}
	else
	{// just plain item save
		auto item = dir->get_sub_item(name);
		if (!item)
		{
			return name + " does not exists!";
		}
		return item->save();
	}
}

rx_result rx_save_item(const string_type& name
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	std::function<rx_result()> func = [=]() {
		return save_item_helper(name, ctx.directory);
	};
	rx_do_with_callback<rx_result, rx_object_ptr>(func, RX_DOMAIN_META, callback, ctx.object);
	
	return true;
}


}
}
}


