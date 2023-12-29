

/****************************************************************************
*
*  win32_hosts\rx_win32_service.h
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


#ifndef rx_win32_service_h
#define rx_win32_service_h 1



// rx_win32_headless
#include "win32_hosts/rx_win32_headless.h"
// rx_lock
#include "lib/rx_lock.h"


#define SERVICE_DESC "{rx-platform} Win32 Service"



namespace win32 {

class win32_service_host;
namespace
{
win32_service_host* g_host = nullptr;
}




class win32_service_host : public win32_headless_host  
{

  public:
      win32_service_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~win32_service_host();


      void get_host_info (string_array& hosts);

      rx_result start_service (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins);

      rx_result stop_service ();

      static rx_result install_service ();

      static rx_result uninstall_service ();

      static win32_service_host& instance ();

      string_type get_host_name ();

      string_type get_default_user_storage () const;


  protected:

      static string_type get_win32_service_info ();


  private:

      static VOID WINAPI ServiceCtrlHandler (DWORD ctrlCode, DWORD eventType, LPVOID data, LPVOID reserved);

      static VOID WINAPI ServiceMain (DWORD argc, LPSTR* argv);



      static rx::locks::event finished_;

      static rx::locks::event stop_;


      bool supports_ansi_;

      bool is_service_;

      string_type service_name_;

      std::vector<library::rx_plugin_base*> plugins_;


};


} // namespace win32



#endif
