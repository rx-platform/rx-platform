

/****************************************************************************
*
*  sys_internal\rx_storage_build.cpp
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


// rx_storage_build
#include "sys_internal/rx_storage_build.h"

#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"


namespace sys_internal {

namespace builders {

namespace storage {

// Class sys_internal::builders::storage::configuration_storage_builder

configuration_storage_builder::configuration_storage_builder (meta::rx_storage_type storage_type)
      : storage_type_(storage_type)
{
}


configuration_storage_builder::~configuration_storage_builder()
{
}



rx_result configuration_storage_builder::do_build (platform_root::smart_ptr root)
{
	rx_storage_ptr storage;
	switch (storage_type_)
	{
	case rx_storage_type::system_storage:
		storage = rx_gate::instance().get_host()->get_system_storage();
		break;
	case rx_storage_type::user_storage:
		storage = rx_gate::instance().get_host()->get_user_storage();
		break;
	case rx_storage_type::test_storage:
		storage = rx_gate::instance().get_host()->get_test_storage();
		break;
	default:
		return "Unexpected storage type as parameter for storage building!";
	}
	auto result = build_from_storage(root, *storage, storage_type_);
	if (result)
	{
		// log the right thing to be aware of system building status
		switch (storage_type_)
		{
		case rx_storage_type::system_storage:
			BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from system storage done.");
			break;
		case rx_storage_type::user_storage:
			BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from user storage done.");
			break;
		case rx_storage_type::test_storage:
			BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from test storage done.");
			break;
		default://this should really not happened (look at the first switch case in the function!!!
			RX_ASSERT(false);
		}
	}
	else
	{
		for (auto& err : result.errors())
			BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
		// log the right thing to be aware of system building status
		switch (storage_type_)
		{
		case rx_storage_type::system_storage:
			BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from system storage");
			break;
		case rx_storage_type::user_storage:
			BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from user storage");
			break;
		case rx_storage_type::test_storage:
			BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from test storage");
			break;
		default://this should really not happened (look at the first switch case in the function!!!
			RX_ASSERT(false);
		}
	}
	return result;
}

rx_result configuration_storage_builder::build_from_storage (platform_root::smart_ptr root, rx_platform::storage_base::rx_platform_storage& storage, meta::rx_storage_type storage_type)
{
	if (!storage.is_valid_storage())
		return "Storage not initialized!";

	using storage_items = std::vector<rx_storage_item_ptr>;
	storage_items items;
	auto result = storage.list_storage(items);
	if (result)
	{
		for (auto& item : items)
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
						result = create_type_from_storage(stream, std::move(item), storage_type, root);
						break;
					case STREAMING_TYPE_OBJECT:
						item->close();
						continue;
					default:
						item->close();
						result = "Invalid serialization type!";
					}
				}
				else
					result.register_error("Error in deserialization from " + item->get_item_reference());
			}
			else
			{// we had an error
				result.register_error("Error in opening item " + item->get_item_reference());
			}
			if (!result)
				dump_errors_to_log(result.errors());
		}
		for (auto& item : items)
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
						item->close();
						continue;
						break;
					case STREAMING_TYPE_OBJECT:
						result = create_object_from_storage(stream, std::move(item), storage_type, root);
						break;
					default:
						item->close();
						result = "Invalid serialization type!";
					}
				}
				else
					result.register_error("Error in deserialization from " + item->get_item_reference());
			}
			else
			{// we had an error
				result.register_error("Error in opening item " + item->get_item_reference());
			}
			if (!result)
				dump_errors_to_log(result.errors());
		}
	}
	else
	{
		result.register_error("Error listing storage");
	}
	return result;
}

rx_result configuration_storage_builder::create_object_from_storage (base_meta_reader& stream, rx_storage_item_ptr&& storage, meta::rx_storage_type storage_type, platform_root::smart_ptr root)
{
	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_OBJECT, target_type);
	if (!result)
		return result;

	directory_creator creator;
	auto dir = creator.get_or_create_direcotry(root, meta.get_path());
	if (dir)
	{
		meta.storage_info.assign_storage(storage_type);

		switch (target_type)
		{
			// objects
		case rx_item_type::rx_object:
			result = create_concrete_object_from_storage(meta, stream, dir, std::move(storage), tl::type2type<object_type>());
			break;
		case rx_item_type::rx_port:
			result = create_concrete_object_from_storage(meta, stream, dir, std::move(storage), tl::type2type<port_type>());
			break;
		case rx_item_type::rx_application:
			result = create_concrete_object_from_storage(meta, stream, dir, std::move(storage), tl::type2type<application_type>());
			break;
		case rx_item_type::rx_domain:
			result = create_concrete_object_from_storage(meta, stream, dir, std::move(storage), tl::type2type<domain_type>());
			break;
		default:
			storage->close();
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

rx_result configuration_storage_builder::create_type_from_storage (base_meta_reader& stream, rx_storage_item_ptr&& storage, meta::rx_storage_type storage_type, platform_root::smart_ptr root)
{
	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_TYPE, target_type);
	if (!result)
		return result;

	directory_creator creator;
	auto dir = creator.get_or_create_direcotry(root, meta.get_path());
	if (dir)
	{
		meta.storage_info.assign_storage(storage_type);

		switch (target_type)
		{
		// object types
		case rx_item_type::rx_object_type:
			result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<object_type>());
			break;
		case rx_item_type::rx_port_type:
			result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<port_type>());
			break;
		case rx_item_type::rx_application_type:
			result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<application_type>());
			break;
		case rx_item_type::rx_domain_type:
			result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<domain_type>());
			break;
		// simple types
		case rx_item_type::rx_struct_type:
			result = create_concrete_simple_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<struct_type>());
			break;
		case rx_item_type::rx_variable_type:
			result = create_concrete_simple_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<variable_type>());
			break;
		// variable sub-types
		case rx_item_type::rx_source_type:
			result = create_concrete_simple_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<source_type>());
			break;
		case rx_item_type::rx_filter_type:
			result = create_concrete_simple_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<filter_type>());
			break;
		case rx_item_type::rx_event_type:
			result = create_concrete_simple_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<event_type>());
			break;
		case rx_item_type::rx_mapper_type:
			result = create_concrete_simple_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<mapper_type>());
			break;
		default:
			storage->close();
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

void configuration_storage_builder::dump_errors_to_log (const string_array& errors)
{
	for (auto& err : errors)
		BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
}


template<class T>
rx_result configuration_storage_builder::create_concrete_type_from_storage(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>)
{
	auto created = rx_create_reference<T>();
	created->meta_info() = meta;
	auto result = created->deserialize_definition(stream, STREAMING_TYPE_TYPE);
	storage->close();
	if (result)
	{
		auto create_result = model::algorithms::types_model_algorithm<T>::create_type_sync(
			"", "", created, dir
			, created->meta_info().get_attributes());
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			BUILD_LOG_TRACE("configuration_storage_builder", 100, ("Created "s + rx_item_type_name(T::type_id) + " "s + rx_type_item->get_name()).c_str());
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(T::type_id) + " " + meta.get_name());
			return create_result.errors();
		}
	}
	return result;
}

template<class T>
rx_result configuration_storage_builder::create_concrete_simple_type_from_storage(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>)
{
	auto created = rx_create_reference<T>();
	created->meta_info() = meta;
	auto result = created->deserialize_definition(stream, STREAMING_TYPE_TYPE);
	storage->close();
	if (result)
	{
		auto create_result = model::algorithms::simple_types_model_algorithm<T>::create_type_sync(
			"", "", created, dir
			, created->meta_info().get_attributes());
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			BUILD_LOG_TRACE("configuration_storage_builder", 100, ("Created "s + rx_item_type_name(T::type_id) + " "s + rx_type_item->get_name()).c_str());
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(T::type_id) + " " + meta.get_name());
			return create_result.errors();
		}
	}
	return result;
}


template<class T>
rx_result configuration_storage_builder::create_concrete_object_from_storage(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>)
{
	auto init_data = std::make_unique< data::runtime_values_data>();
	bool ret = false;
	if (stream.start_object("def"))
	{
		if (stream.read_init_values("values", *init_data))
		{
			ret = true;
		}
	}
	storage->close();
	if (ret)
	{
		auto create_result = model::algorithms::runtime_model_algorithm<T>::create_runtime_sync(
			meta, init_data.release(), dir, rx_object_ptr::null_ptr);
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			BUILD_LOG_TRACE("configuration_storage_builder", 100, ("Created "s + rx_item_type_name(T::RType::type_id) + " "s + rx_type_item->get_name()).c_str());
			return true;
		}
		else
		{
			create_result.register_error("Error creating "s + rx_item_type_name(T::RType::type_id) + " " + meta.get_name());
			return create_result.errors();
		}
	}
	else
		return "Error reading initialize values";
}

// Class sys_internal::builders::storage::directory_creator


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
} // namespace sys_internal

