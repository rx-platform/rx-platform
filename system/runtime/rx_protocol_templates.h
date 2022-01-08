

/****************************************************************************
*
*  system\runtime\rx_protocol_templates.h
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


#ifndef rx_protocol_templates_h
#define rx_protocol_templates_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename endpointT>
class slave_server_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard many<=>many slave/server protocol port implementation");

    DECLARE_REFERENCE_PTR(slave_server_port_impl);

    /*struct sync_port_data : rx_protocol_stack_entry
    {
        std_protocol_impl* whose_;
        rx_protocol_stack_entry* bellow_;
        sync_port_data(std_protocol_impl* whose, rx_protocol_stack_entry* bellow)
            : whose(whose_)
            , bellow_(bellow)
        {
            rx_init_stack_entry(this);
            this->send_function = [](rx_protocol_stack_entry* reference, send_protocol_packet packet)
            {
                rx_protocol_stack_entry* loc_reference = reference;
                rx_packet_buffer buffer;


                whose_->send_io_function([](rx_protocol_stack_entry* reference, send_protocol_packet packet)
                    {
                        rx_protocol_result_t result = rx_move_packet_down(me->bellow_, packet);
                    },
                    reference, entry);
                sync_port_data* me = (sync_port_data*)reference;
                send_protocol_packet addr_packet = packet;
                addr_packet.from = me->local_addr_.to_protocol_address();
                addr_packet.to = me->remote_addr_.to_protocol_address();
                return rx_move_packet_down(me->bellow_, addr_packet);
            };
        }
    };*/

    typedef std::map<rx_protocol_stack_endpoint*, rx_reference<endpointT> > active_endpoints_type;
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint* , rx_reference<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);


  protected:

      rx_reference<endpointT> get_endpoint (rx_protocol_stack_endpoint* stack);


  private:


      active_endpoints_type active_endpoints_;


};






template <typename endpointT>
class master_client_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard many<=>many master/client protocol port implementation");

    DECLARE_REFERENCE_PTR(master_client_port_impl);
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, rx_reference<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_initiator_endpoint ();

      void destroy_endpoint (rx_protocol_stack_endpoint* what);


  protected:

      rx_reference<endpointT> active_endpoint ();


  private:


      rx_reference<endpointT> active_endpoint_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::slave_server_port_impl 


template <typename endpointT>
rx_protocol_stack_endpoint* slave_server_port_impl<endpointT>::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
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
            rx_io_buffer temp;
            temp.attach(buffer);
            whose->get_port()->release_io_buffer(std::move(temp));

            return RX_PROTOCOL_OK;
        };
    }
    active_endpoints_.emplace(endpoint_data.first, std::move(endpoint_data.second));
    return endpoint_data.first;
}

template <typename endpointT>
void slave_server_port_impl<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        active_endpoints_.erase(it);
    }
}

template <typename endpointT>
rx_reference<endpointT> slave_server_port_impl<endpointT>::get_endpoint (rx_protocol_stack_endpoint* stack)
{
    auto it = active_endpoints_.find(stack);
    if (it != active_endpoints_.end())
        return it->second;
    else
        return rx_reference<endpointT>::null_ptr;
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::master_client_port_impl 


template <typename endpointT>
rx_protocol_stack_endpoint* master_client_port_impl<endpointT>::construct_initiator_endpoint ()
{
    if (!construct_func)
        return nullptr;
    auto endpoint_data = construct_func();

    if (endpoint_data.first->closed_function == nullptr)
    {
        endpoint_data.first->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->close_endpoint();
            whose->get_port()->disconnect_stack_endpoint(entry);
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
            rx_io_buffer temp;
            temp.attach(buffer);
            whose->get_port()->release_io_buffer(std::move(temp));

            return RX_PROTOCOL_OK;
        };
    }
    active_endpoint_ = std::move(endpoint_data.second);
    return endpoint_data.first;
}

template <typename endpointT>
void master_client_port_impl<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    if (active_endpoint_)
    {
        active_endpoint_ = rx_reference<endpointT>();
    }
}

template <typename endpointT>
rx_reference<endpointT> master_client_port_impl<endpointT>::active_endpoint ()
{
    return active_endpoint_;
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
