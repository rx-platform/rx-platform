

/****************************************************************************
*
*  model\rx_model_algorithms.cpp
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


// rx_model_algorithms
#include "model/rx_model_algorithms.h"

#include "runtime_internal/rx_runtime_internal.h"
#include "system/server/rx_async_functions.h"
#include "lib/rx_lib.h"
#include "sys_internal/rx_internal_ns.h"


namespace model {

namespace algorithms {
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

using namespace rx;

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
//helper functions, anonymus namespace i think is perfect for theese
namespace
{


rx_result_with<rx_node_id> resolve_some_reference(const item_reference& ref, ns::rx_directory_resolver& directories, meta_data& info, rx_item_type& ret_type)
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
		ret = item.get_meta().get_id();
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
rx_result delete_some_type(typeCache& cache, const item_reference& item_reference, rx_directory_ptr dir, rx_transaction_type& transaction)
{
	rx_node_id id;
	string_type name;
	rx_namespace_item item;
	if (!dir)
		dir = rx_gate::instance().get_root_directory();
	if (!item_reference.is_node_id())
	{
		name = item_reference.get_path();
		item = dir->get_sub_item(name);
		if (!item)
		{// error, item does not exists
			return name + " does not exists";
		}
		id = item.get_meta().get_id();
		if (id.is_null())
		{// error, item does not have id
			return name + " does not have id";
		}
	}
	else
	{
		if(item_reference.is_null())
		{// error, item does not have id
			return "Invalid " RX_NULL_ITEM_NAME " id.";
		}
		id = item_reference.get_node_id();
		meta_data item_meta;
		platform_types_manager::instance().get_types_resolver().get_item_data(id, item_meta);

		item = dir->get_sub_item(item_meta.get_full_path());
		if (!item)
		{// error, item does not exists
			return item_meta.get_full_path() + " does not exists";
		}
		name = item.get_meta().get_name();
		dir = rx_gate::instance().get_root_directory()->get_sub_directory(item.get_meta().get_path());
	}
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto work_item = get_platform_item_sync(typeCache::HType::type_id, id);
		if(!work_item)
		{
			work_item.register_error("Error retriveing type item "s + item.get_meta().get_full_path());
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
rx_result_with<typeType> create_some_type(typeCache& cache, const string_type& name, const item_reference& base_reference, typeType prototype, rx_directory_ptr dir, namespace_item_attributes attributes, rx_transaction_type& transaction)
{
	if (!prototype)
		prototype = typeType(pointers::_create_new);

	prototype->meta_info().resolve();

	rx_node_id base_id = prototype->meta_info().get_parent();
	rx_node_id item_id = prototype->meta_info().get_id();
	string_type type_name = prototype->meta_info().get_name();
	if (!attributes)
		attributes = prototype->meta_info().get_attributes();

	if (!dir)
	{
		if (!prototype->meta_info().get_path().empty())
			dir = rx_gate::instance().get_root_directory()->get_sub_directory(prototype->meta_info().get_path());
		if (!dir)
			dir = rx_gate::instance().get_root_directory();
	}

	if (type_name.empty())
	{
		string_type sub_dir;
		rx_split_path(name, sub_dir, type_name);
		if (!sub_dir.empty())
		{
			dir = dir->get_sub_directory(sub_dir);
			if (!dir)
			{
				return sub_dir + " is invalid path!";
			}
		}
	}

	string_type path;
	auto dir_result = dir->reserve_name(type_name, path);
	if (!dir_result)
		return dir_result.errors();

	transaction.push([=] () mutable {
		auto cancel_reserve = dir->cancel_reserve(type_name);
		});

	if (!base_id && !base_reference.is_null())
	{
		if (base_reference.is_node_id())
		{
			base_id = base_reference.get_node_id();
		}
		else
		{
			auto item = dir->get_sub_item(base_reference.get_path());
			if (!item)
			{// type does not exists
				return "Type "s + base_reference.get_path() + " does not exists!";
			}
			base_id = item.get_meta().get_id();
			if (base_id.is_null())
			{// item does not have id
				return base_reference.get_path() + " does not have valid Id!";
			}
		}
	}
	if (!item_id)
		item_id = rx_node_id::generate_new();

	prototype->meta_info().construct(type_name, item_id, base_id, attributes, path);
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
rx_result_with<typeT> update_some_type(typeCache& cache, typeT prototype, rx_directory_ptr dir, bool increment_version, rx_transaction_type& transaction)
{
	if (!dir)
		dir = rx_gate::instance().get_root_directory();

	prototype->meta_info().resolve();
	prototype->meta_info().increment_version(increment_version);
	type_check_context ctx;
	ctx.get_directories().add_paths({ prototype->meta_info().get_path() });
	auto result = prototype->check_type(ctx);
	if (!result)
	{
		return ctx.get_errors();
	}

	auto ret = cache.get_type_definition(prototype->meta_info().get_id());
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
rx_result delete_some_runtime(const item_reference& item_reference, rx_directory_ptr dir, rx_thread_handle_t result_target, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	rx_node_id id;
	string_type name;
	rx_namespace_item item;
	if (!dir)
		dir = rx_gate::instance().get_root_directory();
	if (!item_reference.is_node_id())
	{
		name = item_reference.get_path();
		item = dir->get_sub_item(name);
		if (!item)
		{// error, item does not exists
			return name + " does not exists";
		}
		id = item.get_meta().get_id();
		if (id.is_null())
		{// error, item does not have id
			return name + " does not have id";
		}
	}
	else
	{
		if (item_reference.is_null())
		{// error, item does not have id
			return "Invalid " RX_NULL_ITEM_NAME " id.";
		}
		id = item_reference.get_node_id();
		meta_data item_meta;
		platform_types_manager::instance().get_types_resolver().get_item_data(id, item_meta);

		item = dir->get_sub_item(item_meta.get_full_path());
		if (!item)
		{// error, item does not exists
			return item_meta.get_full_path() + " does not exists";
		}
		name = item.get_meta().get_name();
		dir = rx_gate::instance().get_root_directory()->get_sub_directory(item.get_meta().get_path());;
	}
	auto obj_ptr = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		return obj_ptr.errors();
	}

	runtime::runtime_deinit_context ctx;
	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(), [name, dir, id, item, callback](rx_result&& deinit_result) mutable
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
				auto work_item = get_platform_item_sync(typeT::RType::type_id, id);
				if (!work_item)
				{
					work_item.register_error("Error retriveing type item "s + item.get_meta().get_full_path());
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
					auto work_item = get_platform_item_sync(typeT::RType::type_id, id);
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

		}, ctx);
	return true;
}
template<class typeCache>
rx_result_with<typename typeCache::RTypePtr> create_some_runtime(typeCache& cache,const meta_data& info , data::runtime_values_data* init_data, typename typeCache::HType::instance_data_t instance_data, rx_directory_ptr dir, namespace_item_attributes attributes, rx_transaction_type& transaction)
{
	rx_node_id base_id = info.get_parent();
	rx_node_id item_id = info.get_id();
	string_type full_name = info.get_full_path();
	if (!attributes)
		attributes = info.get_attributes();

	if (!dir)
	{
		if (!info.get_path().empty())
			dir = rx_gate::instance().get_root_directory()->get_sub_directory(info.get_path());
		if (!dir)
			dir = rx_gate::instance().get_root_directory();
	}

	string_type sub_dir;
	string_type  runtime_name;
	rx_split_path(full_name, sub_dir, runtime_name);
	if (!sub_dir.empty())
	{
		dir = dir->get_sub_directory(sub_dir);
		if (!dir)
		{
			return sub_dir + " is invalid path!";
		}
	}

	string_type path;
	auto result = dir->reserve_name(runtime_name, path);
	if (!result)
		return result.errors();

	transaction.push([=]() mutable {
		auto cancel_reserve = dir->cancel_reserve(runtime_name);
		});


	if (!item_id)
		item_id = rx_node_id::generate_new();


	meta_data meta(info);
	meta.construct(runtime_name, item_id, base_id, attributes, path);
	meta.resolve();
	auto ret_value = cache.create_runtime(meta, std::move(instance_data), init_data);
	if (!ret_value)
	{// error, didn't created runtime
		ret_value.register_error("Unable to create runtime in repository.");
		return ret_value;
	}

	transaction.push([&cache, item_id]() mutable {
		auto delete_result = cache.delete_runtime(item_id);
		});

	result = dir->add_item(ret_value.value()->get_item_ptr());
	if (!result)
	{
		// error, can't add this name
		result.register_error("Unable to add "s + runtime_name + " to directory!");
		return result.errors();
	}
	else if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		transaction.push([=]() mutable {
			auto remove_result = dir->delete_item(runtime_name);
			});
		// we have to do save, we are running
		auto save_result = ret_value.value()->get_item_ptr()->save();
		if (!save_result)
		{
			save_result.register_error("Error saving type item "s + ret_value.value()->meta_info().get_full_path());
			return save_result.errors();
		}
	}
	ret_value = cache.mark_runtime_running(item_id);
	if (!ret_value)
	{
		// error, can't add this name
		ret_value.register_error("Unable to mark "s + runtime_name + " as running!");
		return ret_value;
	}
	transaction.push([&cache, item_id]() mutable {
		auto mark_result = cache.mark_runtime_for_delete(item_id);
		});

	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		runtime::runtime_init_context ctx;
		result = sys_runtime::platform_runtime_manager::instance().init_runtime<typename typeCache::HType>(ret_value.value(), ctx);
		if (!result)
		{
			result.register_error("Unable to initialize "s + runtime_name);
			return result.errors();
		}
		META_LOG_INFO("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::RType::type_id) + " "s + meta.get_full_path());
	}
	else
		META_LOG_TRACE("runtime_model_algorithm", 100, "Created "s + rx_item_type_name(typeCache::RType::type_id) + " "s + meta.get_full_path());

	return ret_value;
}

}

rx_result_with<rx_node_id> resolve_reference(const item_reference& ref, ns::rx_directory_resolver& directories)
{
	rx_item_type type;
	meta_data temp;
	return resolve_some_reference(ref, directories, temp, type);
}


template<typename typeT>
rx_result_with<rx_node_id> resolve_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<typeT>)
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

template rx_result_with<rx_node_id> resolve_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<object_type>);
template rx_result_with<rx_node_id> resolve_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<port_type>);
template rx_result_with<rx_node_id> resolve_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<domain_type>);
template rx_result_with<rx_node_id> resolve_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<application_type>);


rx_result_with<rx_node_id> resolve_relation_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories)
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
rx_result_with<rx_node_id> resolve_simple_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<typeT>)
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

template rx_result_with<rx_node_id> resolve_simple_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<struct_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<variable_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<source_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<filter_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<event_type>);
template rx_result_with<rx_node_id> resolve_simple_type_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<mapper_type>);

template<typename typeT>
rx_result_with<rx_node_id> resolve_runtime_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<typeT>)
{
	rx_item_type type;
	meta_data info;
	auto result = resolve_some_reference(ref, directories, info, type);
	if (!result)
		return result;
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		if (type != typeT::RType::type_id)
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

template rx_result_with<rx_node_id> resolve_runtime_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<object_type>);
template rx_result_with<rx_node_id> resolve_runtime_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<domain_type>);
template rx_result_with<rx_node_id> resolve_runtime_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<port_type>);
template rx_result_with<rx_node_id> resolve_runtime_reference(const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<application_type>);


rx_result_with<platform_item_ptr> get_working_runtime_sync(const rx_node_id& id)
{
	meta::meta_data info;
	auto type = model::platform_types_manager::instance().get_types_resolver().get_item_data(id, info);
	if (type == rx_item_type::rx_invalid_type)
	{
		return (id.to_string() + " is not the registered id!");
	}
	switch (type)
	{
	case rx_item_type::rx_object:
		{
			auto ret_val = platform_types_manager::instance().get_type_repository<object_type>().get_runtime(id);
			if (ret_val)
				return ret_val.value()->get_item_ptr();
			else
				return ret_val.errors();
		}
		break;
	case rx_item_type::rx_domain:
		{
			auto ret_val = platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(id);
			if (ret_val)
				return ret_val.value()->get_item_ptr();
			else
				return ret_val.errors();
		}
		break;
	case rx_item_type::rx_application:
		{
			auto ret_val = platform_types_manager::instance().get_type_repository<application_type>().get_runtime(id);
			if (ret_val)
				return ret_val.value()->get_item_ptr();
			else
				return ret_val.errors();
		}
		break;
	case rx_item_type::rx_port:
		{
			auto ret_val = platform_types_manager::instance().get_type_repository<port_type>().get_runtime(id);
			if (ret_val)
				return ret_val.value()->get_item_ptr();
			else
				return ret_val.errors();
		}
		break;
	default:
		{
			return "Can't retreive details about "s + rx_item_type_name(type);
		}
	}
}

std::vector<rx_result_with<platform_item_ptr> > get_working_runtimes_sync(const rx_node_ids& ids)
{
	std::vector<rx_result_with<platform_item_ptr> > return_value;
	return_value.reserve(ids.size());
	for (const auto& id : ids)
	{
		return_value.emplace_back(get_working_runtime_sync(id));
	}
	return return_value;
}



//////////////////////////////////////////////////////////////////////////////////////////

// Parameterized Class model::algorithms::types_model_algorithm


template <class typeT>
void types_model_algorithm<typeT>::check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref)
{
	std::function<type_check_context(const string_type, rx_directory_ptr)> func = [=](const string_type loc_name, rx_directory_ptr loc_dir) mutable {
		return check_type_sync(loc_name, loc_dir);
	};
	rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
}

template <class typeT>
type_check_context types_model_algorithm<typeT>::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	type_check_context ret;
	auto item = dir->get_sub_item(name);
	if (!item)
	{
		ret.add_error(name + " does not exists!");
		return ret;
	}
	auto id = item.get_meta().get_id();
	if (id.is_null())
	{// error, item does not have id
		ret.add_error(name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!");
		return ret;
	}
	auto result = platform_types_manager::instance().get_type_repository<typeT>().check_type(id, ret);
	return ret;
}

template <class typeT>
void types_model_algorithm<typeT>::create_type (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return create_type_sync(name, base_reference, prototype, dir, attributes);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::create_type_sync (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes)
{
	rx_transaction_type transaction;
	auto result = create_some_type(platform_types_manager::instance().get_type_repository<typeT>()
		, name, base_reference, prototype,
		dir, attributes, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
void types_model_algorithm<typeT>::delete_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	std::function<rx_result(void)> func = [=]() {
		return delete_type_sync(item_reference, dir);
	};
	rx_do_with_callback<rx_result, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result types_model_algorithm<typeT>::delete_type_sync (const item_reference& item_reference, rx_directory_ptr dir)
{
	rx_transaction_type transaction;
	auto result = delete_some_type(platform_types_manager::instance().get_type_repository<typeT>(), item_reference, dir, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
void types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return update_type_sync(prototype, dir, increment_version);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_type_repository<typeT>(), prototype, dir, increment_version, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
void types_model_algorithm<typeT>::get_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return get_type_sync(item_reference, dir);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> types_model_algorithm<typeT>::get_type_sync (const item_reference& item_reference, rx_directory_ptr dir)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto item = dir->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().get_id();
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_type_repository<typeT>().get_type_definition(id);
}


// Parameterized Class model::algorithms::simple_types_model_algorithm


template <class typeT>
void simple_types_model_algorithm<typeT>::check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref)
{
	std::function<type_check_context(const string_type, rx_directory_ptr)> func = [=](const string_type loc_name, rx_directory_ptr loc_dir) mutable {
		return check_type_sync(loc_name, loc_dir);
	};
	rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
}

template <class typeT>
type_check_context simple_types_model_algorithm<typeT>::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	type_check_context ret;
	auto item = dir->get_sub_item(name);
	if (!item)
	{
		ret.add_error(name + " does not exists!");
		return ret;
	}
	auto id = item.get_meta().get_id();
	if (id.is_null())
	{// error, item does not have id
		ret.add_error(name + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!");
		return ret;
	}
	platform_types_manager::instance().get_simple_type_repository<typeT>().check_type(id, ret);
	return ret;
}

template <class typeT>
void simple_types_model_algorithm<typeT>::create_type (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return create_type_sync(name, base_reference, prototype, dir, attributes);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::create_type_sync (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes)
{
	rx_transaction_type transaction;
	auto result = create_some_type(platform_types_manager::instance().get_simple_type_repository<typeT>()
		, name, base_reference, prototype,
		dir, attributes, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
void simple_types_model_algorithm<typeT>::delete_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	std::function<rx_result(void)> func = [=]() {
		return delete_type_sync(item_reference, dir);
	};
	rx_do_with_callback<rx_result, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result simple_types_model_algorithm<typeT>::delete_type_sync (const item_reference& item_reference, rx_directory_ptr dir)
{
	rx_transaction_type transaction;
	auto result = delete_some_type(platform_types_manager::instance().get_simple_type_repository<typeT>(), item_reference, dir, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
void simple_types_model_algorithm<typeT>::update_type (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return update_type_sync(prototype, dir, increment_version);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::update_type_sync (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_simple_type_repository<typeT>(), prototype, dir, increment_version, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
void simple_types_model_algorithm<typeT>::get_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return get_type_sync(item_reference, dir);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::smart_ptr> simple_types_model_algorithm<typeT>::get_type_sync (const item_reference& item_reference, rx_directory_ptr dir)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto item = dir->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().get_id();
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_simple_type_repository<typeT>().get_type_definition(id);
}


// Parameterized Class model::algorithms::runtime_model_algorithm


template <class typeT>
void runtime_model_algorithm<typeT>::delete_runtime (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	auto result_target = rx_thread_context();
	std::function<void(rx_thread_handle_t, rx_reference_ptr)> func= [=](rx_thread_handle_t result_target, rx_reference_ptr ref)
		{
			auto res = delete_runtime_sync(item_reference, dir, result_target, callback, ref);
			if (!res)
			{
				auto jt = rx_gate::instance().get_infrastructure().get_executer(result_target);
				jt->append(
					rx_create_reference<jobs::lambda_job<rx_result, rx_reference_ptr> >(
						[=](rx_result&& ret_val) mutable
						{
							callback(std::move(ret_val));
						},
						std::move(res), ref));
			}
		};
	rx_post_function_to<rx_reference_ptr, rx_thread_handle_t, rx_reference_ptr>(RX_DOMAIN_META, func
		, ref, result_target, ref);
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::delete_runtime_sync (const item_reference& item_reference, rx_directory_ptr dir, rx_thread_handle_t result_target, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	auto result = delete_some_runtime<typeT>(item_reference, dir, result_target, callback, ref);
	return result;
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_runtime (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	std::function<rx_result_with<typename typeT::RTypePtr>()> func = [info, init_data, dir, ref, instance_data]() mutable {
		return create_runtime_sync(info, init_data, std::move(instance_data), dir, ref);
	};
	rx_do_with_callback<rx_result_with<typename typeT::RTypePtr>, rx_reference_ptr>(std::move(func), RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_runtime_sync (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, rx_reference_ptr ref)
{
	rx_transaction_type transaction;
	auto result = create_some_runtime<types_repository<typeT> >(platform_types_manager::instance().get_type_repository<typeT>()
		, info, init_data, instance_data
		, dir, namespace_item_attributes::namespace_item_full_access, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_prototype (const meta_data& info, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	std::function<rx_result_with<typename typeT::RTypePtr>()> func = [info, instance_data, dir]() mutable {
		return create_prototype_sync(info, instance_data, dir);
	};
	rx_do_with_callback<rx_result_with<typename typeT::RTypePtr>, rx_reference_ptr>(std::move(func), RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_prototype_sync (const meta_data& info, typename typeT::instance_data_t instance_data, rx_directory_ptr dir)
{
	string_type path;
	auto dir_result = dir->reserve_name(info.get_name(), path);
	if (dir_result)
		dir->cancel_reserve(info.get_name()); // cancel it straight away, this is just prototype!!!
	if (!dir_result)
		return dir_result.errors();

	rx_node_id super_id(info.get_parent());

	namespace_item_attributes attributes = info.get_attributes() == namespace_item_attributes::namespace_item_null ?
		namespace_item_attributes::namespace_item_full_access : info.get_attributes();
	meta_data meta;
	meta.construct(info.get_name(), rx_node_id::null_id, super_id, attributes, path);
	auto ret = platform_types_manager::instance().get_type_repository<typeT>().create_runtime(meta, std::move(instance_data), nullptr, true);

	return ret;
}

template <class typeT>
void runtime_model_algorithm<typeT>::create_runtime_implicit (const string_type& name, const item_reference& base_reference, namespace_item_attributes attributes, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	std::function<rx_result_with<typename typeT::RTypePtr>()> func = [name, base_reference, attributes, init_data, dir, ref, instance_data]() mutable {
		return create_runtime_implicit_sync(name, base_reference, attributes, init_data, std::move(instance_data), dir, ref);
	};
	rx_do_with_callback<rx_result_with<typename typeT::RTypePtr>, rx_reference_ptr>(std::move(func), RX_DOMAIN_META, callback, ref);

}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::create_runtime_implicit_sync (const string_type& name, const item_reference& base_reference, namespace_item_attributes attributes, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, rx_reference_ptr ref)
{
	rx_transaction_type transaction;
	meta_data base_meta;
	rx_item_type ret_type;
	ns::rx_directory_resolver resolver;
	auto resolved = resolve_some_reference(base_reference, resolver, base_meta, ret_type);
	if (!resolved)
		return resolved.errors();
	string_type path;
	dir->fill_path(path);
	meta_data meta;
	meta.construct(name, rx_node_id::null_id, resolved.value(), attributes, path);
	auto result = create_some_runtime<types_repository<typeT> >(platform_types_manager::instance().get_type_repository<typeT>()
		, meta, init_data, instance_data
		, dir, attributes, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::init_runtime (typename typeT::RTypePtr what)
{
	runtime::runtime_init_context ctx;
	auto init_result = sys_runtime::platform_runtime_manager::instance().init_runtime<typeT>(what, ctx);
	if (init_result)
	{// make object running in state

		auto init_result = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_running(what->meta_info().get_id());
	}
	return init_result;
}

template <class typeT>
void runtime_model_algorithm<typeT>::update_runtime (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, bool increment_version, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::RTypePtr>;
	auto result_target = rx_thread_context();
	std::function<void(rx_thread_handle_t)> func = [=](rx_thread_handle_t target) {
		auto ret = update_runtime_sync(info, init_data, instance_data, increment_version, dir, callback, ref, target);
		if (!ret)
			rx_post_result_to<decltype(ref), typename typeT::RTypePtr>(result_target, [callback](result_t&& res)
				{
					callback(std::move(res));
				}, ref, result_t(ret.errors()));
	};
	rx_post_function_to<decltype(ref), rx_thread_handle_t>(RX_DOMAIN_META, func, ref, result_target);
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::update_runtime_sync (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, bool increment_version, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref, rx_thread_handle_t result_target)
{
	auto id = info.get_id();
	if (id.is_null())
	{// error, item does not have id
		return info.get_name() + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!";
	}
	auto obj_ptr = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		return obj_ptr.errors();
	}

	runtime::runtime_deinit_context ctx;
	meta_data meta_info(info);
	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(), [meta_info, increment_version, callback, init_data, instance_data, dir, ref] (rx_result&& deinit_result) mutable
		{
			auto ret = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(meta_info.get_id());
			if (ret)
			{
				auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(meta_info.get_path());
				if (dir)
					dir->delete_item(meta_info.get_name());
				if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
				{
					auto storage_result = meta_info.resolve_storage();
					if (storage_result)
					{
						auto item_result = storage_result.value()->get_item_storage(meta_info);
						if (item_result)
						{
							item_result.value()->delete_item();
							META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::type_id) + " "s + meta_info.get_name());
						}
					}
				}
				meta_info.increment_version(increment_version);
				auto create_result = create_runtime_sync(meta_info, init_data, instance_data, dir, ref);
				callback(std::move(create_result));
			}
			if (!ret)
				callback(ret.errors());
		}, ctx);

	return result;
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::helper_delete_runtime_sync (meta_data_t info)
{
	auto id = info.get_id();
	if (id.is_null())
	{// error, item does not have id
		return info.get_name() + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!";
	}
	auto ret = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(id);
	if (ret)
	{
		auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(info.get_path());
		if (dir)
			dir->delete_item(info.get_name());
		if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
		{
			auto storage_result = info.resolve_storage();
			if (storage_result)
			{
				auto item_result = storage_result.value()->get_item_storage(info);
				if (item_result)
				{
					item_result.value()->delete_item();
					META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::type_id) + " "s + info.get_name());
				}
			}
		}
	}
	return ret;
}

template <class typeT>
rx_result runtime_model_algorithm<typeT>::delete_runtime_sync (meta_data_t info, rx_thread_handle_t result_target, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	auto id = info.get_id();
	if (id.is_null())
	{// error, item does not have id
		return info.get_name() + " does not have valid " + rx_item_type_name(typeT::type_id) + " id!";
	}
	auto obj_ptr = platform_types_manager::instance().get_type_repository<typeT>().mark_runtime_for_delete(id);
	if (!obj_ptr)
	{
		return obj_ptr.errors();
	}

	runtime::runtime_deinit_context ctx;
	auto result = sys_runtime::platform_runtime_manager::instance().deinit_runtime<typeT>(obj_ptr.value(), [info, callback](rx_result&& deinit_result)
		{
			auto ret = platform_types_manager::instance().get_type_repository<typeT>().delete_runtime(info.get_id());
			if (ret)
			{
				auto dir = rx_gate::instance().get_root_directory()->get_sub_directory(info.get_path());
				if (dir)
					dir->delete_item(info.get_name());
				if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
				{
					auto storage_result = info.resolve_storage();
					if (storage_result)
					{
						auto item_result = storage_result.value()->get_item_storage(info);
						if (item_result)
						{
							item_result.value()->delete_item();
							META_LOG_TRACE("runtime_model_algorithm", 100, "Deleted "s + rx_item_type_name(typeT::RType::type_id) + " "s + info.get_name());
						}
					}
				}
			}
			callback(std::move(ret));
		}, ctx);

	return result;
}

template <class typeT>
void runtime_model_algorithm<typeT>::get_runtime (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename typeT::RTypePtr>;
	std::function<result_t(void)> func = [=]() {
		return get_runtime_sync(item_reference, dir);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> runtime_model_algorithm<typeT>::get_runtime_sync (const item_reference& item_reference, rx_directory_ptr dir)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto item = dir->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().get_id();
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_type_repository<typeT>().get_runtime(id);
}


// Class model::algorithms::relation_types_algorithm


void relation_types_algorithm::check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref)
{
	std::function<type_check_context(const string_type, rx_directory_ptr)> func = [=](const string_type loc_name, rx_directory_ptr loc_dir) mutable {
		return check_type_sync(loc_name, loc_dir);
	};
	rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
}

type_check_context relation_types_algorithm::check_type_sync (const string_type& name, rx_directory_ptr dir)
{
	type_check_context ret;
	rx_namespace_item item = dir->get_sub_item(name);
	if (!item)
	{
		ret.add_error(name + " does not exists!");
		return ret;
	}
	auto id = item.get_meta().get_id();
	if (id.is_null())
	{// error, item does not have id
		ret.add_error(name + " does not have valid " + rx_item_type_name(relation_type::type_id) + " id!");
		return ret;
	}
	platform_types_manager::instance().get_relations_repository().check_type(id, ret);
	return ret;
}

void relation_types_algorithm::create_type (const string_type& name, const item_reference& base_reference, relation_type::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<relation_type::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<relation_type::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return create_type_sync(name, base_reference, prototype, dir, attributes);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

rx_result_with<relation_type::smart_ptr> relation_types_algorithm::create_type_sync (const string_type& name, const item_reference& base_reference, relation_type::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes)
{
	rx_transaction_type transaction;
	auto result = create_some_type(platform_types_manager::instance().get_relations_repository()
		, name, base_reference, prototype,
		dir, attributes, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

void relation_types_algorithm::delete_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref)
{
	std::function<rx_result(void)> func = [=]() {
		return delete_type_sync(item_reference, dir);
	};
	rx_do_with_callback<rx_result, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

rx_result relation_types_algorithm::delete_type_sync (const item_reference& item_reference, rx_directory_ptr dir)
{
	rx_transaction_type transaction;
	auto result = delete_some_type(platform_types_manager::instance().get_relations_repository(), item_reference, dir, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

void relation_types_algorithm::update_type (relation_type::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<relation_type::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename relation_type::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return update_type_sync(prototype, dir, increment_version);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

rx_result_with<relation_type::smart_ptr> relation_types_algorithm::update_type_sync (relation_type::smart_ptr prototype, rx_directory_ptr dir, bool increment_version)
{
	rx_transaction_type transaction;
	auto result = update_some_type(platform_types_manager::instance().get_relations_repository(), prototype, dir, increment_version, transaction);
	if (result)
	{
		transaction.commit();
	}
	return result;
}

void relation_types_algorithm::get_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<relation_type::smart_ptr>&&)> callback, rx_reference_ptr ref)
{
	using result_t = rx_result_with<typename relation_type::smart_ptr>;
	std::function<result_t(void)> func = [=]() {
		return get_type_sync(item_reference, dir);
	};
	rx_do_with_callback<result_t, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ref);
}

rx_result_with<relation_type::smart_ptr> relation_types_algorithm::get_type_sync (const item_reference& item_reference, rx_directory_ptr dir)
{
	rx_node_id id;
	if (item_reference.is_node_id())
	{
		id = item_reference.get_node_id();
	}
	else
	{
		auto item = dir->get_sub_item(item_reference.get_path());
		if (item)
		{
			id = item.get_meta().get_id();
		}
		else
		{
			return item_reference.get_path() + " is not valid path.";
		}
	}
	return platform_types_manager::instance().get_relations_repository().get_type_definition(id);
}


} // namespace algorithms
} // namespace model

