

/****************************************************************************
*
*  runtime_internal\rx_variables.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_variables_h
#define rx_variables_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_values
#include "lib/rx_values.h"



namespace rx_internal {

namespace sys_runtime {

namespace variables {
rx_result register_variables_constructors();





class register_variable : public rx_platform::runtime::blocks::variable_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of register variable.");

    DECLARE_REFERENCE_PTR(register_variable);

  public:

      rx_result initialize_variable (runtime::runtime_init_context& ctx);

      rx_result start_variable (runtime::runtime_start_context& ctx);


  protected:

  private:

      rx_value get_variable_input (runtime_process_context* ctx, runtime_sources_type& sources);

      rx_result variable_write (write_data&& data, runtime_process_context* ctx, runtime_sources_type& sources);



      rx::values::rx_timed_value value_;


      local_value<bool> persist_;


};


} // namespace variables
} // namespace sys_runtime
} // namespace rx_internal



#endif
