

/****************************************************************************
*
*  sys_internal\rx_security\rx_platform_security.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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

#include "system/serialization/rx_ser.h"


#define RX_DEFAULT_SECURITY_NAME RX_NONE_SECURITY_NAME

#define RX_NONE_SECURITY_INFO "{rx-platform} Empty Security Provider"



namespace rx_internal {

namespace rx_security {
namespace
{
platform_security* g_instance = nullptr;
string_type g_none_security_name = RX_NONE_SECURITY_NAME;
}

// Class rx_internal::rx_security::maintenance_context 

maintenance_context::maintenance_context (const string_type& port, const string_type& location)
{
    location_ = location;
    user_name_ = "management";
    full_name_ = "management@" + location_;
}


maintenance_context::~maintenance_context()
{
}



bool maintenance_context::is_system () const
{
  return true;

}

bool maintenance_context::has_console () const
{
  return true;

}

bool maintenance_context::is_interactive () const
{
  return true;

}


// Class rx_internal::rx_security::platform_security 

platform_security::platform_security()
      : default_provider_(nullptr)
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
    auto internal_providers = collect_internal_providers();
    for (auto& one : internal_providers)
        register_provider(std::move(one), host);
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
        }
        else
        {
            default_provider_ = providers_[RX_DEFAULT_SECURITY_NAME].get();
            default_is_none = true;
        }
    }
    if (default_is_none)
        SECURITY_LOG_WARNING("platform_security", 999, "Default security provider is set to none. This rx-platform instance is not secured!");
    return true;
}

void platform_security::deinitialize ()
{
    for (auto& one : providers_)
        one.second->deinitialize();
    security::security_manager::instance().deinitialize();
    delete this;
}

rx_result platform_security::register_role (const string_type& role, const string_type& parent_role, hosting::rx_platform_host* host)
{
    return true;
}

rx_result platform_security::register_provider (std::unique_ptr<platform_security_provider>  who, hosting::rx_platform_host* host)
{
    auto it = providers_.find(who->get_name());
    if (it == providers_.end())
    {
        auto result = who->initialize(host);
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
    return ret;
}


// Class rx_internal::rx_security::built_in_security_context 

built_in_security_context::built_in_security_context()
{
    location_ = rx_get_server_name();
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


// Class rx_internal::rx_security::host_security_context 

host_security_context::host_security_context (const string_type& name, const string_type& instance)
{
    user_name_ = name;
    location_ = instance;
    full_name_ = user_name_ + "@";
    full_name_ += location_;
}


host_security_context::~host_security_context()
{
}



bool host_security_context::is_system () const
{
  return true;

}


// Class rx_internal::rx_security::process_context 

process_context::process_context (const string_type& port, const string_type& location)
{
}


process_context::~process_context()
{
}



bool process_context::is_system () const
{
  return true;

}

bool process_context::has_console () const
{
  return true;

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

rx_result none_security_provider::initialize (hosting::rx_platform_host* host)
{
    return true;
}

void none_security_provider::deinitialize ()
{
}


} // namespace rx_security
} // namespace rx_internal

