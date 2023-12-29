

/****************************************************************************
*
*  sys_internal\rx_subscription_items.h
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


#ifndef rx_subscription_items_h
#define rx_subscription_items_h 1



// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"



namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace items_messages {





class remove_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;

      std::vector<runtime_handle_t> items;


  protected:

  private:


};






class execute_item_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;

      runtime_transaction_id_t transaction_id;

      runtime_handle_t item;

      data::runtime_values_data data;


  protected:

  private:


};






class write_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;

      runtime_transaction_id_t transaction_id;

      std::vector<std::pair<runtime_handle_t, rx_simple_value> > values;


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






class read_items_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class add_item_data 
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


      bool active;

      string_type path;

      runtime_handle_t client_handle;

      subscription_trigger_type trigger_type;


  protected:

  private:


};






class add_items_request : public rx_request_message  
{
	typedef std::vector<add_item_data> items_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      items_type items;


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class modify_item_data : public add_item_data  
{

  public:

      runtime_handle_t handle;


  protected:

  private:


};






class modify_items_request : public rx_request_message  
{
	typedef std::vector<modify_item_data> items_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      items_type items;


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class add_item_result_data 
{

  public:
      add_item_result_data (rx_result_with<runtime_handle_t>&& result);


      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


      runtime_handle_t handle;

      uint32_t error_code;

      string_type error_text;

	  add_item_result_data() = default;
	  add_item_result_data(const add_item_result_data&) = default;
	  add_item_result_data(add_item_result_data&&) = default;
  protected:

  private:


};






class add_items_response : public rx_message_base  
{
	typedef std::vector<add_item_result_data> results_type;

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


      results_type results;


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;


  protected:

  private:


};






class item_result_data 
{

  public:
      item_result_data (rx_result&& result);


      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


      uint32_t error_code;

      string_type error_text;

	  item_result_data() = default;
	  item_result_data(const item_result_data&) = default;
	  item_result_data(item_result_data&&) = default;
  protected:

  private:


};






class subscription_items_response : public rx_message_base  
{
	typedef std::vector<item_result_data> results_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


      results_type results;


      rx_uuid subscription_id;


  protected:

  private:


};






class remove_items_response : public subscription_items_response  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class write_items_response : public subscription_items_response  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class modify_items_response : public subscription_items_response  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class read_items_response : public subscription_items_response  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};


} // namespace items_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal



#endif
