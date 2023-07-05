

/****************************************************************************
*
*  system\logic\rx_logic.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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

method_runtime::method_runtime()
{
}

method_runtime::method_runtime (const string_type& name, const rx_node_id& id)
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

method_execution_context* method_runtime::create_execution_context (execute_data data, security::security_guard_ptr guard)
{
	return new method_execution_context(data, guard);
}

rx_result method_runtime::execute (data::runtime_values_data args, method_execution_context* context)
{
	return "Undefined method type execution!";
}


// Class rx_platform::logic::method_execution_context 

method_execution_context::method_execution_context (execute_data data, security::security_guard_ptr guard)
      : data_(data),
        context_(nullptr),
        method_data_(nullptr),
        security_guard_(guard)
{
}



void method_execution_context::execution_complete (rx_result result, data::runtime_values_data data)
{
	method_execute_result_data result_data;
	result_data.result = std::move(result);
	result_data.transaction_id = data_.transaction_id;
	result_data.data = std::move(data);
	result_data.whose = method_data_;
	context_->method_result_pending(std::move(result_data));
}

void method_execution_context::execution_complete (rx_result result)
{
	execution_complete(std::move(result), data::runtime_values_data());
}

void method_execution_context::execution_complete (data::runtime_values_data data)
{
	execution_complete(true, std::move(data));
}

security::security_guard_ptr method_execution_context::get_security_guard ()
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


} // namespace logic
} // namespace rx_platform

