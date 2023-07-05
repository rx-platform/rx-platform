

/****************************************************************************
*
*  sys_internal\rx_ns_commands.cpp
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


// rx_ns_commands
#include "sys_internal/rx_ns_commands.h"

#include "terminal/rx_console.h"
#include "sys_internal/rx_namespace_algorithms.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "terminal/rx_term_table.h"
#include "system/server/rx_directory_cache.h"
#include "terminal/parser3000.h"


namespace rx_internal {

namespace internal_ns {

namespace namespace_commands {
namespace
{
bool dump_items_on_console(rx_row_type& row, const term_list_item_options& options, const ns::rx_namespace_item& one)
{
	if (one.is_type())
		row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_TYPE_COLOR, ANSI_COLOR_RESET });
	else if (one.is_object())
	{
		switch (one.get_type())
		{
		case rx_item_type::rx_application:
			row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_APP_COLOR, ANSI_COLOR_RESET });
			break;
		case rx_item_type::rx_domain:
			row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_DOMAIN_COLOR, ANSI_COLOR_RESET });
			break;
		case rx_item_type::rx_port:
			row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_PORT_COLOR, ANSI_COLOR_RESET });
			break;
		case rx_item_type::rx_object:
			row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET });
			break;
        default:
            RX_ASSERT(false);
            row.emplace_back(one.get_meta().name);
		}
	}
	else
		row.emplace_back(one.get_meta().name, ANSI_RX_DATA_COLOR, ANSI_COLOR_RESET);

	if (options.list_type)
	{
		row.emplace_back(rx_item_type_name(one.get_type()));
	}
	if (options.list_attributes)
	{
		string_type attrs;
		ns::fill_attributes_string(one.get_meta().attributes, attrs);
		row.emplace_back(attrs);
	}
	if (options.list_qualities || options.list_timestamps)
	{
		values::rx_value val = one.get_value();

		if (options.list_qualities)
		{
			string_type quality_stirng;
			fill_quality_string(val, quality_stirng);
			row.emplace_back(quality_stirng);
		}
		if (options.list_timestamps)
			row.emplace_back(val.get_time().get_string());
	}
	if (options.list_created)
	{
		row.emplace_back(one.get_meta().created_time.get_string());
	}

	return true;
}


bool dump_dirs_on_console(rx_row_type& row, const term_list_item_options& options, rx_directory_ptr one, const string_type& name)
{

	row.emplace_back(name, ANSI_RX_DIR_COLOR, ANSI_COLOR_RESET);
	if (options.list_type)
	{
		row.emplace_back(RX_CPP_DIRECORY_TYPE_NAME);
	}
	if (options.list_attributes)
	{
		string_type attrs;
		ns::fill_attributes_string(one->meta_info().attributes, attrs);
		row.emplace_back(attrs);
	}
	if (options.list_qualities || options.list_timestamps)
	{
		values::rx_value val;
		val.set_time(one->meta_info().modified_time);
		val.set_good_locally();

		if (options.list_qualities)
		{
			string_type quality_stirng;
			fill_quality_string(val, quality_stirng);
			row.emplace_back(quality_stirng);
		}
		if (options.list_timestamps)
			row.emplace_back(val.get_time().get_string());
	}
	if (options.list_size)
	{
		std::ostringstream temp;
		temp << 55;
		row.emplace_back(temp.str());
	}
	if (options.list_created)
	{
		row.emplace_back(one->meta_info().created_time.get_string());
	}

	return true;
}

}

// Class rx_internal::internal_ns::namespace_commands::cd_command 

cd_command::cd_command()
	: server_command("cd")
{
}


cd_command::~cd_command()
{
}



bool cd_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type path;
	in >> path;

	if (path.empty() || path == ".")
		return true;

	rx_namespace_item item;
	string_type full_path;
	auto res = internal_ns::namespace_algorithms::translate_path(ctx->get_current_directory(), path, full_path);
	if (!res)
	{
		err << RX_INVALID_PATH ":";
		err << res.errors_line();
		return false;
	}
	rx_directory_ptr where = ns::rx_directory_cache::instance().get_directory(full_path);
	if (!where)
	{
		err << RX_INVALID_PATH;
		return false;
	}
	ctx->set_current_directory(full_path);
	return true;
}

void cd_command::register_suggestions (const string_type& line, suggestions_type& suggestions)
{
}


// Class rx_internal::internal_ns::namespace_commands::dir_command 

dir_command::dir_command()
	: list_command("dir")
{
}


dir_command::~dir_command()
{
}



// Class rx_internal::internal_ns::namespace_commands::ls_command 

ls_command::ls_command()
	: list_command("ls")
{
}


ls_command::~ls_command()
{
}



// Class rx_internal::internal_ns::namespace_commands::list_command 

list_command::list_command (const string_type& console_name)
	: server_command(console_name)
{
}


list_command::~list_command()
{
}



bool list_command::list_directory (std::ostream& out, std::ostream& err, const string_type& filter, const term_list_item_options& options, rx_directory_ptr directory)
{
	platform_directories_type dirs;
	platform_items_type items;


	ns::rx_directory_resolver resolver;


	directory->list_content(dirs, items, filter);

	size_t count = dirs.size() + items.size();

	rx_table_type table(count + 1);


	table[0].emplace_back("Name");
	if (options.list_type)
		table[0].emplace_back("Type");
	if (options.list_attributes)
		table[0].emplace_back("Attributes");
	if (options.list_qualities)
		table[0].emplace_back("Quality");
	if (options.list_timestamps)
		table[0].emplace_back("Time Stamp");
	if (options.list_size)
		table[0].emplace_back("Size");
	if (options.list_created)
		table[0].emplace_back("Created Time");

	size_t idx = 1;
	for (auto& one : dirs)
	{
		dump_dirs_on_console(table[idx], options, one, one->meta_info().name);
		idx++;
	}
	for (auto& one : items)
	{
		dump_items_on_console(table[idx], options, one);
		idx++;
	}

	rx_dump_table(table, out, true, false);

	return true;
}

bool list_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{

	string_type path = ".";

	string_type filter;
	term_list_item_options options;

	options.list_attributes = false;
	options.list_qualities = false;
	options.list_timestamps = false;
	options.list_created = false;
	options.list_type = true;
	options.list_size = false;
	bool list = false;

	if (!in.eof())
	{
		auto pos = in.tellg();
		in >> path;
		if (!path.empty() && path[0] == '-')
		{
			path = "";
			in.seekg(pos);
		}

		using parser_t = urke::parser::parser3000;
		parser_t parser;

		bool help = false;
		bool version = false;

		parser.add_bit_option('a', "attr", &options.list_attributes, "Display attributes.");
		parser.add_bit_option('h', "help", &help, "Prints help.");
		parser.add_bit_option('v', "version", &version, "Prints version of the command.");
		parser.add_bit_option('l', "long", &list, "Long format of list.");
		parser.add_bit_option('t', "time", &options.list_timestamps, "List timestamps.");
		parser.add_bit_option('q', "quality", &options.list_qualities, "List qualities.");
		parser.add_string_option('f', "filter", &filter, "Search pattern filter.");
		parser.add_bit_option('y', "type", &options.list_type, "List types.");




		auto ret = parser.parse(in, err);
		if (ret)
		{
			if (help)
			{
				parser.print_help("ls [path] ", out);
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
		}
		else
		{
			return ret;
		}
	}

	rx_directory_resolver resl;
	resl.add_paths({ ctx->get_current_directory() });
	rx_directory_ptr dir = resl.resolve_directory(path);
	if (!dir)
	{
		err << RX_INVALID_PATH;
		return false;
	}
	if (list)
	{
		return list_directory(out, err, filter, options, dir);
	}
	else
	{
		return do_non_list_command(out, err, filter, options, dir, ctx);
	}
}

bool list_command::do_non_list_command (std::ostream& out, std::ostream& err, const string_type& filter, const term_list_item_options& options, rx_directory_ptr directory, console_context_ptr ctx)
{


	platform_items_type items;
	platform_directories_type dirs;

	directory->list_content(dirs, items, filter);

	size_t count = dirs.size() + items.size();

	rx_row_type row;
	row.reserve(count);

	for (auto& one : dirs)
	{
		row.emplace_back(one->meta_info().name, ANSI_RX_DIR_COLOR, ANSI_COLOR_RESET);
	}
	for (auto& one : items)
	{
		if (one.is_type())
			row.emplace_back(one.get_meta().name, ANSI_RX_TYPE_COLOR, ANSI_COLOR_RESET);
		else if (one.is_object())
		{
			switch (one.get_type())
			{
			case rx_item_type::rx_application:
				row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_APP_COLOR, ANSI_COLOR_RESET });
				break;
			case rx_item_type::rx_domain:
				row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_DOMAIN_COLOR, ANSI_COLOR_RESET });
				break;
			case rx_item_type::rx_port:
				row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_PORT_COLOR, ANSI_COLOR_RESET });
				break;
			case rx_item_type::rx_object:
				row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET });
				break;
			default:
				row.emplace_back(one.get_meta().name, ANSI_COLOR_BOLD, ANSI_COLOR_RESET);
			}
		}
		else
			row.emplace_back(one.get_meta().name, ANSI_RX_DATA_COLOR, ANSI_COLOR_RESET);

	}
	rx_dump_large_row(row, out, ctx->get_terminal_width());
	return true;
}


// Class rx_internal::internal_ns::namespace_commands::mkdir_command 

mkdir_command::mkdir_command()
	: server_command("mkdir")
{
}


mkdir_command::~mkdir_command()
{
}



bool mkdir_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type path;
	in >> path;

	auto dir = ns::rx_directory_cache::instance().get_directory(ctx->get_current_directory());
	if (!dir)
	{
		err << RX_INVALID_PATH;
		return false;
	}

	auto ret = internal_ns::namespace_algorithms::get_or_create_direcotry(dir, path);
	if (!ret)
	{
		err << "Error adding directory!\r\n";
		ctx->raise_error(ret);
		return false;
	}
	return true;
}


// Class rx_internal::internal_ns::namespace_commands::rmdir_command 

rmdir_command::rmdir_command()
	: server_command("rmdir")
{
}


rmdir_command::~rmdir_command()
{
}



bool rmdir_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	
  // TODO rmdir is now a just for empty directories
	
	string_type path;
	in >> path;

	string_type full_path;
	auto ret = internal_ns::namespace_algorithms::translate_path(ctx->get_current_directory(), path, full_path);
	if (!ret)
	{
		err << RX_INVALID_PATH "!\r\n";
		rx_dump_error_result(err, std::move(ret));
	}
	ret = ns::rx_directory_cache::instance().remove_directory(full_path);
	if (!ret)
	{
		err << "Error deleting directory!\r\n";
		rx_dump_error_result(err, std::move(ret));
		return false;
	}
	return true;
}


// Class rx_internal::internal_ns::namespace_commands::clone_system_command 

clone_system_command::clone_system_command()
	: server_command("clonesys")
{
}


clone_system_command::~clone_system_command()
{
}



bool clone_system_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	out << "Cloning rx-platform system types and objects...\r\n";
	err << "Nema jos jebiga!!!\r\n";
	return false;
}


// Class rx_internal::internal_ns::namespace_commands::move_command 

move_command::move_command()
	: server_command("mv")
{
}


move_command::~move_command()
{
}



bool move_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	err << RX_NOT_IMPLEMENTED;
	return false;

	/*string_type old_path;
	string_type new_path;
	in >> old_path;
	in >> new_path;

	auto ret = ctx->get_current_directory()->move_directory(old_path, new_path);
	if (!ret)
	{
		err << "Error deleting directory!\r\n";
		rx_dump_error_result(err, std::move(ret));
		return false;
	}
	return true;
	*/
}


// Class rx_internal::internal_ns::namespace_commands::clone_command 

clone_command::clone_command()
	: server_command("cp")
{
}


clone_command::~clone_command()
{
}



bool clone_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	err << RX_NOT_IMPLEMENTED;
	return false;

	/*string_type old_path;
	string_type new_path;
	in >> old_path;
	in >> new_path;

	auto ret = ctx->get_current_directory()->copy_directory(old_path, new_path);
	if (!ret)
	{
		err << "Error deleting directory!\r\n";
		rx_dump_error_result(err, std::move(ret));
		return false;
	}
	return true;
	*/
}


} // namespace namespace_commands
} // namespace internal_ns
} // namespace rx_internal



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	if (in.eof())
	{// dump here
		string_type filter;
		auto current_directory = ns::rx_directory_cache::instance().get_directory(ctx->get_current_directory());

		platform_directories_type dirs{ current_directory };
		platform_items_type items;
		current_directory->list_content(dirs, items, filter);

		size_t count = dirs.size() + items.size();

		rx_row_type row;
		row.reserve(count);

		for (auto& one : dirs)
		{
			row.emplace_back(one->meta_info().name, ANSI_RX_DIR_COLOR, ANSI_COLOR_RESET);
		}
		for (auto& one : items)
		{
			if (one.is_type())
				row.emplace_back(one.get_meta().name, ANSI_RX_TYPE_COLOR, ANSI_COLOR_RESET);
			else if (one.is_object())
			{
				switch (one.get_type())
				{
				case rx_item_type::rx_application:
					row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_APP_COLOR, ANSI_COLOR_RESET });
					break;
				case rx_item_type::rx_domain:
					row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_DOMAIN_COLOR, ANSI_COLOR_RESET });
					break;
				case rx_item_type::rx_port:
					row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_PORT_COLOR, ANSI_COLOR_RESET });
					break;
				case rx_item_type::rx_object:
					row.emplace_back(rx_table_cell_struct{ one.get_meta().name, ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET });
					break;
				default:
					row.emplace_back(one.get_meta().name, ANSI_COLOR_BOLD, ANSI_COLOR_RESET);
				}
			}
			else
				row.emplace_back(one.get_meta().name, ANSI_RX_DATA_COLOR, ANSI_COLOR_RESET);

		}
		rx_dump_large_row(row, out, ctx->get_terminal_width());
		return true;

	}
	else
	{
		return list_command::do_console_command(in, out, err, ctx);
	}

#endif
