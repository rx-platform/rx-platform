

/****************************************************************************
*
*  runtime_internal\rx_events.h
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


#ifndef rx_events_h
#define rx_events_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"

#include "system/runtime/rx_value_templates.h"
#include "model/rx_meta_internals.h"
using namespace rx_platform;


namespace rx_internal {

namespace sys_runtime {

namespace events {

rx_result register_event_constructors();





class object_chaged_event : public rx_platform::runtime::blocks::event_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of object changed system event.");

    DECLARE_REFERENCE_PTR(object_chaged_event);

  public:
      object_chaged_event();

      ~object_chaged_event();


      rx_result initialize_event_internal (runtime::runtime_init_context& ctx);

      rx_result initialize_event (runtime::runtime_init_context& ctx);

      rx_result start_event (runtime::runtime_start_context& ctx);

      rx_result stop_event (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_event (runtime::runtime_deinit_context& ctx);


      static std::map<rx_node_id, object_chaged_event::smart_ptr> runtime_instances;


  protected:

  private:


      runtime::local_value<bool> enabled_;


};


} // namespace events
} // namespace sys_runtime
} // namespace rx_internal



#endif
