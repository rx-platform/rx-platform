

/****************************************************************************
*
*  interfaces\rx_port_stack_data.h
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


#ifndef rx_port_stack_data_h
#define rx_port_stack_data_h 1


#include "lib/rx_io_addr.h"
#include "system/runtime/rx_runtime_helpers.h"
#include "system/runtime/rx_value_templates.h"
using namespace rx_platform;
using namespace rx_platform::runtime;

// rx_io_buffers
#include "lib/rx_io_buffers.h"

#include "api/rx_platform_api.h"
using io::rx_io_buffer;


namespace rx_internal {

namespace interfaces {

namespace port_stack {





class port_active_map 
{
    typedef std::map<rx_protocol_stack_endpoint*, rx_port_ptr> endpoints_map_type;

  public:
      port_active_map();


      rx_result register_endpoint (rx_protocol_stack_endpoint* what, rx_port_ptr whose, rx_port_ptr owner);

      rx_result unregister_endpoint (rx_protocol_stack_endpoint* what, rx_port_ptr owner);

      void close_all_endpoints ();


      async_owned_value<int16_t> active_endpoints;


  protected:

  private:


      endpoints_map_type endpoints_map_;

      locks::slim_lock map_lock_;


};






class port_buffers 
{
    typedef std::vector<rx_io_buffer> free_buffers_type;

  public:
      port_buffers();


      static rx_result_with<io::rx_io_buffer> alloc_io_buffer (rx_port_ptr& whose);

      static void release_io_buffer (rx_port_ptr& whose, io::rx_io_buffer buff);


      free_buffers_type free_buffers;


      size_t buffer_back_capacity;

      size_t buffer_front_capacity;

      size_t buffer_discard_size;

      async_owned_value<uint32_t> buffer_count;

      async_owned_value<int64_t> discard_buffer_count;


  protected:

  private:


      locks::slim_lock buffers_lock_;


};






class port_build_map 
{
    typedef std::set<rx_port_ptr> registered_type;

  public:
      port_build_map();


      rx_result register_port (rx_port_ptr who, rx_port_ptr owner);

      rx_result unregister_port (rx_port_ptr who, rx_port_ptr owner);

      std::vector<rx_port_ptr> get_registered ();


      rx_port_ptr stack_top;

      owned_value<bool> stack_ready;


  protected:

  private:

      registered_type registered_;



};






class port_passive_map 
{
    typedef std::pair<io::any_address, io::any_address> addr_pair_t;    
    typedef std::map<addr_pair_t, rx_port_ptr> passive_map_type;
    typedef std::map<rx_port_ptr, addr_pair_t> inverse_passive_map_type;

  public:
      port_passive_map();


      rx_result register_passive (rx_port_ptr who, io::any_address& local_addr, io::any_address& remote_addr, rx_port_ptr owner);

      rx_result unregister_passive (rx_port_ptr who, rx_port_ptr owner);

      rx_port_ptr get_binded_port (const io::any_address& local_addr, const io::any_address& remote_addr);

      std::vector<rx_port_ptr> get_binded ();

      bool empty () const;


      async_owned_value<bool> stack_binded;

      rx_port_ptr bind_port;


  protected:

  private:


      passive_map_type passive_map_;

      inverse_passive_map_type inverse_passive_map_;

      locks::slim_lock map_lock_;


};






class port_stack_data 
{
    typedef std::map<rx_node_id, api::query_result_detail> connected_items_type;

  public:
      ~port_stack_data();


      rx_result init_runtime_data (runtime::runtime_init_context& ctx);


      port_passive_map passive_map;

      port_active_map active_map;

      port_build_map build_map;

      port_buffers buffers;


      connected_items_type connected_items;


  protected:

  private:


};


} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal



#endif
