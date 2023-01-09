

/****************************************************************************
*
*  protocols\opcua\rx_opcua_std.cpp
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

// rx_opcua_std
#include "protocols/opcua/rx_opcua_std.h"

#include "rx_opcua_server.h"


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
    references_tree_ = references_tree_type{
        { 0 , {1,2,3}}
    };
}


opcua_std_address_space::~opcua_std_address_space()
{
}



void opcua_std_address_space::read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const
{
    for (const auto& one : to_read)
    {
        data_value temp_val;
        temp_val.status_code = opcid_Bad_NodeIdUnknown;
        if (one.node_id.is_opc())
        {
            locks::const_auto_read_lock _(get_lock());

            auto val_it = get_valued_by_id(one.node_id.get_numeric());
            if (val_it != registered_valued_nodes_.end())
            {
                val_it->read_attribute(one.attr_id, one.range, one.data_encoding.name, temp_val, config_ts_);
            }
            else
            {
                auto it = get_by_id(one.node_id.get_numeric());
                if (it != registered_nodes_.end())
                {
                    it->read_attribute(one.attr_id, one.range, one.data_encoding.name, temp_val, config_ts_);
                }
            }
        }
        values.push_back(std::move(temp_val));
    }
}

std::pair<opcua_result_t, runtime_transaction_id_t> opcua_std_address_space::write_attribute (const rx_node_id& node_id, attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep)
{
    std::pair<opcua_result_t, runtime_transaction_id_t> result = { opcid_Bad_NodeIdUnknown, 0 };
    if (node_id.is_opc())
    {
        locks::const_auto_read_lock _(get_lock());

        auto val_it = get_valued_by_id(node_id.get_numeric());
        if (val_it != registered_valued_nodes_.end())
        {
            result = val_it->write_attribute(id, range, value, ep);
        }
        else
        {
            auto it = get_by_id(node_id.get_numeric());
            if (it != registered_nodes_.end())
            {
                result = it->write_attribute(id, range, value, ep);
            }
        }
    }
    return result;
}

void opcua_std_address_space::browse (const opcua_view_description& view, const std::vector<opcua_browse_description>& to_browse, std::vector<browse_result_internal>& results) const
{
    for (const auto& one : to_browse)
    {
        browse_result_internal temp_val;
        temp_val.status_code = opcid_Bad_NodeIdUnknown;
        if (one.node_id.is_opc())
        {
            locks::const_auto_read_lock _(get_lock());

            opcua_browse_context ctx(this);

            auto val_it = get_valued_by_id(one.node_id.get_numeric());
            if (val_it != registered_valued_nodes_.end())
            {
                val_it->browse(one, temp_val, &ctx);
            }
            else
            {
                auto it = get_by_id(one.node_id.get_numeric());
                if (it != registered_nodes_.end())
                {
                    it->browse(one, temp_val, &ctx);
                }
            }
        }
        results.push_back(std::move(temp_val));
    }
}

void opcua_std_address_space::translate (const std::vector<browse_path>& browse_paths, std::vector<browse_path_result>& results, opcua_address_space_base* root) const
{
    for (const auto& one : browse_paths)
    {
        browse_path_result temp_val;
        temp_val.status_code = opcid_Bad_NodeIdUnknown;
        if (one.starting_node.is_opc())
        {
            locks::const_auto_read_lock _(get_lock());

            opcua_browse_context ctx(this);

            auto val_it = get_valued_by_id(one.starting_node.get_numeric());
            if (val_it != registered_valued_nodes_.end())
            {
                temp_val.status_code = opcid_OK;
                val_it->translate(one.path, temp_val, &ctx);
            }
            else
            {
                auto it = get_by_id(one.starting_node.get_numeric());
                if (it != registered_nodes_.end())
                {
                    temp_val.status_code = opcid_OK;
                    it->translate(one.path, temp_val, &ctx);
                }
            }
        }
        results.push_back(std::move(temp_val));
    }
}

rx_result opcua_std_address_space::fill_relation_types (const rx_node_id& base_id, bool include_subtypes, std::set<rx_node_id>& buffer) const
{
    RX_ASSERT(!base_id.is_null());
    if (base_id.is_opc())
    {
        auto it = references_tree_.find(base_id.get_numeric());
        if (it != references_tree_.end())
        {
            buffer.insert(base_id);
            if (include_subtypes)
            {
                for (const auto& one : it->second)
                {
                    buffer.emplace(one, 0);
                }
            }
        }
    }
    return true;
}

rx_result opcua_std_address_space::set_node_value (const rx_node_id& id, values::rx_value&& val)
{
    if (!id.is_null() && id.is_opc())
    {
        locks::auto_write_lock _(get_lock());

        auto val_node_it = get_valued_by_id(id.get_numeric());
        if (val_node_it != registered_valued_nodes_.end())
        {
            return val_node_it->set_node_value(std::move(val));
        }
        else
        {
            auto node_it = get_by_id(id.get_numeric());
            if (node_it != registered_nodes_.end())
            {
                return node_it->set_node_value(std::move(val));
            }
        }
    }
    return RX_INVALID_ARGUMENT;
}

locks::rw_slim_lock* opcua_std_address_space::get_lock ()
{
    return &ns_lock_;
}

const locks::rw_slim_lock* opcua_std_address_space::get_lock () const
{
    return &ns_lock_;
}

opcua_node_base* opcua_std_address_space::connect_node_reference (opcua_node_base* node, const reference_data& ref_data, bool inverse)
{
    if (ref_data.target_id.is_null() || !ref_data.target_id.is_opc())
        return nullptr;
    auto it = get_by_id(ref_data.target_id.get_numeric());
    if (it != registered_nodes_.end())
    {
        if (!it->references_.connect_node_reference(node, ref_data, inverse))
            return nullptr;
        return &(*it);
    }
    auto it_val = get_valued_by_id(ref_data.target_id.get_numeric());
    if (it_val != registered_valued_nodes_.end())
    {
        if (!it_val->references_.connect_node_reference(node, ref_data, inverse))
            return nullptr;
        return &(*it_val);
    }
    return nullptr;
}

opcua_result_t opcua_std_address_space::register_value_monitor (opcua_subscriptions::opcua_monitored_value* who, data_value& val)
{
    return opcid_Bad_NotSupported;
}

opcua_result_t opcua_std_address_space::unregister_value_monitor (opcua_subscriptions::opcua_monitored_value* who)
{
    return opcid_Bad_NotSupported;
}

opcua_std_address_space::registered_nodes_type::const_iterator opcua_std_address_space::get_by_id (uint32_t id) const
{
    auto it = std::lower_bound(registered_nodes_.begin(), registered_nodes_.end(), id, std_nodes_comparator());
    if (it != registered_nodes_.end() && it->node_id == id)
        return it;
    else
        return registered_nodes_.end();
}

opcua_std_address_space::registered_nodes_type::iterator opcua_std_address_space::get_by_id (uint32_t id)
{
    auto it = std::lower_bound(registered_nodes_.begin(), registered_nodes_.end(), id, std_nodes_comparator());
    if (it != registered_nodes_.end() && it->node_id == id)
        return it;
    else
        return registered_nodes_.end();
}

opcua_std_address_space::registered_valued_nodes_type::const_iterator opcua_std_address_space::get_valued_by_id (uint32_t id) const
{
    auto it = std::lower_bound(registered_valued_nodes_.begin(), registered_valued_nodes_.end(), id, std_nodes_comparator());
    if (it != registered_valued_nodes_.end() && it->node_id == id)
        return it;
    else
        return registered_valued_nodes_.end();
}

opcua_std_address_space::registered_valued_nodes_type::iterator opcua_std_address_space::get_valued_by_id (uint32_t id)
{
    auto it = std::lower_bound(registered_valued_nodes_.begin(), registered_valued_nodes_.end(), id, std_nodes_comparator());
    if (it != registered_valued_nodes_.end() && it->node_id == id)
        return it;
    else
        return registered_valued_nodes_.end();
}


// Class protocols::opcua::opcua_addr_space::opcua_std_node 

opcua_std_node::opcua_std_node()
    : node_class(node_class_type::node),
    name(nullptr),
    inverse_name(nullptr),
    type_id(0),
    node_id(0)
{
}

opcua_std_node::opcua_std_node (uint32_t id)
    : node_class(node_class_type::node),
    name(nullptr),
    inverse_name(nullptr),
    type_id(0),
    node_id(id)
{
}

opcua_std_node::opcua_std_node (const opcua_std_node_argument_t& arg)
    : node_class{ arg.class_type },
    name{ arg.name },
    inverse_name{ arg.inverse_name },
    type_id{ arg.type_id },
    node_id{ arg.id },
    bit_data{ arg.bit_options }
{
    if (arg.references.size > 0 && arg.references.data)
    {
        references_.references.insert(references_.references.end()
            , arg.references.data, arg.references.data + arg.references.size);
    }
    if (arg.inverse_references.size > 0 && arg.inverse_references.data)
    {
        references_.inverse_references.insert(references_.inverse_references.end()
            , arg.inverse_references.data, arg.inverse_references.data + arg.inverse_references.size);
    }
}



void opcua_std_node::read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
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
        value_storage.value = variant_type(rx_node_id::opcua_standard_id(node_id));
        value_storage.set_timestamp(config_ts);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::node_class:
        value_storage.value = variant_type((int32_t)node_class);
        value_storage.set_timestamp(config_ts);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::browse_name:
        value_storage.value = variant_type(qualified_name{ 0, name });
        value_storage.set_timestamp(config_ts);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::display_name:
        value_storage.value = variant_type(localized_text{ name });
        value_storage.set_timestamp(config_ts);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::description:
        value_storage.value = variant_type(localized_text{ name });
        value_storage.set_timestamp(config_ts);
        value_storage.status_code = opcid_OK;
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
                        value_storage.value = variant_type(bit_data[abstract_idx]);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    case attribute_id::symetric:
                        value_storage.value = variant_type(bit_data[symmetric_idx]);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    case attribute_id::inverse_name:
                        value_storage.value = variant_type(inverse_name);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    default:
                        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
                    }
                }
                break;
            case node_class_type::object:
                {
                    switch (id)
                    {
                    case attribute_id::event_notifier:
                        value_storage.value = variant_type((uint8_t)0);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    default:
                        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
                    }
                }
                break;
            case node_class_type::object_type:
                {
                    switch (id)
                    {
                    case attribute_id::is_abstract:
                        value_storage.value = variant_type(bit_data[abstract_idx]);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    default:
                        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
                    }
                }
                break;
            case node_class_type::method:
                {
                    switch (id)
                    {
                    case attribute_id::executable:
                        value_storage.value = variant_type(bit_data[executable_idx]);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    case attribute_id::user_executable:
                        value_storage.value = variant_type(bit_data[user_executable_idx]);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    default:
                        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
                    }
                }
                break;
            case node_class_type::data_type:
                {
                    switch (id)
                    {
                    case attribute_id::is_abstract:
                        value_storage.value = variant_type(bit_data[abstract_idx]);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                        // TODO DEFINITION MISSING!!!
                    default:
                        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
                    }
                }
                break;
            default:
                value_storage.status_code = opcid_Bad_AttributeIdInvalid;
            }
        }
    }
}

std::pair<opcua_result_t, runtime_transaction_id_t> opcua_std_node::write_attribute (attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep)
{
    return { opcid_Bad_NotWritable, 0 };
}

void opcua_std_node::browse (const opcua_browse_description& to_browse, browse_result_internal& result, opcua_browse_context* ctx) const
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
        for (const auto& one : references_.references)
        {
            if (one.resolved_node
                && (to_browse.node_class_mask==0 
                    || ((uint32_t)one.resolved_node->get_node_class() & to_browse.node_class_mask) != 0))
            {
                if(!refIds.empty() && refIds.count(one.reference_id))
                    result.add_reference_data(true, one, to_browse);
            }
        }
    }
    if (to_browse.direction == browse_direction_type::browse_direction_both
        || to_browse.direction == browse_direction_type::browse_direction_backward)
    {
        for (const auto& one : references_.inverse_references)
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

void opcua_std_node::translate (const relative_path& path, browse_path_result& results, opcua_browse_context* ctx) const
{
    std::set<rx_node_id> refIds;
    if (!results.targets.empty() && results.targets.rbegin()->remaining_index < path.elements.size())
    {
        results.targets.rbegin()->remaining_index = (uint32_t)(-1);
        return;
    }
    size_t idx = 0;
    if (!results.targets.empty())
        idx = results.targets.rbegin()->remaining_index + 1;
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
        for (const auto& one : references_.references)
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
        for (const auto& one : references_.inverse_references)
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

node_class_type opcua_std_node::get_node_class () const
{
    return node_class;
}

rx_node_id opcua_std_node::get_node_id () const
{
    return rx_node_id::opcua_standard_id(node_id);
}

qualified_name opcua_std_node::get_browse_name () const
{
    return qualified_name{ 0, name };
}

string_type opcua_std_node::get_display_name () const
{
    return name;
}

rx_node_id opcua_std_node::get_type_id () const
{
    return rx_node_id::opcua_standard_id(type_id);
}

rx_result opcua_std_node::set_node_value (values::rx_value&& val)
{
    return RX_NOT_SUPPORTED;
}

node_references& opcua_std_node::get_reference_data ()
{
    return references_;
}


// Class protocols::opcua::opcua_addr_space::opcua_std_valued_node 

opcua_std_valued_node::opcua_std_valued_node()
    : data_type_id(0),
    value_rank(0)
{
}

opcua_std_valued_node::opcua_std_valued_node (opcua_std_valued_node_argument_t arg)
    : opcua_std_node{ arg.base_data },
    data_type_id{ arg.data_type_id },
    value_rank{ arg.value_rank },
    dimensions{ arg.dimensions }
{
}



void opcua_std_valued_node::read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const
{
       
    switch (id)
    {
    case attribute_id::value:
        if (value.value.get_type() == opcid_Null && data_type_id != 0)
        {// handle default value
            const_size_vector<uint32_t> dummy;
            const_cast<opcua_std_valued_node*>(this)->value.value.set_default(data_type_id, value_rank, dummy);
        }
        value_storage.value = value.value;
        if (value.source_ts.t_value)
        {
            value_storage.source_ts = value.source_ts;
            value_storage.source_ps = 0;
        }
        if (value.server_ts.t_value)
        {
            value_storage.server_ts = value.server_ts;
            value_storage.server_ps = 0;
        }
        if(!value_storage.server_ts.t_value)
        {
            value_storage.server_ps = 0;
            value_storage.server_ts = config_ts;
        }
        if (value.status_code != opcid_OK)
        {
            value_storage.status_code = value.status_code;
        }
        else
        {
            // i know it's a bit strange peace of code below
            // but in case of extending this functionality,
            // witch if placeable?
            // i'll just leave it like this
            value_storage.status_code = opcid_OK;
        }
        break;
    case attribute_id::data_type:
        value_storage.value = variant_type(rx_node_id::opcua_standard_id(data_type_id));
        value_storage.set_timestamp(config_ts);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::value_rank:
        value_storage.value = variant_type(value_rank);
        value_storage.set_timestamp(config_ts);
        value_storage.status_code = opcid_OK;
        break;
    case attribute_id::array_dimensions:
        value_storage.status_code = opcid_Bad_AttributeIdInvalid;
        break;
       default:
        {
            switch (node_class)
            {
            case node_class_type::variable:
                {
                    switch (id)
                    {
                    case attribute_id::access_level:
                    case attribute_id::user_access_level:
                        {
                            opc_access_level temp = opc_access_level::current_read;
                            if (bit_data[historizing_idx])
                                temp = temp | opc_access_level::history_read;
                            value_storage.value = variant_type((uint8_t)temp);
                            value_storage.set_timestamp(config_ts);
                            value_storage.status_code = opcid_OK;
                        }
                        break;
                        break;
                    case attribute_id::minimum_sampling:
                        value_storage.value = variant_type(0.0);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    case attribute_id::historizing:
                        value_storage.value = variant_type(bit_data[historizing_idx]);
                        value_storage.set_timestamp(config_ts);
                        value_storage.status_code = opcid_OK;
                        break;
                    case attribute_id::access_level_ex:
                        {
                            opc_access_level_ex temp = opc_access_level_ex::current_read;
                            if (bit_data[historizing_idx])
                                temp = temp | opc_access_level_ex::history_read;
                            value_storage.value = variant_type((uint8_t)temp);
                            value_storage.set_timestamp(config_ts);
                            value_storage.status_code = opcid_OK;
                            value_storage.value = variant_type((uint32_t)temp);
                        }
                        break;
                    default:
                        opcua_std_node::read_attribute(id, range, encoding, value_storage, config_ts);
                    }
                }
                break;
            default:
                opcua_std_node::read_attribute(id, range, encoding, value_storage, config_ts);
            }
        }
    }
}

rx_result opcua_std_valued_node::set_node_value (values::rx_value&& val)
{
    rx_result ret;
    if (value.from_rx_value(std::move(val)))
        return true;
    else
        return RX_INVALID_CONVERSION;
}


} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols

