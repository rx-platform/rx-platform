

/****************************************************************************
*
*  system\runtime\rx_port_stack_active.h
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


#ifndef rx_port_stack_active_h
#define rx_port_stack_active_h 1



// rx_port_instance
#include "system/runtime/rx_port_instance.h"
// rx_port_stack_data
#include "system/runtime/rx_port_stack_data.h"



namespace rx_platform {

namespace runtime {

namespace io_types {






class active_builder 
{

  public:

      static rx_result bind_stack_endpoint (rx_port_ptr who, rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr);


  protected:

  private:


};






class extern_behavior : public port_active_behavior  
{

  public:

      bool is_extern ();

      bool is_endpoit_binder ();


  protected:

  private:


};






class passive_transport_behavior : public port_active_behavior  
{

  public:

      bool is_extern ();

      bool is_endpoit_binder ();


  protected:

  private:


};






class active_transport_behavior : public port_active_behavior  
{

  public:

      bool is_extern ();

      bool is_endpoit_binder ();


  protected:

  private:


};






class application_behavior : public port_active_behavior  
{

  public:

      bool is_extern ();

      bool is_endpoit_binder ();


  protected:

  private:


};


} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
