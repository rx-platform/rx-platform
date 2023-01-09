

/****************************************************************************
*
*  interfaces\rx_full_duplex_packet.h
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


#ifndef rx_full_duplex_packet_h
#define rx_full_duplex_packet_h 1



// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// dummy
#include "dummy.h"

namespace rx_internal {
namespace interfaces {
namespace ports_lib {
template <typename addrT> class listener_instance;
template <typename addrT> class full_duplex_addr_packet_port;

} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal


using namespace rx_platform::runtime;


namespace rx_internal {

namespace interfaces {

namespace ports_lib {
template <typename addrT>
class initiator_data_type;
template <typename addrT>
class listener_data_type;
template <typename addrT>
class listener_instance;


// adapter
template <typename addrT>
struct duplex_port_adapter
{
    typedef addrT key_type;
    typedef std::pair<addrT, addrT> listener_key_type;

    static key_type get_key_from_addresses_initiator(const addrT& local_address, const addrT& remote_address);
    static std::pair<bool, key_type> get_key_from_packet_initiator(const recv_protocol_packet& packet);
    static listener_key_type get_key_from_addresses_listener(const addrT& local_address, const addrT& remote_address);
    static std::pair<bool, listener_key_type> get_key_from_packet_listener(const recv_protocol_packet& packet);
    static listener_key_type get_key_from_listener(const listener_data_type<addrT>& session_data);
    static void fill_send_packet(send_protocol_packet& packet, const initiator_data_type<addrT>& session_data);
    static void fill_send_packet(send_protocol_packet& packet, const listener_data_type<addrT>& session_data);

    static std::pair<key_type, std::unique_ptr<initiator_data_type<addrT> > > create_initiator_data(const protocol_address* local_address, const protocol_address* remote_address);
    static std::unique_ptr<listener_instance<addrT> > create_listener_instance(const protocol_address* local_address, const protocol_address* remote_address);
    static std::pair<listener_key_type, std::unique_ptr<listener_data_type<addrT> > > create_listener_data(const protocol_address* local_address, const protocol_address* remote_address);
};




template <typename addrT>
class initiator_data_type 
{

  public:
      initiator_data_type();

      ~initiator_data_type();


      rx_protocol_stack_endpoint stack_endpoint;

      full_duplex_addr_packet_port<addrT>* my_port;


      addrT local_addr;

      addrT remote_addr;

      initiator_data_type(const initiator_data_type&) = delete;
      initiator_data_type(initiator_data_type&&) noexcept = default;
      initiator_data_type& operator=(const initiator_data_type&) = delete;
      initiator_data_type& operator=(initiator_data_type&&) noexcept = default;
  protected:

  private:


};






template <typename addrT>
class listener_data_type 
{

  public:
      listener_data_type();

      ~listener_data_type();


      rx_protocol_stack_endpoint stack_endpoint;

      full_duplex_addr_packet_port<addrT>* my_port;

      listener_instance<addrT>* my_instance;


      addrT local_addr;

      addrT remote_addr;

      bool binded;

      listener_data_type(const listener_data_type&) = delete;
      listener_data_type(listener_data_type&&) noexcept = default;
      listener_data_type& operator=(const listener_data_type&) = delete;
      listener_data_type& operator=(listener_data_type&&) noexcept = default;
  protected:

  private:


};






template <typename addrT>
class listener_instance 
{
    typedef duplex_port_adapter<addrT> address_adapter_type;
    typedef typename duplex_port_adapter<addrT>::listener_key_type listener_key_type;

    typedef listener_data_type<addrT> listener_data_t;
    typedef std::map<listener_key_type, listener_data_t*> listeners_type;

  public:
      listener_instance();

      ~listener_instance();


      rx_protocol_result_t route_listeners_packet (recv_protocol_packet packet);

      rx_protocol_result_t listener_connected_received (rx_session* session);

      void listener_closed_received (rx_protocol_result_t result);

      rx_protocol_result_t remove_listener (const listener_key_type& key);


      full_duplex_addr_packet_port<addrT>* my_port;

      rx_protocol_stack_endpoint stack_endpoint;


      addrT local_addr;

      addrT remote_addr;

      listener_instance(const listener_instance&) = delete;
      listener_instance(listener_instance&&) noexcept = default;
      listener_instance& operator=(const listener_instance&) = delete;
      listener_instance& operator=(listener_instance&&) noexcept = default;
  protected:

  private:

      rx_protocol_stack_endpoint* find_listener_endpoint (const listener_key_type& key, const protocol_address* local_address, const protocol_address* remote_address);



      listeners_type listeners_;


      locks::slim_lock routing_lock_;


};






template <typename addrT>
class full_duplex_addr_packet_port : public rx_platform::runtime::items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
standard single endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(full_duplex_addr_packet_port);

    typedef initiator_data_type<addrT> initiator_data_t;
    typedef listener_data_type<addrT> listener_data_t;
    typedef listener_instance<addrT> listener_instance_t;

    friend class initiator_data_type<addrT>;
    friend class listener_instance<addrT>;
    friend class listener_data_type<addrT>;

    typedef duplex_port_adapter<addrT> address_adapter_type;
    typedef typename duplex_port_adapter<addrT>::key_type key_type;
    typedef typename duplex_port_adapter<addrT>::listener_key_type listener_key_type;

    // listeners initiator initiators

    typedef std::map<key_type, std::unique_ptr<initiator_data_t> > initiators_type;
    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<listener_instance_t> > listener_endpoints_type;
    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<listener_data_t> > listening_type;

    enum current_port_state : uint_fast8_t
    {
        port_state_inactive         = 0x00,
        port_state_started          = 0x01,
        port_state_wait_listening   = 0x02,
        port_state_wait_connecting  = 0x04,
        port_state_listening        = 0x08,
        port_state_connected        = 0x10,

        port_listener               = 0x0a,
        port_connector              = 0x14,

        port_state_error        = 0xff
    };
protected:

  public:
      full_duplex_addr_packet_port();


      rx_protocol_stack_endpoint* construct_initiator_endpoint ();

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result_with<port_connect_result> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      void stack_assembled ();

      void stack_disassembled ();

      rx_result stop_passive ();

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      void received_packet ();

      void sent_packet ();


      rx_platform::runtime::io_types::simple_port_status status;


  protected:

  private:

      rx_protocol_result_t route_initiator_packet (recv_protocol_packet packet);

      rx_protocol_result_t initiator_connected_received (rx_session* session);

      rx_protocol_stack_endpoint* find_initiators_endpoint (const key_type& key);

      rx_protocol_result_t remove_initiator (const key_type& key);



      initiators_type initiators_;

      rx_protocol_stack_endpoint initiator_endpoint_;

      listener_endpoints_type listener_endpoints_;

      listening_type listening_;


      locks::slim_lock routing_lock_;

      std::atomic<current_port_state> state_;

      remote_local_value<uint32_t> session_timeout_;


};






typedef full_duplex_addr_packet_port< io::numeric_address<uint8_t>  > byte_routing_port_base;






class byte_routing_port : public byte_routing_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Byte address routing port implementation, for both initiators and listeners");

    DECLARE_REFERENCE_PTR(byte_routing_port);

  public:

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


  protected:

  private:


};






typedef full_duplex_addr_packet_port< io::ip4_address  > ip4_routing_port_base;






class ip4_routing_port : public ip4_routing_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
IP4 routing port implementation, for both initiators and listeners");

    DECLARE_REFERENCE_PTR(ip4_routing_port);

  public:

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


  protected:

  private:


};






typedef full_duplex_addr_packet_port< io::mac_address  > ether_routing_port_base;






class mac_routing_port : public ether_routing_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
MAC address routing port implementation, for both publishers and subscribers");

    DECLARE_REFERENCE_PTR(mac_routing_port);

  public:

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


  protected:

  private:


};






typedef full_duplex_addr_packet_port< io::numeric_address<uint16_t>  > word_routing_port_base;






class word_routing_port : public word_routing_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Word address routing port implementation, for both initiators and listeners");

    DECLARE_REFERENCE_PTR(word_routing_port);

  public:

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


  protected:

  private:


};


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal



#endif
