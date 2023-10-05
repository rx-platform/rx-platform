

/****************************************************************************
*
*  system\runtime\rx_transport_templates.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_transport_templates_h
#define rx_transport_templates_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename endpointT>
class connection_transport_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 2, 0, "\
one<=>one transport port implementation");

    DECLARE_REFERENCE_PTR(connection_transport_port_impl);

    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> > active_endpoints_type;
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> >(const protocol_address* local_address, const protocol_address* remote_address)> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_initiator_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result stack_endpoint_connected (rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr);


  protected:

  private:

      rx_protocol_stack_endpoint* construct_endpoint (const protocol_address* local_address, const protocol_address* remote_address);



      active_endpoints_type active_endpoints_;


};






template <typename endpointT>
class transport_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
one<=>one transport port implementation");

    DECLARE_REFERENCE_PTR(transport_port_impl);

    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> > active_endpoints_type;
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> >(const protocol_address* local_address, const protocol_address* remote_address)> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_initiator_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);


  protected:

  private:

      rx_protocol_stack_endpoint* construct_endpoint (const protocol_address* local_address, const protocol_address* remote_address);



      active_endpoints_type active_endpoints_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::connection_transport_port_impl 


template <typename endpointT>
rx_protocol_stack_endpoint* connection_transport_port_impl<endpointT>::construct_initiator_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    return construct_endpoint(local_address, remote_address);
}

template <typename endpointT>
rx_protocol_stack_endpoint* connection_transport_port_impl<endpointT>::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    return construct_endpoint(local_address, remote_address);
}

template <typename endpointT>
void connection_transport_port_impl<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        it->second->close_endpoint();
        active_endpoints_.erase(it);
    }
}

template <typename endpointT>
rx_protocol_stack_endpoint* connection_transport_port_impl<endpointT>::construct_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    if (!construct_func)
        return nullptr;
    auto endpoint_data = construct_func(local_address, remote_address);

    if (endpoint_data.first == nullptr)
    {
        return nullptr;
    }
    if (endpoint_data.first->closed_function == nullptr)
    {
        endpoint_data.first->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->unbind_stack_endpoint(entry);
        };
    }
    if (endpoint_data.first->allocate_packet == nullptr)
    {
        endpoint_data.first->allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            auto result = whose->get_port()->alloc_io_buffer();
            if (result)
            {
                result.value().detach(buffer);
                return RX_PROTOCOL_OK;
            }
            else
            {
                return RX_PROTOCOL_OUT_OF_MEMORY;
            }
        };
    }
    if (endpoint_data.first->release_packet == nullptr)
    {
        endpoint_data.first->release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            io::rx_io_buffer temp;
            temp.attach(buffer);
            whose->get_port()->release_io_buffer(std::move(temp));

            return RX_PROTOCOL_OK;
        };
    }
    active_endpoints_.emplace(endpoint_data.first, std::move(endpoint_data.second));
    return endpoint_data.first;
}

template <typename endpointT>
rx_result connection_transport_port_impl<endpointT>::stack_endpoint_connected (rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr)
{
    return add_stack_endpoint(what, local_addr, remote_addr);
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::transport_port_impl 


template <typename endpointT>
rx_protocol_stack_endpoint* transport_port_impl<endpointT>::construct_initiator_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    return construct_endpoint(local_address, remote_address);
}

template <typename endpointT>
rx_protocol_stack_endpoint* transport_port_impl<endpointT>::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    return construct_endpoint(local_address, remote_address);
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

template <typename endpointT>
rx_protocol_stack_endpoint* transport_port_impl<endpointT>::construct_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    if (!construct_func)
        return nullptr;
    auto endpoint_data = construct_func(local_address, remote_address);

    if (endpoint_data.first->closed_function == nullptr)
    {
        endpoint_data.first->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->unbind_stack_endpoint(entry);
        };
    }
    if (endpoint_data.first->allocate_packet == nullptr)
    {
        endpoint_data.first->allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            auto result = whose->get_port()->alloc_io_buffer();
            if (result)
            {
                result.value().detach(buffer);
                return RX_PROTOCOL_OK;
            }
            else
            {
                return RX_PROTOCOL_OUT_OF_MEMORY;
            }
        };
    }
    if (endpoint_data.first->release_packet == nullptr)
    {
        endpoint_data.first->release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            io::rx_io_buffer temp;
            temp.attach(buffer);
            whose->get_port()->release_io_buffer(std::move(temp));

            return RX_PROTOCOL_OK;
        };
    }
    active_endpoints_.emplace(endpoint_data.first, std::move(endpoint_data.second));
    return endpoint_data.first;
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
