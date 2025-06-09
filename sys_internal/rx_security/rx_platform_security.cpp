

/****************************************************************************
*
*  sys_internal\rx_security\rx_platform_security.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


// rx_platform_security
#include "sys_internal/rx_security/rx_platform_security.h"

#include "system/runtime/rx_internal_objects.h"
#include "rx_x509_security.h"
#include "rx_aspnet_security.h"
namespace rx_platform
{
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();
}
#include "lib/rx_ser_json.h"
#include "system/server/rx_server.h"

#define RX_DEFAULT_SECURITY_NAME RX_NONE_SECURITY_NAME

#define RX_NONE_SECURITY_INFO "{rx-platform} Empty Security Provider"



namespace rx_internal {

namespace rx_security {
namespace
{
platform_security* g_instance = nullptr;
string_type g_none_security_name = RX_NONE_SECURITY_NAME;
string_type g_cert_security_name = RX_CERT_SECURITY_NAME;
}

// Class rx_internal::rx_security::maintenance_context 

maintenance_context::maintenance_context (bool has_console)
    : built_in_security_context(has_console)
{
    location_ = rx_gate::instance().get_node_name();
}


maintenance_context::~maintenance_context()
{
}



// Class rx_internal::rx_security::platform_security 

platform_security::platform_security()
      : default_provider_(nullptr),
        unassigned_ctx_(rx_create_reference<security::unathorized_security_context>())
{
}


platform_security::~platform_security()
{
}



platform_security& platform_security::instance ()
{
    if (g_instance == nullptr)
        g_instance = new platform_security();
    return *g_instance;
}

rx_result platform_security::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
    security::security_manager::instance();

    auto roles_result = roles_manager_.initialize(host, data);
    if (!roles_result)
    {
        roles_result.register_error("Unable to initialize roles manager!");
        return roles_result;
    }

    auto internal_providers = collect_internal_providers();
    for (auto& one : internal_providers)
        register_provider(std::move(one), host, data);
    // handle the default provider
    bool default_is_none = false;
    if (data.other.rx_security.empty() || data.other.rx_security == RX_NONE_SECURITY_NAME)
    {
        default_provider_ = providers_[RX_NONE_SECURITY_NAME].get();
        default_is_none = true;
    }
    else
    {
        auto it = providers_.find(data.other.rx_security);
        if (it != providers_.end())
        {
            default_provider_ = it->second.get();
            SECURITY_LOG_INFO("platform_security", 999, "Default security provider is set to "s + default_provider_->get_info());
        }
        else
        {
            default_provider_ = providers_[RX_DEFAULT_SECURITY_NAME].get();
            default_is_none = true;
        }
    }
    if (default_is_none)
        SECURITY_LOG_WARNING("platform_security", 999, "Default security provider is set to none. This rx-platform instance is not secured!");


    auto ctx_result = default_provider_->create_host_context(host, data);
    if (ctx_result)
    {

        sys_objects::host_application::instance()->host_identity = ctx_result.value();
        host_ctx_ = ctx_result.move_value();


        SECURITY_LOG_INFO("platform_security", 999, "Logging to host account...");
        auto result = host_ctx_->login();
        if (result)
        {
            rx_push_security_context(host_ctx_->get_handle());
        }
        else
        {
            SECURITY_LOG_ERROR("platform_security", 999, "Error logging on as "s + host_ctx_->get_full_name() + ".");
            return result;
        }

        ctx_result = default_provider_->create_system_context(host, data);
        if (ctx_result)
        {
            sys_objects::system_application::instance()->system_identity = ctx_result.value();
            system_ctx_ = ctx_result.move_value();

            SECURITY_LOG_INFO("platform_security", 999, "Logging to system account...");
            auto result = system_ctx_->login();
            if (!result)
            {
                SECURITY_LOG_ERROR("platform_security", 999, "Error logging on as "s + system_ctx_->get_full_name() + ".");
                return result;
            }
        }
        else
        {
            SECURITY_LOG_WARNING("platform_security", 999, "System security identity not set >> defaulting to host identity!");
            sys_objects::system_application::instance()->system_identity = host_ctx_;
        }
        ctx_result = default_provider_->create_world_context(host, data);
        if (ctx_result)
        {
            sys_objects::world_application::instance()->world_identity = ctx_result.value();
            world_ctx_ = ctx_result.move_value();

            SECURITY_LOG_INFO("platform_security", 999, "Logging to world account...");
            auto result = world_ctx_->login();
            if (!result)
            {
                SECURITY_LOG_ERROR("platform_security", 999, "Error logging on as "s + world_ctx_->get_full_name() + ".");
                return result;
            }
        }
        else
        {
            SECURITY_LOG_WARNING("platform_security", 999, "World security identity not set >> defaulting to system identity!");
            sys_objects::world_application::instance()->world_identity = sys_objects::system_application::instance()->system_identity;
        }
    }
    else
    {
        SECURITY_LOG_ERROR("platform_security", 999, "Error creating host security context.");
        return ctx_result.errors();
    }


    return true;
}

rx_result platform_security::initialize_roles (std::vector<rx_roles_storage_item_ptr> storages)
{
    return roles_manager_.initialize_roles(std::move(storages));
}

rx_result platform_security::start (hosting::rx_platform_host* host)
{
    for(auto& one : providers_)
    {
        auto result = one.second->start(host);
        if (!result)
        {
            std::ostringstream ss;
            ss << "Error starting security provider: "
                << one.second->get_info()
                << " - "
                << result.errors_line();
            SECURITY_LOG_ERROR("platform_security", 999, ss.str());
            return result;
        }
        else
        {
            std::ostringstream ss;
            ss << "Started security provider: "
                << one.second->get_info();
            SECURITY_LOG_INFO("platform_security", 500, ss.str());
        }
	}
    return true;
}

void platform_security::stop ()
{
}

void platform_security::deinitialize ()
{
    if (host_ctx_)
    {
        rx_pop_security_context();
        host_ctx_->logout();
        host_ctx_ = security::security_context_ptr::null_ptr;
    }
    if (system_ctx_)
    {
        system_ctx_->logout();
        system_ctx_ = security::security_context_ptr::null_ptr;
    }
    if (world_ctx_)
    {
        world_ctx_->logout();
        world_ctx_ = security::security_context_ptr::null_ptr;
    }
    if (unassigned_ctx_)
    {
        unassigned_ctx_->logout();
        unassigned_ctx_ = security::security_context_ptr::null_ptr;
    }
    for (auto& one : providers_)
        one.second->deinitialize();
    security::security_manager::instance().deinitialize();
    delete this;
}

rx_result platform_security::register_role (const string_type& role, const string_type& parent_role, hosting::rx_platform_host* host)
{
    return true;
}

rx_result platform_security::register_provider (std::unique_ptr<platform_security_provider>  who, hosting::rx_platform_host* host, configuration_data_t& data)
{
    auto it = providers_.find(who->get_name());
    if (it == providers_.end())
    {
        auto result = who->initialize(host, data);
        if (!result)
        {
            std::ostringstream ss;
            ss << "Error initializing  "
                << who->get_info()
                << " security provider:"
                << result.errors_line();
            SECURITY_LOG_ERROR("platform_security", 999, ss.str());
            return result;
        }
        else
        {
            std::ostringstream ss;
            ss << "Initialized "
                << who->get_info()
                << " security provider";
            SECURITY_LOG_INFO("platform_security", 500, ss.str());
            providers_.emplace(string_type(who->get_name()), std::move(who));
            return true;
        }
    }
    else
    {
        std::ostringstream ss;
        ss << "Error initializing  "
            << who->get_info()
            << " security provider:"
            << "provider name "
            << who->get_name()
            << "already exists";
        SECURITY_LOG_ERROR("platform_security", 999, ss.str());
        return "Provider already exists";
    }
}

platform_security_provider* platform_security::get_provider (const string_type& name)
{
    if (name.empty())
    {
        return default_provider_;
    }
    else
    {
        auto it = providers_.find(name);
        if (it != providers_.end())
        {
            return it->second.get();
        }
        else
        {
            return default_provider_;
        }
    }
}

std::vector<std::unique_ptr<platform_security_provider> > platform_security::collect_internal_providers ()
{
    std::vector<std::unique_ptr<platform_security_provider> > ret;
    ret.push_back(std::make_unique<none_security_provider>());
    ret.push_back(std::make_unique<x509::certificate_security_provider>());
    ret.push_back(std::make_unique<aspnet_forms::aspnet_security_provider>());
    return ret;
}

bool platform_security::check_permissions (security_mask_t mask, const string_type& path, security::security_context_ptr ctx)
{
    return roles_manager_.check_permissions(mask, path, ctx);
}

bool platform_security::check_role (security_mask_t mask, const string_type& role, security::security_context_ptr ctx)
{
    if (ctx->is_in_role(role, mask))
    {
        return true;
    }
	else
	{
		SECURITY_LOG_WARNING("platform_security", 999, "Role "s + role + " not found in " + ctx->get_full_name());
		return false;
	}
}

security::security_context_ptr platform_security::get_world_context ()
{
    return world_ctx_;
}


// Class rx_internal::rx_security::built_in_security_context 

built_in_security_context::built_in_security_context (bool has_console)
      : has_console_(has_console)
{
}


built_in_security_context::~built_in_security_context()
{
}



rx_result built_in_security_context::serialize (base_meta_writer& stream) const
{
    if (!stream.write_string("name", user_name_.c_str()))
        return stream.get_error();
    if (!stream.write_string("description", description_.c_str()))
        return stream.get_error();
    return true;
}

rx_result built_in_security_context::deserialize (base_meta_reader& stream)
{
    if (!stream.read_string("name", user_name_))
        return stream.get_error();
    if (!stream.read_string("description", description_))
        return stream.get_error();
    full_name_ = user_name_ + '@' + location_;
    return true;
}


bool built_in_security_context::has_console () const
{
  return has_console_;
}


// Class rx_internal::rx_security::host_security_context 

host_security_context::host_security_context (bool has_console)
    : built_in_security_context(has_console)
{
    user_name_ = "host";
    location_ = rx_gate::instance().get_node_name();
    full_name_ = user_name_ + "@" + location_;
}


host_security_context::~host_security_context()
{
}



bool host_security_context::is_system () const
{
  return true;

}

bool host_security_context::is_hosted () const
{
  return true;

}


// Class rx_internal::rx_security::process_context 

process_context::process_context (const string_type& name, bool sys, bool has_console)
      : system_(sys)
    , built_in_security_context(has_console)
{
    user_name_ = name;
    location_ = rx_gate::instance().get_node_name();
    if(!user_name_.empty())
        full_name_ = user_name_ + "@" + location_;
}


process_context::~process_context()
{
}



bool process_context::is_system () const
{
  return system_;

}

bool process_context::is_interactive () const
{
  return true;

}


// Class rx_internal::rx_security::platform_security_provider 


// Class rx_internal::rx_security::none_security_provider 


const string_type& none_security_provider::get_name ()
{
    return g_none_security_name;
}

string_type none_security_provider::get_info ()
{
    return RX_NONE_SECURITY_INFO;
}

rx_result none_security_provider::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
    return true;
}

void none_security_provider::deinitialize ()
{
}

rx_result_with<security::security_context_ptr> none_security_provider::create_host_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    security::security_context_ptr ret = rx_create_reference<host_security_context>(false);
    return ret;
}

rx_result_with<security::security_context_ptr> none_security_provider::create_system_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    security::security_context_ptr ret = rx_create_reference<process_context>("system", true);
    return ret;
}

rx_result_with<security::security_context_ptr> none_security_provider::create_world_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    security::security_context_ptr ret = rx_create_reference<process_context>(data.instance.name);
    return ret;
}

rx_result none_security_provider::start (hosting::rx_platform_host* host)
{
    return true;
}

void none_security_provider::stop ()
{
}


} // namespace rx_security
} // namespace rx_internal

