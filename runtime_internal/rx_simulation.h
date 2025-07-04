

/****************************************************************************
*
*  runtime_internal\rx_simulation.h
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


#ifndef rx_simulation_h
#define rx_simulation_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"

#include "system/runtime/rx_runtime_helpers.h"
using namespace rx_platform::runtime;


namespace rx_internal {

namespace sys_runtime {

namespace simulation {

rx_result register_simulation_constructors();




class local_register_source : public rx_platform::runtime::blocks::source_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Local Register Source. Source implementation for register.");

    DECLARE_REFERENCE_PTR(local_register_source);

  public:
      local_register_source();


      rx_result start_source (runtime::runtime_start_context& ctx);

      rx_result initialize_source (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result source_write (write_data&& data, runtime_process_context* ctx);



      bool persistent_;

      rx_simple_value current_value_;


};


} // namespace simulation
} // namespace sys_runtime
} // namespace rx_internal



#endif
