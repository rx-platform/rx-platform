

/****************************************************************************
*
*  system\meta\rx_queries.cpp
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


// rx_queries
#include "system/meta/rx_queries.h"

#include "model/rx_meta_internals.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace meta {

namespace queries {

// Class rx_platform::meta::queries::derived_types_query 

string_type derived_types_query::query_name = "derived";


rx_result derived_types_query::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("typeName", type_name))
		return stream.get_error();
	if (!stream.write_string("baseType", base_type))
		return stream.get_error();
	if(!stream.write_bool("subTypes", include_subtypes))
		return stream.get_error();
	if (!stream.write_string("subfolder", subfolder))
		return stream.get_error();

	return true;
}

rx_result derived_types_query::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("typeName", type_name))
		return stream.get_error();
	if (!stream.read_string("baseType", base_type))
		return stream.get_error();
	if (!stream.read_bool("subTypes", include_subtypes))
		return stream.get_error();
	if (!stream.read_string("subfolder", subfolder))
		return stream.get_error();

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
	else if (type_name == "relation")
	{
		return do_relation_query(result, dir);
	}
	else if (type_name == "program")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::program_type>());
	}
	else if (type_name == "method")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::method_type>());
	}
	else if (type_name == "display")
	{
		return do_simple_query(result, dir, tl::type2type<basic_types::display_type>());
	}
	else if (type_name == "data")
	{
		return do_data_query(result, dir);
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
		id = item.get_meta().id;
	}
	result = rx_internal::model::platform_types_manager::instance().get_type_repository<T>().get_derived_types(id);

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
		id = item.get_meta().id;
	}
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<T>().get_derived_types(id);

	return result.success;
}
rx_result derived_types_query::do_relation_query(api::query_result& result, rx_directory_ptr dir)
{

	rx_node_id id = rx_node_id::null_id;
	if (!base_type.empty())
	{
		auto item = dir->get_sub_item(base_type);
		if (!item)
			return base_type + " not found!";
		id = item.get_meta().id;
	}
	result = rx_internal::model::platform_types_manager::instance().get_relations_repository().get_derived_types(id);

	return result.success;
}
rx_result derived_types_query::do_data_query(api::query_result& result, rx_directory_ptr dir)
{

	rx_node_id id = rx_node_id::null_id;
	if (!base_type.empty())
	{
		auto item = dir->get_sub_item(base_type);
		if (!item)
			return base_type + " not found!";
		id = item.get_meta().id;
	}
	result = rx_internal::model::platform_types_manager::instance().get_data_types_repository().get_derived_types(id);

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
	registered_queries_.emplace(translate_query::query_name, [] { return std::make_shared<translate_query>(); });
	registered_queries_.emplace(port_stack_query::query_name, [] { return std::make_shared<port_stack_query>(); });
	return true;
}

void rx_query::deinit_query_types ()
{
	registered_queries_.clear();
}

rx_result_with<query_ptr> rx_query::create_query (base_meta_reader& stream)
{
	if (!stream.start_object("query"))
		return stream.get_error();
	string_type query_type;
	if (!stream.read_string("queryType", query_type))
		return stream.get_error();

	auto result = create_query_from_name(query_type);
	if (!result)
		return result.errors();

	auto ret = result.value()->deserialize(stream);
	if (!ret)
		return ret.errors();

	if (!stream.end_object())
		return stream.get_error();

	return result;
}


// Class rx_platform::meta::queries::runtime_objects_query 

string_type runtime_objects_query::query_name = "runtime";


rx_result runtime_objects_query::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("typeName", type_name))
		return stream.get_error();
	if (!stream.write_string("instanceName", instance_name))
		return stream.get_error();
	if (!stream.write_id("instance", instance))
		return stream.get_error();
	if (!stream.write_string("subfolder", subfolder))
		return stream.get_error();

	return true;
}

rx_result runtime_objects_query::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("typeName", type_name))
		return stream.get_error();
	if (!stream.read_string("instanceName", instance_name))
		return stream.get_error();
	if (!stream.read_id("instance", instance))
		return stream.get_error();
	if (!stream.read_string("subfolder", subfolder))
		return stream.get_error();

	return true;
}

const string_type& runtime_objects_query::get_query_type ()
{
  return query_name;

}

rx_result runtime_objects_query::do_query (api::query_result& result, rx_directory_ptr dir)
{
	auto type = rx_parse_type_name(type_name);
	switch (type)
	{// useful  suggested suggestion 
	case rx_item_type::rx_application:
		{
			rx_internal::sys_runtime::platform_runtime_manager::instance().get_applications(result, subfolder);
		}
		break;
	case rx_item_type::rx_port:
		{
			if (!instance_name.empty() || !instance.is_null())
			{
				rx_node_id id = instance;
				if (id.is_null())
				{
					auto item = dir->get_sub_item(instance_name);
					if (!item)
						return type_name + " not found!";
					id = item.get_meta().id;
				}
				auto app_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(id);
				if (app_ptr)
				{
					app_ptr.value()->get_instance_data().get_ports(result, true);
				}
			}
		}
		break;
	case rx_item_type::rx_domain:
		{
			if (!instance_name.empty() || !instance.is_null())
			{
				rx_node_id id = instance;
				if (id.is_null())
				{
					auto item = dir->get_sub_item(instance_name);
					if (!item)
						return type_name + " not found!";
					id = item.get_meta().id;
				}
				auto app_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(id);
				if (app_ptr)
				{
					app_ptr.value()->get_instance_data().get_domains(result);
				}
			}
		}
		break;
	case rx_item_type::rx_object:
		{
			if (!instance_name.empty() || !instance.is_null())
			{
				rx_node_id id = instance;
				if (id.is_null())
				{
					auto item = dir->get_sub_item(instance_name);
					if (!item)
						return type_name + " not found!";
					id = item.get_meta().id;
				}
				auto app_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(id);
				if (app_ptr)
				{
					app_ptr.value()->get_instance_data().get_objects(result);
				}
			}
		}
		break;
	default:
		return type_name + " is invalid type name!";
	}
	return true;
}


// Class rx_platform::meta::queries::translate_query 

string_type translate_query::query_name = "translate";


rx_result translate_query::serialize (base_meta_writer& stream) const
{
	if (!stream.start_array("items", items.size()))
		return stream.get_error();
	for(const auto& one : items)
	{

		if (!stream.write_item_reference("ref", one))
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result translate_query::deserialize (base_meta_reader& stream)
{
	if (!stream.start_array("items"))
		return stream.get_error();
	items.clear();
	while (!stream.array_end())
	{
		rx_item_reference temp;
		if (!stream.read_item_reference("ref", temp))
			return stream.get_error();
		items.emplace_back(std::move(temp));
	}
	return true;
}

const string_type& translate_query::get_query_type ()
{
  return query_name;

}

rx_result translate_query::do_query (api::query_result& result, rx_directory_ptr dir)
{
	for (const auto& one : items)
	{
		if (one.is_node_id())
		{
			meta_data temp;
			auto type = rx_internal::model::platform_types_manager::instance().get_types_resolver().get_item_data(one.get_node_id(), temp);
			if (type != rx_item_type::rx_invalid_type)
			{
				result.items.emplace_back(api::query_result_detail{ type, temp });
			}
		}
		else
		{
			auto item = dir->get_sub_item(one.get_path());
			if (item)
			{
				result.items.emplace_back(api::query_result_detail{ item.get_type(), item.get_meta() });
			}
		}
	}
	result.success = true;
	return true;
}


// Class rx_platform::meta::queries::port_stack_query 

string_type port_stack_query::query_name = "io";


rx_result port_stack_query::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("typeName", type_name))
		return stream.get_error();
	if (!stream.write_string("instanceName", instance_name))
		return stream.get_error();
	if (!stream.write_id("instance", instance))
		return stream.get_error();
	if (!stream.write_string("subfolder", subfolder))
		return stream.get_error();

	return true;
}

rx_result port_stack_query::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("typeName", type_name))
		return stream.get_error();
	if (!stream.read_string("instanceName", instance_name))
		return stream.get_error();
	if (!stream.read_id("instance", instance))
		return stream.get_error();
	if (!stream.read_string("subfolder", subfolder))
		return stream.get_error();

	return true;
}

const string_type& port_stack_query::get_query_type ()
{
  return query_name;

}

rx_result port_stack_query::do_query (api::query_result& result, rx_directory_ptr dir)
{
	if (!instance_name.empty() || !instance.is_null())
	{
		rx_node_id id = instance;
		if (id.is_null())
		{
			auto item = dir->get_sub_item(instance_name);
			if (!item)
				return type_name + " not found!";
			id = item.get_meta().id;
		}
		auto port_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(id);
		if (port_ptr)
		{
			;
			rx_port_ptr stack_top = port_ptr.value()->get_instance_data().stack_data.build_map.stack_top;
			std::vector<rx_port_ptr> up_ports = port_ptr.value()->get_instance_data().stack_data.build_map.get_registered();
			result.items.reserve(stack_top ? up_ports.size() + 1 : up_ports.size());
			for (const auto& one : up_ports)
			{
				result.items.emplace_back(api::query_result_detail{ rx_port, one->meta_info() });
			}
			//if(stack_top)
				//result.items.emplace_back(api::query_result_detail{ rx_port, stack_top->meta_info() });
		}
	}
	return true;
}


} // namespace queries
} // namespace meta
} // namespace rx_platform

