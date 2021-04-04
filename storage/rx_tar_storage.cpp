

/****************************************************************************
*
*  storage\rx_tar_storage.cpp
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


// rx_tar_storage
#include "storage/rx_tar_storage.h"



namespace storage {

namespace tar {

// Class storage::tar::tar_file_storage_holder 


string_type tar_file_storage_holder::get_storage_info ()
{
}

rx_result tar_file_storage_holder::init_storage (const string_type& storage_reference, hosting::rx_platform_host* host)
{
}

rx_result_with<rx_storage_ptr> tar_file_storage_holder::get_and_init_storage (const string_type& name, hosting::rx_platform_host* host)
{
}

string_type tar_file_storage_holder::get_storage_reference ()
{
}


// Class storage::tar::tar_file_storage 

tar_file_storage::tar_file_storage()
{
}


tar_file_storage::~tar_file_storage()
{
}



string_type tar_file_storage::get_storage_info ()
{
}

sys_handle_t tar_file_storage::get_host_test_file (const string_type& path)
{
}

sys_handle_t tar_file_storage::get_host_console_script_file (const string_type& path)
{
}

const string_type& tar_file_storage::get_license ()
{
}

rx_result tar_file_storage::init_storage (const string_type& storage_reference, hosting::rx_platform_host* host)
{
}

void tar_file_storage::deinit_storage ()
{
}

rx_result tar_file_storage::list_storage (std::vector<rx_storage_item_ptr>& items)
{
}

rx_result tar_file_storage::recursive_list_storage (const string_type& path, const string_type& file_path, std::vector<rx_storage_item_ptr>& items)
{
}

string_type tar_file_storage::get_storage_reference ()
{
}

bool tar_file_storage::is_valid_storage () const
{
}

std::unique_ptr<rx_file_item> tar_file_storage::get_storage_item_from_file_path (const string_type& path)
{
}

rx_result tar_file_storage::ensure_path_exsistence (const string_type& path)
{
}

rx_result tar_file_storage::recursive_create_directory (const string_type& path)
{
}

rx_result_with<rx_storage_item_ptr> tar_file_storage::get_item_storage (const meta::meta_data& data)
{
}

string_type tar_file_storage::get_file_path (const meta::meta_data& data, const string_type& root, const string_type& base)
{
}

void tar_file_storage::add_file_path (const meta::meta_data& data, const string_type& path)
{
}


// Class storage::tar::tar_file_item 

tar_file_item::tar_file_item (const string_type& serialization_type, const string_type& file_path)
{
}


tar_file_item::~tar_file_item()
{
}



values::rx_value tar_file_item::get_value () const
{
}

rx_time tar_file_item::get_created_time () const
{
}

size_t tar_file_item::get_size () const
{
}

rx_result tar_file_item::delete_item ()
{
}

string_type tar_file_item::get_file_path () const
{
}

string_type tar_file_item::get_file_storage_info ()
{
}

const string_type& tar_file_item::get_item_reference () const
{
}

bool tar_file_item::preprocess_meta_data (meta::meta_data& data)
{
}


} // namespace tar
} // namespace storage

