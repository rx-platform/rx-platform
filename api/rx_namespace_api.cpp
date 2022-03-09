

/****************************************************************************
*
*  api\rx_namespace_api.cpp
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


// rx_namespace_api
#include "api/rx_namespace_api.h"

#include "system/meta/rx_meta_data.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "system/server/rx_ns.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_async_functions.h"
#include "system/server/rx_directory_cache.h"

namespace rx_platform
{
namespace api
{
namespace ns
{

rx_result_with<rx_node_id> rx_resolve_reference(const rx_item_reference& ref, const rx_directory_resolver& directories)
{
	return rx_internal::model::algorithms::resolve_reference(ref, directories);
}


meta_data rx_resolve_reference(const rx_item_reference& ref, rx_item_type& type,  const rx_directory_resolver& directories)
{
	return rx_internal::model::algorithms::resolve_reference(ref, type, directories);
}



template<typename typeT>
rx_result_with<rx_node_id> rx_resolve_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<typeT> _)
{
	return rx_internal::model::algorithms::resolve_type_reference<typeT>(ref, directories, tl::type2type<typeT>());
}
template rx_result_with<rx_node_id> rx_resolve_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<object_type>);
template rx_result_with<rx_node_id> rx_resolve_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<application_type>);
template rx_result_with<rx_node_id> rx_resolve_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<port_type>);
template rx_result_with<rx_node_id> rx_resolve_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<domain_type>);


template<typename typeT>
rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<typeT>)
{
	return rx_internal::model::algorithms::resolve_simple_type_reference<typeT>(ref, directories, tl::type2type<typeT>());
}
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<struct_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<variable_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<source_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<event_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<filter_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<mapper_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<program_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<method_type>);
template rx_result_with<rx_node_id> rx_resolve_simple_type_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<display_type>);


rx_result_with<rx_node_id> rx_resolve_relation_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories)
{
	return rx_internal::model::algorithms::resolve_relation_reference(ref, directories);

}

rx_result_with<rx_node_id> rx_resolve_data_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories)
{
	return rx_internal::model::algorithms::resolve_data_type_reference(ref, directories);

}

template<typename typeT>
rx_result_with<rx_node_id> rx_resolve_runtime_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<typeT>)
{
	return rx_internal::model::algorithms::resolve_runtime_reference<typeT>(ref, directories, tl::type2type<typeT>());
}

template rx_result_with<rx_node_id> rx_resolve_runtime_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<object_type>);
template rx_result_with<rx_node_id> rx_resolve_runtime_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<domain_type>);
template rx_result_with<rx_node_id> rx_resolve_runtime_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<port_type>);
template rx_result_with<rx_node_id> rx_resolve_runtime_reference(const rx_item_reference& ref
	, const rx_directory_resolver& directories, tl::type2type<application_type>);



rx_result_with<directory_browse_result> rx_list_directory(const string_type& name // directory's path
	, const string_type& pattern // search pattern
	, rx_context ctx)
{

	directory_browse_result ret_val;
	rx_platform::ns::rx_directory_resolver dirs;
	dirs.add_paths({ ctx.active_path });
	rx_directory_ptr who = rx_gate::instance().get_directory(name, &dirs);
	if (who)
	{
		who->list_content(ret_val.directories, ret_val.items, pattern);
		ret_val.success = true;
		return ret_val;
	}
	else
	{
		return (name + " not found!");
	}
}

void do_recursive_list(rx_directory_ptr who, std::vector<rx_namespace_item>& items, platform_directories_type& dirs, const string_type& pattern)
{
	directory_browse_result ret_val;
	who->list_content(ret_val.directories, ret_val.items, pattern);
	for (const auto& one : ret_val.items)
		items.emplace_back(one);
	for (auto sub : ret_val.directories)
	{
		dirs.emplace_back(sub);
		do_recursive_list(sub, items, dirs, pattern);
	}
}

rx_result_with<directory_browse_result> rx_recursive_list_items(const string_type& name // directory's path
	, const string_type& pattern // search pattern
	, rx_context ctx)
{
	directory_browse_result ret_val;
	rx_platform::ns::rx_directory_resolver dirs;
	dirs.add_paths({ ctx.active_path });
	rx_directory_ptr who = rx_gate::instance().get_directory(name, &dirs);
	if (who)
	{
		do_recursive_list(who, ret_val.items, ret_val.directories, pattern);
		ret_val.success = true;
		return ret_val;
	}
	else
	{
		return (name + " not found!");
	}
}

// !!!!!!!!rx_list_runtime has to be with two hoops
template<typename typeT>
rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, browse_result_callback_t&& callback
	, rx_context ctx, tl::type2type<typeT>)
{
	auto result = rx_internal::model::algorithms::do_with_runtime_item(id,
		[path=path, pattern=pattern, callback=std::move(callback)] (platform_item_ptr&& item) mutable
		{
			if (item)
			{
				item->browse("", path, pattern, std::move(callback));
			}
			else
			{
				callback("Item not found", std::vector<runtime_item_attribute>());
			}
		}, ctx);

	return result;
}
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, browse_result_callback_t&& callback
	, rx_context ctx, tl::type2type<object_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, browse_result_callback_t&& callback
	, rx_context ctx, tl::type2type<port_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, browse_result_callback_t&& callback
	, rx_context ctx, tl::type2type<domain_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, browse_result_callback_t&& callback
	, rx_context ctx, tl::type2type<application_type>);



rx_result rx_query_model(std::vector<meta::query_ptr> queries
	, rx_result_with_callback<query_result>&& callback
	, rx_context ctx)
{

	rx_post_function_to(RX_DOMAIN_META, ctx.object, [] (rx_result_with_callback<query_result>&& callback, std::vector<rx_platform::meta::query_ptr> queries, const string_type from)
		{

			query_result ret_val;

			size_t count = queries.size();
			std::vector<std::vector<query_result_detail> > results(count, std::vector<query_result_detail>());

			for (size_t i = 0; i < count; i++)
			{
				query_result temp;
				auto one_ret = queries[i]->do_query(temp, from);
				if (!one_ret)
				{
					callback(one_ret.errors());
					return;
				}
				std::copy(temp.items.begin(), temp.items.end(), std::back_inserter(ret_val.items));
			}
			ret_val.success = true;
			callback(std::move(ret_val));

		}, std::move(callback), queries, ctx.active_path);

	return true;
}



}
}
}


