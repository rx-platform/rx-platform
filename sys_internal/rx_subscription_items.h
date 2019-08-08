

/****************************************************************************
*
*  sys_internal\rx_subscription_items.h
*
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


#ifndef rx_subscription_items_h
#define rx_subscription_items_h 1



// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"



namespace sys_internal {

namespace rx_protocol {

namespace messages {

namespace items_messages {





class modify_items_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class write_items_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class execute_item_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class remove_items_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class add_items_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class add_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class modify_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class remove_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class write_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class execute_item_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class read_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class read_items_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};


} // namespace items_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal



#endif
