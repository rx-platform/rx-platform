

/****************************************************************************
*
*  system\logic\rx_logic.cpp
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


// rx_logic
#include "system/logic/rx_logic.h"

#include "rx_configuration.h"
#include "sys_internal/rx_internal_ns.h"



extern "C"
{

	void c_method_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
	{
		if (whose != nullptr)
		{
			rx_platform::logic::extern_method_runtime* self = (rx_platform::logic::extern_method_runtime*)whose;
			self->start_timer(timer, period);
		}
	}
	void c_method_suspend_timer(void* whose, runtime_handle_t timer)
	{
		if (whose != nullptr)
		{
			rx_platform::logic::extern_method_runtime* self = (rx_platform::logic::extern_method_runtime*)whose;
			self->suspend_timer(timer);
		}
	}
	void c_method_destroy_timer(void* whose, runtime_handle_t timer)
	{
		if (whose != nullptr)
		{
			rx_platform::logic::extern_method_runtime* self = (rx_platform::logic::extern_method_runtime*)whose;
			self->destroy_timer(timer);
		}
	}

	void c_method_result(void* whose, rx_result_struct result, runtime_transaction_id_t id, struct typed_value_type out_val)
	{
		if (whose != nullptr)
		{
			rx_platform::logic::extern_method_runtime* self = (rx_platform::logic::extern_method_runtime*)whose;
			self->method_result(result, id, std::move(out_val));
		}
	}

	host_method_def_struct _g_method_def_
	{
		{
			nullptr
			, nullptr
			, c_method_start_timer
			, c_method_suspend_timer
			, c_method_destroy_timer
		},
		c_method_result
	};
}


namespace rx_platform {

namespace logic {

// Class rx_platform::logic::program_runtime

program_runtime::program_runtime()
{
}

program_runtime::program_runtime (const string_type& name, const rx_node_id& id)
{
	name_ = name;
}


program_runtime::~program_runtime()
{
}



rx_result program_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	return true;
}

rx_result program_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{
	return true;
}

rx_result program_runtime::save_program (base_meta_writer& stream, uint8_t type) const
{
	return serialize(stream, type);
}

bool program_runtime::load_program (base_meta_reader& stream, uint8_t type)
{
	return deserialize(stream, type);
}

rx_result program_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result program_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result program_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result program_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}


// Class rx_platform::logic::method_runtime

string_type method_runtime::type_name = RX_CPP_METHOD_TYPE_NAME;

method_runtime::method_runtime()
{
}

method_runtime::method_runtime (const string_type& name, const rx_node_id& id)
{
}

method_runtime::method_runtime (lock_reference_struct* extern_data)
	: reference_object(extern_data)
{
}


method_runtime::~method_runtime()
{
}



rx_result method_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result method_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result method_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result method_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_result method_runtime::execute (execute_data data, runtime::runtime_process_context* ctx)
{
	return "Abstract method execution!";
}

void method_runtime::execute_result_received (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id)
{
	if (container_)
	{
		container_->execution_complete(id, std::move(result), std::move(out_val));
	}
}

data::runtime_data_model method_runtime::get_method_inputs ()
{
	if (container_)
	{
		return container_->get_method_inputs();
	}
	else
	{
		return data::runtime_data_model();
	}
}

data::runtime_data_model method_runtime::get_method_outputs ()
{
	if (container_)
	{
		return container_->get_method_outputs();
	}
	else
	{
		return data::runtime_data_model();
	}
}


// Class rx_platform::logic::method_execution_context1

method_execution_context1::method_execution_context1 (runtime::execute_data data, security::security_guard_ptr guard)
      : data_(data),
        context_(nullptr),
        method_data_(nullptr),
        security_guard_(guard)
{
}


method_execution_context1::~method_execution_context1()
{
}



void method_execution_context1::execution_complete (rx_result result, values::rx_simple_value data)
{
	method_execute_result_data result_data;
	result_data.result = std::move(result);
	result_data.transaction_id = data_.transaction_id;
	result_data.data = std::move(data);
	result_data.whose = method_data_;
	context_->method_result_pending(std::move(result_data));
}

void method_execution_context1::execution_complete (rx_result result)
{
	execution_complete(std::move(result), values::rx_simple_value());
}

void method_execution_context1::execution_complete (values::rx_simple_value data)
{
	execution_complete(true, std::move(data));
}

security::security_guard_ptr method_execution_context1::get_security_guard ()
{
	return security_guard_;
}


// Class rx_platform::logic::ladder_program

ladder_program::ladder_program()
{
}

ladder_program::ladder_program (const string_type& name, const rx_node_id& id)
	: program_runtime(name,id)
{
}


ladder_program::~ladder_program()
{
}



rx_result ladder_program::serialize (base_meta_writer& stream, uint8_t type) const
{
	auto result = program_runtime::serialize(stream, type);
	if (!result)
		return result;

	return true;
}

rx_result ladder_program::deserialize (base_meta_reader& stream, uint8_t type)
{
	auto result = program_runtime::deserialize(stream, type);
	if (!result)
		return result;

	return true;
}


// Class rx_platform::logic::program_context

program_context::program_context (program_context* parent, program_runtime_ptr runtime, security::security_guard_ptr guard)
      : runtime_(runtime),
        security_guard_(guard)
{
}



void program_context::init_scan ()
{
}

bool program_context::schedule_scan (uint32_t interval)
{
	if (parent_)
		return parent_->schedule_scan(interval);
	else
		return false;
}

security::security_guard_ptr program_context::get_security_guard ()
{
	return security_guard_;
}


// Class rx_platform::logic::extern_method_runtime

extern_method_runtime::extern_method_runtime (plugin_method_runtime_struct* impl)
      : impl_(impl)
	, method_runtime(&impl->anchor)
{
	impl_->host = this;
	impl_->host_def = &_g_method_def_;
}


extern_method_runtime::~extern_method_runtime()
{
}



rx_result extern_method_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return impl_->def->init_method(impl_->anchor.target, &ctx);
}

rx_result extern_method_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return impl_->def->deinit_method(impl_->anchor.target);
}

rx_result extern_method_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return impl_->def->start_method(impl_->anchor.target, &ctx);
}

rx_result extern_method_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return impl_->def->stop_method(impl_->anchor.target);
}

rx_result extern_method_runtime::execute (execute_data data, runtime::runtime_process_context* ctx)
{
	return impl_->def->execute_method(impl_->anchor.target, data.transaction_id, data.test ? 1 : 0, data.identity, data.value.move(), ctx);
}

void extern_method_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
	rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_method_runtime::suspend_timer (runtime_handle_t handle)
{
	rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_method_runtime::destroy_timer (runtime_handle_t handle)
{
	rx_platform::extern_timers::instance().destroy_timer(handle);
}

void extern_method_runtime::method_result (rx_result&& result, runtime_transaction_id_t id, rx_simple_value out_vals)
{
	this->execute_result_received(std::move(out_vals), std::move(result), id);
}


} // namespace logic
} // namespace rx_platform

