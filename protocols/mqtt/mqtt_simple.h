

/****************************************************************************
*
*  protocols\mqtt\mqtt_simple.h
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


#ifndef mqtt_simple_h
#define mqtt_simple_h 1



// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_blocks_templates
#include "system/runtime/rx_blocks_templates.h"
// mqtt_subscription
#include "protocols/mqtt/mqtt_subscription.h"
// mqtt_base
#include "protocols/mqtt/mqtt_base.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace protocols {
namespace mqtt {
namespace mqtt_simple {
class mqtt_simple_source;
class mqtt_simple_client_port;

} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols




namespace protocols {

namespace mqtt {

namespace mqtt_simple {
typedef rx_reference<mqtt_simple_client_port> mqtt_simple_client_port_ptr;
class mqtt_simple_source;
class mqtt_simple_mapper;
typedef rx_reference<mqtt_simple_source> mqtt_source_ptr;
typedef rx_reference<mqtt_simple_mapper> mqtt_mapper_ptr;





class mqtt_simple_client_endpoint : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(mqtt_simple_client_endpoint);

public:
    bool is_connected_;

  public:
      mqtt_simple_client_endpoint (mqtt_simple_client_port_ptr port, rx_timer_ptr timer);

      ~mqtt_simple_client_endpoint();


      rx_protocol_stack_endpoint* bind_endpoint ();

      void close_endpoint ();

      rx_port_impl_ptr get_port ();

      uint16_t start_transaction (mqtt_common::mqtt_transaction* trans);

      void timer_fired ();

      uint16_t send_transaction_result (mqtt_common::mqtt_transaction* trans);


  protected:

  private:

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      static rx_protocol_result_t disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      rx_protocol_result_t received_packet (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



      mqtt_simple_client_port_ptr port_;

      std::unique_ptr<mqtt_common::mqtt_connect_transaction> connect_transaction_;

      std::unique_ptr<mqtt_common::mqtt_ping_transaction> ping_transaction_;


      rx_protocol_stack_endpoint stack_;

      rx_timer_ptr timer_;

      mqtt_common::mqtt_packet_decoder decoder_;


};







typedef rx_platform::runtime::io_types::ports_templates::master_client_port_impl< mqtt_simple_client_endpoint  > mqtt_simple_client_port_base;






class mqtt_simple_client_port : public mqtt_simple_client_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
MQTT client/publisher simple port. Implementation of MQTT client/publisher simple JSON protocol.");

    DECLARE_REFERENCE_PTR(mqtt_simple_client_port);

    struct mqtt_source_def
    {
        std::set<mqtt_source_ptr> sources;
        mqtt_qos_level qos;
        uint16_t subs_msg_id;
        bool subscribed;
    };

    typedef std::map<string_type, mqtt_source_def> sources_type;

    typedef std::map<string_type, mqtt_common::mqtt_publish_data> publish_cahce_type;
       

    typedef std::map<uint16_t, mqtt_transaction_ptr> transactions_type;

  public:
      mqtt_simple_client_port();

      ~mqtt_simple_client_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void register_source (const string_type& topic, mqtt_qos_level qos, mqtt_source_ptr what);

      void unregister_source (const string_type& topic, mqtt_source_ptr what);

      void connected (mqtt_common::mqtt_connection_data data);

      void disconnected ();

      uint16_t get_next_message_id ();

      rx_protocol_result_t initiator_transaction_result_received (uint16_t msg_id, uint8_t ctrl, io::rx_const_io_buffer& buff);

      rx_protocol_result_t listener_transaction_result_received (uint16_t msg_id, uint8_t ctrl, io::rx_const_io_buffer& buff);

      void publish (mqtt_common::mqtt_publish_data data);

      void publish_received (mqtt_common::mqtt_publish_data data);


      const mqtt_common::mqtt_connection_data& get_connection_data () const;


      rx_platform::runtime::io_types::simple_port_status status;


      static std::map<rx_node_id, mqtt_simple_client_port::smart_ptr> runtime_instances;

      string_type client_id;

      uint16_t keep_alive;


  protected:

  private:

      void timer_fired ();



      sources_type sources_;

      transactions_type initiated_ransactions_;

      mqtt_common::mqtt_connection_data connection_data_;

      transactions_type listened_transactions_;

      rx_reference<mqtt_topics::mqtt_session> session_;


      locks::slim_lock lock_;

      std::atomic<uint16_t> message_id_;

      rx_timer_ptr timer_;

      publish_cahce_type publish_cache_;

      async_owned_value<bool> connected_;

      async_owned_value<uint64_t> published_;

      async_owned_value<uint64_t> received_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_mapper_impl< mqtt_simple_client_port  > mqtt_simple_mapper_base;






class mqtt_simple_mapper : public mqtt_simple_mapper_base  
{
    DECLARE_REFERENCE_PTR(mqtt_simple_mapper);

  public:
      mqtt_simple_mapper();

      ~mqtt_simple_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      virtual void update_publish_from_value (rx_value&& val, mqtt_common::mqtt_publish_data& data) = 0;


  protected:

  private:

      void mapped_value_changed (rx_value&& val, runtime::runtime_process_context* ctx);

      void mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime::runtime_process_context* ctx);



      string_type topic_;

      mqtt_qos_level qos_;

      bool retain_;


};


typedef mqtt_simple_mapper::smart_ptr mqtt_mapper_ptr;





class mqtt_json_mapper : public mqtt_simple_mapper  
{
    DECLARE_REFERENCE_PTR(mqtt_json_mapper);

  public:
      mqtt_json_mapper();

      ~mqtt_json_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void update_publish_from_value (rx_value&& val, mqtt_common::mqtt_publish_data& data);


  protected:

  private:


      data::runtime_data_model model_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_source_impl< mqtt_simple_client_port  > mqtt_simple_source_base;






class mqtt_simple_source : public mqtt_simple_source_base  
{
    DECLARE_REFERENCE_PTR(mqtt_simple_source);

  public:
      mqtt_simple_source();

      ~mqtt_simple_source();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      virtual void topic_changed (const byte_string& val, rx_time now) = 0;


  protected:

  private:


      string_type topic_;

      mqtt_qos_level qos_;

      bool retain_;


};






class mqtt_json_source : public mqtt_simple_source  
{
    DECLARE_REFERENCE_PTR(mqtt_json_source);

  public:
      mqtt_json_source();

      ~mqtt_json_source();


      void topic_changed (const byte_string& val, rx_time now);

      rx_result initialize_source (runtime::runtime_init_context& ctx);


  protected:

  private:


      string_type value_path_;


};






class mqtt_simple_event_mapper : public mqtt_simple_mapper_base  
{
    DECLARE_REFERENCE_PTR(mqtt_simple_event_mapper);

    typedef std::map<string_type, mqtt_common::mqtt_publish_data> unpublished_cahce_type;

  public:
      mqtt_simple_event_mapper();

      ~mqtt_simple_event_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      virtual void update_publish_from_value (rx_timed_value val, mqtt_common::mqtt_publish_data& data, string_type& topic_add) = 0;


  protected:

  private:

      virtual void mapped_event_fired (rx_timed_value val, runtime_process_context* ctx);



      string_type topic_;

      mqtt_qos_level qos_;

      bool retain_;

      unpublished_cahce_type unpublished_;


};






class mqtt_json_event_mapper : public mqtt_simple_event_mapper  
{

  public:
      mqtt_json_event_mapper();

      ~mqtt_json_event_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void update_publish_from_value (rx_timed_value val, mqtt_common::mqtt_publish_data& data, string_type& topic_add);


  protected:

  private:


      data::runtime_data_model model_;


};


} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols



#endif
