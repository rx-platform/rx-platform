

/****************************************************************************
*
*  sys_internal\rx_ns_commands.cpp
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


// rx_ns_commands
#include "sys_internal/rx_ns_commands.h"



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
		row.emplace_back(one.get_meta().name);

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
		ns::fill_attributes_string(one->get_attributes(), attrs);
		row.emplace_back(attrs);
	}
	if (options.list_qualities || options.list_timestamps)
	{
		values::rx_value val;
		one->get_value(val);

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

	rx_namespace_item item;

	rx_directory_ptr where = ctx->get_current_directory()->get_sub_directory(path);
	if (!where)
	{
		where = ctx->get_current_directory();
		item = where->get_sub_item(path);
		if (!item)
		{
			err << "Item not found!\r\n";
			return false;
		}
	}
	ctx->set_current_directory(where);
	return true;
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



bool ls_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	if (in.eof())
	{// dump here
		string_type filter;
		auto current_directory = ctx->get_current_directory();

		platform_directories_type dirs;
		platform_items_type items;
		current_directory->get_content(dirs, items, filter);

		size_t count = dirs.size() + items.size();

		rx_row_type row;
		row.reserve(count);

		for (auto& one : dirs)
		{
			row.emplace_back(one->get_name(), ANSI_RX_DIR_COLOR, ANSI_COLOR_RESET);
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
				row.emplace_back(one.get_meta().name, ANSI_COLOR_BOLD, ANSI_COLOR_RESET);

		}
		rx_dump_large_row(row, out, RX_CONSOLE_WIDTH);
		return true;

	}
	else
	{
		return list_command::do_console_command(in, out, err, ctx);
	}
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
	directory->get_content(dirs, items, filter);

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
		dump_dirs_on_console(table[idx], options, one, one->get_name());
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
	string_type filter;
	term_list_item_options options;
	options.list_attributes = false;
	options.list_qualities = false;
	options.list_timestamps = false;
	options.list_created = false;
	options.list_type = true;
	options.list_size = false;
	while (!in.eof())
	{
		string_type opt;
		in >> opt;
		if (opt == "-q")
			options.list_qualities = true;
		if (opt == "-t")
			options.list_timestamps = true;
		if (opt == "-a")
			options.list_attributes = true;
		if (opt == "-c")
			options.list_created = true;
		if (opt == "-s")
			options.list_size = true;
		if (opt == "-f" || opt=="-h" || opt=="-l")
		{
			options.list_attributes = true;
			options.list_qualities = true;
			options.list_timestamps = true;
		}
	}

	rx_directory_ptr dir = ctx->get_current_directory();
	return list_directory(out, err, filter, options, dir);
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

	auto ret = ctx->get_current_directory()->add_sub_directory(path);
	if (!ret)
	{
		err << "Error adding directory!\r\n";
		dump_error_result(err, std::move(ret));
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
	string_type path;
	in >> path;

	auto ret = ctx->get_current_directory()->delete_sub_directory(path);
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
	: server_command("clone-system")
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


} // namespace namespace_commands
} // namespace internal_ns
} // namespace rx_internal

