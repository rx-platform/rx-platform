

/****************************************************************************
*
*  sys_internal\rx_internal_ns.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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

#include "lib/rx_ser_json.h"
#include "system/runtime/rx_blocks.h"
#include "system/meta/rx_obj_types.h"
#include "testing/rx_test.h"
#include "sys_internal/rx_internal_builders.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/meta/rx_meta_algorithm.h"
#include "system/runtime/rx_holder_algorithms.h"
#include "lib/rx_ser_bin.h"


namespace rx_internal {

namespace internal_ns {

// Parameterized Class rx_internal::internal_ns::rx_item_implementation 

template <class TImpl>
rx_item_implementation<TImpl>::rx_item_implementation (TImpl impl)
      : impl_(impl)
{
}


template <class TImpl>
rx_item_implementation<TImpl>::~rx_item_implementation()
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
void rx_item_implementation<TImpl>::read_value (const string_type& path, read_result_callback_t callback) const
{
	runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::read_value(path, std::move(callback), *impl_);
}

template <class TImpl>
void rx_item_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, write_result_callback_t callback)
{
	runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::write_value(path, std::move(val), std::move(callback), *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::do_command (rx_object_command_t command_type)
{
	return impl_->do_command(command_type);
}

template <class TImpl>
void rx_item_implementation<TImpl>::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback)
{
	runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::browse(prefix, path, filter, std::move(callback), *impl_);
}

template <class TImpl>
std::vector<rx_result_with<runtime_handle_t> > rx_item_implementation<TImpl>::connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::connect_items(paths, monitor, *impl_);
}

template <class TImpl>
std::vector<rx_result> rx_item_implementation<TImpl>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::disconnect_items(items, monitor, *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor, api::rx_context ctx)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::read_items(items, monitor, *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor)
{
    return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::write_items(transaction_id, items, monitor, *impl_);
}

template <class TImpl>
string_type rx_item_implementation<TImpl>::get_definition_as_json () const
{
	serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	impl_->serialize(writer, STREAMING_TYPE_OBJECT);

	writer.write_footer();

	return writer.get_string();
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
void rx_item_implementation<TImpl>::read_struct (string_view_type path, read_struct_data data) const
{
	runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::read_struct(path, std::move(data), *impl_);
}

template <class TImpl>
void rx_item_implementation<TImpl>::write_struct (string_view_type path, write_struct_data data)
{
	runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::write_struct(path, std::move(data), *impl_);
}

template <class TImpl>
void rx_item_implementation<TImpl>::execute_method (const string_type& path, data::runtime_values_data data, execute_method_callback_t callback)
{
	runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::execute_method(path, std::move(data), std::move(callback), *impl_);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::execute_item (runtime_transaction_id_t transaction_id, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor)
{
	return runtime::algorithms::runtime_holder_algorithms<typename TImpl::pointee_type::DefType>::execute_item(transaction_id, handle, data, monitor, *impl_);
}

template <class TImpl>
byte_string rx_item_implementation<TImpl>::get_definition_as_bytes () const
{
	memory::std_buffer buff;
	serialization::std_buffer_writer writer(buff);

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	impl_->serialize(writer, STREAMING_TYPE_OBJECT);

	writer.write_footer();

	return writer.get_data();
}


// Parameterized Class rx_internal::internal_ns::rx_meta_item_implementation 

template <class TImpl>
rx_meta_item_implementation<TImpl>::rx_meta_item_implementation (TImpl impl)
      : impl_(impl)
{
}


template <class TImpl>
rx_meta_item_implementation<TImpl>::~rx_meta_item_implementation()
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
void rx_meta_item_implementation<TImpl>::read_value (const string_type& path, read_result_callback_t callback) const
{
	callback(RX_NOT_IMPLEMENTED, rx_value());
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, write_result_callback_t callback)
{
	callback(RX_NOT_IMPLEMENTED);
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::do_command (rx_object_command_t command_type)
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback)
{
	callback(RX_NOT_VALID_TYPE, std::vector<runtime_item_attribute>());
}

template <class TImpl>
std::vector<rx_result_with<runtime_handle_t> > rx_meta_item_implementation<TImpl>::connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor)
{
    std::vector<rx_result_with<runtime_handle_t> > result;
    result.reserve(paths.size());
    for (size_t idx = 0; idx < paths.size(); idx++)
    {
        result.emplace_back(RX_NOT_VALID_TYPE);
    }
    return result;
}

template <class TImpl>
std::vector<rx_result> rx_meta_item_implementation<TImpl>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor)
{
	std::vector<rx_result> result;
	result.reserve(items.size());
	for (size_t idx = 0; idx < items.size(); idx++)
	{
		result.emplace_back(RX_NOT_VALID_TYPE);
	}
	return result;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor, api::rx_context ctx)
{
	return RX_NOT_VALID_TYPE;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor)
{
    return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
string_type rx_meta_item_implementation<TImpl>::get_definition_as_json () const
{
	using algorithm_type = typename TImpl::pointee_type::algorithm_type;

	serialization::json_writer writer;
	writer.write_header(STREAMING_TYPE_TYPE, 0);

	algorithm_type::serialize_type(*impl_, writer, STREAMING_TYPE_TYPE);

	writer.write_footer();

	return writer.get_string();
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
rx_result rx_meta_item_implementation<TImpl>::serialize_value (base_meta_writer& stream, runtime_value_type type) const
{
    return RX_NOT_VALID_TYPE;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::deserialize_value (base_meta_reader& stream, runtime_value_type type)
{
    return RX_NOT_VALID_TYPE;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::save () const
{
	return rx_save_platform_item(*this);
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::read_struct (string_view_type path, read_struct_data data) const
{
	data.callback(RX_NOT_VALID_TYPE, data::runtime_values_data());
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::write_struct (string_view_type path, write_struct_data data)
{
	data.callback(RX_NOT_VALID_TYPE, std::vector<rx_result>());
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::execute_method (const string_type& path, data::runtime_values_data data, execute_method_callback_t callback)
{
	callback(RX_NOT_VALID_TYPE, data::runtime_values_data());
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::execute_item (runtime_transaction_id_t transaction_id, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor)
{
	return RX_NOT_VALID_TYPE;
}

template <class TImpl>
byte_string rx_meta_item_implementation<TImpl>::get_definition_as_bytes () const
{

	using algorithm_type = typename TImpl::pointee_type::algorithm_type;
	
	memory::std_buffer buff;
	serialization::std_buffer_writer writer(buff);
	algorithm_type::serialize_type(*impl_, writer, STREAMING_TYPE_MESSAGE);

	return writer.get_data();
}


// Parameterized Class rx_internal::internal_ns::rx_other_implementation 

template <class TImpl>
rx_other_implementation<TImpl>::rx_other_implementation (TImpl impl)
      : impl_(impl)
{
}


template <class TImpl>
rx_other_implementation<TImpl>::~rx_other_implementation()
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
void rx_other_implementation<TImpl>::read_value (const string_type& path, read_result_callback_t callback) const
{
	callback(RX_NOT_IMPLEMENTED, rx_value());
}

template <class TImpl>
void rx_other_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, write_result_callback_t callback)
{
	callback(RX_NOT_IMPLEMENTED);
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::do_command (rx_object_command_t command_type)
{
	return RX_NOT_IMPLEMENTED;
}

template <class TImpl>
void rx_other_implementation<TImpl>::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback)
{
	callback(RX_NOT_VALID_TYPE, std::vector<runtime_item_attribute>());
}

template <class TImpl>
std::vector<rx_result_with<runtime_handle_t> > rx_other_implementation<TImpl>::connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor)
{
    std::vector<rx_result_with<runtime_handle_t> > result;
    result.reserve(paths.size());
    for (size_t idx = 0; idx < paths.size(); idx++)
    {
        result.emplace_back(RX_NOT_VALID_TYPE);
    }
    return result;
}

template <class TImpl>
std::vector<rx_result> rx_other_implementation<TImpl>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor)
{
	std::vector<rx_result> result;
	result.reserve(items.size());
	for (size_t idx = 0; idx < items.size(); idx++)
	{
		result.emplace_back(RX_NOT_VALID_TYPE);
	}
	return result;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor, api::rx_context ctx)
{
	return RX_NOT_VALID_TYPE;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor)
{
    return RX_NOT_IMPLEMENTED;
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
rx_result rx_other_implementation<TImpl>::serialize_value (base_meta_writer& stream, runtime_value_type type) const
{
    return RX_NOT_VALID_TYPE;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::deserialize_value (base_meta_reader& stream, runtime_value_type type)
{
    return RX_NOT_VALID_TYPE;
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::save () const
{
	return rx_save_platform_item(*this);
}

template <class TImpl>
void rx_other_implementation<TImpl>::read_struct (string_view_type path, read_struct_data data) const
{
	data.callback(RX_NOT_VALID_TYPE, data::runtime_values_data());
}

template <class TImpl>
void rx_other_implementation<TImpl>::write_struct (string_view_type path, write_struct_data data)
{
	data.callback(RX_NOT_VALID_TYPE, std::vector<rx_result>());
}

template <class TImpl>
void rx_other_implementation<TImpl>::execute_method (const string_type& path, data::runtime_values_data data, execute_method_callback_t callback)
{
	callback(RX_NOT_VALID_TYPE, data::runtime_values_data());
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::execute_item (runtime_transaction_id_t transaction_id, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor)
{
	return RX_NOT_VALID_TYPE;
}

template <class TImpl>
byte_string rx_other_implementation<TImpl>::get_definition_as_bytes () const
{
	return byte_string();
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
