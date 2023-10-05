

/****************************************************************************
*
*  protocols\mqtt\mqtt_simple_server.h
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


#ifndef mqtt_simple_server_h
#define mqtt_simple_server_h 1


#include "mqtt_base.h"

// rx_blocks_templates
#include "system/runtime/rx_blocks_templates.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// mqtt_subscription
#include "protocols/mqtt/mqtt_subscription.h"
// mqtt_topics
#include "protocols/mqtt/mqtt_topics.h"
// mqtt_base
#include "protocols/mqtt/mqtt_base.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace protocols {
namespace mqtt {
namespace mqtt_simple {
class mqtt_simple_broker_source;
class mqtt_simple_server_port;

} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols




namespace protocols {

namespace mqtt {

namespace mqtt_simple {
typedef rx_reference<mqtt_simple_server_port> mqtt_simple_server_port_ptr;
class mqtt_simple_broker_source;
class mqtt_simple_broker_mapper;
typedef rx_reference<mqtt_simple_broker_source> mqtt_broker_source_ptr;
typedef rx_reference<mqtt_simple_broker_mapper> mqtt_broker_mapper_ptr;





class mqtt_simple_server_endpoint : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(mqtt_simple_server_endpoint);

public:

  public:
      mqtt_simple_server_endpoint (mqtt_simple_server_port_ptr port, rx_timer_ptr timer);

      ~mqtt_simple_server_endpoint();


      rx_protocol_stack_endpoint* bind_endpoint ();

      void close_endpoint ();

      rx_port_impl_ptr get_port ();

      void timer_fired ();


  protected:

  private:

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      static rx_protocol_result_t disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      rx_protocol_result_t received_packet (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      rx_protocol_result_t do_transaction (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_transaction_ptr trans);



      mqtt_simple_server_port_ptr port_;

      mqtt_common::mqtt_connection_data connection_data_;

      mqtt_session_ptr session_;


      rx_protocol_stack_endpoint stack_;

      rx_timer_ptr timer_;

      mqtt_common::mqtt_packet_decoder decoder_;

      uint8_t protocol_version_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< mqtt_simple_server_endpoint  > mqtt_simple_server_base;






class mqtt_simple_server_port : public mqtt_simple_server_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
MQTT Broker simple port. Implementation of MQTT Broker simple JSON protocol.");

    DECLARE_REFERENCE_PTR(mqtt_simple_server_port);

    struct mqtt_source_def
    {
        std::set<mqtt_broker_source_ptr> sources;
        uint16_t subs_msg_id;
        bool subscribed;
    };

    typedef std::map<string_type, mqtt_source_def> sources_type;

    typedef std::map<string_type, mqtt_common::mqtt_publish_data> publish_cahce_type;

  public:
      mqtt_simple_server_port();

      ~mqtt_simple_server_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void register_source (const string_type& topic, mqtt_broker_source_ptr what);

      void unregister_source (const string_type& topic, mqtt_broker_source_ptr what);

      mqtt_topics::session_request_result connected (mqtt_common::mqtt_connection_data& data);

      void disconnected ();

      void publish (mqtt_common::mqtt_publish_data data);

      void publish_received (mqtt_common::mqtt_publish_data data);

      uint16_t get_next_message_id ();


      rx_platform::runtime::io_types::simple_port_status status;


      static std::map<rx_node_id, mqtt_simple_server_port::smart_ptr> runtime_instances;


  protected:

  private:

      void timer_fired ();



      sources_type sources_;

      mqtt_topics::sessions_cache sessions_;

      mqtt_topics::topics_cache topics_;


      locks::slim_lock lock_;

      rx_timer_ptr timer_;

      publish_cahce_type publish_cache_;

      std::atomic<uint16_t> message_id_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_source_impl< protocols::mqtt::mqtt_simple::mqtt_simple_server_port  > mqtt_simple_broker_source_base;






class mqtt_simple_broker_source : public mqtt_simple_broker_source_base  
{
    DECLARE_REFERENCE_PTR(mqtt_simple_broker_source);

  public:
      mqtt_simple_broker_source();

      ~mqtt_simple_broker_source();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      virtual void topic_changed (const byte_string& val, rx_time now) = 0;


  protected:

  private:


      string_type topic_;


};






class mqtt_json_broker_source : public mqtt_simple_broker_source  
{

  public:
      mqtt_json_broker_source();

      ~mqtt_json_broker_source();


      void topic_changed (const byte_string& val, rx_time now);

      rx_result initialize_source (runtime::runtime_init_context& ctx);


  protected:

  private:


      string_type value_path_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_mapper_impl< protocols::mqtt::mqtt_simple::mqtt_simple_server_port  > mqtt_simple_broker_mapper_base;






class mqtt_simple_broker_mapper : public mqtt_simple_broker_mapper_base  
{
    DECLARE_REFERENCE_PTR(mqtt_simple_broker_mapper);

  public:
      mqtt_simple_broker_mapper();

      ~mqtt_simple_broker_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      virtual void update_publish_from_value (rx_value&& val, mqtt_common::mqtt_publish_data& data) = 0;


  protected:

  private:

      void mapped_value_changed (rx_value&& val, runtime::runtime_process_context* ctx);

      void mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime::runtime_process_context* ctx);



      string_type topic_;


};






class mqtt_json_broker_mapper : public mqtt_simple_broker_mapper  
{
    DECLARE_REFERENCE_PTR(mqtt_json_broker_mapper);

  public:
      mqtt_json_broker_mapper();

      ~mqtt_json_broker_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void update_publish_from_value (rx_value&& val, mqtt_common::mqtt_publish_data& data);


  protected:

  private:


};


} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols



#endif
