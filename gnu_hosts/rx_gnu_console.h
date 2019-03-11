

/****************************************************************************
*
*  gnu_hosts\rx_gnu_console.h
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


#ifndef rx_gnu_console_h
#define rx_gnu_console_h 1



// rx_gnu_file_sys
#include "gnu_hosts/rx_gnu_file_sys.h"
// rx_interactive
#include "host/rx_interactive.h"



namespace gnu {






class gnu_console_host : public host::interactive::interactive_console_host  
{

  public:
      gnu_console_host (rx_platform::hosting::rx_platform_storage::smart_ptr storage);

      ~gnu_console_host();


      bool shutdown (const string_type& msg);

      bool start (rx_platform::configuration_data_t& config);

      void get_host_info (string_array& hosts);

      bool is_canceling () const;

      bool break_host (const string_type& msg);

      bool read_stdin (std::array<char,0x100>& chars, size_t& count);

      bool write_stdout (const void* data, size_t size);

      std::vector<ETH_interface> get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      std::vector<IP_interface> get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      rx_result setup_console (int argc, char* argv[]);

      rx_result restore_console ();


  protected:

  private:


      termios ttyold_;


};


} // namespace gnu



#endif
