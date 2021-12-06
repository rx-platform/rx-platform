

/****************************************************************************
*
*  model\rx_model_algorithms.cpp
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


// rx_model_algorithms
#include "model/rx_model_algorithms.h"

#include "runtime_internal/rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"
#include "lib/rx_lib.h"
#include "sys_internal/rx_internal_ns.h"
#include "api/rx_platform_api.h"
#include "system/meta/rx_meta_algorithm.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_dependencies.h"
#include "model/rx_model_transactions.h"
#include "system/server/rx_directory_cache.h"

#define RX_NEW_SYMBOL_LOG_PREFIX_STR "#"s


namespace rx_internal {

namespace model {

namespace algorithms {

using namespace rx;

namespace
{
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

	case rx_item_type::rx_method_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<method_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_program_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<program_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;
	case rx_item_type::rx_display_type:
		{
			auto result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().get_type_definition(id);
			if (result)
				return result.value()->get_item_ptr();
			else
				return result.errors();
		}
		break;

	case rx_item_type::rx_data_type:
		{
			auto result = model::platform_types_manager::instance().get_data_types_repository().get_type_definition(id);
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
		auto item = ns::rx_directory_cache::instance().get_cached_item(ref.get_path());
		if (!item)
		{// found it in cache, return!
			item = directories.resolve_item(ref.get_path());
			if (!item)
			{
				return ref.get_path() + " does not exists!";
			}
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
	rx_directory_ptr dir;
	if (!rx_item_reference.is_node_id())
	{
		item = rx_gate::instance().get_namespace_item(rx_item_reference.get_path());
		if (!item)
		{// error, item does not exists
			return rx_item_reference.get_path() + " does not exists";
		}
		id = item.get_meta().id;
		if (id.is_null())
		{// error, item does not have id
			return rx_item_reference.get_path() + " does not have id";
		}
		name = item.get_meta().name;
		dir = rx_gate::instance().get_directory(item.get_meta().path);
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
		dir = rx_gate::instance().get_directory(item_meta.path);
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
		META_LOG_INFO("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeCache::HType::type_id) + " "s + item.get_meta().get_full_path());
	else
		META_LOG_TRACE("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeCache::HType::type_id) + " "s + item.get_meta().get_full_path());
	return true;
}
template<class typeCache, class typeType>
rx_result_with<typeType> create_some_type(typeCache& cache, typeType prototype, rx_transaction_type& transaction)
{
	using algorithm_type = typename typeType::pointee_type::algorithm_type;

	rx_node_id item_id = prototype->meta_info.id;
	string_type type_name = prototype->meta_info.name;
	string_type path = prototype->meta_info.path;

	if (item_id.is_null())
	{
		item_id = rx_node_id::generate_new();
		prototype->meta_info.id = item_id;
	}

	auto dir = rx_gate::instance().get_directory(path);

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
		auto result = algorithm_type::check_type(*prototype, ctx);
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
			ret.register_error("Error saving type item "s + prototype->meta_info.get_full_path());
			return ret;
		}
		META_LOG_INFO("types_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::HType::type_id) + " "s + prototype->meta_info.get_full_path());
	}
	else
		META_LOG_TRACE("types_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::HType::type_id) + " "s + prototype->meta_info.get_full_path());

	return prototype;
}
template<class typeCache, class typeT>
rx_result_with<typeT> update_some_type(typeCache& cache, typeT prototype, rx_update_type_data update_data, rx_transaction_type& transaction)
{
	using algorithm_type = typename typeT::pointee_type::algorithm_type;


	prototype->meta_info.increment_version(update_data.increment_version);
	type_check_context ctx;
	ctx.get_directories().add_paths({ prototype->meta_info.path });
	auto result = algorithm_type::check_type(*prototype, ctx);
	if (!result)
	{
		return ctx.get_errors();
	}

	auto ret = cache.get_type_definition(prototype->meta_info.id);
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
			ret.register_error("Error saving type item "s + prototype->meta_info.get_full_path());
			return ret;
		}
		META_LOG_INFO("types_model_algorithm", 100, "Updated "s + rx_item_type_name(typeCache::HType::type_id) + " "s + prototype->meta_info.get_full_path());
	}
	else
		META_LOG_TRACE("types_model_algorithm", 100, "Updated "s + rx_item_type_name(typeCache::HType::type_id) + " "s + prototype->meta_info.get_full_path());
	return prototype;
}
template<class typeT>
rx_result delete_some_runtime(const rx_item_reference& rx_item_reference, rx_result_callback&& callback)
{
	rx_node_id id;
	string_type name;
	rx_namespace_item item;
	rx_directory_ptr dir; if (!rx_item_reference.is_node_id())
	{
		item = rx_gate::instance().get_namespace_item(rx_item_reference.get_path());
		if (!item)
		{// error, item does not exists
			return rx_item_reference.get_path() + " does not exists";
		}
		id = item.get_meta().id;
		if (id.is_null())
		{// error, item does not have id
			return rx_item_reference.get_path() + " does not have id";
		}
		name = item.get_meta().name;
		dir = rx_gate::instance().get_directory(item.get_meta().path);
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
		dir = rx_gate::instance().get_directory(item_meta.path);
	}
	auto obj_ptr = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		return obj_ptr.errors();
	}

	auto anchor = callback.get_anchor();
	rx_result_callback  my_callback(anchor, [callback = std::move(callback), name, dir, id, item](rx_result&& deinit_result) mutable
		{
			rx_transaction_type transaction;
			transaction.push([=] {
				auto unmark = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_running(id);
				});

			if (!deinit_result)
			{
				callback(std::move(deinit_result));
				return;
			}
			if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			{
				auto work_item = get_platform_item_sync(typeT::RImplType::type_id, id);
				if (!work_item)
				{
					work_item.register_error("Error retrieving type item "s + item.get_meta().get_full_path());
					callback(std::move(rx_result(work_item.errors())));
					return;
				}
				auto delete_result = work_item.value()->delete_item();
				if (!delete_result)
				{
					delete_result.register_error("Error deleting runtime item "s + item.get_meta().get_full_path());
					callback(std::move(delete_result));
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
				callback(std::move(ret));
				return;
			}
			transaction.push([dir, item]() mutable {
				auto add_result = dir->add_item(item);
				});

			ret = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(id);
			if (!ret)
			{// error, didn't deleted runtime
				ret.register_error("Error deleting runtime from the repository.");
				callback(std::move(ret));
				return;
			}
			if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
				META_LOG_INFO("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::get_type_id()) + " "s + item.get_meta().get_full_path());
			else
				META_LOG_TRACE("types_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::get_type_id()) + " "s + item.get_meta().get_full_path());
			transaction.commit();
			callback(std::move(ret));

		});

	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(), std::move(my_callback));
	return true;
}
template<class typeCache>
rx_result_with<create_runtime_result<typename typeCache::HType> > create_some_runtime(typeCache& cache, typename typeCache::HType::instance_data_t instance_data, data::runtime_values_data runtime_data, rx_transaction_type& transaction)
{
	string_type path = instance_data.meta_info.path;
	string_type runtime_name = instance_data.meta_info.name;

	rx_directory_ptr dir = rx_gate::instance().get_directory(path);

	if (!dir)
		return path + " not found!";

	auto result = dir->reserve_name(runtime_name);
	if (!result)
		return result.errors();

	transaction.push([=]() mutable {
		auto cancel_reserve = dir->cancel_reserve(runtime_name);
		});

	auto ret_value = cache.create_runtime(std::move(instance_data), std::move(runtime_data));
	if (!ret_value)
	{// error, didn't created runtime
		ret_value.register_error("Unable to create runtime in repository.");
		return ret_value.errors();
	}

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
	auto item_id = ret_value.value().ptr->meta_info().id;
	result = cache.mark_runtime_running(item_id);
	if (!result)
	{
		// error, can't add this name
		result.register_error("Unable to mark "s + runtime_name + " as running!");
		return result.errors();
	}
	transaction.push([&cache, item_id]() mutable {
		auto mark_result = cache.mark_runtime_for_delete(item_id);
		});

	return ret_value.value();
}

}

rx_result_with<rx_node_id> resolve_reference(const rx_item_reference& ref, const ns::rx_directory_resolver& directories)
{
	rx_item_type type;
	meta_data temp;
	return resolve_some_reference(ref, directories, temp, type);
}
meta_data resolve_reference(const rx_item_reference& ref, rx_item_type& type, const ns::rx_directory_resolver& directories)
{
	meta_data temp;
	auto result = resolve_some_reference(ref, directories, temp, type);
	if (result)
	{
		return temp;
	}
	else
	{
		type = rx_item_type::rx_invalid_type;
		return meta_data();
	}
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


rx_result_with<rx_node_id> resolve_data_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories)
{
	rx_item_type type;
	meta_data info;
	auto result = resolve_some_reference(ref, directories, info, type);
	if (!result)
		return result;
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		if (type != data_type::type_id)
		{
			return info.get_full_path() + " is " + rx_item_type_name(type) + " and not " + rx_item_type_name(data_type::type_id) + "!";
		}
	}
	auto ret = model::platform_types_manager::instance().get_data_types_repository().type_exists(result.value());
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
	string_type ref_path;
	if (!ref.is_node_id())
		ref_path = ref.get_path();
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
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<method_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<program_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<display_type>);

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
	rx_do_with_callback(RX_DOMAIN_META, get_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void types_model_algorithm<typeT>::create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, types_model_algorithm<typeT>::create_type_sync, std::move(callback), prototype);
}

template <class typeT>
void types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, update_type_sync, std::move(callback), prototype, std::move(update_data));
}

template <class typeT>
void types_model_algorithm<typeT>::delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback)
{
	rx_reference_ptr anchor = callback.get_anchor();
	rx_post_function_to(RX_DOMAIN_META, anchor, [](const rx_item_reference& item_reference, rx_result_callback&& callback)
		{
			ns::rx_directory_resolver dirs;
			auto temp = resolve_type_reference<typeT>(item_reference, dirs, tl::type2type<typeT>());
			if (!temp)
				callback(temp.errors());
			auto executer = rx_create_reference<transactions::model_transactions_executer>();
			executer->add_transaction(std::make_unique<transactions::delete_type_transaction<typeT> >(temp.move_value(), callback.get_anchor()));
			executer->execute(std::move(callback));
		}, item_reference, std::move(callback));
	//rx_do_with_callback(RX_DOMAIN_META, types_model_algorithm<typeT>::delete_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void types_model_algorithm<typeT>::check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, types_model_algorithm<typeT>::check_type_sync, std::move(callback), name, dir);
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
		auto item = rx_gate::instance().get_namespace_item(item_reference.get_path());
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
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, rx_update_type_data update_data)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_type_repository<typeT>(), prototype, std::move(update_data), transaction);
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
rx_result_with<check_type_result> types_model_algorithm<typeT>::check_type_sync (const string_type& name, const string_type& dir)
{
	rx_result_with<check_type_result> ret;
	ns::rx_directory_resolver dirs;
	dirs.add_paths({ dir });
	rx_namespace_item item = rx_gate::instance().get_namespace_item(name, &dirs);
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
	rx_do_with_callback(RX_DOMAIN_META, simple_types_model_algorithm<typeT>::get_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void simple_types_model_algorithm<typeT>::create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, create_type_sync, std::move(callback), prototype);
}

template <class typeT>
void simple_types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, update_type_sync, std::move(callback), prototype, std::move(update_data));
}

template <class typeT>
void simple_types_model_algorithm<typeT>::delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, delete_type_sync, std::move(callback), item_reference);
}

template <class typeT>
void simple_types_model_algorithm<typeT>::check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, simple_types_model_algorithm<typeT>::check_type_sync, std::move(callback), name, dir);
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
		auto item = rx_gate::instance().get_namespace_item(item_reference.get_path());
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
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, rx_update_type_data update_data)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_simple_type_repository<typeT>(), prototype, std::move(update_data), transaction);
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
rx_result_with<check_type_result> simple_types_model_algorithm<typeT>::check_type_sync (const string_type& name, const string_type& dir)
{
	rx_result_with<check_type_result> ret;
	ns::rx_directory_resolver dirs;
	dirs.add_paths({ dir });
	rx_namespace_item item = rx_gate::instance().get_namespace_item(name, &dirs);
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
	rx_do_with_callback(RX_DOMAIN_META, get_runtime_sync, std::move(callback), item_reference);
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_runtime (instanceT&& instance_data, data::runtime_values_data&& runtime_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, runtime_model_algorithm<typeT>::create_runtime_sync, std::move(callback), std::move(instance_data), std::move(runtime_data));
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_prototype (instanceT&& instance_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, runtime_model_algorithm<typeT>::create_prototype_sync, std::move(callback)
        , std::move(instance_data));
}

template <class typeT>
void runtime_model_algorithm<typeT>::update_runtime (instanceT&& instance_data, rx_update_runtime_data update_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_post_function_to(RX_DOMAIN_META, callback.get_anchor(), &runtime_model_algorithm<typeT>::update_runtime_with_depends_sync
		, std::move(instance_data), std::move(update_data), std::move(callback), rx_thread_context());

}

template <class typeT>
void runtime_model_algorithm<typeT>::delete_runtime (const rx_item_reference& item_reference, rx_result_callback&& callback)
{
	auto result_target = rx_thread_context();
	rx_post_function_to(RX_DOMAIN_META, callback.get_anchor(), &delete_runtime_sync
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
		auto item = rx_gate::instance().get_namespace_item(item_reference.get_path());
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
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_runtime_sync (instanceT&& instance_data, data::runtime_values_data&& runtime_data)
{
	rx_transaction_type transaction;
	auto result = create_some_runtime<types_repository<typeT> >(platform_types_manager::instance().get_type_repository<typeT>()
		, std::move(instance_data), std::move(runtime_data), transaction);
	if (result)
	{
		rx_node_id item_id = result.value().ptr->meta_info().id;
		sys_runtime::platform_runtime_manager::instance().get_cache().add_functions(item_id, result.value().register_f, result.value().unregister_f);
		transaction.push([item_id]() mutable {
			sys_runtime::platform_runtime_manager::instance().get_cache().remove_functions(item_id);
			auto delete_result = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(item_id);
			});

		if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
		{
			auto init_result = sys_runtime::platform_runtime_manager::instance().init_runtime<typeT>(result.value().ptr);
			if (!init_result)
			{
				init_result.register_error("Unable to initialize "s + result.value().ptr->meta_info().get_full_path());
				return result.errors();
			}
			META_LOG_INFO("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + result.value().ptr->meta_info().get_full_path());
		}
		else
			META_LOG_TRACE("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + result.value().ptr->meta_info().get_full_path());

		transaction.commit();
		return result.value().ptr;
	}
	return result.errors();
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_prototype_sync (instanceT&& instance_data)
{
	auto ret = platform_types_manager::instance().get_type_repository<typeT>().create_runtime(std::move(instance_data), data::runtime_values_data(), true);
	if (ret)
		return ret.value().ptr;
	else
		return ret.errors();
}

template <class typeT>
void runtime_model_algorithm<typeT>::update_runtime_sync (instanceT&& instance_data, rx_update_runtime_data update_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback, rx_thread_handle_t result_target)
{
	using ret_type = rx_result_with<typename typeT::RTypePtr>;
	auto id = instance_data.meta_info.id;
	bool is_empty = false;
	if (id.is_null())
	{// error, item does not have id
		string_type path = instance_data.meta_info.get_full_path();
		if (path.empty())
		{
			callback(ret_type(RX_NEW_SYMBOL_LOG_PREFIX_STR + instance_data.meta_info.name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!"));
			return;
		}
		else
		{
			ns::rx_directory_resolver dirs;
			auto resolved = resolve_reference(path, dirs);
			if (!resolved)
			{
				callback(ret_type(RX_NEW_SYMBOL_LOG_PREFIX_STR + path + " not existing " + rx_item_type_name(typeT::type_id)));
				return;
			}
			is_empty = true;
			id = resolved.value();
		}
	}
	auto transaction_ptr = std::make_shared<rx_transaction_type>();
	auto& transaction = *transaction_ptr;
	auto obj_ptr = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		callback(ret_type(obj_ptr.errors()));
		return;
	}
	meta_data old_meta = obj_ptr.value()->meta_info();
	if (is_empty)
	{
		instance_data.meta_info = old_meta;
		instance_data.overrides = obj_ptr.value()->get_overrides();
		instance_data.instance_data = obj_ptr.value()->get_instance_data().get_data();
	}
	// now remove old runtime from platform if needed
	///////////////////////////////////////////////////////////////////////////
	auto ret = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(old_meta.id);
	if (!ret)
	{
		callback(ret_type(ret.errors()));
		return;
	}
	data::runtime_values_data runtime_data;
	rx_storage_item_ptr runtime_storage;
	auto dir = rx_gate::instance().get_directory(old_meta.path);
	if (dir)
		dir->delete_item(old_meta.name);
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto storage_result = instance_data.meta_info.resolve_storage();
		if (storage_result)
		{
			auto temp_result = storage_result.value()->get_runtime_storage(instance_data.meta_info, typeT::type_id);
			if (temp_result)
			{
				if (update_data.initialize_data)
					temp_result.value()->delete_item();
				else
					runtime_storage = temp_result.move_value();

			}

			auto item_result = storage_result.value()->get_item_storage(old_meta, obj_ptr.value()->get_type_id());
			if (item_result)
			{

				item_result.value()->delete_item();
				META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RImplType::type_id) + " storage for "s + old_meta.get_full_path());
			}
		}
	}
	if (runtime_storage)
	{
		auto rt_result = runtime_storage->open_for_read();
		if (rt_result)
		{
			runtime_storage->read_stream().read_init_values(nullptr, runtime_data);
			runtime_storage->close_read();
		}
	}
	if (instance_data.meta_info.version <= old_meta.version)
	{
		instance_data.meta_info.version = old_meta.version;
		instance_data.meta_info.increment_version(update_data.increment_version);
	}

	///////////////////////////////////////////////////////////////////////////
	// now prepare runtime for creations
	auto create_result = create_some_runtime(platform_types_manager::instance().get_type_repository<typeT>()
		, std::move(instance_data), std::move(runtime_data), transaction);

	if (!create_result)
	{
		callback(ret_type(create_result.errors()));
		return;
	}

	auto anchor = callback.get_anchor();

	transaction_ptr->commit();
	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(),
		rx_result_callback(anchor, [
					result_target
					, update_data = std::move(update_data)
					, instance_data = std::move(instance_data)
					, transaction_ptr
					, create_data = create_result.value()
					, callback=std::move(callback)
				]
				(rx_result&& deinit_result) mutable
				{
					auto& transaction = *transaction_ptr;
					if (deinit_result)
					{
						rx_node_id item_id = create_data.ptr->meta_info().id;
						sys_runtime::platform_runtime_manager::instance().get_cache().add_functions(item_id, create_data.register_f, create_data.unregister_f);
						transaction.push([item_id]() mutable {
							sys_runtime::platform_runtime_manager::instance().get_cache().remove_functions(item_id);
							auto delete_result = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(item_id);
							});

						if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
						{
							auto result = sys_runtime::platform_runtime_manager::instance().init_runtime<typeT>(create_data.ptr);
							if (!result)
							{
								result.register_error("Unable to initialize "s + RX_NEW_SYMBOL_LOG_PREFIX_STR + create_data.ptr->meta_info().name);
								callback(result.errors());
							}
							else
							{
								transaction.commit();
								callback(create_data.ptr);
								META_LOG_INFO("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + RX_NEW_SYMBOL_LOG_PREFIX_STR + create_data.ptr->meta_info().get_full_path());
							}
						}
						else
						{
							META_LOG_WARNING("runtime_model_algorithm", 100, "Platform stopping no created "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + RX_NEW_SYMBOL_LOG_PREFIX_STR + create_data.ptr->meta_info().get_full_path());
						}
					}
					else
					{
						callback(deinit_result.errors());
					}
				}));
	if (!result)
	{
		transaction_ptr->uncommit();
		callback(ret_type(result.errors()));
	}
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::delete_runtime_sync (const rx_item_reference& item_reference, rx_thread_handle_t result_target, rx_result_callback&& callback)
{
	auto result = delete_some_runtime<typeT>(item_reference, std::move(callback));
	return result;
}

template <class typeT>
void runtime_model_algorithm<typeT>::update_runtime_with_depends_sync (instanceT&& instance_data, rx_update_runtime_data update_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback, rx_thread_handle_t result_target)
{
	auto result = transaction_algorithm::get_dependents(instance_data.meta_info.id, instance_data.meta_info.path);
	if (result)
	{
		if (result.value().items.empty())
		{
			update_runtime_sync(std::move(instance_data), std::move(update_data), std::move(callback), result_target);
		}
		else
		{
			transactions::local_dependecy_builder::smart_ptr builder_ptr = rx_create_reference<transactions::local_dependecy_builder>();
			instance_data.meta_info.increment_version(update_data.increment_version);
			api::query_result_detail temp(typeT::runtime_type_id, instance_data.meta_info);
			builder_ptr->add(temp, true, true, true);
			for (auto& one : result.value().items)
			{
				builder_ptr->add(one, true, true, false);
			}
			rx_reference_ptr anchor = callback.get_anchor();
			rx_result_callback my_callback(anchor, [builder_ptr, callback = std::move(callback)](rx_result&& res) mutable
				{
				if (!res)
					callback(res.errors());
				else
					callback(builder_ptr->extract_single_result<typeT>());
				});
			auto ret = builder_ptr->apply_items(std::move(my_callback));
				
		}
	}
	else
	{
		callback(result.errors());
	}
}


// Class rx_internal::model::algorithms::relation_types_algorithm 


void relation_types_algorithm::get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, get_type_sync, std::move(callback), item_reference);
}

void relation_types_algorithm::create_type (relation_type::smart_ptr prototype, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, relation_types_algorithm::create_type_sync, std::move(callback)
        , prototype);
}

void relation_types_algorithm::update_type (relation_type::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, relation_types_algorithm::update_type_sync, std::move(callback)
		, prototype, update_data);
}

void relation_types_algorithm::delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, delete_type_sync, std::move(callback), item_reference);
}

void relation_types_algorithm::check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, relation_types_algorithm::check_type_sync, std::move(callback), name, dir);
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
		auto item = rx_gate::instance().get_namespace_item(item_reference.get_path());
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

rx_result_with<relation_type::smart_ptr> relation_types_algorithm::update_type_sync (relation_type::smart_ptr prototype, rx_update_type_data update_data)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_relations_repository(), prototype, std::move(update_data), transaction);
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

rx_result_with<check_type_result> relation_types_algorithm::check_type_sync (const string_type& name, const string_type& dir)
{
	rx_result_with<check_type_result> ret;
	ns::rx_directory_resolver dirs;
	dirs.add_paths({ dir });
	rx_namespace_item item = rx_gate::instance().get_namespace_item(name, &dirs);
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
template class simple_types_model_algorithm<method_type>;
template class simple_types_model_algorithm<program_type>;
template class simple_types_model_algorithm<display_type>;

template class runtime_model_algorithm<object_type>;
template class runtime_model_algorithm<port_type>;
template class runtime_model_algorithm<domain_type>;
template class runtime_model_algorithm<application_type>;
// Class rx_internal::model::algorithms::data_types_model_algorithm 


void data_types_model_algorithm::get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename data_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, data_types_model_algorithm::get_type_sync, std::move(callback), item_reference);
}

void data_types_model_algorithm::create_type (data_type::smart_ptr prototype, rx_result_with_callback<typename data_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, create_type_sync, std::move(callback), prototype);
}

void data_types_model_algorithm::update_type (data_type::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename data_type::smart_ptr>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, update_type_sync, std::move(callback), prototype, std::move(update_data));
}

void data_types_model_algorithm::delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, delete_type_sync, std::move(callback), item_reference);
}

void data_types_model_algorithm::check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback)
{
	rx_do_with_callback(RX_DOMAIN_META, data_types_model_algorithm::check_type_sync, std::move(callback), name, dir);
}

rx_result_with<data_type::smart_ptr> data_types_model_algorithm::get_type_sync (const rx_item_reference& item_reference)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto item = rx_gate::instance().get_namespace_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().id;
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_data_types_repository().get_type_definition(id);
}

rx_result_with<data_type::smart_ptr> data_types_model_algorithm::create_type_sync (data_type::smart_ptr prototype)
{
	rx_transaction_type transaction;
	auto result = create_some_type(platform_types_manager::instance().get_data_types_repository(), prototype, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

rx_result_with<data_type::smart_ptr> data_types_model_algorithm::update_type_sync (data_type::smart_ptr prototype, rx_update_type_data update_data)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_data_types_repository(), prototype, std::move(update_data), transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

rx_result data_types_model_algorithm::delete_type_sync (const rx_item_reference& item_reference)
{
	rx_transaction_type transaction;
	auto result = delete_some_type(platform_types_manager::instance().get_data_types_repository(), item_reference, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

rx_result_with<check_type_result> data_types_model_algorithm::check_type_sync (const string_type& name, const string_type& dir)
{
	return RX_NOT_IMPLEMENTED;
}


// Class rx_internal::model::algorithms::transaction_algorithm 


rx_result_with<api::query_result> transaction_algorithm::get_dependents (rx_item_reference item, string_view_type dir)
{
	std::vector<api::query_result_detail> ret;
	ns::rx_directory_resolver dirs;
	dirs.add_paths({ string_type(dir) });
	rx_item_type type;
	meta_data meta = api::ns::rx_resolve_reference(item, type, dirs);
	if (type == rx_item_type::rx_invalid_type)
		return RX_INVALID_PATH;

	std::set<rx_node_id> ids;
	rx_result repository_result = platform_types_manager::instance().get_dependecies_cache().fill_dependents(meta.id, ids);

	if (!repository_result)
		return repository_result.errors();


	api::query_result result;
	result.success = true;

	for (auto& one : ids)
	{
		meta_data temp;
		auto type = rx_internal::model::platform_types_manager::instance().get_types_resolver().get_item_data(one, temp);
		if (type != rx_item_type::rx_invalid_type)
		{
			result.items.emplace_back(api::query_result_detail{ type, temp });
		}
	}

	return result;
}

template<typename T>
rx_result transaction_algorithm::fill_simple_type_dependents(const rx_node_id& id, std::set<rx_node_id>& results)
{
	return platform_types_manager::instance().get_dependecies_cache().fill_dependents(id, results);
}
} // namespace algorithms
} // namespace model
} // namespace rx_internal

