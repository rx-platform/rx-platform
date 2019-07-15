

/****************************************************************************
*
*  api\rx_namespace_api.cpp
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


// rx_namespace_api
#include "api/rx_namespace_api.h"

#include "system/meta/rx_meta_data.h"
#include "model/rx_meta_internals.h"
#include "system/server/rx_async_functions.h"

namespace rx_platform
{
namespace api
{
namespace ns
{


rx_result rx_get_directory(const string_type& name // directory's path
	, std::function<void(rx_result_with<rx_directory_ptr>&&)> callback
	, rx_context ctx)
{
	return true;
}


rx_result rx_get_items(const string_array& names // item's path
	, std::function<void(std::vector<rx_result_with<platform_item_ptr> >)> callback
	, rx_context ctx)
{
	std::function<std::vector<rx_result_with<platform_item_ptr> >(const string_array, rx_directory_ptr)> func = [=](const string_array names, rx_directory_ptr dir) mutable -> std::vector<rx_result_with<platform_item_ptr> > {
		std::vector<rx_result_with<platform_item_ptr> > ret;
		rx_directory_ptr from = dir ? dir : rx_gate::instance().get_root_directory();
		for (const auto& path : names)
		{
			platform_item_ptr who = from->get_sub_item(path);
			if (who)
			{
				ret.emplace_back(who);
			}
			else
			{
				ret.emplace_back(path + " not found!");
			}
		}
		return ret;
	};
	rx_do_with_callback<std::vector<rx_result_with<platform_item_ptr> >, rx_reference_ptr, string_array, rx_directory_ptr>(func, RX_DOMAIN_META, callback, ctx.object, names, ctx.directory);
	return true;
}

rx_result rx_list_directory(const string_type& name // directory's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<directory_browse_result>&&)> callback
	, rx_context ctx)
{
	std::function<rx_result_with<directory_browse_result>(const string_type, rx_directory_ptr)> func = [=](const string_type path, rx_directory_ptr dir) mutable -> rx_result_with<directory_browse_result> {
		directory_browse_result ret_val;
		rx_directory_ptr from = dir ? dir : rx_gate::instance().get_root_directory();
		rx_directory_ptr who = from->get_sub_directory(path);
		if (who)
		{
			who->get_content(ret_val.directories, ret_val.items, pattern);
			ret_val.success = true;
			return ret_val;
		}
		else
		{
			return (path + " not found!");
		}
	};
	rx_do_with_callback<rx_result_with<directory_browse_result>, rx_reference_ptr, string_type, rx_directory_ptr>(func, RX_DOMAIN_META, callback, ctx.object, name, ctx.directory);
	return true;
}


// !!!!!!!!rx_list_runtime has to be with two hoops
template<typename typeT>
rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx, tl::type2type<typeT>)
{
	std::function<rx_result_with<runtime_browse_result>(const string_type, const string_type, rx_node_id)> func = [](const string_type path, const string_type pattern, rx_node_id id) mutable -> rx_result_with<runtime_browse_result> {
		runtime_browse_result ret_val;
		auto rt_item = model::platform_types_manager::instance().internal_get_type_cache<typeT>().get_runtime(id);
		if (!rt_item)
			return "Runtime not found";
		auto result = rt_item->browse(path, pattern, ret_val.items);
		if (!result)
			return result.errors();
		ret_val.success = true;
		return ret_val;
	};
	rx_do_with_callback<rx_result_with<runtime_browse_result>, rx_reference_ptr, string_type, string_type, rx_node_id>(func, RX_DOMAIN_META, callback, ctx.object, path, pattern, id);
	return true;
}
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx, tl::type2type<object_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx, tl::type2type<port_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx, tl::type2type<domain_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx, tl::type2type<application_type>);



rx_result rx_list_runtime_from_path(
	const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx)
{
	std::function<rx_result_with<runtime_browse_result>(const string_type, const string_type, rx_directory_ptr)> func = [](const string_type path, const string_type pattern, rx_directory_ptr dir) mutable -> rx_result_with<runtime_browse_result> {
		runtime_browse_result ret_val;
		platform_item_ptr item = platform_item_ptr::null_ptr;
		size_t idx = path.rfind(RX_DIR_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{
			item = dir->get_sub_item(path);
		}
		else
		{
			item = dir->get_sub_item(path.substr(idx+1));
		}
		if (!item)
			return "Runtime not found";
		auto result = item->browse(path, pattern, ret_val.items);
		if (!result)
			return result.errors();
		ret_val.success = true;
		return ret_val;
	};
	rx_do_with_callback<rx_result_with<runtime_browse_result>, rx_reference_ptr, string_type, string_type, rx_directory_ptr>
		(func, RX_DOMAIN_META, callback, ctx.object, path, pattern, ctx.directory ? ctx.directory : rx_gate::instance().get_root_directory());
	return true;
}


rx_result rx_query_model(std::vector<meta::query_ptr> queries
	, std::function<void(rx_result_with<query_result>&&)> callback
	, rx_context ctx)
{
	std::function<rx_result_with<query_result>(std::vector<meta::query_ptr>, rx_directory_ptr)> func = [=]
	(std::vector<rx_platform::meta::query_ptr> queries, rx_directory_ptr dir) mutable -> rx_result_with<query_result> {

		query_result ret_val;
		rx_directory_ptr from = dir ? dir : rx_gate::instance().get_root_directory();

		size_t count = queries.size();
		std::vector<std::vector<query_result_detail> > results(count, std::vector<query_result_detail>());

		for (size_t i = 0; i < count; i++)
		{
			query_result temp;
			auto one_ret = queries[i]->do_query(temp, from);
			if (!one_ret)
				return one_ret.errors();
			std::copy(temp.items.begin(), temp.items.end(), std::back_inserter(ret_val.items));
		}
		ret_val.success = true;
		return ret_val;
	};
	rx_do_with_callback<rx_result_with<query_result>, rx_reference_ptr, std::vector<meta::query_ptr>, rx_directory_ptr>(func, RX_DOMAIN_META, callback, ctx.object, queries, ctx.directory);

	return true;
}



}
}
}


