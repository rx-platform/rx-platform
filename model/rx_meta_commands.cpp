

/****************************************************************************
*
*  model\rx_meta_commands.cpp
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


// rx_meta_commands
#include "model/rx_meta_commands.h"

#include "model/rx_meta_internals.h"
#include "terminal/rx_terminal_style.h"
#include "system/meta/rx_obj_types.h"
#include "rx_model_algorithms.h"
#include "sys_internal/rx_internal_ns.h"

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



bool create_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	if (!in.eof())
	{
		string_type what;
		in >> what;
		if (!what.empty())
		{
			auto type_id = rx_parse_type_name(what);
			bool ret = false;
			switch (type_id)
			{
			case rx_item_type::rx_object:
				{
					runtime::objects::object_instance_data instance_data;
					ret = create_object(std::move(instance_data), in, out, err, ctx, tl::type2type<object_type>());
				}
				break;
			case rx_item_type::rx_domain:
				{
					runtime::objects::domain_instance_data instance_data;
					ret = create_object(std::move(instance_data), in, out, err, ctx, tl::type2type<domain_type>());
				}
				break;
			case rx_item_type::rx_port:
				{
					runtime::objects::port_instance_data instance_data;
					ret = create_object(std::move(instance_data), in, out, err, ctx, tl::type2type<port_type>());
				}
				break;
			case rx_item_type::rx_application:
				{
					runtime::objects::application_instance_data instance_data;
					ret = create_object(std::move(instance_data), in, out, err, ctx, tl::type2type<application_type>());
				}
				break;
			case rx_item_type::rx_object_type:
				{
					ret = create_type(in, out, err, ctx, tl::type2type<object_type>());
				}
				break;
			case rx_item_type::rx_application_type:
				{
					ret = create_type(in, out, err, ctx, tl::type2type<application_type>());
				}
				break;
			case rx_item_type::rx_port_type:
				{
					ret = create_type(in, out, err, ctx, tl::type2type<port_type>());
				}
				break;
			case rx_item_type::rx_domain_type:
				{
					ret = create_type(in, out, err, ctx, tl::type2type<domain_type>());
				}
				break;
			case rx_item_type::rx_struct_type:
				{
					ret = create_simple_type(in, out, err, ctx, tl::type2type<struct_type>());
				}
				break;
			case rx_item_type::rx_variable_type:
				{
					ret = create_simple_type(in, out, err, ctx, tl::type2type<variable_type>());
				}
				break;
			case rx_item_type::rx_source_type:
				{
					ret = create_simple_type(in, out, err, ctx, tl::type2type<source_type>());
				}
				break;
			case rx_item_type::rx_filter_type:
				{
					ret = create_simple_type(in, out, err, ctx, tl::type2type<filter_type>());
				}
				break;
			case rx_item_type::rx_event_type:
				{
					ret = create_simple_type(in, out, err, ctx, tl::type2type<event_type>());
				}
				break;
			case rx_item_type::rx_mapper_type:
				{
					ret = create_simple_type(in, out, err, ctx, tl::type2type<mapper_type>());
				}
				break;
			default:
				err << "Unknown type:" << what << "\r\n";
				return false;
			}
			if (ret)
			{
				ctx->set_waiting();
			}
			return ret;
		}
		else
		{
			err << "Create type is unknown!\r\n";
		}
	}
	else
	{
		err << "Create type is unknown!\r\n";
	}
	return false;
}


template<class T>
bool create_command::create_object(typename T::instance_data_t instance_data, std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	string_type from_command;
	string_type base_type_name;
	rx_item_reference base_reference;
	string_type as_command;
	string_type def_command;
	in >> name
		>> from_command
		>> base_type_name
		>> as_command
		>> def_command;
	// read the rest of it
	string_type definition;
	if (!in.eof())
		std::getline(in, definition, '\0');

	typename T::RTypePtr object_ptr;

	//// try to acquire the type
	if (from_command == "from")
	{
		base_reference = base_type_name;
	}
	else
	{
		err << "Unknown base "
			<< rx_item_type_name(T::RType::type_id)
			<< " specifier:"
			<< from_command << "!";
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
			if (reader.read_init_values("values", init_data))
			{
				rx_context rxc;
				rxc.object = ctx->get_client();
				rxc.directory = ctx->get_current_directory();
				auto result = rx_platform::api::meta::rx_create_runtime_implicit<T>(name, base_type_name
					, namespace_item_attributes::namespace_item_full_access, &init_data, std::move(instance_data),
					[=](rx_result_with<typename T::RTypePtr>&& result)
					{
						if (!result)
						{
							ctx->get_stderr() << "Error creating "
								<< rx_item_type_name(T::RType::type_id) << ":\r\n";
							dump_error_result(ctx->get_stderr(), result);
						}
						else
						{
							ctx->get_stdout() << "Created " << rx_item_type_name(T::RType::type_id) << " "
								<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
								<< ".\r\n";
						}
						ctx->send_results(result);
					}, rxc);
				if (!result)
				{
					ctx->get_stderr() << "Error creating "
						<< rx_item_type_name(T::RType::type_id) << ":\r\n";
					dump_error_result(ctx->get_stderr(), result);
				}
				return result;
			}
			else
			{
				err << "Error deserialization of initialization data for "
					<< rx_item_type_name(T::RType::type_id)
					<< " as " << def_command << "!";
				return false;
			}
		}
		else
		{
			err << "Unknown "
				<< rx_item_type_name(T::RType::type_id)
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
		auto result = rx_platform::api::meta::rx_create_runtime_implicit<T>(name, base_type_name
			, namespace_item_attributes::namespace_item_full_access, nullptr, std::move(instance_data),
			[=](rx_result_with<typename T::RTypePtr>&& result)
			{
				if (!result)
				{
					auto& err = ctx->get_stderr();
					err << "Error creating "
						<< rx_item_type_name(T::RType::type_id) << ":\r\n";
					dump_error_result(err, result);
				}
				else
				{
					auto& out = ctx->get_stdout();
					out << "Created " << rx_item_type_name(T::RType::type_id) << " "
						<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
						<< ".\r\n";
				}
				ctx->send_results(result);
			}, rxc);
		if (!result)
		{
			ctx->get_stderr() << "Error creating "
				<< rx_item_type_name(T::RType::type_id) << ":\r\n";
			dump_error_result(ctx->get_stderr(), result);
		}
		return result;
	}
	else
	{
		err << "Unknown "
			<< rx_item_type_name(T::RType::type_id)
			<< " creation type:"
			<< as_command << "!";
		return false;
	}
}
template<class T>
bool create_command::create_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	string_type from_command;
	string_type base_type_name;
	rx_item_reference base_reference;
	string_type as_command;
	string_type def_command;
	in >> name
		>> from_command
		>> base_type_name
		>> as_command
		>> def_command;
	// read the rest of it
	string_type definition;
	if (!in.eof())
		std::getline(in, definition, '\0');

	//// try to acquire the type
	if (from_command == "from")
	{
		base_reference = base_type_name;
	}
	else
	{
		err << "Unknown base "
			<< rx_item_type_name(T::type_id)
			<< " specifier:"
			<< from_command << "!";
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
			if (reader.read_init_values("values", init_data))
			{
				rx_context rxc;
				rxc.object = ctx->get_client();
				rxc.directory = ctx->get_current_directory();
				auto result = rx_platform::api::meta::rx_create_type<T>(name, base_reference, T::smart_ptr::null_ptr, namespace_item_attributes::namespace_item_full_access,
					[=](rx_result_with<typename T::smart_ptr>&& result)
					{
						if (!result)
						{
							ctx->get_stderr() << "Error creating "
								<< rx_item_type_name(T::RType::type_id) << ":\r\n";
							dump_error_result(ctx->get_stderr(), result);
						}
						else
						{
							ctx->get_stdout() << "Created " << rx_item_type_name(T::type_id) << " "
								<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
								<< ".\r\n";
						}
						ctx->send_results(result);
					}, rxc);
				if (!result)
				{
					ctx->get_stderr() << "Error creating "
						<< rx_item_type_name(T::RType::type_id) << ":\r\n";
					dump_error_result(ctx->get_stderr(), result);
				}
				return result;
			}
			else
			{
				err << "Error deserialization of initialization data for "
					<< rx_item_type_name(T::RType::type_id)
					<< " as " << def_command << "!";
				return false;
			}
		}
		else
		{
			err << "Unknown "
				<< rx_item_type_name(T::RType::type_id)
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
		auto result = rx_platform::api::meta::rx_create_type<T>(name, base_reference, T::smart_ptr::null_ptr, namespace_item_attributes::namespace_item_full_access,
			[=](rx_result_with<typename T::smart_ptr>&& result)
			{
				if (!result)
				{
					auto& err = ctx->get_stderr();
					err << "Error creating "
						<< rx_item_type_name(T::type_id) << ":\r\n";
					dump_error_result(err, result);
				}
				else
				{
					auto& out = ctx->get_stdout();
					out << "Created " << rx_item_type_name(T::type_id) << " "
						<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
						<< ".\r\n";
				}
				ctx->send_results(result);
			}
		, rxc);
		if (!result)
		{
			ctx->get_stderr() << "Error creating "
				<< rx_item_type_name(T::RType::type_id) << ":\r\n";
			dump_error_result(ctx->get_stderr(), result);
		}
		return result;
	}
	else
	{
		err << "Unknown "
			<< rx_item_type_name(T::type_id)
			<< " creation type:"
			<< as_command << "!";
		return false;
	}
}

template<class T>
bool create_command::create_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	string_type from_command;
	string_type base_type_name;
	rx_item_reference base_reference;
	string_type as_command;
	string_type def_command;
	in >> name
		>> from_command
		>> base_type_name
		>> as_command
		>> def_command;
	// read the rest of it
	string_type definition;
	if (!in.eof())
		std::getline(in, definition, '\0');

	//// try to acquire the type
	if (from_command == "from")
	{
		base_reference = base_type_name;
	}
	else
	{
		err << "Unknown base "
			<< rx_item_type_name(T::type_id)
			<< " specifier:"
			<< from_command << "!";
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
			if (reader.read_init_values("values", init_data))
			{
				rx_context rxc;
				rxc.object = ctx->get_client();
				rxc.directory = ctx->get_current_directory();
				auto result = rx_platform::api::meta::rx_create_simple_type<T>(name, base_reference, T::smart_ptr::null_ptr, namespace_item_attributes::namespace_item_full_access,
					[=](rx_result_with<typename T::smart_ptr>&& result)
					{
						if (!result)
						{
							ctx->get_stderr() << "Error creating "
								<< rx_item_type_name(T::type_id) << ":\r\n";
							dump_error_result(ctx->get_stderr(), result);
						}
						else
						{
							ctx->get_stdout() << "Created " << rx_item_type_name(T::type_id) << " "
								<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
								<< ".\r\n";
						}
						ctx->send_results(result);
					}, rxc);
				if (!result)
				{
					ctx->get_stderr() << "Error creating "
						<< rx_item_type_name(T::type_id) << ":\r\n";
					dump_error_result(ctx->get_stderr(), result);
				}
				return result;
			}
			else
			{
				err << "Error deserialization of initialization data for "
					<< rx_item_type_name(T::type_id)
					<< " as " << def_command << "!";
				return false;
			}
		}
		else
		{
			err << "Unknown "
				<< rx_item_type_name(T::type_id)
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
		auto result = rx_platform::api::meta::rx_create_simple_type<T>(name, base_reference, T::smart_ptr::null_ptr, namespace_item_attributes::namespace_item_full_access,
			[=](rx_result_with<typename T::smart_ptr>&& result)
			{
				if (!result)
				{
					auto& err = ctx->get_stderr();
					err << "Error creating "
						<< rx_item_type_name(T::type_id) << ":\r\n";
					dump_error_result(err, result);
				}
				else
				{
					auto& out = ctx->get_stdout();
					out << "Created " << rx_item_type_name(T::type_id) << " "
						<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
						<< ".\r\n";
				}
				ctx->send_results(result);
			}
		, rxc);
		if (!result)
		{
			ctx->get_stderr() << "Error creating "
				<< rx_item_type_name(T::type_id) << ":\r\n";
			dump_error_result(ctx->get_stderr(), result);
		}
		return result;
	}
	else
	{
		err << "Unknown "
			<< rx_item_type_name(T::type_id)
			<< " creation type:"
			<< as_command << "!";
		return false;
	}
}
// Class model::meta_commands::dump_types_command

dump_types_command::dump_types_command()
	: server_command("dump-types")
{
}


dump_types_command::~dump_types_command()
{
}



bool dump_types_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type item_type;
	in >> item_type;
	if (item_type == "objects")
	{
		return dump_types_to_console(tl::type2type<object_types::object_type>(), in, out, err, ctx);
	}
	else if (item_type == "ports")
	{
		return dump_types_to_console(tl::type2type<object_types::port_type>(), in, out, err, ctx);
	}
	else if (item_type == "apps" || item_type == "applications")
	{
		return dump_types_to_console(tl::type2type<object_types::application_type>(), in, out, err, ctx);
	}
	else if (item_type == "domains")
	{
		return dump_types_to_console(tl::type2type<object_types::domain_type>(), in, out, err, ctx);
	}
	return true;
}


template<typename T>
bool dump_types_command::dump_types_to_console(tl::type2type<T>, std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type where_from;
	in >> where_from;
	rx_node_id start;
	if (!where_from.empty())
	{
		start = rx_node_id::from_string(where_from.c_str());
	}
	out << "Listing of " << rx_item_type_name(T::type_id) << "s\r\n";
	out << RX_CONSOLE_HEADER_LINE << "\r\n";
	out << "Starting from id " << start.to_string() << "\r\n";

	dump_types_recursive(tl::type2type<T>(), start, 0, in, out, err, ctx);

	return true;
}

template<typename T>
bool dump_types_command::dump_types_recursive(tl::type2type<T>, rx_node_id start, int indent, std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type indent_str(indent * 4ll, ' ');
	auto result = platform_types_manager::instance().get_type_repository<T>().get_derived_types(start);
	for (auto one : result.items)
	{
		out << indent_str << one.data.get_name() << " [" << one.data.get_id().to_string() << "]\r\n";
		dump_types_recursive(tl::type2type<T>(), one.data.get_id(), indent + 1, in, out, err, ctx);
	}
	return true;
}
// Class model::meta_commands::delete_command

delete_command::delete_command (const string_type& console_name)
	: server_command(console_name)
{
}



bool delete_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	bool ret = false;
	if (!in.eof())
	{
		string_type what;
		in >> what;
		if (!what.empty())
		{
			auto type_id = rx_parse_type_name(what);
			switch (type_id)
			{
			case rx_item_type::rx_object:
				{
					ret = delete_object(in, out, err, ctx, tl::type2type<object_type>());
				}
				break;
			case rx_item_type::rx_application:
				{
					ret = delete_object(in, out, err, ctx, tl::type2type<application_type>());
				}
				break;
			case rx_item_type::rx_port:
				{
					ret = delete_object(in, out, err, ctx, tl::type2type<port_type>());
				}
				break;
			case rx_item_type::rx_domain:
				{
					ret = delete_object(in, out, err, ctx, tl::type2type<domain_type>());
				}
				break;
			case rx_item_type::rx_object_type:
				{
					ret = delete_type(in, out, err, ctx, tl::type2type<object_type>());
				}
				break;
			case rx_item_type::rx_application_type:
				{
					ret = delete_type(in, out, err, ctx, tl::type2type<application_type>());
				}
				break;
			case rx_item_type::rx_port_type:
				{
					ret = delete_type(in, out, err, ctx, tl::type2type<port_type>());
				}
				break;
			case rx_item_type::rx_domain_type:
				{
					ret = delete_type(in, out, err, ctx, tl::type2type<domain_type>());
				}
				break;
			case rx_item_type::rx_struct_type:
				{
					ret = delete_simple_type(in, out, err, ctx, tl::type2type<struct_type>());
				}
				break;
			case rx_item_type::rx_variable_type:
				{
					ret = delete_simple_type(in, out, err, ctx, tl::type2type<variable_type>());
				}
				break;
			case rx_item_type::rx_source_type:
				{
					ret = delete_simple_type(in, out, err, ctx, tl::type2type<source_type>());
				}
				break;
			case rx_item_type::rx_filter_type:
				{
					ret = delete_simple_type(in, out, err, ctx, tl::type2type<filter_type>());
				}
				break;
			case rx_item_type::rx_event_type:
				{
					ret = delete_simple_type(in, out, err, ctx, tl::type2type<event_type>());
				}
				break;
			case rx_item_type::rx_mapper_type:
				{
					ret = delete_simple_type(in, out, err, ctx, tl::type2type<mapper_type>());
				}
				break;
			default:
				err << "Unknown type:" << what << "\r\n";
				return false;
			}
		}
		else
			err << "Delete type is unknown!\r\n";
	}
	else
		err << "Delete type is unknown!\r\n";

	if (ret)
	{
		ctx->set_waiting();
	}
	return ret;
}

template<class T>
bool delete_command::delete_object(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;

	if (name.empty())
	{
		err << "Error deleting "
			<< rx_item_type_name(T::type_id) << ", name of object is empty";
		return false;
	}

	// these are type definition and stream for creation
	typename T::smart_ptr type_definition;
	typename T::RTypePtr object_ptr;

	rx_context rxc;
	rxc.object = ctx->get_client();
	rxc.directory = ctx->get_current_directory();
	auto result = rx_platform::api::meta::rx_delete_runtime<T>(rx_item_reference(name),
		[ctx, name, this](rx_result&& result)
		{
			if (!result)
			{
				auto& err = ctx->get_stderr();
				err << "Error deleting "
					<< rx_item_type_name(T::RType::type_id) << ":\r\n";
				rx_dump_error_result(err, std::move(result));
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

	if (!result)
	{
		err << "Error deleting "
			<< rx_item_type_name(T::type_id) << "\r\n";
		for (const auto& one : result.errors())
		{
			err << one << "\r\n";
		}
	}
	return result;
}

template<class T>
bool delete_command::delete_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;

	if (name.empty())
	{
		err << "Error deleting "
			<< rx_item_type_name(T::type_id) << " type, name of type is empty";
		return false;
	}

	// these are type definition and stream for creation
	typename T::smart_ptr type_definition;
	typename T::RTypePtr object_ptr;

	algorithms::types_model_algorithm<T>::delete_type(name, ctx->get_current_directory(),
		[ctx, name, this](rx_result result)
		{
			if (!result)
			{
				auto& err = ctx->get_stderr();
				err << "Error deleting "
					<< rx_item_type_name(T::type_id) << " type:\r\n";
				rx_dump_error_result(err, std::move(result));
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

template<class T>
bool delete_command::delete_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;

	if (name.empty())
	{
		err << "Error deleting "
			<< rx_item_type_name(T::type_id) << " type, name of type is empty";
		return false;
	}

	// these are type definition and stream for creation
	typename T::smart_ptr type_definition;
	typename T::RTypePtr object_ptr;

	algorithms::simple_types_model_algorithm<T>::delete_type(name, ctx->get_current_directory(),
		[ctx, name, this](rx_result result)
		{
			if (!result)
			{
				auto& err = ctx->get_stderr();
				err << "Error deleting "
					<< rx_item_type_name(T::type_id) << " type:\r\n";
				rx_dump_error_result(err, std::move(result));
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



bool check_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{

	bool ret = false;
	if (!in.eof())
	{
		string_type what;
		in >> what;
		if (!what.empty())
		{
			auto type_id = rx_parse_type_name(what);
			switch (type_id)
			{
			case rx_item_type::rx_object_type:
				{
					ret = check_type(in, out, err, ctx, tl::type2type<object_type>());
				}
				break;
			case rx_item_type::rx_application_type:
				{
					ret = check_type(in, out, err, ctx, tl::type2type<application_type>());
				}
				break;
			case rx_item_type::rx_port_type:
				{
					ret = check_type(in, out, err, ctx, tl::type2type<port_type>());
				}
				break;
			case rx_item_type::rx_domain_type:
				{
					ret = check_type(in, out, err, ctx, tl::type2type<domain_type>());
				}
				break;
			case rx_item_type::rx_struct_type:
				{
					ret = check_simple_type(in, out, err, ctx, tl::type2type<struct_type>());
				}
				break;
			case rx_item_type::rx_variable_type:
				{
					ret = check_simple_type(in, out, err, ctx, tl::type2type<variable_type>());
				}
				break;
			case rx_item_type::rx_source_type:
				{
					ret = check_simple_type(in, out, err, ctx, tl::type2type<source_type>());
				}
				break;
			case rx_item_type::rx_filter_type:
				{
					ret = check_simple_type(in, out, err, ctx, tl::type2type<filter_type>());
				}
				break;
			case rx_item_type::rx_event_type:
				{
					ret = check_simple_type(in, out, err, ctx, tl::type2type<event_type>());
				}
				break;
			case rx_item_type::rx_mapper_type:
				{
					ret = check_simple_type(in, out, err, ctx, tl::type2type<mapper_type>());
				}
				break;
			default:
				err << "Unknown type:" << what << "\r\n";
				return false;
			}
		}
		else
			err << "Check type is unknown!\r\n";
	}
	else
		err << "Check type is unknown!\r\n";

	if (ret)
	{
		ctx->set_waiting();
	}
	return ret;
}


template<class T>
bool check_command::check_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;
	if (!name.empty())
	{
		algorithms::types_model_algorithm<T>::check_type(name, ctx->get_current_directory(),
			[ctx, name, this](type_check_context result)
			{
				if (!result.is_check_ok())
				{
					auto& out = ctx->get_stdout();
					out << rx_item_type_name(T::type_id) << " has errors:\r\n";
					for(auto& one : result.get_errors())
						out << ANSI_RX_ERROR_LIST ">>" ANSI_COLOR_RESET << one << "\r\n";
				}
				else
				{
					ctx->get_stdout() << rx_item_type_name(T::type_id) << " "
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
bool check_command::check_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	in >> name;
	if (!name.empty())
	{
		algorithms::simple_types_model_algorithm<T>::check_type(name, ctx->get_current_directory(),
			[ctx, name, this](type_check_context result)
		{
			if (!result.is_check_ok())
			{
				auto& out = ctx->get_stdout();
				out << rx_item_type_name(T::type_id) << " has errors:\r\n";
				for (auto& one : result.get_errors())
					out << ANSI_RX_ERROR_LIST ">>" ANSI_COLOR_RESET << one << "\r\n";
			}
			else
			{
				ctx->get_stdout() << rx_item_type_name(T::type_id) << " "
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
// Class model::meta_commands::prototype_command

prototype_command::prototype_command()
	: server_command("proto")
{
}


prototype_command::~prototype_command()
{
}



bool prototype_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	if (!in.eof())
	{
		string_type what;
		in >> what;
		if (!what.empty())
		{
			bool ret = false;
			auto type_id = rx_parse_type_name(what);
			switch (type_id)
			{
			case rx_item_type::rx_object:
				{
					ret = create_prototype(in, out, err, ctx, tl::type2type<object_type>());
				}
				break;
			case rx_item_type::rx_application:
				{
					ret = create_prototype(in, out, err, ctx, tl::type2type<application_type>());
				}
				break;
			case rx_item_type::rx_port:
				{
					ret = create_prototype(in, out, err, ctx, tl::type2type<port_type>());
				}
				break;
			case rx_item_type::rx_domain:
				{
					ret = create_prototype(in, out, err, ctx, tl::type2type<domain_type>());
				}
				break;
			default:
				err << "Unknown type:" << what << "\r\n";
				return false;
			}
			if (ret)
			{
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

template<class T>
bool prototype_command::create_prototype(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>)
{
	string_type name;
	string_type from_command;
	string_type base_type_name;
	rx_item_reference base_reference;
	string_type to_command;
	string_type def_command;
	in >> name
		>> from_command
		>> base_type_name
		>> to_command
		>> def_command;

	// these are type definition and stream for creation
	typename T::smart_ptr type_definition;
	typename T::RTypePtr object_ptr;

	//// try to acquire the type
	if (from_command == "from")
	{
		base_reference = base_type_name;
	}
	else
	{
		err << "Unknown base "
			<< rx_item_type_name(T::type_id)
			<< " specifier:"
			<< from_command << "!";
		return false;
	}

	if (to_command == "to")
	{
		if (def_command == "json")
		{

			rx_context rxc;
			rxc.object = ctx->get_client();
			rxc.directory = ctx->get_current_directory();
			meta_data info;
			typename T::instance_data_t data{};
			auto result = rx_platform::api::meta::rx_create_prototype<T>(info, data,
				[=](rx_result_with<typename T::RTypePtr>&& result)
			{
				bool ret = result;
				if (!result)
				{
					auto& err = ctx->get_stderr();
					err << "Error prototyping "
						<< rx_item_type_name(T::RType::type_id) << ":\r\n";
					dump_error_result(err, result);
				}
				else
				{
					auto& out = ctx->get_stdout();

					out << "Prototyped " << rx_item_type_name(T::RType::type_id) << " "
						<< ANSI_RX_OBJECT_COLOR << name << ANSI_COLOR_RESET
						<< ".\r\n";
					out << result.value()->get_item_ptr()->get_definition_as_json();
				}
				ctx->send_results(ret);
			}, rxc);
			if (!result)
			{
				ctx->get_stderr() << "Error creating "
					<< rx_item_type_name(T::RType::type_id) << ":\r\n";
				dump_error_result(ctx->get_stderr(), result);
			}
			return result;
		}
		else
		{
			err << "Unknown "
				<< rx_item_type_name(T::RType::type_id)
				<< " target type!";
			return false;
		}
	}
	else
	{
		err << "Unknown "
			<< rx_item_type_name(T::RType::type_id)
			<< " target!";
		return false;
	}
}
// Class model::meta_commands::save_command

save_command::save_command()
	: server_command("save")
{
}


save_command::~save_command()
{
}



bool save_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	bool ret = false;
	if (!in.eof())
	{
		string_type path;
		in >> path;
		if (!path.empty())
		{
			api::rx_context rx_ctx = ctx->create_api_context();
			auto result = rx_platform::api::meta::rx_save_item(path,
				[ctx, path, this](rx_result result)
				{
					if (!result)
					{
						auto& err = ctx->get_stderr();
						err << "Error saving item\r\n";
						rx_dump_error_result(err, std::move(result));
					}
					else
					{
						ctx->get_stdout() << "Saved item "
							<< ANSI_RX_OBJECT_COLOR << path << ANSI_COLOR_RESET
							<< ".\r\n";
					}
					ctx->send_results(result);
				}, rx_ctx);
			if (!result)
			{
				auto& err = ctx->get_stderr();
				err << "Error saving item\r\n";
				rx_dump_error_result(err, std::move(result));
			}
			else
				ctx->set_waiting();

			ret = result;
		}
		else
			err << "Specify item to save!\r\n";
	}
	else
		err << "Specify item to save!\r\n";

	return ret;
}


} // namespace meta_commands
} // namespace model

