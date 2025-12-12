

/****************************************************************************
*
*  nodejs\rx_nodejs.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef rx_nodejs_h
#define rx_nodejs_h 1



// rx_api
#include "rx_api.h"



namespace nodejs {





class nodejs_plugin : public rx_platform_api::rx_platform_plugin  
{

  public:

      rx_result init_plugin ();

      rx_result deinit_plugin ();

      rx_result build_plugin ();


  protected:

  private:


};


} // namespace nodejs



#endif
