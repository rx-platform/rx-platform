

/****************************************************************************
*
*  protocols\opcua\rx_opcua_addr_space.cpp
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

// rx_opcua_addr_space
#include "protocols/opcua/rx_opcua_addr_space.h"



namespace protocols {

namespace opcua {

namespace opcua_addr_space {

// Class protocols::opcua::opcua_addr_space::opcua_base_node_type 

opcua_base_node_type::opcua_base_node_type (node_class_type node_cls)
      : node_class(node_cls),
        write_mask(write_mask_type::none),
        user_write_mask(write_mask_type::none),
        access_restrictions(access_restrictions_type::none),
        timestamp_(rx_time::now())
{
}


opcua_base_node_type::~opcua_base_node_type()
{
}



void opcua_base_node_type::read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value)
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
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::node_class:
        value.value = variant_type((uint32_t)node_class);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::browse_name:
        value.value = variant_type(browse_name);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::display_name:
        value.value = variant_type(display_name);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::description:
        value.value = variant_type(description);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    default:
        internal_read_attribute(id, range, encoding, value);
    }
}

void opcua_base_node_type::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value)
{
    value.status_code = opcid_Bad_AttributeIdInvalid;
}


// Class protocols::opcua::opcua_addr_space::opcua_variable_base_node 

opcua_variable_base_node::opcua_variable_base_node()
      : value_rank(-1),
        access_level(opc_access_level::none),
        user_access_level(opc_access_level::none),
        sampling_interval(0),
        historizing(false),
        access_level_ex(opc_access_level_ex::none)
    , opcua_base_node_type(node_class_type::variable)
{
}



void opcua_variable_base_node::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value)
{
    switch (id)
    {
    case attribute_id::value:
        value = this->value;
        if (value.source_ts.t_value == 0)
            value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::data_type:
        value.value = variant_type(data_type);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::value_rank:
        value.value = variant_type(value_rank);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::array_dimensions:
        RX_ASSERT(false);
        value.status_code = opcid_Bad_AttributeIdInvalid;
        //value.value = variant_type(array_dimenstions);
        //value.source_ts = timestamp_;
        //return opcid_OK;
        break;
    case attribute_id::access_level:
        value.value = variant_type((uint8_t)access_level);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::user_access_level:
        value.value = variant_type((uint8_t)user_access_level);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::minimum_sampling:
        value.value = variant_type(sampling_interval);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::historizing:
        value.value = variant_type(historizing);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::access_level_ex:
        value.value = variant_type((uint32_t)access_level_ex);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    default:
        value.status_code = opcid_Bad_AttributeIdInvalid;
    }
}


// Class protocols::opcua::opcua_addr_space::opcua_reference_type 

opcua_reference_type::opcua_reference_type()
      : is_abstract(false),
        symmetric(false)
    , opcua_base_node_type(node_class_type::reference_type)
{
}



void opcua_reference_type::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value)
{
    switch (id)
    {
    case attribute_id::is_abstract:
        value.value = variant_type(is_abstract);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::symetric:
        value.value = variant_type(symmetric);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    case attribute_id::inverse_name:
        value.value = variant_type(inverse_name);
        value.source_ts = timestamp_;
        value.status_code = opcid_OK;
        break;
    default:
        value.status_code = opcid_Bad_AttributeIdInvalid;
    }
}


// Class protocols::opcua::opcua_addr_space::opcua_property_node 

opcua_property_node::opcua_property_node()
{
}



void opcua_property_node::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value)
{
}


// Class protocols::opcua::opcua_addr_space::opcua_variable_node 

opcua_variable_node::opcua_variable_node()
{
}



void opcua_variable_node::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value)
{
}


// Class protocols::opcua::opcua_addr_space::reference_data 

reference_data::reference_data()
    : forward(false)
{
}

reference_data::reference_data (rx_node_id ref_id, rx_node_id targ_id, bool fwd)
    : reference_id(std::move(ref_id))
    , target_id(std::move(targ_id))
    , forward(fwd)
{
}



// Class protocols::opcua::opcua_addr_space::opcua_node_base 


// Class protocols::opcua::opcua_addr_space::opcua_address_space_base 


} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols



