

/****************************************************************************
*
*  system\runtime\address_endpoint.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef address_endpoint_h
#define address_endpoint_h 1






namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename addrT>
class address_routing_translator 
{
public:
    typedef addrT session_key_type;
    typedef std::true_type translates_packet;
    typedef std::false_type translates_session;
    typedef std::false_type client_master_translator;

  public:

      typename address_routing_translator<addrT>::session_key_type get_reference (const rx_recv_protocol_packet_def* packet);

      rx_protocol_result_t translate_packet (const rx_recv_protocol_packet_def* packet, session_key_type& reference, addrT& local_addr, addrT& remote_addr);

      rx_protocol_result_t translate_session (const rx_session_def* session, session_key_type& reference, addrT& local_addr, addrT& remote_addr);

      rx_protocol_result_t create_references (addrT& local_addr, addrT& remote_addr, session_key_type& reference, rx_address_reference_type& local_ref, rx_address_reference_type& remote_ref);


  protected:

  private:


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::address_routing_translator 


template <typename addrT>
typename address_routing_translator<addrT>::session_key_type address_routing_translator<addrT>::get_reference (const rx_recv_protocol_packet_def* packet)
{
    io::ip4_address addr;
    if (addr.parse(packet->from_addr))
        return addr;
    else
        return addrT();
}

template <typename addrT>
rx_protocol_result_t address_routing_translator<addrT>::translate_packet (const rx_recv_protocol_packet_def* packet, session_key_type& reference, addrT& local_addr, addrT& remote_addr)
{
    if(!local_addr.parse(packet->to_addr))
        return RX_PROTOCOL_INVALID_ADDR;
    if (!remote_addr.parse(packet->from_addr))
        return RX_PROTOCOL_INVALID_ADDR;
    reference = remote_addr;
    return RX_PROTOCOL_OK;
}

template <typename addrT>
rx_protocol_result_t address_routing_translator<addrT>::translate_session (const rx_session_def* session, session_key_type& reference, addrT& local_addr, addrT& remote_addr)
{
    return RX_PROTOCOL_INVALID_ADDR;
}

template <typename addrT>
rx_protocol_result_t address_routing_translator<addrT>::create_references (addrT& local_addr, addrT& remote_addr, session_key_type& reference, rx_address_reference_type& local_ref, rx_address_reference_type& remote_ref)
{
    return RX_PROTOCOL_OK;
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
