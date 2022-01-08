

/****************************************************************************
*
*  win32_hosts\rx_win32_file_sys.h
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


#ifndef rx_win32_file_sys_h
#define rx_win32_file_sys_h 1



// rx_file_storage
#include "storage/rx_file_storage.h"



namespace win32 {





class win32_file_system_storage : public storage::files::file_system_storage  
{
    DECLARE_REFERENCE_PTR(win32_file_system_storage);

  public:

  protected:

  private:


};


} // namespace win32



#endif
