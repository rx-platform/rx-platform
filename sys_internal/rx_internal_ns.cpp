

/****************************************************************************
*
*  sys_internal\rx_internal_ns.cpp
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


// rx_internal_ns
#include "sys_internal/rx_internal_ns.h"

#include "testing/rx_test.h"
#include "sys_internal/rx_internal_builders.h"


namespace sys_internal {

namespace internal_ns {

// Class sys_internal::internal_ns::platform_root 

rx_platform::ns::rx_names_cache platform_root::cache_;

platform_root::platform_root()
	: rx_platform_directory("", namespace_item_internal_access)
{
}


platform_root::~platform_root()
{
}



platform_item_ptr platform_root::get_cached_item (const string_type& name)
{
	return cache_.get_cached_item(name);
}

rx_result platform_root::insert_cached_item (const string_type& name, platform_item_ptr item)
{
	return cache_.insert_cached_item(name, item);
}


// Class sys_internal::internal_ns::user_directory 

user_directory::user_directory (const string_type& name)
	: rx_platform_directory(name, namespace_item_read_access | namespace_item_write_access | namespace_item_delete_access)
{
}


user_directory::~user_directory()
{
}



// Class sys_internal::internal_ns::unassigned_directory 

unassigned_directory::unassigned_directory()
	: rx_platform_directory(RX_NS_UNASSIGNED_NAME, namespace_item_internal_access)
{
}


unassigned_directory::~unassigned_directory()
{
}



// Class sys_internal::internal_ns::world_directory 

world_directory::world_directory()
	: rx_platform_directory(RX_NS_WORLD_NAME, namespace_item_internal_access)
{
}


world_directory::~world_directory()
{
}



// Parameterized Class sys_internal::internal_ns::rx_item_implementation 

template <class TImpl>
rx_item_implementation<TImpl>::rx_item_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
void rx_item_implementation<TImpl>::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
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
	return impl_->get_value();
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	impl_->serialize(writer, STREAMING_TYPE_OBJECT);

	writer.write_footer();

	string_type result;
	bool out = writer.get_string(result, true);

	if (out)
		def << result;
	else
		def << "\r\n" ANSI_COLOR_RED "Error in JSON deserialization." ANSI_COLOR_RESET "\r\n";

	return true;
}

template <class TImpl>
string_type rx_item_implementation<TImpl>::get_name () const
{
	return impl_->get_name();
}

template <class TImpl>
size_t rx_item_implementation<TImpl>::get_size () const
{
	return impl_->get_size();
}

template <class TImpl>
rx_node_id rx_item_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_info().get_id();
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::save () const
{
	const auto& meta = impl_->meta_info();
	auto storage_result = meta.storage_info.resolve_storage();
	if (storage_result)
	{
		auto result = storage_result.value()->save_item(impl_->get_item_ptr());
		if (!result)
			result.register_error("Error saving item "s + meta.get_path());
		return result;
	}
	else // !storage_result
	{
		rx_result result(storage_result.errors());
		result.register_error("Error saving item "s + meta.get_path());
		return result;
	}
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
rx_result rx_item_implementation<TImpl>::deserialize (base_meta_reader& stream)
{
	int type;
	auto ret = stream.read_header(type);
	if (ret)
	{
		ret = impl_->deserialize(stream, STREAMING_TYPE_OBJECT);
		if (ret)
			stream.read_footer();
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
	impl_->fill_code_info(info, name);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::read_value (const string_type& path, rx_value& val) const
{
	return impl_->read_value(path, val);
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return impl_->write_value(path, std::move(val), std::move(callback), ctx);
}


// Parameterized Class sys_internal::internal_ns::rx_meta_item_implementation 

template <class TImpl>
rx_meta_item_implementation<TImpl>::rx_meta_item_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
void rx_meta_item_implementation<TImpl>::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

template <class TImpl>
rx_item_type rx_meta_item_implementation<TImpl>::get_type_id () const
{
	return impl_->get_type_id();
}

template <class TImpl>
values::rx_value rx_meta_item_implementation<TImpl>::get_value () const
{
	return impl_->meta_info().get_value();
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_TYPE, 0);

	bool out = false;

	out = impl_->serialize_definition(writer, STREAMING_TYPE_TYPE);

	writer.write_footer();

	string_type result;
	if (out)
	{
		out = writer.get_string(result, true);
	}

	if (out)
		def << result;
	else
		def << "\r\n" ANSI_COLOR_RED "Error in JSON deserialization." ANSI_COLOR_RESET "\r\n";

	return out;
}

template <class TImpl>
string_type rx_meta_item_implementation<TImpl>::get_name () const
{
	return impl_->meta_info().get_name();
}

template <class TImpl>
size_t rx_meta_item_implementation<TImpl>::get_size () const
{
	return sizeof(*this) + sizeof(TImpl);
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::fill_code_info (std::ostream& info, const string_type& name)
{
	impl_->fill_code_info(info, name);
}

template <class TImpl>
rx_node_id rx_meta_item_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_info().get_id();
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::save () const
{
	const auto& meta = impl_->meta_info();
	auto storage_result = meta.storage_info.resolve_storage();
	if (storage_result)
	{
		auto result = storage_result.value()->save_item(impl_->get_item_ptr());
		if (!result)
			result.register_error("Error saving type item "s + meta.get_path());
		return result;
	}
	else // !storage_result
	{
		rx_result result(storage_result.errors());
		result.register_error("Error saving type item "s + meta.get_path());
		return result;
	}
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::serialize (base_meta_writer& stream) const
{
	stream.write_header(STREAMING_TYPE_TYPE, 0);
	auto ret = impl_->serialize_definition(stream, STREAMING_TYPE_TYPE);
	stream.write_footer();
	return ret;
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::deserialize (base_meta_reader& stream)
{
	return false;
}

template <class TImpl>
const meta_data_t& rx_meta_item_implementation<TImpl>::meta_info () const
{
	return impl_->meta_info();
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::read_value (const string_type& path, rx_value& val) const
{
	return "Not Implemented!";
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return "Not Implemented!";
}


// Class sys_internal::internal_ns::internal_directory 

internal_directory::internal_directory (const string_type& name)
	: rx_platform_directory(name, namespace_item_internal_access)
{
}


internal_directory::~internal_directory()
{
}



// Parameterized Class sys_internal::internal_ns::rx_other_implementation 

template <class TImpl>
rx_other_implementation<TImpl>::rx_other_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
void rx_other_implementation<TImpl>::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
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
rx_result rx_other_implementation<TImpl>::generate_json (std::ostream& def, std::ostream& err) const
{
	return "Not valid for this item";
}

template <class TImpl>
string_type rx_other_implementation<TImpl>::get_name () const
{
	return impl_->get_name();
}

template <class TImpl>
size_t rx_other_implementation<TImpl>::get_size () const
{
	return impl_->get_size();
}

template <class TImpl>
void rx_other_implementation<TImpl>::fill_code_info (std::ostream& info, const string_type& name)
{
	impl_->fill_code_info(info, name);
}

template <class TImpl>
rx_node_id rx_other_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_info().get_id();
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::save () const
{
	return "Not valid for this item";
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::serialize (base_meta_writer& stream) const
{
	return "Not valid for this item";
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::deserialize (base_meta_reader& stream)
{
	return "Not valid for this item";
}

template <class TImpl>
const meta_data_t& rx_other_implementation<TImpl>::meta_info () const
{
	return impl_->meta_info();
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::read_value (const string_type& path, rx_value& val) const
{
	return "Not Implemented!";
}

template <class TImpl>
rx_result rx_other_implementation<TImpl>::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return "Not Implemented!";
}


} // namespace internal_ns
} // namespace sys_internal

template class sys_internal::internal_ns::rx_item_implementation<rx_domain_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<rx_application_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<rx_object_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<rx_port_ptr>;
template class sys_internal::internal_ns::rx_other_implementation<testing::test_case::smart_ptr>;
template class sys_internal::internal_ns::rx_other_implementation<prog::command_ptr>;
template class sys_internal::internal_ns::rx_other_implementation<program_runtime_ptr>;

template class sys_internal::internal_ns::rx_meta_item_implementation<meta::application_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::domain_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::port_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::object_type_ptr>;

template class sys_internal::internal_ns::rx_meta_item_implementation<meta::struct_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::mapper_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::variable_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::event_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::filter_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::source_type_ptr>;
