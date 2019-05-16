

/****************************************************************************
*
*  system\runtime\rx_objbase.cpp
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


// rx_objbase
#include "system/runtime/rx_objbase.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_internal_ns.h"
#include "lib/rx_ser_lib.h"
#include "system/serialization/rx_ser.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {
void split_item_path(const string_type& full_path, string_type& object_path, string_type& item_path)
{
	auto idx = full_path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// no directory stuff
		idx = full_path.find(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{// just plain object name
			object_path = full_path;
		}
		else
		{
			object_path = full_path.substr(0, idx);
			item_path = full_path.substr(idx + 1);
		}
	}
	else
	{// we have directory stuff
		idx = full_path.find(RX_OBJECT_DELIMETER, idx + 1);
		if (idx == string_type::npos)
		{// just object path
			object_path = full_path;
		}
		else
		{
			object_path = full_path.substr(0, idx);
			item_path = full_path.substr(idx + 1);
		}
	}
}

namespace runtime {

namespace objects {
template<typename CT>
namespace_item_attributes create_attributes_from_creation_data(const CT& data)
{
	if (data.system)
	{
		return namespace_item_system_access;
	}
	else
	{
		return namespace_item_full_access;
	}
}

// Class rx_platform::runtime::objects::application_runtime 

rx_item_type application_runtime::type_id = rx_item_type::rx_application;

application_runtime::application_runtime()
{
}

application_runtime::application_runtime (const meta::meta_data& meta, const application_instance_data& instance)
	: meta_info_(meta)
	, instance_data_(instance)
{
}


application_runtime::~application_runtime()
{
}



platform_item_ptr application_runtime::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

rx_item_type application_runtime::get_type () const
{
  return type_id;

}

meta::meta_data& application_runtime::meta_info ()
{
  return meta_info_;

}

bool application_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_info_.serialize_meta_data(stream, type, get_type()))
		return false;

	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.serialize(stream, type))
		return false;
	
	if (!runtime_.serialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

bool application_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.deserialize(stream, type))
		return false;

	if (!runtime_.deserialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

string_type application_runtime::get_name () const
{
	return meta_info_.get_name();
}

size_t application_runtime::get_size () const
{
	return sizeof(*this);
}

rx_thread_handle_t application_runtime::get_executer () const
{
	return executer_;
}

rx_result application_runtime::check_validity ()
{
	return true;
}

rx_result application_runtime::initialize_object ()
{
	return true;
}

rx_result application_runtime::read_value (const string_type& path, rx_value& val) const
{
	if (path.empty())
	{// our value
		val = get_value();
		return true;
	}
	else
	{
		return runtime_.read_value(path, val, *this);
	}
}

rx_result application_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, *this);
}

values::rx_value application_runtime::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_info_.get_version(), meta_info_.get_modified_time());
	return temp;
}

void application_runtime::fill_data (const data::runtime_values_data& data)
{
  return runtime_.runtime.fill_data(data);

}

void application_runtime::collect_data (data::runtime_values_data& data) const
{
  return runtime_.runtime.collect_data(data);

}


// Class rx_platform::runtime::objects::domain_runtime 

rx_item_type domain_runtime::type_id = rx_item_type::rx_domain;

domain_runtime::domain_runtime()
{
}

domain_runtime::domain_runtime (const meta::meta_data& meta, const domain_instance_data& instance)
	: meta_info_(meta)
	, instance_data_(instance)
{
}


domain_runtime::~domain_runtime()
{
}



rx_thread_handle_t domain_runtime::get_executer () const
{
	return executer_;
}

platform_item_ptr domain_runtime::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

rx_item_type domain_runtime::get_type () const
{
  return type_id;

}

meta::meta_data& domain_runtime::meta_info ()
{
  return meta_info_;

}

values::rx_value domain_runtime::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_info_.get_version(), meta_info_.get_modified_time());
	return temp;
}

bool domain_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_info_.serialize_meta_data(stream, type, get_type()))
		return false;

	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.serialize(stream, type))
		return false;

	if (!runtime_.serialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

bool domain_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{

	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.deserialize(stream, type))
		return false;

	if (!runtime_.deserialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

string_type domain_runtime::get_name () const
{
	return meta_info_.get_name();
}

size_t domain_runtime::get_size () const
{
	return sizeof(*this);
}

bool domain_runtime::connect_application (rx_application_ptr&& app)
{
	my_application_ = std::move(app);
	instance_data_.app_id = my_application_->meta_info().get_id();
	return true;
}

rx_result domain_runtime::check_validity ()
{
	return true;
}

rx_result domain_runtime::initialize_object ()
{
	return true;
}

rx_result domain_runtime::read_value (const string_type& path, rx_value& val) const
{
	if (path.empty())
	{// our value
		val = get_value();
		return true;
	}
	else
	{
		return runtime_.read_value(path, val, *this);
	}
}

rx_result domain_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, *this);
}


// Class rx_platform::runtime::objects::port_runtime 

rx_item_type port_runtime::type_id = rx_item_type::rx_port;

port_runtime::port_runtime()
{
}

port_runtime::port_runtime (const meta::meta_data& meta, const port_instance_data& instance)
	: meta_info_(meta)
	, instance_data_(instance)
{
}


port_runtime::~port_runtime()
{
}



bool port_runtime::write (buffer_ptr what)
{
	return false;
}

bool port_runtime::readed (buffer_ptr what, rx_thread_handle_t destination)
{
	return true;
}

platform_item_ptr port_runtime::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

rx_protocol_stack_entry* port_runtime::get_stack_entry ()
{
	RX_ASSERT(false);
	return nullptr;
}

rx_result_with<io_types::rx_io_buffer> port_runtime::allocate_io_buffer (size_t initial_capacity)
{
	io_types::rx_io_buffer ret;
	auto result = rx_init_packet_buffer(&ret, initial_capacity, get_stack_entry());
	if (result == RX_PROTOCOL_OK)
		return ret;
	else
		return rx_protocol_error_message(result);
}

rx_item_type port_runtime::get_type () const
{
  return type_id;

}

meta::meta_data& port_runtime::meta_info ()
{
  return meta_info_;

}

values::rx_value port_runtime::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_info_.get_version(), meta_info_.get_modified_time());
	return temp;
}

bool port_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_info_.serialize_meta_data(stream, type, get_type()))
		return false;

	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.serialize(stream, type))
		return false;
	
	if (!runtime_.serialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

bool port_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.deserialize(stream, type))
		return false;

	if (!runtime_.deserialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

string_type port_runtime::get_name () const
{
	return meta_info_.get_name();
}

size_t port_runtime::get_size () const
{
	return sizeof(*this);
}

rx_thread_handle_t port_runtime::get_executer () const
{
	if (my_application_)
		return my_application_->get_executer();
	else
		return RX_DOMAIN_GENERAL;
}

rx_result port_runtime::check_validity ()
{
	return true;
}

rx_result port_runtime::initialize_object ()
{
	return true;
}

rx_result port_runtime::read_value (const string_type& path, rx_value& val) const
{
	if (path.empty())
	{// our value
		val = get_value();
		return true;
	}
	else
	{
		return runtime_.read_value(path, val, *this);
	}
}

rx_result port_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, *this);
}

bool port_runtime::connect_application (rx_application_ptr&& app)
{
	my_application_ = std::move(app);
	instance_data_.app_id = my_application_->meta_info().get_id();
	return true;
}


// Class rx_platform::runtime::objects::object_instance_data 


bool object_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.write_id("domain", domain_id))
		return false;
	if (!stream.write_id("app", app_id))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool object_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.read_id("domain", domain_id))
		return false;
	if (!stream.read_id("app", app_id))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}


// Class rx_platform::runtime::objects::port_instance_data 


bool port_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.write_id("app", app_id))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool port_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.read_id("app", app_id))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}


// Class rx_platform::runtime::objects::application_instance_data 


bool application_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.write_int("processor", processor))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool application_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.read_int("processor", processor))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}


// Class rx_platform::runtime::objects::domain_instance_data 


bool domain_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.write_int("processor", processor))
		return false;
	if (!stream.write_id("app", app_id))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool domain_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.read_int("processor", processor))
		return false;
	if (!stream.read_id("app", app_id))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}


// Class rx_platform::runtime::objects::object_runtime 

rx_item_type object_runtime::type_id = rx_item_type::rx_object;

object_runtime::object_runtime()
	: meta_info_(namespace_item_pull_access)
{
}

object_runtime::object_runtime (const meta::meta_data& meta, const object_instance_data& instance)
	: meta_info_(meta)
	, instance_data_(instance)
{
}


object_runtime::~object_runtime()
{
}



values::rx_value object_runtime::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_info_.get_version(), meta_info_.get_modified_time());
	return temp;
}

void object_runtime::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

bool object_runtime::connect_domain (rx_domain_ptr&& domain)
{
	my_domain_ = std::move(domain);
	instance_data_.domain_id = my_domain_->meta_info().get_id();
	return true;
}

bool object_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_info_.serialize_meta_data(stream, type, get_type()))
		return false;

	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.serialize(stream, type))
		return false;

	if (!runtime_.serialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;
	
	return true;
}

bool object_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{

	if (!stream.start_object("def"))
		return false;

	if (!instance_data_.deserialize(stream, type))
		return false;

	if (!runtime_.deserialize(stream, type))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

platform_item_ptr object_runtime::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

string_type object_runtime::get_name () const
{
	return meta_info_.get_name();
}

size_t object_runtime::get_size () const
{
	return sizeof(*this);
}

meta::meta_data& object_runtime::meta_info ()
{
  return meta_info_;

}

rx_thread_handle_t object_runtime::get_executer () const
{
	if (my_domain_)
		return my_domain_->get_executer();
	else
		return RX_DOMAIN_GENERAL;
}

bool object_runtime::connect_application (rx_application_ptr&& app)
{
	my_application_ = std::move(app);
	instance_data_.app_id = my_application_->meta_info().get_id();
	return true;
}

rx_result object_runtime::check_validity ()
{
	return true;
}

rx_item_type object_runtime::get_type () const
{
  return type_id;

}

rx_result object_runtime::initialize_object ()
{
	return true;
}

rx_result object_runtime::read_value (const string_type& path, rx_value& val) const
{
	if (path.empty())
	{// our value
		val = get_value();
		return true;
	}
	else
	{
		return runtime_.read_value(path, val, *this);
	}
}

rx_result object_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, *this);
}


} // namespace objects
} // namespace runtime
} // namespace rx_platform

