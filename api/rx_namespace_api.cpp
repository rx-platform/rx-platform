

/****************************************************************************
*
*  api\rx_namespace_api.cpp
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


// rx_namespace_api
#include "api/rx_namespace_api.h"

#include "system/meta/rx_meta_data.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "system/server/rx_ns.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_async_functions.h"

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
	rx_directory_ptr from = ctx.directory ? ctx.directory : rx_gate::instance().get_root_directory();
	rx_directory_ptr who = from->get_sub_directory(name);
	if (who)
	{
		who->get_content(ret_val.directories, ret_val.items, pattern);
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
	, rx_result_with_callback<runtime_browse_result>&& callback
	, rx_context ctx, tl::type2type<typeT>)
{
	auto result = rx_internal::model::algorithms::do_with_runtime_item<runtime_browse_result>(id, 
		[=] (rx_result_with<platform_item_ptr>&& item) -> rx_result_with<runtime_browse_result>
		{
			runtime_browse_result items;
			if (item)
			{
				auto result = item.value()->browse("", path, pattern, items.items);
				if (result)
				{
					items.success = result;
				}
				else
				{
					return result.errors();
				}
			}
			else
			{
				return item.errors();
			}
			return items;
		}, std::move(callback), ctx);

	return result;
}
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, rx_result_with_callback<runtime_browse_result>&& callback
	, rx_context ctx, tl::type2type<object_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, rx_result_with_callback<runtime_browse_result>&& callback
	, rx_context ctx, tl::type2type<port_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, rx_result_with_callback<runtime_browse_result>&& callback
	, rx_context ctx, tl::type2type<domain_type>);
template rx_result rx_list_runtime(
	rx_node_id id
	, const string_type& path // item's path
	, const string_type& pattern // search pattern
	, rx_result_with_callback<runtime_browse_result>&& callback
	, rx_context ctx, tl::type2type<application_type>);



rx_result rx_query_model(std::vector<meta::query_ptr> queries
	, rx_result_with_callback<query_result>&& callback
	, rx_context ctx)
{
	rx_do_with_callback(RX_DOMAIN_META, ctx.object, [] (std::vector<rx_platform::meta::query_ptr> queries, rx_directory_ptr dir) -> rx_result_with<query_result>
		{

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

		}, std::move(callback), queries, ctx.directory);

	return true;
}



}
}
}


