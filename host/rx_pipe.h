

/****************************************************************************
*
*  host\rx_pipe.h
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_pipe_h
#define rx_pipe_h 1



// rx_host
#include "system/hosting/rx_host.h"



namespace host {

namespace pipe {





class rx_pipe_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      rx_pipe_host (hosting::rx_host_storages& storage);

      ~rx_pipe_host();


      void get_host_info (string_array& hosts);

      void server_started_event ();

      bool shutdown (const string_type& msg);

      bool exit () const;

      void get_host_objects (std::vector<rx_platform::runtime::object_runtime_ptr>& items);

      void get_host_types (std::vector<rx_platform::meta::object_type_ptr>& items);

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);

      bool break_host (const string_type& msg);

      int pipe_main (int argc, char* argv[]);

      static string_type get_pipe_info ();


  protected:

  private:


      bool exit_;


};


} // namespace pipe
} // namespace host



#endif
