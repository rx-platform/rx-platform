

/****************************************************************************
*
*  interfaces\rx_inverter_ports.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_inverter_ports_h
#define rx_inverter_ports_h 1



// dummy
#include "dummy.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"

namespace rx_internal {
namespace interfaces {
namespace ports_lib {
class i2l_listener_endpoint;
class initiator_to_listener_port;

} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal




namespace rx_internal {

namespace interfaces {

namespace ports_lib {





class listener_to_initiator_port : public rx_platform::runtime::items::port_runtime  
{

    DECLARE_REFERENCE_PTR(listener_to_initiator_port);

    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Listener to Initiator port connection");

public:
    enum current_port_state : uint_fast8_t
    {
        port_state_inactive = 0x00,
        port_state_started = 0x01,
        port_state_wait_listening = 0x02,
        port_state_wait_connecting = 0x04,
        port_state_listening = 0x08,
        port_state_connected = 0x10,

        port_listener = 0x0a,
        port_connector = 0x14,

        port_state_error = 0xff
    };

  public:
      listener_to_initiator_port();

      ~listener_to_initiator_port();


      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      void stack_assembled ();

      void stack_disassembled ();

      rx_result stop_passive ();


  protected:

  private:


      std::atomic<current_port_state> state_;


};






class initiator_to_listener_port : public rx_platform::runtime::items::port_runtime  
{

    DECLARE_REFERENCE_PTR(initiator_to_listener_port);

    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Initiator to Listener port connection");

public:
    enum current_port_state : uint_fast8_t
    {
        port_state_inactive = 0x00,
        port_state_started = 0x01,
        port_state_wait_listening = 0x02,
        port_state_wait_connecting = 0x04,
        port_state_listening = 0x08,
        port_state_connected = 0x10,
        port_state_stopped = 0x20,

        port_state_error = 0xff
    };

  public:
      initiator_to_listener_port();

      ~initiator_to_listener_port();


      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result_with<port_connect_result> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      void stack_assembled ();

      void stack_disassembled ();

      rx_result stop_passive ();

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      rx_protocol_result_t move_packet_down (send_protocol_packet packet);

      rx_protocol_result_t move_packet_up (recv_protocol_packet packet);

      rx_protocol_result_t listener_connected (rx_session* session);

      rx_protocol_result_t listener_disconnected (rx_session* session, rx_protocol_result_t reason);

      void listener_closed (rx_protocol_result_t reason);


  protected:

  private:


      rx_protocol_stack_endpoint initiator_endpoint_;

      std::unique_ptr<i2l_listener_endpoint> active_endpoint_;


      std::atomic<current_port_state> state_;


};






class i2l_listener_endpoint 
{

  public:
      i2l_listener_endpoint (initiator_to_listener_port* port);

      ~i2l_listener_endpoint();


      rx_protocol_stack_endpoint stack_endpoint;


  protected:

  private:

      rx_protocol_result_t connected (rx_session* session);

      rx_protocol_result_t received_packet (recv_protocol_packet packet);

      rx_protocol_result_t disconnected (rx_session* session, rx_protocol_result_t reason);

      void closed (rx_protocol_result_t reason);



      initiator_to_listener_port *my_port_;


};


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal



#endif
