

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

#include "model/rx_meta_internals.h"
#include "terminal/rx_terminal_style.h"
#include "system/meta/rx_obj_types.h"

using namespace rx_platform::api;
using namespace rx_platform::api::meta;


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
	rx_reference<create_data_t> data = ctx->get_instruction_data<create_data_t>();
	if (!data)
	{
		if (!in.eof())
		{
			string_type what;
			in >> what;
			if (!what.empty())
			{
				bool ret = false;
				if (what == "object")
				{
					ret = create_object<object_type>(in, out, err, ctx, tl::type2type<object_type>());
				}
				else if (what == "domain")
				{
					ret = create_object<domain_type>(in, out, err, ctx, tl::type2type<domain_type>());
				}
				else if (what == "app" || what == "application")
				{
					ret = create_object<application_type>(in, out, err, ctx, tl::type2type<application_type>());
				}
				else if (what == "type")
				{
					ret = create_type<object_type>(in, out, err, ctx, tl::type2type<object_type>());
				}
				else
				{
					err << "Unknown type:" << what << "\r\n";
					return false;
				}
				if (ret)
				{
					data = rx_create_reference<create_data_t>();
					data->started = rx_get_us_ticks();
					ctx->set_instruction_data(data);
					ctx->set_waiting();
				}
				return ret;
			}
			else
				err << "Create type is unknown!\r\n";
		}
		else
			err << "Create type is unknown!\r\n";
		return false;
	}
	else
	{// we are returned here
		uint64_t lasted = rx_get_us_ticks() - data->started;
		if (ctx->is_canceled())
		{
			out << "Create was canceled after ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		else
		{
			out << "Create lasted ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		return true;
	}
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
			<< T::RType::type_name
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
			data::runtime_values_data init_data;
			if (reader.read_init_values("Values", init_data))
			{
				rx_context rxc;
				rxc.object = ctx->get_client();
				rxc.directory = ctx->get_current_directory();
				rx_platform::api::meta::rx_create_object(name, class_name, &init_data,
					[=](rx_result_with<rx_object_ptr>&& result)
					{
						if (!result)
						{
							ctx->get_stderr() << "Error creating "
								<< T::RType::type_name << ":\r\n";
							dump_error_result(ctx->get_stderr(), result);
						}
						else
						{
							ctx->get_stdout() << "Created " << T::RType::type_name << " "
								<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
								<< ".\r\n";
						}
						ctx->send_results(result);
					}
					, rxc);
				return true;
			}
			else
			{
				err << "Error deserialization of initialization data for "
					<< T::RType::type_name
					<< " as " << def_command << "!";
				return false;
			}
		}
		else
		{
			err << "Unknown "
				<< T::RType::type_name
				<< " definition specifier:"
				<< def_command << "!";
			return false;
		}
	}
	else if (as_command.empty())
	{
		rx_context rxc;
		rxc.object = ctx->get_client();
		rxc.directory = ctx->get_current_directory();
		rx_platform::api::meta::rx_create_object(name, class_name, nullptr,
			[=](rx_result_with<rx_result_with<rx_object_ptr> >&& result)
			{
				if (!result)
				{
					auto& err = ctx->get_stderr();
					err << "Error creating "
						<< T::RType::type_name << ":\r\n";
					dump_error_result(err, result);
				}
				else
				{
					auto& out = ctx->get_stdout();
					out << "Created " << T::RType::type_name << " "
						<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
						<< ".\r\n";
				}
				ctx->send_results(result);
			}
			, rxc);
			return true;
	}
	else
	{
		err << "Unknown "
			<< T::RType::type_name
			<< " creation type:"
			<< as_command << "!";
		return false;
	}
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
		return dump_types_to_console(tl::type2type<object_types::object_type>(), in, out, err, ctx);
	}
	if (item_type == "ports")
	{
		return dump_types_to_console(tl::type2type<object_types::port_type>(), in, out, err, ctx);
	}
	return true;
}


template<typename T>
bool dump_types_command::dump_types_to_console(tl::type2type<T>, std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
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

	dump_types_recursive(tl::type2type<T>(), start, 0, in, out, err, ctx);

	return true;
}

template<typename T>
bool dump_types_command::dump_types_recursive(tl::type2type<T>, rx_node_id start, int indent, std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type indent_str(indent * 4, ' ');
	const auto& result = platform_types_manager::instance().get_type_cache<T>().get_derived_types(start);
	for (auto one : result.details)
	{
		out << indent_str << one.name << " [" << one.id.to_string() << "]\r\n";
		dump_types_recursive(tl::type2type<T>(), one.id, indent + 1, in, out, err, ctx);
	}
	return true;
}
// Class model::meta_commands::delete_command 

delete_command::delete_command (const string_type& console_name)
	: server_command(console_name)
{
}



bool delete_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	rx_reference<delete_data_t> data = ctx->get_instruction_data<delete_data_t>();
	if (!data)
	{// we just entered to command
		bool ret = false;
		if (!in.eof())
		{
			string_type what;
			in >> what;
			if (!what.empty())
			{
				if (what == "object")
				{
					ret = delete_object<object_type>(in, out, err, ctx, tl::type2type<object_type>());
				}
				else if (what == "port")
				{
					ret = delete_object<port_type>(in, out, err, ctx, tl::type2type<port_type>());
				}
				else if (what == "domain")
				{
					ret = delete_object<domain_type>(in, out, err, ctx, tl::type2type<domain_type>());
				}
				else if (what == "app" || what == "application")
				{
					ret = delete_object<application_type>(in, out, err, ctx, tl::type2type<application_type>());
				}
				else if (what == "object_type")
				{
					ret = delete_type<object_type>(in, out, err, ctx, tl::type2type<object_type>());
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

		if (ret)
		{			
			data = rx_create_reference<delete_data_t>();
			data->started = rx_get_us_ticks();
			ctx->set_instruction_data(data);
			ctx->set_waiting();
		}
		return ret;
	}
	else
	{// we are returned here
		uint64_t lasted = rx_get_us_ticks() - data->started;
		if (ctx->is_canceled())
		{
			out << "Delete was canceled after ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		else
		{
			out << "Delete lasted ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		return true;
	}
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

	rx_context rxc;
	rxc.object = ctx->get_client();
	rx_platform::api::meta::rx_delete_object(name,
		[ctx, name, this](rx_result&& result)
		{
			if (!result)
			{
				auto& err = ctx->get_stderr();
				err << "Error deleting "
					<< T::RType::type_name << ":\r\n";
				this->dump_error_result(err, std::move(result));
			}
			else
			{
				ctx->get_stdout() << "Deleted object "
					<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
					<< ".\r\n";
			}
			ctx->send_results(result);
		}
		, rxc);
	
	return true;
}

template<class T>
bool delete_command::delete_type(std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;

	if (name.empty())
	{
		err << "Error deleting "
			<< T::type_name << " type, name of type is empty";
		return false;
	}

	// these are type definition and stream for creation
	typename T::smart_ptr type_definition;
	typename T::RTypePtr object_ptr;

	platform_types_manager::instance().delete_type<T, console_client::smart_ptr>(name, ctx->get_current_directory(),
		[ctx, name, this](rx_result result)
		{
			if (!result)
			{
				auto& err = ctx->get_stderr();
				err << "Error deleting "
					<< T::type_name << " type:\r\n";
				this->dump_error_result(err, std::move(result));
			}
			else
			{
				ctx->get_stdout() << "Deleted type "
					<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
					<< ".\r\n";
			}
			ctx->send_results(result);
		}
		, ctx->get_client()
	);
	ctx->set_waiting();
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



// Class model::meta_commands::check_command 

check_command::check_command()
	: server_command("check")
{
}


check_command::~check_command()
{
}



bool check_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	rx_reference<check_data_t> data = ctx->get_instruction_data<check_data_t>();
	if (!data)
	{// we just entered to command
		bool ret = false;
		if (!in.eof())
		{
			string_type what;
			in >> what;
			if (!what.empty())
			{
				if (what == "object")
				{
					ret = check_type<object_type>(in, out, err, ctx, tl::type2type<object_type>());
				}
				else if (what == "struct")
				{
					ret = check_simple_type<struct_type>(in, out, err, ctx, tl::type2type<struct_type>());
				}
				else if (what == "port")
				{
					ret = check_type<port_type>(in, out, err, ctx, tl::type2type<port_type>());
				}
				else if (what == "domain")
				{
					ret = check_type<domain_type>(in, out, err, ctx, tl::type2type<domain_type>());
				}
				else if (what == "app" || what == "application")
				{
					ret = check_type<application_type>(in, out, err, ctx, tl::type2type<application_type>());
				}
				else
				{
					err << "Unknown type:" << what << "\r\n";
				}
			}
			else
				err << "Check type is unknown!\r\n";
		}
		else
			err << "Check type is unknown!\r\n";
		
		if (ret)
		{
			data = rx_create_reference<check_data_t>();
			data->started = rx_get_us_ticks();
			ctx->set_instruction_data(data);
			ctx->set_waiting();
		}
		return ret;
	}
	else
	{// we are returned here
		uint64_t lasted = rx_get_us_ticks() - data->started;
		if (ctx->is_canceled())
		{
			out << "Check was canceled after ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		else
		{
			out << "Check lasted ";
			rx_dump_ticks_to_stream(out, lasted);
			out << ".\r\n";
		}
		return true;
	}
}


template<class T>
bool check_command::check_type(std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;
	if (!name.empty())
	{
		platform_types_manager::instance().check_type<T, console_client::smart_ptr>(name, ctx->get_current_directory(),
			[ctx, name, this](type_check_context result)
			{
				if (!result.is_check_ok())
				{
					auto& out = ctx->get_stdout();
					out << T::type_name << " has errors:\r\n";
					for(auto& one : result.get_errors())
						out << ANSI_RX_ERROR_LIST ">>" ANSI_COLOR_RESET << one << "\r\n";
				}
				else
				{
					ctx->get_stdout() << T::type_name << " "
						<< name << "O.K.\r\n";
				}
				ctx->send_results(true);
			}
			, ctx->get_client());
		return true;
	}
	else
	{
		err << "Undefined name!";
		return false;
	}
}
template<class T>
bool check_command::check_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;
	if (!name.empty())
	{
		platform_types_manager::instance().check_simple_type<T, console_client::smart_ptr>(name, ctx->get_current_directory(),
			[ctx, name, this](type_check_context result)
		{
			if (!result.is_check_ok())
			{
				auto& out = ctx->get_stdout();
				out << T::type_name << " has errors:\r\n";
				for (auto& one : result.get_errors())
					out << ANSI_RX_ERROR_LIST ">>" ANSI_COLOR_RESET << one << "\r\n";
			}
			else
			{
				ctx->get_stdout() << T::type_name << " "
					<< name << " O.K.\r\n";
			}
			ctx->send_results(true);
		}
		, ctx->get_client());
		return true;
	}
	else
	{
		err << "Undefined name!";
		return false;
	}
}

} // namespace meta_commands
} // namespace model

