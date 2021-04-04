

/****************************************************************************
*
*  win32_hosts\rx_win32_pipe.h
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


#ifndef rx_win32_pipe_h
#define rx_win32_pipe_h 1



// rx_pipe
#include "host/rx_pipe.h"



namespace win32 {





class win32_pipe_host : public host::pipe::rx_pipe_host  
{

  public:
      win32_pipe_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~win32_pipe_host();


      string_type get_default_name () const;

      void get_host_info (string_array& hosts);

      static string_type get_win32_pipe_info ();

      string_type get_full_path (const string_type& path);

      bool supports_ansi () const;

      rx_result setup_console (int argc, char* argv[]);


  protected:

      rx_result fill_host_directories (hosting::rx_host_directories& data);


  private:

      void get_stdio_handles (sys_handle_t& in, sys_handle_t& out, sys_handle_t& err);



      bool supports_ansi_;


};


} // namespace win32



#endif
