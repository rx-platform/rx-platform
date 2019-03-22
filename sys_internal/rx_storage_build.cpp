

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


namespace sys_internal {

namespace builders {

namespace storage {

// Class sys_internal::builders::storage::configuration_storage_builder 

configuration_storage_builder::configuration_storage_builder()
{
}


configuration_storage_builder::~configuration_storage_builder()
{
}



rx_result configuration_storage_builder::do_build (platform_root::smart_ptr root)
{
	using storage_items = std::vector<rx_storage_item_ptr>;
	auto storage = rx_gate::instance().get_host()->get_system_storage();
	auto result = build_from_storage(root, *storage);
	if (result)
	{
		BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from system storage done.");
		storage = rx_gate::instance().get_host()->get_user_storage();
		result = build_from_storage(root, *storage);
		if (result)
		{
			BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from user storage done.");
			storage = rx_gate::instance().get_host()->get_test_storage();
			if (!storage->is_valid_storage())
			{
				// just return true, we can run without test storage
				BUILD_LOG_WARNING("configuration_storage_builder", 900, "Test storage not initialized!");
			}
			else
			{
				result = build_from_storage(root, *storage);
				if (result)
				{

					BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from test storage done.");
				}
				else
				{
					for (auto& err : result.errors())
						BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
					BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from test storage");
					// we still can start without test storage so just return success!!!
					result = true;
				}
			}
		}
		else
		{
			for (auto& err : result.errors())
				BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
			BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from user storage");
		}
	}
	else
	{
		for (auto& err : result.errors())
			BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
		BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from system storage");
	}
	return result;
}

rx_result configuration_storage_builder::build_from_storage (platform_root::smart_ptr root, rx_platform::storage_base::rx_platform_storage& storage)
{
	if (!storage.is_valid_storage())
		return "Storage not initialized!";

	directory_creator creator;

	using storage_items = std::vector<rx_storage_item_ptr>;
	storage_items items;
	auto result = storage.list_storage(items);
	if (result)
	{
		for (auto& item : items)
		{
			auto dir = creator.get_or_create_direcotry(root, item->get_path());
			if (dir)
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
							result = create_type_from_storage(stream, dir, std::move(item));
							break;
						case STREAMING_TYPE_OBJECT:
							result = create_object_from_storage(stream, dir, std::move(item));
							break;
						default:
							result = "Invalid serialization type!";
						}
					}
					if (!result)
						break;
				}
			}
			else
			{
				result = dir.errors();
				result.register_error("Error retrieving directory for the new item!");
			}
			if (!result)
				break;
		}
	}
	else
	{
		result.register_error("Error listing storage");		
	}
	return result;
}

rx_result configuration_storage_builder::create_object_from_storage (base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage)
{
	meta::meta_data meta;
	string_type target_type;
	auto result = stream.start_object("Meta");
	if (!result)
		return result;
	result = meta.deserialize_meta_data(stream, STREAMING_TYPE_OBJECT, target_type);
	if (!result)
		return result;
	result = stream.end_object();
	return result;
}

rx_result configuration_storage_builder::create_type_from_storage (base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage)
{
	meta::meta_data meta;
	string_type target_type;
	auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_TYPE, target_type);
	if (!result)
		return result;
	if (target_type == "object_type")
	{
		result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<object_type>());
	}
	else if (target_type == "port_type")
	{
		result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<port_type>());
	}
	else if (target_type == "application_type")
	{
		result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<application_type>());
	}
	else if (target_type == "domain_type")
	{
		result = create_concrete_type_from_storage(meta, stream, dir, std::move(storage), tl::type2type<domain_type>());
	}
	else
	{
		storage->close();
		result = "Unknown type: "s + target_type;
	}
	return result;
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
		created->assign_storage(std::move(storage));
		auto create_result = model::platform_types_manager::instance().create_type_helper<T>(
			"", "", created, dir
			, ns::namespace_item_attributes::namespace_item_full_access
			, tl::type2type<T>());
		if (create_result)
		{
			auto rx_type_item = create_result.value()->get_item_ptr();
			BUILD_LOG_INFO("configuration_storage_builder", 100, ("Created "s + T::type_name + " "s + rx_type_item->get_name()).c_str());
			return true;
		}
		else
		{
			return create_result.errors();
		}
	}
	return result;
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
