

/****************************************************************************
*
*  terminal\rx_con_commands.cpp
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


// rx_con_commands
#include "terminal/rx_con_commands.h"

#include "terminal/rx_terminal_style.h"
#include "help/rx_help_security.h"
#include "testing/testing.h"

#include "system/server/rx_server.h"
#include "system/python/py_support.h"
#include "system/server/rx_cmds.h"
#include "sys_internal/rx_internal_ns.h"


namespace terminal {

namespace console {

namespace console_commands {
namespace
{
bool dump_info(std::ostream& out, rx_platform_item::smart_ptr& item)
{
	string_type quality_string;
	values::rx_value val(item->get_value());
	ns::fill_quality_string(val, quality_string);
	string_type attrs;
	ns::fill_attributes_string(item->meta_info().get_attributes(), attrs);
	string_type cls_name;
	bool has_code = false;
	string_type console;
	item->get_class_info(cls_name, console, has_code);
	cls_name=item->get_class_name();
	string_type storage_name = item->meta_info().storage_info.storage_name();
	if (storage_name.empty())
		storage_name = ANSI_STATUS_ERROR;


	string_type pera = g_complie_time;
	out << "\r\nINFO" << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Name       : " << item->get_name() << "\r\n";
	out << "Full Path  : " << item->meta_info().get_path() << "\r\n";
	if(!console.empty())
		out << "Console    : " << console << "\r\n";
	out << "Type       : " << item->get_type_name() << "\r\n";
	out << "Attributes : " << attrs << "\r\n";
	out << "Storage    : " << storage_name << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Value      : ";
	val.get_storage().dump_to_stream(out);
	out << "\r\n";
	out << "Quality	   : " << quality_string << "\r\n";
	out << "Time stamp : " << val.get_time().get_string() << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Class      : " << cls_name << "\r\n";
	out << "Has Code   : " << (has_code ? "true" : "false") << "\r\n";
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

// Class terminal::console::console_commands::info_command 

info_command::info_command()
  : directory_aware_command("info")
{
}


info_command::~info_command()
{
}



bool info_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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
			platform_item_ptr item = ctx->get_current_directory()->get_sub_item(whose);
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
	ns::fill_quality_string(val, quality_stirng);
	string_type attrs;
	ns::fill_attributes_string(directory->get_attributes(), attrs);
	string_type cls_name;
	bool has_code = false;
	string_type console;
	directory->get_class_info(cls_name, console, has_code);


	string_type pera = g_complie_time;
	out << "\r\nINFO" << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Name       : " << directory->get_name() << "\r\n";
	out << "Full Path  : " << directory->get_path() << "\r\n";
	out << "Type       : " << directory->get_type_name() << "\r\n";
	out << "Attributes : " << attrs << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Value      : " << "<<null>>" << "\r\n";
	out << "Quality	   : " << quality_stirng << "\r\n";
	out << "Time stamp : " << val.get_time().get_string() << "\r\n\r\n";
	out << "Created    : " << directory->get_created().get_string() << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Class      : " << cls_name << "\r\n";
	out << "Has Code   : " << (has_code ? "true" : "false") << "\r\n";
	return true;
}


// Class terminal::console::console_commands::code_command 

code_command::code_command()
  : directory_aware_command("code")
{
}


code_command::~code_command()
{
}



bool code_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type whose;
	if (!in.eof())
		in >> whose;
	if (!whose.empty())
	{
		rx_directory_ptr dir = ctx->get_current_directory()->get_sub_directory(whose);
		if (dir)
		{
			dir->fill_code_info(out);
		}
		else
		{
			platform_item_ptr item = ctx->get_current_directory()->get_sub_item(whose);
			if (item)
			{
				item->fill_code_info(out,whose);
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
		ctx->get_current_directory()->fill_code_info(out);
	}
	return true;
}


// Class terminal::console::console_commands::rx_name_command 

rx_name_command::rx_name_command()
  : server_command("pname")
{
}


rx_name_command::~rx_name_command()
{
}



bool rx_name_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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
	rx_collect_processor_info(buff, sizeof(buff) / sizeof(buff[0]));
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


// Class terminal::console::console_commands::cls_command 

cls_command::cls_command()
  : server_command("cls")
{
}


cls_command::~cls_command()
{
}



bool cls_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	out << ANSI_CLS ANSI_CUR_HOME;
	return true;
}


// Class terminal::console::console_commands::shutdown_command 

shutdown_command::shutdown_command()
  : server_command("shutdown")
{
}


shutdown_command::~shutdown_command()
{
}



bool shutdown_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	std::istreambuf_iterator<char> begin(in), end;
	std::string msg(begin, end);
	if (msg.empty())
		msg = RX_NULL_ITEM_NAME;
	if(!rx_gate::instance().shutdown(msg))
		err << ANSI_COLOR_RED RX_ACCESS_DENIED ANSI_COLOR_RESET;
	return false;
}


// Class terminal::console::console_commands::log_command 

log_command::log_command()
	: server_command("log")
{
}


log_command::~log_command()
{
}



bool log_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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
	else if (sub_command == "hist")
	{// testing stuff
		do_hist_command(in, out, err, ctx);
	}
	return true;
}

bool log_command::do_test_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{

	char buffer[0x100];

	snprintf(buffer, sizeof(buffer), "User %s performing log test."
		, security::active_security()->get_full_name().c_str());

	const char* line = buffer;
	CONSOLE_LOG_INFO("log", 900,line);
	out << line << "\r\n";

	double spans[4];

	for (size_t i = 0; i < sizeof(spans) / sizeof(spans[0]); i++)
	{
		snprintf(buffer, sizeof(buffer), "Console log test pass %d...", (int)i);
		rx::locks::event ev(false);
		uint64_t first_tick = rx_get_us_ticks();
		RX_LOG_TEST(buffer, &ev);
		ev.wait_handle();
		uint64_t second_tick = rx_get_us_ticks();
		double ms = (double)(second_tick - first_tick) / 1000.0;
		snprintf(buffer, sizeof(buffer), "Console log test %d passed. Delay time: %g ms...", (int)i, ms);
		CONSOLE_LOG_TRACE("log",0,buffer);
		out << buffer << "\r\n";
		spans[i] = ms;
		rx_msleep(10);
	}

	double val = 0.0;
	size_t count = sizeof(spans) / sizeof(spans[0]);
	if (count > 1)
	{
		for (size_t i = 1; i < count; i++)
		{
			val += spans[i];
		}
		val = val / (double(count - 1));
	}
	else
		val = spans[0];
	snprintf(buffer, sizeof(buffer), "Average response time: %g ms...", val);
	line = buffer;
	CONSOLE_LOG_INFO("log", 900,line);
	out << line << "\r\n";

	snprintf(buffer, sizeof(buffer), "User %s log test completed."
		, security::active_security()->get_full_name().c_str());

	line = buffer;
	CONSOLE_LOG_INFO("log", 900,line);
	out << line << "\r\n";

	return true;
}

bool log_command::do_hist_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	log::log_query_type query;
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
		dump_log_items(result, options, out, err, ctx);
	}
	return true;
}

bool log_command::dump_log_items (const log::log_events_type& items, list_log_options options, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	rx_table_type table(items.size() + 1);
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
	for (const auto& one : items)
	{
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

	return true;
}

rx_table_cell_struct log_command::create_log_type_cell (log::log_event_type type)
{
	switch (type)
	{
	case log::log_event_type::info_log_event:
		return rx_table_cell_struct{ "INFO", ANSI_RX_LOG_INFO, ANSI_COLOR_RESET };
	case log::log_event_type::warning_log_event:
		return rx_table_cell_struct{ "WARNING", ANSI_RX_LOG_WARNING, ANSI_COLOR_RESET };
	case log::log_event_type::error_log_event:
		return rx_table_cell_struct{ "ERROR", ANSI_RX_LOG_ERROR, ANSI_COLOR_RESET };
	case log::log_event_type::critical_log_event:
		return rx_table_cell_struct{ "CRITICAL", ANSI_RX_LOG_CRITICAL, ANSI_COLOR_RESET };
	case log::log_event_type::debug_log_event:
		return rx_table_cell_struct{ "DEBUG", ANSI_RX_LOG_DEBUG, ANSI_COLOR_RESET };
	case log::log_event_type::trace_log_event:
		return rx_table_cell_struct{ "TRACE", ANSI_RX_LOG_TRACE, ANSI_COLOR_RESET };
	default:
		return rx_table_cell_struct{ "***UNKNOWN***", ANSI_RX_LOG_UNKNOWN, ANSI_COLOR_RESET };
	}
}


// Class terminal::console::console_commands::sec_command 

sec_command::sec_command()
	: server_command("sec")
{
}


sec_command::~sec_command()
{
}



bool sec_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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
		sub_command.clear();
		in >> sub_command;
		if(sub_command.empty())
		{
			out << get_help();
		}
		else if (sub_command == "active")
		{
			out << "\r\n";
			out << HELP_RX_SEC_ACTIVE_COMMAND;
		}
		else
		{
			err << "Unknown sub-command";
			return false;
		}
	}
	return true;
}

bool sec_command::do_active_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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
		table[i + 1].emplace_back(ctxs[i]->get_port());
	}
	rx_dump_table(table, out,true,true);

	return true;
}

const char* sec_command::get_help () const
{
	return HELP_RX_SEC_COMMAND;
}


// Class terminal::console::console_commands::time_command 

time_command::time_command()
	: server_command("time")
{
}


time_command::~time_command()
{
}



bool time_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	out << "Start Time\t: " << rx_gate::instance().get_started().get_string() << " UTC\r\n";
	out << "Current Time\t: " << rx_time::now().get_string() << " UTC\r\n";
	return true;
}


// Class terminal::console::console_commands::sleep_command 

sleep_command::sleep_command()
	: server_command("sleep")
{
}


sleep_command::~sleep_command()
{
}



bool sleep_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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


// Class terminal::console::console_commands::def_command 

def_command::def_command()
	: directory_aware_command("def")
{
}


def_command::~def_command()
{
}



bool def_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type whose;
	if (!in.eof())
		in >> whose;
	if (!whose.empty())
	{
		platform_item_ptr item = ctx->get_current_directory()->get_sub_item(whose);
		if (item)
		{
			dump_object_definition(out,err, item);
		}
		else
		{
			err << "ERROR: unknown object name";
			return false;
		}
	}
	else
	{
		err << "ERROR: unknown object name";
		return false;
	}
	return true;
}

bool def_command::dump_object_definition (std::ostream& out, std::ostream& err, platform_item_ptr item)
{
	bool ret = false;
	if (item)
	{
		ret =item->generate_json(out, err);
	}
	else
	{
		err << "Unknown Item!!!";
	}
	return ret;
}


// Class terminal::console::console_commands::directory_aware_command 

directory_aware_command::directory_aware_command (const string_type& console_name)
	: server_command(console_name)
{
}


directory_aware_command::~directory_aware_command()
{
}



// Class terminal::console::console_commands::phyton_command 

phyton_command::phyton_command()
	: server_command("python")
{
}


phyton_command::~phyton_command()
{
}



bool phyton_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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


// Class terminal::console::console_commands::license_command 

license_command::license_command()
	: server_command("license")
{
}


license_command::~license_command()
{
}



bool license_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	auto storage = rx_gate::instance().get_host()->get_system_storage();
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


// Class terminal::console::console_commands::help_command 

help_command::help_command()
	: server_command("help")
{
}


help_command::~help_command()
{
}



bool help_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type command_name;
	in >> command_name;
	out << "Printing help, second variant now,\r\n";
	out << RX_CONSOLE_HEADER_LINE "\r\n";
	if (command_name.empty())
	{
		out << get_help();
		return true;
	}
	else
	{//try to find a command
		auto command = terminal::commands::server_command_manager::instance()->get_command_by_name(command_name);
		if (command)
		{
			out << ANSI_COLOR_GREEN << ":>";
			out << ANSI_COLOR_YELLOW ANSI_COLOR_BOLD << command->get_name() << ANSI_COLOR_RESET << "\r\n";
			out << RX_CONSOLE_HEADER_LINE "\r\n";
			out << command->get_help();
			out << "\r\nhope this helps...\r\n";
			return true;
		}
		else
		{
			err << "Unknown command!";
			return false;
		}
	}
}

const char* help_command::get_help () const
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

			std::vector<command_ptr> commands;
			commands::server_command_manager::instance()->get_commands(commands);

			rx_row_type names;
			names.reserve(commands.size());
			for (auto one : commands)
				names.emplace_back(one->get_name(), ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET);

			rx_dump_large_row(names, out, RX_CONSOLE_WIDTH);
			
			out << "\r\nchoose one and type " ANSI_COLOR_GREEN ANSI_COLOR_BOLD "help " ANSI_COLOR_YELLOW ANSI_COLOR_BOLD  "<command>" ANSI_COLOR_RESET " for more details.";
			out << "\r\n\r\nThese are actually code comment dump, at least most of them." ANSI_COLOR_GREEN ANSI_COLOR_BOLD ":)" ANSI_COLOR_RESET "\r\n";
			out << ANSI_COLOR_MAGENTA "Don't know what to tell you, sorry, try reading some other reference...\r\n" ANSI_COLOR_RESET;
			out << ANSI_COLOR_BOLD ANSI_COLOR_RED "/" ANSI_COLOR_YELLOW "/" ANSI_COLOR_GREEN "/" ANSI_COLOR_CYAN "/" ANSI_COLOR_RESET
				<< "  sinclair ZX Spectrum!!!\r\n\r\n";
			out.flush();
			help = out.str();
		}
	}
	return help.c_str();
}


} // namespace console_commands
} // namespace console
} // namespace terminal

