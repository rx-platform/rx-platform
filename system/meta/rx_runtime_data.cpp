

/****************************************************************************
*
*  system\meta\rx_runtime_data.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_runtime_data
#include "system/meta/rx_runtime_data.h"

#include "system/serialization/rx_ser.h"
#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_runtime_holder.h"


namespace rx_platform {

namespace meta {

namespace runtime_data {
namespace
{
template<typename typeT>
rx_result serialize_runtime_data(const typeT& what,base_meta_writer& stream, uint8_t type)
{
    using target_type = typename typeT::targetType::RImplType;
    if (!what.meta_info.serialize_meta_data(stream, type, target_type::type_id))
        return false;

    if (!stream.start_object("def"))
        return false;

    if (!what.instance_data.serialize(stream, type))
        return false;

    if (!stream.write_init_values("overrides", what.overrides))
        return false;
    if (!stream.start_array("programs", 0))
        return false;
    /*for (const auto& one : programs_)
    {
        if (!one->save_program(stream, type))
            return false;
    }*/
    if (!stream.end_array())
        return false;

    if (!stream.end_object())
        return false;

    return true;
}
template<typename typeT>
rx_result deserialize_runtime_data(typeT& what, base_meta_reader& stream, uint8_t type, const meta_data& meta)
{
    if (!stream.start_object("def"))
        return false;

    if (!what.instance_data.deserialize(stream, type))
        return false;

    if (!stream.read_init_values("overrides", what.overrides))
        return false;

    if (!stream.start_array("programs"))
        return false;

    if (!stream.array_end())
        return RX_NOT_IMPLEMENTED;

    if (!stream.end_object())
        return false;

    what.meta_info = meta;

    return true;
}
template<typename typeT>
rx_result deserialize_runtime_data(typeT& what, base_meta_reader& stream, rx_item_type item_type, uint8_t type)
{
    rx_item_type readed_type;
    auto result = what.meta_info.deserialize_meta_data(stream, type, readed_type);
    if (!result)
        return result;
    if (readed_type != item_type)
        return "Wrong item type readed!";

    if (!stream.start_object("def"))
        return false;

    if (!what.instance_data.deserialize(stream, type))
        return false;

    if (!stream.read_init_values("overrides", what.overrides))
        return false;

    if (!stream.start_array("programs"))
        return false;

    if (!stream.array_end())
        return RX_NOT_IMPLEMENTED;

    if (!stream.end_object())
        return false;

    return true;
}
}

// Class rx_platform::meta::runtime_data::application_data 


bool application_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_int("processor", processor))
        return false;
    if (!stream.write_byte("priority", (uint8_t)priority))
        return false;
    if (identity.empty())
    {
        if (!stream.write_bytes("identity", nullptr, 0))
            return false;
    }
    else
    {
        if (!stream.write_bytes("identity", &identity[0], identity.size()))
            return false;
    }
    if (!stream.end_object())
        return false;
    return true;
}

bool application_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_int("processor", processor))
        return false;
    uint8_t temp;
    if (!stream.read_byte("priority", temp) || temp > (uint8_t)rx_domain_priority::priority_count)
        return false;
    priority = (rx_domain_priority)temp;
    if (!stream.read_bytes("identity", identity))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}


// Class rx_platform::meta::runtime_data::domain_data 


bool domain_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_int("processor", processor))
        return false;
    if (!stream.write_byte("priority", (uint8_t)priority))
        return false;
    if (!stream.write_id("app", app_id))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}

bool domain_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_int("processor", processor))
        return false;
    uint8_t temp;
    if (!stream.read_byte("priority", temp) || temp > (uint8_t)rx_domain_priority::priority_count)
        return false;
    priority = (rx_domain_priority)temp;
    if (!stream.read_id("app", app_id))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}


// Class rx_platform::meta::runtime_data::object_data 


bool object_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_id("domain", domain_id))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}

bool object_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_id("domain", domain_id))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}


// Class rx_platform::meta::runtime_data::port_data 


bool port_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_id("app", app_id))
        return false;
    if (identity.empty())
    {
        if (!stream.write_bytes("identity", nullptr, 0))
            return false;
    }
    else
    {
        if (!stream.write_bytes("identity", &identity[0], identity.size()))
            return false;
    }
    if (!stream.end_object())
        return false;
    return true;
}

bool port_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_id("app", app_id))
        return false;
    if (!stream.read_bytes("identity", identity))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}


// Class rx_platform::meta::runtime_data::basic_runtime_data 


// Class rx_platform::meta::runtime_data::object_runtime_data 


bool object_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    return serialize_runtime_data(*this, stream, type);
}

rx_result object_runtime_data::deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta)
{
    return deserialize_runtime_data(*this, stream, type, meta);
}

rx_result object_runtime_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    return deserialize_runtime_data(*this, stream, rx_item_type::rx_object, type);
}


// Class rx_platform::meta::runtime_data::domain_runtime_data 


bool domain_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    return serialize_runtime_data(*this, stream, type);
}

rx_result domain_runtime_data::deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta)
{
    return deserialize_runtime_data(*this, stream, type, meta);
}

rx_result domain_runtime_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    return deserialize_runtime_data(*this, stream, rx_item_type::rx_domain, type);
}


// Class rx_platform::meta::runtime_data::port_runtime_data 


bool port_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    return serialize_runtime_data(*this, stream, type);
}

rx_result port_runtime_data::deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta)
{
    return deserialize_runtime_data(*this, stream, type, meta);
}

rx_result port_runtime_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    return deserialize_runtime_data(*this, stream, rx_item_type::rx_port, type);
}


// Class rx_platform::meta::runtime_data::application_runtime_data 


bool application_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    return serialize_runtime_data(*this, stream, type);
}

rx_result application_runtime_data::deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta)
{
    return deserialize_runtime_data(*this, stream, type, meta);
}

rx_result application_runtime_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    return deserialize_runtime_data(*this, stream, rx_item_type::rx_application, type);
}


} // namespace runtime_data
} // namespace meta
} // namespace rx_platform

