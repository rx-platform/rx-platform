

/****************************************************************************
*
*  interfaces\rx_port_stack_construction.h
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


#ifndef rx_port_stack_construction_h
#define rx_port_stack_construction_h 1



// rx_port_instance
#include "interfaces/rx_port_instance.h"

namespace rx_internal {
namespace interfaces {
namespace port_stack {
class port_stack_data;

} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal


#include "lib/rx_io_addr.h"
#include "system/runtime/rx_objbase.h"


namespace rx_internal {

namespace interfaces {

namespace port_stack {

namespace stack_build {





class assemble_sender : public port_build_behavior  
{

  public:

      bool is_assemble_sender ();

      bool is_assemble_subscriber ();

      bool is_external ();

      bool is_application ();


  protected:

  private:


};






class assemble_ignorant : public port_build_behavior  
{

  public:

      bool is_assemble_sender ();

      bool is_assemble_subscriber ();

      bool is_external ();

      bool is_application ();


  protected:

  private:


};






class assemble_subscriber : public port_build_behavior  
{

  public:

      bool is_assemble_sender ();

      bool is_assemble_subscriber ();

      bool is_external ();

      bool is_application ();


  protected:

  private:


};






class assemble_sender_subscriber : public port_build_behavior  
{

  public:

      bool is_assemble_sender ();

      bool is_assemble_subscriber ();

      bool is_external ();

      bool is_application ();


  protected:

  private:


};







class stack_builder 
{

  public:

      static rx_result connect_stack_top (rx_port_ptr top, rx_port_ptr who);

      static rx_result disconnect_stack (rx_port_ptr who);


  protected:

  private:

      static void recursive_send_stack_assembled (rx_port_ptr who);

      static void recursive_send_stack_disassembled (rx_port_ptr who);

      static void recursive_get_stack (rx_port_ptr top, std::vector<rx_port_ptr>& stack);



};


} // namespace stack_build
} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal



#endif
