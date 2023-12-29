

/****************************************************************************
*
*  api\rx_platform_api.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_platform_api
#include "api/rx_platform_api.h"


#include "lib/rx_ser_bin.h"
#include "system/meta/rx_meta_algorithm.h"
#include "model/rx_model_algorithms.h"
#include "system/server/rx_directory_cache.h"
#include "system/runtime/rx_extern_blocks.h"
#include "system/runtime/rx_extern_items.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "system/libraries/rx_plugin.h"
#include "system/storage_base/rx_storage.h"



template<class T>
rx_result create_concrete_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, tl::type2type<T>)
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
rx_result create_concrete_simple_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, tl::type2type<T>)
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

rx_result create_concrete_relation_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir)
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


rx_result create_concrete_data_type_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir)
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
rx_result create_type_from_stream(rx_item_type target_type, const char* name, const char* path
	, const rx_node_id_struct* id, const rx_node_id_struct* parent
	, uint32_t version, rx_time_struct modified, base_meta_reader& stream)
{
	meta_data meta;
	rx_result result;
	meta.path = path;
	meta.name = name;
	meta.id = rx_node_id(id);
	meta.parent = rx_node_id(parent);
	meta.version = version;
	meta.attributes = namespace_item_attributes::namespace_item_system;
	meta.created_time = rx_time::now();
	meta.modified_time = rx_time(modified);

	auto dir = ns::rx_directory_cache::instance().get_or_create_directory(meta.path);
	if (dir)
	{
		switch (target_type)
		{
			// object types
		case rx_item_type::rx_object_type:
			result = create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<object_type>());
			break;
		case rx_item_type::rx_port_type:
			result = create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<port_type>());
			break;
		case rx_item_type::rx_application_type:
			result = create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<application_type>());
			break;
		case rx_item_type::rx_domain_type:
			result = create_concrete_type_from_stream(meta, stream, dir.value(), tl::type2type<domain_type>());
			break;
			// simple types
		case rx_item_type::rx_struct_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<struct_type>());
			break;
		case rx_item_type::rx_variable_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<variable_type>());
			break;
			// variable sub-types
		case rx_item_type::rx_source_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<source_type>());
			break;
		case rx_item_type::rx_filter_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<filter_type>());
			break;
		case rx_item_type::rx_event_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<event_type>());
			break;
		case rx_item_type::rx_mapper_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<mapper_type>());
			break;
		case rx_item_type::rx_relation_type:
			result = create_concrete_relation_type_from_stream(meta, stream, dir.value());
			break;
		case rx_item_type::rx_method_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<method_type>());
			break;
		case rx_item_type::rx_program_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<program_type>());
			break;
		case rx_item_type::rx_display_type:
			result = create_concrete_simple_type_from_stream(meta, stream, dir.value(), tl::type2type<display_type>());
			break;
		case rx_item_type::rx_data_type:
			result = create_concrete_data_type_from_stream(meta, stream, dir.value());
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


template<class T>
rx_result create_concrete_runtime_from_stream(meta_data& meta, base_meta_reader& stream, rx_directory_ptr dir, tl::type2type<T>)
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

rx_result create_runtime_from_stream(rx_item_type target_type, const char* name, const char* path
	, const rx_node_id_struct* id, const rx_node_id_struct* parent
	, uint32_t version, rx_time_struct modified, base_meta_reader& stream)
{
	meta_data meta;
	rx_result result;
	meta.path = path;
	meta.name = name;
	meta.id = rx_node_id(id);
	meta.parent = rx_node_id(parent);
	meta.version = version;
	meta.attributes = namespace_item_attributes::namespace_item_system;
	meta.created_time = rx_time::now();
	meta.modified_time = rx_time(modified);

	auto dir = ns::rx_directory_cache::instance().get_or_create_directory(meta.path);
	if (dir)
	{
		switch (target_type)
		{
			// object types
		case rx_item_type::rx_object:
			result = create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<object_type>());
			break;
		case rx_item_type::rx_port:
			result = create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<port_type>());
			break;
		case rx_item_type::rx_application:
			result = create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<application_type>());
			break;
		case rx_item_type::rx_domain:
			result = create_concrete_runtime_from_stream(meta, stream, dir.value(), tl::type2type<domain_type>());
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



#ifdef __cplusplus
extern "C" {
#endif

	RX_PLATFORM_API void rxWriteLog(uintptr_t plugin, int type, const char* library, const char* source, uint16_t level, const char* code, const char* message)
	{
		rx_platform::log::log_object::instance().log_event_fast((rx_platform::log::log_event_type)type, library, source, level, code, log_callback_func_t(), message);
	}

	RX_PLATFORM_API rx_result_struct rxRegisterItem(uintptr_t plugin, uint8_t item_type, const char* name, const char* path
		, const rx_node_id_struct* id, const rx_node_id_struct* parent
		, uint32_t version, rx_time_struct modified, uint32_t stream_version, const uint8_t* data, size_t count)
	{
		if (count < 0x10 || data == nullptr)
			return rx_result("Invalid data!").move();
		auto plugin_obj = library::rx_dynamic_plugin::get_registered_plugin(plugin);
		if(!plugin_obj)
			return rx_result("Invalid plugin!").move();

		memory::std_buffer buffer;
		buffer.push_data(data, count);
		rx::serialization::std_buffer_reader stream(buffer, stream_version > 0 ? stream_version : plugin_obj->get_stream_version());

		return create_type_from_stream((rx_item_type)item_type, name, path, id, parent, version, modified, stream).move();
	}
	RX_PLATFORM_API rx_result_struct rxRegisterRuntimeItem(uintptr_t plugin, uint8_t item_type, const char* name, const char* path
		, const rx_node_id_struct* id, const rx_node_id_struct* parent
		, uint32_t version, rx_time_struct modified, uint32_t stream_version, const uint8_t* data, size_t count)
	{
		if (count < 0x10 || data == nullptr)
			return rx_result("Invalid data!").move();
		auto plugin_obj = library::rx_dynamic_plugin::get_registered_plugin(plugin);
		if (!plugin_obj)
			return rx_result("Invalid plugin!").move();

		memory::std_buffer buffer;
		buffer.push_data(data, count);
		rx::serialization::std_buffer_reader stream(buffer, stream_version > 0 ? stream_version : plugin_obj->get_stream_version());

		return create_runtime_from_stream((rx_item_type)item_type, name, path, id, parent, version, modified, stream).move();
		//return create_type_from_stream((rx_item_type)item_type, name, path, id, parent, version, stream).move();
	}


	RX_PLATFORM_API void rxLockRuntimeManager()
	{
		g_runtime_lock->lock();
	}
	RX_PLATFORM_API void rxUnlockRuntimeManager()
	{
		g_runtime_lock->unlock();
	}

	RX_PLATFORM_API rx_result_struct rxRegisterSourceRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_source_register_data construct_data)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<source_type>().register_constructor(
			id, [construct_data] {
				return rx_create_reference<runtime::blocks::extern_source_runtime<plugin_source_runtime_struct> >(construct_data.constructor());
			});
		return result.move();

	}
	RX_PLATFORM_API rx_result_struct rxRegisterMapperRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_mapper_constructor_t construct_func)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<mapper_type>().register_constructor(
			id, [construct_func] {

				auto construct_result = construct_func();

				return rx_create_reference<runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct> >(construct_func());
			});
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxRegisterMapperRuntime3(uintptr_t plugin, const rx_node_id_struct* id, rx_mapper_constructor3_t construct_func)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<mapper_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3> >(construct_func());
			});
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxRegisterFilterRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_filter_constructor_t construct_func)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<runtime::blocks::extern_filter_runtime>(construct_func());
			});
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxRegisterStructRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_struct_constructor_t construct_func)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<struct_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<runtime::blocks::extern_struct_runtime>(construct_func());
			});
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxRegisterVariableRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_variable_constructor_t construct_func)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<runtime::blocks::extern_variable_runtime>(construct_func());
			});
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxRegisterEventRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_event_constructor_t construct_func)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<event_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<runtime::blocks::extern_event_runtime>(construct_func());
			});
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxRegisterMethodRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_method_constructor_t construct_func)
	{
		auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<method_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<logic::extern_method_runtime>(construct_func());
			});
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxRegisterProgramRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_program_constructor_t construct_func)
	{
		return rx_result(RX_NOT_IMPLEMENTED).move();
		/*auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<program_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<runtime::blocks::extern_program_runtime>(construct_func());
			});
		return result.move();*/
	}
	RX_PLATFORM_API rx_result_struct rxRegisterDisplayRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_display_constructor_t construct_func)
	{
		return rx_result(RX_NOT_IMPLEMENTED).move();
		/*auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
			id, [construct_func] {
				return rx_create_reference<runtime::blocks::extern_display_runtime>(construct_func());
			});
		return result.move();*/
	}

	RX_PLATFORM_API rx_result_struct rxRegisterObjectRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_object_register_data construct_data)
	{
		std::function<constructed_data_t<typename object_type::RImplPtr>(const rx_node_id&)> func =
			[construct_data](const rx_node_id& new_id) // full constructor function
		{
			constructed_data_t<typename object_type::RImplPtr> ret;
			auto new_ptr = rx_create_reference<runtime::items::extern_object_runtime>(construct_data.constructor());
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
			if (construct_data.register_func)
			{
				lock_reference_struct* extern_data = new_ptr->get_extern_ref();
				ret.register_f = [construct_data, extern_data](const rx_node_id& id)
				{
					construct_data.register_func(id.c_ptr(), extern_data);
				};
			}
			if (construct_data.unregister_func)
			{
				ret.unregister_f = [construct_data](const rx_node_id& id)
				{
					construct_data.unregister_func(id.c_ptr());
				};
			}
			return ret;
		};
		auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
			id, func);
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxRegisterApplicationRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_application_register_data construct_data)
	{
		std::function<constructed_data_t<typename application_type::RImplPtr>(const rx_node_id&)> func =
			[construct_data](const rx_node_id& new_id) // full constructor function
		{
			constructed_data_t<typename application_type::RImplPtr> ret;
			auto new_ptr = rx_create_reference<runtime::items::extern_application_runtime>(construct_data.constructor());
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
			if (construct_data.register_func)
			{
				lock_reference_struct* extern_data = new_ptr->get_extern_ref();
				ret.register_f = [construct_data, extern_data](const rx_node_id& id)
				{
					construct_data.register_func(id.c_ptr(), extern_data);
				};
			}
			if (construct_data.unregister_func)
			{
				ret.unregister_f = [construct_data](const rx_node_id& id)
				{
					construct_data.unregister_func(id.c_ptr());
				};
			}
			return ret;
		};
		auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<application_type>().register_constructor(
			id, func);
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxRegisterDomainRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_domain_register_data construct_data)
	{
		std::function<constructed_data_t<typename domain_type::RImplPtr>(const rx_node_id&)> func =
			[construct_data](const rx_node_id& new_id) // full constructor function
		{
			constructed_data_t<typename domain_type::RImplPtr> ret;
			auto new_ptr = rx_create_reference<runtime::items::extern_domain_runtime>(construct_data.constructor());
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
			if (construct_data.register_func)
			{
				lock_reference_struct* extern_data = new_ptr->get_extern_ref();
				ret.register_f = [construct_data, extern_data](const rx_node_id& id)
				{
					construct_data.register_func(id.c_ptr(), extern_data);
				};
			}
			if (construct_data.unregister_func)
			{
				ret.unregister_f = [construct_data](const rx_node_id& id)
				{
					construct_data.unregister_func(id.c_ptr());
				};
			}
			return ret;
		};
		auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<domain_type>().register_constructor(
			id, func);
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxRegisterPortRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_port_register_data construct_data)
	{
		std::function<constructed_data_t<typename port_type::RImplPtr>(const rx_node_id&)> func =
			[construct_data](const rx_node_id& new_id) // full constructor function
		{
			constructed_data_t<typename port_type::RImplPtr> ret;
			auto new_ptr = rx_create_reference<runtime::items::extern_port_runtime>(construct_data.constructor());
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
			if (construct_data.register_func)
			{
				lock_reference_struct* extern_data = new_ptr->get_extern_ref();
				ret.register_f = [construct_data, extern_data](const rx_node_id& id)
				{
					construct_data.register_func(id.c_ptr(), extern_data);
				};
			}
			if (construct_data.unregister_func)
			{
				ret.unregister_f = [construct_data](const rx_node_id& id)
				{
					construct_data.unregister_func(id.c_ptr());
				};
			}
			return ret;
		};
		auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			id, func);
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxRegisterRelationRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_relation_register_data construct_data)
	{
		std::function<constructed_data_t<typename relation_type::RImplPtr>(const rx_node_id&)> func =
			[construct_data](const rx_node_id& new_id) // full constructor function
		{
			constructed_data_t<typename relation_type::RImplPtr> ret;
			auto new_ptr = rx_create_reference<runtime::relations::extern_relation_runtime>(construct_data.constructor());
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
			if (construct_data.register_func)
			{
				lock_reference_struct* extern_data = new_ptr->get_extern_ref();
				ret.register_f = [construct_data, extern_data](const rx_node_id& id)
				{
					construct_data.register_func(id.c_ptr(), extern_data);
				};
			}
			if (construct_data.unregister_func)
			{
				ret.unregister_f = [construct_data](const rx_node_id& id)
				{
					construct_data.unregister_func(id.c_ptr());
				};
			}
			return ret;
		};
		auto result = rx_internal::model::platform_types_manager::instance().get_relations_repository().register_constructor(
			id, func);
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxInitCtxBindItem(init_ctx_ptr ctx, const char* path, runtime_handle_t* handle, runtime_ctx_ptr* rt_ctx, bind_callback_data* callback)
	{
		runtime::runtime_init_context* self = (runtime::runtime_init_context*)ctx;
		*rt_ctx = self->context;
		auto result = self->bind_item(path
			, callback ? [callback](const rx_value& val)
			{
				callback->callback(callback->target, val.c_ptr());
			} : runtime::tag_blocks::binded_callback_t());
		if (result)
		{
			*handle = result.value();
			return rx_result(true).move();
		}
		else
		{
			*handle = 0;
			return rx_result(result.errors()).move();
		}
	}

	RX_PLATFORM_API rx_result_struct rxInitCtxConnectItem(init_ctx_ptr ctx, const char* path, uint32_t rate, runtime_handle_t* handle, runtime_ctx_ptr* rt_ctx, connect_callback_data* callback)
	{
		runtime::runtime_init_context* self = (runtime::runtime_init_context*)ctx;
		*rt_ctx = self->context;
		auto result = self->connect_item(path,  rate
			, callback ? [callback](const rx_value& val)
			{
				callback->callback(callback->target, val.c_ptr());
			} : runtime::tag_blocks::binded_callback_t());
		if (result)
		{
			*handle = result.value();
			return rx_result(true).move();
		}
		else
		{
			*handle = 0;
			return rx_result(result.errors()).move();
		}
	}
	RX_PLATFORM_API const char* rxInitCtxGetCurrentPath(init_ctx_ptr ctx)
	{
		runtime::runtime_init_context* self = (runtime::runtime_init_context*)ctx;
		return self->path.get_current_path().c_str();
	}
	RX_PLATFORM_API rx_result_struct rxInitCtxGetLocalValue(init_ctx_ptr ctx, const char* path, struct typed_value_type* val)
	{
		rx_simple_value temp;
		runtime::runtime_init_context* self = (runtime::runtime_init_context*)ctx;
		auto result = self->get_item(path, temp);
		if (result)
		{
			*val = temp.move();
		}
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxInitCtxSetLocalValue(init_ctx_ptr ctx, const char* path, struct typed_value_type val)
	{
		rx_simple_value temp(val);
		runtime::runtime_start_context* self = (runtime::runtime_start_context*)ctx;
		auto result = self->set_item(path, std::move(temp));
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxInitCtxGetMappingValues(init_ctx_ptr ctx, const rx_node_id_struct* id, const char* path, values_array_struct* vals)
	{
		runtime::runtime_init_context* self = (runtime::runtime_init_context*)ctx;
		auto result = self->mappers.get_mapping_values(id, path);
		std::vector<typed_value_type> temp_vals;
		if (!result.empty())
		{
			temp_vals.reserve(result.size());
			for (auto& one : result)
			{
				temp_vals.emplace_back(one.move());
			}
			rx_init_values_array_struct(vals, &temp_vals[0], temp_vals.size());
		}
		else
		{
			rx_init_values_array_struct(vals, nullptr, 0);
		}
		rx_result_struct ret;
		rx_init_result_struct(&ret);
		return ret;
	}

	RX_PLATFORM_API rx_result_struct rxInitCtxGetSourceValues(init_ctx_ptr ctx, const rx_node_id_struct* id, const char* path, values_array_struct* vals)
	{
		runtime::runtime_init_context* self = (runtime::runtime_init_context*)ctx;
		auto result = self->sources.get_source_values(id, path);
		std::vector<typed_value_type> temp_vals;
		if (!result.empty())
		{
			temp_vals.reserve(result.size());
			for (auto& one : result)
			{
				temp_vals.emplace_back(one.move());
			}
			rx_init_values_array_struct(vals, &temp_vals[0], temp_vals.size());
		}
		else
		{
			rx_init_values_array_struct(vals, nullptr, 0);
		}
		rx_result_struct ret;
		rx_init_result_struct(&ret);
		return ret;
	}

	RX_PLATFORM_API void rxInitCtxGetItemMeta(init_ctx_ptr ctx, const rx_node_id_struct** id, const char** path, const char** name)
	{
		runtime::runtime_init_context* self = (runtime::runtime_init_context*)ctx;
		*id = self->meta.id.c_ptr();
		*path = self->meta.path.c_str();
		*name = self->meta.name.c_str();
	}

	RX_PLATFORM_API rx_result_struct rxCtxGetValue(runtime_ctx_ptr ctx, runtime_handle_t handle, typed_value_type* val)
	{
		runtime::runtime_process_context* self = (runtime::runtime_process_context*)ctx;
		values::rx_simple_value temp_val;
		auto result = self->get_value(handle, temp_val);
		if (result)
		{
			*val = temp_val.move();
		}
		return result.move();
	}

	RX_PLATFORM_API rx_result_struct rxCtxSetValue(runtime_ctx_ptr ctx, runtime_handle_t handle, typed_value_type val)
	{
		runtime::runtime_process_context* self = (runtime::runtime_process_context*)ctx;
		return self->set_value(handle, values::rx_simple_value(val)).move();
	}

	RX_PLATFORM_API rx_result_struct rxCtxWriteConnected(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type va, runtime_transaction_id_t trans_id)
	{
		runtime::runtime_process_context* self = (runtime::runtime_process_context*)ctx;
		return self->write_connected(handle, values::rx_simple_value(va), trans_id).move();
	}

	RX_PLATFORM_API rx_result_struct rxCtxExecuteConnected(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type va, runtime_transaction_id_t trans_id)
	{
		runtime::runtime_process_context* self = (runtime::runtime_process_context*)ctx;
		return self->execute_connected(handle, values::rx_simple_value(va), trans_id).move();
	}

	RX_PLATFORM_API void rxCtxSetRemotePending(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type val)
	{
		runtime::runtime_process_context* self = (runtime::runtime_process_context*)ctx;
		runtime::async_data data;
		data.handle = handle;
		data.value = values::rx_simple_value(val);
		self->async_value_pending(std::move(data));

	}
	RX_PLATFORM_API void rxCtxSetAsyncPending(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type val)
	{
		runtime::runtime_process_context* self = (runtime::runtime_process_context*)ctx;
		runtime::async_data data;
		data.handle = handle;
		data.value = values::rx_simple_value(val);
		self->async_value_pending(std::move(data));

	}
	RX_PLATFORM_API const char* rxStartCtxGetCurrentPath(start_ctx_ptr ctx)
	{
		runtime::runtime_start_context* self = (runtime::runtime_start_context*)ctx;
		return self->path.get_current_path().c_str();
	}
	RX_PLATFORM_API runtime_handle_t rxStartCtxCreateTimer(start_ctx_ptr ctx, int type, plugin_job_struct* job, uint32_t period)
	{
		runtime::runtime_start_context* self = (runtime::runtime_start_context*)ctx;
		switch (type)
		{
		case RX_TIMER_REGULAR:
			return rx_platform::extern_timers::instance().create_timer(job, period, self->queue);
		case RX_TIMER_CALC:
			return rx_platform::extern_timers::instance().create_calc_timer(job, period, self->queue);
		case RX_TIMER_IO:
			return rx_platform::extern_timers::instance().create_timer(job, period, rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_IO));
		default:
			return 0;//unknown type
		}
	}
	RX_PLATFORM_API rx_result_struct rxStartCtxGetLocalValue(start_ctx_ptr ctx, const char* path, struct typed_value_type* val)
	{
		rx_simple_value temp;
		runtime::runtime_start_context* self = (runtime::runtime_start_context*)ctx;
		auto result = self->get_item(path, temp);
		if (result)
		{
			*val = temp.move();
		}
		return result.move();
	}
	RX_PLATFORM_API rx_result_struct rxStartCtxSetLocalValue(start_ctx_ptr ctx, const char* path, struct typed_value_type val)
	{
		rx_simple_value temp(val);
		runtime::runtime_start_context* self = (runtime::runtime_start_context*)ctx;
		auto result = self->set_item(path, std::move(temp));
		return result.move();
	}



	RX_PLATFORM_API rx_result_struct rxStartCtxSubscribeRelation(start_ctx_ptr ctx, const char* name, relation_subscriber_data* callback)
	{
		runtime::runtime_start_context* self = (runtime::runtime_start_context*)ctx;
		return self->register_extern_relation_subscriber(name, callback).move();
	}



	RX_PLATFORM_API rx_result_struct rxRegisterStorageType(uintptr_t plugin, const char* prefix, rx_storage_constructor_t construct_func)
	{
		storage_base::rx_platform_storage_type* temp = new storage_base::rx_plugin_storage_type(prefix, construct_func);
		auto ret = rx_gate::instance().get_host()->register_storage_type(prefix, temp);
		if (!ret)
		{
			delete temp;
		}
		return ret.move();
	}


#ifdef __cplusplus
}
#endif

namespace rx_platform
{
platform_api g_api;
platform_api2 g_api2;
platform_api3 g_api3;

namespace api
{

void bind_plugins_dynamic_api()
{
	g_api.general.pWriteLog = rxWriteLog;
	g_api.general.pRegisterItem = rxRegisterItem;
	g_api.general.prxRegisterRuntimeItem = rxRegisterRuntimeItem;

	g_api.general.prxLockRuntimeManager = rxLockRuntimeManager;
	g_api.general.prxUnlockRuntimeManager = rxUnlockRuntimeManager;

	g_api.runtime.prxRegisterSourceRuntime = rxRegisterSourceRuntime;
	g_api.runtime.prxRegisterMapperRuntime = rxRegisterMapperRuntime;
	g_api.runtime.prxRegisterFilterRuntime = rxRegisterFilterRuntime;
	g_api.runtime.prxRegisterStructRuntime = rxRegisterStructRuntime;
	g_api.runtime.prxRegisterVariableRuntime = rxRegisterVariableRuntime;
	g_api.runtime.prxRegisterEventRuntime = rxRegisterEventRuntime;

	g_api.runtime.prxRegisterMethodRuntime = rxRegisterMethodRuntime;
	g_api.runtime.prxRegisterDisplayRuntime = rxRegisterDisplayRuntime;
	g_api.runtime.prxRegisterProgramRuntime = rxRegisterProgramRuntime;

	g_api.runtime.prxRegisterObjectRuntime = rxRegisterObjectRuntime;
	g_api.runtime.prxRegisterDomainRuntime = rxRegisterDomainRuntime;
	g_api.runtime.prxRegisterApplicationRuntime = rxRegisterApplicationRuntime;
	g_api.runtime.prxRegisterPortRuntime = rxRegisterPortRuntime;

	g_api.runtime.prxRegisterRelationRuntime = rxRegisterRelationRuntime;

	g_api.runtime.prxInitCtxBindItem = rxInitCtxBindItem;
	g_api.runtime.prxInitCtxGetCurrentPath = rxInitCtxGetCurrentPath;
	g_api.runtime.prxInitCtxGetLocalValue = rxInitCtxGetLocalValue;
	g_api.runtime.prxInitCtxSetLocalValue = rxInitCtxSetLocalValue;
	g_api.runtime.prxInitCtxGetMappingValues = rxInitCtxGetMappingValues;
	g_api.runtime.prxInitCtxGetSourceValues = rxInitCtxGetSourceValues;
	g_api.runtime.prxInitCtxGetItemMeta = rxInitCtxGetItemMeta;


	g_api.runtime.prxStartCtxGetCurrentPath = rxStartCtxGetCurrentPath;
	g_api.runtime.prxStartCtxCreateTimer = rxStartCtxCreateTimer;
	g_api.runtime.prxStartCtxGetLocalValue = rxStartCtxGetLocalValue;
	g_api.runtime.prxStartCtxSubscribeRelation = rxStartCtxSubscribeRelation;

	g_api.runtime.prxCtxGetValue = rxCtxGetValue;
	g_api.runtime.prxCtxSetValue = rxCtxSetValue;
	g_api.runtime.prxCtxSetRemotePending = rxCtxSetRemotePending;


	g_api2.general.pWriteLog = rxWriteLog;
	g_api2.general.pRegisterItem = rxRegisterItem;
	g_api2.general.prxRegisterRuntimeItem = rxRegisterRuntimeItem;

	g_api2.general.prxLockRuntimeManager = rxLockRuntimeManager;
	g_api2.general.prxUnlockRuntimeManager = rxUnlockRuntimeManager;

	g_api2.runtime.prxRegisterSourceRuntime = rxRegisterSourceRuntime;
	g_api2.runtime.prxRegisterMapperRuntime = rxRegisterMapperRuntime;
	g_api2.runtime.prxRegisterFilterRuntime = rxRegisterFilterRuntime;
	g_api2.runtime.prxRegisterStructRuntime = rxRegisterStructRuntime;
	g_api2.runtime.prxRegisterVariableRuntime = rxRegisterVariableRuntime;
	g_api2.runtime.prxRegisterEventRuntime = rxRegisterEventRuntime;

	g_api2.runtime.prxRegisterMethodRuntime = rxRegisterMethodRuntime;
	g_api2.runtime.prxRegisterDisplayRuntime = rxRegisterDisplayRuntime;
	g_api2.runtime.prxRegisterProgramRuntime = rxRegisterProgramRuntime;

	g_api2.runtime.prxRegisterObjectRuntime = rxRegisterObjectRuntime;
	g_api2.runtime.prxRegisterDomainRuntime = rxRegisterDomainRuntime;
	g_api2.runtime.prxRegisterApplicationRuntime = rxRegisterApplicationRuntime;
	g_api2.runtime.prxRegisterPortRuntime = rxRegisterPortRuntime;

	g_api2.runtime.prxRegisterRelationRuntime = rxRegisterRelationRuntime;

	g_api2.runtime.prxInitCtxBindItem = rxInitCtxBindItem;
	g_api2.runtime.prxInitCtxGetCurrentPath = rxInitCtxGetCurrentPath;
	g_api2.runtime.prxInitCtxGetLocalValue = rxInitCtxGetLocalValue;
	g_api2.runtime.prxInitCtxSetLocalValue = rxInitCtxSetLocalValue;
	g_api2.runtime.prxInitCtxGetMappingValues = rxInitCtxGetMappingValues;
	g_api2.runtime.prxInitCtxGetSourceValues = rxInitCtxGetSourceValues;
	g_api2.runtime.prxInitCtxGetItemMeta = rxInitCtxGetItemMeta;


	g_api2.runtime.prxStartCtxGetCurrentPath = rxStartCtxGetCurrentPath;
	g_api2.runtime.prxStartCtxCreateTimer = rxStartCtxCreateTimer;
	g_api2.runtime.prxStartCtxGetLocalValue = rxStartCtxGetLocalValue;
	g_api2.runtime.prxStartCtxSubscribeRelation = rxStartCtxSubscribeRelation;

	g_api2.runtime.prxCtxGetValue = rxCtxGetValue;
	g_api2.runtime.prxCtxSetValue = rxCtxSetValue;
	g_api2.runtime.prxCtxSetRemotePending = rxCtxSetRemotePending;

	g_api2.storage.prxRegisterStorageType = rxRegisterStorageType;


	g_api3.general.pWriteLog = rxWriteLog;
	g_api3.general.pRegisterItem = rxRegisterItem;
	g_api3.general.prxRegisterRuntimeItem = rxRegisterRuntimeItem;

	g_api3.general.prxLockRuntimeManager = rxLockRuntimeManager;
	g_api3.general.prxUnlockRuntimeManager = rxUnlockRuntimeManager;

	g_api3.runtime.prxRegisterSourceRuntime = rxRegisterSourceRuntime;
	g_api3.runtime.prxRegisterMapperRuntime = rxRegisterMapperRuntime;
	g_api3.runtime.prxRegisterMapperRuntime3 = rxRegisterMapperRuntime3;
	g_api3.runtime.prxRegisterFilterRuntime = rxRegisterFilterRuntime;
	g_api3.runtime.prxRegisterStructRuntime = rxRegisterStructRuntime;
	g_api3.runtime.prxRegisterVariableRuntime = rxRegisterVariableRuntime;
	g_api3.runtime.prxRegisterEventRuntime = rxRegisterEventRuntime;

	g_api3.runtime.prxRegisterMethodRuntime = rxRegisterMethodRuntime;
	g_api3.runtime.prxRegisterDisplayRuntime = rxRegisterDisplayRuntime;
	g_api3.runtime.prxRegisterProgramRuntime = rxRegisterProgramRuntime;

	g_api3.runtime.prxRegisterObjectRuntime = rxRegisterObjectRuntime;
	g_api3.runtime.prxRegisterDomainRuntime = rxRegisterDomainRuntime;
	g_api3.runtime.prxRegisterApplicationRuntime = rxRegisterApplicationRuntime;
	g_api3.runtime.prxRegisterPortRuntime = rxRegisterPortRuntime;

	g_api3.runtime.prxRegisterRelationRuntime = rxRegisterRelationRuntime;

	g_api3.runtime.prxInitCtxBindItem = rxInitCtxBindItem;
	g_api3.runtime.prxInitCtxGetCurrentPath = rxInitCtxGetCurrentPath;
	g_api3.runtime.prxInitCtxGetLocalValue = rxInitCtxGetLocalValue;
	g_api3.runtime.prxInitCtxSetLocalValue = rxInitCtxSetLocalValue;
	g_api3.runtime.prxInitCtxGetMappingValues = rxInitCtxGetMappingValues;
	g_api3.runtime.prxInitCtxGetSourceValues = rxInitCtxGetSourceValues;
	g_api3.runtime.prxInitCtxGetItemMeta = rxInitCtxGetItemMeta;


	g_api3.runtime.prxStartCtxGetCurrentPath = rxStartCtxGetCurrentPath;
	g_api3.runtime.prxStartCtxCreateTimer = rxStartCtxCreateTimer;
	g_api3.runtime.prxStartCtxGetLocalValue = rxStartCtxGetLocalValue;
	g_api3.runtime.prxStartCtxSubscribeRelation = rxStartCtxSubscribeRelation;

	g_api3.runtime.prxCtxGetValue = rxCtxGetValue;
	g_api3.runtime.prxCtxSetValue = rxCtxSetValue;
	g_api3.runtime.prxCtxSetAsyncPending = rxCtxSetAsyncPending;

	g_api3.storage.prxRegisterStorageType = rxRegisterStorageType;
}

const platform_api_t* get_plugins_dynamic_api()
{
	return &g_api;
}



const platform_api2_t* get_plugins_dynamic_api2()
{
	return &g_api2;
}



const platform_api3_t* get_plugins_dynamic_api3()
{
	return &g_api3;
}


} // api
} // rx_platform


namespace rx_platform {

// Class rx_platform::extern_timer_job 

extern_timer_job::extern_timer_job (plugin_job_struct* extern_data)
      : anchor_(&extern_data->anchor),
        extern_data_(extern_data)
{
}



void extern_timer_job::process ()
{
	extern_data_->process(extern_data_->anchor.target);
}


// Class rx_platform::extern_timers 


extern_timers& extern_timers::instance ()
{
	static extern_timers g_obj;
	return g_obj;
}

runtime_handle_t extern_timers::create_timer (plugin_job_struct* job_impl, uint32_t period, threads::job_thread* pool)
{
	runtime_handle_t ret = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();
	extern_timer_job::smart_ptr job_ptr = rx_create_reference<extern_timer_job>(job_impl);
	{
		locks::auto_lock_t _(&lock_);
		auto ret_val = timers_.emplace(ret, job_ptr);
		if (!ret_val.second)
			return 0;
	}
	rx_internal::infrastructure::server_runtime::instance().append_timer_job(job_ptr, pool);
	if (period > 0)
		job_ptr->start(period);
	return ret;
}

runtime_handle_t extern_timers::create_calc_timer (plugin_job_struct* job_impl, uint32_t period, threads::job_thread* pool)
{
	runtime_handle_t ret = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();
	extern_timer_job::smart_ptr job_ptr = rx_create_reference<extern_timer_job>(job_impl);
	{
		locks::auto_lock_t _(&lock_);
		auto ret_val = timers_.emplace(ret, job_ptr);
		if (!ret_val.second)
			return 0;
	}
	rx_internal::infrastructure::server_runtime::instance().append_calculation_job(job_ptr, pool);
	if (period > 0)
		job_ptr->start(period);
	return ret;
}

runtime_handle_t extern_timers::create_io_timer (plugin_job_struct* job_impl, uint32_t period)
{
	runtime_handle_t ret = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();
	extern_timer_job::smart_ptr job_ptr = rx_create_reference<extern_timer_job>(job_impl);
	{
		locks::auto_lock_t _(&lock_);
		auto ret_val = timers_.emplace(ret, job_ptr);
		if (!ret_val.second)
			return 0;
	}
	rx_internal::infrastructure::server_runtime::instance().append_timer_io_job(job_ptr);
	if (period > 0)
		job_ptr->start(period);
	return ret;
}

void extern_timers::start_timer (runtime_handle_t handle, uint32_t period)
{
	extern_timer_job::smart_ptr job_ptr;
	{
		locks::auto_lock_t _(&lock_);

		auto it = timers_.find(handle);
		if (it != timers_.end())
		{
			job_ptr = it->second;
		}
	}
	if (job_ptr)
		job_ptr->start(period);
}

void extern_timers::suspend_timer (runtime_handle_t handle)
{
	extern_timer_job::smart_ptr job_ptr;
	{
		locks::auto_lock_t _(&lock_);

		auto it = timers_.find(handle);
		if (it != timers_.end())
		{
			job_ptr = it->second;
		}
	}
	if (job_ptr)
		job_ptr->suspend();
}

void extern_timers::destroy_timer (runtime_handle_t handle)
{
	extern_timer_job::smart_ptr job_ptr;
	{
		locks::auto_lock_t _(&lock_);

		auto it = timers_.find(handle);
		if (it != timers_.end())
		{
			job_ptr = it->second;
			timers_.erase(it);
		}
	}
	if (job_ptr)
		job_ptr->cancel();
}


// Class rx_platform::extern_job 

extern_job::extern_job (plugin_job_struct* extern_data)
      : extern_data_(extern_data),
        anchor_(&extern_data->anchor)
{
}



void extern_job::process ()
{
	extern_data_->process(extern_data_->anchor.target);
}


// Class rx_platform::extern_period_job 

extern_period_job::extern_period_job (plugin_job_struct* extern_data)
      : anchor_(&extern_data->anchor),
        extern_data_(extern_data)
{
}



void extern_period_job::process ()
{
}


} // namespace rx_platform

