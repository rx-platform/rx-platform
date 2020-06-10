

/****************************************************************************
*
*  runtime_internal\rx_runtime_commands.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_runtime_commands
#include "runtime_internal/rx_runtime_commands.h"

#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"


namespace rx_internal {

namespace sys_runtime {

namespace runtime_commands {

// Class rx_internal::sys_runtime::runtime_commands::read_command

read_command::read_command()
	: runtime_command_base("read")
{
}


read_command::~read_command()
{
}



bool read_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, rx_simple_value&& value, console_context_ptr ctx, std::ostream& out, std::ostream& err)
{
	rx_value my_value;
	auto result = rt_item->read_value(sub_item, my_value);
	if (result)
	{
		out << rt_item->get_name();
		if (!sub_item.empty())
		{
			out << ".";
			out << sub_item;
		}
		out << " = ";
		rx_dump_value(my_value, out, false);
		return true;
	}
	else
	{
		out << "Error reading item "
			<< sub_item;
		for (const auto& one : result.errors())
			out << "\r\n" << one;
		return false;
	}
}


// Class rx_internal::sys_runtime::runtime_commands::pull_command

pull_command::pull_command()
	: terminal::commands::server_command("pull")
{
}


pull_command::~pull_command()
{
}



bool pull_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	out << "Hello from pull!!!\r\n";
	err << RX_NOT_IMPLEMENTED;
	return false;
}


// Class rx_internal::sys_runtime::runtime_commands::write_command

write_command::write_command()
	: runtime_command_base("write")
{
}


write_command::~write_command()
{
}



bool write_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, rx_simple_value&& value, console_context_ptr ctx, std::ostream& out, std::ostream& err)
{
	rx_simple_value my_copy;
	auto now = rx_time::now();
	out << "Write prepared. \r\n";
	out << sub_item << " <= "
		<< ANSI_RX_GOOD_COLOR;
	rx_dump_value(value, out, true);
	out << ANSI_COLOR_RESET "\r\n";
	out << "Start time: " << now.get_string() << "\r\n";
	uint64_t us1 = rx_get_us_ticks();
	auto result = rt_item->write_value(sub_item, std::move(value), [ctx, this, sub_item, value, my_copy, us1](rx_result result)
		{
		///////////////////////////////////////////////
			std::function<void(rx_result , string_type, rx_simple_value, console_context_ptr)> send_func =
				[us1, this](rx_result result, string_type full_path, rx_simple_value value, console_context_ptr ctx) -> void
				{
					uint64_t us2 = rx_get_us_ticks() - us1;
					auto& out = ctx->get_stdout();
					auto& err = ctx->get_stderr();
					if (result)
					{
						out << "Write to "
							<< full_path 
							<< " " ANSI_RX_GOOD_COLOR "succeeded" ANSI_COLOR_RESET ". \r\n";
						out << "Time elapsed: " ANSI_RX_GOOD_COLOR << us2 << ANSI_COLOR_RESET " us\r\n";
					}
					else
					{
						out << "Write " ANSI_COLOR_BOLD ANSI_COLOR_GREEN "failed" ANSI_COLOR_RESET ". \r\n";
						dump_error_result(err, result);
					}
					ctx->get_client()->process_event(result, ctx->get_out(), ctx->get_err(), true);
				};
			rx_post_function_to(ctx->get_executer()
                ,send_func , smart_this(), std::move(result), sub_item, my_copy, ctx);
        //////////////////////////////////////////////

		}, ctx->create_api_context());
	if (result)
	{		
		ctx->set_waiting();
		return true;
	}
	else
	{
		out << "Error writing item "
			<< sub_item << "\r\n";
		result.errors_line();
		return false;
	}
}


// Class rx_internal::sys_runtime::runtime_commands::turn_on_command

turn_on_command::turn_on_command()
	: terminal::commands::server_command("turn-on")
{
}


turn_on_command::~turn_on_command()
{
}



bool turn_on_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type object_path;
	in >> object_path;
	if (object_path.empty())
	{
		err << "Empty path!";
		return false;
	}
	auto item = ctx->get_current_directory()->get_sub_item(object_path);
	if (!item)
	{
		err << object_path << " not found!";
		return false;
	}
	err << RX_NOT_IMPLEMENTED;
	return false;
	/*rx_value val;
	auto result = item->do_command(rx_object_command_t::rx_turn_on);
	if (!result)
	{
		dump_error_result(err, result);
		return false;
	}
	return true;*/
}


// Class rx_internal::sys_runtime::runtime_commands::turn_off_command

turn_off_command::turn_off_command()
	: terminal::commands::server_command("turn-off")
{
}


turn_off_command::~turn_off_command()
{
}



bool turn_off_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type object_path;
	in >> object_path;
	if (object_path.empty())
	{
		err << "Empty path!";
		return false;
	}
	auto item = ctx->get_current_directory()->get_sub_item(object_path);
	if (!item)
	{
		err << object_path << " not found!";
		return false;
	}
	err << RX_NOT_IMPLEMENTED;
	return false;
	/*rx_value val;
	auto result = item->do_command(rx_object_command_t::rx_turn_off);
	if (!result)
	{
		dump_error_result(err, result);
		return false;
	}
	return true;*/
}


// Class rx_internal::sys_runtime::runtime_commands::browse_command

browse_command::browse_command()
	: runtime_command_base("brw")
{
}


browse_command::~browse_command()
{
}



bool browse_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, rx_simple_value&& value, console_context_ptr ctx, std::ostream& out, std::ostream& err)
{
	std::vector<runtime_item_attribute> items;
	auto result = rt_item->browse("", sub_item, "", items);
	if (result)
	{
		rx_table_type table(items.size() + 1);
		size_t idx = 0;
		table[0].emplace_back("Name");
		table[0].emplace_back("Value");
		table[0].emplace_back("Type");
		idx++;
		for (const auto& one : items)
		{
			string_type postfix;
			string_type value = one.value.get_storage().to_string();
			if(one.value.is_null())
				value = RX_TERMINAL_STRUCT_SYMBOL;
			switch (one.type)
			{
			case rx_attribute_type::const_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_CONST_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::value_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_VALUE_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::variable_attribute_type:
				//postfix = RX_TERMINAL_STRUCT_SYMBOL;
				table[idx].emplace_back(one.name, ANSI_RX_VARIABLE_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::struct_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_STRUCT_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::filter_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_FILTER_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::event_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_EVENT_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::source_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_SOURCE_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::mapper_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_MAPPER_COLOR, ANSI_COLOR_RESET);
				break;
			case rx_attribute_type::relation_attribute_type:
				table[idx].emplace_back(one.name, ANSI_RX_RELATION_COLOR, ANSI_COLOR_RESET);
				if (one.value.get_type() == RX_STRING_TYPE)
					value = "->"s + one.value.get_storage().get_string_value();
				else if (one.value.is_null())
					value = "->";
				else
					value = "->"s + one.value.get_storage().to_string();
				break;
			default:
				table[idx].emplace_back(one.name);
				break;
			}
			table[idx].emplace_back(value, ANSI_COLOR_WHITE ANSI_COLOR_BOLD, ANSI_COLOR_RESET);
			if(!postfix.empty())
				table[idx].emplace_back(postfix + rx_runtime_attribute_type_name(one.type));
			else
				table[idx].emplace_back(/*string_type(RX_TERMINAL_STRUCT_SYMBOL_SIZE, ' ') + */rx_runtime_attribute_type_name(one.type));
			idx++;
		}
		rx_dump_table(table, out, true, false);
		return true;
	}
	else
	{
		dump_error_result(err, result);
		return false;
	}
}


// Class rx_internal::sys_runtime::runtime_commands::runtime_command_base

runtime_command_base::runtime_command_base (const string_type& name)
	: server_command(name)
{
}



bool runtime_command_base::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type full_path;
	string_type val_str;
	rx_simple_value to_write;
	in >> full_path;
	if (!in.eof())
	{
		in >> val_str;
		to_write.parse(val_str);
	}
	if (full_path.empty())
	{
		err << "Empty path!";
		return false;
	}
	string_type whose;
	string_type item_path;
	rx_split_item_path(full_path, whose, item_path);
	if (!whose.empty())
	{
		string_type path;
		ctx->get_current_directory()->fill_path(path);
		rx_directory_resolver directories;
		directories.add_paths({ path });
		api::rx_context context;
		context.directory = ctx->get_current_directory();
		context.object = smart_this();

		auto resolve_result = api::ns::rx_resolve_reference(whose, directories);
		if (!resolve_result)
		{
			dump_error_result(err, resolve_result);
			return resolve_result;

		}
		rx_result result = model::algorithms::do_with_runtime_item<bool>(resolve_result.value()
			, [ctx, item_path, to_write, this](rx_result_with<platform_item_ptr>&& data) mutable -> bool
			{
				ctx->postpone_done();
				auto& out = ctx->get_stdout();
				auto& err = ctx->get_stderr();
				if (data)
				{
					return this->do_with_item(data.move_value(), item_path, std::move(to_write), ctx, out, err);
				}
				else
				{
					dump_error_result(err, data);
					return false;
				}
			}, [ctx](bool&& result) mutable
			{
				if (!ctx->is_postponed())
					ctx->get_client()->process_event(result, ctx->get_out(), ctx->get_err(), true);
			}, context);
		if (result)
		{
			ctx->set_waiting();
		}
		return result;
	}
	else
	{
		err << "Please, define item!";
		return false;
	}
}


} // namespace runtime_commands
} // namespace sys_runtime
} // namespace rx_internal

