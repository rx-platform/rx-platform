

/****************************************************************************
*
*  system\runtime\rx_protocol_templates.h
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


#ifndef rx_protocol_templates_h
#define rx_protocol_templates_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename endpointT>
class application_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard many<=>many protocol port implementation");

    DECLARE_REFERENCE_PTR(application_port_impl);

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

      rx_protocol_stack_endpoint* create_endpoint ();

      void remove_endpoint (rx_protocol_stack_endpoint* what);

      rx_reference<endpointT> get_endpoint (rx_protocol_stack_endpoint* who);


  protected:

  private:

      virtual rx_reference<endpointT> construct_endpoint () = 0;



      active_endpoints_type active_endpoints_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::application_port_impl 


template <typename endpointT>
rx_protocol_stack_endpoint* application_port_impl<endpointT>::create_endpoint ()
{
    auto endpoint_ptr = construct_endpoint();
    rx_protocol_stack_endpoint* entry = endpoint_ptr->bind_endpoint([this](int64_t count)
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
    return entry;
}

template <typename endpointT>
void application_port_impl<endpointT>::remove_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
        active_endpoints_.erase(it);
}

template <typename endpointT>
rx_reference<endpointT> application_port_impl<endpointT>::get_endpoint (rx_protocol_stack_endpoint* who)
{
    auto it = active_endpoints_.find(who);
    if (it != active_endpoints_.end())
        return it->second;
    else
        return rx_reference<endpointT>();
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
