

/****************************************************************************
*
*  runtime_internal\rx_simulation.h
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


#ifndef rx_simulation_h
#define rx_simulation_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"

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


  protected:

  private:

      rx_result source_write (write_data&& data, runtime_process_context* ctx);



};






class periodic_source : public rx_platform::runtime::blocks::source_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Periodic Function Source. Base source implementation for periodic functions.");

    DECLARE_REFERENCE_PTR(periodic_source);

  public:
      periodic_source();


      rx_result start_source (runtime::runtime_start_context& ctx);

      rx_result initialize_source (runtime::runtime_init_context& ctx);

      rx_result stop_source (runtime::runtime_stop_context& ctx);

      virtual void source_tick (rx_time now) = 0;


  protected:

  private:

      rx_result source_write (write_data&& data, runtime_process_context* ctx);



      rx_timer_ptr timer_;

      runtime::local_value<uint32_t> period_;


};






class ramp_source : public periodic_source  
{

  public:
      ramp_source();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      rx_result start_source (runtime::runtime_start_context& ctx);

      void source_tick (rx_time now);


  protected:

  private:


      runtime::local_value<double> amplitude_;

      runtime::local_value<double> increment_;

      double current_value_;


};


} // namespace simulation
} // namespace sys_runtime
} // namespace rx_internal



#endif
