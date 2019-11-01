

/****************************************************************************
*
*  api\rx_namespace_api.h
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


#ifndef rx_namespace_api_h
#define rx_namespace_api_h 1




#include "rx_platform_api.h"
#include "system/server/rx_ns.h"
#include "system/meta/rx_queries.h"

namespace rx_platform
{


namespace api
{
namespace ns
{
rx_result_with<rx_node_id> rx_resolve_reference(
	const item_reference& ref
	, rx_directory_resolver& directories);

template<typename typeT>
rx_result_with<rx_node_id> rx_resolve_type_reference(
	const item_reference& ref
	, rx_directory_resolver& directories
	, tl::type2type<typeT> _);

template<typename typeT>
rx_result_with<rx_node_id> rx_resolve_simple_type_reference(
	const item_reference& ref
	, rx_directory_resolver& directories
	, tl::type2type<typeT> _);

template<typename typeT>
rx_result_with<rx_node_id> rx_resolve_runtime_reference(
	const item_reference& ref
	, rx_directory_resolver& directories
	, tl::type2type<typeT> _);

rx_result rx_get_directory(
	const string_type& name // directories's path
	, std::function<void(rx_result_with<rx_directory_ptr>&&)> callback
	, rx_context ctx);

rx_result rx_get_items(
	const string_array& name // directories's path
	, std::function<void(std::vector<rx_result_with<platform_item_ptr> >)> callback
	, rx_context ctx);

struct directory_browse_result
{
	platform_directories_type directories;
	platform_items_type items;

	bool success = false;
	operator bool() const
	{
		return success;
	}
};

rx_result rx_list_directory(
	const string_type& name // directory's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<directory_browse_result>&&)> callback
	, rx_context ctx);


template<typename typeT>
rx_result rx_list_runtime(
	rx_node_id id
	,const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx, tl::type2type<typeT>);

rx_result rx_list_runtime_from_path(
	const string_type& path // item's path
	, const string_type& pattern // search pattern
	, std::function<void(rx_result_with<runtime_browse_result>&&)> callback
	, rx_context ctx);

rx_result rx_query_model(
	std::vector<rx_platform::meta::query_ptr> queries
	, std::function<void(rx_result_with<query_result>&&)> callback
	, rx_context ctx);

}
}
}




#endif
