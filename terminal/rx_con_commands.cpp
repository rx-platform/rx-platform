

/****************************************************************************
*
*  terminal\rx_con_commands.cpp
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


// rx_con_commands
#include "terminal/rx_con_commands.h"

#include "terminal/rx_terminal_style.h"
#include "testing/testing.h"

#include "system/server/rx_server.h"
#include "system/python/py_support.h"
#include "system/server/rx_cmds.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"


namespace rx_internal {

namespace terminal {

namespace console {

namespace console_commands {
namespace
{
bool dump_info(std::ostream& out, rx_namespace_item& item)
{
	string_type quality_string;
	values::rx_value val(item.get_value());
	fill_quality_string(val, quality_string);
	string_type attrs;
	ns::fill_attributes_string(item.get_meta().attributes, attrs);
	string_type console;
	string_type storage_name(RX_NULL_ITEM_NAME);
	string_type storage_reference(RX_NULL_ITEM_NAME);
	auto storage_result = item.get_meta().resolve_storage();
	if (storage_result)
	{
		storage_name = storage_result.value()->get_storage_info();
		storage_reference = storage_result.value()->get_storage_reference();
	}

	string_type pera = g_complie_time;
	out << "\r\nINFO" << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Name       : " << item.get_meta().name << "\r\n";
	out << "Path       : " << item.get_meta().path << "\r\n";
	if(!console.empty())
		out << "Console    : " << console << "\r\n";
	out << "Type       : " << rx_item_type_name(item.get_type()) << "\r\n";
	out << "Attributes : " << attrs << "\r\n";
	out << "Storage    : " << storage_name << "\r\n";
	out << "Storage Ref: " << storage_reference << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Value      : ";
	out << val.get_storage().to_string();
	out << "\r\n";
	out << "Quality	   : " << quality_string << "\r\n";
	out << "Time stamp : " << val.get_time().get_string() << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	return true;
}
void fill_context_attributes(security::security_context_ptr ctx,string_type& val)
{
	val.assign(3, '-');
	if (ctx->is_authenticated())
		val[0] = 'a';
	if (ctx->is_system())
		val[1] = 's';
	if (ctx->is_hosted())
		val[2] = 'h';
}

}

// Class rx_internal::terminal::console::console_commands::info_command 

info_command::info_command()
  : server_command("info")
{
}


info_command::~info_command()
{
}



bool info_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type whose;
	if (!in.eof())
		in >> whose;
	if (!whose.empty())
	{
		rx_directory_ptr dir = ctx->get_current_directory()->get_sub_directory(whose);
		if (dir)
		{
			dump_dir_info(out,dir);
		}
		else
		{
			auto item = ctx->get_current_directory()->get_sub_item(whose);
			if (item)
			{
				dump_info(out, item);
			}
			else
			{
				err << "ERROR: unknown object name";
				return false;
			}
		}
	}
	else
	{
		dump_dir_info(out, ctx->get_current_directory());
	}
	return true;
}

bool info_command::dump_dir_info (std::ostream& out, rx_directory_ptr directory)
{
	string_type quality_stirng;
	values::rx_value val;
	directory->get_value(val);
	fill_quality_string(val, quality_stirng);
	string_type attrs;
	ns::fill_attributes_string(directory->get_attributes(), attrs);
	string_type cls_name;
	bool has_code = false;
	string_type console;
	directory->get_class_info(cls_name, console, has_code);

	meta::meta_data info = directory->meta_info();
	string_type pera = g_complie_time;
	out << "\r\nINFO" << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Name       : " << info.name << "\r\n";
	out << "Full Path  : " << info.path << "\r\n";
	out << "Type       : "  RX_CPP_DIRECORY_TYPE_NAME "\r\n";
	out << "Attributes : " << attrs << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Value      : " << "<<null>>" << "\r\n";
	out << "Quality	   : " << quality_stirng << "\r\n";
	out << "Time stamp : " << val.get_time().get_string() << "\r\n\r\n";
	out << "Created    : " << info.created_time.get_string() << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Class      : " << cls_name << "\r\n";
	out << "Has Code   : " << (has_code ? "true" : "false") << "\r\n";
	return true;
}


// Class rx_internal::terminal::console::console_commands::code_command 

code_command::code_command()
  : item_query_command("code")
{
}


code_command::~code_command()
{
}



bool code_command::do_with_item (platform_item_ptr&& item, std::ostream& out, std::ostream& err)
{
	string_type name = item->meta_info().get_full_path();
	item->fill_code_info(out, name);
	return true;
}


// Class rx_internal::terminal::console::console_commands::rx_name_command 

rx_name_command::rx_name_command()
  : server_command("pname")
{
}


rx_name_command::~rx_name_command()
{
}



bool rx_name_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	out << "System Information\r\n";
	out << RX_CONSOLE_HEADER_LINE "\r\n";
	out << "Instance Name: " << rx_gate::instance().get_rx_name() << "\r\n";
	out << "Node Name: " << rx_get_server_name() << "\r\n";
	out << "Engine Version: " << rx_gate::instance().get_rx_version() << "\r\n";
	out << "Library Version: " << rx_gate::instance().get_lib_version() << "\r\n";
	out << "OS/HW Interface: " << rx_gate::instance().get_hal_version() << "\r\n";
	out << "Compiler: " << rx_gate::instance().get_comp_version() << "\r\n";
	out << "OS: " << rx_gate::instance().get_os_info() << " [PID:" << rx_gate::instance().get_pid() << "]\r\n";

	/////////////////////////////////////////////////////////////////////////
	// Processor
	char buff[0x100];
	size_t cpu_count = 1;
	rx_collect_processor_info(buff, sizeof(buff) / sizeof(buff[0]), &cpu_count);
	out << "CPU: " << buff
		<< ( rx_big_endian ? "; Big-endian" : "; Little-endian" )
		<< "\r\n";
	/////////////////////////////////////////////////////////////////////////
	// memory
	size_t total = 0;
	size_t free = 0;
	size_t process = 0;
	rx_collect_memory_info(&total, &free, &process);
	out << "Memory: Total "
		<< (int)(total / 1048576ull)
		<< "MiB / Free "
		<< (int)(free / 1048576ull)
		<< "MiB / Process "
		<< (int)(process / 1024ull)
		<< "KiB \r\n";
	/////////////////////////////////////////////////////////////////////////
	out << "Page size: " << (int)rx_os_page_size() << " bytes\r\n";

	return true;
}


// Class rx_internal::terminal::console::console_commands::cls_command 

cls_command::cls_command()
  : server_command("cls")
{
}


cls_command::~cls_command()
{
}



bool cls_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	out << ANSI_CLS ANSI_CUR_HOME;
	return true;
}


// Class rx_internal::terminal::console::console_commands::shutdown_command 

shutdown_command::shutdown_command()
  : server_command("shutdown")
{
}


shutdown_command::~shutdown_command()
{
}



bool shutdown_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	std::istreambuf_iterator<char> begin(in), end;
	std::string msg(begin, end);
	if (msg.empty())
		msg = RX_NULL_ITEM_NAME;
	if (!rx_gate::instance().shutdown(msg))
		err << ANSI_COLOR_RED RX_ACCESS_DENIED ANSI_COLOR_RESET;
	return false;
}


// Class rx_internal::terminal::console::console_commands::log_command 

log_command::log_command()
	: server_command("log")
{
}


log_command::~log_command()
{
}



bool log_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type sub_command;
	in >> sub_command;
	if (sub_command.empty())
	{// empty
		out << get_help();
	}
	else if (sub_command == "test")
	{// testing stuff
		do_test_command(in, out, err, ctx);
	}
	else if (sub_command == "last")
	{// testing stuff
		do_last_command(in, out, err, ctx);
	}
	else if (sub_command == "help")
	{
		out << get_help();
	}
	else
	{
		err << sub_command
			<< " is unknown command type.";
		return false;
	}
	return true;
}

bool log_command::do_test_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{

	char buffer[0x100];

	snprintf(buffer, sizeof(buffer), "User %s performing log test."
		, security::active_security()->get_full_name().c_str());

	const char* line = buffer;
	CONSOLE_LOG_INFO("log", 900, line);
	out << line << "\r\n";

	size_t spans_count = 4;

	spans_.assign(spans_count, 0.0);

	smart_ptr hold_ref = smart_this();

	callback_ = [spans_count, this, ctx] () mutable {
		auto api_ctx = ctx->create_api_context();
		std::function<void(console_context_ptr)> func = [spans_count, this](console_context_ptr ctx)
			{
				int pass = 0;
				for (auto& one : spans_)
				{
					if (one == 0.)
					{
						char buffer[0x100];
						const char* line = buffer;
						uint64_t second_tick = rx_get_us_ticks();
						double ms = (double)(second_tick - last_tick_) / 1000.0;
						one = ms;
						snprintf(buffer, sizeof(buffer), "Console log test %d passed. Delay time: %g ms...", pass, ms);
						ctx->get_stdout() << buffer << "\r\n";
						CONSOLE_LOG_INFO("log", 900, line);
						snprintf(buffer, sizeof(buffer), "Console log test pass %d...", 0);
						ctx->get_stdout() << buffer << "\r\n";
						last_tick_ = rx_get_us_ticks();
						RX_LOG_TEST(buffer, callback_);
						return;
					}
					pass++;
				}
				char buffer[0x100];
				const char* line = buffer;
				double val = 0.0;
				size_t count = spans_.size();
				if (count > 1)
				{
					for (size_t i = 1; i < count; i++)
					{
						val += spans_[i];
					}
					val = val / (double(count - 1));
				}
				else
					val = spans_[0];
				snprintf(buffer, sizeof(buffer), "Average response time: %g ms...", val);
				line = buffer;
				CONSOLE_LOG_INFO("log", 900, line);
				ctx->get_stdout() << line << "\r\n";

				snprintf(buffer, sizeof(buffer), "User %s log test completed."
					, security::active_security()->get_full_name().c_str());

				line = buffer;
				CONSOLE_LOG_INFO("log", 900, line);
				ctx->get_stdout() << line << "\r\n";
				ctx->send_results(true);
			};
		rx_post_function_to(ctx->get_executer(), api_ctx.object, std::move(func), ctx);
	};


	snprintf(buffer, sizeof(buffer), "Console log test pass %d...", 0);
	last_tick_ = rx_get_us_ticks();
	RX_LOG_TEST(buffer, callback_);

	ctx->set_waiting();

	return true;
}

bool log_command::do_last_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	log::log_query_type query;
	query.type = log::rx_log_query_type::normal_level;
	string_type options;
	int count = 20;
	in >> options;
	if (!options.empty())
	{
		auto temp = atoi(options.c_str());
		if (temp > 0)
			count = temp;
		in >> options;
		if (options == "-t")
			query.type = log::rx_log_query_type::trace_level;
		if (options == "-d")
			query.type = log::rx_log_query_type::debug_level;
		if (options == "-w")
			query.type = log::rx_log_query_type::warining_level;
		if (options == "-e")
			query.type = log::rx_log_query_type::error_level;
	}
	log::log_events_type result;
	auto ret = rx_gate::instance().read_log(query, result);
	if (ret)
	{
		list_log_options options;
		options.list_level = false;
		options.list_code = false;
		options.list_library = false;
		options.list_source = false;
		options.list_dates = false;
		dump_log_items(result, options, out, count);
	}
	return true;
}

void log_command::dump_log_items (const log::log_events_type& items, list_log_options options, std::ostream& out, int count)
{
	size_t first_index = 0;
	size_t full_count = items.size();
	size_t rows_count = full_count;
	if (count > 0 && (size_t)count + 1 < full_count)
	{
		first_index = full_count - (size_t)count - 1;
		rows_count = full_count - first_index + 1;
	}
	rx_table_type table(rows_count + 1);
	table[0].emplace_back("Time");
	table[0].emplace_back("Type");
	if (options.list_source)
		table[0].emplace_back("Source");
	if (options.list_level)
		table[0].emplace_back("Level");
	if (options.list_library)
		table[0].emplace_back("Library");
	table[0].emplace_back("Message");
	if (options.list_code)
		table[0].emplace_back("Source Code");

	size_t idx = 1;

	for (auto i = first_index; i < full_count; i++)
	{
		auto& one = items[i];
		table[idx].emplace_back(one.when.get_string(options.list_dates));

		table[idx].emplace_back(create_log_type_cell(one.event_type));
		if (options.list_source)
			table[idx].emplace_back(one.source);
		if (options.list_level)
			table[idx].emplace_back(std::to_string(one.level));
		if (options.list_library)
			table[idx].emplace_back(one.library);
		table[idx].emplace_back(one.message);
		if (options.list_code)
			table[idx].emplace_back(one.code);
		idx++;
	}

	rx_dump_table(table, out, true, false);
}

rx_table_cell_struct log_command::create_log_type_cell (log::log_event_type type)
{
	switch (type)
	{
	case log::log_event_type::info:
		return rx_table_cell_struct{ "INFO", ANSI_RX_LOG_INFO, ANSI_COLOR_RESET };
	case log::log_event_type::warning:
		return rx_table_cell_struct{ "WARNING", ANSI_RX_LOG_WARNING, ANSI_COLOR_RESET };
	case log::log_event_type::error:
		return rx_table_cell_struct{ "ERROR", ANSI_RX_LOG_ERROR, ANSI_COLOR_RESET };
	case log::log_event_type::critical:
		return rx_table_cell_struct{ "CRITICAL", ANSI_RX_LOG_CRITICAL, ANSI_COLOR_RESET };
	case log::log_event_type::debug:
		return rx_table_cell_struct{ "DEBUG", ANSI_RX_LOG_DEBUG, ANSI_COLOR_RESET };
	case log::log_event_type::trace:
		return rx_table_cell_struct{ "TRACE", ANSI_RX_LOG_TRACE, ANSI_COLOR_RESET };
	default:
		return rx_table_cell_struct{ "***UNKNOWN***", ANSI_RX_LOG_UNKNOWN, ANSI_COLOR_RESET };
	}
}

void log_command::log_fired (console_context_ptr ctx)
{
}


// Class rx_internal::terminal::console::console_commands::sec_command 

sec_command::sec_command()
	: server_command("sec")
{
}


sec_command::~sec_command()
{
}



bool sec_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type sub_command;
	in >> sub_command;
	if (sub_command.empty())
	{// empty
		out << get_help();
	}
	else if (sub_command == "active")
	{
		do_active_command(in, out, err, ctx);
	}
	else if (sub_command == "help")
	{
		out << get_help();
	}
	else
	{
		err << "Unknown sub-command";
		return false;
	}
	return true;
}

bool sec_command::do_active_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	std::vector<security::security_context_ptr> ctxs;
	security::security_manager::instance().get_active_contexts(ctxs);
	out << "Dumping active users:\r\n\r\n";

	size_t count = ctxs.size();
	rx_table_type table(count+1);

	table[0].emplace_back("Id");
	table[0].emplace_back("User Name");
	table[0].emplace_back("Console");
	table[0].emplace_back("System");
	table[0].emplace_back("Port");

	rx_security_handle_t my_handle = security::active_security()->get_handle();

	for (size_t i=0; i<count; i++)
	{
		string_type name = ctxs[i]->get_full_name();
		rx_security_handle_t sec_handle = ctxs[i]->get_handle();
		string_type prefix;
		bool is_console = ctxs[i]->has_console();
		if (sec_handle == my_handle)
		{
			prefix = ANSI_COLOR_GREEN;
			name += "*";
		}
		else if (is_console)
			prefix = ANSI_COLOR_CYAN;

		char buff[0x20];
		snprintf(buff, sizeof(buff), "[%" PRIxPTR "]",sec_handle);
		table[i + 1].emplace_back(buff);
		if (prefix.empty())
			table[i + 1].emplace_back(name);
		else
			table[i + 1].emplace_back(name, prefix,ANSI_COLOR_RESET);
		string_type temp;
		fill_context_attributes(ctxs[i], temp);
		//table[i + 1].emplace_back(temp);
		if (is_console)
			table[i + 1].emplace_back(RX_CONSOLE_CHECKED);
		else
			table[i + 1].emplace_back(RX_CONSOLE_UNCHECKED);
		if(ctxs[i]->is_system())
			table[i + 1].emplace_back(RX_CONSOLE_CHECKED);
		else
			table[i + 1].emplace_back(RX_CONSOLE_UNCHECKED);
		table[i + 1].emplace_back(ctxs[i]->get_location());
	}
	rx_dump_table(table, out,true,true);

	return true;
}


// Class rx_internal::terminal::console::console_commands::time_command 

time_command::time_command()
	: server_command("time")
{
}


time_command::~time_command()
{
}



bool time_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	out << "Start Time\t: " << rx_gate::instance().get_started().get_string() << " UTC\r\n";
	out << "Current Time\t: " << rx_time::now().get_string() << " UTC\r\n";
	return true;
}


// Class rx_internal::terminal::console::console_commands::sleep_command 

sleep_command::sleep_command()
	: server_command("sleep")
{
}


sleep_command::~sleep_command()
{
}



bool sleep_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{

	rx_reference<sleep_data_t> data = ctx->get_instruction_data<sleep_data_t>();
	if (!data)
	{// we just entered to command
		uint32_t period = 0;
		in >> period;

		if (period == 0)
		{
			err << "Invalid period specified!";
			return false;
		}
		else
		{
			data = rx_create_reference<sleep_data_t>();
			data->started = rx_get_us_ticks();
			ctx->set_instruction_data(data);
			ctx->postpone(period);
			ctx->one_more_time();
		}
	}
	else
	{// timer expired or canceled
		uint64_t lasted = rx_get_us_ticks() - data->started;
		if (ctx->is_canceled())
		{
			out << "Sleep was canceled after ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		else
		{
			out << "Sleep lasted ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
	}
	return true;
}


// Class rx_internal::terminal::console::console_commands::def_command 

def_command::def_command()
	: item_query_command("def")
{
}


def_command::~def_command()
{
}



bool def_command::do_with_item (platform_item_ptr&& item, std::ostream& out, std::ostream& err)
{
	out << "Definition of " ANSI_RX_OBJECT_COLOR
		<< item->meta_info().get_full_path()
		<< ANSI_COLOR_RESET ":\r\n";
	auto def = item->get_definition_as_json();
	out << def;
	return true;
}


// Class rx_internal::terminal::console::console_commands::item_query_command 

item_query_command::item_query_command (const string_type& console_name)
	: server_command(console_name)
{
}


item_query_command::~item_query_command()
{
}



bool item_query_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type whose;
	if (!in.eof())
		in >> whose;
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
		rx_result result = model::algorithms::do_with_item(resolve_result.move_value()
			, [ctx, this](rx_result_with<platform_item_ptr>&& data) mutable -> rx_result
			{
				auto& out = ctx->get_stdout();
				auto& err = ctx->get_stderr();
				if (data)
				{
					return do_with_item(data.move_value(), out, err);
				}
				else
				{
					dump_error_result(err, data);
					return false;
				}
			}
			, rx_result_callback(context.object, [ctx](rx_result&& result) mutable
				{
					bool arg = result;
					ctx->get_client()->process_event(arg, ctx->get_out(), ctx->get_err(), true);
				}), context);
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


// Class rx_internal::terminal::console::console_commands::phyton_command 

phyton_command::phyton_command()
	: server_command("python")
{
}


phyton_command::~phyton_command()
{
}



bool phyton_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
#ifdef PYTHON_SUPPORT
	string_type sub_command;
	in >> sub_command;
	if (sub_command.empty())
	{// "jbg prazno
		err << "It,s not over yet, be more paitent...";
		return false;
	}
	else if (sub_command == "version" || sub_command == "ver")
	{// testing stuff
		out << "Embedded Python Version\r\n======================================\r\n";
		python::py_script::instance().dump_script_information(out);
		out << "\r\n";
	}
	else
	{
		err << "Unknown command type!!!!\r\n";
		return false;
	}
	return true;
#else
	err << "No python support!!!!\r\n";
	return false;
#endif
}


// Class rx_internal::terminal::console::console_commands::license_command 

license_command::license_command()
	: server_command("license")
{
}


license_command::~license_command()
{
}



bool license_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	auto storage = rx_gate::instance().get_host()->get_system_storage("sys");
	if (storage)
	{
		const string_type& lic_info = "aaaa";//storage->get_license();
		if (lic_info.empty())
		{
			err << "No valid LICENSE file in directory!!!";
			return false;
		}
		else
		{
			out << lic_info;
			return true;
		}
	}
	else
	{
		err << "No LICENSE!!!";
		return false;
	}
}


// Class rx_internal::terminal::console::console_commands::help_command 

help_command::help_command()
	: server_command("help")
{
}


help_command::~help_command()
{
}



bool help_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type command_name;
	in >> command_name;
	out << "Printing help...\r\n";
	if (command_name.empty())
	{
		out << RX_CONSOLE_HEADER_LINE "\r\n";
		out << get_help();
		return true;
	}
	else
	{//try to find a command
		auto command = terminal::commands::server_command_manager::instance()->get_command_by_name(command_name);
		if (command)
		{
			out << command->get_help();
			return true;
		}
		else
		{
			err << "Unknown command!";
			return false;
		}
	}
}

string_type help_command::get_help () const
{
	static string_type help;
	static locks::slim_lock lock;
	if (help.empty())
	{
		// double lock here, just in case
		locks::auto_lock_t<decltype(lock)> _(&lock);
		if (help.empty())
		{
			std::ostringstream out;
			out << "Well, this is a list of commands:\r\n\r\n";

			std::vector<server_command_ptr> commands;
			commands::server_command_manager::instance()->get_commands(commands);

			rx_row_type names;
			names.reserve(commands.size());
			for (auto one : commands)
				names.emplace_back(one->get_name(), ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET);

			rx_dump_large_row(names, out, RX_CONSOLE_WIDTH);

			out << "\r\nchoose one and type " ANSI_COLOR_GREEN ANSI_COLOR_BOLD "help " ANSI_COLOR_YELLOW ANSI_COLOR_BOLD  "<command>" ANSI_COLOR_RESET " for more details.";
			out << "\r\n\r\n" ANSI_COLOR_MAGENTA "Maybe this is final help implementation...\r\n" ANSI_COLOR_RESET;
			out << ANSI_COLOR_BOLD ANSI_COLOR_RED "/" ANSI_COLOR_YELLOW "/" ANSI_COLOR_GREEN "/" ANSI_COLOR_CYAN "/" ANSI_COLOR_RESET
				<< "  sinclair ZX Spectrum!!!\r\n";
			out.flush();
			help = out.str();
		}
	}
	return help.c_str();
}


} // namespace console_commands
} // namespace console
} // namespace terminal
} // namespace rx_internal

