

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
#include "terminal/rx_terminal_style.h"
#include "system/meta/rx_obj_types.h"


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
				return create_object<object_type>(in, out, err, ctx, tl::type2type<object_type>());
			}
			if (what == "type")
			{
				return create_type<object_type>(in, out, err, ctx, tl::type2type<object_type>());
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


template<class T>
bool create_command::create_object(std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx, tl::type2type<T>)
{
	string_type name;
	string_type from_command;
	string_type class_name;
	string_type as_command;
	string_type def_command;
	in >> name
		>> from_command
		>> class_name
		>> as_command
		>> def_command;
	// read the rest of it
	string_type definition;
	if (!in.eof())
		std::getline(in, definition, '\0');

	// these are type definition and stream for creation
	typename T::smart_ptr type_definition;
	typename T::RTypePtr object_ptr;

	// try to acquire the type
	if (from_command == "from")
	{
		type_definition = platform_types_manager::instance().get_type<T>(class_name, ctx->get_current_directory());
	}
	else
	{
		err << "Unknown base "
			<< T::type_name
			<< " specifier:"
			<< from_command << "!";
		return false;
	}
	// do we have type
	if (!type_definition)
	{
		err << "Undefined "
			<< T::type_name
			<< ":"
			<< class_name << "!";
		return false;
	}
	// try to acquire definition
	if (as_command == "with")
	{
		if (def_command == "json")
		{
			serialization::json_reader reader;
			reader.parse_data(definition);
			runtime::data::runtime_values_data init_data;
			if (init_data.deserialize(reader))
			{
				object_ptr = platform_types_manager::instance().create_runtime<T>(name, class_name, &init_data, ctx->get_current_directory());
			}
			else
			{
				err << "Error deserialization of initialization data for "
					<< T::type_name
					<< " as " << def_command << "!";
				return false;
			}
		}
		else
		{
			err << "Unknown "
				<< T::type_name
				<< " definition specifier:"
				<< def_command << "!";
			return false;
		}
	}
	if (as_command.empty())
	{
		object_ptr = platform_types_manager::instance().create_runtime<T>(name, class_name, nullptr, ctx->get_current_directory());
	}
	else
	{
		err << "Unknown "
			<< T::type_name
			<< " creation type:"
			<< as_command << "!";
		return false;
	}
	if (object_ptr)
	{
		out << "Created object "
			<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
			<< ".\r\n";
	}
	else
	{
		err << "something went wrong!!!";
		return false;
	}
	return true;
}

template<class T>
bool create_command::create_type(std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx, tl::type2type<T>)
{
	string_type def;
	std::getline(in, def, '\0');
	out << "Hello from create type!!!\r\n";
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
// Class model::meta_commands::rm_command 

rm_command::rm_command()
	: delete_command("rm")
{
}



// Class model::meta_commands::del_command 

del_command::del_command()
	: delete_command("del")
{
}



// Class model::meta_commands::delete_command 

delete_command::delete_command (const string_type& console_name)
	: server_command(console_name)
{
}



bool delete_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	if (!in.eof())
	{
		string_type what;
		in >> what;
		if (!what.empty())
		{
			if (what == "object")
			{
				return delete_object<object_type>(in, out, err, ctx, tl::type2type<object_type>());
			}
			if (what == "type")
			{
				return delete_type<object_type>(in, out, err, ctx, tl::type2type<object_type>());
			}
			else
			{
				err << "Unknown type:" << what << "\r\n";
			}
		}
		else
			err << "Delete type is unknown!\r\n";
	}
	else
		err << "Delete type is unknown!\r\n";
	return false;
}

template<class T>
bool delete_command::delete_object(std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;

	if (name.empty())
	{
		err << "Error deleting "
			<< T::type_name << ", name of object is empty";
		return false;
	}

	// these are type definition and stream for creation
	typename T::smart_ptr type_definition;
	typename T::RTypePtr object_ptr;

	if (!platform_types_manager::instance().delete_runtime<T>(name, ctx->get_current_directory()))
	{
		err << "Error deleting "
			<< T::type_name	<< "!";
		return false;
	}
	return true;
}

template<class T>
bool delete_command::delete_type(std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx, tl::type2type<T>)
{
	string_type def;
	std::getline(in, def, '\0');
	out << "Hello from delete type!!!\r\n";
	return true;
}

} // namespace meta_commands
} // namespace model

