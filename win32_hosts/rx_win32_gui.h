

/****************************************************************************
*
*  win32_hosts\rx_win32_gui.h
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


#ifndef rx_win32_gui_h
#define rx_win32_gui_h 1



// rx_gui_host
#include "host/rx_gui_host.h"



namespace win32 {





class win32_gui_host : public host::gui::gui_platform_host  
{

  public:
      win32_gui_host (hosting::rx_host_storages& storage);

      ~win32_gui_host();


      string_type get_config_path () const;

      string_type get_default_name () const;

      void get_host_info (string_array& hosts);


  protected:

      static string_type get_win32_gui_info ();

      string_type get_default_manual_path () const;


  private:


};


} // namespace win32



#endif
