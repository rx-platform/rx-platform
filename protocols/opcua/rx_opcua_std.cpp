

/****************************************************************************
*
*  protocols\opcua\rx_opcua_std.cpp
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

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

// rx_opcua_std
#include "protocols/opcua/rx_opcua_std.h"



namespace protocols {

namespace opcua {

namespace opcua_addr_space {
struct std_nodes_comparator
{
    bool operator()(const opcua_std_node& left, const opcua_std_node& right)
    {
        return left.node_id < right.node_id;
    }
};

// Class protocols::opcua::opcua_addr_space::opcua_std_address_space 

opcua_std_address_space::opcua_std_address_space()
      : config_ts_(rx_time::now())
{
}


opcua_std_address_space::~opcua_std_address_space()
{
}



rx_result opcua_std_address_space::register_node (opcua_node_base* what)
{
    RX_ASSERT(false);
    return RX_NOT_SUPPORTED;
}

rx_result opcua_std_address_space::unregister_node (opcua_node_base* what)
{
    RX_ASSERT(false);
    return RX_NOT_SUPPORTED;
}

void opcua_std_address_space::read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const
{
    for (const auto& one : to_read)
    {
        data_value temp_val;
        temp_val.status_code = opcid_Bad_NodeIdUnknown;
        opcua_std_node found;
        if (one.node_id.is_opc())
        {// WARNING manual lock used here to avoid locks while creating data for return
            const_cast<opcua_std_address_space*>(this)->ns_lock_.lock();
            auto it = get_by_id(one.node_id.get_numeric());
            if (it!=registered_nodes_.end())
            {
                found = *it;
                const_cast<opcua_std_address_space*>(this)->ns_lock_.unlock();
                found.read_attribute(one.attr_id, one.range, one.data_encoding.name, temp_val, config_ts_);
            }
            else
            {
                const_cast<opcua_std_address_space*>(this)->ns_lock_.unlock();
            }
        }
        values.push_back(std::move(temp_val));
    }
}

opcua_std_address_space::registered_nodes_type::const_iterator opcua_std_address_space::get_by_id (uint32_t id) const
{
    auto it = std::lower_bound(registered_nodes_.begin(), registered_nodes_.end(), opcua_std_node{ id }, std_nodes_comparator());
    if (it != registered_nodes_.end() && it->node_id == id)
        return it;
    else
        return registered_nodes_.end();
}

opcua_std_address_space::registered_nodes_type::iterator opcua_std_address_space::get_by_id (uint32_t id)
{
    auto it = std::lower_bound(registered_nodes_.begin(), registered_nodes_.end(), opcua_std_node{ id }, std_nodes_comparator());
    if (it != registered_nodes_.end() && it->node_id == id)
        return it;
    else
        return registered_nodes_.end();
}


// Class protocols::opcua::opcua_addr_space::opcua_std_node 

opcua_std_node::opcua_std_node()
      : node_class(node_class_type::node),
        name(nullptr),
        inverse_name(nullptr)
    , node_id(0)
{
}

opcua_std_node::opcua_std_node (uint32_t id)
      : node_class(node_class_type::node),
        name(nullptr),
        inverse_name(nullptr)
    , node_id(id)
{
}



void opcua_std_node::read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value, const rx_time& config_ts) const
{
    if (!encoding.empty())
    {
        value.status_code = opcid_Bad_DataEncodingUnsupported;
        return;
    }
    if (!range.empty())
    {
        value.status_code = opcid_Bad_IndexRangeInvalid;
        return;
    }
    switch (id)
    {
    case attribute_id::node_id:
        value.value = variant_type(node_id);
        value.source_ts = config_ts;
        value.status_code = opcid_OK;
        break;
    case attribute_id::node_class:
        value.value = variant_type((uint32_t)node_class);
        value.source_ts = config_ts;
        value.status_code = opcid_OK;
        break;
    case attribute_id::browse_name:
        value.value = variant_type(qualified_name{ 0, name });
        value.source_ts = config_ts;
        value.status_code = opcid_OK;
        break;
    case attribute_id::display_name:
        value.value = variant_type(localized_text{ name });
        value.source_ts = config_ts;
        value.status_code = opcid_OK;
        break;
    case attribute_id::description:
        value.value = variant_type("");
        value.source_ts = config_ts;
        value.status_code = opcid_OK;
        break;
    default:
        {
            switch (node_class)
            {
            case node_class_type::reference_type:
                {
                    switch (id)
                    {
                    case attribute_id::is_abstract:
                        value.value = variant_type(bit_data[abstract_idx]);
                        value.source_ts = config_ts;
                        value.status_code = opcid_OK;
                        break;
                    case attribute_id::symetric:
                        value.value = variant_type(bit_data[symmetric_idx]);
                        value.source_ts = config_ts;
                        value.status_code = opcid_OK;
                        break;
                    case attribute_id::inverse_name:
                        value.value = variant_type(inverse_name);
                        value.source_ts = config_ts;
                        value.status_code = opcid_OK;
                        break;
                    default:
                        value.status_code = opcid_Bad_AttributeIdInvalid;
                    }
                }
                break;
            default:
                value.status_code = opcid_Bad_AttributeIdInvalid;
            }
        }
    }
}

rx_node_id opcua_std_node::get_node_id () const
{
    return rx_node_id::opcua_standard_id(node_id);
}


} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols

