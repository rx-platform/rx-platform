

/****************************************************************************
*
*  sys_internal\rx_internal_subscription.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_internal_subscription_h
#define rx_internal_subscription_h 1



// rx_subscription_items
#include "sys_internal/rx_subscription_items.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"

#include "system/runtime/rx_runtime_helpers.h"
#include "system/runtime/rx_runtime_holder.h"
using rx_platform::runtime::update_item;


namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace subscription_messages {





class create_subscription_request : public rx_request_message  
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

      uint32_t publish_rate;

      uint32_t keep_alive_period;

      bool active;

      uint8_t priority;


  protected:

  private:


};






class create_subscriptions_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;

      uint32_t revised_publish_rate;

      uint32_t revised_keep_alive_period;


  protected:

  private:


};






class delete_subscription_request : public rx_request_message  
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






class update_subscription_request : public rx_request_message  
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

      uint32_t publish_rate;

      uint32_t keep_alive_period;

      bool active;

      uint8_t priority;


  protected:

  private:


};






class delete_subscription_response : public rx_message_base  
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






class update_subscription_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;

      uint32_t revised_publish_rate;

      uint32_t revised_keep_alive_period;


  protected:

  private:


};






class subscription_items_change : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;

      std::vector<update_item> items;


  protected:

  private:


};






class subscription_write_done : public rx_message_base  
{
    typedef std::tuple<runtime_handle_t, uint32_t, string_type> result_type;
    typedef std::vector<result_type> results_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();

      void add_result (runtime_handle_t handle, rx_result&& result);


      results_type results;


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_uuid subscription_id;

      runtime_transaction_id_t transaction_id;


  protected:

  private:


};


} // namespace subscription_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal



#endif
