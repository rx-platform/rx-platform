

/****************************************************************************
*
*  runtime_internal\rx_runtime_commands.cpp
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


// rx_runtime_commands
#include "runtime_internal/rx_runtime_commands.h"



namespace sys_runtime {

namespace runtime_commands {

// Class sys_runtime::runtime_commands::read_command 

read_command::read_command()
	: terminal::commands::server_command("read")
{
}


read_command::~read_command()
{
}



bool read_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type full_path;
	in >> full_path;
	if (full_path.empty())
	{
		err << "Empty path!";
		return false;
	}
	string_type object_path;
	string_type item_path;
	split_item_path(full_path, object_path, item_path);
	auto item = ctx->get_current_directory()->get_sub_item(object_path);
	if (!item)
	{
		err << object_path << " not found!";
		return false;
	}
	rx_value val;
	auto result = item->read_value(item_path, val);
	if (!result)
	{
		dump_error_result(err, result);
		return false;
	}
	out << full_path << " = ";
	if (val.is_good())
		out << ANSI_RX_GOOD_COLOR;
	else if (val.is_uncertain())
		out << ANSI_RX_UNCERTAIN_COLOR;
	else
		out << ANSI_RX_BAD_COLOR;
	val.dump_to_stream(out);
	out	<< ANSI_COLOR_RESET "\r\n";
	return true;
}


// Class sys_runtime::runtime_commands::pull_command 

pull_command::pull_command()
	: terminal::commands::server_command("pull")
{
}


pull_command::~pull_command()
{
}



bool pull_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	out << "Hello from pull!!!\r\n";
	err << "Not implemented yet!!!";
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



bool write_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	rx_reference<write_data_t> data = ctx->get_instruction_data<write_data_t>();
	if (!data)
	{// we just entered to command
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
		split_item_path(full_path, object_path, item_path);
		auto item = ctx->get_current_directory()->get_sub_item(object_path);
		if (!item)
		{
			err << object_path << " not found!";
			return false;
		}
		rx_value val;
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
				data = rx_create_reference<write_data_t>();
				data->started = rx_get_us_ticks();
				ctx->set_instruction_data(data);
				ctx->set_waiting();
				return true;
			}
		}
	}
	else
	{// callback here
		uint64_t lasted = rx_get_us_ticks() - data->started;
		if (ctx->is_canceled())
		{
			out << "Write was canceled after ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		else
		{
			out << "Write lasted ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		return true;
	}
}


// Class sys_runtime::runtime_commands::turn_on_command 

turn_on_command::turn_on_command()
	: terminal::commands::server_command("turn-on")
{
}


turn_on_command::~turn_on_command()
{
}



bool turn_on_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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
	rx_value val;
	auto result = item->do_command(rx_object_command_t::rx_turn_on);
	if (!result)
	{
		dump_error_result(err, result);
		return false;
	}
	return true;
}


// Class sys_runtime::runtime_commands::turn_off_command 

turn_off_command::turn_off_command()
	: terminal::commands::server_command("turn-off")
{
}


turn_off_command::~turn_off_command()
{
}



bool turn_off_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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
	rx_value val;
	auto result = item->do_command(rx_object_command_t::rx_turn_off);
	if (!result)
	{
		dump_error_result(err, result);
		return false;
	}
	return true;
}


// Class sys_runtime::runtime_commands::browse_command 

browse_command::browse_command()
	: terminal::commands::server_command("brw")
{
}


browse_command::~browse_command()
{
}



bool browse_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	out << "Hello from browse command\r\n";
	return true;
}


} // namespace runtime_commands
} // namespace sys_runtime

