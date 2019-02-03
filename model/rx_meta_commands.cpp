

/****************************************************************************
*
*  model\rx_meta_commands.cpp
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_meta_commands
#include "model/rx_meta_commands.h"

#include "model/rx_meta.h"


namespace model {

namespace meta_commands {

// Class model::meta_commands::create_command 

create_command::create_command()
	: server_command("create")
{
}


create_command::~create_command()
{
}



bool create_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	if (!in.eof())
	{
		string_type what;
		in >> what;
		if (!what.empty())
		{
			if (what == "object")
			{
				return create_object(in, out, err, ctx);
			}
			if (what == "type")
			{
				return create_type(in, out, err, ctx);
			}
			else
			{
				err << "Unknown type:" << what << "\r\n";
			}
		}
		else
			err << "Create type is unknown!\r\n";
	}
	else
		err << "Create type is unknown!\r\n";
	return false;
}

bool create_command::create_object (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type def;
	std::getline(in, def, '\0');
	out << "Hello from create object!!!\r\n";
	return true;
}

bool create_command::create_type (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type def;
	std::getline(in, def, '\0');
	out << "Hello from create object!!!\r\n";
	return true;
}


// Class model::meta_commands::dump_types_command 

dump_types_command::dump_types_command()
	: server_command("dump-types")
{
}


dump_types_command::~dump_types_command()
{
}



bool dump_types_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type item_type;
	in >> item_type;
	if (item_type == "objects")
	{
		return dump_types_to_console(platform_types_manager::instance().get_type_cache<meta::object_types::object_type>(),
			in, out, err, ctx);
	}
	if (item_type == "ports")
	{
		return dump_types_to_console(platform_types_manager::instance().get_type_cache<meta::object_types::port_type>(),
			in, out, err, ctx);
	}
	return true;
}


template<typename T>
bool dump_types_command::dump_types_to_console(model::type_hash<T>& hash, std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type where_from;
	in >> where_from;
	rx_node_id start;
	if (!where_from.empty())
	{
		start = rx_node_id::from_string(where_from.c_str());
	}
	out << "Listing of " << T::get_type_name() << "s\r\n";
	out << RX_CONSOLE_HEADER_LINE << "\r\n";
	out << "Starting from id " << start.to_string() << "\r\n";

	dump_types_recursive(start, 0, hash, in, out, err, ctx);

	return true;
}

template<typename T>
bool dump_types_command::dump_types_recursive(rx_node_id start, int indent, model::type_hash<T>& hash, std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type indent_str(indent * 4, ' ');
	auto result = hash.get_derived_types(start);
	for (auto one : result.details)
	{
		out << indent_str << one.name << " [" << one.id.to_string() << "]\r\n";
		dump_types_recursive(one.id, indent + 1, hash, in, out, err, ctx);
	}
	return true;
}
} // namespace meta_commands
} // namespace model

