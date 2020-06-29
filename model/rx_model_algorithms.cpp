

/****************************************************************************
*
*  model\rx_model_algorithms.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_model_algorithms
#include "model/rx_model_algorithms.h"

#include "runtime_internal/rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"
#include "lib/rx_lib.h"
#include "sys_internal/rx_internal_ns.h"
#include "api/rx_platform_api.h"



namespace rx_internal {

namespace model {

namespace algorithms {

using namespace rx;

namespace
{
rx_directory_ptr get_root_directory()
{
	return rx_gate::instance().get_root_directory();
}
}
rx_result_with<platform_item_ptr> get_platform_item_sync(rx_node_id id)
{
	meta::meta_data info;
	auto type = model::platform_types_manager::instance().get_types_resolver().get_item_data(id, info);
	if (type == rx_item_type::rx_invalid_type)
		return (id.to_string() + " is not the registered id!");
	else
		return get_platform_item_sync(type, id);
}

rx_result_with<platform_item_ptr> get_platform_item_sync(rx_item_type type, rx_node_id id)
{
	switch (type)
	{
	case rx_item_type::rx_application:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(id, false);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_domain:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(id, false);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_port:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(id, false);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_object:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<object_type>().get_runtime(id, false);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_application_type:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<application_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_domain_type:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<domain_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_port_type:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<port_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_object_type:
		{
			auto result = model::platform_types_manager::instance().get_type_repository<object_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_struct_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<struct_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_variable_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<variable_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_source_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<source_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_filter_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<filter_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_event_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<event_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_mapper_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<mapper_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;

	case rx_item_type::rx_relation_type:
		{
			auto result = model::platform_types_manager::instance().get_relations_repository().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	default:
		{
			return "Not supported type";
		}
	}
}
//helper functions, anonymous namespace i think is perfect for these
namespace
{


rx_result_with<rx_node_id> resolve_some_reference(const rx_item_reference& ref, const ns::rx_directory_resolver& directories, meta_data& info, rx_item_type& ret_type)
{
	rx_node_id ret;
	if (ref.is_node_id())
	{
		ret = ref.get_node_id();
		if (!ret)
		{
			return "Can't resolve " RX_NULL_ITEM_NAME " reference!";
		}
	}
	else
	{
		auto item = directories.resolve_path(ref.get_path());
		if (!item)
		{
			return ref.get_path() + " does not exists!";
		}
		ret = item.get_meta().id;
		if (ret.is_null())
		{// TODO error, item does not have id
			return ref.get_path() + " does not have valid id!";
		}
		// everything is good we resolved it
	}
	// but, is it registered?
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto type = model::platform_types_manager::instance().get_types_resolver().get_item_data(ret, info);
		if (type == rx_item_type::rx_invalid_type)
		{
			return ret.to_string() + " is not the registered id!";
		}
		ret_type = type;
	}
	return ret;
}


template<class typeCache>
rx_result delete_some_type(typeCache& cache, const rx_item_reference& rx_item_reference, rx_transaction_type& transaction)
{
	rx_node_id id;
	string_type name;
	rx_namespace_item item;
	rx_directory_ptr dir = rx_gate::instance().get_root_directory();
	if (!rx_item_reference.is_node_id())
	{
		string_type path = rx_item_reference.get_path();
		item = dir->get_sub_item(path);
		if (!item)
		{// error, item does not exists
			return path + " does not exists";
		}
		id = item.get_meta().id;
		if (id.is_null())
		{// error, item does not have id
			return path + " does not have id";
		}
		dir = dir->get_sub_directory(item.get_meta().path);
		name = item.get_meta().name;
	}
	else
	{
		if(rx_item_reference.is_null())
		{// error, item does not have id
			return "Invalid " RX_NULL_ITEM_NAME " id.";
		}
		id = rx_item_reference.get_node_id();
		meta_data item_meta;
		auto type = platform_types_manager::instance().get_types_resolver().get_item_data(id, item_meta);
		if(type==rx_item_type::rx_invalid_type)
		{// error, item does not exists
			return id.to_string() + " does not exists";
		}
		name = item_meta.name;
		dir = dir->get_sub_directory(item_meta.path);
	}
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto work_item = get_platform_item_sync(typeCache::HType::type_id, id);
		if(!work_item)
		{
			work_item.register_error("Error retrieving type item "s + item.get_meta().get_full_path());
			return work_item.errors();
		}
		auto delete_result = work_item.value()->delete_item();
		if (!delete_result)
		{
			delete_result.register_error("Error deleting type item "s + item.get_meta().get_full_path());
			return delete_result;
		}
	}
	transaction.push([=] {
		if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
		{
			auto work_item = get_platform_item_sync(typeCache::HType::type_id, id);
			if(work_item)
				auto save_result = work_item.value()->save();
		}
	});
	auto ret = dir->delete_item(name);
	if (!ret)
	{// error, didn't deleted runtime
		ret.register_error("Error deleting type from the namespace.");
		return ret;
	}
	transaction.push([dir, item]() mutable {
			auto add_result = dir->add_item(item);
		});
	ret = cache.delete_type(id);
	if (!ret)
	{// error, didn't deleted runtime
		ret.register_error("Error deleting type from the repository.");
		return ret;
	}
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
		META_LOG_INFO("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeCache::HType::get_type_id()) + " "s + item.get_meta().get_full_path());
	else
		META_LOG_TRACE("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeCache::HType::get_type_id()) + " "s + item.get_meta().get_full_path());
	return true;
}
template<class typeCache, class typeType>
rx_result_with<typeType> create_some_type(typeCache& cache, typeType prototype, rx_transaction_type& transaction)
{
	rx_node_id item_id = prototype->meta_info().id;
	string_type type_name = prototype->meta_info().name;
	string_type path = prototype->meta_info().path;

	auto dir = get_root_directory()->get_sub_directory(path);

	if (!dir)
		return path + " not found!";

	auto dir_result = dir->reserve_name(type_name);
	if (!dir_result)
		return dir_result.errors();

	transaction.push([=] () mutable {
		auto cancel_reserve = dir->cancel_reserve(type_name);
		});

	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		type_check_context ctx;
		ctx.get_directories().add_paths({ path });
		auto result = prototype->check_type(ctx);
		if (!result)
		{
			return ctx.get_errors();
		}
	}
	auto ret = cache.register_type(prototype);
	if (!ret)
	{// error, didn't created runtime
		ret.register_error("Unable to register type to repository.");
		return ret.errors();
	}
	transaction.push([&cache, item_id] () mutable {
		auto delete_result = cache.delete_type(item_id);
		});
	ret = dir->add_item(prototype->get_item_ptr());
	if (!ret)
	{
		// error, can't add this name
		ret.register_error("Unable to add "s + type_name + " to directory!");
		return ret.errors();
	}
	else if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		transaction.push([=]() mutable {
			auto remove_result = dir->delete_item(type_name);
			});
		// we have to do save, we are running

		auto save_result = prototype->get_item_ptr()->save();
		if (!save_result)
		{
			rx_result_with<typeType> ret(save_result.errors());
			ret.register_error("Error saving type item "s + prototype->meta_info().get_full_path());
			return ret;
		}
		META_LOG_INFO("types_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::HType::get_type_id()) + " "s + prototype->meta_info().get_full_path());
	}
	else
		META_LOG_TRACE("types_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::HType::get_type_id()) + " "s + prototype->meta_info().get_full_path());

	return prototype;
}
template<class typeCache, class typeT>
rx_result_with<typeT> update_some_type(typeCache& cache, typeT prototype, bool increment_version, rx_transaction_type& transaction)
{
	rx_directory_ptr dir = rx_gate::instance().get_root_directory();

	prototype->meta_info().increment_version(increment_version);
	type_check_context ctx;
	ctx.get_directories().add_paths({ prototype->meta_info().path });
	auto result = prototype->check_type(ctx);
	if (!result)
	{
		return ctx.get_errors();
	}

	auto ret = cache.get_type_definition(prototype->meta_info().id);
	if (!ret)
	{// error, didn't created runtime
		ret.register_error("Unable to get type from repository.");
		return ret.errors();
	}

	typeT old_value = ret.value();

	result = cache.update_type(prototype);
	if (!ret)
	{// error, didn't created runtime
		ret.register_error("Unable to update type in cache.");
		return ret.errors();
	}
	transaction.push([&cache, old_value]() mutable {
		auto delete_result = cache.update_type(old_value);
		});

	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto save_result = prototype->get_item_ptr()->save();
		if (!save_result)
		{
			rx_result_with<typeT> ret(save_result.errors());
			ret.register_error("Error saving type item "s + prototype->meta_info().get_full_path());
			return ret;
		}
		META_LOG_INFO("types_model_algorithm", 100, "Updated "s + rx_item_type_name(typeCache::HType::get_type_id()) + " "s + prototype->meta_info().get_full_path());
	}
	else
		META_LOG_TRACE("types_model_algorithm", 100, "Updated "s + rx_item_type_name(typeCache::HType::get_type_id()) + " "s + prototype->meta_info().get_full_path());
	return prototype;
}
template<class typeT>
rx_result delete_some_runtime(const rx_item_reference& rx_item_reference, rx_thread_handle_t result_target, rx_function_to_go<rx_result&&>&& callback)
{
	rx_node_id id;
	string_type name;
	rx_namespace_item item;
	rx_directory_ptr dir = rx_gate::instance().get_root_directory();
	if (!rx_item_reference.is_node_id())
	{
		string_type path = rx_item_reference.get_path();
		item = dir->get_sub_item(path);
		if (!item)
		{// error, item does not exists
			return path + " does not exists";
		}
		id = item.get_meta().id;
		if (id.is_null())
		{// error, item does not have id
			return path + " does not have id";
		}
		dir = dir->get_sub_directory(item.get_meta().path);
		name = item.get_meta().name;
	}
	else
	{
		if (rx_item_reference.is_null())
		{// error, item does not have id
			return "Invalid " RX_NULL_ITEM_NAME " id.";
		}
		id = rx_item_reference.get_node_id();
		meta_data item_meta;
		auto type = platform_types_manager::instance().get_types_resolver().get_item_data(id, item_meta);
		if (type == rx_item_type::rx_invalid_type)
		{// error, item does not exists
			return id.to_string() + " does not exists";
		}
		name = item_meta.name;
		dir = dir->get_sub_directory(item_meta.path);
	}
	auto obj_ptr = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		return obj_ptr.errors();
	}

	auto callback_ptr = std::make_shared<rx_result_callback>(std::move(callback));

	auto  my_callback = [name, dir, id, item, callback_ptr](rx_result&& deinit_result) mutable
		{
		auto& callback = *callback_ptr;
			rx_transaction_type transaction;
			transaction.push([=] {
				auto unmark = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_running(id);
				});

			if (!deinit_result)
			{
				callback.set_arguments(std::move(deinit_result));
				callback.call();
				return;
			}
			if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			{
				auto work_item = get_platform_item_sync(typeT::RImplType::type_id, id);
				if (!work_item)
				{
					work_item.register_error("Error retrieving type item "s + item.get_meta().get_full_path());
					callback.set_arguments(std::move(rx_result(work_item.errors())));
					callback.call();
					return;
				}
				auto delete_result = work_item.value()->delete_item();
				if (!delete_result)
				{
					delete_result.register_error("Error deleting runtime item "s + item.get_meta().get_full_path());
					callback.set_arguments(std::move(delete_result));
					callback.call();
					return;
				}
			}
			transaction.push([=] {
				if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
				{
					auto work_item = get_platform_item_sync(typeT::RImplType::type_id, id);
					if (work_item)
						auto save_result = work_item.value()->save();
				}
				});
			auto ret = dir->delete_item(name);
			if (!ret)
			{// error, didn't deleted runtime
				ret.register_error("Error deleting runtime from the namespace.");
				callback.set_arguments(std::move(ret));
				callback.call();
				return;
			}
			transaction.push([dir, item]() mutable {
				auto add_result = dir->add_item(item);
				});

			ret = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(id);
			if (!ret)
			{// error, didn't deleted runtime
				ret.register_error("Error deleting runtime from the repository.");
				callback.set_arguments(std::move(ret));
				callback.call();
				return;
			}
			if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
				META_LOG_INFO("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::get_type_id()) + " "s + item.get_meta().get_full_path());
			else
				META_LOG_TRACE("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::get_type_id()) + " "s + item.get_meta().get_full_path());
			transaction.commit();
			callback.set_arguments(std::move(ret));
			callback.call();

		};

	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(), callback.anchor, result_target
		, std::move(my_callback));
	return true;
}
template<class typeCache>
rx_result_with<typename typeCache::RTypePtr> create_some_runtime(typeCache& cache, typename typeCache::HType::instance_data_t instance_data, rx_transaction_type& transaction)
{
	string_type path = instance_data.meta_info.path;
	string_type runtime_name = instance_data.meta_info.name;

	rx_directory_ptr dir = rx_gate::instance().get_root_directory()->get_sub_directory(path);

	if (!dir)
		return path + " not found!";

	auto result = dir->reserve_name(runtime_name);
	if (!result)
		return result.errors();

	transaction.push([=]() mutable {
		auto cancel_reserve = dir->cancel_reserve(runtime_name);
		});

	auto ret_value = cache.create_runtime(std::move(instance_data));
	if (!ret_value)
	{// error, didn't created runtime
		ret_value.register_error("Unable to create runtime in repository.");
		return ret_value.errors();
	}
	rx_node_id item_id = ret_value.value().ptr->meta_info().id;
	sys_runtime::platform_runtime_manager::instance().get_cache().add_functions(item_id, ret_value.value().register_f, ret_value.value().unregister_f);
	transaction.push([&cache, item_id]() mutable {
		sys_runtime::platform_runtime_manager::instance().get_cache().remove_functions(item_id);
		auto delete_result = cache.delete_runtime(item_id);
		});

	result = dir->add_item(ret_value.value().ptr->get_item_ptr());
	if (!result)
	{
		// error, can't add this name
		result.register_error("Unable to add "s + runtime_name + " to directory!");
		return result.errors();
	}
	transaction.push([=]() mutable {
			auto remove_result = dir->delete_item(runtime_name);
		});

	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{

		// we have to do save, we are running
		auto save_result = ret_value.value().ptr->get_item_ptr()->save();
		if (!save_result)
		{
			save_result.register_error("Error saving type item "s + ret_value.value().ptr->meta_info().get_full_path());
			return save_result.errors();
		}
	}
	result = cache.mark_runtime_running(item_id);
	if (!result)
	{
		// error, can't add this name
		result.register_error("Unable to mark "s + runtime_name + " as running!");
		return ret_value.errors();
	}
	transaction.push([&cache, item_id]() mutable {
		auto mark_result = cache.mark_runtime_for_delete(item_id);
		});

	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		result = sys_runtime::platform_runtime_manager::instance().init_runtime<typename typeCache::HType>(ret_value.value().ptr);
		if (!result)
		{
			result.register_error("Unable to initialize "s + runtime_name);
			return result.errors();
		}
		META_LOG_INFO("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::RImplType::type_id) + " "s + ret_value.value().ptr->meta_info().get_full_path());
	}
	else
		META_LOG_TRACE("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::RImplType::type_id) + " "s + ret_value.value().ptr->meta_info().get_full_path());

	return ret_value.value().ptr;
}

}

rx_result_with<rx_node_id> resolve_reference(const rx_item_reference& ref, const ns::rx_directory_resolver& directories)
{
	rx_item_type type;
	meta_data temp;
	return resolve_some_reference(ref, directories, temp, type);
}


template<typename typeT>
rx_result_with<rx_node_id> resolve_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<typeT>)
{
	rx_item_type type;
	meta_data info;
	auto result = resolve_some_reference(ref, directories, info, type);
	if (!result)
		return result;
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		if (type != typeT::type_id)
		{
			return info.get_full_path() + " is " + rx_item_type_name(type) + " and not " + rx_item_type_name(typeT::type_id) + "!";
		}
	}
	auto ret = model::platform_types_manager::instance().get_type_repository<typeT>().type_exists(result.value());
	if (!ret)
	{// type does not exist
		return ret.errors();
	}
	return result;
}

template rx_result_with<rx_node_id> resolve_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<object_type>);
template rx_result_with<rx_node_id> resolve_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<port_type>);
template rx_result_with<rx_node_id> resolve_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<domain_type>);
template rx_result_with<rx_node_id> resolve_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<application_type>);


rx_result_with<rx_node_id> resolve_relation_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories)
{
	rx_item_type type;
	meta_data info;
	auto result = resolve_some_reference(ref, directories, info, type);
	if (!result)
		return result;
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		if (type != relation_type::type_id)
		{
			return info.get_full_path() + " is " + rx_item_type_name(type) + " and not " + rx_item_type_name(relation_type::type_id) + "!";
		}
	}
	auto ret = model::platform_types_manager::instance().get_relations_repository().type_exists(result.value());
	if (!ret)
	{// type does not exist
		return ret.errors();
	}
	return result;
}

template<typename typeT>
rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<typeT>)
{
	rx_item_type type;
	meta_data info;
	auto result = resolve_some_reference(ref, directories, info, type);
	if (!result)
		return result;
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		if (type != typeT::type_id)
		{
			return info.get_full_path() + " is " + rx_item_type_name(type) + " and not " + rx_item_type_name(typeT::type_id) + "!";
		}
	}
	auto ret = model::platform_types_manager::instance().get_simple_type_repository<typeT>().type_exists(result.value());
	if (!ret)
	{// type does not exist
		return ret.errors();
	}
	return result;
}

template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<struct_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<variable_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<source_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<filter_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<event_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<mapper_type>);

template<typename typeT>
rx_result_with<rx_node_id> resolve_runtime_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<typeT>)
{
	rx_item_type type;
	meta_data info;
	auto result = resolve_some_reference(ref, directories, info, type);
	if (!result)
		return result;
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		if (type != typeT::RImplType::type_id)
		{
			return info.get_full_path() + " is " + rx_item_type_name(type) + " and not " + rx_item_type_name(typeT::type_id) + "!";
		}
	}
	//auto ret = model::platform_types_manager::instance().internal_get_type_cache()<typeT>().(result.value());
	//if (!ret)
	//{// type does not exist
	//	return ret.errors();
	//}
	return result;
}

template rx_result_with<rx_node_id> resolve_runtime_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<object_type>);
template rx_result_with<rx_node_id> resolve_runtime_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<domain_type>);
template rx_result_with<rx_node_id> resolve_runtime_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<port_type>);
template rx_result_with<rx_node_id> resolve_runtime_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<application_type>);


rx_result_with<platform_item_ptr> get_working_runtime(const rx_node_id& id)
{
	auto item = sys_runtime::platform_runtime_manager::instance().get_cache().get_item(id);
	if (!item)
		return (id.to_string() + " is not the registered id!");
	else
		return item;
}

std::vector<rx_result_with<platform_item_ptr> > get_working_runtimes(const rx_node_ids& ids)
{
	std::vector<rx_result_with<platform_item_ptr> > ret;
	ret.reserve(ids.size());
	auto items = sys_runtime::platform_runtime_manager::instance().get_cache().get_items(ids);
	for (auto&& item : items)
	{
		if (!item)
			ret.emplace_back("Not registered id!");
		else
			ret.emplace_back(std::move(item));
	}
	return ret;
}



//////////////////////////////////////////////////////////////////////////////////////////

// Parameterized Class rx_internal::model::algorithms::types_model_algorithm


template <class typeT>
void types_model_algorithm<typeT>::get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &get_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void types_model_algorithm<typeT>::create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, types_model_algorithm<typeT>::create_type_sync, std::move(callback), prototype);
}

template <class typeT>
void types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, bool increment_version, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &update_type_sync, std::move(callback), prototype, increment_version);
}

template <class typeT>
void types_model_algorithm<typeT>::delete_type (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, types_model_algorithm<typeT>::delete_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void types_model_algorithm<typeT>::check_type (const string_type& name, rx_directory_ptr dir, rx_result_with_callback<check_type_result>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &types_model_algorithm<typeT>::check_type_sync, std::move(callback), name, dir);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::get_type_sync (const rx_item_reference& item_reference)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto item = get_root_directory()->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().id;
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_type_repository<typeT>().get_type_definition(id);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::create_type_sync (typename typeT::smart_ptr prototype)
{
	rx_transaction_type transaction;
	auto result = create_some_type(platform_types_manager::instance().get_type_repository<typeT>(), prototype, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, bool increment_version)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_type_repository<typeT>(), prototype, increment_version, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result types_model_algorithm<typeT>::delete_type_sync (const rx_item_reference& item_reference)
{
	rx_transaction_type transaction;
	auto result = delete_some_type(platform_types_manager::instance().get_type_repository<typeT>(), item_reference, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result_with<check_type_result> types_model_algorithm<typeT>::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	rx_result_with<check_type_result> ret;
	rx_namespace_item item = dir->get_sub_item(name);
	if (!item)
	{
		ret.register_error(name + " does not exists!");
		return ret;
	}
	auto id = item.get_meta().id;
	if (id.is_null())
	{// error, item does not have id
		ret.register_error(name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!");
		return ret;
	}
	type_check_context ctx;
	auto result = platform_types_manager::instance().get_type_repository<typeT>().check_type(id, ctx);
	if (!result)
		return result.errors();
	check_type_result check_result;
	check_result.records = ctx.move_records();
	return ret;
}


// Parameterized Class rx_internal::model::algorithms::simple_types_model_algorithm


template <class typeT>
void simple_types_model_algorithm<typeT>::get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &simple_types_model_algorithm<typeT>::get_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void simple_types_model_algorithm<typeT>::create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &create_type_sync, std::move(callback), prototype);
}

template <class typeT>
void simple_types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, bool increment_version, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &update_type_sync, std::move(callback), prototype, increment_version);
}

template <class typeT>
void simple_types_model_algorithm<typeT>::delete_type (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &delete_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void simple_types_model_algorithm<typeT>::check_type (const string_type& name, rx_directory_ptr dir, rx_result_with_callback<check_type_result>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &simple_types_model_algorithm<typeT>::check_type_sync, std::move(callback), name, dir);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::get_type_sync (const rx_item_reference& item_reference)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto dir = get_root_directory();
		auto item = dir->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().id;
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_simple_type_repository<typeT>().get_type_definition(id);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::create_type_sync (typename typeT::smart_ptr prototype)
{
	rx_transaction_type transaction;
	auto result = create_some_type(platform_types_manager::instance().get_simple_type_repository<typeT>(), prototype, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, bool increment_version)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_simple_type_repository<typeT>(), prototype, increment_version, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result simple_types_model_algorithm<typeT>::delete_type_sync (const rx_item_reference& item_reference)
{
	rx_transaction_type transaction;
	auto result = delete_some_type(platform_types_manager::instance().get_simple_type_repository<typeT>(), item_reference, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result_with<check_type_result> simple_types_model_algorithm<typeT>::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	rx_result_with<check_type_result> ret;
	rx_namespace_item item = dir->get_sub_item(name);
	if (!item)
	{
		ret.register_error(name + " does not exists!");
		return ret;
	}
	auto id = item.get_meta().id;
	if (id.is_null())
	{// error, item does not have id
		ret.register_error(name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!");
		return ret;
	}
	type_check_context ctx;
	auto result = platform_types_manager::instance().get_simple_type_repository<typeT>().check_type(id, ctx);
	if (!result)
		return result.errors();
	check_type_result check_result;
	check_result.records = ctx.move_records();
	return ret;
}


// Parameterized Class rx_internal::model::algorithms::runtime_model_algorithm


template <class typeT>
void runtime_model_algorithm<typeT>::get_runtime (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &get_runtime_sync, std::move(callback), item_reference);
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_runtime (instanceT&& instance_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &runtime_model_algorithm<typeT>::create_runtime_sync, std::move(callback), std::move(instance_data));
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_prototype (instanceT&& instance_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &runtime_model_algorithm<typeT>::create_prototype_sync, std::move(callback)
        , std::move(instance_data));
}

template <class typeT>
void runtime_model_algorithm<typeT>::update_runtime (instanceT&& instance_data, bool increment_version, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_post_function_to(RX_DOMAIN_META, callback.anchor, &runtime_model_algorithm<typeT>::update_runtime_sync
		, std::move(instance_data), increment_version, std::move(callback), rx_thread_context());

}

template <class typeT>
void runtime_model_algorithm<typeT>::delete_runtime (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback)
{
	auto result_target = rx_thread_context();
	rx_post_function_to(RX_DOMAIN_META, callback.anchor, &delete_runtime_sync
		, item_reference, result_target, std::move(callback));
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::init_runtime (typename typeT::RTypePtr what)
{
	auto init_result = sys_runtime::platform_runtime_manager::instance().init_runtime<typeT>(what);
	if (init_result)
	{// make object running in state

		auto init_result = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_running(what->meta_info().id);
	}
	return init_result;
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::get_runtime_sync (const rx_item_reference& item_reference)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
		if (!id)
		{
			return RX_NULL_ITEM_NAME " is invalid argument";
		}
	}
	else
	{
		auto item = get_root_directory()->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().id;
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_type_repository<typeT>().get_runtime(id);
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_runtime_sync (instanceT&& instance_data)
{
	rx_transaction_type transaction;
	auto result = create_some_runtime<types_repository<typeT> >(platform_types_manager::instance().get_type_repository<typeT>()
		, std::move(instance_data), transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_prototype_sync (instanceT&& instance_data)
{
	auto ret = platform_types_manager::instance().get_type_repository<typeT>().create_runtime(std::move(instance_data), true);
	if (ret)
		return ret.value().ptr;
	else
		return ret.errors();
}

template <class typeT>
void runtime_model_algorithm<typeT>::update_runtime_sync (instanceT&& instance_data, bool increment_version, rx_result_with_callback<typename typeT::RTypePtr>&& callback, rx_thread_handle_t result_target)
{
	using ret_type = rx_result_with<typename typeT::RTypePtr>;
	auto id = instance_data.meta_info.id;
	if (id.is_null())
	{// error, item does not have id
		callback.set_arguments(ret_type(instance_data.meta_info.name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!"));
		rx_post_packed_to(result_target, std::move(callback));
	}
	auto obj_ptr = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		callback.set_arguments(ret_type(obj_ptr.errors()));
		rx_post_packed_to(result_target, std::move(callback));
	}

	auto anchor = callback.anchor;

	auto callback_ptr = std::make_shared<rx_result_with_callback<typename typeT::RTypePtr> >(std::move(callback));

	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value()
			, anchor, rx_thread_context(), [
					result_target
					, increment_version
					, instance_data = std::move(instance_data)
					, callback_ptr
				]
				(rx_result&& deinit_result) mutable
				{
					auto ret = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(instance_data.meta_info.id);
					if (ret)
					{
						auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(instance_data.meta_info.path);
						if (dir)
							dir->delete_item(instance_data.meta_info.name);
						if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
						{
							auto storage_result = instance_data.meta_info.resolve_storage();
							if (storage_result)
							{
								auto item_result = storage_result.value()->get_item_storage(instance_data.meta_info);
								if (item_result)
								{
									item_result.value()->delete_item();
									META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + instance_data.meta_info.get_full_path());
								}
							}
						}
						instance_data.meta_info.increment_version(increment_version);
						auto create_result = create_runtime_sync(std::move(instance_data));
						callback_ptr->set_arguments(std::move(create_result));
					}
					if (!ret)
						callback_ptr->set_arguments(ret.errors());

					rx_post_packed_to(result_target, std::move(*callback_ptr));
				});
	if (!result)
	{
		callback.set_arguments(ret_type(result.errors()));
		rx_post_packed_to(result_target, std::move(callback));
	}
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::delete_runtime_sync (const rx_item_reference& item_reference, rx_thread_handle_t result_target, rx_function_to_go<rx_result&&>&& callback)
{
	auto result = delete_some_runtime<typeT>(item_reference, result_target, std::move(callback));
	return result;
}


// Class rx_internal::model::algorithms::relation_types_algorithm


void relation_types_algorithm::get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, get_type_sync, std::move(callback), item_reference);
}

void relation_types_algorithm::create_type (relation_type::smart_ptr prototype, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, relation_types_algorithm::create_type_sync, std::move(callback)
        , prototype);
}

void relation_types_algorithm::update_type (relation_type::smart_ptr prototype, bool increment_version, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	using result_t = rx_result_with<typename relation_type::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return update_type_sync(prototype, increment_version);
	};
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, std::move(func), std::move(callback));
}

void relation_types_algorithm::delete_type (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor, &delete_type_sync, std::move(callback), item_reference);
}

void relation_types_algorithm::check_type (const string_type& name, rx_directory_ptr dir, rx_result_with_callback<check_type_result>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, callback.anchor,  &relation_types_algorithm::check_type_sync, std::move(callback), name, dir);
}

rx_result_with<relation_type::smart_ptr> relation_types_algorithm::get_type_sync (const rx_item_reference& item_reference)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto item = rx_gate::instance().get_root_directory()->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().id;
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_relations_repository().get_type_definition(id);
}

rx_result_with<relation_type::smart_ptr> relation_types_algorithm::create_type_sync (relation_type::smart_ptr prototype)
{
	rx_transaction_type transaction;
	auto result = create_some_type(platform_types_manager::instance().get_relations_repository(), prototype, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

rx_result_with<relation_type::smart_ptr> relation_types_algorithm::update_type_sync (relation_type::smart_ptr prototype, bool increment_version)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_relations_repository(), prototype, increment_version, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

rx_result relation_types_algorithm::delete_type_sync (const rx_item_reference& item_reference)
{
	rx_transaction_type transaction;
	auto result = delete_some_type(platform_types_manager::instance().get_relations_repository(), item_reference, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

rx_result_with<check_type_result> relation_types_algorithm::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	rx_result_with<check_type_result> ret;
	rx_namespace_item item = dir->get_sub_item(name);
	if (!item)
	{
		ret.register_error(name + " does not exists!");
		return ret;
	}
	auto id = item.get_meta().id;
	if (id.is_null())
	{// error, item does not have id
		ret.register_error(name + " does not have valid " + rx_item_type_name(relation_type::type_id) + " id!");
		return ret;
	}
	type_check_context ctx;
	auto result = platform_types_manager::instance().get_relations_repository().check_type(id, ctx);
	if (!result)
		return result.errors();
	check_type_result check_result;
	check_result.records = ctx.move_records();
	return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////
//explicit instantiation
template class types_model_algorithm<object_type>;
template class types_model_algorithm<port_type>;
template class types_model_algorithm<domain_type>;
template class types_model_algorithm<application_type>;

template class simple_types_model_algorithm<struct_type>;
template class simple_types_model_algorithm<variable_type>;
template class simple_types_model_algorithm<source_type>;
template class simple_types_model_algorithm<filter_type>;
template class simple_types_model_algorithm<event_type>;
template class simple_types_model_algorithm<mapper_type>;

template class runtime_model_algorithm<object_type>;
template class runtime_model_algorithm<port_type>;
template class runtime_model_algorithm<domain_type>;
template class runtime_model_algorithm<application_type>;
} // namespace algorithms
} // namespace model
} // namespace rx_internal

