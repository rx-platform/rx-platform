

/****************************************************************************
*
*  platform_api\rx_logic_types.h
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


#ifndef rx_logic_types_h
#define rx_logic_types_h 1



// rx_runtime
#include "platform_api/rx_runtime.h"



namespace rx_platform_api {





class rx_method : public rx_runtime  
{

  public:
      rx_method();

      ~rx_method();


      virtual rx_result initialize_port (rx_init_context& ctx);

      virtual rx_result start_port (rx_start_context& ctx);

      virtual rx_result stop_port ();

      virtual rx_result deinitialize_port ();


  protected:

  private:


      plugin_method_runtime_struct impl_;


};






class rx_program : public rx_runtime  
{

  public:
      rx_program();

      ~rx_program();


      virtual rx_result initialize_port (rx_init_context& ctx);

      virtual rx_result start_port (rx_start_context& ctx);

      virtual rx_result stop_port ();

      virtual rx_result deinitialize_port ();


  protected:

  private:


      plugin_program_runtime_struct impl_;


};


} // namespace rx_platform_api



#endif
