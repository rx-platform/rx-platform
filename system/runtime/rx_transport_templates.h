

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
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_endpoint ();

      void destroy_endpoint (rx_protocol_stack_endpoint* what);


  protected:

  private:


      active_endpoints_type active_endpoints_;


};






template <typename endpointT, typename routingT>
class routed_port_endpoint 
{
  public:
    typedef rx_reference<routingT> routing_endpoint_ptr;

  public:
      routed_port_endpoint();

      routed_port_endpoint (runtime::items::port_runtime* port);

      ~routed_port_endpoint();


      typename routed_port_endpoint<endpointT, routingT>::routing_endpoint_ptr router (runtime::items::port_runtime* port);


      std::unique_ptr<endpointT> endpoint;

      routed_port_endpoint(routed_port_endpoint&&) noexcept = default;
      routed_port_endpoint& operator=(routed_port_endpoint&&) noexcept = default;
  protected:

  private:


      routing_endpoint_ptr router_;


};






template <typename endpointT, typename routingT>
class routed_transport_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
one<=>many routed transport port implementation");

    DECLARE_REFERENCE_PTR(routed_transport_port_impl);

    typedef routed_port_endpoint<endpointT, routingT> endpoint_type;
    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpoint_type> > active_endpoints_type;
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_endpoint ();

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result_with<rx_protocol_stack_endpoint*> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      void stack_assembled ();

      void stack_disassembled ();

      rx_result stop_passive ();


  protected:

  private:


      active_endpoints_type active_endpoints_;


};






template <typename endpointT, typename routingT>
class routed_master_transport_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
one<=>many routed master transport port implementation");

    DECLARE_REFERENCE_PTR(routed_master_transport_impl);

    typedef routed_port_endpoint<endpointT, routingT> endpoint_type;
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:
      routed_master_transport_impl();


      rx_protocol_stack_endpoint* construct_endpoint ();

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result_with<rx_protocol_stack_endpoint*> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      void stack_assembled ();

      void stack_disassembled ();

      rx_result stop_passive ();


  protected:

  private:


      endpoint_type active_endpoint_;


      rx_protocol_stack_endpoint* stack_endpoint_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::transport_port_impl 


template <typename endpointT>
rx_protocol_stack_endpoint* transport_port_impl<endpointT>::construct_endpoint ()
{
    if (!construct_func)
        return nullptr;
    auto endpoint_data = construct_func();

    if (endpoint_data.first->closed_function == nullptr)
    {
        endpoint_data.first->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->unbind_stack_endpoint(entry);
        };
    }
    active_endpoints_.emplace(endpoint_data.first, std::move(endpoint_data.second));
    return endpoint_data.first;
}

template <typename endpointT>
void transport_port_impl<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        active_endpoints_.erase(it);
    }
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::routed_port_endpoint 

template <typename endpointT, typename routingT>
routed_port_endpoint<endpointT,routingT>::routed_port_endpoint()
{
}

template <typename endpointT, typename routingT>
routed_port_endpoint<endpointT,routingT>::routed_port_endpoint (runtime::items::port_runtime* port)
    : router_(rx_create_reference<routingT>(port))
{
}


template <typename endpointT, typename routingT>
routed_port_endpoint<endpointT,routingT>::~routed_port_endpoint()
{
}



template <typename endpointT, typename routingT>
typename routed_port_endpoint<endpointT, routingT>::routing_endpoint_ptr routed_port_endpoint<endpointT,routingT>::router (runtime::items::port_runtime* port)
{
    if (!router_)
    {
        router_ = rx_create_reference<routingT>(port);
        port->register_routing_endpoint(&router_->stack);
    }
    return router_;
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::routed_transport_port_impl 


template <typename endpointT, typename routingT>
rx_protocol_stack_endpoint* routed_transport_port_impl<endpointT,routingT>::construct_endpoint ()
{
    auto endpoint_ptr = std::make_unique<endpoint_type>(this);
    if (!construct_func)
        return nullptr;
    auto endpoint_data = construct_func();

    if (endpoint_data.first->closed_function == nullptr)
    {
        endpoint_data.first->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->unbind_stack_endpoint(entry);
        };
    }

    auto push_result = rx_push_stack(endpoint_data.first, &endpoint_ptr->router(this)->stack);
    if (push_result == RX_PROTOCOL_OK)
    {
        endpoint_ptr->endpoint = std::move(endpoint_data.second);
        active_endpoints_.emplace(&endpoint_ptr->router(this)->stack, std::move(endpoint_ptr));
        return endpoint_data.first;
    }
    else
    {
        return nullptr;
    }
}

template <typename endpointT, typename routingT>
void routed_transport_port_impl<endpointT,routingT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        it->second->router(this)->close_sessions();
        active_endpoints_.erase(it);
    }
}

template <typename endpointT, typename routingT>
rx_result routed_transport_port_impl<endpointT,routingT>::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    using client_master_t = typename routingT::client_master_translator;
    if constexpr (client_master_t::value)
        return RX_NOT_SUPPORTED;
    else
        return true;
}

template <typename endpointT, typename routingT>
rx_result_with<rx_protocol_stack_endpoint*> routed_transport_port_impl<endpointT,routingT>::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    return RX_NOT_SUPPORTED;
}

template <typename endpointT, typename routingT>
void routed_transport_port_impl<endpointT,routingT>::stack_assembled ()
{
    using client_master_t = typename routingT::client_master_translator;
    if constexpr (client_master_t::value)
        connect(nullptr, nullptr);
    else
        listen(nullptr, nullptr);
}

template <typename endpointT, typename routingT>
void routed_transport_port_impl<endpointT,routingT>::stack_disassembled ()
{
}

template <typename endpointT, typename routingT>
rx_result routed_transport_port_impl<endpointT,routingT>::stop_passive ()
{
    return true;
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::routed_master_transport_impl 

template <typename endpointT, typename routingT>
routed_master_transport_impl<endpointT,routingT>::routed_master_transport_impl()
      : stack_endpoint_(nullptr)
{
}



template <typename endpointT, typename routingT>
rx_protocol_stack_endpoint* routed_master_transport_impl<endpointT,routingT>::construct_endpoint ()
{
    if (!construct_func)
        return nullptr;
    auto endpoint_data = construct_func();

    if (endpoint_data.first->closed_function == nullptr)
    {
        endpoint_data.first->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->disconnect_stack_endpoint(entry);
        };
    }
    auto push_result = rx_push_stack(endpoint_data.first, &active_endpoint_.router(this)->stack);
    if (push_result == RX_PROTOCOL_OK)
    {
        //auto result = register_routing_endpoint(endpoint_data.first);
        stack_endpoint_ = endpoint_data.first;
        active_endpoint_.endpoint = std::move(endpoint_data.second);
        return endpoint_data.first;
    }
    else
    {
        return nullptr;
    }
}

template <typename endpointT, typename routingT>
void routed_master_transport_impl<endpointT,routingT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    if (what && what == stack_endpoint_)
    {
        stack_endpoint_ = nullptr;
        active_endpoint_.router(this)->close_sessions();
        active_endpoint_.endpoint.reset();
    }
}

template <typename endpointT, typename routingT>
rx_result routed_master_transport_impl<endpointT,routingT>::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    return RX_NOT_SUPPORTED;
}

template <typename endpointT, typename routingT>
rx_result_with<rx_protocol_stack_endpoint*> routed_master_transport_impl<endpointT,routingT>::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    auto result = active_endpoint_.router(this)->create_session(local_address, remote_address, endpoint);
    return result;
}

template <typename endpointT, typename routingT>
void routed_master_transport_impl<endpointT,routingT>::stack_assembled ()
{
    using client_master_t = typename routingT::client_master_translator;
    if constexpr (client_master_t::value)
        connect(nullptr, nullptr);
    else
        listen(nullptr, nullptr);
}

template <typename endpointT, typename routingT>
void routed_master_transport_impl<endpointT,routingT>::stack_disassembled ()
{
}

template <typename endpointT, typename routingT>
rx_result routed_master_transport_impl<endpointT,routingT>::stop_passive ()
{
    return true;
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
