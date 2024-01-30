

/****************************************************************************
*
*  system\storage_base\rx_storage.cpp
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

#include "system/server/rx_server.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_storage
#include "system/storage_base/rx_storage.h"

#include "system/server/rx_server.h"
#include "lib/rx_ser_json.h"
#include "lib/rx_ser_bin.h"


extern "C"
{
    rx_result_struct c_post_storage_job(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        return rx_result(RX_NOT_IMPLEMENTED).move();
    }

    host_storage_def_struct _g_storage_def_
    {
        {
            c_post_storage_job
        }
    };


}


namespace rx_platform {

namespace storage_base {

// Class rx_platform::storage_base::rx_platform_storage

rx_platform_storage::rx_platform_storage()
{
}


rx_platform_storage::~rx_platform_storage()
{
}



rx_result rx_platform_storage::list_storage_roles (std::vector<rx_roles_storage_item_ptr>& items)
{
    return true;
}


// Class rx_platform::storage_base::rx_storage_item

rx_storage_item::rx_storage_item (rx_storage_item_type storage_type)
      : storage_type_(storage_type)
{
}


rx_storage_item::~rx_storage_item()
{
}



bool rx_storage_item::is_read_only () const
{
  return false;

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

bool rx_code_storage_item::preprocess_meta_data (meta_data& data)
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

rx_result_with<rx_storage_item_ptr> rx_code_storage::get_item_storage (const meta_data& data, rx_item_type type)
{
    rx_storage_item_ptr ret = std::make_unique<rx_code_storage_item>();
    return ret;
}

rx_result_with<rx_storage_item_ptr> rx_code_storage::get_runtime_storage (const meta_data& data, rx_item_type type)
{
    return "Not implemented for this storage type.";
}

string_type rx_code_storage::get_storage_reference ()
{
    return RX_CODE_STORAGE_NAME;
}

void rx_code_storage::preprocess_meta_data (meta_data& data)
{
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

rx_result rx_storage_connection::list_storage_roles (std::vector<rx_roles_storage_item_ptr>& items)
{
    for (auto& one : initialized_storages_)
    {
        one.second->list_storage_roles(items);
    }
    return true;
}


// Class rx_platform::storage_base::rx_empty_storage


string_type rx_empty_storage::get_storage_info ()
{
    return RX_NULL_ITEM_NAME " storage";
}

rx_result rx_empty_storage::init_storage (const string_type& storage_reference, hosting::rx_platform_host* host)
{
    return true;
}

rx_result rx_empty_storage::list_storage (std::vector<rx_storage_item_ptr>& items)
{
    return true;
}

bool rx_empty_storage::is_valid_storage () const
{
    return true;
}

rx_result_with<rx_storage_item_ptr> rx_empty_storage::get_item_storage (const meta_data& data, rx_item_type type)
{
    return RX_NOT_SUPPORTED;
}

rx_result_with<rx_storage_item_ptr> rx_empty_storage::get_runtime_storage (const meta_data& data, rx_item_type type)
{
    return RX_NOT_SUPPORTED;
}

string_type rx_empty_storage::get_storage_reference ()
{
    return RX_NULL_ITEM_NAME;
}

void rx_empty_storage::preprocess_meta_data (meta_data& data)
{
}


// Class rx_platform::storage_base::rx_empty_storage_connection


string_type rx_empty_storage_connection::get_storage_reference () const
{
    return RX_NULL_ITEM_NAME;
}

rx_result_with<rx_storage_ptr> rx_empty_storage_connection::get_and_init_storage (const string_type& name, hosting::rx_platform_host* host)
{
    rx_storage_ptr ret = rx_create_reference<rx_empty_storage>();
    return ret;
}

string_type rx_empty_storage_connection::get_storage_info () const
{
    return RX_NULL_ITEM_NAME " storage";
}


// Class rx_platform::storage_base::rx_plugin_storage

rx_plugin_storage::rx_plugin_storage (plugin_storage_struct* impl)
      : impl_(impl)
{
    impl_->host = this;
    impl_->host_def = &_g_storage_def_;
}


rx_plugin_storage::~rx_plugin_storage()
{
}



string_type rx_plugin_storage::get_storage_info ()
{
    return "E jebi GA!!!";
}

rx_result rx_plugin_storage::list_storage (std::vector<rx_storage_item_ptr>& items)
{
    return RX_NOT_IMPLEMENTED;
}

bool rx_plugin_storage::is_valid_storage () const
{
    return true;
}

rx_result_with<rx_storage_item_ptr> rx_plugin_storage::get_item_storage (const meta_data& data, rx_item_type type)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result_with<rx_storage_item_ptr> rx_plugin_storage::get_runtime_storage (const meta_data& data, rx_item_type type)
{
    return RX_NOT_IMPLEMENTED;
}

string_type rx_plugin_storage::get_storage_reference ()
{
    return string_type();
}

rx_result rx_plugin_storage::init_storage (const string_type& name, const string_type& ref)
{
    return impl_->def->init_storage(impl_->anchor.target, ref.c_str());
}

void rx_plugin_storage::preprocess_meta_data (meta_data& data)
{
}


// Class rx_platform::storage_base::rx_plugin_storage_connection

rx_plugin_storage_connection::rx_plugin_storage_connection (rx_storage_constructor_t construct_func)
      : constructor_(construct_func)
{
}



string_type rx_plugin_storage_connection::get_storage_reference () const
{
    return "Borisa!!!";
}

rx_result_with<rx_storage_ptr> rx_plugin_storage_connection::get_and_init_storage (const string_type& name, hosting::rx_platform_host* host)
{
    auto ptr = constructor_();
    rx_plugin_storage::smart_ptr storage = rx_create_reference<rx_plugin_storage>(ptr);
    auto result = storage->init_storage(name, reference_);
    if (result)
    {
        return rx_storage_ptr(storage);
    }
    else
    {
        return result.errors();
    }
}

string_type rx_plugin_storage_connection::get_storage_info () const
{
    return RX_NULL_ITEM_NAME " storage";
}

rx_result rx_plugin_storage_connection::init_connection (const string_type& storage_reference, hosting::rx_platform_host* host)
{
    reference_ = storage_reference;
    return true;
}


// Class rx_platform::storage_base::rx_plugin_storage_item

rx_plugin_storage_item::rx_plugin_storage_item()
    : rx_storage_item(rx_storage_item_type::none)
{
}



base_meta_reader& rx_plugin_storage_item::read_stream ()
{
    RX_ASSERT(false);
    // this should not happen but be safe of stupidity
    static memory::std_buffer dummy_buffer;
    static serialization::std_buffer_reader dummy(dummy_buffer);
    return dummy;
}

base_meta_writer& rx_plugin_storage_item::write_stream ()
{
    RX_ASSERT(false);
    // this should not happen but be safe of stupidity
    static memory::std_buffer dummy_buffer;
    static serialization::std_buffer_writer dummy(dummy_buffer);
    return dummy;
}

rx_result rx_plugin_storage_item::open_for_read ()
{
    return RX_NOT_IMPLEMENTED;
}

rx_result rx_plugin_storage_item::open_for_write ()
{
    return RX_NOT_IMPLEMENTED;
}

rx_result rx_plugin_storage_item::close_read ()
{
    return RX_NOT_IMPLEMENTED;
}

rx_result rx_plugin_storage_item::commit_write ()
{
    return RX_NOT_IMPLEMENTED;
}

const string_type& rx_plugin_storage_item::get_item_reference () const
{
    static string_type ret = "Borisa!!!";
    return ret;
}

rx_result rx_plugin_storage_item::delete_item ()
{
    return RX_NOT_IMPLEMENTED;
}

bool rx_plugin_storage_item::preprocess_meta_data (meta_data& data)
{
    return false;
}

string_type rx_plugin_storage_item::get_item_path () const
{
    return "Borisa!!!";
}


// Class rx_platform::storage_base::rx_plugin_storage_type

rx_plugin_storage_type::rx_plugin_storage_type (const string_type& prefix, rx_storage_constructor_t construct_func)
      : prefix_(prefix),
        constructor_(construct_func)
{
}



string_type rx_plugin_storage_type::get_storage_info ()
{
    return "Neki info";
}

rx_storage_connection::smart_ptr rx_plugin_storage_type::construct_storage_connection ()
{
    return rx_create_reference<rx_plugin_storage_connection>(constructor_);
}

string_type rx_plugin_storage_type::get_reference_prefix () const
{
    return "tst";
}


// Class rx_platform::storage_base::rx_roles_storage_item

rx_roles_storage_item::rx_roles_storage_item()
{
}


rx_roles_storage_item::~rx_roles_storage_item()
{
}



// Class rx_platform::storage_base::rx_others_storage

rx_others_storage::rx_others_storage()
{
}



string_type rx_others_storage::get_storage_info ()
{
    return RX_OTHERS_STORAGE_NAME;
}

rx_result rx_others_storage::list_storage (std::vector<rx_storage_item_ptr>& items)
{
    return true;
}

bool rx_others_storage::is_valid_storage () const
{
    return true;
}

rx_result_with<rx_storage_item_ptr> rx_others_storage::get_item_storage (const meta_data& data, rx_item_type type)
{
    rx_other_storage_item* ret = new rx_other_storage_item();
    ret->meta_info = data;
    return rx_storage_item_ptr(ret);
}

rx_result_with<rx_storage_item_ptr> rx_others_storage::get_runtime_storage (const meta_data& data, rx_item_type type)
{
    rx_other_storage_item* ret = new rx_other_storage_item();
    ret->meta_info = data;
    return rx_storage_item_ptr(ret);
}

string_type rx_others_storage::get_storage_reference ()
{
    return RX_OTHERS_STORAGE_NAME;
}

void rx_others_storage::preprocess_meta_data (meta_data& data)
{
}


// Class rx_platform::storage_base::rx_other_storage_item

rx_other_storage_item::rx_other_storage_item()
    : rx_storage_item(rx_storage_item_type::none)
{
}



base_meta_reader& rx_other_storage_item::read_stream ()
{
    RX_ASSERT(false);
    // this should not happen but be safe of stupidity
    static memory::std_buffer dummy_buffer;
    static serialization::std_buffer_reader dummy(dummy_buffer);
    return dummy;
}

base_meta_writer& rx_other_storage_item::write_stream ()
{
    RX_ASSERT(false);
    // this should not happen but be safe of stupidity
    static memory::std_buffer dummy_buffer;
    static serialization::std_buffer_writer dummy(dummy_buffer);
    return dummy;
}

rx_result rx_other_storage_item::open_for_read ()
{
    return "Storage is read-only!";
}

rx_result rx_other_storage_item::open_for_write ()
{
    return "Storage is read-only!";
}

rx_result rx_other_storage_item::close_read ()
{
    return "Storage is read-only!";
}

rx_result rx_other_storage_item::commit_write ()
{
    return "Storage is read-only!";
}

const string_type& rx_other_storage_item::get_item_reference () const
{
    static string_type name(RX_OTHERS_STORAGE_NAME);
    return name;
}

rx_result rx_other_storage_item::delete_item ()
{
    return true;
}

bool rx_other_storage_item::preprocess_meta_data (meta_data& data)
{
    return false;
}

string_type rx_other_storage_item::get_item_path () const
{
    return meta_info.get_full_path();
}

bool rx_other_storage_item::is_read_only () const
{
    return true;
}


} // namespace storage_base
} // namespace rx_platform

