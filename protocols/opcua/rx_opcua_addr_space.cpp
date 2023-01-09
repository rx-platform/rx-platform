

/****************************************************************************
*
*  protocols\opcua\rx_opcua_addr_space.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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
// rx_opcua_subscriptions
#include "protocols/opcua/rx_opcua_subscriptions.h"

#include "rx_opcua_server.h"


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



void opcua_base_node_type::read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
{
    if (!encoding.empty())
    {
        value_storage.status_code = opcid_Bad_DataEncodingUnsupported;
        return;
    }
    if (!range.empty())
    {
        value_storage.status_code = opcid_Bad_IndexRangeInvalid;
        return;
    }
    switch (id)
    {
    case attribute_id::node_id:
        value_storage.value = variant_type(node_id);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::node_class:
        value_storage.value = variant_type((int32_t)node_class);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::browse_name:
        value_storage.value = variant_type(browse_name);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::display_name:
        value_storage.value = variant_type(localized_text{ display_name });
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::description:
        value_storage.value = variant_type(description);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    default:
        internal_read_attribute(id, range, encoding, value_storage, config_ts);
    }
}

std::pair<opcua_result_t, runtime_transaction_id_t> opcua_base_node_type::write_attribute (attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep)
{
    return { opcid_Bad_NotWritable, 0 };
}

void opcua_base_node_type::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
{
    value_storage.status_code = opcid_Bad_AttributeIdInvalid;
}

void opcua_base_node_type::browse (const opcua_browse_description& to_browse, browse_result_internal& result, opcua_browse_context* ctx) const
{
    std::set<rx_node_id> refIds;
    if (!to_browse.reference_type_id.is_null())
    {
        refIds.insert(to_browse.reference_type_id);
        ctx->root->fill_relation_types(to_browse.reference_type_id, to_browse.sub_types, refIds);
    }
    result.status_code = opcid_OK;
    if (to_browse.direction == browse_direction_type::browse_direction_both
        || to_browse.direction == browse_direction_type::browse_direction_forward)
    {
        for (const auto& one : references.references)
        {
            if (one.resolved_node
                && (to_browse.node_class_mask == 0
                    || ((uint32_t)one.resolved_node->get_node_class() & to_browse.node_class_mask) != 0))
            {
                if (!refIds.empty() && refIds.count(one.reference_id))
                    result.add_reference_data(true, one, to_browse);
            }
        }
    }
    if (to_browse.direction == browse_direction_type::browse_direction_both
        || to_browse.direction == browse_direction_type::browse_direction_backward)
    {
        for (const auto& one : references.inverse_references)
        {
            if (one.resolved_node
                && (to_browse.node_class_mask == 0
                    || ((uint32_t)one.resolved_node->get_node_class() & to_browse.node_class_mask) != 0))
            {
                if (!refIds.empty() && refIds.count(one.reference_id))
                    result.add_reference_data(false, one, to_browse);
            }
        }
    }
}

void opcua_base_node_type::translate (const relative_path& path, browse_path_result& results, opcua_browse_context* ctx) const
{
    std::set<rx_node_id> refIds;
    if (!results.targets.empty() && results.targets.rbegin()->remaining_index >= path.elements.size())
        return;
    size_t idx = 0;
    if (!results.targets.empty())
        idx = results.targets.rbegin()->remaining_index;
    results.targets.push_back(browse_path_target{});
    auto target_it = results.targets.rbegin();
    target_it->remaining_index = (uint32_t)idx;

    const auto& one_path = path.elements[idx];

    if (one_path.target_name.name.empty())
    {
        results.status_code = opcid_Bad_BrowseNameInvalid;
        return;
    }

    if (!one_path.reference_id.is_null())
    {
        refIds.insert(one_path.reference_id);
        ctx->root->fill_relation_types(one_path.reference_id, one_path.sub_types, refIds);
    }
    if (!one_path.is_inverse)
    {
        for (const auto& one : references.references)
        {
            if (one.resolved_node)
            {
                if (!refIds.empty() && refIds.count(one.reference_id)
                    && one.resolved_node->get_browse_name() == one_path.target_name)
                {
                    target_it->target_id = one.target_id;
                    one.resolved_node->translate(path, results, ctx);
                    return;
                }
            }
        }
    }
    else // if (one_path.is_inverse)
    {
        for (const auto& one : references.inverse_references)
        {
            if (one.resolved_node)
            {
                if (!refIds.empty() && refIds.count(one.reference_id)
                    && one.resolved_node->get_browse_name() == one_path.target_name)
                {
                    target_it->target_id = one.target_id;
                    one.resolved_node->translate(path, results, ctx);
                    return;
                }
            }
        }
    }
    results.status_code = opcid_Bad_NoMatch;
}

node_class_type opcua_base_node_type::get_node_class () const
{
    return node_class;
}

rx_node_id opcua_base_node_type::get_node_id () const
{
    return node_id;
}

qualified_name opcua_base_node_type::get_browse_name () const
{
    return browse_name;
}

string_type opcua_base_node_type::get_display_name () const
{
    return display_name;
}

node_references& opcua_base_node_type::get_reference_data ()
{
    return references;
}

rx_result opcua_base_node_type::set_node_value (values::rx_value&& val)
{
    return RX_NOT_SUPPORTED;
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



void opcua_variable_base_node::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
{
    switch (id)
    {
    case attribute_id::value:
        value_storage = this->value;
        if (value_storage.server_ts.t_value == 0 && value_storage.source_ts.t_value == 0)
            value_storage.set_timestamp(timestamp_);
        break;
    case attribute_id::data_type:
        value_storage.value = variant_type(data_type);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::value_rank:
        value_storage.value = variant_type(value_rank);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::array_dimensions:
        //RX_ASSERT(false);
        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
        //value.value = variant_type(array_dimenstions);
        //value.source_ts = timestamp_;
        //return opcid_OK;
        break;
    case attribute_id::access_level:
        value_storage.value = variant_type((uint8_t)access_level);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::user_access_level:
        value_storage.value = variant_type((uint8_t)user_access_level);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::minimum_sampling:
        value_storage.value = variant_type(sampling_interval);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::historizing:
        value_storage.value = variant_type(historizing);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::access_level_ex:
        value_storage.value = variant_type((uint32_t)access_level_ex);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    default:
        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
    }
}

rx_result opcua_variable_base_node::set_node_value (values::rx_value&& val)
{
    rx_result ret;
    if (value.from_rx_value(std::move(val)))
        return true;
    else
        return RX_INVALID_CONVERSION;
}


// Class protocols::opcua::opcua_addr_space::opcua_reference_type 

opcua_reference_type::opcua_reference_type()
      : is_abstract(false),
        symmetric(false)
    , opcua_base_node_type(node_class_type::reference_type)
{
}



void opcua_reference_type::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
{
    switch (id)
    {
    case attribute_id::is_abstract:
        value_storage.value = variant_type(is_abstract);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::symetric:
        value_storage.value = variant_type(symmetric);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::inverse_name:
        value_storage.value = variant_type(inverse_name);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    default:
        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
    }
}


// Class protocols::opcua::opcua_addr_space::opcua_property_node 

opcua_property_node::opcua_property_node()
{
}



void opcua_property_node::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
{
    opcua_variable_base_node::internal_read_attribute(id, range, encoding, value_storage, config_ts);
}


// Class protocols::opcua::opcua_addr_space::opcua_variable_node 

opcua_variable_node::opcua_variable_node()
{
}



void opcua_variable_node::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts)
{
    opcua_variable_base_node::internal_read_attribute(id, range, encoding, value_storage, config_ts);
}

rx_node_id opcua_variable_node::get_type_id () const
{
    return type_id;
}

//constexpr reference_data::reference_data(uint32_t ref_id, uint32_t targ_id)
//    : resolved_node{ nullptr }
//    , reference_id{ rx_node_id(ref_id, 0) }
//    , target_id{ rx_node_id(targ_id, 0) }
//{
//
//}
// Class protocols::opcua::opcua_addr_space::reference_data 

reference_data::reference_data()
    : resolved_node(nullptr)
{
}

reference_data::reference_data (rx_node_id ref_id, rx_node_id targ_id)
    : resolved_node(nullptr)
    , reference_id(std::move(ref_id))
    , target_id(std::move(targ_id))
{
}

reference_data::reference_data (opcua_std_ref_t data)
    : resolved_node{ reinterpret_cast<opcua_node_base*>((uintptr_t)data.target_idx) }
    , reference_id{ data.ref_id, 0 }
    , target_id{ data.target_id, 0 }
{
}



// Class protocols::opcua::opcua_addr_space::opcua_node_base 


// Class protocols::opcua::opcua_addr_space::opcua_address_space_base 


// Class protocols::opcua::opcua_addr_space::node_references 


bool node_references::connect_node_reference (opcua_node_base* node, const reference_data& ref_data, bool inverse)
{
    if (inverse)
    {
        for (auto& one : references)
        {
            if (one.reference_id == ref_data.reference_id && one.target_id == node->get_node_id())
                return false;// duplicated reference
        }
        reference_data new_ref;
        new_ref.reference_id = ref_data.reference_id;
        new_ref.target_id = node->get_node_id();
        new_ref.resolved_node = node;
        references.push_back(std::move(new_ref));

        return true;
    }
    else
    {
        for (auto& one : inverse_references)
        {
            if (one.reference_id == ref_data.reference_id && one.target_id == node->get_node_id())
                return false;// duplicated reference
        }
        reference_data new_ref;
        new_ref.reference_id = ref_data.reference_id;
        new_ref.target_id = node->get_node_id();
        new_ref.resolved_node = node;
        inverse_references.push_back(std::move(new_ref));

        return true;
    }
}

bool node_references::disconnect_node_reference (opcua_node_base* node, const reference_data& ref_data, bool inverse)
{
    if (inverse)
    {
        auto it = references.begin();
        while (it != references.end())
        {
            if (it->reference_id == ref_data.reference_id && it->target_id == node->get_node_id())
            {
                RX_ASSERT(node == it->resolved_node);
                references.erase(it);
                return true;
            }
        }
        return false;
    }
    else
    {
        auto it = inverse_references.begin();
        while (it != inverse_references.end())
        {
            if (it->reference_id == ref_data.reference_id && it->target_id == node->get_node_id())
            {
                RX_ASSERT(node == it->resolved_node);
                inverse_references.erase(it);
                return true;
            }
        }
        return false;
    }
}


// Class protocols::opcua::opcua_addr_space::opcua_object_node 

opcua_object_node::opcua_object_node()
      : event_notifier(0)
    , opcua_base_node_type(node_class_type::object)
{
}



void opcua_object_node::internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
{
    switch (id)
    {
    case attribute_id::event_notifier:
        value_storage.value = variant_type(event_notifier);
        value_storage.set_timestamp(timestamp_);
        value_storage.status_code = opcid_OK;
        break;
    default:
        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
    }
}

rx_node_id opcua_object_node::get_type_id () const
{
    return type_id;
}


} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols

