

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

namespace runtime {

namespace objects {
template<typename CT>
namespace_item_attributes create_attributes_from_creation_data(const object_creation_data& data)
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

// Class rx_platform::runtime::objects::user_object 

user_object::user_object()
{
}

user_object::user_object (object_creation_data&& data)
	: object_runtime(std::move(data))
{
	init_object();
}


user_object::~user_object()
{
}



// Class rx_platform::runtime::objects::server_object 

server_object::server_object (object_creation_data&& data)
	: object_runtime(std::move(data))
{
	init_object();
}


server_object::~server_object()
{
}



// Class rx_platform::runtime::objects::object_runtime 

rx_item_type object_runtime::type_id = rx_item_type::rx_object;

object_runtime::object_runtime()
      : change_time_(rx_time::now())
	, meta_info_(namespace_item_pull_access)
{
}

object_runtime::object_runtime (object_creation_data&& data)
      : change_time_(rx_time::now())
	, meta_info_(std::move(data.name), std::move(data.id), std::move(data.type_id), create_attributes_from_creation_data<object_creation_data>(data) | namespace_item_pull_access, std::move(data.path))
	, my_application_(std::move(data.application))
	, my_domain_(std::move(data.domain))
{
}


object_runtime::~object_runtime()
{
}



rx_value object_runtime::get_value (const string_type path) const
{
	if (path.empty())
	{
		rx_value ret;
		ret.assign_static(meta_info_.get_name(), get_modified_time());
		ret.adapt_quality_to_mode(mode_);
		return ret;
	}
	else
	{
		return item_->get_value({ mode_, get_modified_time(), smart_this() }, path);
	}
}

void object_runtime::turn_on ()
{
	if (mode_.turn_on())
		item_->object_state_changed({mode_, rx_time::now(), smart_this()});
}

void object_runtime::turn_off ()
{
	if (mode_.turn_off())
		item_->object_state_changed({ mode_, rx_time::now(), smart_this() });
}

void object_runtime::set_blocked ()
{
	if (mode_.set_blocked())
		item_->object_state_changed({ mode_, rx_time::now(), smart_this() });
}

void object_runtime::set_test ()
{
	if (mode_.set_test())
		item_->object_state_changed({ mode_, rx_time::now(), smart_this() });
}

values::rx_value object_runtime::get_value () const
{
	// this static object improves performance its, created only once and it is empty
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
	domain_id_ = my_application_->meta_info().get_id();
	return true;
}

bool object_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!meta_info_.serialize_meta_data(stream, type, get_type()))
		return false;

	if (!stream.start_object("def"))
		return false;

	if (!stream.start_object("runtime"))
		return false;	
	if (!stream.write_id("domain", domain_id_))
		return false;
	if (!stream.write_id("app", app_id_))
		return false;
	if (!stream.end_object())
		return false;
	
	data::runtime_values_data temp_data;
	item_->collect_data(temp_data);
	if (!stream.write_init_values("values", temp_data))
		return false;

	if (!stream.start_array("programs", programs_.size()))
		return false;
	for (const auto& one : programs_)
	{
		if (!one->save_program(stream, type))
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

bool object_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{

	if (!stream.start_object("def"))
		return false;

	if (!stream.start_object("runtime"))
		return false;
	if (!stream.read_id("domain", domain_id_))
		return false;
	if (!stream.read_id("app", app_id_))
		return false;
	if (!stream.end_object())
		return false;
	
	data::runtime_values_data temp_data;
	if (!stream.read_init_values("values", temp_data))
		return false;
	structure::init_context ctx;
	item_->fill_data(temp_data, ctx);

	if (!stream.start_array("programs"))
		return false;

	while (!stream.array_end())
	{
		logic::ladder_program::smart_ptr one(pointers::_create_new);
		if (!one->load_program(stream, type))
			return false;
		programs_.push_back(one);
	}

	if (!stream.end_object())
		return false;

	return true;
}

bool object_runtime::init_object ()
{
	return true;
}

platform_item_ptr object_runtime::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

rx_time object_runtime::get_created_time () const
{
	return meta_info_.get_created_time();
}

rx_time object_runtime::get_modified_time () const
{
	return meta_info_.get_modified_time();
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
	app_id_ = my_application_->meta_info().get_id();
	return true;
}

void object_runtime::collect_data (data::runtime_values_data& data) const
{
	item_->collect_data(data);
}

void object_runtime::fill_data (const data::runtime_values_data& data)
{
	auto ctx = structure::init_context::create_initialization_context(smart_this());
	item_->fill_data(data, ctx);
}

rx_result object_runtime::check_validity ()
{
	return true;
}

rx_item_type object_runtime::get_type () const
{
  return type_id;

}


const meta::meta_data& object_runtime::meta_info () const
{
  return meta_info_;
}


// Class rx_platform::runtime::objects::application_runtime 

rx_item_type application_runtime::type_id = rx_item_type::rx_application;

application_runtime::application_runtime()
{
	my_application_ = smart_this();
	app_id_ = meta_info().get_id();
}

application_runtime::application_runtime (application_creation_data&& data)
	: domain_runtime(domain_creation_data{ std::move(data.name), std::move(data.id), std::move(data.type_id), data.system, std::move(data.path), rx_application_ptr::null_ptr })
{
	my_application_ = smart_this();
	app_id_ = meta_info().get_id();
}


application_runtime::~application_runtime()
{
}



bool application_runtime::connect_application (rx_application_ptr&& app)
{
	return false;
}

bool application_runtime::connect_domain (rx_domain_ptr&& domain)
{
	return false;
}

platform_item_ptr application_runtime::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

rx_item_type application_runtime::get_type () const
{
  return type_id;

}


// Class rx_platform::runtime::objects::domain_runtime 

rx_item_type domain_runtime::type_id = rx_item_type::rx_domain;

domain_runtime::domain_runtime()
{
	my_domain_ = smart_this();
	domain_id_ = meta_info().get_id();
}

domain_runtime::domain_runtime (domain_creation_data&& data)
	: object_runtime(object_creation_data{ std::move(data.name),std::move(data.id), std::move(data.type_id), data.system, std::move(data.path), data.application,rx_domain_ptr::null_ptr })
{
	my_domain_ = smart_this();
	domain_id_ = meta_info().get_id();
}


domain_runtime::~domain_runtime()
{
}



rx_thread_handle_t domain_runtime::get_executer () const
{
	return executer_;
}

bool domain_runtime::connect_domain (rx_domain_ptr&& domain)
{
	return false;
}

platform_item_ptr domain_runtime::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

rx_item_type domain_runtime::get_type () const
{
  return type_id;

}


// Class rx_platform::runtime::objects::port_runtime 

rx_item_type port_runtime::type_id = rx_item_type::rx_port;

port_runtime::port_runtime()
{
}

port_runtime::port_runtime (port_creation_data&& data)
	: object_runtime(object_creation_data{ std::move(data.name), std::move(data.id), std::move(data.type_id), true, std::move(data.path), std::move(data.application), std::move(data.application) })// every port is system objects
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


// Class rx_platform::runtime::objects::application_creation_data 


// Class rx_platform::runtime::objects::domain_creation_data 


// Class rx_platform::runtime::objects::object_creation_data 


// Class rx_platform::runtime::objects::port_creation_data 


} // namespace objects
} // namespace runtime
} // namespace rx_platform

