

/****************************************************************************
*
*  runtime_internal\rx_filters.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef rx_filters_h
#define rx_filters_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"



namespace rx_internal {

namespace sys_runtime {

namespace filters {
rx_result register_filter_constructors();





class linear_scaling_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of linear scaling for input and output.");

    DECLARE_REFERENCE_PTR(linear_scaling_filter);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);



      runtime_handle_t hi_raw_;

      runtime_handle_t low_raw_;

      runtime_handle_t hi_eu_;

      runtime_handle_t low_eu_;


};






class bound_filter : public rx_platform::runtime::blocks::filter_runtime  
{

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);



      runtime_handle_t hi_bound_;

      runtime_handle_t low_bound_;


};


} // namespace filters
} // namespace sys_runtime
} // namespace rx_internal



#endif
