

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.h
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


#ifndef rx_internal_protocol_h
#define rx_internal_protocol_h 1



// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// dummy
#include "dummy.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"
// rx_subscription
#include "runtime_internal/rx_subscription.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_internal {
namespace rx_protocol {
class rx_protocol_connection;
class rx_json_protocol_port;
class rx_json_protocol_client_port;

} // namespace rx_protocol
} // namespace rx_internal



/////////////////////////////////////////////////////////////
// logging macros for opcua library
#define RXCOMM_LOG_INFO(src,lvl,msg) RX_LOG_INFO("RXCOMM",src,lvl,msg)
#define RXCOMM_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("RXCOMM",src,lvl,msg)
#define RXCOMM_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("RXCOMM",src,lvl,msg)
#define RXCOMM_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("RXCOMM",src,lvl,msg)
#define RXCOMM_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("RXCOMM",src,lvl,msg)
#define RXCOMM_LOG_TRACE(src,lvl,msg) RX_TRACE("RXCOMM",src,lvl,msg)


namespace rx_internal {

namespace rx_protocol {
typedef rx_reference<rx_protocol_connection> rx_protocol_connection_ptr;





class rx_protocol_client_connection : public rx::pointers::reference_object  
{
    typedef std::map<messages::rx_request_id_t, messages::rx_transaction_ptr> pending_transactions_type;
    typedef std::vector<messages::rx_transaction_ptr> queued_transactions_type;

  public:
      rx_protocol_client_connection();

      ~rx_protocol_client_connection();


      void transaction_received (messages::rx_transaction_ptr response);

      void set_context (api::rx_context ctx, const messages::rx_connection_context_response& req);

      rx_result send_request (messages::rx_transaction_ptr trans, uint32_t timeout);

      virtual void timer_tick ();


  protected:

      void register_transaction (messages::rx_request_id_t id, messages::rx_transaction_ptr trans);

      messages::rx_transaction_ptr get_transaction (messages::rx_request_id_t id);

      virtual void notify_connected () = 0;

      virtual void notify_disconnected () = 0;

      virtual void notify_item_change (const rx_node_id& id, const string_type& path) = 0;


      static std::atomic<messages::rx_request_id_t> g_next_request_id;


  private:

      virtual void send_message (messages::rx_message_base& msg) = 0;



      pending_transactions_type pending_transactions_;


      locks::slim_lock transactions_lock_;


};







class rx_client_connection : public rx_protocol_client_connection  
{
    DECLARE_REFERENCE_PTR(rx_client_connection);

    enum class rx_client_connection_state
    {
        rx_client_connection_idle,
        rx_client_connection_connecting,
        rx_client_connection_active,
        rx_client_connection_disconnected
    };

  public:
      rx_client_connection (rx_json_protocol_client_port* port);

      ~rx_client_connection();


      void close_endpoint ();

      rx_protocol_stack_endpoint* get_endpoint ();

      rx_protocol_stack_endpoint* bind_endpoint ();

      void timer_tick ();


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }


      rx_json_protocol_client_port* get_port ()
      {
        return port_;
      }


      uint32_t get_stream_version () const
      {
        return stream_version_;
      }



  protected:

      void notify_connected ();

      void notify_disconnected ();

      void notify_item_change (const rx_node_id& id, const string_type& path);


  private:

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      static rx_protocol_result_t disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      void send_message (messages::rx_message_base& msg);

      rx_protocol_result_t received (recv_protocol_packet packet);



      rx_protocol_stack_endpoint stack_entry_;


      rx_thread_handle_t executer_;

      rx_json_protocol_client_port* port_;

      uint32_t stream_version_;

      locks::slim_lock state_lock_;

      rx_timer_ticks_t last_sent_;


};







typedef rx_platform::runtime::io_types::ports_templates::master_client_port_impl< rx_client_connection  > rx_json_client_protocol_port_base;






class rx_protocol_client_user : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(rx_protocol_client_user);

  public:

      virtual void client_connected () = 0;

      virtual void client_disconnected () = 0;

      virtual void item_changed (const rx_node_id& id, const string_type& path) = 0;


  protected:

  private:


};






class rx_json_protocol_client_port : public rx_json_client_protocol_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 2, 0, "\
System protocol client port class. Implementation of a rx-platform JSON client protocol");

    DECLARE_REFERENCE_PTR(rx_json_protocol_client_port);


  public:
      rx_json_protocol_client_port();


      void stack_assembled ();

      rx_result send_request (messages::rx_transaction_ptr trans, uint32_t timeout);

      void register_user (rx_protocol_client_user::smart_ptr user);

      void unregister_user (rx_protocol_client_user::smart_ptr user);

      void notify_connected ();

      void notify_disconnected ();

      void notify_item_change (const rx_node_id& id, const string_type& path);

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);


      rx_platform::runtime::io_types::client_port_status status;


      static std::map<rx_node_id,rx_json_protocol_client_port::smart_ptr> runtime_instances;


  protected:

  private:


      rx_reference<rx_protocol_client_user> user_;


      rx_timer_ptr timer_;


};






class rx_local_subscription 
{

  public:

  protected:

  private:


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
      rx_protocol_subscription (subscription_data& data, rx_protocol_connection_ptr conn, rx_mode_type mode);

      ~rx_protocol_subscription();


      rx_result update_subscription (subscription_data& data);

      void items_changed (const std::vector<update_item>& items);

      void execute_completed (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, values::rx_simple_value data);

      void execute_completed (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, data::runtime_values_data data);

      void write_completed (runtime_transaction_id_t transaction_id, std::vector<write_result_item> results);

      void destroy ();

      rx_result add_items (const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results);

      rx_result write_items (runtime_transaction_id_t transaction_id, bool test, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results);

      rx_result execute_item (runtime_transaction_id_t transaction_id, runtime_handle_t handle, data::runtime_values_data data);

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
      rx_protocol_connection (rx_mode_type mode);

      ~rx_protocol_connection();


      void data_processed (message_ptr result);

      void request_received (request_message_ptr&& request);

      void response_received (response_message_ptr&& response);

      rx_result set_current_directory (const string_type& path);

      rx_result connect_subscription (subscription_data& data);

      rx_result delete_subscription (const rx_uuid& id);

      rx_result update_subscription (subscription_data& data);

      rx_result add_items (const rx_uuid& id, const std::vector<subscription_item_data>& items, std::vector<rx_result_with<runtime_handle_t> >& results);

      rx_result write_items (const rx_uuid& id, runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& results);

      rx_result execute_item (const rx_uuid& id, runtime_transaction_id_t transaction_id, runtime_handle_t item, data::runtime_values_data data);

      rx_result remove_items (const rx_uuid& id, std::vector<runtime_handle_t>&& items, std::vector<rx_result>& results);

      void close_connection ();

      virtual message_ptr set_context (api::rx_context ctx, const messages::rx_connection_context_request& req);


      const string_type& get_current_directory_path () const
      {
        return current_directory_path_;
      }



  protected:

  private:

      virtual void send_message (message_ptr msg) = 0;



      subscriptions_type subscriptions_;


      rx_directory_ptr current_directory_;

      string_type current_directory_path_;

      rx_mode_type mode_;


};







class rx_server_connection : public rx_protocol_connection  
{
    DECLARE_REFERENCE_PTR(rx_server_connection);

  public:
      rx_server_connection (rx_json_protocol_port* port);

      ~rx_server_connection();


      rx_protocol_stack_endpoint* bind_endpoint ();

      void close_endpoint ();

      message_ptr set_context (api::rx_context ctx, const messages::rx_connection_context_request& req);

      rx_result request_stream_version (uint32_t sversion);


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }


      rx_json_protocol_port* get_port ()
      {
        return port_;
      }


      uint32_t get_stream_version () const
      {
        return stream_version_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      rx_protocol_result_t received (recv_protocol_packet packet);

      void send_message (message_ptr msg);



      rx_protocol_stack_endpoint stack_entry_;


      rx_thread_handle_t executer_;

      rx_json_protocol_port* port_;

      uint32_t stream_version_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< rx_server_connection  > rx_json_server_protocol_port_base;






class rx_json_protocol_port : public rx_json_server_protocol_port_base  
{
	DECLARE_CODE_INFO("rx", 0, 2, 0, "\
System protocol port class. Implementation of a rx-platform JSON protocol");

	DECLARE_REFERENCE_PTR(rx_json_protocol_port);


  public:
      rx_json_protocol_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime_init_context& ctx);


      rx_platform::runtime::io_types::simple_port_status status;


  protected:

  private:


};


} // namespace rx_protocol
} // namespace rx_internal



#endif
