

/****************************************************************************
*
*  system\runtime\slave_endpoint.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef slave_endpoint_h
#define slave_endpoint_h 1






namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename addrT>
class slave_routing_translator
{

  public:

      rx_address_reference_type translate_from_passive (addrT& local_addr, addrT& remote_addr, rx_address_reference_type& local_reference, rx_address_reference_type& remote_reference, rx_address_reference_type& map_reference);

      rx_address_reference_type get_reference (const rx_recv_protocol_packet_def* packet);

      rx_protocol_result_t translate_packet (const rx_recv_protocol_packet_def* packet, rx_address_reference_type& reference, io::any_address& local_addr, io::any_address& remote_addr);


  protected:

  private:


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::slave_routing_translator


template <typename addrT>
rx_address_reference_type slave_routing_translator<addrT>::translate_from_passive (addrT& local_addr, addrT& remote_addr, rx_address_reference_type& local_reference, rx_address_reference_type& remote_reference, rx_address_reference_type& map_reference)
{
  return 0;
}

template <typename addrT>
rx_address_reference_type slave_routing_translator<addrT>::get_reference (const rx_recv_protocol_packet_def* packet)
{
  return 0;
}

template <typename addrT>
rx_protocol_result_t slave_routing_translator<addrT>::translate_packet (const rx_recv_protocol_packet_def* packet, rx_address_reference_type& reference, io::any_address& local_addr, io::any_address& remote_addr)
{
  return "jebi se";
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
