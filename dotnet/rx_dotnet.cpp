

/****************************************************************************
*
*  dotnet\rx_dotnet.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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

#include "rx_dotnet_version.h"

// rx_dotnet_host
#include "dotnet/rx_dotnet_host.h"
// rx_dotnet
#include "dotnet/rx_dotnet.h"

#include "dotnet_api.h"
#include <terminal/parser3000.h>
#include "terminal/rx_con_programs.h"
#include "sys_internal/rx_async_functions.h"
#include "system/server/rx_file_helpers.h"
#include "model/rx_meta_internals.h"
#include "system/server/rx_directory_cache.h"


namespace rx_internal {

namespace builders {

template<class T>
rx_reference<T> create_type(meta::object_type_creation_data data);
template<class T>
void add_type_to_configuration(rx_directory_ptr dir, rx_reference<T> what, bool abstract_type);
template<class T>
rx_result add_object_to_configuration(rx_directory_ptr dir, typename T::instance_data_t&& data, data::runtime_values_data&& runtime_data, tl::type2type<T>, startup_create_data_type& startup_data
    , std::function<void(typename T::RTypePtr)> result_f = std::function<void(typename T::RTypePtr)>());

} // namespace builders



namespace dotnet {

// Class rx_internal::dotnet::dotnet_builder 


rx_result dotnet_builder::do_build (configuration_data_t& config, startup_create_data_type& data)
{
    string_type full_path(RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_DOTNET_NAME);
    auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
    if (dir_result)
    {
        auto dir = dir_result;


        auto app = builders::create_type<application_type>(meta::object_type_creation_data{
            RX_DOTNET_APPLICATION_TYPE_NAME
            , RX_DOTNET_APPLICATION_TYPE_ID
            , RX_INTERNAL_APP_TYPE_ID
            , namespace_item_attributes::namespace_item_internal_access
            , full_path
            });
        builders::add_type_to_configuration(dir, app, false);

        auto dom = builders::create_type<domain_type>(meta::object_type_creation_data{
            RX_DOTNET_DOMAIN_TYPE_NAME
            , RX_DOTNET_DOMAIN_TYPE_ID
            , RX_INTERNAL_DOMAIN_TYPE_ID
            , namespace_item_attributes::namespace_item_internal_access
            , full_path
            });
        builders::add_type_to_configuration(dir, dom, false);

        runtime_data::application_runtime_data app_instance_data;
        app_instance_data.meta_info.name = RX_DOTNET_APPLICATION_NAME;
        app_instance_data.meta_info.id = RX_DOTNET_APPLICATION_ID;
        app_instance_data.meta_info.parent = RX_DOTNET_APPLICATION_TYPE_ID;
        app_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
        app_instance_data.meta_info.path = full_path;
        app_instance_data.instance_data.processor = 0;
        app_instance_data.instance_data.priority = rx_domain_priority::normal;
        auto result = builders::add_object_to_configuration(dir, std::move(app_instance_data), data::runtime_values_data(), tl::type2type<application_type>(), data);

        runtime_data::domain_runtime_data domain_instance_data;
        domain_instance_data.meta_info.name = RX_DOTNET_DOMAIN_NAME;
        domain_instance_data.meta_info.id = RX_DOTNET_DOMAIN_ID;
        domain_instance_data.meta_info.parent = RX_DOTNET_DOMAIN_TYPE_ID;
        domain_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
        domain_instance_data.meta_info.path = full_path;
        domain_instance_data.instance_data.processor = -1;
        domain_instance_data.instance_data.app_ref = rx_node_id(RX_DOTNET_APPLICATION_ID);
        domain_instance_data.instance_data.priority = rx_domain_priority::normal;
        result = builders::add_object_to_configuration(dir, std::move(domain_instance_data), data::runtime_values_data(), tl::type2type<domain_type>(), data);

    }
	return dotnet_hosting::instance().do_build(config, data);
}


// Class rx_internal::dotnet::dotnet_command 

dotnet_command::dotnet_command()
	: server_command("dotnet")
{
}


dotnet_command::~dotnet_command()
{
}



bool dotnet_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx)
{
    using parser_t = urke::parser::parser3000;
	parser_t parser;
	bool show_version = false;
    bool show_help = false;
    bool list = false;
    string_type reload_command;
    parser.add_string_option('r', "reload", &reload_command, "Reloads a .NET library.");
    parser.add_bit_option('l', "list", &list, "Lists loaded .NET libraries.");
	parser.add_bit_option('\0', "version", &show_version, "Displays .NET hosting versions.");
    parser.add_bit_option('h', "help", &show_help, "Displays this help.");

    if (!parser.parse(in, err))
        return false;
    
    if(reload_command.size() > 0)
    {
		return do_reload_command(reload_command, in, out, err, ctx);
	}
    else if (list)
    {
		std::vector<std::pair<string_type, string_type> > libs = dotnet_hosting::instance().get_loaded_libraries();
        for (const auto& lib : libs)
        {
            out << ANSI_COLOR_GREEN ANSI_COLOR_BOLD
				<< lib.first
				<< " - " ANSI_COLOR_RESET
                << lib.second
                << "\r\n";
			out << "Assemblies:\r\n";
            auto assemblies = dotnet_hosting::instance().get_loaded_assemblies(lib.first);
            for (const auto& assembly : assemblies)
            {
                out << "   " ANSI_COLOR_CYAN ANSI_COLOR_BOLD
                    << assembly
                    << ANSI_COLOR_RESET << "\r\n";
            }
			out << "\r\n";
        }
        return true;
    }
    else if (show_version)
    {
        auto info = dotnet_hosting::instance().get_dotnet_info();
        out << "Native hosting module: " ANSI_COLOR_GREEN ANSI_COLOR_BOLD << info.rx_native_version << ANSI_COLOR_RESET << "\r\n";
        out << "Worker process: " ANSI_COLOR_GREEN ANSI_COLOR_BOLD << info.rx_worker_version << ANSI_COLOR_RESET << "\r\n";
        out << ".NET hosting module: " ANSI_COLOR_GREEN ANSI_COLOR_BOLD << info.rx_dotnet_version << ANSI_COLOR_RESET << "\r\n\r\n";
        out << "Hosted .NET runtime: " ANSI_COLOR_BLUE ANSI_COLOR_BOLD << info.runtime_version << ANSI_COLOR_RESET << "\r\n";
        return true;
    }
	else if (show_help) // nothing else to show
    {
        parser.print_help("dotnet", out);
        return true;
    }
    else
    {
        err << "Unknown command options. Use '--help' to see available options." << "\r\n";
        return false;
    }
}

bool dotnet_command::do_reload_command (const string_type& what, std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx)
{
	ctx->set_waiting();

    auto api_ctx = ctx->create_api_context();
    rx_post_function_to(RX_DOMAIN_META, api_ctx.object,
        [ctx](string_type what) mutable -> void
        {
            auto& out = ctx->get_stdout();
            auto& err = ctx->get_stderr();

            out << "Reloading .NET library " << ANSI_COLOR_GREEN ANSI_COLOR_BOLD 
                << what << ANSI_COLOR_RESET << std::endl;

            auto api_ctx = ctx->create_api_context();
			rx_result_callback callback = rx_result_callback(api_ctx.object, [ctx, what](rx_result result) mutable
			{
				auto& out = ctx->get_stdout();
				auto& err = ctx->get_stderr();

				if (!result)
				{
                    ctx->raise_error(result);
				}
                else
                {
                    out << "Successfully reloaded .NET library: " << ANSI_COLOR_GREEN << what << ANSI_COLOR_RESET << std::endl;
                }

                ctx->continue_scan();
			});

            dotnet_hosting::instance().reload_assembly(what, std::move(callback));
        }, string_type(what)
        );

    return true;
}


// Class rx_internal::dotnet::dotnet_hosting 

dotnet_hosting::dotnet_hosting()
{
}


dotnet_hosting::~dotnet_hosting()
{
}



dotnet_hosting& dotnet_hosting::instance ()
{
	static dotnet_hosting instance_;
	return instance_;
}

rx_result dotnet_hosting::initialize (hosting::rx_platform_host* host, const configuration_data_t& data)
{
    string_type host_path = data.dotnet.host_path;
    if (data.dotnet.host_path.empty())
    {
        host_path = host->get_dotnet_host_path();
    }
    if(data.dotnet.version.empty() || host_path.empty() || data.dotnet.app_path.empty())
    {
		return true;// dotnet not configured
	}
    auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<method_type>().register_constructor(
        RX_DOTNET_METHOD_TYPE_ID, [] {
            return rx_create_reference<dotnet_method>();
        });
    if (!result)
        return result;

    result = rx_internal::model::platform_types_manager::instance().get_relations_repository().register_constructor(
        RX_DOTNET_RELATION_TYPE_ID, [] {
            return rx_create_reference<dotnet_relation>();
        });
    if (!result)
        return result;


    if (data.dotnet.lib_path.empty())
        DOTNET_LOG_WARNING("dotnet_hosting::initialize", 100, ".NET User Library Path not set.");
    DOTNET_LOG_INFO("dotnet_hosting::initialize", 500, "Initializing .NET hosting.");
    
    rx_dotnet_config_data_t config;
	config.requested_version = data.dotnet.version;
	config.server_gc = data.dotnet.server_gc;
	config.concurrent_gc = data.dotnet.concurrent_gc;
	config.max_threads = data.dotnet.max_threads;
    config.min_threads = data.dotnet.min_threads;
    config.host_path = host_path;
    config.app_path = data.dotnet.app_path;
    config.lib_path = data.dotnet.lib_path;
	config.launch_debugger = data.dotnet.launch_debugger;
	config.scan_rate = data.dotnet.scan_rate;
	config.write_timeout = data.dotnet.write_timeout;
	config.exec_timeout = data.dotnet.exec_timeout;

	host_ = std::make_unique<dotnet_host>();
	result = host_->initialize(config);

    if(!result)
    {
        DOTNET_LOG_ERROR("dotnet_hosting::initialize", 500, "Error initializing .NET hosting. " + result.errors_line());
        host_->stop();
        host_.reset();
        return result;
	}


    DOTNET_LOG_INFO("dotnet_hosting::initialize", 500, ".NET hosting initialized; "s + host_->get_native_host_version());

	return true;
}

rx_result dotnet_hosting::start (hosting::rx_platform_host* host, const configuration_data_t& data, initialize_data_type& init_data)
{
    if (host_)
    {
        DOTNET_LOG_INFO("dotnet_hosting::start", 500, "Starting .NET hosting.");
        auto result = host_->start();
        if (!result)
        {
            DOTNET_LOG_ERROR("dotnet_hosting::start", 500, "Error starting .NET hosting. " + result.errors_line());
            host_.reset();
            return result;
        }
    }
    return true;
}

void dotnet_hosting::deinitialize (rx_void_callback callback)
{
    if (!host_ || host_->get_hosted_dotnet_version().empty())
    {
        callback();
        return;// dotnet not configured
    }

    DOTNET_LOG_INFO("dotnet_hosting::initialize", 500, "Deinitializing .NET hosting.");

    if(host_)
    {
        host_->deinitialize(std::move(callback));
	}

    DOTNET_LOG_INFO("dotnet_hosting::initialize", 500, ".NET hosting deinitialized");
}

dotnet_runtime_info_t dotnet_hosting::get_dotnet_info () const
{
    
    dotnet_runtime_info_t ret;
    ret.runtime_version = host_ ? host_->get_hosted_dotnet_version() : string_type();
    ret.rx_native_version = dotnet_host::get_native_host_version(); 
    ret.rx_dotnet_version = host_ ? host_->get_dotnet_host_version() : string_type();
	ret.rx_worker_version = host_ ? host_->get_worker_version() : string_type();
    return ret;
}

string_array dotnet_hosting::get_loaded_assemblies (const string_type& lib)
{
    return get_loaded_dotnet_assemblies(lib);
}

rx_result dotnet_hosting::do_build (configuration_data_t& config, startup_create_data_type& data)
{
	if (!host_)
        return true;// dotnet not configured
	return host_->do_build(config, data);
}

std::vector<std::pair<string_type, string_type> > dotnet_hosting::get_loaded_libraries ()
{
	return get_loaded_dotnet_libraries();
}

void dotnet_hosting::reload_assembly (const string_type& what, rx_result_callback callback)
{
    if(!host_)
		callback(rx_result("Dotnet hosting is not initialized."));
    else
    {
		auto anchor = callback.get_anchor();
		rx_result_callback local_callback(anchor, [what = string_type(what), this, callback = std::move(callback)](rx_result result) mutable -> void
        {
            if (result)
            {
                DOTNET_LOG_INFO("dotnet_hosting::reload_assembly", 500, "Assembly unloaded successfully.");
                if(!host_)
                {
                    DOTNET_LOG_ERROR("dotnet_hosting::reload_assembly", 500, "Dotnet hosting is not initialized.");
					result = rx_result("Error reloading assembly. Dotnet hosting is not initialized.");
                }
                else
                {
					result = host_->do_reload(what);
                    if(!result)
                    {
                        DOTNET_LOG_ERROR("dotnet_hosting::reload_assembly", 500, "Error reloading assembly. " + result.errors_line());
					}
                }
            }
            else
            {
                DOTNET_LOG_ERROR("dotnet_hosting::reload_assembly", 500, "Error reloading assembly. " + result.errors_line());
            }
            callback(std::move(result));
        });
        host_->do_unload(what, std::move(local_callback));
    }
}

void dotnet_hosting::received_result_callback (uint32_t transId, rx_result result)
{
    if (host_)
    {
		host_->received_result_callback(transId, std::move(result));
    }
}

void dotnet_hosting::received_void_callback (uint32_t transId)
{
    if (host_)
    {
        host_->received_void_callback(transId);
    }
}

void dotnet_hosting::received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback)
{
    if(host_)
    {
        host_->received_write_value(type, transId, inst, idx, std::move(val), std::move(callback));
    }
    else
    {
        callback(rx_result("Dotnet hosting is not initialized."));
    }
}

void dotnet_hosting::received_execute_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_simple_value&&, rx_result&&)> callback)
{
    if(host_)
    {
        host_->received_execute_value(type, transId, inst, idx, std::move(val), std::move(callback));
    }
    else
    {
        callback(rx_simple_value(), rx_result("Dotnet hosting is not initialized."));
        }
}

void dotnet_hosting::bind_runtime (const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_object_runtime> ref, const string_type& init)
{
    if (host_)
    {
        host_->bind_runtime(id, path, type_id, ptr, ref, init);
    }
}

void dotnet_hosting::unbind_runtime (const rx_node_id& id, const rx_node_id& type_id, void* ptr)
{
    if (host_)
    {
        host_->unbind_runtime(id, type_id, ptr);
    }
}

void dotnet_hosting::bind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_struct> ref, const string_type& init)
{
    if (host_)
    {
        host_->bind_struct(id, path, type_id, ptr, ref, init);
    }
}

void dotnet_hosting::unbind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    if (host_)
    {
        host_->unbind_struct(id, path, type_id, ptr);
    }
}

void dotnet_hosting::bind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_source> ref, const string_type& init)
{
    if (host_)
    {
        host_->bind_source(id, path, type_id, ptr, ref, init);
    }
}

void dotnet_hosting::unbind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    if (host_)
    {
        host_->unbind_source(id, path, type_id, ptr);
    }
}

void dotnet_hosting::bind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_mapper> ref, const string_type& init)
{
    if (host_)
    {
        host_->bind_mapper(id, path, type_id, ptr, ref, init);
    }
}

void dotnet_hosting::unbind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    if (host_)
    {
        host_->unbind_mapper(id, path, type_id, ptr);
    }
}

void dotnet_hosting::bind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_event> ref, const string_type& init)
{
    if (host_)
    {
        host_->bind_event(id, path, type_id, ptr, ref, init);
    }
}

void dotnet_hosting::unbind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    if (host_)
    {
        host_->unbind_event(id, path, type_id, ptr);
    }
}

void dotnet_hosting::runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose)
{
    if (host_)
    {
        host_->runtime_value_changed(idx, std::move(val), type, whose);
    }
}

void dotnet_hosting::send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose)
{
    if (host_)
    {
        host_->send_initial_values(std::move(names), std::move(val), type, whose);
    }
}

rx_result dotnet_hosting::execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value)
{
    if (!host_)
        return rx_result("Dotnet hosting is not initialized.");
	return host_->execute_method(trans_id, whose, method, value);
}

rx_result dotnet_hosting::source_write (uint32_t trans_id, void* whose, typed_value_type val)
{
    if (!host_)
        return rx_result("Dotnet hosting is not initialized.");
    return host_->source_write(trans_id, whose, std::move(val));
}

void dotnet_hosting::received_execute_done (uint32_t transId, void* inst, const string_type& val, rx_result result)
{
    if(host_)
    {
        host_->received_execute_done(transId, inst, val, std::move(result));
	}
}

void dotnet_hosting::received_source_change (void* inst, rx_value val)
{
    if (host_)
    {
        host_->received_source_change(inst, std::move(val));
    }
}

void dotnet_hosting::received_source_write_done (uint32_t trans_id, void* inst, rx_result result)
{
    if (host_)
    {
        host_->received_source_write_done(trans_id, inst, std::move(result));
    }
}

void dotnet_hosting::received_fire_event (void* inst, string_type val, string_type queue, bool state, bool remove)
{
    if (host_)
    {
        host_->received_fire_event(inst, std::move(val), std::move(queue), state, remove);
    }
}


} // namespace dotnet
} // namespace rx_internal

