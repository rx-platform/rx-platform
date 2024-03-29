

/****************************************************************************
*
*  gnu_hosts\rx_gnu_headless.h
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


#ifndef rx_gnu_headless_h
#define rx_gnu_headless_h 1



// rx_headless_host
#include "host/rx_headless_host.h"



namespace gnu {





class gnu_headless_host : public host::headless::headless_platform_host  
{

  public:
      gnu_headless_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~gnu_headless_host();


      void get_host_info (string_array& hosts);

      string_type get_full_path (const string_type& path);

      bool supports_ansi () const;

      void fill_plugin_libs (string_array& paths);


  protected:

      static string_type get_gnu_headless_info ();

      rx_result fill_host_directories (hosting::rx_host_directories& data);


  private:


};


} // namespace gnu



#endif
