

/****************************************************************************
*
*  gnu_hosts\rx_gnu_pipe.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_gnu_pipe_h
#define rx_gnu_pipe_h 1



// rx_pipe
#include "host/rx_pipe.h"
// rx_gnu_file_sys
#include "gnu_hosts/rx_gnu_file_sys.h"



namespace gnu {






class gnu_pipe_host : public host::pipe::rx_pipe_host  
{

  public:
      gnu_pipe_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~gnu_pipe_host();


      string_type get_default_name () const;

      void get_host_info (string_array& hosts);

      static string_type get_gnu_pipe_info ();

      string_type get_full_path (const string_type& path);

      bool supports_ansi () const;


  protected:

      rx_result fill_host_directories (hosting::rx_host_directories& data);


  private:

      void get_stdio_handles (sys_handle_t& in, sys_handle_t& out, sys_handle_t& err);



};


} // namespace gnu



#endif
