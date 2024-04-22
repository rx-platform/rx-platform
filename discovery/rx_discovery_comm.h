

/****************************************************************************
*
*  discovery\rx_discovery_comm.h
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


#ifndef rx_discovery_comm_h
#define rx_discovery_comm_h 1



// rx_datagram_io
#include "interfaces/rx_datagram_io.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_internal {
namespace discovery {
class discovery_register;

} // namespace discovery
} // namespace rx_internal




namespace rx_internal {

namespace discovery {
class peer_discovery_algorithms;
class discovery_manager;





class discovery_point : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(discovery_point);

    struct port_subscriber_data
    {
        rx_reference_ptr anchor;
        std::function<void(uint32_t)> callback;
    };
    typedef std::map<rx_thread_handle_t, std::map<uint32_t, port_subscriber_data> > port_subscribers_type;

    typedef std::stack< buffer_ptr, std::vector<buffer_ptr> > free_buffers_type;

    friend class peer_discovery_algorithms;

    enum class discovery_state
    {
        idle = 0,
        opening_default = 1,
        default_active = 2,
        opening_fallback = 3,
        fallback_active = 4,
        stopping = 5
    };
    struct peer_data
    {
        rx_timer_ticks_t last_heard;
        uint32_t version;
        io::ip4_address binded;
    };
    typedef std::map<rx_uuid, peer_data> registered_peers_type;


    struct socket_holder_t : public interfaces::ip_endpoints::udp_socket_std_buffer
    {
        DECLARE_REFERENCE_PTR(discovery_point::socket_holder_t);
        friend class discovery_point;
    private:
        discovery_point::smart_ptr whose;
        io::ip4_address bind_address;
    protected:
        void release_buffer(buffer_ptr what);
        bool readed(const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity);
        void on_shutdown(rx_security_handle_t identity);
    public:
        socket_holder_t(discovery_point::smart_ptr whose, const io::ip4_address& bind_addr);
        socket_holder_t(socket_holder_t&& right) noexcept;
        void timer_tick(rx_timer_ticks_t ticks);
        void disconnect();
    };
    friend struct discovery_point::socket_holder_t;

    typedef rx_reference<socket_holder_t> socket_ptr_type;

    struct socket_struct_t
    {
        io::ip4_address bind_address;
        socket_ptr_type socket;
    };

    typedef std::map<io::ip4_address, socket_struct_t> sockets_type;

  public:
      discovery_point();

      ~discovery_point();


      bool on_startup (rx_thread_handle_t destination);

      void on_shutdown (rx_security_handle_t identity);

      void activate ();

      void deactivate ();

      void timer_tick (rx_timer_ticks_t ticks);

      buffer_ptr get_buffer ();

      uint32_t subscribe_to_port (std::function<void(uint16_t)> callback, rx_reference_ptr anchor);

      void unsubscribe_from_port (uint32_t id);


  protected:

      bool readed (const void* data, size_t count, const struct sockaddr* bind, const struct sockaddr* addr, rx_security_handle_t identity);

      void release_buffer (buffer_ptr what);


  private:

      void port_changed ();

      bool send_broadcast (buffer_ptr buffer);

      void close_sockets ();

      rx_result open_sockets (uint16_t port);



      discovery_register *my_register_;

      sockets_type sockets_;


      rx_thread_handle_t executer_;

      io::ip4_address multicast_address_;

      const sockaddr_in* multicast_arg_;

      free_buffers_type free_buffers_;

      locks::slim_lock free_buffers_lock_;

      uint32_t last_id_;

      jobs::periodic_job::smart_ptr timer_;

      locks::slim_lock state_lock_;

      discovery_state state_;

      port_subscribers_type port_subscribers_;

      uint16_t current_port_;

      uint16_t system_port_;

      uint16_t default_port_;

      rx_timer_ticks_t next_tick_;

      registered_peers_type registered_masters_;

      registered_peers_type registered_followers_;

      socket_ptr_type local_socket_ptr_;


};


struct discovered_network_point
{
    string_type address;
    string_type network;

    bool operator==(const discovered_network_point& right) const
    {
        return address == right.address
            && network == right.network;
    }

    bool operator!=(const discovered_network_point& right) const
    {
        return !operator==(right);
    }
};




struct discovered_peer_data 
{

      bool operator==(const discovered_peer_data &right) const;

      bool operator!=(const discovered_peer_data &right) const;


      bool serialize (base_meta_writer& writer, const string_type& name, uint32_t version) const;

      bool deserialize (base_meta_reader& reader, const string_type& name, uint32_t version);


      rx_uuid id;

      string_type instance;

      string_type node;

      std::vector<discovered_network_point> addresses;

      uint32_t version;

  public:

  protected:

  private:


};






class discovery_register 
{
    typedef std::map<rx_uuid, std::vector<discovered_peer_data> > peers_type;

    friend class peer_discovery_algorithms;
    friend class discovery_manager;

  public:
      discovery_register();

      ~discovery_register();


      void set_system_port (uint16_t port);

      bool is_this_you (const rx_uuid_t& id);

      const rx_uuid_t& get_identity () const;

      std::vector<io::ip4_address> get_own_addresses (uint16_t port) const;


      const io::ip4_address get_multicast_address () const
      {
        return multicast_address_;
      }



  protected:

  private:


      peers_type peers_;

      rx_reference<discovery_point> comm_point_;

      discovered_peer_data my_data_;

      peers_type followers_;


      rx_uuid identity_;

      uint16_t system_port_;

      std::vector<discovered_network_point> system_addresses_;

      locks::slim_lock register_lock_;

      io::ip4_address multicast_address_;


};


} // namespace discovery
} // namespace rx_internal



#endif
