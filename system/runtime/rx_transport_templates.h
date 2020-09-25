

/****************************************************************************
*
*  system\runtime\rx_transport_templates.h
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


#ifndef rx_transport_templates_h
#define rx_transport_templates_h 1



// rx_active_endpoints
#include "system/runtime/rx_active_endpoints.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename endpointT>
class transport_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
one<=>one transport port implementation");

    DECLARE_REFERENCE_PTR(transport_port_impl);

    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> > active_endpoints_type;

  public:

      rx_protocol_stack_endpoint* create_endpoint ();

      void remove_endpoint (rx_protocol_stack_endpoint* what);


  protected:

  private:

      virtual std::unique_ptr<endpointT> construct_endpoint () = 0;



      active_endpoints_type active_endpoints_;


};






template <typename endpointT, typename routingT>
class routed_port_endpoint
{

  public:
      routed_port_endpoint (runtime::items::port_runtime* port);


      routingT router;


      std::unique_ptr<endpointT> endpoint;

      routed_port_endpoint(routed_port_endpoint&&) noexcept = default;
      routed_port_endpoint& operator=(routed_port_endpoint&&) noexcept = default;
  protected:

  private:


};






template <typename endpointT, typename routingT>
class routed_transport_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
one<=>many routed transport port implementation");

    DECLARE_REFERENCE_PTR(routed_transport_port_impl);

    typedef routed_port_endpoint<endpointT, routingT> endpoint_type;
    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpoint_type> > active_endpoints_type;

  public:

      rx_protocol_stack_endpoint* create_endpoint ();

      void remove_endpoint (rx_protocol_stack_endpoint* what);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      void stack_assembled ();

      void stack_disassembled ();


  protected:

  private:

      virtual std::unique_ptr<endpointT> construct_endpoint () = 0;



      active_endpoints_type active_endpoints_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::transport_port_impl


template <typename endpointT>
rx_protocol_stack_endpoint* transport_port_impl<endpointT>::create_endpoint ()
{
    auto endpoint_ptr = construct_endpoint();
    rx_protocol_stack_endpoint* entry = endpoint_ptr->bind([this](int64_t count)
        {
        },
        [this](int64_t count)
        {
        });
    if (entry->closed_function == nullptr)
    {
        entry->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->remove_endpoint(entry);
        };
    }
    active_endpoints_.emplace(entry, std::move(endpoint_ptr));
    auto reg_result = bind_stack_endpoint(entry, nullptr, nullptr);
    return entry;
}

template <typename endpointT>
void transport_port_impl<endpointT>::remove_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
        active_endpoints_.erase(it);
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::routed_port_endpoint

template <typename endpointT, typename routingT>
routed_port_endpoint<endpointT,routingT>::routed_port_endpoint (runtime::items::port_runtime* port)
    : router(port)
{
}



// Parameterized Class rx_platform::runtime::io_types::ports_templates::routed_transport_port_impl


template <typename endpointT, typename routingT>
rx_protocol_stack_endpoint* routed_transport_port_impl<endpointT,routingT>::create_endpoint ()
{
    auto endpoint_ptr = std::make_unique<endpoint_type>(this);
    endpoint_ptr->endpoint = construct_endpoint();
    rx_protocol_stack_endpoint* entry = endpoint_ptr->endpoint->bind([this](int64_t count)
        {
        },
        [this](int64_t count)
        {
        });
    auto push_result = rx_push_stack(entry, &endpoint_ptr->router.stack);
    if (push_result)
    {

    }
    active_endpoints_.emplace(&endpoint_ptr->router.stack, std::move(endpoint_ptr));
    return entry;
}

template <typename endpointT, typename routingT>
void routed_transport_port_impl<endpointT,routingT>::remove_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
        active_endpoints_.erase(it);
}

template <typename endpointT, typename routingT>
rx_result routed_transport_port_impl<endpointT,routingT>::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    return true;
}

template <typename endpointT, typename routingT>
rx_result routed_transport_port_impl<endpointT,routingT>::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    return RX_NOT_SUPPORTED;
}

template <typename endpointT, typename routingT>
void routed_transport_port_impl<endpointT,routingT>::stack_assembled ()
{
    listen(nullptr, nullptr);
}

template <typename endpointT, typename routingT>
void routed_transport_port_impl<endpointT,routingT>::stack_disassembled ()
{
    if (!active_endpoints_.empty())
    {
        for (auto& one : active_endpoints_)
        {
            one.second->router.close_sessions();
        }
    }
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
