

/****************************************************************************
*
*  system\runtime\simple_master_endpoint.h
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


#ifndef simple_master_endpoint_h
#define simple_master_endpoint_h 1






namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename addrT>
class simple_master_routing_translator 
{
public:
    typedef rx_address_reference_type session_key_type;
    typedef std::true_type translates_packet;
    typedef std::false_type translates_session;
    typedef std::true_type client_master_translator;

  public:

      rx_address_reference_type get_reference (const rx_recv_protocol_packet_def* packet);

      rx_protocol_result_t translate_packet (const rx_recv_protocol_packet_def* packet, rx_address_reference_type& reference, io::numeric_address<addrT>& local_addr, io::numeric_address<addrT>& remote_addr);

      rx_protocol_result_t translate_session (const rx_session_def* session, rx_address_reference_type& reference, io::numeric_address<addrT>& local_addr, io::numeric_address<addrT>& remote_addr);

      rx_protocol_result_t create_references (io::numeric_address<addrT>& local_addr, io::numeric_address<addrT>& remote_addr, rx_address_reference_type& reference, rx_address_reference_type& local_ref, rx_address_reference_type& remote_ref);


  protected:

  private:


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::simple_master_routing_translator 


template <typename addrT>
rx_address_reference_type simple_master_routing_translator<addrT>::get_reference (const rx_recv_protocol_packet_def* packet)
{
    return packet->from;
}

template <typename addrT>
rx_protocol_result_t simple_master_routing_translator<addrT>::translate_packet (const rx_recv_protocol_packet_def* packet, rx_address_reference_type& reference, io::numeric_address<addrT>& local_addr, io::numeric_address<addrT>& remote_addr)
{
    remote_addr = packet->from;
    reference = packet->from;    
    local_addr = packet->to;
    return RX_PROTOCOL_OK;
}

template <typename addrT>
rx_protocol_result_t simple_master_routing_translator<addrT>::translate_session (const rx_session_def* session, rx_address_reference_type& reference, io::numeric_address<addrT>& local_addr, io::numeric_address<addrT>& remote_addr)
{
    RX_ASSERT(false);
    return RX_PROTOCOL_EMPTY;
}

template <typename addrT>
rx_protocol_result_t simple_master_routing_translator<addrT>::create_references (io::numeric_address<addrT>& local_addr, io::numeric_address<addrT>& remote_addr, rx_address_reference_type& reference, rx_address_reference_type& local_ref, rx_address_reference_type& remote_ref)
{
    remote_ref = remote_addr.get_address();
    local_ref = local_addr.get_address();
    reference = remote_ref;
    return RX_PROTOCOL_OK;
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
