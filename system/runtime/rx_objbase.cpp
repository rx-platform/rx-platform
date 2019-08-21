

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
#include "runtime_internal/rx_runtime_internal.h"
#include "system/server/rx_async_functions.h"


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

// Class rx_platform::runtime::objects::object_runtime

rx_item_type object_runtime::type_id = rx_item_type::rx_object;

object_runtime::object_runtime()
      : job_pending_(false)
	, meta_info_(namespace_item_pull_access)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_object_ptr> >(smart_this());
}

object_runtime::object_runtime (const meta::meta_data& meta, const object_instance_data& instance)
      : job_pending_(false)
	, meta_info_(meta)
	, instance_data_(instance)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_object_ptr> >(smart_this());
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
	if (my_domain_)
	{
		auto temp = my_domain_;
		my_domain_ = rx_domain_ptr::null_ptr;
		locks::const_auto_lock_t<decltype(temp->objects_lock_)> _(&temp->objects_lock_);
		temp->objects_.erase(meta_info_.get_id());
	}
	if (domain)
	{
		my_domain_ = std::move(domain);
		locks::const_auto_lock_t<decltype(my_domain_->objects_lock_)> _(&my_domain_->objects_lock_);
		my_domain_->objects_.emplace(meta_info_.get_id(), smart_this());
	}
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

rx_result object_runtime::check_validity ()
{
	return true;
}

rx_item_type object_runtime::get_type () const
{
  return type_id;

}

rx_result object_runtime::read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const
{
	if (path.empty())
	{// our value
		auto val = get_value();
		callback(val);
		return true;
	}
	else
	{
		return runtime_.read_value(path, callback, ctx, get_executer());
	}
}

rx_result object_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, get_executer());
}

rx_result object_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = runtime_.initialize_runtime(ctx);
	return result;
}

rx_result object_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = runtime_.deinitialize_runtime(ctx);
	return result;
}

rx_result object_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = runtime_.start_runtime(ctx);
	return result;
}

rx_result object_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = runtime_.stop_runtime(ctx);
	return result;
}

rx_result object_runtime::do_command (rx_object_command_t command_type)
{
	return runtime_.do_command(command_type);
}

void object_runtime::process_runtime ()
{
	object_runtime_algorithms<meta::object_types::object_type>::process_runtime(this);
}

rx_result object_runtime::browse (const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	return runtime_.browse(path, filter, items);
}

rx_result object_runtime::connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::object_type>::connect_items(paths, callback, monitor, ctx, this);
}

void object_runtime::fire_job ()
{
	object_runtime_algorithms<meta::object_types::object_type>::fire_job(this);
}

rx_result object_runtime::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::object_type>::read_items(items, monitor, ctx, this);
}


// Class rx_platform::runtime::objects::application_runtime

rx_item_type application_runtime::type_id = rx_item_type::rx_application;

application_runtime::application_runtime()
      : job_pending_(false)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_application_ptr> >(smart_this());
}

application_runtime::application_runtime (const meta::meta_data& meta, const application_instance_data& instance)
      : job_pending_(false)
	, meta_info_(meta)
	, instance_data_(instance)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_application_ptr> >(smart_this());
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

rx_result application_runtime::read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const
{
	if (path.empty())
	{// our value
		auto val = get_value();
		callback(val);
		return true;
	}
	else
	{
		return runtime_.read_value(path, callback, ctx, get_executer());
	}
}

rx_result application_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, get_executer());
}

values::rx_value application_runtime::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_info_.get_version(), meta_info_.get_modified_time());
	return temp;
}

void application_runtime::fill_data (const data::runtime_values_data& data)
{
	runtime_.fill_data(data);
}

void application_runtime::collect_data (data::runtime_values_data& data) const
{
	runtime_.collect_data(data);
}

rx_result application_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = runtime_.initialize_runtime(ctx);
	if (result)
	{
		executer_ = sys_runtime::platform_runtime_manager::instance().resolve_app_processor(instance_data_);
		auto hndl = ctx.tags->bind_item("CPU", ctx);
		if (hndl)
		{
			ctx.structure.get_root()->set_binded_as<int>(hndl.value(), (int)executer_);
		}
	}
	return result;
}

rx_result application_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	sys_runtime::platform_runtime_manager::instance().remove_one(executer_);
	auto result = runtime_.deinitialize_runtime(ctx);
	return result;
}

rx_result application_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = runtime_.start_runtime(ctx);
	return result;
}

rx_result application_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = runtime_.stop_runtime(ctx);
	return result;
}

rx_result application_runtime::do_command (rx_object_command_t command_type)
{
	return runtime_.do_command(command_type);
}

void application_runtime::process_runtime ()
{
	object_runtime_algorithms<meta::object_types::application_type>::process_runtime(this);
}

void application_runtime::get_ports (api::query_result& result)
{
	result.items.reserve(ports_.size());
	for (const auto& one : ports_)
	{
		if(one)
			result.items.emplace_back(api::query_result_detail{ rx_port, one->meta_info() });
	}
}

void application_runtime::add_port (rx_port_ptr what)
{
	for (auto& one : ports_)
	{
		if (!one)
		{
			one = what;
			return;
		}
	}
	ports_.emplace_back(what);
}

void application_runtime::add_domain (rx_domain_ptr what)
{
	for (auto& one : domains_)
	{
		if (!one)
		{
			one = what;
			return;
		}
	}
	domains_.emplace_back(what);
}

void application_runtime::remove_port (rx_port_ptr what)
{
	for (auto& one : ports_)
	{
		if (one == what)
		{
			one = rx_port_ptr::null_ptr;
			return;
		}
	}
}

void application_runtime::remove_domain (rx_domain_ptr what)
{
	for (auto& one : domains_)
	{
		if (one == what)
		{
			one = rx_domain_ptr::null_ptr;
			return;
		}
	}
}

void application_runtime::get_domains (api::query_result& result)
{
	result.items.reserve(domains_.size());
	for (const auto& one : domains_)
	{
		if (one)
			result.items.emplace_back(api::query_result_detail{ rx_domain, one->meta_info() });
	}
}

rx_result application_runtime::browse (const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	return runtime_.browse(path, filter, items);
}

rx_result application_runtime::connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::application_type>::connect_items(paths, callback, monitor, ctx, this);
}

void application_runtime::fire_job ()
{
	object_runtime_algorithms<meta::object_types::application_type>::fire_job(this);
}

rx_result application_runtime::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::application_type>::read_items(items, monitor, ctx, this);
}


// Class rx_platform::runtime::objects::domain_runtime

rx_item_type domain_runtime::type_id = rx_item_type::rx_domain;

domain_runtime::domain_runtime()
      : job_pending_(false)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_domain_ptr> >(smart_this());
}

domain_runtime::domain_runtime (const meta::meta_data& meta, const domain_instance_data& instance)
      : job_pending_(false)
	, meta_info_(meta)
	, instance_data_(instance)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_domain_ptr> >(smart_this());
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
	my_application_->add_domain(smart_this());
	return true;
}

rx_result domain_runtime::check_validity ()
{
	return true;
}

rx_result domain_runtime::read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const
{
	if (path.empty())
	{// our value
		auto val = get_value();
		callback(val);
		return true;
	}
	else
	{
		return runtime_.read_value(path, callback, ctx, get_executer());
	}
}

rx_result domain_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, get_executer());
}

rx_result domain_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = runtime_.initialize_runtime(ctx);
	if (result)
	{
		executer_ = sys_runtime::platform_runtime_manager::instance().resolve_domain_processor(instance_data_);
		auto hndl = ctx.tags->bind_item("CPU", ctx);
		if (hndl)
		{
			ctx.structure.get_root()->set_binded_as<int>(hndl.value(), (int)executer_);
		}
	}
	return result;
}

rx_result domain_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = runtime_.deinitialize_runtime(ctx);
	if (my_application_)
		my_application_->remove_domain(smart_this());
	return result;
}

rx_result domain_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = runtime_.start_runtime(ctx);
	return result;
}

rx_result domain_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = runtime_.stop_runtime(ctx);
	return result;
}

rx_result domain_runtime::do_command (rx_object_command_t command_type)
{
	return runtime_.do_command(command_type);
}

void domain_runtime::process_runtime ()
{
	object_runtime_algorithms<meta::object_types::domain_type>::process_runtime(this);
}

void domain_runtime::get_objects (api::query_result& result)
{
	result.items.reserve(objects_.size());
	for (const auto& one : objects_)
	{
		if (one.second)
			result.items.emplace_back(api::query_result_detail{ rx_object, one.second->meta_info() });
	}
}

void domain_runtime::add_object (rx_object_ptr what)
{
	objects_.emplace(what->meta_info().get_id(), what);
}

void domain_runtime::remove_object (rx_object_ptr what)
{
	auto it = objects_.find(what->meta_info().get_id());
	if (it != objects_.end())
		objects_.erase(it);
}

rx_result domain_runtime::browse (const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	return runtime_.browse(path, filter, items);
}

rx_result domain_runtime::connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::domain_type>::connect_items(paths, callback, monitor, ctx, this);
}

void domain_runtime::fire_job ()
{
	object_runtime_algorithms<meta::object_types::domain_type>::fire_job(this);
}

rx_result domain_runtime::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::domain_type>::read_items(items, monitor, ctx, this);
}


// Class rx_platform::runtime::objects::port_runtime

rx_item_type port_runtime::type_id = rx_item_type::rx_port;

port_runtime::port_runtime()
      : rx_packets_item_(0),
        tx_packets_item_(0),
        job_pending_(false)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_port_ptr> >(smart_this());
}

port_runtime::port_runtime (const meta::meta_data& meta, const port_instance_data& instance)
      : rx_packets_item_(0),
        tx_packets_item_(0),
        job_pending_(false)
	, meta_info_(meta)
	, instance_data_(instance)
{
	my_job_ptr_ = rx_create_reference<process_runtime_job<rx_port_ptr> >(smart_this());
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

rx_result port_runtime::read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const
{
	if (path.empty())
	{// our value
		auto val = get_value();
		callback(val);
		return true;
	}
	else
	{
		return runtime_.read_value(path, callback, ctx, get_executer());
	}
}

rx_result port_runtime::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return runtime_.write_value(path, std::move(val), callback, ctx, get_executer());
}

bool port_runtime::connect_application (rx_application_ptr&& app)
{
	my_application_ = std::move(app);
	my_application_->add_port(smart_this());
	return true;
}

rx_result port_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = runtime_.initialize_runtime(ctx);
	if (result)
	{
		auto bind_result = ctx.tags->bind_item("Status.RxPackets", ctx);
		if (bind_result)
			rx_packets_item_ = bind_result.value();
		else
			RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

		bind_result = ctx.tags->bind_item("Status.TxPackets", ctx);
		if (bind_result)
			tx_packets_item_ = bind_result.value();
		else
			RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");
	}
	return result;
}

rx_result port_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = runtime_.deinitialize_runtime(ctx);
	if (my_application_)
		my_application_->remove_port(smart_this());
	return result;
}

rx_result port_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = runtime_.start_runtime(ctx);
	return result;
}

rx_result port_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = runtime_.stop_runtime(ctx);
	return result;
}

rx_result port_runtime::do_command (rx_object_command_t command_type)
{
	return runtime_.do_command(command_type);
}

void port_runtime::update_received_packets (size_t count)
{
	if(rx_packets_item_)
	{
		auto current = runtime_.get_binded_as<int64_t>(rx_packets_item_, 0);
		current += count;
		runtime_.set_binded_as<int64_t>(rx_packets_item_, std::move(current));
	}
}

void port_runtime::process_runtime ()
{
	object_runtime_algorithms<meta::object_types::port_type>::process_runtime(this);
}

rx_result port_runtime::browse (const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	return runtime_.browse(path, filter, items);
}

rx_result port_runtime::connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::port_type>::connect_items(paths, callback, monitor, ctx, this);
}

void port_runtime::fire_job ()
{
	object_runtime_algorithms<meta::object_types::port_type>::fire_job(this);
}

rx_result port_runtime::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx)
{
	return object_runtime_algorithms<meta::object_types::port_type>::read_items(items, monitor, ctx, this);
}


// Class rx_platform::runtime::objects::object_instance_data


bool object_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object("instance"))
		return false;
	if (!stream.write_id("domain", domain_id))
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

	if (!stream.start_array("objects", objects.size()))
		return false;
	for (const auto& one : objects)
	{
		if (!stream.write_id("id", one))
			return false;
	}
	if (!stream.end_array())
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

	if (!stream.start_array("objects"))
		return false;
	while(!stream.array_end())
	{
		rx_node_id one;
		if (!stream.read_id("id", one))
			return false;
	}
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


// Parameterized Class rx_platform::runtime::objects::object_runtime_algorithms


template <class typeT>
rx_result object_runtime_algorithms<typeT>::connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx, typename typeT::RType* whose)
{
	using connect_result_t = std::vector<rx_result_with<runtime_handle_t> >;
	using smart_ptr = typename typeT::RTypePtr;

	std::function<connect_result_t(string_array, operational::tags_callback_ptr, smart_ptr)> func = [](string_array paths, operational::tags_callback_ptr monitor, smart_ptr whose)
	{
		connect_result_t results;
		bool has_errors = false;
		auto ret = whose->runtime_.connect_items(paths, monitor, results, has_errors);
		if (ret)
		{
			whose->runtime_context_.process_tag_connections = true;
		}
		else
		{
			auto size = paths.size();
			results.reserve(size);
			for (size_t i = 0; i < size; i++)
				results.emplace_back(ret.errors());
		}
		if (whose->runtime_context_.process_tag_connections)
		{
			whose->fire_job();
		}
		return results;
	};
	auto ret_thread = whose->get_executer();
	rx_do_with_callback<connect_result_t, decltype(ctx.object), string_array, operational::tags_callback_ptr, smart_ptr>(func, ret_thread, callback, ctx.object, paths, monitor, whose->smart_this());
	return true;
}

template <class typeT>
void object_runtime_algorithms<typeT>::fire_job (typename typeT::RType* whose)
{
	locks::auto_lock_t<decltype(whose->job_lock_)> _(&whose->job_lock_);
	if (!whose->job_pending_)
	{
		whose->job_pending_ = true;
		RX_ASSERT(whose->my_job_ptr_);
		auto executer = rx_gate::instance().get_infrastructure().get_executer(whose->get_executer());
		executer->append(whose->my_job_ptr_);
	}
}

template <class typeT>
void object_runtime_algorithms<typeT>::process_runtime (typename typeT::RType* whose)
{
	whose->job_lock_.lock();
	whose->job_pending_ = false;
	whose->job_lock_.unlock();
	whose->runtime_.process_runtime(whose->runtime_context_);
}

template <class typeT>
rx_result object_runtime_algorithms<typeT>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx, typename typeT::RType* whose)
{
	using smart_ptr = typename typeT::RTypePtr;

	std::function<void(const std::vector<runtime_handle_t>&, runtime::operational::tags_callback_ptr, smart_ptr)> func = [](const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, smart_ptr whose)
	{
		auto ret = whose->runtime_.read_items(items, monitor);
		if (ret)
		{
			whose->runtime_context_.process_tag_connections = true;
		}
		if (whose->runtime_context_.process_tag_connections)
		{
			whose->fire_job();
		}
		return ret;
	};
	auto ret_thread = whose->get_executer();
	rx_post_function_to<decltype(ctx.object), const std::vector<runtime_handle_t>&, runtime::operational::tags_callback_ptr, smart_ptr>(ret_thread, func, ctx.object, items, monitor, whose->smart_this());
	return true;
}


// Parameterized Class rx_platform::runtime::objects::process_runtime_job

template <class typePtr>
process_runtime_job<typePtr>::process_runtime_job (typePtr whose)
      : whose_(whose)
{
}



template <class typePtr>
void process_runtime_job<typePtr>::process ()
{
	whose_->process_runtime();
}


} // namespace objects
} // namespace runtime
} // namespace rx_platform

