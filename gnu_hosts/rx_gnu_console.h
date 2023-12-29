

/****************************************************************************
*
*  gnu_hosts\rx_gnu_console.h
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


#ifndef rx_gnu_console_h
#define rx_gnu_console_h 1



// rx_interactive
#include "host/rx_interactive.h"
// rx_gnu_file_sys
#include "gnu_hosts/rx_gnu_file_sys.h"



namespace gnu {






class gnu_console_host : public host::interactive::interactive_console_host  
{

  public:
      gnu_console_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~gnu_console_host();


      bool shutdown (const string_type& msg);

      void get_host_info (string_array& hosts);

      bool is_canceling () const;

      bool break_host (const string_type& msg);

      bool read_stdin (std::array<char,0x100>& chars, size_t& count);

      bool write_stdout (const void* data, size_t size);

      rx_result setup_console (int argc, char* argv[]);

      void restore_console ();

      static string_type get_gnu_interactive_info ();

      bool supports_ansi () const;

      string_type get_full_path (const string_type& path);

      void fill_plugin_libs (string_array& paths);


  protected:

      rx_result fill_host_directories (hosting::rx_host_directories& data);


  private:

      void adjust_terminal_size ();



      termios ttyold_;

      int width_;

      int height_;

      std::atomic_bool win_changed_;


};


} // namespace gnu



#endif
