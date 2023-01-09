

/****************************************************************************
*
*  system\runtime\rx_scan_algorithms.h
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


#ifndef rx_scan_algorithms_h
#define rx_scan_algorithms_h 1



// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"



namespace rx_platform {

namespace runtime {

namespace algorithms {






template <class typeT>
class runtime_scan_algorithms 
{
    static constexpr bool C_has_remote_updates  = true;
    static constexpr bool C_has_status_response = true;

    static constexpr bool C_has_source_results  = true;
    static constexpr bool C_has_source_updates  = true;
    static constexpr bool C_has_mapper_writes   = true;
    static constexpr bool C_has_tag_writes      = true;

    static constexpr bool C_has_variables       = true;
    static constexpr bool C_has_programs        = true;
    static constexpr bool C_has_events          = true;
    static constexpr bool C_has_filters         = true;
    static constexpr bool C_has_structs         = true;
    static constexpr bool C_has_own             = true;

    static constexpr bool C_has_tag_updates     = true;
    static constexpr bool C_has_mapper_updates  = true;
    static constexpr bool C_has_source_writes   = true;


  public:

      static void process_runtime (typename typeT::RType& whose);


  protected:

  private:

      static void check_context (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_from_remotes (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_status_change (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_source_inputs (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_mapper_inputs (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_subscription_inputs (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_variables (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_programs (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_events (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_filters (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_structs (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_own (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_subscription_outputs (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_mapper_outputs (typename typeT::RType& whose, runtime_process_context& ctx);

      static void process_source_outputs (typename typeT::RType& whose, runtime_process_context& ctx);



      static void process_debug_scan (typename typeT::RType& whose);


};


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform



#endif
