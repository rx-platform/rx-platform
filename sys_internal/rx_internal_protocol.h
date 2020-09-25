

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.h
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


#ifndef rx_internal_protocol_h
#define rx_internal_protocol_h 1



// dummy
#include "dummy.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_subscription
#include "runtime_internal/rx_subscription.h"

namespace rx_internal {
namespace rx_protocol {
class rx_protocol_connection;

} // namespace rx_protocol
} // namespace rx_internal




namespace rx_internal {

namespace rx_protocol {






class rx_json_endpoint 
{
	friend class rx_protocol_connection;

  public:
      rx_json_endpoint();


      rx_result send_string (const string_type& what);

      void bind (rx_protocol_connection_ptr conn, std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



      rx_reference<rx_protocol_connection> connection_;

      rx_protocol_stack_endpoint stack_entry_;


      std::function<void(int64_t)> sent_func_;

      std::function<void(int64_t)> received_func_;


};






class rx_protocol_port : public rx_platform::runtime::items::port_runtime  
{
	DECLARE_CODE_INFO("rx", 0, 2, 0, "\
System protocol port class. Implementation of a rx-platform protocol");

	DECLARE_REFERENCE_PTR(rx_protocol_port);

    typedef std::map<rx_protocol_stack_endpoint*, rx_reference<rx_protocol_connection> > active_endpoints_type;

  public:
      rx_protocol_port();


      void stack_assembled ();

      rx_protocol_stack_endpoint* create_endpoint ();

      void remove_endpoint (rx_protocol_stack_endpoint* what);


  protected:

  private:


      active_endpoints_type active_endpoints_;


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
    typedef std::map<runtime_handle_t, runtime_handle_t> handles_type;

  public:
      rx_protocol_subscription (subscription_data& data, rx_protocol_connection_ptr conn);

      ~rx_protocol_subscription();


      rx_result update_subscription (subscription_data& data);

      void items_changed (const std::vector<update_item>& items);

      void transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items);

      void write_completed (runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_result> > results);

      void destroy ();

      rx_result add_items (const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results);

      rx_result write_items (runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results);

      rx_result remove_items (std::vector<runtime_handle_t >&& items, std::vector<rx_result>& results);


  protected:

  private:


      rx_reference<sys_runtime::subscriptions::rx_subscription> my_subscription_;

      subscription_data data_;

      items_type items_;

      rx_reference<rx_protocol_connection> connection_;


      handles_type handles_;


};






class rx_protocol_connection : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(rx_protocol_connection);


    typedef std::map<rx_uuid, std::unique_ptr<rx_protocol_subscription> > subscriptions_type;

  public:
      rx_protocol_connection();


      void data_received (const string_type& data, rx_packet_id_type packet_id);

      void data_processed (message_ptr result);

      rx_result set_current_directory (const string_type& path);

      rx_result connect_subscription (subscription_data& data);

      rx_result delete_subscription (const rx_uuid& id);

      rx_result update_subscription (subscription_data& data);

      rx_result add_items (const rx_uuid& id, const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results);

      rx_result write_items (const rx_uuid& id, runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results);

      rx_result remove_items (const rx_uuid& id, std::vector<runtime_handle_t>&& items, std::vector<rx_result>& results);

      rx_protocol_stack_endpoint* bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);


      const string_type& get_current_directory_path () const
      {
        return current_directory_path_;
      }


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }



  protected:

  private:


      subscriptions_type subscriptions_;

      rx_json_endpoint endpoint_;


      rx_directory_ptr current_directory_;

      string_type current_directory_path_;

      rx_thread_handle_t executer_;


};


} // namespace rx_protocol
} // namespace rx_internal



#endif
