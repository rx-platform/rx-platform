

/****************************************************************************
*
*  system\runtime\rx_ports_templates.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_ports_templates_h
#define rx_ports_templates_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename endpointT>
class extern_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard single endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(extern_port_impl);

    typedef endpointT endpoint_t;
    typedef typename std::unique_ptr<endpointT> endpoint_ptr_t;


  protected:
    typedef std::map<rx_protocol_stack_endpoint*, endpoint_ptr_t> active_endpoints_type;

  public:

      void destroy_endpoint (rx_protocol_stack_endpoint* what);


  protected:

      rx_result add_stack_endpoint (rx_protocol_stack_endpoint* what, std::unique_ptr<endpointT>&& ep, const protocol_address* local_address, const protocol_address* remote_address);


  private:


      active_endpoints_type active_endpoints_;


};






template <typename endpointT>
class extern_singleton_port_impl : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
standard single endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(extern_singleton_port_impl);

    typedef endpointT endpoint_t;
    typedef typename std::unique_ptr<endpointT> endpoint_ptr_t;

  public:

      void destroy_endpoint (rx_protocol_stack_endpoint* what);


  protected:

  private:


      endpoint_ptr_t active_endpoint_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::extern_port_impl 


template <typename endpointT>
rx_result extern_port_impl<endpointT>::add_stack_endpoint (rx_protocol_stack_endpoint* what, std::unique_ptr<endpointT>&& ep, const protocol_address* local_address, const protocol_address* remote_address)
{
    if (what->closed_function == nullptr)
    {
        what->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->unbind_stack_endpoint(entry);
        };
    }
    active_endpoints_.emplace(what, std::move(ep));
    auto result = port_runtime::add_stack_endpoint(what, local_address, remote_address);
    if (!result)
    {
        active_endpoints_.erase(what);
    }
    return result;
}

template <typename endpointT>
void extern_port_impl<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        active_endpoints_.erase(it);// i just might want something to do with it...
    }
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::extern_singleton_port_impl 


template <typename endpointT>
void extern_singleton_port_impl<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    active_endpoint_ = endpoint_ptr_t();
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
