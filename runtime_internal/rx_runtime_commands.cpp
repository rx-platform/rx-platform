

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


namespace sys_runtime {

namespace runtime_commands {

// Class sys_runtime::runtime_commands::read_command 

read_command::read_command()
	: runtime_command_base("read")
{
}


read_command::~read_command()
{
}



bool read_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, std::ostream& out, std::ostream& err)
{
	rx_value value;
	auto result = rt_item->read_value(sub_item, value);
	if (result)
	{
		out << sub_item << " = ";
		value.dump_to_stream(out);
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


// Class sys_runtime::runtime_commands::pull_command 

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


// Class sys_runtime::runtime_commands::write_command 

write_command::write_command()
	: terminal::commands::server_command("write")
{
}


write_command::~write_command()
{
}



bool write_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{

	string_type full_path;
	string_type val_str;
	in >> full_path;
	in >> val_str;
	if (full_path.empty())
	{
		err << "Empty path!";
		return false;
	}
	string_type object_path;
	string_type item_path;
	rx_split_item_path(full_path, object_path, item_path);
	auto item = ctx->get_current_directory()->get_sub_item(object_path);
	if (!item)
	{
		err << object_path << " not found!";
		return false;
	}

	err << RX_NOT_IMPLEMENTED;
	return false;
	/*rx_value val;
	rx_simple_value to_write;
	to_write.parse(val_str);
	if (to_write.is_null())
	{
		err << "Nothing to write!";
		return false;
	}
	else
	{
		api::rx_context rx_ctx;
		rx_ctx.object = ctx->get_client();
		rx_ctx.directory = ctx->get_current_directory();
		auto result = item->write_value(item_path, std::move(to_write), [ctx, full_path, val_str](rx_result callback_ret)
			{
				if (!callback_ret)
				{
					auto& err = ctx->get_stderr();
					rx_dump_error_result(err, std::move(callback_ret));
				}
				else
				{
					auto& out = ctx->get_stdout();
					out << full_path << " <= "
						<< ANSI_RX_GOOD_COLOR
						<< val_str
						<< ANSI_COLOR_RESET "\r\n";
				}
				ctx->send_results(callback_ret);
			}, rx_ctx);

		if (!result)
		{
			dump_error_result(err, result);
			return false;
		}
		else
		{
			ctx->set_waiting();
			return true;
		}
	}*/
}


// Class sys_runtime::runtime_commands::turn_on_command 

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


// Class sys_runtime::runtime_commands::turn_off_command 

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


// Class sys_runtime::runtime_commands::browse_command 

browse_command::browse_command()
	: runtime_command_base("brw")
{
}


browse_command::~browse_command()
{
}



bool browse_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, std::ostream& out, std::ostream& err)
{
	std::vector<runtime_item_attribute> items;
	auto result = rt_item->browse("", sub_item, "", items);
	if (result)
	{
		rx_table_type table(items.size() + 1);
		size_t idx = 0;
		table[0].emplace_back("Name");
		table[0].emplace_back("Type");
		for (const auto& one : items)
		{
			idx++;
			if (one.is_complex())
				table[idx].emplace_back(one.name, ANSI_RX_DIR_COLOR, ANSI_COLOR_RESET);
			else
				table[idx].emplace_back(one.name, ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET);
			table[idx].emplace_back(rx_runtime_attribute_type_name(one.type));
		}
		rx_dump_table(table, out, true, true);
		return true;
	}
	else
	{
		dump_error_result(err, result);
		return false;
	}
}


// Class sys_runtime::runtime_commands::runtime_command_base 

runtime_command_base::runtime_command_base (const string_type& name)
	: server_command(name)
{
}



bool runtime_command_base::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type full_path;
	in >> full_path;
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
			, [ctx, item_path, this](rx_result_with<platform_item_ptr>&& data) -> bool
			{
				auto& out = ctx->get_stdout();
				auto& err = ctx->get_stderr();
				if (data)
				{
					return do_with_item(data.move_value(), item_path, out, err);
				}
				else
				{
					dump_error_result(err, data);
					return false;
				}
			}, [ctx](bool&& result) mutable
			{
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

