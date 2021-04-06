

/****************************************************************************
*
*  sys_internal\rx_internal_ns.cpp
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


// rx_internal_ns
#include "sys_internal/rx_internal_ns.h"

#include "system/meta/rx_obj_types.h"
#include "testing/rx_test.h"
#include "sys_internal/rx_internal_builders.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/meta/rx_meta_algorithm.h"
#include "system/runtime/rx_holder_algorithms.h"


namespace rx_internal {

namespace internal_ns {

// Class rx_internal::internal_ns::platform_root 

rx_platform::ns::rx_names_cache platform_root::cache_;

platform_root::platform_root()
	: rx_platform_directory("", namespace_item_internal_access)
{
}


platform_root::~platform_root()
{
}



rx_namespace_item platform_root::get_cached_item (const string_type& name)
{
	return cache_.get_cached_item(name);
}

rx_result platform_root::insert_cached_item (const string_type& name, const rx_namespace_item& item)
{
	return cache_.insert_cached_item(name, item);
}


// Class rx_internal::internal_ns::user_directory 

user_directory::user_directory (const string_type& name)
	: rx_platform_directory(name, namespace_item_read_access | namespace_item_write_access | namespace_item_delete_access)
{
}


user_directory::~user_directory()
{
}



// Class rx_internal::internal_ns::unassigned_directory 

unassigned_directory::unassigned_directory()
	: rx_platform_directory(RX_NS_UNASSIGNED_NAME, namespace_item_internal_access, rx_create_reference<storage_base::rx_code_storage>())
{
    auto storage = resolve_storage();
    if (storage)
    {
        string_type base(RX_DIR_DELIMETER_STR);
        base += RX_NS_UNASSIGNED_NAME;
        storage.value()->set_base_path(base);
    }
}


unassigned_directory::~unassigned_directory()
{
}



// Class rx_internal::internal_ns::world_directory 

world_directory::world_directory()
	: rx_platform_directory(RX_NS_WORLD_NAME, namespace_item_internal_access, rx_gate::instance().get_host()->get_user_storage().value())
{
	auto storage = resolve_storage();
	if (storage)
	{
		string_type base(RX_DIR_DELIMETER_STR);
		base += RX_NS_WORLD_NAME;
		storage.value()->set_base_path(base);
	}
}


world_directory::~world_directory()
{
}



// Parameterized Class rx_internal::internal_ns::rx_item_implementation 

template <class TImpl>
rx_item_implementation<TImpl>::rx_item_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
rx_item_type rx_item_implementation<TImpl>::get_type_id () const
{
	return impl_->get_type_id();
}

template <class TImpl>
values::rx_value rx_item_implementation<TImpl>::get_value () const
{
    values::rx_value ret;
    ret.assign_static(impl_->meta_info().version);
	return ret;
}

template <class TImpl>
string_type rx_item_implementation<TImpl>::get_name () const
{
	return impl_->meta_info().name;
}

template <class TImpl>
rx_node_id rx_item_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_info().id;
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::serialize (base_meta_writer& stream) const
{
	auto ret = stream.write_header(STREAMING_TYPE_OBJECT, 0);
	if (ret)
	{
		ret = impl_->serialize(stream, STREAMING_TYPE_OBJECT);
		if (ret)
			stream.write_footer();
	}
	return ret;
}

template <class TImpl>
const meta_data_t& rx_item_implementation<TImpl>::meta_info () const
{
	return impl_->meta_info();
}

template <class TImpl>
void rx_item_implementation<TImpl>::fill_code_info (std::ostream& info, const string_type& name)
{
	impl_->get_implementation()->fill_code_info(info, name);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::read_value (const string_type& path, rx_value& value) const
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::read_value(path, value, *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, rx_result_callback callback, api::rx_context ctx)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::write_value(path, std::move(val), std::move(callback), ctx, *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::do_command (rx_object_command_t command_type)
{
	return impl_->do_command(command_type);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::browse(prefix, path, filter, items, *impl_);
}

template <class TImpl>
std::vector<rx_result_with<runtime_handle_t> > rx_item_implementation<TImpl>::connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::connect_items(paths, monitor, *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::read_items(items, monitor, *impl_);
}

template <class TImpl>
string_type rx_item_implementation<TImpl>::get_definition_as_json () const
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	impl_->serialize(writer, STREAMING_TYPE_OBJECT);

	writer.write_footer();

	string_type result;
	bool out = writer.get_string(result, true);

	if (out)
		return result;
	else
		return string_type();
}

template <class TImpl>
rx_platform_item::smart_ptr rx_item_implementation<TImpl>::clone () const
{
	return impl_->get_item_ptr();
}

template <class TImpl>
rx_thread_handle_t rx_item_implementation<TImpl>::get_executer () const
{
	return impl_->get_executer();
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor)
{
    return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::write_items(transaction_id, items, monitor, *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::serialize_value (base_meta_writer& stream, runtime_value_type type) const
{
    return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::serialize_runtime_value(stream, type, *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::deserialize_value (base_meta_reader& stream, runtime_value_type type)
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::save () const
{
	return rx_save_platform_item(*this);
}

template <class TImpl>
std::vector<rx_result> rx_item_implementation<TImpl>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::disconnect_items(items, monitor, *impl_);
}


// Parameterized Class rx_internal::internal_ns::rx_meta_item_implementation 

template <class TImpl>
rx_meta_item_implementation<TImpl>::rx_meta_item_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
rx_item_type rx_meta_item_implementation<TImpl>::get_type_id () const
{
	return impl_->type_id;
}

template <class TImpl>
values::rx_value rx_meta_item_implementation<TImpl>::get_value () const
{
	return impl_->meta_info.get_value();
}

template <class TImpl>
string_type rx_meta_item_implementation<TImpl>::get_name () const
{
	return impl_->meta_info.name;
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::fill_code_info (std::ostream& info, const string_type& name)
{
	impl_->fill_code_info(info, name);
}

template <class TImpl>
rx_node_id rx_meta_item_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_info.id;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::serialize (base_meta_writer& stream) const
{
	using algorithm_type = typename TImpl::pointee_type::algorithm_type;
	stream.write_header(STREAMING_TYPE_TYPE, 0);
	auto ret = algorithm_type::serialize_type(*impl_, stream, STREAMING_TYPE_TYPE);
	stream.write_footer();
	return ret;
}

template <class TImpl>
const meta_data_t& rx_meta_item_implementation<TImpl>::meta_info () const
{
	return impl_->meta_info;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::read_value (const string_type& path, rx_value& value) const
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, rx_result_callback callback, api::rx_context ctx)
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::do_command (rx_object_command_t command_type)
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	return "Not valid for this type!";
}

template <class TImpl>
std::vector<rx_result_with<runtime_handle_t> > rx_meta_item_implementation<TImpl>::connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor)
{
    std::vector<rx_result_with<runtime_handle_t> > result;
    result.reserve(paths.size());
    for (size_t idx = 0; idx < paths.size(); idx++)
    {
        result.emplace_back("Not valid for this type!");
    }
    return result;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return "Not valid for this type!";
}

template <class TImpl>
string_type rx_meta_item_implementation<TImpl>::get_definition_as_json () const
{
	using algorithm_type = typename TImpl::pointee_type::algorithm_type;

	rx_platform::serialization::json_writer writer;
	writer.write_header(STREAMING_TYPE_TYPE, 0);
	bool out = false;

	out = algorithm_type::serialize_type(*impl_, writer, STREAMING_TYPE_TYPE);

	writer.write_footer();

	string_type result;
	if (out)
	{
		out = writer.get_string(result, true);
	}

    if (out)
        return result;
    else
        return string_type();
}

template <class TImpl>
rx_platform_item::smart_ptr rx_meta_item_implementation<TImpl>::clone () const
{
    return impl_->get_item_ptr();
}

template <class TImpl>
rx_thread_handle_t rx_meta_item_implementation<TImpl>::get_executer () const
{
	return RX_DOMAIN_META;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor)
{
    return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::serialize_value (base_meta_writer& stream, runtime_value_type type) const
{
    return "Not valid for this type!";
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::deserialize_value (base_meta_reader& stream, runtime_value_type type)
{
    return "Not valid for this type!";
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::save () const
{
	return rx_save_platform_item(*this);
}

template <class TImpl>
std::vector<rx_result> rx_meta_item_implementation<TImpl>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor)
{
	std::vector<rx_result> result;
	result.reserve(items.size());
	for (size_t idx = 0; idx < items.size(); idx++)
	{
		result.emplace_back("Not valid for this type!");
	}
	return result;
}


// Class rx_internal::internal_ns::internal_directory 

internal_directory::internal_directory (const string_type& name)
	: rx_platform_directory(name, namespace_item_internal_access)
{
}


internal_directory::~internal_directory()
{
}



// Parameterized Class rx_internal::internal_ns::rx_other_implementation 

template <class TImpl>
rx_other_implementation<TImpl>::rx_other_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
rx_item_type rx_other_implementation<TImpl>::get_type_id () const
{
	return impl_->get_type_id();
}

template <class TImpl>
values::rx_value rx_other_implementation<TImpl>::get_value () const
{
	return impl_->get_value();
}

template <class TImpl>
string_type rx_other_implementation<TImpl>::get_name () const
{
	return impl_->get_name();
}

template <class TImpl>
void rx_other_implementation<TImpl>::fill_code_info (std::ostream& info, const string_type& name)
{
	impl_->fill_code_info(info, name);
}

template <class TImpl>
rx_node_id rx_other_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_info().id;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::serialize (base_meta_writer& stream) const
{
	return "Not valid for this item";
}

template <class TImpl>
const meta_data_t& rx_other_implementation<TImpl>::meta_info () const
{
	return impl_->meta_info();
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::read_value (const string_type& path, rx_value& value) const
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, rx_result_callback callback, api::rx_context ctx)
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::do_command (rx_object_command_t command_type)
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	return "Not valid for this type!";
}

template <class TImpl>
std::vector<rx_result_with<runtime_handle_t> > rx_other_implementation<TImpl>::connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor)
{
    std::vector<rx_result_with<runtime_handle_t> > result;
    result.reserve(paths.size());
    for (size_t idx = 0; idx < paths.size(); idx++)
    {
        result.emplace_back("Not valid for this type!");
    }
    return result;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return "Not valid for this type!";
}

template <class TImpl>
string_type rx_other_implementation<TImpl>::get_definition_as_json () const
{
    return string_type();
}

template <class TImpl>
rx_platform_item::smart_ptr rx_other_implementation<TImpl>::clone () const
{
	return impl_->get_item_ptr();
}

template <class TImpl>
rx_thread_handle_t rx_other_implementation<TImpl>::get_executer () const
{
	return RX_DOMAIN_META;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor)
{
    return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::serialize_value (base_meta_writer& stream, runtime_value_type type) const
{
    return "Not valid for this type!";
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::deserialize_value (base_meta_reader& stream, runtime_value_type type)
{
    return "Not valid for this type!";
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::save () const
{
	return rx_save_platform_item(*this);
}

template <class TImpl>
std::vector<rx_result> rx_other_implementation<TImpl>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor)
{
	std::vector<rx_result> result;
	result.reserve(items.size());
	for (size_t idx = 0; idx < items.size(); idx++)
	{
		result.emplace_back("Not valid for this type!");
	}
	return result;
}


// Class rx_internal::internal_ns::system_directory 

system_directory::system_directory()
	: rx_platform_directory(RX_NS_SYS_NAME, namespace_item_internal_access, rx_gate::instance().get_host()->get_system_storage("sys").value())
{
	auto storage = resolve_storage();
	if (storage)
	{
		string_type base(RX_DIR_DELIMETER_STR);
		base += RX_NS_SYS_NAME;
		storage.value()->set_base_path(base);
	}
}


system_directory::~system_directory()
{
}



// Class rx_internal::internal_ns::host_directory 

host_directory::host_directory()
	: rx_platform_directory(RX_NS_HOST_NAME, namespace_item_internal_access
		, rx_gate::instance().get_host()->get_system_storage(rx_gate::instance().get_host()->get_host_name()).value())
{
	auto storage = resolve_storage();
	if (storage)
	{
		string_type base(RX_DIR_DELIMETER_STR);
		base += RX_NS_SYS_NAME;
		base += RX_DIR_DELIMETER_STR;
		base += RX_NS_HOST_NAME;
		storage.value()->set_base_path(base);
	}
}


host_directory::~host_directory()
{
}



// Class rx_internal::internal_ns::plugin_directory 

plugin_directory::plugin_directory (rx_plugin_ptr plugin)
	: rx_platform_directory(plugin->get_plugin_name(), namespace_item_internal_access
		, rx_gate::instance().get_host()->get_system_storage(plugin->get_plugin_name()).value())
{
	auto storage = resolve_storage();
	if (storage)
	{
		string_type base(RX_DIR_DELIMETER_STR);
		base += RX_NS_SYS_NAME;
		base += RX_DIR_DELIMETER_STR;
		base += RX_NS_PLUGINS_NAME;
		base += RX_DIR_DELIMETER_STR;
		base += plugin->get_plugin_name();
		storage.value()->set_base_path(base);
	}
}


plugin_directory::~plugin_directory()
{
}



} // namespace internal_ns
} // namespace rx_internal

template class rx_internal::internal_ns::rx_item_implementation<rx_domain_ptr>;
template class rx_internal::internal_ns::rx_item_implementation<rx_application_ptr>;
template class rx_internal::internal_ns::rx_item_implementation<rx_object_ptr>;
template class rx_internal::internal_ns::rx_item_implementation<rx_port_ptr>;
template class rx_internal::internal_ns::rx_other_implementation<testing::test_case::smart_ptr>;

template class rx_internal::internal_ns::rx_meta_item_implementation<rx_application_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<rx_domain_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<rx_port_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<rx_object_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<relation_type_ptr>;

template class rx_internal::internal_ns::rx_meta_item_implementation<struct_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<mapper_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<variable_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<event_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<filter_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<source_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<method_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<program_type_ptr>;
template class rx_internal::internal_ns::rx_meta_item_implementation<display_type_ptr>;

template class rx_internal::internal_ns::rx_meta_item_implementation<data_type_ptr>;
