

/****************************************************************************
*
*  terminal\rx_commands.cpp
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


// rx_con_programs
#include "terminal/rx_con_programs.h"
// rx_commands
#include "terminal/rx_commands.h"

#include "system/server/rx_server.h"
#include "terminal/rx_con_commands.h"
#include "testing/rx_test.h"
#include "model/rx_meta_commands.h"
#include "sys_internal/rx_ns_commands.h"
#include "runtime_internal/rx_runtime_commands.h"
#include "sys_internal/rx_plugin_manager.h"
#include "rx_console.h"
#include "sys_internal/rx_internal_ns.h"
#include "model/rx_meta_internals.h"
#include "sys_internal/rx_internal_protocol.h"
#include "terminal/rx_vt100.h"
#include "terminal/rx_telnet.h"
#include "protocols/opcua/rx_opcua_mapping.h"
#include "protocols/http/rx_http_mapping.h"
#include "interfaces/rx_io.h"


namespace rx_internal {

namespace terminal {

namespace commands {
#include "others/others.h"
namespace
{
server_command_manager::smart_ptr g_inst;
void read_to_eol(string_type::const_iterator& it, const string_type& data, std::ostringstream& out)
{
	while (it != data.end() && *it != '\r' && *it != '\n')
	{
		out << *it;
		it++;
	}
}
string_type parse_man_file(const string_type& data)
{
	std::ostringstream out;

	auto it = data.begin();
	while (it != data.end())
	{
		if (*it == '.')
		{// macro stuff
			it++;
			string_type macro;
			auto it_temp = it;
			while (it != data.end() && !std::isblank(*it))
				it++;
			std::copy(it_temp, it, std::back_inserter(macro));
			if (it != data.end())
				it++;
			if (macro == "B")
			{
				out << ANSI_COLOR_BOLD ANSI_COLOR_YELLOW;
				read_to_eol(it, data, out);
				out << ANSI_COLOR_RESET;
			}
		}
		else
			read_to_eol(it, data, out);
		while (it != data.end() && (*it == '\r' || *it == '\n'))
		{
			out << *it;
			it++;
		}
	}
	return out.str();
}
}

// Class rx_internal::terminal::commands::server_command_manager 

server_command_manager::server_command_manager()
{
	register_others(other_commands_);
}


server_command_manager::~server_command_manager()
{
}



void server_command_manager::register_command (server_command_base_ptr cmd)
{
	string_type con_name = cmd->get_console_name();
	locks::auto_lock dummy(&lock_);
	registered_commands_.emplace(con_name, cmd);
}

void server_command_manager::register_internal_commands ()
{
	
	// register constructors
	auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
		RX_COMMANDS_MANAGER_TYPE_ID, [] {
			return server_command_manager::instance();
		});
	// handle rx_protocol stuff!
	result = rx_internal::rx_protocol::messages::rx_message_base::init_messages();
	// register protocol constructors
	result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
		RX_RX_JSON_TYPE_ID, [] {
			return rx_create_reference<rx_internal::rx_protocol::rx_json_protocol_port>();
		});
	result = rx_internal::model::register_internal_constructor<port_type, protocols::opcua::opcua_transport_port>(
		nullptr ,RX_OPCUA_TRANSPORT_PORT_TYPE_ID, [] {
			return rx_create_reference<protocols::opcua::opcua_transport_port>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
		RX_VT00_TYPE_ID, [] {
			return rx_create_reference<rx_internal::terminal::term_ports::vt100_port>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
		RX_TELNET_TYPE_ID, [] {
			return rx_create_reference<rx_internal::terminal::term_ports::telnet_transport_port>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
		RX_NS_HTTP_TYPE_ID, [] {
			return rx_create_reference<protocols::rx_http::rx_http_port>();
		});
	
	auto commands = get_internal_commands();
	for (auto& one : commands)
	{
		register_command(one);
	}

	for (auto& one : registered_commands_)
	{
		command_ptr temp = one.second;
		result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<method_type>().register_constructor(
			one.first.c_str(), [temp] {
				return temp;
			});
	}

	
}

server_command_base_ptr server_command_manager::get_command_by_name (const string_type& name)
{
	locks::auto_lock dummy(&lock_);
	auto it = registered_commands_.find(name);
	if (it != registered_commands_.end())
	{
		return it->second;
	}
	it = other_commands_.find(name);
	if (it != other_commands_.end())
	{
		return it->second;
	}
	return server_command_base_ptr{};
}

server_command_manager::smart_ptr server_command_manager::instance ()
{
	if (!g_inst)
	{
		g_inst = smart_ptr::create_from_pointer_without_bind(new server_command_manager());
	}
	return g_inst;
}

namespace_item_attributes server_command_manager::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

void server_command_manager::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_CommandsManager";
	has_own_code_info = true;
}

void server_command_manager::get_commands (std::vector<command_ptr>& sub_items) const
{
	locks::const_auto_lock dummy(&lock_);
	sub_items.reserve(registered_commands_.size());
	for (const auto& one : registered_commands_)
		sub_items.emplace_back(one.second);
}

void server_command_manager::register_suggestions (const string_type& line, suggestions_type& suggestions)
{
}

void server_command_manager::clear ()
{
	rx_internal::rx_protocol::messages::rx_message_base::deinit_messages();
	g_inst = smart_ptr::null_ptr;
}

std::vector<server_command_base_ptr> server_command_manager::get_internal_commands ()
{
	std::vector<server_command_base_ptr> ret_commands;
	
	// general commands
	ret_commands.push_back(rx_create_reference<echo_server_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::info_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::code_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::rx_name_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::cls_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::shutdown_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::log_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::sec_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::time_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::sleep_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::def_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::phyton_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::license_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::copyright_command>());
	ret_commands.push_back(rx_create_reference<console::console_commands::help_command>());
	// namespace commands
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::dir_command>());
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::ls_command>());
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::cd_command>());
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::mkdir_command>());
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::rmdir_command>());
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::move_command>());
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::clone_command>());
	ret_commands.push_back(rx_create_reference<rx_internal::internal_ns::namespace_commands::clone_system_command>());
	// test command
	ret_commands.push_back(rx_create_reference<testing::test_command>());
	// meta commands
	ret_commands.push_back(rx_create_reference<model::meta_commands::del_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::rm_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::create_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::update_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::prototype_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::dump_types_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::check_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::save_command>());
	ret_commands.push_back(rx_create_reference<model::meta_commands::query_command>());
	// runtime commands
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::read_command>());
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::struct_command>());
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::write_command>());
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::pull_command>());
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::turn_on_command>());
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::turn_off_command>());
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::browse_command>());
	ret_commands.push_back(rx_create_reference<sys_runtime::runtime_commands::execute_command>());
	// plug-ins commands
	ret_commands.push_back(rx_create_reference<rx_internal::plugins::plugin_command>());
	// interfaces commands
	ret_commands.push_back(rx_create_reference<rx_internal::interfaces::io_endpoints::net_command>());

	return ret_commands;
}


// Class rx_internal::terminal::commands::echo_server_command 

echo_server_command::echo_server_command()
  : server_command("echo")
{
}


echo_server_command::~echo_server_command()
{
}



bool echo_server_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	// just copy from one steam to another
	if (!in.eof())
	{
		std::copy(std::istreambuf_iterator<char>(in),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(out));
	}
	else
		out << "echo";
	return true;
}


// Class rx_internal::terminal::commands::server_command 

server_command::server_command (const string_type& name)
      : time_stamp_(rx_time::now()),
        console_name_(name),
        security_guard_(std::make_unique<security::security_guard>()),
        modified_time_(rx_time::now())
	, method_runtime(name, name.c_str())
{
}


server_command::~server_command()
{
}



string_type server_command::get_type_name () const
{
	static string_type type_name = RX_CPP_COMMAND_TYPE_NAME;
	return type_name;
}

values::rx_value server_command::get_value () const
{
	values::rx_value temp;
	temp.assign_static(get_console_name().c_str(), time_stamp_);
	return temp;
}

namespace_item_attributes server_command::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_execute_access | namespace_item_read_access | namespace_item_system);
}

bool server_command::console_execute (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	if (security_guard_->check_premissions(security::rx_security_execute_access, security::rx_security_ext_null))
	{
		return do_console_command(in, out, err, ctx);
	}
	else
	{
		err << ANSI_COLOR_RED RX_ACCESS_DENIED ANSI_COLOR_RESET;
		return false;
	}
}

bool server_command::dword_check_premissions (security::security_mask_t mask, security::extended_security_mask_t extended_mask)
{
	return security_guard_->check_premissions(mask, extended_mask);
}

rx_time server_command::get_created_time () const
{
	return rx_gate::instance().get_started();
}

bool server_command::generate_json (std::ostream& def, std::ostream& err) const
{
	return false;
}

string_type server_command::get_name () const
{
	return console_name_;
}

bool server_command::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool server_command::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

string_type server_command::get_help () const
{
	std::ostringstream ss;
	ss << RX_CONSOLE_HEADER_LINE "\r\n";
	ss << ANSI_COLOR_GREEN << ":>";
	ss << ANSI_COLOR_YELLOW ANSI_COLOR_BOLD << get_name() << ANSI_COLOR_RESET << "\r\n";
	ss << RX_CONSOLE_HEADER_LINE "\r\n\r\n";
	string_type str = rx_gate::instance().get_host()->get_manual("commands/"s + get_name());
	if (str.empty())
		ss << "jebi ga bato!!!";
	else
		ss << parse_man_file(str);
	ss << "\r\n\r\n";
	return ss.str();
}

void server_command::register_suggestions (const string_type& line, suggestions_type& suggestions)
{
}

rx_result server_command::execute (data::runtime_values_data args, logic::method_execution_context* context)
{
	string_type line = this->get_console_name() + " " +  args.get_value_static("In", ""s);
	
	rx_reference<console::console_runtime> console_program = rx_create_reference<console::console_runtime>(rx_thread_context(),
		[context](bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
		{
			if (done)
			{
				string_type out_str;
				string_type err_str;
				if (!out_buffer->empty())
					out_str.assign(out_buffer->pbase(), out_buffer->get_size());
				if (!err_buffer->empty())
					err_str.assign(err_buffer->pbase(), err_buffer->get_size());
				data::runtime_values_data out_result;
				out_result.add_value_static("Out", out_str);
				out_result.add_value_static("Err", err_str);
				out_result.add_value_static("Result", result);
				context->execution_complete(std::move(out_result));
			}
		});
	console_program->do_command(line, security::active_security());
	return true;
}


} // namespace commands
} // namespace terminal
} // namespace rx_internal

