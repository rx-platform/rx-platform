

/****************************************************************************
*
*  interfaces\rx_io_relations.h
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


#ifndef rx_io_relations_h
#define rx_io_relations_h 1



// rx_relation_impl
#include "system/runtime/rx_relation_impl.h"

#include "system/runtime/rx_runtime_helpers.h"
using namespace rx_platform;


namespace rx_internal {

namespace interfaces {

namespace io_endpoints {





class port_stack_relation : public rx_platform::runtime::relations::relation_runtime  
{
    DECLARE_REFERENCE_PTR(port_stack_relation);

  public:
      port_stack_relation();


      void process_stack ();

      rx_result initialize_relation (runtime::runtime_init_context& ctx);

      rx_result deinitialize_relation (runtime::runtime_deinit_context& ctx);

      rx_result start_relation (runtime::runtime_start_context& ctx, bool is_target);

      rx_result stop_relation (runtime::runtime_stop_context& ctx, bool is_target);


  protected:

  private:

      rx_result_with<platform_item_ptr> resolve_runtime_sync (const rx_node_id& id);

      void relation_connected ();

      void relation_disconnected ();



      rx_port_ptr from_;

      rx_port_ptr to_;


};






class port_reference_relation : public rx_platform::runtime::relations::relation_runtime  
{
    DECLARE_REFERENCE_PTR(port_reference_relation);

  public:
      port_reference_relation();


      rx_result initialize_relation (runtime::runtime_init_context& ctx);

      rx_result deinitialize_relation (runtime::runtime_deinit_context& ctx);

      rx_result start_relation (runtime::runtime_start_context& ctx, bool is_target);

      rx_result stop_relation (runtime::runtime_stop_context& ctx, bool is_target);


  protected:

  private:

      rx_result_with<platform_item_ptr> resolve_runtime_sync (const rx_node_id& id);

      void relation_connected ();

      void relation_disconnected ();



      rx_port_ptr to_;

      meta_data meta_from_;

      rx_item_type from_type_;


};


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
