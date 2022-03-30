

/****************************************************************************
*
*  runtime_internal\rx_runtime_commands.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_runtime_commands
#include "runtime_internal/rx_runtime_commands.h"

#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "terminal/rx_console.h"
#include "lib/rx_ser_json.h"
#include "terminal/rx_term_table.h"


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



bool read_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, console_context_ptr ctx, std::istream& in, std::ostream& out, std::ostream& err, rx_thread_handle_t executer)
{
	string_type full_path = rt_item->meta_info().get_full_path();
	if (!sub_item.empty())
	{
		full_path += ".";
		full_path += sub_item;
	}
	auto rctx = ctx->create_api_context();
	rt_item->read_value(sub_item,
		read_result_callback_t(rctx.object, [anchor = rctx.object, ctx, full_path=std::move(full_path), executer](rx_result&& result, rx_value&& value)
			{				
				auto& out = ctx->get_stdout();
				if (result)
				{
					out << full_path;
					out << " = ";
					rx_dump_value(value, out, false);
					out << "\r\n";
				}
				else
				{
					out << "Error reading item "
						<< full_path;
					for (const auto& one : result.errors())
						out << "\r\n" << one;
					out << "\r\n";
				}
				ctx->continue_scan();
			}, executer));
	
	return false;
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



bool write_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, console_context_ptr ctx, std::istream& in, std::ostream& out, std::ostream& err, rx_thread_handle_t executer)
{
	rx_simple_value value;
	string_type val_str;
	in >> val_str;
	if (val_str.empty())
	{
		ctx->raise_error(rx_result("No value specified!"));
		return true;
	}
	value.parse(val_str);
	auto now = rx_time::now();
	out << "Write prepared. \r\n";
	out << sub_item << " <= "
		<< ANSI_RX_GOOD_COLOR;
	rx_dump_value(value, out, true);
	out << ANSI_COLOR_RESET "\r\n";
	out << "Start time: " << now.get_string() << "\r\n";
	uint64_t us1 = rx_get_us_ticks();
	auto rctx = ctx->create_api_context();
	ctx->set_waiting();

	rt_item->write_value(sub_item, std::move(value),
		write_result_callback_t(rctx.object, [ctx, this, sub_item, value, us1](rx_result&& result)
		{
			uint64_t us2 = rx_get_us_ticks() - us1;
			auto& out = ctx->get_stdout();
			if (result)
			{
				out << "Write to "
					<< sub_item
					<< " " ANSI_RX_GOOD_COLOR "succeeded" ANSI_COLOR_RESET ". \r\n";
				out << "Time elapsed: " ANSI_RX_GOOD_COLOR << us2 << ANSI_COLOR_RESET " us\r\n";
			}
			else
			{
				out << "Write " ANSI_COLOR_BOLD ANSI_COLOR_RED "failed" ANSI_COLOR_RESET ". \r\n";
				ctx->raise_error(result);
			}
			ctx->continue_scan();

		}, executer));

	return false;	
}


// Class rx_internal::sys_runtime::runtime_commands::turn_on_command 

turn_on_command::turn_on_command()
	: terminal::commands::server_command("turnon")
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
	ns::rx_directory_resolver dirs;
	dirs.add_paths({ ctx->get_current_directory() });
	auto item = rx_gate::instance().get_namespace_item(object_path, &dirs);
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
	: terminal::commands::server_command("turnoff")
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
	ns::rx_directory_resolver dirs;
	dirs.add_paths({ ctx->get_current_directory() });
	auto item = rx_gate::instance().get_namespace_item(object_path, &dirs);
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



bool browse_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, console_context_ptr ctx, std::istream& in, std::ostream& out, std::ostream& err, rx_thread_handle_t executer)
{

	auto rctx = ctx->create_api_context();
	
	rt_item->browse("", sub_item, "", browse_result_callback_t(rctx.object
		, [ctx](rx_result&& result, std::vector<runtime_item_attribute>&& items)
		{
			auto& out = ctx->get_stdout();
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
					bool is_value = false;
					string_type postfix;
					string_type value = one.value.extract_static<string_type>(""s);
					if (one.value.is_null())
						value = RX_TERMINAL_STRUCT_SYMBOL;
					switch (one.type)
					{
					case rx_attribute_type::const_attribute_type:
						is_value = true;
						table[idx].emplace_back(one.name, ANSI_RX_CONST_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::const_array_attribute_type:
						value = RX_TERMINAL_ARRAY_SYMBOL;
						table[idx].emplace_back(one.name, ANSI_RX_CONST_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::value_attribute_type:
						is_value = true;
						table[idx].emplace_back(one.name, ANSI_RX_VALUE_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::value_array_attribute_type:
						value = RX_TERMINAL_ARRAY_SYMBOL;
						table[idx].emplace_back(one.name, ANSI_RX_VALUE_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::variable_attribute_type:
						is_value = true;
						table[idx].emplace_back(one.name, ANSI_RX_VARIABLE_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::variable_array_attribute_type:
						is_value = false;
						value = RX_TERMINAL_ARRAY_SYMBOL;
						table[idx].emplace_back(one.name, ANSI_RX_VARIABLE_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::struct_attribute_type:
						table[idx].emplace_back(one.name, ANSI_RX_STRUCT_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::struct_array_attribute_type:
						value = RX_TERMINAL_ARRAY_SYMBOL;
						table[idx].emplace_back(one.name, ANSI_RX_STRUCT_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::filter_attribute_type:
						table[idx].emplace_back(one.name, ANSI_RX_FILTER_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::event_attribute_type:
						table[idx].emplace_back(one.name, ANSI_RX_EVENT_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::source_attribute_type:
						is_value = true;
						table[idx].emplace_back(one.name, ANSI_RX_SOURCE_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::mapper_attribute_type:
						is_value = true;
						table[idx].emplace_back(one.name, ANSI_RX_MAPPER_COLOR, ANSI_COLOR_RESET);
						break;
					case rx_attribute_type::relation_attribute_type:
						table[idx].emplace_back(one.name, ANSI_RX_RELATION_COLOR, ANSI_COLOR_RESET);
						if (one.value.is_string())
							value = RX_TERMINAL_RELATION_SYMBOL + one.value.get_string();
						else if (one.value.is_null())
							value = RX_TERMINAL_RELATION_SYMBOL;
						else
							value = RX_TERMINAL_RELATION_SYMBOL + one.value.to_string();
						break;
					case rx_attribute_type::relation_target_attribute_type:
						table[idx].emplace_back(one.name, ANSI_RX_RELATION_TARGET_COLOR, ANSI_COLOR_RESET);
						if (one.value.is_string())
							value = RX_TERMINAL_RELATION_TARGET_SYMBOL + one.value.get_string();
						else if (one.value.is_null())
							value = RX_TERMINAL_RELATION_TARGET_SYMBOL;
						else
							value = RX_TERMINAL_RELATION_TARGET_SYMBOL + one.value.to_string();
						break;
					default:
						table[idx].emplace_back(one.name);
						break;
					}
					if (is_value)
						table[idx].emplace_back(value, ANSI_COLOR_WHITE ANSI_COLOR_BOLD, ANSI_COLOR_RESET);
					else
						table[idx].emplace_back(value, ANSI_COLOR_WHITE, ANSI_COLOR_RESET);

					if (!postfix.empty())
						table[idx].emplace_back(postfix + rx_runtime_attribute_type_name(one.type));
					else
						table[idx].emplace_back(/*string_type(RX_TERMINAL_STRUCT_SYMBOL_SIZE, ' ') + */rx_runtime_attribute_type_name(one.type));
					idx++;
				}
				rx_dump_table(table, out, true, false);
			}
			else
			{
				ctx->raise_error(result);
			}
			ctx->continue_scan();
		}, executer));

	return false;
}


// Class rx_internal::sys_runtime::runtime_commands::runtime_command_base 

runtime_command_base::runtime_command_base (const string_type& name)
	: server_command(name)
{
}



bool runtime_command_base::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type full_path;
	string_type line;

	in >> full_path;
	if (!in.eof())
	{
		std::istreambuf_iterator<char> eos;
		line = string_type(std::istreambuf_iterator<char>(in), eos);
	}
	if (full_path.empty())
	{
		err << "Empty path!";
		return false;
	}
	string_type whose;
	string_type item_path;
	if(full_path[0]!=-'-')
		rx_split_item_path(full_path, whose, item_path);
	if (!whose.empty())
	{
		rx_directory_resolver directories;
		directories.add_paths({ ctx->get_current_directory() });
		api::rx_context context;
		context.active_path = ctx->get_current_directory();
		context.object = smart_this();

		auto resolve_result = api::ns::rx_resolve_reference(whose, directories);
		if (!resolve_result)
		{
			ctx->raise_error(resolve_result);
			return resolve_result;

		}

		rx_thread_handle_t executer = rx_thread_context();
		ctx->set_waiting();
		rx_result result = model::algorithms::do_with_runtime_item(resolve_result.value()
			, [ctx, item_path, this, executer, line = std::move(line)](rx_result_with<platform_item_ptr>&& data) mutable -> bool
			{
				auto& out = ctx->get_stdout();
				auto& err = ctx->get_stderr();
				if (data)
				{
					std::istringstream in(line);
					return this->do_with_item(data.move_value(), item_path, ctx, in, out, err, executer);
				}
				else
				{
					ctx->raise_error(data);
					return true;
				}
			}
			, callback::rx_any_callback<bool>(context.object, [ctx](bool done) mutable
				{
					if(done)
						ctx->continue_scan();
				})
			, context);

		if (!result)
		{
			ctx->raise_error(result);
		}
		return result;
	}
	else
	{
		err << "Please, define item!";
		return false;
	}
}


// Class rx_internal::sys_runtime::runtime_commands::struct_command 

struct_command::struct_command()
	: runtime_command_base("json")
{
}


struct_command::~struct_command()
{
}



bool struct_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, console_context_ptr ctx, std::istream& in, std::ostream& out, std::ostream& err, rx_thread_handle_t executer)
{
	using parser_t = urke::parser::parser3000;
	bool pretty = false;
	bool help = false;
	bool version = false;

	if (!in.eof())
	{
		parser_t parser;
		parser.add_bit_option('h', "pretty", &pretty, "\"Pretty\" - human readable form of output. The <h> switch is obvious.");

		auto ret = parser.parse(in, err);
		if (ret)
		{
			if (help)
			{
				parser.print_help("struct json [PATH] [VALUE] [OPTIONS]", out);
				return true;
			}
			else if (version)
			{
				auto version = code_version();
				out << "Version "
					<< version[0] << ". "
					<< version[1] << ". "
					<< version[2] << ".";
				return true;
			}
			else
			{
				// do the rest, fall through
			}
		}
		else
		{
			return false;
		}
		
	}

	string_type full_path = rt_item->meta_info().get_full_path();
	if (!sub_item.empty())
	{
		full_path += ".";
		full_path += sub_item;
	}
	auto rctx = ctx->create_api_context();
	read_struct_data data;
	data.type = runtime_value_type::simple_runtime_value;
	data.callback = read_struct_callback_t(rctx.object, [pretty, ctx, full_path = std::move(full_path)](rx_result&& result, data::runtime_values_data&& data)
	{
		auto& out = ctx->get_stdout();
		if (result)
		{
			if (pretty)
			{
				serialization::pretty_json_writer writer;
				writer.write_header(STREAMING_TYPE_MESSAGE, 0);
				if (writer.write_init_values(nullptr, data))
				{
					result = writer.write_footer();
				}
				else
				{
					result = writer.get_error();
				}
				out << writer.get_string();
			}
			else
			{
				serialization::json_writer writer;
				writer.write_header(STREAMING_TYPE_MESSAGE, 0);
				if (writer.write_init_values(nullptr, data))
				{
					result = writer.write_footer();
				}
				else
				{
					result = writer.get_error();
				}
				out << writer.get_string();
			}
			out << "\r\n";
		}
		if(!result)
		{
			out << "Error reading structure "
				<< full_path;
			for (const auto& one : result.errors())
				out << "\r\n" << one;
			out << "\r\n";
		}
		ctx->continue_scan();
	}, executer);
	rt_item->read_struct(sub_item, std::move(data));

	return false;
}


// Class rx_internal::sys_runtime::runtime_commands::execute_command 

execute_command::execute_command()
	: runtime_command_base("exec")
{
}


execute_command::~execute_command()
{
}



bool execute_command::do_with_item (platform_item_ptr&& rt_item, string_type sub_item, console_context_ptr ctx, std::istream& in, std::ostream& out, std::ostream& err, rx_thread_handle_t executer)
{

	std::istreambuf_iterator<char> eos;
	string_type data = string_type(std::istreambuf_iterator<char>(in), eos);
	data::runtime_values_data rt_data;
	serialization::json_reader reader;
	if(reader.parse_data(data))
		reader.read_init_values(nullptr, rt_data);
	auto now = rx_time::now();
	out << "Execute prepared. \r\n";
	out << "Start time: " << now.get_string() << "\r\n";
	uint64_t us1 = rx_get_us_ticks();
	auto rctx = ctx->create_api_context();
	rt_item->execute_method(sub_item, std::move(rt_data), execute_method_callback_t(rctx.object, [us1, ctx, sub_item](rx_result result, data::runtime_values_data data)
		{
			uint64_t us2 = rx_get_us_ticks() - us1;
			auto& out = ctx->get_stdout();
			if (result)
			{
				out << "Execute "
					<< sub_item
					<< " " ANSI_RX_GOOD_COLOR "succeeded" ANSI_COLOR_RESET ". \r\n";
				out << "Result:";
				serialization::pretty_json_writer writer;
				writer.write_header(STREAMING_TYPE_MESSAGE, 0);
				writer.write_init_values(nullptr, data);
				writer.write_footer();
				out << writer.get_string();
				out << "\r\n";
					
				out << "Time elapsed: " ANSI_RX_GOOD_COLOR << us2 << ANSI_COLOR_RESET " us\r\n";
			}
			else
			{
				out << "Execute " ANSI_COLOR_BOLD ANSI_COLOR_RED "failed" ANSI_COLOR_RESET ". \r\n";
				ctx->raise_error(result);
			}
			ctx->continue_scan();
		}, executer));
	return false;
}


} // namespace runtime_commands
} // namespace sys_runtime
} // namespace rx_internal

