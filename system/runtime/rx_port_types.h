

/****************************************************************************
*
*  system\runtime\rx_port_types.h
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


#ifndef rx_port_types_h
#define rx_port_types_h 1



// dummy
#include "dummy.h"
// rx_runtime_instance
#include "system/runtime/rx_runtime_instance.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"

#include "rx_runtime_helpers.h"


namespace rx_platform {

namespace runtime {
using items::port_instance_data;

namespace io_types {






class physical_port : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 0, 2, "\
physical port class. basic implementation of a physical port");

    DECLARE_REFERENCE_PTR(physical_port);

  public:
      physical_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_port_impl_ptr up_stack () const;

      rx_port_impl_ptr down_stack () const;

      void connect_up_stack (rx_port_impl_ptr who);

      void connect_down_stack (rx_port_impl_ptr who);


  protected:

      void update_received_counters (size_t count);

      void update_sent_counters (size_t count);

      void update_received_packets (size_t count);

      void update_sent_packets (size_t count);

      void update_connected_status (bool status);

      void structure_changed ();


  private:

      bool has_up_port () const;



      rx_protocol_stack_entry *my_endpoints_;

      rx_reference<items::port_runtime> next_up_;


      runtime_handle_t rx_bytes_item_;

      runtime_handle_t tx_bytes_item_;

      runtime_handle_t rx_packets_item_;

      runtime_handle_t tx_packets_item_;

      runtime_handle_t connected_item_;


};







class protocol_port : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 0, 2, "\
protocol port class. basic implementation of a protocol port");

    DECLARE_REFERENCE_PTR(protocol_port);

  public:
      protocol_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      void update_received_counters (size_t count);

      void update_sent_counters (size_t count);

      rx_port_impl_ptr up_stack () const;

      rx_port_impl_ptr down_stack () const;

      void connect_up_stack (rx_port_impl_ptr who);

      void connect_down_stack (rx_port_impl_ptr who);


  protected:

      void update_received_packets (size_t count);

      void update_sent_packets (size_t count);

      void structure_changed ();


  private:

      bool has_up_port () const;



      rx_reference<items::port_runtime> next_down_;


      runtime_handle_t rx_bytes_item_;

      runtime_handle_t tx_bytes_item_;

      runtime_handle_t rx_packets_item_;

      runtime_handle_t tx_packets_item_;


};







class transport_port : public items::port_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 0, 2, "\
transport port class. basic implementation of a transport port");

    DECLARE_REFERENCE_PTR(transport_port);

  public:
      transport_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_port_impl_ptr up_stack () const;

      rx_port_impl_ptr down_stack () const;

      void connect_up_stack (rx_port_impl_ptr who);

      void connect_down_stack (rx_port_impl_ptr who);


  protected:

      void update_received_counters (size_t count);

      void update_sent_counters (size_t count);

      void update_received_packets (size_t count);

      void update_sent_packets (size_t count);

      void structure_changed ();


  private:

      bool has_up_port () const;



      rx_reference<items::port_runtime> next_up_;

      rx_reference<items::port_runtime> next_down_;


      runtime_handle_t rx_bytes_item_;

      runtime_handle_t tx_bytes_item_;

      runtime_handle_t rx_packets_item_;

      runtime_handle_t tx_packets_item_;


};






template <typename endpointT>
class std_transport_impl : public transport_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard one<=>many transport port implementation");

    DECLARE_REFERENCE_PTR(transport_port);

    typedef std::map<rx_protocol_stack_entry*, std::unique_ptr<endpointT> > endpoints_type;

  public:

      rx_protocol_stack_entry* create_stack_entry ();


  protected:

      void structure_changed ();


  private:


      endpoints_type endpoints_;


};






template <typename endpointT>
class physical_single_port_impl : public physical_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard single endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(physical_single_port_impl);

  public:

  protected:

      void structure_changed ();


  private:

      virtual rx_protocol_stack_entry* get_stack_entry () = 0;



};






template <typename endpointT>
class physical_multiple_port_impl : public physical_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard multiple endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(physical_multiple_port_impl);

    typedef std::map<rx_protocol_stack_entry*, std::unique_ptr<endpointT> > connections_type;

  public:

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void remove_connection ( rx_protocol_stack_entry* what);


  protected:

      void structure_changed ();

      rx_protocol_stack_entry* register_stack_entry (std::unique_ptr<endpointT>&& what);


  private:


      connections_type connections_;


};






template <typename endpointT>
class std_protocol_impl : public protocol_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard many<=>many protocol port implementation");

    DECLARE_REFERENCE_PTR(std_protocol_impl);

    typedef std::map<rx_protocol_stack_entry*, endpointT> endpoints_type;

  public:

      rx_protocol_stack_entry* create_stack_entry ();


  protected:

  private:

      virtual endpointT create_endpoint () = 0;



      endpoints_type endpoints_;


};


// Parameterized Class rx_platform::runtime::io_types::std_transport_impl 


template <typename endpointT>
void std_transport_impl<endpointT>::structure_changed ()
{
    auto up = up_stack();
    if (up)
    {
        for (auto& one : endpoints_)
        {
            rx_protocol_result_t res = rx_push_stack(one.first, up->create_stack_entry());
        }
    }
}

template <typename endpointT>
rx_protocol_stack_entry* std_transport_impl<endpointT>::create_stack_entry ()
{
    auto endpoint_ptr = std::make_unique<endpointT>();
    rx_protocol_stack_entry* entry = endpoint_ptr->bind([this](int64_t count)
        {
            update_sent_counters(count);
        },
        [this](int64_t count)
        {
            update_received_counters(count);
        });
    endpoints_.emplace(entry, std::move(endpoint_ptr));

    auto up = up_stack();
    if (up)
    {
        rx_protocol_result_t res = rx_push_stack(entry, up->create_stack_entry());
    }

    return entry;
}


// Parameterized Class rx_platform::runtime::io_types::physical_single_port_impl 


template <typename endpointT>
void physical_single_port_impl<endpointT>::structure_changed ()
{
    auto up = up_stack();
    auto my_entry = this->get_stack_entry();
    if (up && my_entry)
    {
        my_entry->identity = get_identity();
        rx_protocol_result_t res = rx_push_stack(my_entry, up->create_stack_entry());
    }
}


// Parameterized Class rx_platform::runtime::io_types::physical_multiple_port_impl 


template <typename endpointT>
void physical_multiple_port_impl<endpointT>::structure_changed ()
{
    auto up = up_stack();
    if (up)
    {
        for (auto& one : connections_)
        {
            rx_protocol_result_t res = rx_push_stack(one.first, up->create_stack_entry());
            if (res == RX_PROTOCOL_OK)
            {

            }
            else
            {

            }
        }
    }
}

template <typename endpointT>
rx_protocol_stack_entry* physical_multiple_port_impl<endpointT>::register_stack_entry (std::unique_ptr<endpointT>&& what)
{
    rx_protocol_stack_entry* entry = what.get();
    entry->identity = get_identity();
    connections_.emplace(entry, std::move(what));

    auto up = up_stack();
    if (up)
    {
        rx_protocol_result_t res = rx_push_stack(entry, up->create_stack_entry());
        if (res == RX_PROTOCOL_OK)
        {
            security::secured_scope ctx(entry->identity);
            rx_send_connected(entry);
        }
    }
    return entry;
}

template <typename endpointT>
rx_result physical_multiple_port_impl<endpointT>::stop_runtime (runtime::runtime_stop_context& ctx)
{
    for (auto& one : connections_)
    {
        one.second->close();
    }
    connections_.clear();
    return true;
}

template <typename endpointT>
void physical_multiple_port_impl<endpointT>::remove_connection ( rx_protocol_stack_entry* what)
{
    auto it = connections_.find(what);
    if (it != connections_.end())
    {
        connections_.erase(it);
    }
}


// Parameterized Class rx_platform::runtime::io_types::std_protocol_impl 


template <typename endpointT>
rx_protocol_stack_entry* std_protocol_impl<endpointT>::create_stack_entry ()
{
    auto endpoint = this->create_endpoint();
    rx_protocol_stack_entry* entry = endpoint->bind_endpoint([this](int64_t count)
        {
            update_sent_counters(count);
        },
        [this](int64_t count)
        {
            update_received_counters(count);
        });
    entry->identity = get_identity();
    endpoints_.emplace(entry, std::move(endpoint));
    
    return entry;
}


} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
