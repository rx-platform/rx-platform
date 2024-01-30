

/****************************************************************************
*
*  discovery\rx_discovery_main.h
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


#ifndef rx_discovery_main_h
#define rx_discovery_main_h 1


#include "system/runtime/rx_value_templates.h"
#include "rx_discovery_algorithm.h"

// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_internal_protocol
#include "sys_internal/rx_internal_protocol.h"
// rx_discovery_comm
#include "discovery/rx_discovery_comm.h"
// rx_discovery_items
#include "discovery/rx_discovery_items.h"

namespace rx_internal {
namespace discovery {
class peer_object;

} // namespace discovery
} // namespace rx_internal


#include "system/hosting/rx_host.h"
using rx_internal::rx_protocol::messages::rx_transaction_ptr;
using rx_internal::rx_protocol::messages::rx_transaction_data;



namespace rx_internal {

namespace discovery {
class peer_item;
typedef rx_reference<peer_item> peer_item_ptr;





struct peer_endpoint 
{

      peer_endpoint (string_type name, io::ip4_address addr, string_type ep = "rx-platform/*");

      peer_endpoint (string_type name, string_type addr, uint16_t port, string_type ep = "rx-platform/*");


      string_type get_url () const;


      io::ip4_address ip4;

      string_type com_port;

      string_type eth_port;

      string_type path;

      string_type peer_name;

  public:
      peer_endpoint(const peer_endpoint& right);
      peer_endpoint(peer_endpoint&& right) noexcept;
  protected:

  private:


};







class peer_connection : public rx_protocol::rx_protocol_client_user  
{
    DECLARE_REFERENCE_PTR(peer_connection);
    
    typedef std::tuple<peer_directory_algorithm, peer_refresh_algorithm> algorithms_type;

    typedef std::map<rx_node_id, peer_item_ptr> cached_items_type;
    typedef std::map<string_type, peer_item_ptr> cached_paths_type;

  public:
      peer_connection (peer_endpoint endpoint);

      ~peer_connection();


      rx_result build (hosting::rx_platform_host* host, configuration_data_t& config, uint32_t id);

      rx_result send_request (rx_transaction_ptr trans, uint32_t timeout);

      bool port_connected (rx_protocol::rx_json_protocol_client_port::smart_ptr port);

      void port_disconnected ();

      rx_result initialize_runtime (runtime_init_context& ctx, rx_reference<peer_object> object);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      void client_connected ();

      void client_disconnected ();

      void item_changed (const rx_node_id& id, const string_type& path);

      void add_peer_item (peer_item_ptr item);

      void remove_peer_item (const rx_node_id& id);

      template<class reqT>
      rx_result send_request(std::unique_ptr<reqT> req, uint32_t timeout)
      {
          auto trans = rx_create_reference<rx_transaction_data<reqT, typename reqT::response_type> >(std::move(req));
          return send_request(std::move(trans), timeout);
      }
  protected:

  private:


      peer_endpoint endpoint_;

      rx_reference<rx_protocol::rx_json_protocol_client_port> my_port_;

      rx_reference<peer_object> object_;

      cached_items_type cached_items_;

      cached_paths_type cached_paths_;


      algorithms_type algorithms_;

      locks::slim_lock items_lock_;


};







class discovery_manager 
{
    typedef std::map<string_type, peer_connection_ptr> connections_type;

  public:
      discovery_manager();

      ~discovery_manager();


      static discovery_manager& instance ();

      void clear ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& config);

      rx_result start (hosting::rx_platform_host* host, const configuration_data_t& config);

      void stop ();

      void deinitialize ();

      std::vector<peer_endpoint> get_peers ();

      peer_connection_ptr get_peer (const string_type& name);

      uint32_t subscribe_to_port (std::function<void(uint16_t)> callback, rx_reference_ptr anchor);

      void unsubscribe_from_port (uint32_t id);

      std::vector<discovery::discovered_peer_data> get_peers_network ();


  protected:

  private:


      connections_type connections_;

      discovery_register peers_register_;


      static std::unique_ptr<discovery_manager> g_obj;

      static std::atomic<uint32_t> g_next_id;


};






class peer_object : public rx_platform::runtime::items::object_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Peer connection object implementation");
    DECLARE_REFERENCE_PTR(peer_object);

    class peer_connection_resolver_user : public runtime::relation_subscriber
    {
    public:
        peer_object* my_obj;
        void relation_connected(const string_type& name, const platform_item_ptr& item)
        {
            my_obj->internal_port_connected(item);
        }
        void relation_disconnected(const string_type& name)
        {
            my_obj->internal_port_disconnected();
        }
    };
    peer_connection_resolver_user resolver_user_;
    friend class peer_object::peer_connection_resolver_user;

  public:
      peer_object();

      ~peer_object();


      virtual rx_result initialize_runtime (runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime_stop_context& ctx);

      void set_status (bool online, const string_type& ver);


  protected:

  private:

      bool internal_port_connected (const platform_item_ptr& item);

      void internal_port_disconnected ();



      rx_reference<peer_connection> connection_;


      async_owned_value<bool> online_;

      async_owned_value<string_type> stream_version_;


    friend class peer_connection;
};


} // namespace discovery
} // namespace rx_internal



#endif
