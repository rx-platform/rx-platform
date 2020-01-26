

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



// rx_runtime_instance
#include "system/runtime/rx_runtime_instance.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// dummy
#include "dummy.h"

#include "rx_runtime_helpers.h"


namespace rx_platform {

namespace runtime {

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

      void process_stack ();


  protected:

      void update_received_counters (size_t count);

      void update_sent_counters (size_t count);

      void update_received_packets (size_t count);

      void update_sent_packets (size_t count);

      void update_connected_status (bool status);


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

      rx_port_impl_ptr up_stack () const;

      rx_port_impl_ptr down_stack () const;

      void process_stack ();


  protected:

      void update_received_counters (size_t count);

      void update_sent_counters (size_t count);

      void update_received_packets (size_t count);

      void update_sent_packets (size_t count);


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

      void process_stack ();


  protected:

      void update_received_counters (size_t count);

      void update_sent_counters (size_t count);

      void update_received_packets (size_t count);

      void update_sent_packets (size_t count);


  private:

      bool has_up_port () const;



      rx_reference<items::port_runtime> next_up_;

      rx_reference<items::port_runtime> next_down_;


      runtime_handle_t rx_bytes_item_;

      runtime_handle_t tx_bytes_item_;

      runtime_handle_t rx_packets_item_;

      runtime_handle_t tx_packets_item_;


};


} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
