

/****************************************************************************
*
*  system\meta\rx_runtime_data.cpp
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
    auto ret = what.meta_info.serialize_meta_data(stream, type, target_type::type_id);
    if (!ret)
        return ret;

    if (!stream.start_object("def"))
        return stream.get_error();

    ret = what.instance_data.serialize(stream, type);
    if(!ret)
        return ret;

    if (!stream.write_init_values("overrides", what.overrides))
        return stream.get_error();
    if (!stream.start_array("programs", 0))
        return stream.get_error();
    /*for (const auto& one : programs_)
    {
        if (!one->save_program(stream, type))
		return stream.get_error();
    }*/
    if (!stream.end_array())
        return stream.get_error();

    if (!stream.end_object())
        return stream.get_error();

    return true;
}
template<typename typeT>
rx_result deserialize_runtime_data(typeT& what, base_meta_reader& stream, uint8_t type, const meta_data& meta)
{
    if (!stream.start_object("def"))
        return stream.get_error();

    auto ret = what.instance_data.deserialize(stream, type);
    if (!ret)
        return ret;

    if (!stream.read_init_values("overrides", what.overrides))
        return stream.get_error();

    if (!stream.start_array("programs"))
        return stream.get_error();

    if (!stream.array_end())
        return RX_NOT_IMPLEMENTED;

    if (!stream.end_object())
        return stream.get_error();

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
        return stream.get_error();

    auto ret = what.instance_data.deserialize(stream, type);
    if (!ret)
        return ret;

    if (!stream.read_init_values("overrides", what.overrides))
        return stream.get_error();

    if (!stream.start_array("programs"))
        return stream.get_error();

    if (!stream.array_end())
        return RX_NOT_IMPLEMENTED;

    if (!stream.end_object())
        return stream.get_error();

    return true;
}
}

// Class rx_platform::meta::runtime_data::basic_runtime_data 


// Class rx_platform::meta::runtime_data::object_runtime_data 


rx_result object_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
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


rx_result domain_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
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


rx_result port_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
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


rx_result application_runtime_data::serialize (base_meta_writer& stream, uint8_t type) const
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


// Class rx_platform::meta::runtime_data::application_data 


rx_result application_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (!stream.write_int("processor", processor))
        return stream.get_error();
    if (!stream.write_byte("priority", (uint8_t)priority))
        return stream.get_error();
    if (identity.empty())
    {
        if (!stream.write_bytes("identity", nullptr, 0))
            return stream.get_error();
    }
    else
    {
        if (!stream.write_bytes("identity", &identity[0], identity.size()))
            return stream.get_error();
    }
    if (!stream.end_object())
        return stream.get_error();
    return true;
}

rx_result application_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (!stream.read_int("processor", processor))
        return stream.get_error();
    uint8_t temp;
    if (!stream.read_byte("priority", temp) || temp > (uint8_t)rx_domain_priority::priority_count)
        return stream.get_error();
    priority = (rx_domain_priority)temp;
    if (!stream.read_bytes("identity", identity))
        return stream.get_error();
    if (!stream.end_object())
        return stream.get_error();
    return true;
}


// Class rx_platform::meta::runtime_data::port_data 


rx_result port_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (stream.get_version() >= RX_PARENT_REF_VERSION)
    {
        if (!stream.write_item_reference("app", app_ref))
            return stream.get_error();
    }
    else
    {// old version <= RX_FIRST_SERIZALIZE_VERSION
        if (app_ref.is_node_id())
        {
            if (!stream.write_id("app", app_ref.get_node_id()))
                return stream.get_error();
        }
        else
        {
            if (!stream.write_id("app", rx_node_id::null_id))
                return stream.get_error();
        }
    }
    if (identity.empty())
    {
        if (!stream.write_bytes("identity", nullptr, 0))
            return stream.get_error();
    }
    else
    {
        if (!stream.write_bytes("identity", &identity[0], identity.size()))
            return stream.get_error();
    }
    if (!stream.end_object())
        return stream.get_error();
    return true;
}

rx_result port_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (stream.get_version() >= RX_PARENT_REF_VERSION)
    {
        if (!stream.read_item_reference("app", app_ref))
            return stream.get_error();
    }
    else
    {// old version <= RX_FIRST_SERIZALIZE_VERSION
        rx_node_id id;
        if (!stream.read_id("app", id))
            return stream.get_error();
        app_ref = id;
    }
    if (!stream.read_bytes("identity", identity))
        return stream.get_error();
    if (!stream.end_object())
        return stream.get_error();
    return true;
}


// Class rx_platform::meta::runtime_data::domain_data 


rx_result domain_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (!stream.write_int("processor", processor))
        return stream.get_error();
    if (!stream.write_byte("priority", (uint8_t)priority))
        return stream.get_error();
    if (stream.get_version() >= RX_PARENT_REF_VERSION)
    {
        if (!stream.write_item_reference("app", app_ref))
            return stream.get_error();
    }
    else
    {// old version <= RX_FIRST_SERIZALIZE_VERSION
        if (app_ref.is_node_id())
        {
            if (!stream.write_id("app", app_ref.get_node_id()))
                return stream.get_error();
        }
        else
        {
            if (!stream.write_id("app", rx_node_id::null_id))
                return stream.get_error();
        }
    }
    if (!stream.end_object())
        return stream.get_error();
    return true;
}

rx_result domain_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (!stream.read_int("processor", processor))
        return stream.get_error();
    uint8_t temp;
    if (!stream.read_byte("priority", temp) || temp > (uint8_t)rx_domain_priority::priority_count)
        return stream.get_error();
    priority = (rx_domain_priority)temp;

    if (stream.get_version() >= RX_PARENT_REF_VERSION)
    {
        if (!stream.read_item_reference("app", app_ref))
            return stream.get_error();
    }
    else
    {// old version <= RX_FIRST_SERIZALIZE_VERSION
        rx_node_id id;
        if (!stream.read_id("app", id))
            return stream.get_error();
        app_ref = id;
    }
    if (!stream.end_object())
        return stream.get_error();
    return true;
}


// Class rx_platform::meta::runtime_data::object_data 


rx_result object_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (stream.get_version() >= RX_PARENT_REF_VERSION)
    {
        if (!stream.write_item_reference("domain", domain_ref))
            return stream.get_error();
    }
    else
    {// old version <= RX_FIRST_SERIZALIZE_VERSION
        if (domain_ref.is_node_id())
        {
            if (!stream.write_id("domain", domain_ref.get_node_id()))
                return stream.get_error();
        }
        else
        {
            if (!stream.write_id("domain", rx_node_id::null_id))
                return stream.get_error();
        }
    }
    if (!stream.end_object())
        return stream.get_error();
    return true;
}

rx_result object_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return stream.get_error();
    if (stream.get_version() >= RX_PARENT_REF_VERSION)
    {
        if (!stream.read_item_reference("domain", domain_ref))
            return stream.get_error();
    }
    else
    {// old version <= RX_FIRST_SERIZALIZE_VERSION
        rx_node_id id;
        if (!stream.read_id("domain", id))
            return stream.get_error();
        domain_ref = id;
    }
    if (!stream.end_object())
        return stream.get_error();
    return true;
}


} // namespace runtime_data
} // namespace meta
} // namespace rx_platform

