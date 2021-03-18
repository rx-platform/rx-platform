

/****************************************************************************
*
*  sys_internal\rx_storage_build.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_storage_build
#include "sys_internal/rx_storage_build.h"

#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "system/meta/rx_meta_algorithm.h"


namespace rx_internal {

namespace builders {

namespace storage {

// Class rx_internal::builders::storage::configuration_storage_builder 

configuration_storage_builder::configuration_storage_builder (rx_storage_ptr storage)
      : storage_(storage)
{
}


configuration_storage_builder::~configuration_storage_builder()
{
}



rx_result configuration_storage_builder::do_build (rx_directory_ptr root)
{
	auto result = build_from_storage(root, *storage_);
	if (!result)
	{
		for (auto& err : result.errors())
			BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
	}
	return result;
}

rx_result configuration_storage_builder::build_from_storage (rx_directory_ptr root, rx_platform::storage_base::rx_platform_storage& storage)
{
	if (!storage.is_valid_storage())
		return "Storage not initialized!";

	using storage_items = std::vector<rx_storage_item_ptr>;
	storage_items items, type_items, instance_items;
	std::map<string_type, rx_storage_item_ptr> runtime_items;
	auto result = storage.list_storage(items);
	if (result)
	{
		for (auto& item : items)
		{
			if (item)
			{
				switch (item->get_storage_type())
				{
				case rx_storage_item_type::type:
					type_items.emplace_back(std::move(item));
					break;
				case rx_storage_item_type::instance:
					instance_items.emplace_back(std::move(item));
					break;
				case rx_storage_item_type::runtime:
					runtime_items.emplace(item->get_item_path(), std::move(item));
					break;
				default:
					RX_ASSERT(false);
				}
			}
		}
		// first create all types
		for (auto& item : type_items)
		{
			if (item->get_storage_type() == rx_storage_item_type::type)
			{
				result = item->open_for_read();
				if (result)
				{
					auto& stream = item->read_stream();
					int type = 0;
					result = stream.read_header(type);
					if (result)
					{
						switch (type)
						{
						case STREAMING_TYPE_TYPE:
							result = create_type_from_storage(item, root);
							break;
						case STREAMING_TYPE_OBJECT:
							continue;
						default:
							result = "Invalid serialization type!";
						}
					}
					else
						result.register_error("Error in deserialization from " + item->get_item_reference());

					item->close_read();
				}
				else
				{// we had an error
					result.register_error("Error in opening item " + item->get_item_reference());
				}
				if (!result)
					dump_errors_to_log(result.errors());
			}
		}
		// then create runtime instances
		for (auto& item : instance_items)
		{
			if (item)
			{
				if (item->get_storage_type() == rx_storage_item_type::instance)
				{
					result = item->open_for_read();
					if (result)
					{
						auto& stream = item->read_stream();
						int type = 0;
						result = stream.read_header(type);
						if (result)
						{
							switch (type)
							{
							case STREAMING_TYPE_TYPE:
								continue;
								break;
							case STREAMING_TYPE_OBJECT:
								{
									auto jbg = item->get_item_path();
									auto rt_it = runtime_items.find(item->get_item_path());
									if (rt_it != runtime_items.end())
									{
										auto rt_result = rt_it->second->open_for_read();
										if (rt_result)
										{
											result = create_object_from_storage(item, rt_it->second, root);
											rt_it->second->close_read();
										}
									}
									else
									{
										rx_storage_item_ptr null_storage_ptr;
										result = create_object_from_storage(item, null_storage_ptr, root);
									}
								}
								break;
							default:
								result = "Invalid serialization type!";
							}
						}
						else
							result.register_error("Error in deserialization from " + item->get_item_reference());

						item->close_read();
					}
					else
					{// we had an error
						result.register_error("Error in opening item " + item->get_item_reference());
					}
					if (!result)
						dump_errors_to_log(result.errors());
				}
			}
		}
	}
	else
	{
		result.register_error("Error listing storage");
	}
	return result;
}

rx_result configuration_storage_builder::create_object_from_storage (rx_storage_item_ptr& storage, rx_storage_item_ptr& runtime_storage, rx_directory_ptr root)
{
	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(storage->read_stream(), STREAMING_TYPE_OBJECT, target_type);
	if (!result)
		return result;
	bool do_save = false;
	bool meta_changed = storage->preprocess_meta_data(meta);
	if (meta_changed)
	{
		do_save = true;
		meta.id = rx_node_id::generate_new();
		META_LOG_WARNING("configuration_storage_builder", 250, "Created new instance of an object " + meta.get_full_path());
	}

	directory_creator creator;
	auto dir = creator.get_or_create_direcotry(root, meta.path);
	if (dir)
	{
		switch (target_type)
		{
			// objects
		case rx_item_type::rx_object:
			result = create_concrete_object_from_storage(meta, storage, runtime_storage, dir.value(), do_save, tl::type2type<object_type>());
			break;
		case rx_item_type::rx_port:
			result = create_concrete_object_from_storage(meta, storage, runtime_storage, dir.value(), do_save, tl::type2type<port_type>());
			break;
		case rx_item_type::rx_application:
			result = create_concrete_object_from_storage(meta, storage, runtime_storage, dir.value(), do_save, tl::type2type<application_type>());
			break;
		case rx_item_type::rx_domain:
			result = create_concrete_object_from_storage(meta, storage, runtime_storage, dir.value(), do_save, tl::type2type<domain_type>());
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
	return result;
}

rx_result configuration_storage_builder::create_type_from_storage (rx_storage_item_ptr& storage, rx_directory_ptr root)
{
	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(storage->read_stream(), STREAMING_TYPE_TYPE, target_type);
	if (!result)
		return result;

	bool do_save = false;
	bool meta_changed = storage->preprocess_meta_data(meta);
	if (meta_changed)
	{
		do_save = true;
		meta.id = rx_node_id::generate_new();
		META_LOG_WARNING("configuration_storage_builder", 250, "Created new type " + meta.get_full_path());
	}

	directory_creator creator;
	auto dir = creator.get_or_create_direcotry(root, meta.path);
	if (dir)
	{
		switch (target_type)
		{
		// object types
		case rx_item_type::rx_object_type:
			result = create_concrete_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<object_type>());
			break;
		case rx_item_type::rx_port_type:
			result = create_concrete_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<port_type>());
			break;
		case rx_item_type::rx_application_type:
			result = create_concrete_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<application_type>());
			break;
		case rx_item_type::rx_domain_type:
			result = create_concrete_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<domain_type>());
			break;
		// simple types
		case rx_item_type::rx_struct_type:
			result = create_concrete_simple_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<struct_type>());
			break;
		case rx_item_type::rx_variable_type:
			result = create_concrete_simple_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<variable_type>());
			break;
		// variable sub-types
		case rx_item_type::rx_source_type:
			result = create_concrete_simple_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<source_type>());
			break;
		case rx_item_type::rx_filter_type:
			result = create_concrete_simple_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<filter_type>());
			break;
		case rx_item_type::rx_event_type:
			result = create_concrete_simple_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<event_type>());
			break;
		case rx_item_type::rx_mapper_type:
			result = create_concrete_simple_type_from_storage(meta, storage, dir.value(), do_save, tl::type2type<mapper_type>());
			break;
		case rx_item_type::rx_relation_type:
			result = create_concrete_relation_type_from_storage(meta, storage, dir.value(), do_save);
			break;
		default:
			result = "Unknown type: "s + rx_item_type_name(target_type);
		}
		storage->close_read();
		if (meta_changed)
		{
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

void configuration_storage_builder::dump_errors_to_log (const string_array& errors)
{
	for (auto& err : errors)
		BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
}


template<class T>
rx_result configuration_storage_builder::create_concrete_type_from_storage(meta::meta_data& meta, rx_storage_item_ptr& storage, rx_directory_ptr dir, bool save, tl::type2type<T>)
{
	using algorithm_type = typename T::algorithm_type;
	auto created = rx_create_reference<T>();
	created->meta_info = meta;
	auto result = algorithm_type::deserialize_type(*created, storage->read_stream(), STREAMING_TYPE_TYPE);
	storage->close_read();
	if (result)
	{
		auto create_result = model::algorithms::types_model_algorithm<T>::create_type_sync(created);
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
rx_result configuration_storage_builder::create_concrete_simple_type_from_storage(meta::meta_data& meta, rx_storage_item_ptr& storage, rx_directory_ptr dir, bool save, tl::type2type<T>)
{
	using algorithm_type = typename T::algorithm_type;
	auto created = rx_create_reference<T>();
	created->meta_info = meta;
	auto result = algorithm_type::deserialize_type(*created, storage->read_stream(), STREAMING_TYPE_TYPE);
	storage->close_read();
	if (result)
	{
		auto create_result = model::algorithms::simple_types_model_algorithm<T>::create_type_sync(created);
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

rx_result configuration_storage_builder::create_concrete_relation_type_from_storage(meta::meta_data& meta, rx_storage_item_ptr& storage, rx_directory_ptr dir, bool save)
{
	auto created = rx_create_reference<relation_type>();
	created->meta_info = meta;
	auto result = meta::meta_algorithm::relation_type_algorithm::deserialize_type(*created, storage->read_stream(), STREAMING_TYPE_TYPE);
	storage->close_read();
	if (result)
	{
		auto create_result = model::algorithms::relation_types_algorithm::create_type_sync(created);
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


template<class T>
rx_result configuration_storage_builder::create_concrete_object_from_storage(meta::meta_data& meta, rx_storage_item_ptr& storage, rx_storage_item_ptr& runtime_storage, rx_directory_ptr dir, bool save, tl::type2type<T>)
{
	data::runtime_values_data runtime_data;
	typename T::instance_data_t instance_data;
	auto result = instance_data.deserialize(storage->read_stream(), STREAMING_TYPE_OBJECT, meta);
	if (result)
	{
		if (runtime_storage)
		{
			runtime_storage->read_stream().read_init_values(nullptr, runtime_data);
			RUNTIME_LOG_DEBUG("runtime_model_algorithm", 100, "Readed runtime "s + rx_item_type_name(T::RImplType::type_id) + " "s + meta.get_full_path());
				
		}
		auto create_result = model::algorithms::runtime_model_algorithm<T>::create_runtime_sync(std::move(instance_data), std::move(runtime_data));
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

// Class rx_internal::builders::storage::directory_creator 


rx_result_with<rx_directory_ptr> directory_creator::get_or_create_direcotry (rx_directory_ptr from, const string_type& path)
{
	rx_result_with<rx_directory_ptr> result;
	rx_directory_ptr current_dir = from;
	rx_directory_ptr temp_dir;
	string_type temp_path;
	size_t last = 0;
	size_t next = 0;
	while ((next = path.find(RX_DIR_DELIMETER, last)) != string_type::npos)
	{
		temp_path = path.substr(last, next - last);
		temp_dir = current_dir->get_sub_directory(temp_path);
		if (temp_dir)
		{
			current_dir = temp_dir;
		}
		else
		{
			result = current_dir->add_sub_directory(temp_path);
			if (!result)
				return result;
			current_dir = result.value();

		}
		last = next + 1;
	}
	temp_path = path.substr(last);
	temp_dir = current_dir->get_sub_directory(temp_path);
	if (temp_dir)
	{
		current_dir = temp_dir;
	}
	else
	{
		result = current_dir->add_sub_directory(temp_path);
			if (!result)
				return result;
			current_dir = result.value();
	}
	return current_dir;
}


} // namespace storage
} // namespace builders
} // namespace rx_internal

