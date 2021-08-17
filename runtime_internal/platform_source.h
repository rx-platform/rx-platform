

/****************************************************************************
*
*  runtime_internal\platform_source.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef platform_source_h
#define platform_source_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_value_point
#include "runtime_internal/rx_value_point.h"

#include "system/runtime/rx_value_templates.h"


namespace rx_internal {

namespace sys_runtime {

namespace data_source {





class platform_source : public rx_platform::runtime::blocks::source_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Platform Source. Source implementation for platform connection, may include different source types.");

    DECLARE_REFERENCE_PTR(platform_source);

  public:
      platform_source();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      rx_result start_source (runtime::runtime_start_context& ctx);

      rx_result stop_source (runtime::runtime_stop_context& ctx);


  protected:

  private:

      rx_result connect (const string_type& path);

      void disconnect ();

      rx_result source_write (write_data&& data, runtime_process_context* ctx);



      value_point point_;


      runtime::local_value<string_type> path_;


};


} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal



#endif
