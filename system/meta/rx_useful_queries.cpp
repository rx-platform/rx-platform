

/****************************************************************************
*
*  system\meta\rx_useful_queries.cpp
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


// rx_useful_queries
#include "system/meta/rx_useful_queries.h"

#include "model/rx_meta_internals.h"


namespace rx_platform {

namespace meta {

namespace queries {

// Class rx_platform::meta::queries::ns_suggetions_query 

string_type ns_suggetions_query::query_name = "suggestion";


rx_result ns_suggetions_query::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("typeName", type_name))
		return stream.get_error();
	if (!stream.write_string("instanceName", instance_name))
		return stream.get_error();
	if (!stream.write_id("instance", instance))
		return stream.get_error();
	if (!stream.write_string("subfolder", suggested_path))
		return stream.get_error();

	return true;
}

rx_result ns_suggetions_query::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("typeName", type_name))
		return stream.get_error();
	if (!stream.read_string("instanceName", instance_name))
		return stream.get_error();
	if (!stream.read_id("instance", instance))
		return stream.get_error();
	if (!stream.read_string("suggestion", suggested_path))
		return stream.get_error();

	return true;
}

const string_type& ns_suggetions_query::get_query_type ()
{
  return query_name;

}

rx_result ns_suggetions_query::do_query (api::query_result& result, rx_directory_ptr dir)
{
	auto type = rx_parse_type_name(type_name);
	rx_directory_ptr target_dir;
	string_type local_stuff;
	size_t idx = suggested_path.rfind(RX_DIR_DELIMETER);

	if (idx == string_type::npos)
	{
		target_dir = dir;
		local_stuff = suggested_path;
	}
	else
	{
		target_dir = dir->get_sub_directory(suggested_path.substr(0, idx));
		if (!target_dir)
		{
			return suggested_path + " not found!";
		}
		local_stuff = suggested_path.substr(idx + 1);
	}
	
	RX_ASSERT(target_dir);
	if (target_dir)// can't help this one :(
	{
		platform_directories_type sub_dirs;
		platform_items_type items;
		target_dir->get_content(sub_dirs, items, local_stuff + "*");
		for (auto& one : sub_dirs)
		{
			result.items.emplace_back(api::query_result_detail{ rx_directory, one->meta_info() });
		}
		for (auto& one : items)
		{
			result.items.emplace_back(api::query_result_detail{ one.get_type(), one.get_meta() });
		}
	}
	return true;
}


} // namespace queries
} // namespace meta
} // namespace rx_platform

