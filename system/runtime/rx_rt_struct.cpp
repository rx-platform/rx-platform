

/****************************************************************************
*
*  system\runtime\rx_rt_struct.cpp
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

#include "system/rx_platform_typedefs.h"
#include "system/server/rx_server.h"

// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"

#include "lib/base64.h"
#include "system/runtime/rx_display_blocks.h"
#include "system/runtime/rx_runtime_logic.h"
#include "rx_library.h"
#include "rx_blocks.h"
#include "rx_objbase.h"
#include "rx_relations.h"
#include "lib/rx_ser_json.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "rx_rt_item_types.h"

namespace rx_platform
{
rx_security_handle_t rx_security_context();
}


namespace rx_platform {

namespace runtime {

namespace structure {

namespace
{

runtime_filters_type g_empty_filters;
runtime_sources_type g_empty_sources;
runtime_mappers_type g_empty_mappers;
runtime_events_type g_empty_events;
runtime_structs_type g_empty_structs;
runtime_variables_type g_empty_variables;

}




// Parameterized Class rx_platform::runtime::structure::empty 


// Parameterized Class rx_platform::runtime::structure::has 


// Class rx_platform::runtime::structure::variable_data 

string_type variable_data::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_data::variable_data()
{
	pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, write_task*> >();
}

variable_data::variable_data (runtime_item::smart_ptr&& rt, variable_runtime_ptr&& var, const variable_data& prototype)
	: item(std::move(rt))
	, variable_ptr(std::move(var))
{
	pending_tasks_ = std::make_unique< std::map<runtime_transaction_id_t, write_task*> >();
}



void variable_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	if (type != runtime_value_type::persistent_runtime_value
		|| value_opt[runtime::structure::value_opt_persistent])
		data.add_value(RX_DEFAULT_VARIABLE_NAME, value.to_simple());
	item->collect_data(data, type);
}

void variable_data::fill_data (const data::runtime_values_data& data)
{
	auto val = data.get_value(RX_DEFAULT_VARIABLE_NAME);
	if (!val.is_null())
	{
		rx_value_t my_type = value.get_type();
		value = rx_value(val, rx_time::now());
		value.convert_to(my_type);
	}
	item->fill_data(data);
}

rx_result variable_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	if (type != runtime_value_type::persistent_runtime_value
		|| value_opt[runtime::structure::value_opt_persistent])
	{
		std::vector<rx_simple_value> temp(2);
		temp[0] = value.to_simple();
		auto result = item->collect_value(temp[1], type);
		if (!result)
			return result;
		data.assign_static(temp);
		return true;
	}
	else
	{
		return item->collect_value(data, type);
	}
}

rx_result variable_data::fill_value (const values::rx_simple_value& data)
{
	return RX_NOT_IMPLEMENTED;
}

rx_value variable_data::get_value (runtime_process_context* ctx) const
{
	if (ctx)
		return ctx->adapt_value(value);
	else
		return value;
}

void variable_data::set_value (rx_simple_value&& val)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_value(std::move(val), rx_time::now());
	}
}

rx_result variable_data::write_value (write_data&& data, write_task* task, runtime_process_context* ctx)
{

	if (rx_is_debug_instance())
	{
		static string_type message;
		static char* message_buffer = nullptr;
		if (message.empty() || message_buffer == nullptr)
		{
			std::ostringstream ss;
			ss << "Variable "
				<< full_path
				<< " received write request with id:%08X";
			message = ss.str();
			message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/];
		}
		sprintf(message_buffer, message.c_str(), data.transaction_id);
		RUNTIME_LOG_DEBUG("variable_runtime", 500, message_buffer);
	}

	rx_result result;

	if (!data.value.convert_to(value.get_type()))
	{
		result = RX_INVALID_CONVERSION;
	}
	else
	{
		auto& filters = item->get_filters();
		if (!filters.empty())
		{
			for (auto& filter : filters)
			{
				if (filter.is_output()
					&& !(result = filter.filter_output(data.value)))
					break;
			}
			if (!result)
			{
				result.register_error("Unable to filter write value.");
				return result;
			}
		}
		auto& sources = item->get_sources();
		auto new_trans = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
		pending_tasks_->emplace(new_trans, task);
		data.transaction_id = new_trans;
		result = variable_ptr->variable_write(std::move(data), ctx, sources);
		if (!result)
			pending_tasks_->erase(new_trans);
	}
	if (!result)
	{
		if (rx_is_debug_instance())
		{
			static string_type message;
			char* message_buffer = nullptr;
			if (message.empty())
			{
				std::ostringstream ss;
				ss << "Variable "
					<< full_path
					<< " received write result for id:%08X - %s";
				message = ss.str();
			}
			string_type error = result.errors_line();
			message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/ + error.size()];
			sprintf(message_buffer, message.c_str(), data.transaction_id, error.c_str());
			RUNTIME_LOG_DEBUG("variable_runtime", 500, message_buffer);
			delete[] message_buffer;
		}
	}
	return result;
}

rx_result variable_data::internal_initialize_runtime (runtime::runtime_init_context& ctx, bool in_complex)
{

	variable_ptr->container_ = this;
	ctx.structure.push_item(*item);
	if(!in_complex)
		ctx.variables.push_variable(this);
	auto result = item->initialize_runtime(ctx);
	if(result)
		result = variable_ptr->initialize_variable(ctx);

	if (rx_is_debug_instance())
		full_path = ctx.meta.get_full_path() + RX_OBJECT_DELIMETER + ctx.path.get_current_path();

	if (!in_complex)
		ctx.variables.pop_variable();
	ctx.structure.pop_item();
	return result;
}

rx_result variable_data::internal_deinitialize_runtime (runtime::runtime_deinit_context& ctx, bool in_complex)
{
	if (!in_complex)
		ctx.variables.push_variable(this);
	auto result = variable_ptr->deinitialize_variable(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	if (!in_complex)
		ctx.variables.pop_variable();
	return result;
}

rx_result variable_data::internal_start_runtime (runtime::runtime_start_context& ctx, bool in_complex)
{
	ctx.structure.push_item(*item);
	if (!in_complex)
		ctx.variables.push_variable(this);
	auto result = item->start_runtime(ctx);
	if (result)
		result = variable_ptr->start_variable(ctx);
	process_runtime(ctx.context);
	if (!in_complex)
		ctx.variables.pop_variable();
	ctx.structure.pop_item();
	if (result)
	{
		auto& mappers = item->get_mappers();
		if (!mappers.empty())
		{
			for (auto& one : mappers)
			{
				one.value_changed(rx_value(value));
			}
		}
	}
	return result;
}

rx_result variable_data::internal_stop_runtime (runtime::runtime_stop_context& ctx, bool in_complex)
{
	if (!in_complex)
		ctx.variables.push_variable(this);
	auto result = variable_ptr->stop_variable(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	if (!in_complex)
		ctx.variables.pop_variable();
	return result;
}

void variable_data::process_runtime (runtime_process_context* ctx)
{
	rx_value prepared_value;
	if (prepare_value(prepared_value, ctx))
	{
		update_prepared(std::move(prepared_value), ctx);

	}
}

void variable_data::process_result (runtime_transaction_id_t id, rx_result&& result)
{
	auto it = pending_tasks_->find(id);
	if (it != pending_tasks_->end())
	{
		if (it->second)
		{
			if (rx_is_debug_instance())
			{
				static string_type message;
				static char* message_buffer = nullptr;
				if (message.empty())
				{
					std::ostringstream ss;
					ss << "Variable "
						<< full_path
						<< " received write result for id:%08X - %s";
					message = ss.str();
					message_buffer = new char[message.size() + 0x20/*This will hold digits and just a bit reserve*/];
				}
				if (result)
				{
					sprintf(message_buffer, message.c_str(), it->second->get_id(), "OK");
					RUNTIME_LOG_DEBUG("variable_runtime", 500, message_buffer);
				}
				else
				{
					string_type error = result.errors_line();
					char* error_message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/ + error.size()];
					sprintf(error_message_buffer, message.c_str(), it->second->get_id(), error.c_str());
					RUNTIME_LOG_DEBUG("variable_runtime", 500, error_message_buffer);
					delete[] error_message_buffer;
				}
			}
			it->second->process_result(std::move(result));
		}
		pending_tasks_->erase(it);
	}
}

rx_result variable_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		val = get_value(ctx);
		return true;
	}
	else
	{
		return item->get_value(path, val, ctx);
	}
}

rx_result variable_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		ref.ref_type = rt_value_ref_type::rt_variable;
		ref.ref_value_ptr.variable = this;
		return true;
	}
	else
	{
		return item->get_value_ref(path, ref, true);
	}
}

rx_result variable_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* variable_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result variable_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

void variable_data::variable_result_pending (runtime_process_context* ctx, rx_result&& result, runtime_transaction_id_t id)
{
	if (ctx)
	{
		write_result_struct<structure::variable_data> data;
		data.whose = this;
		data.transaction_id = id;
		data.result = std::move(result);
		ctx->variable_result_pending(std::move(data));
	}
}

void variable_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
	ctx->variable_pending(this);
}

rx_simple_value variable_data::simple_get_value () const
{
	return value.to_simple();
}

rx_result variable_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return internal_initialize_runtime(ctx, false);
}

rx_result variable_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return internal_deinitialize_runtime(ctx, false);
}

rx_result variable_data::start_runtime (runtime::runtime_start_context& ctx)
{
	return internal_start_runtime(ctx, false);
}

rx_result variable_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return internal_stop_runtime(ctx, false);
}

bool variable_data::prepare_value (rx_value& prepared_value, runtime_process_context* ctx)
{
	auto& sources = item->get_sources();

	prepared_value = variable_ptr->get_variable_input(ctx, sources);
	if (prepared_value.get_time().is_null())
		return false;

	if (prepared_value.is_null())
	{
		rx_value temp_val(value);
		temp_val.set_quality(prepared_value.get_quality());
		temp_val.set_time(prepared_value.get_time());
		prepared_value = std::move(temp_val);
	}
	if (ctx->get_mode().can_callculate(prepared_value))
	{
		prepared_value = ctx->adapt_value(prepared_value);
		if (prepared_value.convert_to(value.get_type()))
		{
			if (ctx->get_mode().can_callculate(prepared_value))
			{
				rx_result result;
				auto& filters = item->get_filters();
				if (!filters.empty())
				{
					for (auto& filter : filters)
					{
						if (filter.is_input())
						{
							result = filter.filter_input(prepared_value);
							if (!result)
								break;
						}
					}
					if (!result)
					{
						result.register_error("Unable to filter read value.");
						prepared_value.set_quality(RX_BAD_QUALITY_SYNTAX_ERROR);
					}
				}
			}
		}
		else
		{
			prepared_value = value;
			prepared_value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);

		}
	}
	if (!value.compare(prepared_value, time_compare_type::skip))
	{
		if (!prepared_value.convert_to(value.get_type()))
		{
			prepared_value = value;
			prepared_value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
		}
		return true;
	}
	return false;
}

void variable_data::update_prepared (rx_value prepared_value, runtime_process_context* ctx)
{
	// value has changed
	value = prepared_value;
	// send subscription update
	ctx->variable_value_changed(this, prepared_value);
	// send update to mappers
	if (ctx->get_mode().can_callculate(prepared_value))
	{
		auto& mappers = item->get_mappers();
		if (mappers.size() == 1)
		{
			mappers[0].value_changed(std::move(prepared_value));
		}
		else
		{
			for (auto& one : mappers)
			{
				one.value_changed(rx_value(prepared_value));
			}
		}
	}
}


// Class rx_platform::runtime::structure::struct_data 

string_type struct_data::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_data::struct_data()
{
}

struct_data::struct_data (runtime_item::smart_ptr&& rt, struct_runtime_ptr&& var, const struct_data& prototype)
	: item(std::move(rt))
	, struct_ptr(std::move(var))
{
}



void struct_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void struct_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}

rx_result struct_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	return item->collect_value(data, type);
}

rx_result struct_data::fill_value (const values::rx_simple_value& data)
{
	return item->fill_value(data);
}

rx_result struct_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = struct_ptr->initialize_struct(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result struct_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = struct_ptr->deinitialize_struct(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	return result;
}

rx_result struct_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = struct_ptr->start_struct(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result struct_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = struct_ptr->stop_struct(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}

void struct_data::process_runtime (runtime_process_context* ctx)
{
}

rx_result struct_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	return item->get_value(path, val, ctx);
}

rx_result struct_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	return item->get_value_ref(path, ref, false);
}

rx_result struct_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* struct_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result struct_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

void struct_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}


// Class rx_platform::runtime::structure::mapper_data 

string_type mapper_data::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_data::mapper_data()
      : context_(nullptr)
{
}

mapper_data::mapper_data (runtime_item::smart_ptr&& rt, mapper_runtime_ptr&& var, const mapper_data& prototype)
      : context_(nullptr)
	, item(std::move(rt))
	, mapper_ptr(std::move(var))
{
}



void mapper_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void mapper_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}

rx_result mapper_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	return item->collect_value(data, type);
}

rx_result mapper_data::fill_value (const values::rx_simple_value& data)
{
	return item->fill_value(data);
}

rx_result mapper_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);

	rx_value_t val_type=ctx.structure.get_current_item().get_local_as<rx_value_t>("ValueType", RX_NULL_TYPE);
	if(val_type!= RX_NULL_TYPE)
		mapped_value.value.convert_to(val_type);
	if (ctx.method)
	{
		io_.set_in_method(true);
		my_owner_.method_ptr = ctx.method;
	}
	else if (ctx.event)
	{
		io_.set_in_event(true);
		my_owner_.event_ptr = ctx.event;
	}
	else
	{
		auto cur_var = ctx.variables.get_current_variable();
		if (cur_var.index() == 0)
		{
			io_.set_complex(false);
			my_owner_.variable_ptr = std::get<0>(cur_var);
		}
		else if (cur_var.index() == 1)
		{
			io_.set_complex(true);
			my_owner_.block_ptr = std::get<1>(cur_var);
		}
		else
		{
			RX_ASSERT(false);// shouldn't happen
			return RX_INTERNAL_ERROR;
		}
	}
	auto& var_val = get_variable_value();
	if (val_type == RX_NULL_TYPE)
	{
		mapped_value.value.convert_to(var_val.get_type());
		mapper_ptr->value_type_ = mapped_value.value.get_type();

	}
	else
	{
		mapper_ptr->value_type_ = val_type;
	}
	mapper_ptr->container_ = this;
	ctx.mappers.push_mapper(mapper_id, this);

	auto result = item->initialize_runtime(ctx);
	if (result)
		result = mapper_ptr->initialize_mapper(ctx);
	if (rx_is_debug_instance())
		full_path = ctx.meta.get_full_path() + RX_OBJECT_DELIMETER + ctx.path.get_current_path();
	ctx.structure.pop_item();
	ctx.mappers.pop_mapper(mapper_id);
	return result;
}

rx_result mapper_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = mapper_ptr->deinitialize_mapper(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	mapper_ptr->container_ = nullptr;
	my_owner_ = mapped_owner_ptr();
	return result;
}

rx_result mapper_data::start_runtime (runtime::runtime_start_context& ctx)
{
	context_ = ctx.context;
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
	{
		auto value_type = mapped_value.value.get_type();
		mapper_ptr->value_type_ = value_type;
		result = mapper_ptr->start_mapper(ctx);
	}
	ctx.structure.pop_item();
	return result;
}

rx_result mapper_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = mapper_ptr->stop_mapper(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	context_ = nullptr;
	return result;
}

void mapper_data::process_update (values::rx_value&& value)
{
	if (mapper_ptr && context_ && !io_.get_in_method() && !io_.get_in_event())
	{
		if (value.is_null() && my_owner_.variable_ptr)
			value = context_->adapt_value(get_variable_value());
		rx_result result;
		rx_simple_value prepared_value = value.to_simple();
		auto quality = value.get_quality();
		auto& filters = item->get_filters();
		if (!filters.empty() && !value.is_null())
		{
			for (auto& filter : filters)
			{
				if (filter.is_output())
				{
					result = filter.filter_output(prepared_value);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter input value.");
				quality = RX_BAD_QUALITY_FAILURE;
			}
			if (!prepared_value.convert_to(mapper_ptr->value_type_))
				quality = RX_BAD_QUALITY_TYPE_MISMATCH;
			rx_value filtered_value = rx_value(std::move(prepared_value), value.get_time());
			filtered_value.set_quality(quality);
			mapped_value.set_value(filtered_value, context_);
			mapper_ptr->mapped_value_changed(std::move(filtered_value), context_);
		}
		else
		{
			if (!value.convert_to(mapper_ptr->value_type_))
				value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
			mapped_value.set_value(value, context_);
			mapper_ptr->mapped_value_changed(std::move(value), context_);
		}
	}
}

void mapper_data::process_write (write_data&& data)
{
	if (!io_.get_in_method() && !io_.get_in_event())
	{
		if (my_owner_.variable_ptr)
		{
			auto trans_id = data.transaction_id;
			rx_value prepared_value = rx_value(std::move(data.value), rx_time());
			if (!prepared_value.convert_to(get_variable_value().get_type()))
			{
				if (rx_is_debug_instance())
				{
					static string_type message;
					static char* message_buffer = nullptr;
					if (message.empty())
					{
						std::ostringstream ss;
						ss << "Mapper "
							<< full_path
							<< " received write result for id:%08X - "
							<< RX_INVALID_CONVERSION;
						message = ss.str();
						message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/];
					}
					sprintf(message_buffer, message.c_str(), trans_id);
					RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
				}
				mapper_ptr->mapper_result_received(RX_INVALID_CONVERSION, trans_id, context_);
			}
			else
			{
				rx_result result;
				auto& filters = item->get_filters();
				if (!filters.empty())
				{
					for (auto& filter : filters)
					{
						if (filter.is_input())
						{
							result = filter.filter_input(prepared_value);
							if (!result)
								break;
						}
					}
					if (!result)
					{
						result.register_error("Unable to filter output value.");
					}
				}
				if (!result)
				{
					if (rx_is_debug_instance())
					{
						static string_type message;
						char* message_buffer = nullptr;
						if (message.empty())
						{
							std::ostringstream ss;
							ss << "Mapper "
								<< full_path
								<< " received write result for id:%08X - %s";
							message = ss.str();
						}
						string_type error = result.errors_line();
						message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/ + error.size()];
						sprintf(message_buffer, message.c_str(), trans_id, error.c_str());
						RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
						delete[] message_buffer;
					}
					mapper_ptr->mapper_result_received(std::move(result), trans_id, context_);
				}
				else
				{
					auto task = new mapper_write_task(this, trans_id);
					data.value = prepared_value.to_simple();
					if (io_.get_complex())
						result = my_owner_.block_ptr->write_value(std::move(data), task, context_);
					else
						result = my_owner_.variable_ptr->write_value(std::move(data), task, context_);
					if (!result)
					{
						if (rx_is_debug_instance())
						{
							static string_type message;
							char* message_buffer = nullptr;
							if (message.empty())
							{
								std::ostringstream ss;
								ss << "Mapper "
									<< full_path
									<< " received write result for id:%08X - %s";
								message = ss.str();
							}
							string_type error = result.errors_line();
							message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/ + error.size()];
							sprintf(message_buffer, message.c_str(), trans_id, error.c_str());
							RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
							delete[] message_buffer;
						}
						mapper_ptr->mapper_result_received(std::move(result), trans_id, context_);
					}
				}
			}
		}
	}
	else
	{
		RX_ASSERT(false);
	}
}

void mapper_data::process_execute (execute_data&& data)
{
	if (io_.get_in_method())
	{
		if (my_owner_.method_ptr)
		{
			auto trans_id = data.transaction_id;
			auto task = new mapper_execute_task(this, trans_id);
			context_execute_data ctx_data;
			ctx_data.data = std::move(data.value);
			ctx_data.identity = data.identity;
			ctx_data.internal = data.internal;
			ctx_data.test = data.test;
			ctx_data.transaction_id = trans_id;

			auto result = my_owner_.method_ptr->execute(std::move(ctx_data), task, context_);
			if (!result)
			{
				if (rx_is_debug_instance())
				{
					static string_type message;
					char* message_buffer = nullptr;
					if (message.empty())
					{
						std::ostringstream ss;
						ss << "Mapper "
							<< full_path
							<< " received execute result for id:%08X - %s";
						message = ss.str();
					}
					string_type error = result.errors_line();
					message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/ + error.size()];
					sprintf(message_buffer, message.c_str(), trans_id, error.c_str());
					RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
					delete[] message_buffer;
				}
				mapper_ptr->mapper_execute_result_received(std::move(result), values::rx_simple_value(), trans_id, context_);
			}
		}
	}
	else
	{
		RX_ASSERT(false);
	}
}

void mapper_data::mapper_write_pending (write_data&& data)
{
	if (!io_.get_in_method() && !io_.get_in_event())
	{
		write_data_struct<mapper_data> reg_data;
		reg_data.whose = this;
		if (data.identity == 0)
		{
			data.identity = rx_security_context();
		}
		reg_data.data = std::move(data);

		if (context_)
			context_->mapper_write_pending(std::move(reg_data));
	}
	else
	{
		RX_ASSERT(false);
	}
}

void mapper_data::mapper_execute_pending (execute_data&& data)
{
	if (io_.get_in_method())
	{
		execute_data_struct<mapper_data> reg_data;
		reg_data.whose = this;
		if (data.identity == 0)
		{
			data.identity = rx_security_context();
		}
		reg_data.data = std::move(data);

		if (context_)
			context_->mapper_execute_pending(std::move(reg_data));
	}
}

rx_value mapper_data::get_mapped_value () const
{
	if (!io_.get_in_method() && !io_.get_in_event())
	{
		if (my_owner_.variable_ptr && context_)
		{
			return context_->adapt_value(get_variable_value());
		}
	}
	else
	{
		if (my_owner_.method_ptr && context_)
		{
			return my_owner_.method_ptr->get_value(context_);
		}
	}

	rx_value val;
	val.set_time(rx_time::now());
	val.set_quality(RX_BAD_QUALITY_OFFLINE);
	return val;
}

rx_result mapper_data::value_changed (rx_value&& val)
{
	if (!io_.get_in_method() && !io_.get_in_event())
	{
		if (context_)
			context_->mapper_update_pending({ this, std::move(val) });
	}
	return true;
}

bool mapper_data::can_read () const
{
	return mapper_ptr ? mapper_ptr->supports_read() && io_.get_input() : false;
}

bool mapper_data::can_write () const
{
	return mapper_ptr ? mapper_ptr->supports_write() && io_.get_output() : false;
}

void mapper_data::process_write_result (rx_result&& result, runtime_transaction_id_t id)
{
	if (mapper_ptr)
	{
		if (rx_is_debug_instance())
		{
			static string_type message;
			static char* message_buffer = nullptr;
			if (message.empty())
			{
				std::ostringstream ss;
				ss << "Mapper "
					<< full_path
					<< " received write result for id:%08X";
				message = ss.str() + " - OK";
				message_buffer = new char[message.size() + 0x20/*This will hold digits and just a bit reserve*/];
			}
			if (result)
			{
				sprintf(message_buffer, message.c_str(), id);
				RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
			}
			else
			{
				string_type error = " - "s + result.errors_line();
				char* error_message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/ + error.size()];
				sprintf(error_message_buffer, (message + " - %s").c_str(), id, error.c_str());
				RUNTIME_LOG_DEBUG("mapper_runtime", 500, error_message_buffer);
				delete[] error_message_buffer;
			}
		}
		mapper_ptr->mapper_result_received(std::move(result), id, context_);
	}
}

void mapper_data::process_event (values::rx_simple_value data)
{
	if (mapper_ptr)
	{
		if (rx_is_debug_instance())
		{
			static string_type message;
			static char* message_buffer = nullptr;
			if (message.empty())
			{
				std::ostringstream ss;
				ss << "Mapper "
					<< full_path
					<< " received event";
				message = ss.str() + " - OK";
				message_buffer = new char[message.size() + 0x20/*This will hold digits and just a bit reserve*/];
			}
		}
		mapper_ptr->mapped_event_fired(std::move(data), context_);
	}
}

void mapper_data::process_execute_result (rx_result&& result, values::rx_simple_value out_data, runtime_transaction_id_t id)
{
	if (mapper_ptr)
	{
		if (rx_is_debug_instance())
		{
			static string_type message;
			static char* message_buffer = nullptr;
			if (message.empty())
			{
				std::ostringstream ss;
				ss << "Mapper "
					<< full_path
					<< " received execute result for id:%08X";
				message = ss.str() + " - OK";
				message_buffer = new char[message.size() + 0x20/*This will hold digits and just a bit reserve*/];
			}
			if (result)
			{
				sprintf(message_buffer, message.c_str(), id);
				RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
			}
			else
			{
				string_type error = " - "s + result.errors_line();
				char* error_message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/ + error.size()];
				sprintf(error_message_buffer, (message + " - %s").c_str(), id, error.c_str());
				RUNTIME_LOG_DEBUG("mapper_runtime", 500, error_message_buffer);
				delete[] error_message_buffer;
			}
		}
		mapper_ptr->mapper_execute_result_received(std::move(result), std::move(out_data), id, context_);
	}
}

rx_result mapper_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		val = get_value(ctx);
		return true;
	}
	else
	{
		return item->get_value(path, val, ctx);
	}
}

rx_result mapper_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		ref.ref_type = rt_value_ref_type::rt_full_value;
		ref.ref_value_ptr.full_value = &mapped_value;
		return true;
	}
	else
	{
		return item->get_value_ref(path, ref, false);
	}
}

rx_result mapper_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* mapper_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result mapper_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

rx_value mapper_data::get_value (runtime_process_context* ctx) const
{
	return mapped_value.get_value(ctx);
}

void mapper_data::object_state_changed (runtime_process_context* ctx)
{
	rx_value val = ctx->adapt_value(mapped_value.value);
	ctx->mapper_update_pending({ this, std::move(val) });
	item->object_state_changed(ctx);
}

const rx_value& mapper_data::get_variable_value () const
{
	static rx_value g_empty;
	if (!io_.get_in_method() && !io_.get_in_event())
	{
		if (io_.get_complex())
		{
			if (my_owner_.block_ptr)
				return my_owner_.block_ptr->variable.value;
		}
		else
		{
			if (my_owner_.variable_ptr)
				return my_owner_.variable_ptr->value;
		}
	}
	return g_empty;
}

data::runtime_data_model mapper_data::get_method_inputs ()
{
	if (io_.get_in_method() && my_owner_.method_ptr)
	{
		return my_owner_.method_ptr->get_method_inputs();
	}
	else
	{
		RX_ASSERT(false);
		return data::runtime_data_model();
	}
}

data::runtime_data_model mapper_data::get_method_outputs ()
{
	if (io_.get_in_method() && my_owner_.method_ptr)
	{
		return my_owner_.method_ptr->get_method_outputs();
	}
	else
	{
		RX_ASSERT(false);
		return data::runtime_data_model();
	}
}

data::runtime_data_model mapper_data::get_event_arguments ()
{
	if (io_.get_in_event() && my_owner_.event_ptr)
	{
		return my_owner_.event_ptr->get_arguments();
	}
	else
	{
		RX_ASSERT(false);
		return data::runtime_data_model();
	}
}


// Class rx_platform::runtime::structure::source_data 

string_type source_data::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_data::source_data()
      : context_(nullptr)
{
}

source_data::source_data (runtime_item::smart_ptr&& rt, source_runtime_ptr&& var, const source_data& prototype)
      : context_(nullptr)
	, item(std::move(rt))
	, source_ptr(std::move(var))
{
}



void source_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void source_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}

rx_result source_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	return item->collect_value(data, type);
}

rx_result source_data::fill_value (const values::rx_simple_value& data)
{
	return item->fill_value(data);
}

rx_result source_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(*item);

	rx_value_t val_type = ctx.structure.get_current_item().get_local_as<rx_value_t>("ValueType", RX_NULL_TYPE);
	if (val_type != RX_NULL_TYPE)
		input_value.value.convert_to(val_type);
	auto cur_var = ctx.variables.get_current_variable();
	if (cur_var.index() == 0)
	{
		io_.set_complex(false);
		my_variable_.variable_ptr = std::get<0>(cur_var);
	}
	else if (cur_var.index() == 1)
	{
		io_.set_complex(true);
		my_variable_.block_ptr = std::get<1>(cur_var);
	}
	else
	{
		RX_ASSERT(false);// shouldn't happen
		return RX_INTERNAL_ERROR;
	}
	if (input_value.value.get_type() == RX_NULL_TYPE 
		&& my_variable_.variable_ptr)
		input_value.value.convert_to(get_variable_value().get_type());
	source_ptr->value_type_ = input_value.value.get_type();
	input_value.value.set_quality(RX_DEFAULT_VALUE_QUALITY);
	source_ptr->container_ = this;

	auto result = item->initialize_runtime(ctx);
	if (result)
		result = source_ptr->initialize_source(ctx);

	if (current_value_.is_null())
	{
		current_value_ = input_value.value;
		current_value_.increment_signal_level();
	}

	if (rx_is_debug_instance())
		full_path = ctx.meta.get_full_path() + RX_OBJECT_DELIMETER + ctx.path.get_current_path();

	ctx.structure.pop_item();
	return result;
}

rx_result source_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = source_ptr->deinitialize_source(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	source_ptr->container_ = nullptr;
	/*this union is plain union of two pointers so setting one of them is enough*/
	my_variable_.variable_ptr = nullptr;
	return result;
}

rx_result source_data::start_runtime (runtime::runtime_start_context& ctx)
{
	context_ = ctx.context;
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
	{
		auto value_type = input_value.value.get_type();
		source_ptr->value_type_ = value_type;
		result = source_ptr->start_source(ctx);
	}
	ctx.structure.pop_item();
	return result;
}

rx_result source_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = source_ptr->stop_source(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	context_ = nullptr;
	return result;
}

rx_result source_data::write_value (write_data&& data)
{
	if (!context_ || !source_ptr)
		return RX_ERROR_STOPPED;
	else if (!source_ptr->is_output())
		return RX_NOT_SUPPORTED;

	write_data_struct<source_data> reg_data;
	reg_data.whose = this;
	reg_data.data = std::move(data);

	context_->source_write_pending(std::move(reg_data));
	return true;
}

void source_data::process_update (values::rx_value&& value)
{
	RX_ASSERT(source_ptr);
	if (source_ptr && context_ 
		&& my_variable_.variable_ptr /*this union is plain union of two pointers so test one of them is enough*/)
	{
		if (value != current_value_)
		{
			input_value.set_value(value, context_);
			rx_result result;
			auto& filters = item->get_filters();
			if (!filters.empty())
			{
				for (auto& filter : filters)
				{
					if (filter.is_input())
					{
						result = filter.filter_input(value);
						if (!result)
							break;
					}
				}
				if (!result)
				{
					result.register_error("Unable to filter input value.");
				}
			}
			current_value_ = value;
			if (io_.get_complex())
			{
				if (my_variable_.block_ptr)
				{
					context_->variable_pending(my_variable_.block_ptr);
				}
			}
			else
			{
				if (my_variable_.variable_ptr)
				{
					context_->variable_pending(my_variable_.variable_ptr);
				}
			}
		}
	}
}

void source_data::process_write (write_data&& data)
{
	RX_ASSERT(source_ptr);
	if (source_ptr && context_)
	{
		if (rx_is_debug_instance())
		{
			static string_type message;
			static char* message_buffer = nullptr;
			if (message.empty() || message_buffer == nullptr)
			{
				std::ostringstream ss;
				ss << "Source "
					<< full_path
					<< " received write request with id:%08X";
				message = ss.str();
				message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/];
			}
			sprintf(message_buffer, message.c_str(), data.transaction_id);
			RUNTIME_LOG_DEBUG("source_runtime", 500, message_buffer);
		}
		rx_result result;
		auto& filters = item->get_filters();
		if (!filters.empty())
		{
			for (auto& filter : filters)
			{
				if (filter.is_output())
				{
					result = filter.filter_output(data.value);
					if (!result)
						break;
				}
			}
			if (!result)
			{
				result.register_error("Unable to filter output value.");
			}
		}
		if (result)
		{
			result = source_ptr->source_write(std::move(data), context_);
		}
		if (!result)
			source_result_pending(std::move(result), data.transaction_id);


	}
}

void source_data::source_update_pending (values::rx_value&& value)
{
	if (context_)
		context_->source_update_pending({ this, std::move(value) });
}

bool source_data::is_input () const
{
	return source_ptr ? source_ptr->supports_input() && io_.get_input() : false;
}

bool source_data::is_output () const
{
	return source_ptr ? source_ptr->supports_output() && io_.get_output() : false;
}

const rx_value& source_data::get_current_value () const
{
	return current_value_;
}

void source_data::source_result_pending (rx_result&& result, runtime_transaction_id_t id)
{
	if (context_)
	{
		write_result_struct<structure::source_data> data;
		data.whose = this;
		data.transaction_id = id;
		data.result = std::move(result);
		context_->source_result_pending(std::move(data));
	}
}

void source_data::process_result (runtime_transaction_id_t id, rx_result&& result)
{
	if (io_.get_complex())
	{
		if (my_variable_.block_ptr)
		{
			my_variable_.block_ptr->process_result(id, std::move(result));
		}
	}
	else
	{
		if (my_variable_.variable_ptr)
		{
			my_variable_.variable_ptr->process_result(id, std::move(result));
		}
	}
}

rx_result source_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		val = get_value(ctx);
		return true;
	}
	else
	{
		return item->get_value(path, val, ctx);
	}
}

rx_result source_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		ref.ref_type = rt_value_ref_type::rt_full_value;
		ref.ref_value_ptr.full_value = &input_value;
		return true;
	}
	else
	{
		return item->get_value_ref(path, ref, false);
	}
}

rx_result source_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* source_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result source_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

rx_value source_data::get_value (runtime_process_context* ctx) const
{
	return input_value.get_value(ctx);
}

void source_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}

const rx_value& source_data::get_variable_value () const
{
	static rx_value g_empty;
	if (io_.get_complex())
	{
		if (my_variable_.block_ptr)
			return my_variable_.block_ptr->variable.value;
	}
	else
	{
		if (my_variable_.variable_ptr)
			return my_variable_.variable_ptr->value;
	}
	return g_empty;
}


// Class rx_platform::runtime::structure::event_data 

string_type event_data::type_name = RX_CPP_EVENT_TYPE_NAME;

event_data::event_data()
{
}

event_data::event_data (runtime_item::smart_ptr&& rt, event_runtime_ptr&& var, event_data&& prototype)
	: item(std::move(rt))
	, event_ptr(std::move(var))
	, arguments(std::move(prototype.arguments))
{
}



void event_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
	arguments.collect_data(data, type);
}

void event_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);

	auto child_it = data.children.find("Args");
	if (child_it != data.children.end()
		&& std::holds_alternative<data::runtime_values_data>(child_it->second))
	{
		const auto& simple_child = std::get<data::runtime_values_data>(child_it->second);
		arguments.fill_data(simple_child);
	}
}

rx_result event_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	return item->collect_value(data, type);
}

rx_result event_data::fill_value (const values::rx_simple_value& data)
{
	return item->fill_value(data);
}

rx_result event_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	event_ptr->container_ = this;
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
		result = event_ptr->initialize_event(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result event_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = event_ptr->deinitialize_event(ctx);
	if (result)
	{
		result = item->deinitialize_runtime(ctx);
	}
	event_ptr->container_ = nullptr;
	return result;
}

rx_result event_data::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = event_ptr->start_event(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result event_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = event_ptr->stop_event(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	return result;
}

void event_data::process_runtime (runtime_process_context* ctx)
{
}

rx_result event_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		return RX_INVALID_PATH;
	}
	else
	{
		auto idx = path.find(RX_OBJECT_DELIMETER);
		string_type sub_path;
		string_type rest_path;
		if (idx != string_type::npos)
		{
			sub_path = path.substr(0, idx);
			rest_path = path.substr(idx + 1);
		}
		else
		{
			sub_path = path;
		}
		if (sub_path == "Args")
		{
			return arguments.get_value( rest_path, val, ctx);
		}
		else
		{
			return item->get_value(path, val, ctx);
		}
	}
}

rx_result event_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		return RX_INVALID_PATH;
	}
	else
	{
		auto idx = path.find(RX_OBJECT_DELIMETER);
		string_view_type sub_path;
		string_view_type rest_path;
		if (idx != string_view_type::npos)
		{
			sub_path = path.substr(0, idx);
			rest_path = path.substr(idx + 1);
		}
		else
		{
			sub_path = path;
		}
		if (sub_path == "Args")
		{
			return arguments.get_value_ref(rest_path, ref, false);
		}
		else
		{
			return item->get_value_ref(path, ref, false);
		}
	}
}

rx_result event_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		runtime_item_attribute args_attr;
		args_attr.name = "Args";
		args_attr.type = rx_attribute_type::data_attribute_type;
		args_attr.full_path = prefix.empty() ? args_attr.name : prefix+RX_OBJECT_DELIMETER + args_attr.name;
		items.emplace_back(std::move(args_attr));
		return item->browse_items(prefix, path, filter, items, ctx);
	}
	else
	{
		auto idx = path.find(RX_OBJECT_DELIMETER);
		string_type sub_path;
		string_type rest_path;
		if (idx != string_type::npos)
		{
			sub_path = path.substr(0, idx);
			rest_path = path.substr(idx + 1);
		}
		else
		{
			sub_path = path;
		}
		if (sub_path == "Args")
		{
			string_type bellow_prefix = prefix.empty() ? sub_path : prefix + RX_OBJECT_DELIMETER + sub_path;
			return arguments.browse_items(bellow_prefix, rest_path, filter, items, ctx);
		}
		else
		{
			return item->browse_items(prefix, path, filter, items, ctx);
		}
	}
}

const runtime_item* event_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result event_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

void event_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}

data::runtime_data_model event_data::get_arguments ()
{
	return arguments.create_runtime_model();
}

void event_data::event_fired (rx_simple_value data)
{
}


// Class rx_platform::runtime::structure::filter_data 

string_type filter_data::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_data::filter_data()
      : context_(nullptr)
{
}

filter_data::filter_data (runtime_item::smart_ptr&& rt, filter_runtime_ptr&& var, const filter_data& prototype)
      : context_(nullptr)
	, item(std::move(rt))
	, filter_ptr(std::move(var))
{
}



void filter_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item->collect_data(data, type);
}

void filter_data::fill_data (const data::runtime_values_data& data)
{
	item->fill_data(data);
}

rx_result filter_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	return item->collect_value(data, type);
}

rx_result filter_data::fill_value (const values::rx_simple_value& data)
{
	return item->fill_value(data);
}

rx_result filter_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	filter_ptr->container_ = this;

	auto cur_var = ctx.variables.get_current_variable();
	if (cur_var.index() == 0)
	{
		io_.set_complex(false);
		my_variable_.variable_ptr = std::get<0>(cur_var);
	}
	else if (cur_var.index() == 1)
	{
		io_.set_complex(true);
		my_variable_.block_ptr = std::get<1>(cur_var);
	}
	else
	{
		RX_ASSERT(false);// shouldn't happen
		return RX_INTERNAL_ERROR;
	}
	ctx.structure.push_item(*item);
	auto result = item->initialize_runtime(ctx);
	if (result)
	{
		result = filter_ptr->initialize_filter(ctx);
	}
	ctx.structure.pop_item();
	return result;
}

rx_result filter_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = filter_ptr->deinitialize_filter(ctx);
	if (result)
		result = item->deinitialize_runtime(ctx);
	filter_ptr->container_ = nullptr;
	return result;
}

rx_result filter_data::start_runtime (runtime::runtime_start_context& ctx)
{
	context_ = ctx.context;
	ctx.structure.push_item(*item);
	auto result = item->start_runtime(ctx);
	if (result)
		result = filter_ptr->start_filter(ctx);
	ctx.structure.pop_item();
	return result;
}

rx_result filter_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = filter_ptr->stop_filter(ctx);
	if (result)
		result = item->stop_runtime(ctx);
	context_ = nullptr;
	return result;
}

void filter_data::process_runtime (runtime_process_context* ctx)
{
}

rx_result filter_data::filter_output (rx_simple_value& val)
{
	return filter_ptr ? filter_ptr->filter_output(val) : RX_ERROR_STOPPED;
}

rx_result filter_data::filter_input (rx_value& val)
{
	return filter_ptr ? filter_ptr->filter_input(val) : RX_ERROR_STOPPED;
}

bool filter_data::is_input () const
{
	return filter_ptr ? filter_ptr->supports_input() && io_.get_input() : false;
}

bool filter_data::is_output () const
{
	return filter_ptr ? filter_ptr->supports_output() && io_.get_output() : false;
}

rx_result filter_data::get_value (runtime_handle_t handle, values::rx_simple_value& val) const
{
	if (context_)
	{
		return context_->get_value(handle, val);
	}
	else
	{
		RX_ASSERT(false);
		return "Context not binded!";
	}
}

rx_result filter_data::set_value (runtime_handle_t handle, values::rx_simple_value&& val)
{
	if (context_)
	{
		return context_->set_value(handle, std::move(val));
	}
	else
	{
		RX_ASSERT(false);
		return "Context not binded!";
	}
}

rx_result filter_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	return item->get_value(path, val, ctx);
}

rx_result filter_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	return item->get_value_ref(path, ref, false);
}

rx_result filter_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return item->browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* filter_data::get_child_item (string_view_type path) const
{
	return item->get_child_item(path);
}

rx_result filter_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	return item->get_local_value(path, val);
}

rx_result filter_data::filter_changed ()
{
	if (io_.get_complex())
	{
		if (my_variable_.block_ptr)
		{
			context_->variable_pending(my_variable_.block_ptr);
		}
	}
	else
	{
		if (my_variable_.variable_ptr)
		{
			context_->variable_pending(my_variable_.variable_ptr);
		}
	}
	return true;
}

void filter_data::object_state_changed (runtime_process_context* ctx)
{
	item->object_state_changed(ctx);
}


// Class rx_platform::runtime::structure::const_value_data 

string_type const_value_data::type_name = RX_CONST_VALUE_TYPE_NAME;


rx_value const_value_data::get_value (runtime_process_context* ctx) const
{
	return ctx->adapt_value(value);
}

rx_simple_value const_value_data::simple_get_value () const
{
	return value;
}

rx_result const_value_data::set_value (rx_simple_value&& val)
{
	if (val.get_type() == value.get_type())
	{
		value = std::move(val);
		return true;
	}
	else
	{
		if (val.get_type() == RX_STRING_TYPE && value.get_type() == RX_BYTES_TYPE)
		{
			string_type str = val.get_string();
			value.assign_static(urke::get_data(str));
			return true;
		}
		else
		{
			rx_simple_value temp(std::move(val));
			if (temp.convert_to(value.get_type()))
			{
				value = std::move(temp);
				return true;
			}
			else
			{
				return RX_INVALID_CONVERSION;
			}
		}
	}
}


// Class rx_platform::runtime::structure::value_data 

string_type value_data::type_name = RX_VALUE_TYPE_NAME;


rx_value value_data::get_value (runtime_process_context* ctx) const
{
	if (value_opt[opt_state_ignorant])
	{
		static rx_mode_type on_mode;

		rx_value ret(value);
		value.get_value(ret, ctx->get_mode_time(), on_mode);
		ret.set_origin(RX_ALWAYS_ORIGIN);
		return ret;
	}
	else
	{
		return ctx->adapt_value(value);
	}
}

void value_data::set_value (rx_simple_value&& val, const rx_time& time)
{
	if (val.convert_to(value.get_type()))
	{
		value = rx_timed_value(std::move(val), time);
	}
}

void value_data::object_state_changed (runtime_process_context* ctx)
{
	if (ctx->get_mode_time() > value.get_time())
	{
		value.set_time(ctx->get_mode_time());
	}
}

rx_result value_data::write_value (write_data&& data, runtime_process_context* ctx, bool& changed)
{
	security::secured_scope _(data.identity);
	std::ostringstream ss;
	ss << "Writing value "
		<< data.value.to_string();
	RUNTIME_LOG_TRACE("value_data", 200, ss.str());
	auto result = check_set_value(ctx, data.internal, data.test);
	if (!result)
		return result;

	if (data.value.convert_to(value.get_type()))
	{

		rx_timed_value temp(std::move(data.value), rx_time::now());
		if (!temp.compare(value, time_compare_type::skip))
		{
			changed = true;
			value = std::move(temp);
			if (value_opt[runtime::structure::value_opt_persistent])
				ctx->runtime_dirty();
		}
		else
		{
			changed = false;
		}
		return true;
	}
	else
	{
		return "Conversion error!";
	}
}

rx_simple_value value_data::simple_get_value () const
{
	return value.to_simple();
}

rx_result value_data::simple_set_value (rx_simple_value&& val, runtime_process_context* ctx, bool& changed)
{
	auto result = check_set_value(ctx, true, ctx->get_mode().is_test());
	if (!result)
		return result;

	if (val.convert_to(value.get_type()))
	{
		rx_timed_value temp(std::move(val), rx_time::now());
		if (!temp.compare(value, time_compare_type::skip))
		{
			changed = true;
			value = std::move(temp);
			if (value_opt[runtime::structure::value_opt_persistent])
				ctx->runtime_dirty();
		}
		else
		{
			changed = false;
		}
		return true;
	}
	else
	{
		return RX_INVALID_CONVERSION;
	}
}

rx_result value_data::check_set_value (runtime_process_context* ctx, bool internal, bool test)
{
	if (value_opt[runtime::structure::opt_is_constant])
	{
		return RX_NOT_SUPPORTED;
	}
	if (value_opt[runtime::structure::value_opt_readonly] && !internal)
	{
		return RX_ACCESS_DENIED;
	}
	if (!value_opt[runtime::structure::opt_state_ignorant])
	{
		if (ctx->get_mode().is_off())
			return "Runtime if in Off state!";
		if (test != ctx->get_mode().is_test())
			return "Test mode mismatch!";
	}
	return true;
}


// Class rx_platform::runtime::structure::runtime_item 


// Class rx_platform::runtime::structure::indirect_value_data 

string_type indirect_value_data::type_name = RX_INDIRECT_VALUE_TYPE_NAME;


rx_value indirect_value_data::get_value (runtime_process_context* ctx) const
{
	return ctx->adapt_value(value);
}

void indirect_value_data::set_value (rx_simple_value&& val, const rx_time& time)
{
	if (val.convert_to(value.get_type()))
	{
		default_value_ = val;
		value = rx_timed_value(std::move(val), time);
	}
}

void indirect_value_data::object_state_changed (runtime_process_context* ctx)
{
	if (ctx->get_mode_time() > value.get_time())
		value.set_time(ctx->get_mode_time());
}

rx_result indirect_value_data::write_value (context_write_data&& data, runtime_process_context* ctx)
{
	return RX_NOT_IMPLEMENTED;
	/*if (!data.value.convert_to(value.get_type()))
		return RX_INVALID_CONVERSION;
	return true;*/
}


// Class rx_platform::runtime::structure::write_task 

write_task::~write_task()
{
}



// Class rx_platform::runtime::structure::mapper_write_task 

mapper_write_task::mapper_write_task (mapper_data* my_mapper, runtime_transaction_id_t trans_id)
      : my_mapper_(my_mapper),
        transaction_id_(trans_id)
{
}



void mapper_write_task::process_result (rx_result&& result)
{
	my_mapper_->process_write_result(std::move(result), transaction_id_);
}

runtime_transaction_id_t mapper_write_task::get_id () const
{
	return transaction_id_;
}


// Class rx_platform::runtime::structure::block_data 


void block_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	for (const auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
			case rt_const_index_type:
				// simple value
				{
					auto& this_val = values[(one.index >> rt_type_shift)];
					if (this_val.is_array())
					{
						std::vector<rx_simple_value> vals;
						for (int i = 0; i < this_val.get_size(); i++)
							vals.push_back(this_val.get_item(i)->value);
						data.add_value(one.name, std::move(vals));
					}
					else
					{
						data.add_value(one.name, this_val.get_item()->value);
					}
				}
				break;
			case rt_data_index_type:
				{
					auto& this_val = children[(one.index >> rt_type_shift)];
					if (this_val.is_array())
					{
						std::vector<data::runtime_values_data> childs;
						for (int i = 0; i < this_val.get_size(); i++)
						{
							data::runtime_values_data child_data;
							this_val.get_item(i)->collect_data(child_data, type);
							if (!child_data.empty())
								childs.push_back(std::move(child_data));
						}
						data.add_array_child(one.name, std::move(childs));
					}
					else
					{
						data::runtime_values_data child_data;
						this_val.get_item()->collect_data(child_data, type);
						if (!child_data.empty())
							data.add_child(one.name, std::move(child_data));
					}
				}
				break;
			default:
				RX_ASSERT(false);
		}
	}
}

void block_data::fill_data (const data::runtime_values_data& data)
{
	for (auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			{// simple value
				if (!values[one.index >> rt_type_shift].is_array())
				{
					auto val = data.get_value(one.name);
					if (!val.is_null())
					{
						values[one.index >> rt_type_shift].get_item()->set_value(std::move(val));
					}
				}
				else
				{
					std::vector<rx_simple_value> vals;
					if (data.get_array_value(one.name, vals))
					{
						size_t dim = std::min<size_t>(vals.size(), values[(one.index >> rt_type_shift)].get_size());
						for (size_t i = 0; i < dim; i++)
						{
							values[one.index >> rt_type_shift].get_item((int)i)->set_value(std::move(vals[i]));
						}
					}
				}
			}
			break;
		case rt_data_index_type:
			{// child value
				if (!children[(one.index >> rt_type_shift)].is_array())
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end())
					{
						if (std::holds_alternative<data::runtime_values_data>(it->second))
						{
							children[one.index >> rt_type_shift].get_item()->fill_data(
								std::get< data::runtime_values_data>(it->second));
						}
					}
				}
				else
				{
					auto it = data.children.find(one.name);
					if (it != data.children.end())
					{
						if (std::holds_alternative<std::vector<data::runtime_values_data> >(it->second))
						{
							auto& this_val = children[(one.index >> rt_type_shift)];
							auto& childs = std::get<std::vector<data::runtime_values_data>>(it->second);
							if (childs.empty())
							{
								RX_ASSERT(this_val.get_prototype());
								auto one_block = *this_val.get_prototype();
								this_val.declare_null_array(std::move(one_block));
							}
							else if ((int)childs.size() == this_val.get_size())
							{
								for (size_t i = 0; i < childs.size(); i++)
								{
									this_val.get_item((int)i)->fill_data(childs[i]);
								}
							}
							else
							{
								const block_data* prototype = nullptr;
								std::vector<block_data> temp;
								for (int j = 0; j < (int)childs.size(); j++)
								{
									if (j < this_val.get_size())
									{
										this_val.get_item(j)->fill_data(childs[j]);
										temp.push_back(*this_val.get_item(j));
									}
									else
									{
										if (prototype == nullptr)
										{
											RX_ASSERT(this_val.get_prototype());
											prototype = this_val.get_prototype();
										}
										block_data one_block(*prototype);
										one_block.fill_data(childs[j]);
										temp.push_back(std::move(one_block));
									}
								}
								this_val = std::move(temp);
							}
						}
					}
				}
			}
			break;
		default:
			RX_ASSERT(false);
		}
	}
}

rx_result block_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	std::vector<rx_simple_value> my_vals;
	for (const auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			// simple value
			{
				auto& this_val = values[(one.index >> rt_type_shift)];
				if (this_val.is_array())
				{
					std::vector<rx_simple_value> vals;
					for (int i = 0; i < this_val.get_size(); i++)
						vals.push_back(this_val.get_item(i)->value);
					rx_simple_value temp;
					temp.assign_array(vals);
					my_vals.push_back(std::move(temp));
				}
				else
				{
					my_vals.push_back(this_val.get_item()->value);
				}
			}
			break;
		case rt_data_index_type:
			{
				auto& this_val = children[(one.index >> rt_type_shift)];
				if (this_val.is_array())
				{
					std::vector<rx_simple_value> childs;
					for (int i = 0; i < this_val.get_size(); i++)
					{
						rx_simple_value child_data;
						this_val.get_item(i)->collect_value(child_data, type);
						if (!child_data.is_null())
							childs.push_back(std::move(child_data));
					}
					rx_simple_value temp;
					temp.assign_array(childs);
					my_vals.push_back(std::move(temp));
				}
				else
				{
					rx_simple_value child_data;
					this_val.get_item()->collect_value(child_data, type);
					if (!child_data.is_null())
						my_vals.push_back(std::move(child_data));
				}
			}
			break;
		default:
			RX_ASSERT(false);
		}
	}

	if (!my_vals.empty())
		data.assign_static(my_vals);

	return true;
}

rx_result block_data::fill_value (const values::rx_simple_value& data)
{
	rx_result ret = check_value(data.c_ptr()->value_type, data.c_ptr()->value);
	if (ret)
	{
		ret = fill_value_internal(data.c_ptr()->value_type, data.c_ptr()->value);
		RX_ASSERT(ret);
	}
	return ret;
}

rx_result block_data::check_value (const values::rx_simple_value& data)
{
	rx_result ret = check_value(data.c_ptr()->value_type, data.c_ptr()->value);
	return ret;
}

rx_result block_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{// our value
		rx_result result;
		serialization::json_writer writer;
		writer.write_header(STREAMING_TYPE_MESSAGE, 0);
		data::runtime_values_data data;
		collect_data(data, runtime_value_type::simple_runtime_value);
		if (!writer.write_init_values(nullptr, data))
		{
			result = "Error writing values to the stream";
		}
		else
		{
			writer.write_footer();
			string_type temp_str(writer.get_string());
			if (!temp_str.empty())
			{
				val.assign_static(temp_str.c_str(), ctx->now);
				result = true;
			}
			else
			{
				result = "Retrieving string from JSON stream";
			}
		}
		return result;
	}
	else
	{
		string_type mine;
		string_type bellow;
		size_t idx = path.find(RX_OBJECT_DELIMETER);
		if (idx != string_type::npos)
		{
			mine = path.substr(0, idx);
			bellow = path.substr(idx + 1);
			int array_idx = -1;
			auto idx = internal_get_index(mine, array_idx);
			if (idx && is_complex_index(idx))
			{
				switch (idx & rt_type_mask)
				{
				case rt_data_index_type:
					if (array_idx < 0)
					{
						if (!children[idx >> rt_type_shift].is_array())
							return children[idx >> rt_type_shift].get_item()->get_value(bellow, val, ctx);
						else
							return "Not an array!";
					}
					else
					{
						if (!values[idx >> rt_type_shift].is_array())
							return "Not an array";
						else if (array_idx < values[idx >> rt_type_shift].get_size())
							return children[idx >> rt_type_shift].get_item(array_idx)->get_value(bellow, val, ctx);
						else
							return "Out of bounds!";
					}
				default:
					RX_ASSERT(false);
				}
			}
		}
		else// its' ours
		{
			int array_idx = -1;
			auto idx = internal_get_index(path, array_idx);
			if (idx && is_value_index(idx))
			{
				switch (idx & rt_type_mask)
				{
				case rt_const_index_type:
					if (array_idx < 0)
					{
						if (!values[idx >> rt_type_shift].is_array())
						{
							val = values[idx >> rt_type_shift].get_item()->get_value(ctx);
							return true;
						}
						else
						{
							return "Not an array!";
						}

					}
					else
					{
						if (!values[idx >> rt_type_shift].is_array())
						{
							return "Not an array";
						}
						else if (array_idx < values[idx >> rt_type_shift].get_size())
						{
							val = values[idx >> rt_type_shift].get_item(array_idx)->get_value(ctx);
							return true;
						}
						else
						{
							return "Out of bounds!";
						}
					}
				default:
						RX_ASSERT(false);
				}
			}
		}
		return mine + " not found!";
	}
}

void block_data::object_state_changed (runtime_process_context* ctx)
{
	// nothing to do for data
}

rx_result block_data::get_value_ref (string_view_type path, rt_value_ref& ref, bool is_var)
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_view_type mine;
	if (idx != string_view_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_view_type bellow = path.substr(idx + 1);
		int array_idx = -1;
		auto idx = internal_get_index(mine, array_idx);
		if (idx && is_complex_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_data_index_type:
				if (array_idx < 0)
				{
					if (!children[idx >> rt_type_shift].is_array())
						return children[idx >> rt_type_shift].get_item()->get_value_ref(bellow, ref, false);
					else
						return "Invalid array index";
				}
				else
				{
					if (!children[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < children[idx >> rt_type_shift].get_size())
						return children[idx >> rt_type_shift].get_item(array_idx)->get_value_ref(bellow, ref, false);
					else
						return "Out of bounds!";
				}
				break;
			default:
				RX_ASSERT(false);
			}
		}
	}
	else// its' ours
	{
		int array_idx = -1;
		auto idx = internal_get_index(path, array_idx);
		if (idx && is_value_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_const_index_type:
				if (array_idx < 0)
				{
					ref.ref_type = rt_value_ref_type::rt_const_value;
					if (!values[idx >> rt_type_shift].is_array())
						ref.ref_value_ptr.const_value = values[idx >> rt_type_shift].get_item();
					else
						return "Invalid array index";
				}
				else
				{
					ref.ref_type = rt_value_ref_type::rt_const_value;
					if (!values[idx >> rt_type_shift].is_array())
						return "Not an array";
					else if (array_idx < values[idx >> rt_type_shift].get_size())
						ref.ref_value_ptr.const_value = values[idx >> rt_type_shift].get_item(array_idx);
					else
						return "Out of bounds!";
				}
				return true;
			default:
				RX_ASSERT(false);// has to be because of is_value_index;
			}
		}
	}
	return string_type(path) + " not found!";
}

rx_result block_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		for (const auto& one : this->items)
		{
			runtime_item_attribute one_attr;
			switch (one.index & rt_type_mask)
			{
			case rt_const_index_type:
				one_attr.name = one.name;
				if (!values[(one.index >> rt_type_shift)].is_array())
				{
					one_attr.value = values[(one.index >> rt_type_shift)].get_item()->get_value(ctx);
					one_attr.type = rx_attribute_type::const_attribute_type;
				}
				else
				{
					one_attr.type = rx_attribute_type::const_array_attribute_type;
				}
				break;
			case rt_data_index_type:
				one_attr.name = one.name;
				if (!children[(one.index >> rt_type_shift)].is_array())
					one_attr.type = rx_attribute_type::data_attribute_type;
				else
					one_attr.type = rx_attribute_type::data_array_attribute_type;
				break;
			default:
				RX_ASSERT(false);
				one_attr.type = rx_attribute_type::invalid_attribute_type_type;
			}
			if (one_attr.type != rx_attribute_type::invalid_attribute_type_type)
			{
				one_attr.full_path = prefix + path + RX_OBJECT_DELIMETER + one.name;
				items.emplace_back(std::move(one_attr));
			}
		}
		return true;
	}
	else
	{
		size_t idx = path.find(RX_OBJECT_DELIMETER);
		string_type mine;
		string_type bellow;
		if (idx != string_type::npos)
		{// bellow us, split it
			mine = path.substr(0, idx);
			bellow = path.substr(idx + 1);
		}
		else
		{
			mine = path;
		}
		int array_idx = -1;
		auto item_idx = internal_get_index(mine, array_idx);
		if (item_idx)
		{
			if (array_idx < 0)
			{
				string_type prefix_bellow = prefix.empty() ? mine : prefix + RX_OBJECT_DELIMETER + mine;
				switch (item_idx & rt_type_mask)
				{
				case rt_data_index_type:
					if (children[(item_idx >> rt_type_shift)].is_array())
					{
						if (bellow.empty())
						{// browse array
							for (int i = 0; i < children[(item_idx >> rt_type_shift)].get_size(); i++)
							{
								char temp_buff[0x20];
								runtime_item_attribute one_attr;
								sprintf(temp_buff, "[%d]", i);
								one_attr.name = mine + temp_buff;
								one_attr.type = rx_attribute_type::data_attribute_type;
								if (prefix.empty())
									one_attr.full_path = one_attr.name;
								else
									one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
								items.emplace_back(std::move(one_attr));
							}
							return true;
						}
						else
						{// browse bellow
							return children[item_idx >> rt_type_shift].get_item(array_idx)->browse_items(prefix_bellow, bellow, filter, items, ctx);
						}
					}
					else
					{
						return children[item_idx >> rt_type_shift].get_item()->browse_items(prefix_bellow, bellow, filter, items, ctx);
					}
					break;
				case rt_const_index_type:
					if (values[(item_idx >> rt_type_shift)].is_array())
					{
						for (int i = 0; i < values[(item_idx >> rt_type_shift)].get_size(); i++)
						{
							char temp_buff[0x20];
							runtime_item_attribute one_attr;
							sprintf(temp_buff, "[%d]", i);
							one_attr.name = mine + temp_buff;
							one_attr.type = rx_attribute_type::const_attribute_type;
							one_attr.value = values[(item_idx >> rt_type_shift)].get_item(i)->get_value(ctx);
							if (prefix.empty())
								one_attr.full_path = one_attr.name;
							else
								one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
							items.emplace_back(std::move(one_attr));
						}
						return true;
					}
					break;
				default:
					RX_ASSERT(false);

				}
			}
			else
			{
				string_type prefix_bellow = prefix.empty() ? mine : prefix + RX_OBJECT_DELIMETER + mine;
				switch (item_idx & rt_type_mask)
				{
				case rt_data_index_type:
					if (children[(item_idx >> rt_type_shift)].is_array())
					{
						if (children[(item_idx >> rt_type_shift)].get_size() > array_idx)
						{// browse bellow
							return children[item_idx >> rt_type_shift].get_item(array_idx)->browse_items(prefix_bellow, bellow, filter, items, ctx);
						}
						else
						{
							return RX_OUT_OF_BOUNDS;
						}
					}
					else
					{
						return RX_NOT_AN_ARRAY;
					}
					break;
				default:
					RX_ASSERT(false);

				}
			}
		}
		return prefix + path + " not found!";
	}
}

const runtime_item* block_data::get_child_item (string_view_type path) const
{
	if (path.empty())
		return this;

	const runtime_item* ret = nullptr;
	string_view_type mine;
	string_view_type rest;
	auto in = path.find(RX_OBJECT_DELIMETER);
	if (in == string_view_type::npos)
	{
		mine = path;
	}
	else
	{
		mine = path.substr(0, in);
		rest = path.substr(in + 1);
	}
	int array_idx = -1;
	mine = extract_index(mine, array_idx);
	// now compare it
	for (const auto& one : this->items)
	{
		if (is_complex_index(one.index) && one.name == mine)
		{
			switch (one.index & rt_type_mask)
			{
				case rt_data_index_type:
					ret = children[one.index >> rt_type_shift].get_item();
					break;
				default:
					RX_ASSERT(false);
			}
		}
		if (ret)
			break;
	}
	if (ret && !rest.empty())
	{
		// this is not all go bellow
		return ret->get_child_item(rest);
	}
	else
	{
		// this is our's return it
		return ret;
	}
}

rx_result block_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;// nothing to do for data
}

rx_result block_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;// nothing to do for data
}

rx_result block_data::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;// nothing to do for data
}

rx_result block_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;// nothing to do for data
}

runtime_filters_type& block_data::get_filters ()
{
	return g_empty_filters;
}

runtime_sources_type& block_data::get_sources ()
{
	return g_empty_sources;
}

runtime_mappers_type& block_data::get_mappers ()
{
	return g_empty_mappers;
}

runtime_events_type& block_data::get_events ()
{
	return g_empty_events;
}

runtime_variables_type& block_data::get_variables ()
{
	return g_empty_variables;
}

runtime_structs_type& block_data::get_structs ()
{
	return g_empty_structs;
}

rx_result block_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_view_type bellow = path.substr(idx + 1);
		int array_idx = -1;
		auto idx = internal_get_index(mine, array_idx);
		if (idx && is_complex_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_data_index_type:
				if (array_idx < 0 && !children[idx >> rt_type_shift].is_array())
				{
					return children[idx >> rt_type_shift].get_item()->get_local_value(bellow, val);
				}
				else if (array_idx >= 0 && array_idx < children[idx >> rt_type_shift].get_size())
				{
					return children[idx >> rt_type_shift].get_item(array_idx)->get_local_value(bellow, val);
				}
				else
				{
					return "Index out of bounds";
				}

			default:
				RX_ASSERT(false);
			}
		}
	}
	else// its' ours
	{
		int array_idx = -1;
		auto idx = internal_get_index(path, array_idx);
		if (idx && is_value_index(idx))
		{
			switch (idx & rt_type_mask)
			{
			case rt_const_index_type:
				if (array_idx < 0 && !values[idx >> rt_type_shift].is_array())
				{
					val = values[idx >> rt_type_shift].get_item()->simple_get_value();
					return true;
				}
				else if (array_idx >=0 && array_idx < values[idx >> rt_type_shift].get_size())
				{
					val = values[idx >> rt_type_shift].get_item(array_idx)->simple_get_value();
					return true;
				}
				else
				{
					return "Index out of bounds";
				}
			default:
				RX_ASSERT(false);
			}
		}
	}
	return string_type(path) + " not found!";
}

string_view_type block_data::extract_index (string_view_type name, int& idx) const
{
	string_view_type ret;
	if (name.empty())
		return string_view_type();
	if (*name.rbegin() == ']')
	{
		size_t str_idx = name.rfind('[');
		if (str_idx != string_type::npos && str_idx < name.size() - 2)
		{
			uint16_t unsigned_idx;
			string_view_type number = name.substr(str_idx + 1, name.size() - str_idx /*We know this because of the upper condition*/ - 2);
			auto  result = std::from_chars(number.data(), number.data() + number.size(), unsigned_idx);
			if (result.ec != std::errc())
				return ret;//error
			idx = unsigned_idx;
			ret = name.substr(0, str_idx);
		}
		else
		{
			return string_view_type();
		}
	}
	else
	{
		idx = -1;
		ret = name;
	}
	return ret;
}

members_index_type block_data::internal_get_index (string_view_type name, int& idx) const
{
	int item_idx = -1;
	name = extract_index(name, item_idx);
	if (name.empty())
		return 0;
	for (const auto& one : items)
	{
		if (one.name == name)
		{
			idx = item_idx;
			return one.index;
		}
	}
	return 0;
}

bool block_data::is_value_index (members_index_type idx) const
{
	return (idx & rt_type_mask) == rt_const_index_type;
}

bool block_data::is_complex_index (members_index_type idx) const
{
	return (idx & rt_type_mask) == rt_data_index_type;
}

bool block_data::is_this_yours (string_view_type path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	int array_idx = -1;
	if (idx == string_type::npos)
	{
		return internal_get_index(path, array_idx) != 0;
	}
	else
	{
		return internal_get_index(path.substr(0, idx), array_idx) != 0;
	}
	return false;
}

void block_data::read_struct (string_view_type path, read_struct_data data) const
{
}

void block_data::write_struct (string_view_type path, write_struct_data data)
{
}

rx_result block_data::create_safe_runtime_data (const data::runtime_values_data& in, data::runtime_values_data& out)
{
	for (const auto& one : items)
	{
		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			{
				if (!values[(one.index >> rt_type_shift)].is_array())
				{
					auto val = in.get_value(one.name);
					if (val.is_null())
					{
						return "Missing argument "s + one.name;
					}
					string_type temp_name = one.name;

					auto val_type = values[(one.index >> rt_type_shift)].get_item()->value.get_type();
					if (!val.convert_to(val_type))
					{
						return RX_INVALID_CONVERSION " for "s + one.name;
					}
					out.add_value(one.name, std::move(val));
				}
				else
				{
					std::vector<rx_simple_value> vals;
					if (in.get_array_value(one.name, vals))
					{
						size_t dim = vals.size();
						for (size_t i = 0; i < dim; i++)
						{
							values[one.index >> rt_type_shift].get_item((int)i)->set_value(std::move(vals[i]));
						}
					}
					out.add_value(one.name, std::move(vals));
				}
			}
			break;
		case rt_data_index_type:
			{
				if (!children[(one.index >> rt_type_shift)].is_array())
				{
					data::runtime_values_data temp;
					auto it = in.children.find(one.name);
					if (it != in.children.end())
					{
						if (std::holds_alternative<data::runtime_values_data>(it->second))
						{
							auto ret = children[one.index >> rt_type_shift].get_item()->create_safe_runtime_data(std::get< data::runtime_values_data>(it->second), temp);
							if (!ret)
								return ret;
						}
						out.add_child(one.name, std::move(temp));
					}
					else
					{
						return "Missing argument "s + one.name;
					}
				}
				else
				{
					auto it = in.children.find(one.name);
					if (it != in.children.end())
					{
						if (std::holds_alternative<std::vector<data::runtime_values_data> >(it->second))
						{
							auto& this_val = children[(one.index >> rt_type_shift)];
							auto& childs = std::get<std::vector<data::runtime_values_data>>(it->second);
							if (childs.empty())
							{
								RX_ASSERT(this_val.get_prototype());
								auto one_block = *this_val.get_prototype();
								this_val.declare_null_array(std::move(one_block));
							}
							else if ((int)childs.size() == this_val.get_size())
							{
								for (size_t i = 0; i < childs.size(); i++)
								{
									this_val.get_item((int)i)->fill_data(childs[i]);
								}
							}
							else
							{
								const block_data* prototype = nullptr;
								std::vector<block_data> temp;
								for (int j = 0; j < (int)childs.size(); j++)
								{
									if (j < this_val.get_size())
									{
										this_val.get_item(j)->fill_data(childs[j]);
										temp.push_back(*this_val.get_item(j));
									}
									else
									{
										if (prototype == nullptr)
										{
											RX_ASSERT(this_val.get_prototype());
											prototype = this_val.get_prototype();
										}
										block_data one_block(*prototype);
										one_block.fill_data(childs[j]);
										temp.push_back(std::move(one_block));
									}
								}
								this_val = std::move(temp);
							}
						}
					}
				}
			}
		default:
			RX_ASSERT(false);
			return RX_INTERNAL_ERROR;
		}
	}
	return true;
}

rx_result block_data::check_value (rx_value_t val_type, const rx_value_union& data)
{
	if (val_type != RX_STRUCT_TYPE)
		return RX_INVALID_CONVERSION;
	if(data.struct_value.size != items.size())
		return RX_INVALID_CONVERSION;
	for (size_t i = 0; i < data.struct_value.size; i++)
	{
		auto& item = items[i];
		if (data.struct_value.values[i].value_type & RX_ARRAY_VALUE_MASK)
		{// array value
			switch (data.struct_value.values[i].value_type & RX_STRIP_ARRAY_MASK)
			{
			case RX_NULL_TYPE:
				continue;
			case RX_STRUCT_TYPE:
				{
					if ((item.index & rt_type_mask) == rt_data_index_type)
					{
						auto& this_val = children[(item.index >> rt_type_shift)];
						if (!this_val.is_array())
							return RX_INVALID_CONVERSION;

						RX_ASSERT(this_val.get_prototype());
						auto one_block = this_val.get_prototype();
						

						for (int j = 0; j < (int)data.struct_value.values[i].value.array_value.size; j++)
						{
							auto result = one_block->check_value(
								RX_STRUCT_TYPE
								, data.struct_value.values[i].value.array_value.values[j]);

							if (!result)
								return result;
						}
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
				break;
			default:
				{// other values
					if ((item.index & rt_type_mask) == rt_const_index_type)
					{
						auto& this_val = values[(item.index >> rt_type_shift)];
						if (!this_val.is_array())
							return RX_INVALID_CONVERSION;

						RX_ASSERT(this_val.get_prototype());
						auto one_block = this_val.get_prototype();

						for (int j = 0; j < (int)data.struct_value.values[i].value.array_value.size; j++)
						{

							typed_value_type temp;
							rx_get_array_value(j, &temp, &data.struct_value.values[i]);

							rx_simple_value temp_value = temp;
							auto result = temp_value.convert_to(one_block->value.c_ptr()->value_type);

							if (!result)
								return RX_INVALID_CONVERSION;
						}
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
			}
		}
		else
		{// plain value
			switch (data.struct_value.values[i].value_type & RX_STRIP_ARRAY_MASK)
			{
			case RX_NULL_TYPE:
				continue;
			case RX_STRUCT_TYPE:
				{
					if ((item.index & rt_type_mask) == rt_data_index_type)
					{
						auto& this_val = children[(item.index >> rt_type_shift)];
						if (this_val.is_array())
							return RX_INVALID_CONVERSION;
						auto result = this_val.get_item()->check_value(
							data.struct_value.values[i].value_type
							, data.struct_value.values[i].value);
						if (!result)
							return result;
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
				break;
			default:
				{// other values
					if ((item.index & rt_type_mask) == rt_const_index_type)
					{
						auto& this_val = values[(item.index >> rt_type_shift)];
						if (this_val.is_array())
							return RX_INVALID_CONVERSION;


						typed_value_type temp;
						rx_copy_value(&temp, &data.struct_value.values[i]);
						rx_simple_value temp_value = temp;
						auto result = temp_value.convert_to(this_val.get_item()->value.c_ptr()->value_type);
						if (!result)
							return RX_INVALID_CONVERSION;
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
			}
		}		
	}
	return true;
}

rx_result block_data::fill_value_internal (rx_value_t val_type, const rx_value_union& data)
{
	if (val_type != RX_STRUCT_TYPE)
		return RX_INVALID_CONVERSION;
	if (data.struct_value.size != items.size())
		return RX_INVALID_CONVERSION;
	for (size_t i = 0; i < data.struct_value.size; i++)
	{
		auto& item = items[i];
		if (data.struct_value.values[i].value_type & RX_ARRAY_VALUE_MASK)
		{// array value
			switch (data.struct_value.values[i].value_type & RX_STRIP_ARRAY_MASK)
			{
			case RX_NULL_TYPE:
				continue;
			case RX_STRUCT_TYPE:
				{
					if ((item.index & rt_type_mask) == rt_data_index_type)
					{
						auto& this_val = children[(item.index >> rt_type_shift)];
						if (!this_val.is_array())
							return RX_INVALID_CONVERSION;

						if (data.struct_value.values[i].value.array_value.size == 0)
						{
							RX_ASSERT(this_val.get_prototype());
							auto one_block = *this_val.get_prototype();
							this_val.declare_null_array(std::move(one_block));
						}
						else if (this_val.get_size() == (int)data.struct_value.values[i].value.array_value.size)
						{
							for (int j = 0; j < this_val.get_size(); j++)
							{
								auto result = this_val.get_item(j)->fill_value_internal(
									RX_STRUCT_TYPE
									, data.struct_value.values[i].value.array_value.values[j]);

								if (!result)
									return result;
							}
						}
						else// if (this_val.get_size() == 0)
						{
							const block_data* prototype = nullptr;
							std::vector<block_data> temp;
							for (int j = 0; j < (int)data.struct_value.values[i].value.array_value.size; j++)
							{
								if (j < this_val.get_size())
								{
									auto result = this_val.get_item(j)->fill_value_internal(
										RX_STRUCT_TYPE
										, data.struct_value.values[i].value.array_value.values[j]);

									if (!result)
										return result;

									temp.push_back(*this_val.get_item(j));
								}
								else
								{
									if (prototype == nullptr)
									{
										RX_ASSERT(this_val.get_prototype());
										prototype = this_val.get_prototype();
									}
									block_data one_block(std::move(*prototype));
									auto result = one_block.fill_value_internal(
										RX_STRUCT_TYPE
										, data.struct_value.values[i].value.array_value.values[j]);

									if (!result)
										return result;
									temp.push_back(std::move(one_block));
								}
							}
							this_val = std::move(temp);
							return true;
						}
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
				break;
			default:
				{// other values
					if ((item.index & rt_type_mask) == rt_const_index_type)
					{
						auto& this_val = values[(item.index >> rt_type_shift)];
						if (!this_val.is_array())
							return RX_INVALID_CONVERSION;

						if (this_val.get_size() == (int)data.struct_value.values[i].value.array_value.size)
						{
							for (int j = 0; j < this_val.get_size(); j++)
							{
								typed_value_type temp;
								rx_get_array_value(j, &temp, &data.struct_value.values[i]);

								auto result = this_val.get_item(j)->set_value(temp);
						//		rx_destroy_value(&temp);
								if (!result)
									return result;
							}
						}
						else
						{
							return RX_NOT_IMPLEMENTED;
						}
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
			}
		}
		else
		{// plain value
			switch (data.struct_value.values[i].value_type & RX_STRIP_ARRAY_MASK)
			{
			case RX_NULL_TYPE:
				continue;
			case RX_STRUCT_TYPE:
				{
					if ((item.index & rt_type_mask) == rt_data_index_type)
					{
						auto& this_val = children[(item.index >> rt_type_shift)];
						if (this_val.is_array())
							return RX_INVALID_CONVERSION;
						auto result = this_val.get_item()->fill_value_internal(
							data.struct_value.values[i].value_type
							, data.struct_value.values[i].value);
						if (!result)
							return result;
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
				break;
			default:
				{// other values
					if ((item.index & rt_type_mask) == rt_const_index_type)
					{
						auto& this_val = values[(item.index >> rt_type_shift)];
						if (this_val.is_array())
							return RX_INVALID_CONVERSION;


						typed_value_type temp;
						rx_copy_value(&temp, &data.struct_value.values[i]);

						auto result = this_val.get_item()->set_value(temp);
						if (!result)
							return result;
					}
					else
					{
						return RX_INVALID_CONVERSION;
					}
				}
			}
		}
	}
	return true;
}

data::runtime_data_model block_data::create_runtime_model ()
{
	data::runtime_data_model ret;
	for (const auto& one : items)
	{
		auto element = std::make_unique<data::runtime_model_element>();
		element->name = one.name;

		switch (one.index & rt_type_mask)
		{
		case rt_const_index_type:
			// simple value
			{
				auto& this_val = values[(one.index >> rt_type_shift)];
				if (this_val.is_array())
				{
					std::vector<rx_simple_value> vals;
					for (int i = 0; i < this_val.get_size(); i++)
						vals.push_back(this_val.get_item(i)->value);
					rx_simple_value temp;
					temp.assign_array(vals);
					element->value = std::move(temp);
				}
				else
				{
					element->value = this_val.get_item()->value;
				}
			}
			break;
		case rt_data_index_type:
			{
				auto& this_val = children[(one.index >> rt_type_shift)];
				if (this_val.is_array())
				{
					std::vector<data::runtime_data_model> temp_array;

					for (int i = 0; i < this_val.get_size(); i++)
					{
						temp_array.push_back(this_val.get_item(i)->create_runtime_model());
					}
					element->value = std::move(temp_array);
				}
				else
				{
					element->value = this_val.get_item()->create_runtime_model();
				}
			}
			break;
		default:
			RX_ASSERT(false);
		}
		ret.elements.push_back(std::move(element));
	}
	return ret;
}


// Class rx_platform::runtime::structure::full_value_data 


rx_value full_value_data::get_value (runtime_process_context* ctx) const
{
	//if (ctx)
	//	return ctx->adapt_value(value);
	//else
		return value;
}

void full_value_data::set_value (const rx_value& val, runtime_process_context* ctx)
{
	rx_value temp(val);
	if (temp.convert_to(value.get_type()))
	{
		value = std::move(temp);
		ctx->full_value_changed(this);
	}
}

rx_simple_value full_value_data::simple_get_value () const
{
	return value.to_simple();
}


// Class rx_platform::runtime::structure::execute_task 

execute_task::~execute_task()
{
}



// Parameterized Class rx_platform::runtime::structure::array_wrapper 


// Class rx_platform::runtime::structure::value_block_data 

string_type value_block_data::type_name = RX_CPP_STRUCT_TYPE_NAME;


void value_block_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	if (type != runtime_value_type::persistent_runtime_value)
	{
		block.collect_data(data, type);
	}
	else
	{
		if (!struct_value.value_opt[opt_is_constant] && struct_value.value_opt[runtime::structure::value_opt_persistent])
			block.collect_data(data, type);
	}
}

void value_block_data::fill_data (const data::runtime_values_data& data)
{
	block.fill_data(data);
}

rx_result value_block_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	data = struct_value.value.to_simple();
	return true;
}

rx_result value_block_data::fill_value (const values::rx_simple_value& data)
{
	return RX_NOT_SUPPORTED;
}

rx_result value_block_data::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.push_item(block);

	auto ret = block.initialize_runtime(ctx);
	if (ret)
	{
		timestamp = ctx.now;
		rx_simple_value val;
		auto ret = block.collect_value(val, runtime_value_type::simple_runtime_value);
		if (ret)
		{
			struct_value.value = rx_timed_value(std::move(val), ctx.now);
			ctx.bind_item(ctx.path.get_current_path(), tag_blocks::binded_callback_t(), [this](rx_simple_value& val, data::runtime_values_data* data, runtime::runtime_process_context* ctx)
				{
					return do_write_callback(val, data, ctx);
				});
		}
	}
	ctx.structure.pop_item();
	return ret;
}

rx_result value_block_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return block.deinitialize_runtime(ctx);
}

rx_result value_block_data::start_runtime (runtime::runtime_start_context& ctx)
{
	return block.start_runtime(ctx);
}

rx_result value_block_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return block.stop_runtime(ctx);
}

rx_result value_block_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	auto result = block.get_value(path, val, ctx);
	if (result && val.get_time() < timestamp)
		val.set_time(timestamp);
	return result;
}

rx_result value_block_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (path.empty())
	{
		ref.ref_type = rt_value_ref_type::rt_value;
		ref.ref_value_ptr.value = &struct_value;
		return true;
	}
	else
	{
		if (struct_value.value_opt[opt_is_constant])
		{
			return block.get_value_ref(path, ref, false);
		}
		else
		{
			return get_tag_references()->get_value_ref(block, path, ref, struct_value.value.get_time());
		}
	}
}

rx_result value_block_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	return block.browse_items(prefix, path, filter, items, ctx);
}

const runtime_item* value_block_data::get_child_item (string_view_type path) const
{
	return block.get_child_item(path);
}

rx_result value_block_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	return block.get_local_value(path, val);
}

void value_block_data::object_state_changed (runtime_process_context* ctx)
{
	if (ctx->get_mode_time() > timestamp)
		timestamp = ctx->get_mode_time();
}

rx_value value_block_data::get_value (runtime_process_context* ctx) const
{
	if (ctx)
		return ctx->adapt_value(struct_value.value);
	else
		return struct_value.value;
}

block_data_references* value_block_data::get_tag_references ()
{
	if (!tag_references_)
		tag_references_ = std::make_unique<block_data_references>();
	return tag_references_.get();
}

rx_result value_block_data::do_write_callback (rx_simple_value& val, data::runtime_values_data* data, runtime::runtime_process_context* ctx)
{
	rx_result ret = struct_value.check_set_value(ctx, false, ctx->get_mode().is_test());
	if (ret)
	{
		if (data)
		{
			block_data temp = block;
			temp.fill_data(*data);
			rx_simple_value temp_val;
			ret = temp.collect_value(temp_val, runtime_value_type::simple_runtime_value);
			if (ret)
			{
				block = std::move(temp);
				rx_simple_value temp_val;
				ret = block.collect_value(temp_val, runtime_value_type::simple_runtime_value);
				if (ret)
				{
					if (tag_references_)
						tag_references_->block_data_changed(block, ctx);
					val = temp_val;
				}
			}
		}
		else
		{
			ret = block.fill_value(val);
			if (ret)
			{
				rx_simple_value temp_val;
				ret = block.collect_value(temp_val, runtime_value_type::simple_runtime_value);
				if (ret)
				{
					if (tag_references_)
						tag_references_->block_data_changed(block, ctx);
					val = temp_val;
				}
			}
		}
	}
	return ret;
}


// Class rx_platform::runtime::structure::variable_block_data 

string_type variable_block_data::type_name = RX_CPP_STRUCT_TYPE_NAME;


void variable_block_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	block.collect_data(data, type);
	if (type != runtime_value_type::simple_runtime_value)
		variable.collect_data(data, type);
}

void variable_block_data::fill_data (const data::runtime_values_data& data)
{
	block.fill_data(data);
	variable.fill_data(data);
}

rx_result variable_block_data::collect_value (values::rx_simple_value& data, runtime_value_type type) const
{
	data = variable.value.to_simple();
	return true;
}

rx_result variable_block_data::fill_value (const values::rx_simple_value& data)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result variable_block_data::initialize_runtime (runtime::runtime_init_context& ctx)
{

	ctx.structure.push_item(block);

	rx_simple_value val;
	auto ret = block.collect_value(val, runtime_value_type::simple_runtime_value);
	if (ret)
	{
		variable.value = rx_value(std::move(val), ctx.now);
		ctx.bind_item(ctx.path.get_current_path(), tag_blocks::binded_callback_t(), [this](rx_simple_value& val, data::runtime_values_data* data, runtime::runtime_process_context* ctx)
			{
				return do_write_callback(val, data, ctx);
			});
	}

	auto result = block.initialize_runtime(ctx);
	if (!result)
		return result;
	ctx.structure.pop_item();

	ctx.variables.push_variable(this);

	result= variable.internal_initialize_runtime(ctx, true);
	if (!result)
		return result;

	ctx.variables.pop_variable();
	return true;
}

rx_result variable_block_data::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	ctx.variables.push_variable(this);

	auto result = variable.internal_deinitialize_runtime(ctx, true);
	if (!result)
		return result;

	ctx.variables.pop_variable();

	result = block.deinitialize_runtime(ctx);
	if (!result)
		return result;

	return true;
}

rx_result variable_block_data::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = block.start_runtime(ctx);
	if (!result)
		return result;

	ctx.variables.push_variable(this);

	result = variable.internal_start_runtime(ctx, true);
	if (!result)
		return result;

	ctx.variables.pop_variable();

	return true;
}

rx_result variable_block_data::stop_runtime (runtime::runtime_stop_context& ctx)
{
	ctx.variables.push_variable(this);

	auto result = variable.internal_stop_runtime(ctx, true);
	if (!result)
		return result;

	ctx.variables.pop_variable();

	result = block.stop_runtime(ctx);
	if (!result)
		return result;
	return true;
}

rx_result variable_block_data::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	if (path.empty())
	{
		val = variable.get_value(ctx);
		return true;
	}
	else
	{
		return block.get_value(path, val, ctx);
	}
}

rx_result variable_block_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	if (!path.empty())
	{
		auto ret = get_tag_references()->get_value_ref(block, path, ref
			, variable.value.get_time()
			, variable.value.get_quality()
			, variable.value.get_origin());
		if(!ret)
			ret = variable.get_value_ref(path, ref);
		
		return ret;

	}
	return variable.get_value_ref(path, ref);
}

rx_result variable_block_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx) const
{
	if (!path.empty())
	{
		auto result = block.browse_items(prefix, path, filter, items, ctx);
		if (!result)
			return variable.browse_items(prefix, path, filter, items, ctx);
		return result;
	}
	else
	{
		auto result = block.browse_items(prefix, path, filter, items, ctx);
		if (result)
			result = variable.browse_items(prefix, path, filter, items, ctx);;
		return result;
	}
}

const runtime_item* variable_block_data::get_child_item (string_view_type path) const
{
	const runtime_item* ret = nullptr;
	ret = block.get_child_item(path);
	if (ret == nullptr)
	{
		ret = variable.get_child_item(path);
	}
	return ret;
}

rx_result variable_block_data::get_local_value (string_view_type path, rx_simple_value& val) const
{
	if (path.empty())
	{
		val = variable.simple_get_value();
		return true;
	}
	else
	{
		auto result = block.get_local_value(path, val);
		if (!result)
			return variable.get_local_value(path, val);
		return result;
	}
}

rx_value variable_block_data::get_value (runtime_process_context* ctx) const
{
	return variable.get_value(ctx);
}

rx_simple_value variable_block_data::simple_get_value () const
{
	return variable.simple_get_value();
}

rx_result variable_block_data::set_value (rx_simple_value&& val)
{
	return RX_NOT_IMPLEMENTED;// struct_value.set_value(std::move(val));
}

void variable_block_data::object_state_changed (runtime_process_context* ctx)
{
	block.object_state_changed(ctx);
	variable.object_state_changed(ctx);
}

variable_block_data_references* variable_block_data::get_tag_references ()
{
	if (!tag_references_)
		tag_references_ = std::make_unique<variable_block_data_references>();
	return tag_references_.get();
}

rx_result variable_block_data::do_write_callback (rx_simple_value& val, data::runtime_values_data* data, runtime::runtime_process_context* ctx)
{
	auto ret = block.fill_value(val);
	if (ret)
	{
		rx_simple_value temp_val;
		ret = block.collect_value(temp_val, runtime_value_type::simple_runtime_value);
		if (ret)
		{
			val = temp_val;
		}
	}
	return ret;
}

rx_result variable_block_data::write_value (write_data&& data, write_task* task, runtime_process_context* ctx)
{
	auto ret = block.fill_value(data.value);
	if (ret)
	{
		rx_simple_value temp_val;
		ret = block.collect_value(temp_val, runtime_value_type::simple_runtime_value);
		if (ret)
		{
			data.value = temp_val;
			ret = variable.write_value(std::move(data), task, ctx);
		}
	}
	return ret;
}

void variable_block_data::process_result (runtime_transaction_id_t id, rx_result&& result)
{
	variable.process_result(id, std::move(result));
}

void variable_block_data::process_runtime (runtime_process_context* ctx)
{
	rx_value prepared_value;
	if (variable.prepare_value(prepared_value, ctx))
	{
		auto ret = block.fill_value(prepared_value.to_simple());
		if (!ret)
		{
			prepared_value = variable.value;
			prepared_value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
		}
		variable.update_prepared(std::move(prepared_value), ctx);

		get_tag_references()->block_data_changed(block, ctx
			, variable.value.get_quality()
			, variable.value.get_origin());
	}
}


// Class rx_platform::runtime::structure::block_data_references 


rx_result block_data_references::get_value_ref (block_data& data, string_view_type path, rt_value_ref& ref, rx_time ts)
{
	rt_value_ref temp_ref;
	temp_ref.ref_type = rt_value_ref_type::rt_null;

	auto it = references_.find(string_type(path));
	if (it != references_.end())
	{
		ref.ref_type = rt_value_ref_type::rt_value;
		ref.ref_value_ptr.value = it->second.get();
		return true;
	}

	auto result = data.get_value_ref(path, temp_ref, false);
	if (result)
	{
		if (temp_ref.ref_type == rt_value_ref_type::rt_const_value)
		{
			std::unique_ptr<value_data> new_value = std::make_unique< value_data>();
			new_value->value = rx_timed_value(temp_ref.ref_value_ptr.const_value->value, ts);
			new_value->value_opt[opt_is_constant] = true;
			ref.ref_type = rt_value_ref_type::rt_value;
			ref.ref_value_ptr.value = new_value.get();
			references_.emplace(path, std::move(new_value));

			return true;
		}
	}
	// we didn't found it so add dummy reference just in case
	std::unique_ptr<value_data> new_value = std::make_unique< value_data>();
	new_value->value.set_time(ts);
	new_value->value_opt[opt_is_constant] = true;
	ref.ref_type = rt_value_ref_type::rt_value;
	ref.ref_value_ptr.value = new_value.get();
	references_.emplace(path, std::move(new_value));

	return true;
}

void block_data_references::block_data_changed (const block_data& data, runtime::runtime_process_context* ctx)
{
	for (auto& one : references_)
	{
		rx_simple_value temp;
		bool changed = false;
		rx_timed_value temp_val;
		temp_val.set_time(ctx->now);
		auto result = data.get_local_value(one.first, temp);
		if (result)
		{
			temp_val = rx_timed_value(std::move(temp), ctx->now);
		}
		if (!one.second->value.compare(temp_val, time_compare_type::skip))
		{
			one.second->value = std::move(temp_val);
			changed = true;
		}		
		if(changed)
			ctx->value_changed(one.second.get());
	}
}


// Class rx_platform::runtime::structure::variable_block_data_references 


rx_result variable_block_data_references::get_value_ref (block_data& data, string_view_type path, rt_value_ref& ref, rx_time ts, uint32_t quality, uint32_t origin)
{
	rt_value_ref temp_ref;
	temp_ref.ref_type = rt_value_ref_type::rt_null;

	auto it = references_.find(string_type(path));
	if (it != references_.end())
	{
		ref.ref_type = rt_value_ref_type::rt_full_value;
		ref.ref_value_ptr.full_value = it->second.get();
		return true;
	}

	auto result = data.get_value_ref(path, temp_ref, false);
	if (result)
	{
		if (temp_ref.ref_type == rt_value_ref_type::rt_const_value)
		{
			std::unique_ptr<full_value_data> new_value = std::make_unique<full_value_data>();
			new_value->value = rx_value(temp_ref.ref_value_ptr.const_value->value, ts, quality);
			new_value->value.set_origin(origin);
			ref.ref_type = rt_value_ref_type::rt_full_value;
			ref.ref_value_ptr.full_value = new_value.get();
			references_.emplace(path, std::move(new_value));

			return true;
		}
	}
	return RX_INVALID_PATH;
}

void variable_block_data_references::block_data_changed (const block_data& data, runtime::runtime_process_context* ctx, uint32_t quality, uint32_t origin)
{
	for (auto& one : references_)
	{
		rx_simple_value temp;
		bool changed = false;
		rx_value temp_val;;
		temp_val.set_time(ctx->now);
		temp_val.set_origin(origin);
		temp_val.set_quality(quality);
		auto result = data.get_local_value(one.first, temp);
		if (result)
		{
			temp_val = rx_value(std::move(temp), ctx->now, quality);
			temp_val.set_origin(origin);
		}
		if (!one.second->value.compare(temp_val, time_compare_type::skip))
		{
			one.second->value = std::move(temp_val);
			changed = true;
		}
		if (changed)
			ctx->full_value_changed(one.second.get());
	}
}

//
//template class array_wrapper<value_data>;
//template class array_wrapper<const_value_data>;
//template class array_wrapper<value_block_data>;
//template class array_wrapper<variable_block_data>;
//template class array_wrapper<struct_data>;
//template class array_wrapper<variable_data>;
//template class array_wrapper<block_data>;

// Class rx_platform::runtime::structure::mapper_execute_task 

mapper_execute_task::mapper_execute_task (mapper_data* my_mapper, runtime_transaction_id_t trans_id)
      : my_mapper_(my_mapper),
        transaction_id_(trans_id)
{
}



runtime_transaction_id_t mapper_execute_task::get_id () const
{
	return transaction_id_;
}

void mapper_execute_task::process_result (rx_result&& result, values::rx_simple_value&& data)
{
	my_mapper_->process_execute_result(std::move(result), std::move(data), transaction_id_);
}

void mapper_execute_task::process_result (rx_result&& result, data::runtime_values_data&& data)
{
	//my_mapper_->process_execute_result(std::move(result), std::move(data), transaction_id_);
}


} // namespace structure
} // namespace runtime
} // namespace rx_platform

