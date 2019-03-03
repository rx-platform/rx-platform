

/****************************************************************************
*
*  host\rx_file_sys.h
*
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_file_sys_h
#define rx_file_sys_h 1



// rx_host
#include "system/hosting/rx_host.h"



namespace host {

namespace files {





class file_system_storage : public rx_platform::hosting::rx_platform_storage  
{

  public:
      file_system_storage();

      ~file_system_storage();


      void get_storage_info (string_type& info);

      sys_handle_t get_host_test_file (const string_type& path);

      sys_handle_t get_host_console_script_file (const string_type& path);

      const string_type& get_license ();

      void init_storage ();

      void deinit_storage ();

      void list_storage (const string_type& path, platform_directories_type& sub_directories, platform_items_type& sub_items, const string_type& pattern);


  protected:

      virtual string_type get_root_folder () = 0;


  private:


      string_type root_;


};






class file_system_file : public rx_platform::hosting::rx_platform_file  
{
	DECLARE_REFERENCE_PTR(file_system_file);

  public:
      file_system_file (const string_type& path, const string_type& name);

      ~file_system_file();


      values::rx_value get_value () const;

      rx_time get_created_time () const;

      string_type get_name () const;

      size_t get_size () const;


  protected:

  private:


      sys_handle_t handle_;

      rx_time created_time_;

      string_type path_;

      string_type name_;

      bool valid_;


};


} // namespace files
} // namespace host



#endif
