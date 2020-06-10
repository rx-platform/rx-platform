

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



// rx_port_types
#include "system/runtime/rx_port_types.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {





template <typename endpointT>
class physical_single_port_impl : public physical_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard single endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(physical_single_port_impl);

  public:

      rx_result push (rx_port_impl_ptr who, const meta::meta_data& info);


  protected:

  private:

      virtual rx_protocol_stack_entry* get_stack_entry () = 0;



      rx_reference<items::port_runtime> up_stack_;


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






template <typename endpointT, typename addrT>
class physical_multiple_port_impl : public physical_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard multiple endpoint transport port implementation");

    DECLARE_REFERENCE_PTR(physical_multiple_port_impl);


    typedef std::map<rx_protocol_stack_entry*, std::unique_ptr<endpointT> > connections_type;
    typedef std::map<addrT, rx_port_impl_ptr> up_stack_type;

  public:

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void remove_connection ( rx_protocol_stack_entry* what);

      rx_result push (rx_port_impl_ptr who, const meta::meta_data& info);


  protected:

      rx_protocol_stack_entry* register_stack_entry (std::unique_ptr<endpointT>&& what, const addrT& addr);


  private:


      up_stack_type up_stack_;

      rx_reference<items::port_runtime> default_up_;


      connections_type connections_;


};






template <typename endpointT>
class transport_port_impl : public transport_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard one<=>many transport port implementation");

    DECLARE_REFERENCE_PTR(transport_port_impl);

    typedef std::map<rx_protocol_stack_entry*, std::unique_ptr<endpointT> > endpoints_type;

  public:

      rx_protocol_stack_entry* create_stack_entry ();

      rx_result push (rx_port_impl_ptr who, const meta::meta_data& info);


  protected:

  private:


      rx_reference<items::port_runtime> up_stack_;


      endpoints_type endpoints_;


};






template <typename endpointT, typename addrT>
class addressable_transport_impl : public transport_port  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
standard one<=>many addressable transport port implementation");

    DECLARE_REFERENCE_PTR(addressable_transport_impl);

    typedef std::map<rx_protocol_stack_entry*, std::unique_ptr<endpointT> > endpoints_type;
    typedef std::map<addrT, rx_port_impl_ptr> up_stack_type;

  public:

      rx_protocol_stack_entry* create_stack_entry ();

      rx_result push (rx_port_impl_ptr who, const meta::meta_data& info);


  protected:

      void structure_changed ();


  private:


      up_stack_type up_stack_;

      rx_reference<items::port_runtime> default_up_;


      endpoints_type endpoints_;


};


// Parameterized Class rx_platform::runtime::io_types::ports_templates::physical_single_port_impl 


template <typename endpointT>
rx_result physical_single_port_impl<endpointT>::push (rx_port_impl_ptr who, const meta::meta_data& info)
{
    if(up_stack_)
        return "Already connected.";
    up_stack_ = who;
    auto my_entry = this->get_stack_entry();
    if (my_entry)
    {
        my_entry->identity = get_identity();
        rx_protocol_result_t res = rx_push_stack(my_entry, up_stack_->create_stack_entry());
    }
    return true;
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::std_protocol_impl 


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


// Parameterized Class rx_platform::runtime::io_types::ports_templates::physical_multiple_port_impl 


template <typename endpointT, typename addrT>
rx_protocol_stack_entry* physical_multiple_port_impl<endpointT,addrT>::register_stack_entry (std::unique_ptr<endpointT>&& what, const addrT& addr)
{
    rx_protocol_stack_entry* entry = nullptr;
    static addrT g_null_addr;
    rx_port_impl_ptr up;

    auto it_port = up_stack_.find(addr);
    if (it_port != up_stack_.end())
        up = it_port->second;
    else
        up = default_up_;
    if (up)
    {
        entry = what.get();
        entry->identity = get_identity();
        connections_.emplace(entry, std::move(what));

        rx_protocol_result_t res = rx_push_stack(entry, up->create_stack_entry());
        if (res == RX_PROTOCOL_OK)
        {
            security::secured_scope ctx(entry->identity);
            rx_send_connected(entry);
        }
    }
    return entry;
}

template <typename endpointT, typename addrT>
rx_result physical_multiple_port_impl<endpointT,addrT>::stop_runtime (runtime::runtime_stop_context& ctx)
{
    for (auto& one : connections_)
    {
        one.second->close();
    }
    connections_.clear();
    return true;
}

template <typename endpointT, typename addrT>
void physical_multiple_port_impl<endpointT,addrT>::remove_connection ( rx_protocol_stack_entry* what)
{
    auto it = connections_.find(what);
    if (it != connections_.end())
    {
        connections_.erase(it);
    }
}

template <typename endpointT, typename addrT>
rx_result physical_multiple_port_impl<endpointT,addrT>::push (rx_port_impl_ptr who, const meta::meta_data& info)
{
    auto ep = who->get_address();
    addrT temp;
    auto result = temp.parse(ep);
    if(result)
    {
        if (temp.is_null())
        {
            if (default_up_)
                result = "Default address already connected.";
            default_up_ = who;
        }
        else
        {
            auto it_port = up_stack_.find(temp);
            if (it_port != up_stack_.end())
                result = "Address "s + temp.to_string() + " already connected.";
            else
                up_stack_.emplace(temp, who);
        }
    }
    return result;
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::transport_port_impl 


template <typename endpointT>
rx_protocol_stack_entry* transport_port_impl<endpointT>::create_stack_entry ()
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

    auto up = up_stack_;
    if (up)
    {
        rx_protocol_result_t res = rx_push_stack(entry, up->create_stack_entry());
    }

    return entry;
}

template <typename endpointT>
rx_result transport_port_impl<endpointT>::push (rx_port_impl_ptr who, const meta::meta_data& info)
{
    if(up_stack_)
        return "Already connected.";
    up_stack_ = who;
    for (auto& one : endpoints_)
    {
        rx_protocol_result_t res = rx_push_stack(one.first, up_stack_->create_stack_entry());
    }
    return true;
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::addressable_transport_impl 


template <typename endpointT, typename addrT>
void addressable_transport_impl<endpointT,addrT>::structure_changed ()
{
}

template <typename endpointT, typename addrT>
rx_protocol_stack_entry* addressable_transport_impl<endpointT,addrT>::create_stack_entry ()
{
    rx_protocol_stack_entry* entry = nullptr;
    static addrT g_null_addr;
    rx_port_impl_ptr up;

/*    auto it_port = up_stack_.find(addr);
    if (it_port != up_stack_.end())
        up = it_port->second;
    else
        up = default_up_;
    if (up)
    {
        entry = what.get();
        entry->identity = get_identity();
        connections_.emplace(entry, std::move(what));

        rx_protocol_result_t res = rx_push_stack(entry, up->create_stack_entry());
        if (res == RX_PROTOCOL_OK)
        {
            security::secured_scope ctx(entry->identity);
            rx_send_connected(entry);
        }
    }*/
    return entry;
}

template <typename endpointT, typename addrT>
rx_result addressable_transport_impl<endpointT,addrT>::push (rx_port_impl_ptr who, const meta::meta_data& info)
{
    auto ep = who->get_address();
    addrT temp;
    auto result = temp.parse(ep);
    if (result)
    {
        if (temp.is_null())
        {
            if (default_up_)
                result = "Default address already connected.";
            default_up_ = who;
        }
        else
        {
            auto it_port = up_stack_.find(temp);
            if (it_port != up_stack_.end())
                result = "Address "s + temp.to_string() + " already connected.";
            else
                up_stack_.emplace(temp, who);
        }
    }
    return result;
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
