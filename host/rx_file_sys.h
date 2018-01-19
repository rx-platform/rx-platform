

/****************************************************************************
*
*  host\rx_file_sys.h
*
*  Copyright (c) 2018 Dusan Ciric
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

      virtual ~file_system_storage();


      void get_storage_info (string_type& info);

      sys_handle_t get_host_test_file (const string_type& path);

      sys_handle_t get_host_console_script_file (const string_type& path);

      const string_type& get_license ();

      void init_storage ();

      void deinit_storage ();


  protected:

      virtual string_type get_root_folder () = 0;


  private:


      string_type root_;


};


} // namespace files
} // namespace host



#endif
