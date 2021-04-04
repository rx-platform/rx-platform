

/****************************************************************************
*
*  win32_hosts\rx_win32_interactive.h
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


#ifndef rx_win32_interactive_h
#define rx_win32_interactive_h 1



// rx_interactive
#include "host/rx_interactive.h"

using rx_platform::hosting::rx_host_directories;


namespace win32 {





class win32_console_host : public host::interactive::interactive_console_host  
{
	typedef std::tuple<double> win32_console_types;

  public:
      win32_console_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~win32_console_host();


      bool shutdown (const string_type& msg);

      void get_host_info (string_array& hosts);

      bool is_canceling () const;

      bool break_host (const string_type& msg);

      bool read_stdin (std::array<char,0x100>& chars, size_t& count);

      bool write_stdout (const void* data, size_t size);

      std::vector<ETH_interface> get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      std::vector<IP_interface> get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      rx_result setup_console (int argc, char* argv[]);

      string_type get_default_name () const;

      static string_type get_win32_interactive_info ();

      bool supports_ansi () const;

      void add_command_line_options (hosting::command_line_options_t& options, rx_platform::configuration_data_t& config);

      string_type get_full_path (const string_type& path);

	  template<class tupleType>
	  auto register_types(tupleType before)
	  {
		  return std::tuple_cat(before, win32_console_types());
	  }
  protected:

      rx_result fill_host_directories (rx_host_directories& data);


  private:


      HANDLE out_handle_;

      HANDLE in_handle_;

      bool supports_ansi_;

      WORD default_attribute_;

      bool no_ansi_;

      bool use_ansi_;


};


} // namespace win32



#endif
