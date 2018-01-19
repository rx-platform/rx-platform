

/****************************************************************************
*
*  gnu_hosts\rx_gnu_file_sys.h
*
*  Copyright (c) 2018 Dusan Ciric
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


#ifndef rx_gnu_file_sys_h
#define rx_gnu_file_sys_h 1



// rx_file_sys
#include "host/rx_file_sys.h"



namespace gnu {





class gnu_file_system_storage : public host::files::file_system_storage  
{

  public:
      gnu_file_system_storage();

      virtual ~gnu_file_system_storage();


  protected:

      string_type get_root_folder ();


  private:


};


} // namespace gnu



#endif
