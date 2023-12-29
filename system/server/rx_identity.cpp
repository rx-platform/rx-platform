

/****************************************************************************
*
*  system\server\rx_identity.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_identity
#include "system/server/rx_identity.h"


#include "lib/rx_ser_bin.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "sys_internal/rx_security/rx_x509_security.h"


/*
security account type:

+--+--+--+--+--+--+--+--+
| type|    sub-type     |
+--+--+--+--+--+--+--+--+
 7  6  5  4  3  2  1  0

*/
#define BUILTIN_ACCOUNT 0x40
#define USER_ACCOUNT 0x80

#define ACCOUNT_TYPE_MASK 0xc0
#define ACCOUNT_SUBTYPE_MASK 0x3f
//built in account types
#define PROCESS_PORT 1
#define MAINTENANCE_PORT 2

// certificate account types
#define X509_ACCOUNT 1


namespace rx_platform {

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::security_context_holder 


rx_result security_context_holder::create_context (const string_type& port, const string_type& location, const byte_string& data, security::security_context_ptr& ctx)
{
    if (data.empty())
        return true;
    memory::std_buffer buffer;
    buffer.push_data(&data[0], data.size());
    serialization::std_buffer_reader reader(buffer);

    if (!reader.read_byte("type", type_))
        return reader.get_error();
    if (type_)
    {
        switch (type_ & ACCOUNT_TYPE_MASK)
        {
        case BUILTIN_ACCOUNT:
            {
                rx_internal::rx_security::built_in_security_context::smart_ptr ptr;
                switch (type_ & ACCOUNT_SUBTYPE_MASK)
                {
                case MAINTENANCE_PORT:
                    {
                        ptr = rx_create_reference<rx_internal::rx_security::maintenance_context>();
                    }
                    break;
                case PROCESS_PORT:
                    {
                        ptr = rx_create_reference<rx_internal::rx_security::process_context>();
                    }
                    break;
                default:
                    return "Wrong built-in identity type!";
                }
                auto result = ptr->deserialize(reader);
                if (!result)
                    return result.errors();
                ctx = ptr;
                return true;
            }
            break;
        case USER_ACCOUNT:
            {
                switch (type_ & ACCOUNT_SUBTYPE_MASK)
                {
                case X509_ACCOUNT:
                    {
                        auto ptr = rx_create_reference<rx_internal::rx_security::x509_security_context>();
                        auto result = ptr->deserialize(reader);
                        if (!result)
                            return result.errors();
                        ctx = ptr;
                        return true;
                    }
                    break;
                default:
                    return "Wrong user identity type!";
                }
            }
            break;
        default:
            return "Wrong identity type!";
        }
    }
    return true;
}


} // namespace items
} // namespace runtime
} // namespace rx_platform

