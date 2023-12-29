

/****************************************************************************
*
*  first_plugin\first_logic.h
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


#ifndef first_logic_h
#define first_logic_h 1



// rx_logic_types
#include "rx_logic_types.h"

using namespace rx_platform_api;






class first_method : public rx_platform_api::rx_method  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Method. Method implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_method);

  public:
      first_method();

      ~first_method();


      rx_result initialize_method (rx_init_context& ctx);

      rx_result start_method (rx_start_context& ctx);

      rx_result stop_method ();

      rx_result deinitialize_method ();

      rx_result method_execute (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx);


  protected:

  private:


};




#endif
