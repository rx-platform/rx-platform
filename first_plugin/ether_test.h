

/****************************************************************************
*
*  first_plugin\ether_test.h
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


#ifndef ether_test_h
#define ether_test_h 1



// rx_ptr
#include "lib/rx_ptr.h"
// rx_ports
#include "platform_api/rx_ports.h"

namespace ether {
class ether_subscriber1_port;

} // namespace ether




namespace ether {





class ether_subscriber1_endpoint : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(ether_subscriber1_endpoint);

  public:
      ether_subscriber1_endpoint (ether_subscriber1_port* port);

      ~ether_subscriber1_endpoint();


      rx_protocol_stack_endpoint* get_endpoint ();

      void close_endpoint ();

      rx_protocol_result_t received (recv_protocol_packet packet);

      void send_command (const string_type& val);


      ether_subscriber1_port * get_port ();


  protected:

  private:


      ether_subscriber1_port *port_;


      rx_protocol_stack_endpoint stack_;


};







typedef rx_platform_api::rx_client_master_port< ether_subscriber1_endpoint  > ether_subscriber1_base;






class ether_subscriber1_port : public ether_subscriber1_base  
{
    DECLARE_PLUGIN_CODE_INFO(0, 9, 0, "\
First Client Port. Client Port implementation in test plugin.");

    DECLARE_REFERENCE_PTR(ether_subscriber1_port);

  public:
      ether_subscriber1_port();

      ~ether_subscriber1_port();


      rx_result initialize_port (rx_platform_api::rx_init_context& ctx);

      rx_result start_port (rx_platform_api::rx_start_context& ctx);

      rx_result stop_port ();

      rx_result deinitialize_port ();


  protected:

  private:

      rx_reference<ether_subscriber1_endpoint> construct_endpoint ();



};


} // namespace ether



#endif
