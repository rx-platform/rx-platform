

/****************************************************************************
*
*  dotnet\dotnet_api.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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

#include "api/rx_platform_api.h"
#include "rx_dotnet.h"
#include "system/callbacks/rx_callback.h"




#include "lib/rx_ser_json.h"
#include "system/meta/rx_meta_algorithm.h"
#include "model/rx_model_algorithms.h"
#include "system/server/rx_directory_cache.h"
#include "system/runtime/rx_extern_blocks.h"
#include "system/runtime/rx_extern_items.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "system/libraries/rx_plugin.h"
#include "system/storage_base/rx_storage.h"
#include "dotnet_api.h"
#include "rx_dotnet_runtime.h"


namespace rx_internal
{
namespace dotnet
{

namespace
{
struct module_data
{
	string_type information;
	string_array assemblies;
	std::vector<meta_data> types;
	std::vector<meta_data> runtimes;
};
std::map<string_type, module_data > g_dotnet_modules;

locks::slim_lock g_modules_lock;

}
namespace api
{

template<class T>
rx_result dotnet_create_concrete_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, tl::type2type<T>)
{
	using algorithm_type = typename T::algorithm_type;
	auto created = rx_create_reference<T>();
	created->meta_info = meta;
	auto result = algorithm_type::deserialize_type(*created, stream, STREAMING_TYPE_TYPE);
	if (result)
	{
		auto create_result = rx_internal::model::algorithms::types_model_algorithm<T>::create_type_sync(created);
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(T::type_id) + " " + meta.get_full_path());
			return create_result.errors();
		}
	}
	return result;
}
template<class T>
rx_result dotnet_create_concrete_simple_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, tl::type2type<T>)
{
	using algorithm_type = typename T::algorithm_type;
	auto created = rx_create_reference<T>();
	created->meta_info = meta;
	auto result = algorithm_type::deserialize_type(*created, stream, STREAMING_TYPE_TYPE);
	if (result)
	{
		auto create_result = rx_internal::model::algorithms::simple_types_model_algorithm<T>::create_type_sync(created);
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(T::type_id) + " " + meta.get_full_path());
			return create_result.errors();
		}
	}
	return result;
}

template<class T>
rx_result dotnet_delete_concrete_simple_type(rx_node_id& id, tl::type2type<T>)
{
	using algorithm_type = typename T::algorithm_type;
	auto del_result = rx_internal::model::algorithms::simple_types_model_algorithm<T>::delete_type_sync(id);


	return del_result;
}

template<class T>
rx_result dotnet_delete_concrete_type(rx_node_id& id, tl::type2type<T>)
{
	using algorithm_type = typename T::algorithm_type;
	auto del_result = rx_internal::model::algorithms::types_model_algorithm<T>::delete_type_sync(id);


	return del_result;
}

template<class T>
rx_result dotnet_delete_concrete_runtime(rx_node_id& id, tl::type2type<T>, rx_result_callback callback)
{
	rx_result result;
	rx_result* ret_ptr = &result;

	auto del_result = rx_internal::model::algorithms::runtime_model_algorithm<T>::delete_runtime_sync(id, 0, std::move(callback));
	if (del_result)
	{
		return result;
	}
	else
	{
		return del_result;
	}
}


rx_result dotnet_delete_concrete_relation_type(rx_node_id& id)
{
	auto del_result = rx_internal::model::algorithms::relation_types_algorithm::delete_type_sync(id);


	return del_result;
}

rx_result dotnet_delete_concrete_data_type(rx_node_id& id)
{
	auto del_result = rx_internal::model::algorithms::data_types_model_algorithm::delete_type_sync(id);


	return del_result;
}
rx_result dotnet_create_concrete_relation_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir)
{
	auto created = rx_create_reference<relation_type>();
	created->meta_info = meta;
	auto result = meta::meta_algorithm::relation_type_algorithm::deserialize_type(*created, stream, STREAMING_TYPE_TYPE);
	if (result)
	{
		auto create_result = rx_internal::model::algorithms::relation_types_algorithm::create_type_sync(created);
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(relation_type::type_id) + " " + meta.get_full_path());
			return create_result.errors();
		}
	}
	return result;
}


rx_result dotnet_create_concrete_data_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir)
{
	auto created = rx_create_reference<data_type>();
	created->meta_info = meta;
	auto result = meta::meta_algorithm::data_types_algorithm::deserialize_type(*created, stream, STREAMING_TYPE_TYPE);
	if (result)
	{
		auto create_result = rx_internal::model::algorithms::data_types_model_algorithm::create_type_sync(created);
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(data_type::type_id) + " " + meta.get_full_path());
			return create_result.errors();
		}
	}
	return result;
}
rx_result dotnet_create_type_from_stream(rx_item_type target_type, const string_type& name, const string_type& path
	, const  rx_node_id& id, const  rx_node_id& parent
	, uint32_t version, rx_time_struct modified, base_meta_reader& stream)
{
	meta_data meta;
	rx_result result;
	meta.path = path;
	meta.name = name;
	meta.id = rx_node_id(id);
	meta.parent = rx_node_id(parent);
	meta.version = version;
	meta.attributes = namespace_item_attributes::namespace_item_system_rw_access
		| namespace_item_attributes::namespace_item_code_generated;
	meta.created_time = rx_time::now();
	meta.modified_time = rx_time::now();

	auto dir = ns::rx_directory_cache::instance().get_or_create_directory(meta.path);
	if (dir)
	{
		switch (target_type)
		{
			// object types
			case rx_item_type::rx_object_type:
				result = dotnet_create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<object_type>());
				break;
			case rx_item_type::rx_port_type:
				result = dotnet_create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<port_type>());
				break;
			case rx_item_type::rx_application_type:
				result = dotnet_create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<application_type>());
				break;
			case rx_item_type::rx_domain_type:
				result = dotnet_create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<domain_type>());
				break;
				// simple types
			case rx_item_type::rx_struct_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<struct_type>());
				break;
			case rx_item_type::rx_variable_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<variable_type>());
				break;
				// variable sub-types
			case rx_item_type::rx_source_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<source_type>());
				break;
			case rx_item_type::rx_filter_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<filter_type>());
				break;
			case rx_item_type::rx_event_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<event_type>());
				break;
			case rx_item_type::rx_mapper_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<mapper_type>());
				break;
			case rx_item_type::rx_relation_type:
				result = dotnet_create_concrete_relation_type_from_stream(meta, stream, dir.value());
				break;
			case rx_item_type::rx_method_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<method_type>());
				break;
			case rx_item_type::rx_program_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<program_type>());
				break;
			case rx_item_type::rx_display_type:
				result = dotnet_create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<display_type>());
				break;
			case rx_item_type::rx_data_type:
				result = dotnet_create_concrete_data_type_from_stream(meta, stream, dir.value());
				break;
			default:
				result = "Unknown type: "s + rx_item_type_name(target_type);
		}
	}
	else
	{
		result = dir.errors();
		result.register_error("Error retrieving directory for the new item!");
	}
	if (!result)
		result.register_error("Error building "s + meta.get_full_path());
	return result;
}

rx_result dotnet_delete_type(rx_item_type target_type
	, const rx_node_id_struct* id)
{
	rx_result result;
	rx_node_id nod_id(id);
	switch (target_type)
	{
		// object types
		case rx_item_type::rx_object_type:
			result = dotnet_delete_concrete_type(nod_id, tl::type2type<object_type>());
			break;
		case rx_item_type::rx_port_type:
			result = dotnet_delete_concrete_type(nod_id, tl::type2type<port_type>());
			break;
		case rx_item_type::rx_application_type:
			result = dotnet_delete_concrete_type(nod_id, tl::type2type<application_type>());
			break;
		case rx_item_type::rx_domain_type:
			result = dotnet_delete_concrete_type(nod_id, tl::type2type<domain_type>());
			break;
			// simple types
		case rx_item_type::rx_struct_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<struct_type>());
			break;
		case rx_item_type::rx_variable_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<variable_type>());
			break;
			// variable sub-types
		case rx_item_type::rx_source_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<source_type>());
			break;
		case rx_item_type::rx_filter_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<filter_type>());
			break;
		case rx_item_type::rx_event_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<event_type>());
			break;
		case rx_item_type::rx_mapper_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<mapper_type>());
			break;
		case rx_item_type::rx_relation_type:
			result = dotnet_delete_concrete_relation_type(nod_id);
			break;
		case rx_item_type::rx_method_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<method_type>());
			break;
		case rx_item_type::rx_program_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<program_type>());
			break;
		case rx_item_type::rx_display_type:
			result = dotnet_delete_concrete_simple_type(nod_id, tl::type2type<display_type>());
			break;
		case rx_item_type::rx_data_type:
			result = dotnet_delete_concrete_data_type(nod_id);
			break;
		default:
			result = "Unknown type: "s + rx_item_type_name(target_type);
	}
	if (!result)
	{
		string_type str_id;
		rx_node_id(id).to_string(str_id);
		result.register_error("Error deleting type with id "s + str_id);
	}
	return result;
}


rx_result dotnet_delete_runtime(rx_item_type target_type
	, const rx_node_id& id, rx_result_callback callback)
{
	rx_result result;
	rx_node_id nod_id(id);
	switch (target_type)
	{
		// object types
		case rx_item_type::rx_object:
			result = dotnet_delete_concrete_runtime(nod_id, tl::type2type<object_type>(), std::move(callback));
			break;
		case rx_item_type::rx_port:
			result = dotnet_delete_concrete_runtime(nod_id, tl::type2type<port_type>(), std::move(callback));
			break;
		case rx_item_type::rx_application:
			result = dotnet_delete_concrete_runtime(nod_id, tl::type2type<application_type>(), std::move(callback));
			break;
		case rx_item_type::rx_domain:
			result = dotnet_delete_concrete_runtime(nod_id, tl::type2type<domain_type>(), std::move(callback));
			break;
		default:
			result = "Unknown type: "s + rx_item_type_name(target_type);
	}
	if (!result)
	{
		string_type str_id;
		rx_node_id(id).to_string(str_id);
		result.register_error("Error deleting type with id "s + str_id);
	}
	return result;
}


template<class T>
rx_result dotnet_create_concrete_runtime_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, tl::type2type<T>)
{
	data::runtime_values_data runtime_data;
	typename T::instance_data_t instance_data;
	auto result = instance_data.deserialize(stream, STREAMING_TYPE_OBJECT, meta);
	if (result)
	{
		auto create_result = rx_internal::model::algorithms::runtime_model_algorithm<T>::create_runtime_sync(std::move(instance_data), std::move(runtime_data));
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(T::RImplType::type_id) + " " + meta.get_full_path());
			return create_result.errors();
		}
	}
	return result;
}

rx_result dotnet_create_runtime_from_stream(rx_item_type target_type, const string_type& name, const string_type& path
	, const rx_node_id& id, const rx_node_id& parent
	, uint32_t version, rx_time_struct modified, base_meta_reader& stream)
{
	meta_data meta;
	rx_result result;
	meta.path = path;
	meta.name = name;
	meta.id = rx_node_id(id);
	meta.parent = rx_node_id(parent);
	meta.version = version;
	meta.attributes = namespace_item_attributes::namespace_item_system_rw_access
		| namespace_item_attributes::namespace_item_execute_access
		| namespace_item_attributes::namespace_item_code_generated;
	meta.created_time = rx_time::now();
	meta.modified_time = rx_time(modified);

	auto dir = ns::rx_directory_cache::instance().get_or_create_directory(meta.path);
	if (dir)
	{
		switch (target_type)
		{
			// object types
			case rx_item_type::rx_object:
				result = dotnet_create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<object_type>());
				break;
			case rx_item_type::rx_port:
				result = dotnet_create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<port_type>());
				break;
			case rx_item_type::rx_application:
				result = dotnet_create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<application_type>());
				break;
			case rx_item_type::rx_domain:
				result = dotnet_create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<domain_type>());
				break;
			default:
				result = "Unknown runtime type: "s + rx_item_type_name(target_type);
		}
	}
	else
	{
		result = dir.errors();
		result.register_error("Error retrieving directory for the new item!");
	}
	if (!result)
		result.register_error("Error building "s + meta.get_full_path());
	return result;
}




rx_result dotnet_register_item(const string_type& module, uintptr_t plugin, uint8_t item_type, const string_type& name, const string_type& path
	, const  rx_node_id& id, const  rx_node_id& parent
	, uint32_t version, rx_time_struct modified, uint32_t stream_version, const string_type& data)
{
	if (data.empty())
		return rx_result("Invalid data!");


	rx::serialization::json_reader stream(stream_version > 0 ? stream_version : RX_CURRENT_SERIALIZE_VERSION);

	if (!stream.parse_data(data))
		return rx_result("Failed to parse JSON data!");

	rx_result result = dotnet_create_type_from_stream((rx_item_type)item_type, name, path, id, parent, version, modified, stream).move();

	if (!result)
	{
		return result;
	}
	std::scoped_lock lock(g_modules_lock);

	string_type module_id;
	string_type assembly_name;
	string_type info;
	auto idx = string_type(module).find_first_of(';');
	if (idx != string_type::npos)
	{
		module_id = string_type(module).substr(0, idx);
		auto idx2 = string_type(module).find_first_of(';', idx + 1);
		if(idx2 != string_type::npos)
		{
			assembly_name = string_type(module).substr(idx + 1, idx2 - idx - 1);
			info = string_type(module).substr(idx2 + 1);
		}
		else
		{
			info = string_type(module).substr(idx + 1);
		}
	}
	else
	{
		module_id = string_type(module);
	}
	auto it = g_dotnet_modules.find(module_id);
	if (it == g_dotnet_modules.end())
	{
		module_data data;
		it = g_dotnet_modules.emplace(module_id, module_data()).first;
		if (idx != string_type::npos)
		{
			it->second.information = info;
			if(!assembly_name.empty())
			{
				if (std::find(it->second.assemblies.begin(), it->second.assemblies.end(), assembly_name) == it->second.assemblies.end())
					it->second.assemblies.push_back(assembly_name);
			}
		}
	}
	else if(!assembly_name.empty())
	{
		if (std::find(it->second.assemblies.begin(), it->second.assemblies.end(), assembly_name) == it->second.assemblies.end())
			it->second.assemblies.push_back(assembly_name);
	}
	meta_data meta;
	meta.name = name;
	meta.id = rx_node_id(id);
	meta.parent = rx_item_reference(parent);
	meta.path = path;
	meta.version = 0x10001;

	it->second.types.emplace_back(std::move(meta));

	return result;

}

rx_result_struct dotnet_delete_item(uintptr_t plugin, rx_item_type item_type, const rx_node_id_struct* id)
{

	rx_result result = dotnet_delete_type(item_type, id).move();
	if (result)
	{
	}
	return result.move();
}

rx_result dotnet_register_monitor_item(const string_type& module, rx_item_type item_type, const string_type& name, const string_type& path
	, const rx_node_id& id, const rx_node_id& parent
	, uint32_t version, rx_time_struct modified, uint32_t stream_version, const string_type& data)
{
	if (data.empty())
		return rx_result("Invalid data!");

	rx::serialization::json_reader stream;
	if (!stream.parse_data(data))
		return rx_result("Failed to parse JSON data!");

	std::scoped_lock lock(g_modules_lock);

	string_type module_id;
	auto idx = string_type(module).find_first_of(';');
	if (idx != string_type::npos)
	{
		module_id = string_type(module).substr(0, idx);
	}
	else
	{
		module_id = string_type(module);
	}
	auto dir = rx_directory_cache::instance().get_directory("/world/"s + module_id);
	if (!dir)
	{
		rx_directory_cache::instance().add_directory("/world/"s + module_id,
			rx_create_reference<storage_base::rx_code_storage>(true));
	}

	auto it = g_dotnet_modules.find(module_id);
	if (it == g_dotnet_modules.end())
	{
		it = g_dotnet_modules.emplace(module_id, module_data()).first;

		if (idx != string_type::npos)
		{
			it->second.information = string_type(module).substr(idx + 1);
		}
	}

	rx_result result = dotnet_create_runtime_from_stream(item_type, name, path, id, parent, version, modified, stream);

	meta_data meta;
	meta.name = name;
	meta.id = rx_node_id(id);
	meta.parent = rx_item_reference(parent);
	meta.path = path;
	meta.version = 0x10001;

	it->second.runtimes.emplace_back(std::move(meta));

	return result;

}
rx_result dotnet_register_runtime_item(const string_type& module, rx_item_type item_type, const string_type& name, const string_type& path
	, const rx_node_id& id, const rx_node_id& parent
	, uint32_t version, rx_time_struct modified, uint32_t stream_version, const string_type& data)
{
	if(data.empty())
		return rx_result("Invalid data!");

	rx::serialization::json_reader stream;
	if (!stream.parse_data(data))
		return rx_result("Failed to parse JSON data!");

	std::scoped_lock lock(g_modules_lock);

	string_type module_id;
	auto idx = string_type(module).find_first_of(';');
	if (idx != string_type::npos)
	{
		module_id = string_type(module).substr(0, idx);
	}
	else
	{
		module_id = string_type(module);
	}
	auto dir = rx_directory_cache::instance().get_directory("/world/"s + module_id);
	if (!dir)
	{
		rx_directory_cache::instance().add_directory("/world/"s + module_id,
			rx_create_reference<storage_base::rx_code_storage>(true));
	}

	auto it = g_dotnet_modules.find(module_id);
	if (it == g_dotnet_modules.end())
	{
		it = g_dotnet_modules.emplace(module_id, module_data()).first;
		
		if (idx != string_type::npos)
		{
			it->second.information = string_type(module).substr(idx + 1);
		}
	}

	rx_result result = dotnet_create_runtime_from_stream(item_type, name, path, id, parent, version, modified, stream);
	
	meta_data meta;
	meta.name = name;
	meta.id = rx_node_id(id);
	meta.parent = rx_item_reference(parent);
	meta.path = path;
	meta.version = 0x10001;

	it->second.runtimes.emplace_back(std::move(meta));

	return result;

}
rx_result_struct dotnet_delete_runtime_item(const string_type& module, rx_item_type item_type, const rx_node_id& id, rx_result_callback callback)
{

	rx_result result = dotnet_delete_runtime(item_type, id, std::move(callback)).move();
	if (result)
	{
	}
	return result.move();
}


rx_result dotnetBuildType(rx_item_type type, const string_type& module, const  rx_node_id& id, const  rx_node_id& parent
	, const string_type& name, const string_type& path, uint32_t version, uint32_t flags, const string_type& def, int ownType
	, const string_type& conns, const string_type& code_info)
{
	if (ownType)
	{
		if (type == rx_item_type::rx_object_type)
		{

			string_type captured_conns(conns);
			rx_node_id captured_id(id);
			dotnet_code_info captured_code_info(code_info);
			std::function<constructed_data_t<typename object_type::RImplPtr>(const rx_node_id&)> func =
				[captured_code_info, captured_conns, captured_id](const rx_node_id& new_id) // full constructor function
				{
					constructed_data_t<typename object_type::RImplPtr> ret;
					auto new_ptr = rx_create_reference<rx_internal::dotnet::dotnet_object_runtime>(std::move(captured_conns), captured_id, captured_code_info);
					if (!new_ptr)
					{
						std::ostringstream ss;
						ss << "Instance " << new_id.to_string()
							<< " could not be created for class";
						return ret;
					}
					else
					{
						ret.ptr = new_ptr;
					}
					ret.register_f = [new_ptr, captured_id](const rx_node_id& id) mutable
						{
							{
								std::scoped_lock lock(rx_internal::dotnet::dotnet_object_runtime::instances_lock);
								rx_internal::dotnet::dotnet_object_runtime::runtime_instances.emplace(id, new_ptr);
							}
						};
					ret.unregister_f = [new_ptr, captured_id](const rx_node_id& id) mutable
						{
							{
								std::scoped_lock lock(rx_internal::dotnet::dotnet_object_runtime::instances_lock);
								rx_internal::dotnet::dotnet_object_runtime::runtime_instances.erase(id);
							}

						};
					return ret;
				};
			auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
				id, func);
			if (!result)
				return result;
		}
		else if (type == rx_item_type::rx_source_type)
		{
			rx_node_id captured_id(id);
			string_type captured_conns(conns);
			std::function<typename source_type::RTypePtr()> func =
				[captured_conns, captured_id]()
				{
					auto new_ptr = rx_create_reference<rx_internal::dotnet::dotnet_source>(std::move(captured_conns), std::move(captured_id));
					if (!new_ptr)
					{
						std::ostringstream ss;
						ss << "Instance of source"
							<< " could not be created for class";
					}
					return new_ptr;
				};
			auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<source_type>().register_constructor(
				id, func);
			if (!result)
				return result;
		}
		else if (type == rx_item_type::rx_mapper_type)
		{
			rx_node_id captured_id(id);
			string_type captured_conns(conns);
			std::function<typename mapper_type::RTypePtr()> func =
				[captured_conns, captured_id]()
				{
					auto new_ptr = rx_create_reference<rx_internal::dotnet::dotnet_mapper>(std::move(captured_conns), std::move(captured_id));
					if (!new_ptr)
					{
						std::ostringstream ss;
						ss << "Instance of mapper"
							<< " could not be created for class";
					}
					return new_ptr;
				};
			auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<mapper_type>().register_constructor(
				id, func);
			if (!result)
				return result;
		}
		else if (type == rx_item_type::rx_event_type)
		{
			rx_node_id captured_id(id);
			string_type captured_conns(conns);
			std::function<typename event_type::RTypePtr()> func =
				[captured_conns, captured_id]()
				{
					auto new_ptr = rx_create_reference<rx_internal::dotnet::dotnet_event>(std::move(captured_conns), std::move(captured_id));
					if (!new_ptr)
					{
						std::ostringstream ss;
						ss << "Instance of event"
							<< " could not be created for class";
					}
					return new_ptr;
				};
			auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<event_type>().register_constructor(
				id, func);
			if (!result)
				return result;
		}
		else if (type == rx_item_type::rx_struct_type)
		{
			rx_node_id captured_id(id);
			string_type captured_conns(conns);
			std::function<typename struct_type::RTypePtr()> func =
				[captured_conns, captured_id]()
				{
					auto new_ptr = rx_create_reference<rx_internal::dotnet::dotnet_struct>(std::move(captured_conns), std::move(captured_id));
					if (!new_ptr)
					{
						std::ostringstream ss;
						ss << "Instance of struct"
							<< " could not be created for class";
					}
					return new_ptr;
				};
			auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<struct_type>().register_constructor(
				id, func);
			if (!result)
				return result;
		}
		else
		{
			return rx_result("Cannot build managed type with runtime flag set!");
		}
	}
	rx_result result = dotnet_register_item(module, 0, type, name, path, id, parent, version, { 0 }, 0, def);
	return result;
	
}

rx_result_struct dotnetDeleteType(rx_item_type type, const char* module, const rx_node_id_struct_t* id)
{
	rx_result result = dotnet_delete_item(0, type, id);
	return result.move();
}

void dotnetBuildRuntime(rx_item_type type, const string_type& module, const  rx_node_id& id, const  rx_node_id& parent
	, const string_type& name, const string_type& path, uint32_t version, uint32_t flags, const string_type& def, std::function<void(rx_result&&)> callback)
{

	string_type module_id(module);
	string_type str_name(name);
	string_type str_path(path);
	rx_node_id node_id_id(id);
	rx_node_id node_id_parent(parent);
	string_type str_def(def);
	auto callback_ptr = std::make_shared< std::function<void(rx_result&&)>>(std::move(callback));

	if (version == 0xffffffff)
	{
		(*callback_ptr)(rx_result(RX_NOT_IMPLEMENTED));
		return;
	}
	else
	{
		rx_post_function_to(RX_DOMAIN_META, rx_reference_ptr()
			, [type, module_id = std::move(module_id), node_id_id = std::move(node_id_id), node_id_parent = std::move(node_id_parent), str_name = std::move(str_name), str_path = std::move(str_path), str_def = std::move(str_def), callback_ptr = std::move(callback_ptr)]
			()
			{
				auto result = dotnet_register_runtime_item(module_id, type, str_name, str_path, node_id_id, node_id_parent, 0x10001, { 0 }, 0, str_def);
				(*callback_ptr)(std::move(result));
			});
	}
}

void dotnetDeleteRuntime(rx_item_type type, const char* module, const rx_node_id_struct_t* id, std::function<void(rx_result&&)> callback)
{
	if (module == nullptr)
	{
		rx_result result = rx_result("Invalid module data!").move();
		callback(result.move());
		return;
	}

	string_type module_id(module);
	rx_node_id node_id_id(id);
		
	rx_post_function_to(RX_DOMAIN_META, rx_reference_ptr(), [type, module_id, node_id_id, callback]()
		{
			rx_result_callback native_callback(rx_reference_ptr(), [callback](rx_result&& res)
				{
					callback(std::move(res).move());
				});

			rx_result result = dotnet_delete_runtime_item(module_id, type, node_id_id, std::move(native_callback));
			if (!result)
			{
				callback(result.move());
			}
		});
		
}
/*

void dotnetResultCallback(uint32_t transId, rx_result_struct result)
{
	//rx_internal::dotnet::dotnet_hosting::instance().received_result_callback(transId, rx_result(result).move());
}
void dotnetVoidCallback(uint32_t transId)
{
	//rx_internal::dotnet::dotnet_hosting::instance().received_void_callback(transId);
}
*/
void dotnetWriteValue(
	rx_item_type itemType
	, uint32_t transId
	, void* instance
	, size_t idx
	, typed_value_type value
	, std::function<void(rx_result&&)> callback/*dotnetRuntimeResultDelegate*/)
{	
	rx_simple_value temp_value(std::move(value));
	rx_internal::dotnet::dotnet_hosting::instance().received_write_value(itemType
		, transId
		, instance
		, idx
		, std::move(temp_value)
		, callback);
		
}


void dotnetExecuteValue(
	rx_item_type itemType
	, uint32_t transId
	, void* instance
	, size_t idx
	, typed_value_type value
	, std::function<void(rx_simple_value&&, rx_result&&)> callback/*dotnetRuntimeResultDelegate*/)
{
	rx_simple_value temp_value(std::move(value));
	rx_internal::dotnet::dotnet_hosting::instance().received_execute_value(itemType
		, transId
		, instance
		, idx
		, std::move(temp_value)
		, callback);

}


/*

void dotnetExecuteDone(
	uint32_t transId
	, void* instance
	, const char* value,
	rx_result_struct result)
{
	string_type temp_value(value);
	rx_result temp_res(result);
	//	rx_internal::dotnet::dotnet_hosting::instance().received_execute_done(
	//		transId
	//		, instance
	//		, std::move(temp_value)
	//		, std::move(temp_res));
}

void dotnetSourceWriteDone(
	uint32_t transId
	, void* instance
	, rx_result_struct result)
{
	rx_result temp_value(std::move(result));
	//	rx_internal::dotnet::dotnet_hosting::instance().received_source_write_done(transId, instance, std::move(temp_value));


}


void dotnetSourceChange(
	void* instance
	, full_value_type value)
{
	rx_value temp_value(std::move(value));
	//	rx_internal::dotnet::dotnet_hosting::instance().received_source_change(instance, std::move(temp_value));
}


*/

} // rx_internal
} // dotnet
} // api


namespace rx_internal
{
namespace dotnet
{


std::vector<std::pair<string_type, string_type> > get_loaded_dotnet_libraries()
{
	std::vector<std::pair<string_type, string_type> > result;
	std::scoped_lock lock(g_modules_lock);
	result.reserve(g_dotnet_modules.size());
	for (const auto& one : g_dotnet_modules)
	{
		result.emplace_back(one.first, one.second.information);
	}
	return result;
}


std::vector<string_type> get_loaded_dotnet_assemblies(const string_type lib)
{
	std::vector<string_type> result;
	std::scoped_lock lock(g_modules_lock);
	auto it = g_dotnet_modules.find(lib);
	if(it != g_dotnet_modules.end())
	{
		result.reserve(it->second.assemblies.size());
		for (const auto& assembly : it->second.assemblies)
		{
			result.emplace_back(assembly);
		}
	}
	return result;
}



} // rx_internal
} // dotnet


