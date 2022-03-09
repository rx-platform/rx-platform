

/****************************************************************************
*
*  first_plugin\first_ports.h
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


#ifndef first_ports_h
#define first_ports_h 1



// rx_ptr
#include "lib/rx_ptr.h"
// rx_ports
#include "platform_api/rx_ports.h"

class first_client_port;
class first_server_port;
class first_transport_port;




using rx_platform_api::local_value;
using rx_platform_api::owned_value;




class first_transport_port_endpoint 
{

  public:
      first_transport_port_endpoint (first_transport_port* port);

      ~first_transport_port_endpoint();


      rx_protocol_stack_endpoint* get_endpoint ();

      rx_protocol_result_t received (recv_protocol_packet packet);


      first_transport_port * get_port ();


  protected:

  private:


      first_transport_port *port_;


      rx_protocol_stack_endpoint stack_;


};







typedef rx_platform_api::rx_transport_port< first_transport_port_endpoint  > first_transport_port_base;






class first_transport_port : public first_transport_port_base  
{
    DECLARE_PLUGIN_CODE_INFO(1, 1, 0, "\
First Transport Port. Transport Port implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_transport_port);

  public:
      first_transport_port();

      ~first_transport_port();


      rx_result initialize_port (rx_platform_api::rx_init_context& ctx);

      rx_result start_port (rx_platform_api::rx_start_context& ctx);

      rx_result stop_port ();

      rx_result deinitialize_port ();


  protected:

  private:

      void timer_tick ();

      std::unique_ptr<first_transport_port_endpoint> construct_endpoint ();



      runtime_handle_t timer_;


};






class first_server_endpoint : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(first_server_endpoint);

  public:
      first_server_endpoint (first_server_port* port);

      ~first_server_endpoint();


      rx_protocol_stack_endpoint* get_endpoint ();

      rx_protocol_result_t received (recv_protocol_packet packet);


      first_server_port * get_port ();


  protected:

  private:


      first_server_port *port_;


      rx_protocol_stack_endpoint stack_;


};







typedef rx_platform_api::rx_server_slave_port< first_server_endpoint  > first_server_port_base;






class first_server_port : public first_server_port_base  
{
    DECLARE_PLUGIN_CODE_INFO(0, 9, 0, "\
First Server Port. Server Port implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_server_port);

  public:
      first_server_port();

      ~first_server_port();


      rx_result initialize_port (rx_platform_api::rx_init_context& ctx);

      rx_result start_port (rx_platform_api::rx_start_context& ctx);

      rx_result stop_port ();

      rx_result deinitialize_port ();

      static std::map<rx_node_id, first_server_port::smart_ptr> runtime_instances;
  protected:

  private:

      void timer_tick ();

      rx_reference<first_server_endpoint> construct_endpoint ();



      runtime_handle_t timer_;


};






class first_client_endpoint : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(first_client_endpoint);

  public:
      first_client_endpoint (first_client_port* port);

      ~first_client_endpoint();


      rx_protocol_stack_endpoint* get_endpoint ();

      void close_endpoint ();

      rx_protocol_result_t received (recv_protocol_packet packet);

      void send_command (const string_type& val);


      first_client_port * get_port ();


  protected:

  private:


      first_client_port *port_;


      rx_protocol_stack_endpoint stack_;


};







typedef rx_platform_api::rx_client_master_port< first_client_endpoint  > first_client_port_base;






class first_client_port : public first_client_port_base  
{
    DECLARE_PLUGIN_CODE_INFO(0, 9, 0, "\
First Client Port. Client Port implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_client_port);

  public:
      first_client_port();

      ~first_client_port();


      rx_result initialize_port (rx_platform_api::rx_init_context& ctx);

      rx_result start_port (rx_platform_api::rx_start_context& ctx);

      rx_result stop_port ();

      rx_result deinitialize_port ();


      local_value<string_type> command;

      owned_value<string_type> response;


  protected:

  private:

      void timer_tick ();

      rx_reference<first_client_endpoint> construct_endpoint ();



      runtime_handle_t timer_;


};




#endif
