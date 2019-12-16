

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.h
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


#ifndef rx_internal_protocol_h
#define rx_internal_protocol_h 1



// rx_port_types
#include "system/runtime/rx_port_types.h"
// dummy
#include "dummy.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"
// rx_subscription
#include "runtime_internal/rx_subscription.h"

namespace sys_internal {
namespace rx_protocol {
class rx_protocol_port;

} // namespace rx_protocol
} // namespace sys_internal




namespace sys_internal {

namespace rx_protocol {





class rx_json_protocol : public rx_protocol_stack_entry  
{
	friend class rx_protocol_port;

  public:
      rx_json_protocol();


      rx_result send_string (const string_type& what);


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_const_packet_buffer* buffer);



      rx_reference<rx_protocol_port> my_port_;


};






struct subscription_data 
{


      rx_uuid subscription_id;

      uint32_t publish_rate;

      uint32_t keep_alive_period;

      bool active;

      uint8_t priority;

  public:

  protected:

  private:


};






struct subscription_item_data 
{


      bool active;

      string_type path;

      runtime_handle_t client_handle;

      runtime_handle_t local_handle;

      subscription_trigger_type trigger_type;

  public:

  protected:

  private:


};






class rx_protocol_subscription : public sys_runtime::subscriptions::rx_subscription_callback  
{
	typedef std::map<runtime_handle_t, subscription_item_data> items_type;

  public:
      rx_protocol_subscription (subscription_data& data, rx_reference<rx_protocol_port> port);

      ~rx_protocol_subscription();


      rx_result update_subscription (subscription_data& data);

      void items_changed (const std::vector<update_item>& items);

      void transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items);

      void destroy ();

      rx_result add_items (const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results);


  protected:

  private:


      rx_reference<sys_runtime::subscriptions::rx_subscription> my_subscription_;

      subscription_data data_;

      rx_reference<rx_protocol_port> my_port_;

      items_type items_;


};







class rx_protocol_port : public rx_platform::runtime::io_types::protocol_port  
{
	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
system protocol port class. basic implementation of a rx-platform protocol");

	DECLARE_REFERENCE_PTR(rx_protocol_port);

	typedef std::map<rx_uuid, std::unique_ptr<rx_protocol_subscription> > subscriptions_type;

  public:
      rx_protocol_port();


      void data_received (const string_type& data);

      rx_protocol_stack_entry* get_stack_entry ();

      void data_processed (message_ptr result);

      rx_result set_current_directory (const string_type& path);

      rx_result connect_subscription (subscription_data& data);

      rx_result delete_subscription (const rx_uuid& id);

      rx_result update_subscription (subscription_data& data);

      rx_result add_items (const rx_uuid& id, const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results);


      const string_type& get_current_directory_path () const
      {
        return current_directory_path_;
      }



  protected:

  private:


      rx_json_protocol stack_entry_;

      subscriptions_type subscriptions_;


      rx_directory_ptr current_directory_;

      string_type current_directory_path_;


};


} // namespace rx_protocol
} // namespace sys_internal



#endif
