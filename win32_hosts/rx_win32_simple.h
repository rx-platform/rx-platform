

/****************************************************************************
*
*  win32_hosts\rx_win32_simple.h
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


#ifndef rx_win32_simple_h
#define rx_win32_simple_h 1



// rx_simple_host
#include "host/rx_simple_host.h"
// rx_win32_file_sys
#include "win32_hosts/rx_win32_file_sys.h"



namespace win32 {






class win32_simple_host : public host::simple::simple_platform_host  
{

  public:
      win32_simple_host (hosting::rx_host_storages& storage);

      ~win32_simple_host();


      string_type get_default_name () const;

      void get_host_info (string_array& hosts);

      string_type get_full_path (const string_type& path);

      bool supports_ansi () const;

      rx_result setup_console (int argc, char* argv[]);


  protected:

      static string_type get_win32_simple_info ();

      rx_result fill_host_directories (rx_host_directories& data);


  private:


      bool supports_ansi_;


};


} // namespace win32



#endif
