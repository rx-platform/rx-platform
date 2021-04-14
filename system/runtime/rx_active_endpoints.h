

/****************************************************************************
*
*  system\runtime\rx_active_endpoints.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_active_endpoints_h
#define rx_active_endpoints_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"

// dummy
#include "dummy.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace runtime {
namespace io_types {
namespace ports_templates {
template <typename addrT> class simple_master_routing_translator;
template <typename addrT> class address_routing_translator;
template <typename addrT> class simple_slave_routing_translator;
template <typename translatorT, typename addrT> class routing_endpoint;

} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {
namespace items
{
class port_runtime;
}

namespace io_types {

namespace ports_templates {





template <typename translatorT, typename addrT>
class session_endpoint 
{

  public:
      session_endpoint (addrT local_addr, addrT remote_addr, rx_address_reference_type local_reference, rx_address_reference_type remote_reference, rx_protocol_stack_endpoint* bellow);

      ~session_endpoint();


      rx_protocol_stack_endpoint stack;

      rx_reference<routing_endpoint<translatorT,addrT> > router;


      addrT local_addr;

      addrT remote_addr;

      rx_address_reference_type local_reference;

      rx_address_reference_type remote_reference;


  protected:

  private:

      rx_protocol_result_t send_function (send_protocol_packet packet);

      void closed_function (rx_protocol_result_t reason);



      rx_protocol_stack_endpoint* bellow_;


};







template <typename translatorT, typename addrT>
class routing_endpoint : public rx::pointers::reference_object  
{
    typedef session_endpoint<translatorT, addrT> session_type;
    typedef typename translatorT::session_key_type session_key_t;
    typedef std::map<session_key_t, std::unique_ptr<session_type> > active_map_type;
    DECLARE_REFERENCE_PTR(routing_endpoint);
  public:
    typedef typename translatorT::client_master_translator client_master_translator;

  public:
      routing_endpoint (runtime::items::port_runtime* port);

      ~routing_endpoint();


      void remove_session (const session_type* who);

      void close_sessions ();

      rx_result_with<rx_protocol_stack_endpoint*> create_session (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



      rx_protocol_stack_endpoint stack;


  protected:

  private:

      rx_protocol_result_t recv_packet (recv_protocol_packet* packet);

      rx_protocol_result_t connected (rx_session_def* session);

      rx_protocol_result_t disconnected (rx_session_def* session, rx_protocol_result_t reason);

      void closed (rx_protocol_result_t reason);

      typename routing_endpoint<translatorT, addrT>::session_key_t get_reference (const session_type* who);



      active_map_type active_map_;


      runtime::items::port_runtime* port_;

      translatorT translator_;

      locks::slim_lock sessions_lock_;

      bool connected_;


};


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
