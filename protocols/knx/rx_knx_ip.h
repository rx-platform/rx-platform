

/****************************************************************************
*
*  protocols\knx\rx_knx_ip.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef rx_knx_ip_h
#define rx_knx_ip_h 1



// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_blocks_templates
#include "system/runtime/rx_blocks_templates.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace protocols {
namespace knx {
class knx_ip_port;
class knx_ip_source;

} // namespace knx
} // namespace protocols




namespace protocols {

namespace knx {
typedef rx::pointers::reference<knx_ip_port> knx_ip_port_ptr;
typedef rx::pointers::reference<knx_ip_source> knx_ip_source_ptr;





class knx_ip_endpoint : public rx::pointers::reference_object  
{

  public:
      knx_ip_endpoint (knx_ip_port_ptr port, rx_timer_ptr timer);

      ~knx_ip_endpoint();


      rx_protocol_stack_endpoint* bind_endpoint ();

      void close_endpoint ();

      rx_port_impl_ptr get_port ();

      void timer_fired ();


  protected:

  private:

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      static rx_protocol_result_t disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      rx_protocol_result_t received_packet (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



      knx_ip_port_ptr port_;


      rx_protocol_stack_endpoint stack_;

      rx_timer_ptr timer_;


};







typedef rx_platform::runtime::io_types::ports_templates::master_client_port_impl< knx_ip_endpoint  > knx_ip_prot_base;






class knx_ip_port : public knx_ip_prot_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
KNX IP client simple port. Implementation of simple KNX IP client.");

    DECLARE_REFERENCE_PTR(knx_ip_port);

	typedef std::multimap<string_type, knx_ip_source_ptr> sources_type;

  public:
      knx_ip_port();

      ~knx_ip_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void register_source (const string_type& addr, knx_ip_source_ptr what);

      void unregister_source (const string_type& addr, knx_ip_source_ptr what);

      void connected ();

      void disconnected ();


      static std::map<rx_node_id, knx_ip_port::smart_ptr> runtime_instances;

      uint16_t keep_alive;


  protected:

  private:


      rx_platform::runtime::io_types::simple_port_status status_;

      sources_type sources_;


      locks::slim_lock lock_;

      rx_timer_ptr timer_;

      async_owned_value<bool> connected_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_source_impl< knx_ip_port  > knx_ip_source_base;






class knx_ip_source : public knx_ip_source_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Implementation of KNX IP Source");

    DECLARE_REFERENCE_PTR(knx_ip_source);

  public:
      knx_ip_source();

      ~knx_ip_source();


      void port_connected (knx_ip_port_ptr port);

      void port_disconnected (knx_ip_port_ptr port);


  protected:

  private:


};


} // namespace knx
} // namespace protocols



#endif
