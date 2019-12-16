

/****************************************************************************
*
*  system\server\rx_cmds.h
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


#ifndef rx_cmds_h
#define rx_cmds_h 1


#include "lib/security/rx_security.h"


#include "api/rx_platform_api.h"

using namespace rx;
using namespace rx_platform::ns;


namespace rx_platform {

namespace prog {

struct script_def_t
{
	string_type name;
	string_type user_name;
	string_type version;
};




class server_script_host 
{

  public:
      server_script_host (const script_def_t& definition);

      ~server_script_host();


      virtual void deinitialize () = 0;

      virtual bool initialize () = 0;

      virtual bool dump_script_information (std::ostream& out) = 0;

      virtual void do_testing_stuff () = 0;

      virtual bool init_thread () = 0;

      virtual bool deinit_thread () = 0;


      const script_def_t& get_definition () const
      {
        return definition_;
      }



  protected:

  private:


      script_def_t definition_;


};


} // namespace prog
} // namespace rx_platform



#endif

