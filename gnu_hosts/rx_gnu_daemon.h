

/****************************************************************************
*
*  gnu_hosts\rx_gnu_daemon.h
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


#ifndef rx_gnu_daemon_h
#define rx_gnu_daemon_h 1



// rx_gnu_headless
#include "gnu_hosts/rx_gnu_headless.h"
// rx_lock
#include "lib/rx_lock.h"

#include "gnu_hosts/rx_gnu_file_sys.h"


namespace gnu {





class gnu_daemon_host : public gnu_headless_host  
{

  public:
      gnu_daemon_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~gnu_daemon_host();


      int daemon_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins);

      void get_host_info (string_array& hosts);

      string_type get_host_name ();

      bool shutdown (const string_type& msg);

      string_type get_default_user_storage () const;


  protected:

      static string_type get_gnu_daemon_host_info ();

      void add_command_line_options (hosting::command_line_options_t& options, rx_platform::configuration_data_t& config);


  private:

      int headless_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins);



      rx::locks::event stop_;


      bool is_daemon_;


};


} // namespace gnu



#endif
