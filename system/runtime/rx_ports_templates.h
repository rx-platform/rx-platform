

/****************************************************************************
*
*  system\runtime\rx_ports_templates.h
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


#ifndef rx_ports_templates_h
#define rx_ports_templates_h 1



// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename endpointT, typename routingT>
class extern_routed_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard multiple endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(extern_routed_port_impl);

    typedef routed_port_endpoint<endpointT, routingT> endpoint_type;
    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpoint_type> > active_endpoints_type;

  public:

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void remove_connection (rx_protocol_stack_endpoint* what);


  protected:

  private:


      active_endpoints_type active_endpoints_;


};






template <typename endpointT>
class extern_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard single endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(extern_port_impl);

    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> > active_endpoints_type;

  public:

      void remove_endpoint (rx_protocol_stack_endpoint* what);


  protected:

  private:


      active_endpoints_type active_endpoints_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::extern_routed_port_impl 


template <typename endpointT, typename routingT>
rx_result extern_routed_port_impl<endpointT,routingT>::stop_runtime (runtime::runtime_stop_context& ctx)
{
    for (auto& one : active_endpoints_)
    {
        rx_close(one.first, RX_PROTOCOL_DISCONNECTED);
    }
    active_endpoints_.clear();
    return true;
}

template <typename endpointT, typename routingT>
void extern_routed_port_impl<endpointT,routingT>::remove_connection (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        active_endpoints_.erase(it);
    }
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::extern_port_impl 


template <typename endpointT>
void extern_port_impl<endpointT>::remove_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
        active_endpoints_.erase(it);// i just might want something to do with it...
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
