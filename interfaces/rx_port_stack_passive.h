

/****************************************************************************
*
*  interfaces\rx_port_stack_passive.h
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


#ifndef rx_port_stack_passive_h
#define rx_port_stack_passive_h 1



// rx_port_stack_data
#include "interfaces/rx_port_stack_data.h"
// rx_port_instance
#include "interfaces/rx_port_instance.h"



namespace rx_internal {

namespace interfaces {

namespace port_stack {

namespace stack_passive {






class passive_builder 
{

  public:

      static rx_result send_listen (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr);

      static rx_result send_connect (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr);

      static rx_result unbind_passive (rx_port_ptr who);


  protected:

  private:

      static rx_result send_connect_down_recursive (rx_port_ptr who, rx_protocol_stack_endpoint* ep, io::any_address& local_addr, io::any_address remote_addr, std::pair<rx_protocol_stack_endpoint*, rx_session*>& connected);

      static rx_result send_unbind_up_recursive (rx_port_ptr who);



};






class listen_sender : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class connect_sender : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class passive_ignorant : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class listen_subscriber : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class connect_subscriber : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class listen_connect_subscriber : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class server_master_router : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class client_slave_router : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class full_router : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};






class listen_connect_sender : public port_passive_behavior  
{

  public:

      bool is_listen_sender ();

      bool is_connect_sender ();

      bool is_listen_subscriber ();

      bool is_connect_subscriber ();


  protected:

  private:


};


} // namespace stack_passive
} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal



#endif
