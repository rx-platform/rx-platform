

/****************************************************************************
*
*  storage\rx_file_storage.h
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


#ifndef rx_file_storage_h
#define rx_file_storage_h 1



// rx_storage
#include "system/storage_base/rx_storage.h"
// rx_file_internals
#include "storage/rx_file_internals.h"



using storage_base::rx_storage_connection;


namespace storage {

namespace files {
string_type get_file_storage_info();


template <bool isString>
struct file_item_cache_holder_t { };

template <> struct file_item_cache_holder_t<true>
{
    string_type value;
};
template <> struct file_item_cache_holder_t<false>
{
    byte_string value;
};





template <class fileT, class streamT, bool isStringBased = streamT::string_based>
class rx_file_item : public rx_platform::storage_base::rx_storage_item  
{
    typedef file_item_cache_holder_t<isStringBased> cache_type;


  public:
      rx_file_item (const string_type& file_path, const meta_data& storage_meta, rx_storage_item_type storage_type = rx_storage_item_type::none);

      ~rx_file_item();


      values::rx_value get_value () const;

      rx_time get_created_time () const;

      size_t get_size () const;

      rx_result delete_item ();

      string_type get_file_path () const;

      const string_type& get_item_reference () const;

      bool preprocess_meta_data (meta_data& data);

      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      rx_result close_read ();

      rx_result commit_write_sync ();

      void commit_write (storage_callback_t callback, runtime_transaction_id_t trans_id);

      string_type get_item_path () const;

      rx_result save (runtime_transaction_id_t trans_id, byte_string data, storage_callback_t callback);


  protected:

  private:

      static rx_result internal_save (byte_string data, const string_type& path);



      bool valid_;

      meta_data storage_meta_;

      streamT item_data_;

      string_type file_path_;

      cache_type item_cache_;


};







class file_system_storage : public rx_platform::storage_base::rx_platform_storage  
{
	DECLARE_REFERENCE_PTR(file_system_storage);
    typedef std::map<string_type, string_type> items_cache_type;

  public:
      file_system_storage();

      ~file_system_storage();


      string_type get_storage_info ();

      sys_handle_t get_host_test_file (const string_type& path);

      sys_handle_t get_host_console_script_file (const string_type& path);

      rx_result init_storage (const string_type& storage_reference, hosting::rx_platform_host* host);

      void deinit_storage ();

      rx_result list_storage (std::vector<rx_storage_item_ptr>& items);

      rx_result list_storage_roles (std::vector<rx_roles_storage_item_ptr>& items);

      string_type get_storage_reference ();

      bool is_valid_storage () const;

      rx_result_with<rx_storage_item_ptr> get_item_storage (const meta_data& data, rx_item_type type);

      rx_result_with<rx_storage_item_ptr> get_runtime_storage (const meta_data& data, rx_item_type type);

      void preprocess_meta_data (meta_data& data);

      runtime_transaction_id_t get_new_unique_ids (size_t count);

      void set_next_unique_id (runtime_transaction_id_t id);


  protected:

  private:

      rx_result recursive_list_storage (const string_type& path, const string_type& file_path, std::vector<rx_storage_item_ptr>& items, bool rt);

      rx_storage_item_ptr get_storage_item_from_file_path (const string_type& path, const meta_data& storage_meta);

      rx_result ensure_path_exsistence (const string_type& path);

      rx_result recursive_create_directory (const string_type& path);

      string_type get_file_path (const meta_data& data, const string_type& root, const string_type& base, rx_item_type type);

      void add_file_path (const meta_data& data, const string_type& path);

      string_type get_runtime_file_path (const meta_data& data, const string_type& root, const string_type& base, rx_item_type type);



      string_type root_;

      locks::slim_lock cache_lock_;

      items_cache_type items_cache_;

      items_cache_type runtime_cache_;


};







class file_system_storage_connection : public rx_platform::storage_base::rx_storage_connection  
{

  public:
      ~file_system_storage_connection();


      string_type get_storage_info () const;

      rx_result init_connection (const string_type& storage_reference, hosting::rx_platform_host* host);

      string_type get_storage_reference () const;


  protected:

      rx_result_with<rx_storage_ptr> get_and_init_storage (const string_type& name, hosting::rx_platform_host* host);


  private:


      string_type root_path_;


};







class file_system_storage_type : public rx_platform::storage_base::rx_platform_storage_type  
{

  public:

      string_type get_storage_info ();

      rx_storage_connection::smart_ptr construct_storage_connection ();

      string_type get_reference_prefix () const;


  protected:

  private:


};







class rx_roles_file : public rx_platform::storage_base::rx_roles_storage_item  
{

  public:
      rx_roles_file (const string_type& file_path);

      ~rx_roles_file();


      rx_result delete_item ();

      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      rx_result close_read ();

      rx_result commit_write ();


  protected:

  private:


      rx_json_file item_data_;

      string_type file_path_;


};


} // namespace files
} // namespace storage



#endif
