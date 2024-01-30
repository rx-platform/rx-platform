

/****************************************************************************
*
*  discovery\rx_discovery_algorithm.h
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


#ifndef rx_discovery_algorithm_h
#define rx_discovery_algorithm_h 1


#include "system/runtime/rx_runtime_helpers.h"
#include "system/hosting/rx_host.h"
using namespace rx_platform;

// rx_discovery_items
#include "discovery/rx_discovery_items.h"

namespace rx_internal {
namespace discovery {
class peer_connection;
class discovery_register;

} // namespace discovery
} // namespace rx_internal




namespace rx_internal {

namespace discovery {
struct discovered_peer_data;






class peer_directory_algorithm 
{

  public:
      peer_directory_algorithm();

      ~peer_directory_algorithm();


      rx_result start_algorithm (runtime::runtime_start_context& ctx, rx_object_impl_ptr obj, peer_connection_ptr whose);

      rx_result stop_algorithm (runtime::runtime_stop_context& ctx, peer_connection_ptr whose);

      void client_connected (peer_connection_ptr whose);

      void client_disconnected (peer_connection_ptr whose);

      void item_changed (const rx_node_id& id, const string_type& path, peer_connection_ptr whose);


  protected:

  private:

      void do_browse (peer_connection_ptr whose, const string_type& path);



};






class peer_refresh_algorithm 
{

  public:
      peer_refresh_algorithm();

      ~peer_refresh_algorithm();


      rx_result start_algorithm (runtime::runtime_start_context& ctx, rx_object_impl_ptr obj, peer_connection_ptr whose);

      rx_result stop_algorithm (runtime::runtime_stop_context& ctx, peer_connection_ptr whose);

      void client_connected (peer_connection_ptr whose);

      void client_disconnected (peer_connection_ptr whose);

      void item_changed (const rx_node_id& id, const string_type& path, peer_connection_ptr whose);


  protected:

  private:


};







class peer_discovery_algorithms 
{

  public:

      static rx_result start_register (discovery_register& who, hosting::rx_platform_host* host, const configuration_data_t& config);

      static void stop_register (discovery_register& who);

      static bool create_dafult_query (discovery_register& who, base_meta_writer& writer);

      static bool create_fallback_register (discovery_register& who, base_meta_writer& writer);

      static bool parse_new_master_when_active (discovery_register& who, base_meta_writer& writer, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version);

      static bool parse_new_fallback_when_active (discovery_register& who, base_meta_writer& writer, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version, bool& broadcast);

      static bool parse_master_list_when_active (discovery_register& who, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version);

      static bool parse_master_list_when_inactive (discovery_register& who, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version);

      static void recreate_own_data (discovery_register& who);

      static bool create_master_list_mine (discovery_register& who, base_meta_writer& writer, uint32_t version);

      static bool create_master_list_full (discovery_register& who, base_meta_writer& writer, uint32_t version);

      static bool do_time_checks_master (discovery_register& who, rx_timer_ticks_t ticks, rx_timer_ticks_t timeout);

      static bool do_time_checks_follower (discovery_register& who, rx_timer_ticks_t ticks, rx_timer_ticks_t timeout);

      static std::vector<discovery::discovered_peer_data> get_peers_network (discovery_register& who);

      static bool create_unregister (discovery_register& who, base_meta_writer& writer, uint32_t version);


  protected:

  private:


};


} // namespace discovery
} // namespace rx_internal



#endif
