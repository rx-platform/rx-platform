

/****************************************************************************
*
*  system\storage_base\rx_storage.cpp
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


// rx_host
#include "system/hosting/rx_host.h"
// rx_storage
#include "system/storage_base/rx_storage.h"

#include "system/server/rx_server.h"
#define RX_CODE_STORAGE_NAME "<code>"


namespace rx_platform {

namespace storage_base {

// Class rx_platform::storage_base::rx_platform_storage 

rx_platform_storage::rx_platform_storage()
{
}


rx_platform_storage::~rx_platform_storage()
{
}



rx_result rx_platform_storage::init_storage (const string_type& storage_reference, hosting::rx_platform_host* host)
{
	return RX_NOT_IMPLEMENTED;
}

void rx_platform_storage::deinit_storage ()
{
}


// Class rx_platform::storage_base::rx_storage_item 

rx_storage_item::rx_storage_item (rx_storage_item_type storage_type)
      : storage_type_(storage_type)
{
}


rx_storage_item::~rx_storage_item()
{
}



// Class rx_platform::storage_base::rx_platform_storage_type 

rx_platform_storage_type::rx_platform_storage_type()
{
}


rx_platform_storage_type::~rx_platform_storage_type()
{
}



// Class rx_platform::storage_base::rx_code_storage_item 

rx_code_storage_item::rx_code_storage_item()
    : rx_storage_item(rx_storage_item_type::none)
{
}



base_meta_reader& rx_code_storage_item::read_stream ()
{
    RX_ASSERT(false);
    // this should not happen but be safe of stupidity
    static memory::std_buffer dummy_buffer;
    static serialization::std_buffer_reader dummy(dummy_buffer);
    return dummy;
}

base_meta_writer& rx_code_storage_item::write_stream ()
{
    RX_ASSERT(false);
    // this should not happen but be safe of stupidity
    static memory::std_buffer dummy_buffer;
    static serialization::std_buffer_writer dummy(dummy_buffer);
    return dummy;
}

rx_result rx_code_storage_item::open_for_read ()
{
    return "Storage can not be defined for this item";
}

rx_result rx_code_storage_item::open_for_write ()
{
    return "Storage can not be defined for this item";
}

rx_result rx_code_storage_item::close_read ()
{
    return "Storage can not be defined for this item";
}

rx_result rx_code_storage_item::commit_write ()
{
    return "Storage can not be defined for this item";
}

const string_type& rx_code_storage_item::get_item_reference () const
{
    static string_type name(RX_CODE_STORAGE_NAME);
    return name;
}

rx_result rx_code_storage_item::delete_item ()
{
    return "Delete not valid for this item.";
}

bool rx_code_storage_item::preprocess_meta_data (meta::meta_data& data)
{
    return false;
}

string_type rx_code_storage_item::get_item_path () const
{
    static string_type name(RX_CODE_STORAGE_NAME);
    return name;
}


// Class rx_platform::storage_base::rx_code_storage 

rx_code_storage::rx_code_storage()
{
}



string_type rx_code_storage::get_storage_info ()
{
    return RX_CODE_STORAGE_NAME;
}

rx_result rx_code_storage::list_storage (std::vector<rx_storage_item_ptr>& items)
{
    return true;
}

bool rx_code_storage::is_valid_storage () const
{
    return true;
}

rx_result_with<rx_storage_item_ptr> rx_code_storage::get_item_storage (const meta::meta_data& data, rx_item_type type)
{
    rx_storage_item_ptr ret = std::make_unique<rx_code_storage_item>();
    return ret;
}

rx_result_with<rx_storage_item_ptr> rx_code_storage::get_runtime_storage (const meta::meta_data& data, rx_item_type type)
{
    return "Not implemented for this storage type.";
}

string_type rx_code_storage::get_storage_reference ()
{
    return RX_CODE_STORAGE_NAME;
}

rx_result split_storage_reference(const string_type full_ref, string_type& type, string_type& reference)
{
    if (full_ref.empty())
        return RX_NULL_ITEM_NAME " reference.";

    auto idx = full_ref.find("://");
    if (idx == string_type::npos)
    {
        // nothing useful found, use default
        type = "file";
        reference = full_ref;
    }
    else
    {
        type = full_ref.substr(0, idx);
        reference = full_ref.substr(idx + 3);
        if (reference.empty())
            return RX_NULL_ITEM_NAME " reference.";
    }
    return true;
}
// Class rx_platform::storage_base::rx_storage_connection 


rx_result_with<rx_storage_ptr> rx_storage_connection::get_storage (const string_type& name, hosting::rx_platform_host* host)
{
    auto it = initialized_storages_.find(name);
    if (it != initialized_storages_.end())
    {
        return it->second;
    }
    else if (rx_gate::instance().get_platform_status() == rx_platform_status::initializing)
    {
        auto result = get_and_init_storage(name, host);
        if (result)
            initialized_storages_.emplace(name, result.value());
        return result;
    }
    else
    {
        return "Storage "s + name + "not found!";
    }
}

rx_result rx_storage_connection::init_connection (const string_type& storage_reference, hosting::rx_platform_host* host)
{
    return true;
}

rx_result rx_storage_connection::deinit_connection ()
{
    for (auto one : initialized_storages_)
    {
        one.second->deinit_storage();
    }
    initialized_storages_.clear();
    return true;
}

std::vector<std::pair<string_type, string_type> > rx_storage_connection::get_mounted_storages () const
{
    std::vector<std::pair<string_type, string_type> > ret;
    ret.reserve(initialized_storages_.size());
    for (const auto& one : initialized_storages_)
    {
        ret.emplace_back(one.first, one.second->get_base_path());
    }
    return ret;
}


} // namespace storage_base
} // namespace rx_platform

