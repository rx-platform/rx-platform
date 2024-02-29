

/****************************************************************************
*
*  discovery\rx_discovery_connections.h
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


#ifndef rx_discovery_connections_h
#define rx_discovery_connections_h 1



// rx_discovery_main
#include "discovery/rx_discovery_main.h"



namespace rx_internal {

namespace discovery {





class peer_connections_manager 
{
    struct peer_connection_data
    {
        string_type instance;
        string_type node;
        peer_connection_ptr connection;
    };
    typedef std::map<rx_uuid, peer_connection_data> connections_type;
    typedef std::map<string_type, peer_connection_ptr> connections_cahce_type;

  public:
      peer_connections_manager();

      ~peer_connections_manager();


      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& config, const std::vector<peer_endpoint>& peers);

      rx_result start (hosting::rx_platform_host* host, const configuration_data_t& config);

      void stop ();

      void deinitialize ();

      void peer_registered (const rx_uuid& id, string_view_type instance, string_view_type node, const io::ip4_address& from);

      void peer_unregistered (const rx_uuid& id);

      peer_connection_ptr get_peer (const rx_uuid& id);

      bool is_peer (string_view_type instance, string_view_type node, const io::ip4_address& from);


  protected:

  private:


      connections_type connections_;


      static std::atomic<uint32_t> g_next_id;

      locks::slim_lock connections_lock_;

      string_type my_instance_;


};


} // namespace discovery
} // namespace rx_internal



#endif
