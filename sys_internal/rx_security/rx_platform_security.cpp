

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


namespace rx_internal {

namespace rx_security {

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
{
}


platform_security::~platform_security()
{
}



platform_security& platform_security::instance ()
{
    static platform_security g_instance;
    return g_instance;
}

rx_result platform_security::register_role (const string_type& role, const string_type& parent_role)
{
    return true;
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
    if (!stream.write_string("name", user_name_))
        return stream.get_error();
    if (!stream.write_string("description", description_))
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


} // namespace rx_security
} // namespace rx_internal

