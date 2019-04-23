

/****************************************************************************
*
*  system\meta\rx_queries.cpp
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


// rx_queries
#include "system/meta/rx_queries.h"

#include "model/rx_meta_internals.h"


namespace rx_platform {

namespace meta {

namespace queries {

// Class rx_platform::meta::queries::derived_types_query 

string_type derived_types_query::query_name = "derived";


rx_result derived_types_query::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("typeName", type_name))
		return "Error serializing type_name";
	if (!stream.write_string("baseType", base_type))
		return "Error serializing base_type";
	if(!stream.write_bool("subTypes", include_subtypes))
		return "Error serializing subtypes";
	if (!stream.write_string("subfolder", subfolder))
		return "Error serializing subfolder";

	return true;
}

rx_result derived_types_query::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("typeName", type_name))
		return "Error reading type_name";
	if (!stream.read_string("baseType", base_type))
		return "Error reading base_type";
	if (!stream.read_bool("subTypes", include_subtypes))
		return "Error reading subtypes";
	if (!stream.read_string("subfolder", subfolder))
		return "Error reading subfolder";

	return true;
}

const string_type& derived_types_query::get_query_type ()
{
  return query_name;

}

rx_result derived_types_query::do_query (api::query_result& result, rx_directory_ptr dir)
{
	if (type_name == "object")
	{
		return do_query(result, dir, tl::type2type<object_types::object_type>());
	}
	else if (type_name == "struct")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::struct_type>());
	}
	else if (type_name == "variable")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::variable_type>());
	}
	else if (type_name == "source")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::source_type>());
	}
	else if (type_name == "filter")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::filter_type>());
	}
	else if (type_name == "event")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::event_type>());
	}
	else if (type_name == "mapper")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::mapper_type>());
	}
	else if (type_name == "domain")
	{
		return do_query(result, dir, tl::type2type<object_types::domain_type>());
	}
	else if (type_name == "port")
	{
		return do_query(result, dir, tl::type2type<object_types::port_type>());
	}
	else if (type_name == "application")
	{
		return do_query(result, dir, tl::type2type<object_types::application_type>());
	}
	else
	{
		return type_name + " is unknown type name!";
	}
	return true;
}

template<typename T>
rx_result derived_types_query::do_query(api::query_result& result, rx_directory_ptr dir, tl::type2type<T>)
{
	rx_node_id id = rx_node_id::null_id;
	if (!base_type.empty())
	{
		auto item = dir->get_sub_item(base_type);
		if (!item)
			return base_type + " not found!";
		id = item->meta_info().get_id();
	}
	result = model::platform_types_manager::instance().get_type_cache<T>().get_derived_types(id);

	return result.success;
}
template<typename T>
rx_result derived_types_query::do_simple_query(api::query_result& result, rx_directory_ptr dir, tl::type2type<T>)
{
	rx_node_id id = rx_node_id::null_id;
	if (!base_type.empty())
	{
		auto item = dir->get_sub_item(base_type);
		if (!item)
			return base_type + " not found!";
		id = item->meta_info().get_id();
	}
	result = model::platform_types_manager::instance().get_simple_type_cache<T>().get_derived_types(id);

	return result.success;
}
// Class rx_platform::meta::queries::rx_query 

rx_query::registered_queries_type rx_query::registered_queries_;

rx_query::~rx_query()
{
}



rx_result_with<query_ptr> rx_query::create_query_from_name (const string_type& type)
{
	auto it = registered_queries_.find(type);
	if (it == registered_queries_.end())
		return type + " is unknown query type!";
	else
		return it->second();
}

rx_result rx_query::init_query_types ()
{
	registered_queries_.emplace(derived_types_query::query_name, [] { return std::make_shared<derived_types_query>(); });
	registered_queries_.emplace(runtime_objects_query::query_name, [] { return std::make_shared<runtime_objects_query>(); });

	return true;
}

rx_result_with<query_ptr> rx_query::create_query (base_meta_reader& stream)
{
	if (!stream.start_object("query"))
		return "Query object missing";
	string_type query_type;
	if (!stream.read_string("queryType", query_type))
		return "No query type";

	auto result = create_query_from_name(query_type);
	if (!result)
		return result;

	auto ret = result.value()->deserialize(stream);
	if (!ret)
		return ret.errors();

	if (!stream.end_object())
		return "Error closing query object";

	return result;
}


// Class rx_platform::meta::queries::runtime_objects_query 

string_type runtime_objects_query::query_name = "runtime";


rx_result runtime_objects_query::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("typeName", type_name))
		return "Error serializing type_name";
	if (!stream.write_string("baseType", base_type))
		return "Error serializing base_type";
	if (!stream.write_string("subfolder", subfolder))
		return "Error serializing subfolder";

	return true;
}

rx_result runtime_objects_query::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("typeName", type_name))
		return "Error reading type_name";
	if (!stream.read_string("baseType", base_type))
		return "Error reading base_type";
	if (!stream.read_string("subfolder", subfolder))
		return "Error reading subfolder";

	return true;
}

const string_type& runtime_objects_query::get_query_type ()
{
  return query_name;

}

rx_result runtime_objects_query::do_query (api::query_result& result, rx_directory_ptr dir)
{
	if (type_name == "object")
	{
		return do_query(result, dir, tl::type2type<object_types::object_type>());
	}
	else if (type_name == "domain")
	{
		return do_query(result, dir, tl::type2type<object_types::domain_type>());
	}
	else if (type_name == "port")
	{
		return do_query(result, dir, tl::type2type<object_types::port_type>());
	}
	else if (type_name == "application")
	{
		return do_query(result, dir, tl::type2type<object_types::application_type>());
	}
	else
	{
		return type_name + " is unknown type name!";
	}
	return true;
}

template<typename T>
rx_result runtime_objects_query::do_query(api::query_result& result, rx_directory_ptr dir, tl::type2type<T>)
{
	rx_node_id id = rx_node_id::null_id;
	if (!base_type.empty())
	{
		auto item = dir->get_sub_item(base_type);
		if (!item)
			return base_type + " not found!";
		id = item->meta_info().get_id();
	}
	result = model::platform_types_manager::instance().get_type_cache<T>().get_derived_types(id);

	return result.success;
}

} // namespace queries
} // namespace meta
} // namespace rx_platform

