

/****************************************************************************
*
*  storage\rx_file_storage.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef rx_file_storage_h
#define rx_file_storage_h 1



// rx_storage
#include "system/storage_base/rx_storage.h"



namespace storage {

namespace files {





class rx_file_item : public rx_platform::storage_base::rx_storage_item  
{

  public:
      rx_file_item (const string_type& serialization_type, const string_type& file_path, const meta::meta_data& storage_meta);

      ~rx_file_item();


      values::rx_value get_value () const;

      rx_time get_created_time () const;

      size_t get_size () const;

      rx_result delete_item ();

      string_type get_file_path () const;

      static string_type get_file_storage_info ();

      const string_type& get_item_reference () const;

      bool preprocess_meta_data (meta::meta_data& data);


  protected:

      string_type file_path_;


  private:


      rx_time created_time_;

      bool valid_;

      meta::meta_data storage_meta_;


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

      const string_type& get_license ();

      rx_result init_storage (const string_type& storage_reference, hosting::rx_platform_host* host);

      void deinit_storage ();

      rx_result list_storage (std::vector<rx_storage_item_ptr>& items);

      string_type get_storage_reference ();

      bool is_valid_storage () const;

      rx_result_with<rx_storage_item_ptr> get_item_storage (const meta::meta_data& data);


  protected:

  private:

      rx_result recursive_list_storage (const string_type& path, const string_type& file_path, std::vector<rx_storage_item_ptr>& items);

      std::unique_ptr<rx_file_item> get_storage_item_from_file_path (const string_type& path, const meta::meta_data& storage_meta);

      rx_result ensure_path_exsistence (const string_type& path);

      rx_result recursive_create_directory (const string_type& path);

      string_type get_file_path (const meta::meta_data& data, const string_type& root, const string_type& base);

      void add_file_path (const meta::meta_data& data, const string_type& path);



      string_type root_;

      locks::slim_lock cache_lock_;

      items_cache_type items_cache_;


};






class rx_json_file : public rx_file_item  
{

  public:
      rx_json_file (const string_type& file_path, const meta::meta_data& storage_meta);

      ~rx_json_file();


      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      void close ();


  protected:

  private:


      std::unique_ptr<rx_platform::serialization::json_reader> reader_;

      std::unique_ptr<rx_platform::serialization::json_writer> writer_;


};






class rx_binary_file : public rx_file_item  
{

  public:
      rx_binary_file (const string_type& file_path, const meta::meta_data& storage_meta);

      ~rx_binary_file();


      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      void close ();


  protected:

  private:


      std::unique_ptr<rx_platform::serialization::std_buffer_reader> reader_;

      std::unique_ptr<rx_platform::serialization::std_buffer_writer> writer_;


};







class file_system_storage_holder : public rx_platform::storage_base::rx_platform_storage_holder  
{

  public:

      string_type get_storage_info ();

      rx_result init_storage (const string_type& storage_reference, hosting::rx_platform_host* host);

      string_type get_storage_reference ();


  protected:

      rx_result_with<rx_storage_ptr> get_and_init_storage (const string_type& name, hosting::rx_platform_host* host);


  private:


      string_type root_path_;


};


} // namespace files
} // namespace storage



#endif
