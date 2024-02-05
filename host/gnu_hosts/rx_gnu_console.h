

/****************************************************************************
*
*  host\gnu_hosts\rx_gnu_console.h
*
*  Copyright (c) 2017 Dusan Ciric
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



// rx_interactive
#include "host/rx_interactive.h"



namespace host {

namespace gnu {





class gnu_console_host : public interactive::interactive_console_host  
{

  public:
      gnu_console_host();

      virtual ~gnu_console_host();


      bool shutdown (const string_type& msg);

      sys_handle_t get_host_test_file (const string_type& path);

      bool start (const string_array& args);

      sys_handle_t get_host_console_script_file (const string_type& path);

      void get_host_info (string_array& hosts);


  protected:

  private:


};


} // namespace gnu
} // namespace host



#endif
