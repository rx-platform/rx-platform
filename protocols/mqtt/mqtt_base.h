

/****************************************************************************
*
*  protocols\mqtt\mqtt_base.h
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


#ifndef mqtt_base_h
#define mqtt_base_h 1


#include "lib/rx_io_buffers.h"
#include "lib/rx_lock.h"
#include "mqtt_protocol.h"




namespace protocols {

namespace mqtt {

namespace mqtt_common {
uint8_t create_qos0_control_byte(mqtt_message_type msg_type);
uint8_t create_qos1_control_byte(mqtt_message_type msg_type);
uint8_t create_publish_control_byte(mqtt_qos_level qos_level, bool dup, bool ret);

int32_t read_multibyte_size(io::rx_const_io_buffer& buffer);
bool write_multibyte_size(int32_t size, io::rx_io_buffer& buffer);






class mqtt_transaction 
{

  public:
      mqtt_transaction();

      mqtt_transaction (uint16_t id, uint32_t timeout = 500);

      virtual ~mqtt_transaction();


      virtual rx_protocol_result_t start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version) = 0;

      virtual rx_protocol_result_t process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version) = 0;

      virtual rx_protocol_result_t start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version) = 0;

      virtual rx_protocol_result_t send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version) = 0;

      virtual bool is_done () const = 0;


      uint16_t get_trans_id () const;
      void set_trans_id (uint16_t value);


  protected:

  private:


      uint16_t trans_id_;

      rx_timer_ticks_t timeout_;


};






class mqtt_publish_data 
{

  public:
      mqtt_publish_data();


      rx_result deserialize (uint8_t ctrl, io::rx_const_io_buffer& buff, mqtt_protocol_version version);

      uint32_t get_prop_size (uint8_t ctrl, mqtt_protocol_version version) const;

      rx_result serialize (uint8_t& ctrl, io::rx_io_buffer& buff, mqtt_protocol_version version);


      mqtt_qos_level qos;

      bool dup;

      bool retain;

      string_type topic;

      byte_string data;

      bool utf_string;

      uint32_t message_expiry_interval;

      string_type content_type;

      string_type response_topic;

      byte_string corelation_data;

      std::map<string_type, string_type> user_properties;

      std::set<uint32_t> subs_ids;

      uint16_t packet_id;

      uint16_t topic_alias;

      mqtt_publish_data(const mqtt_publish_data&) = default;
      mqtt_publish_data(mqtt_publish_data&&) noexcept = default;
      mqtt_publish_data& operator=(const mqtt_publish_data&) = default;
      mqtt_publish_data& operator=(mqtt_publish_data&&) noexcept = default;
      ~mqtt_publish_data() = default;
  protected:

  private:


};






class mqtt_publish_transaction : public mqtt_transaction  
{

  public:
      mqtt_publish_transaction (mqtt_publish_data data, uint16_t id, uint32_t timeout = 1000);


      rx_protocol_result_t start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      rx_protocol_result_t process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      bool is_done () const;


  protected:

  private:


      mqtt_publish_data data_;


      int step_;


};






class mqtt_connection_data 
{

  public:
      mqtt_connection_data();


      bool new_session;

      uint32_t session_expiry;

      uint16_t receive_maximum;

      mqtt_qos_level max_qos_level;

      string_type client_identifier;

      uint16_t topic_alias_max;

      uint16_t keep_alive;

      bool retain_available;

      uint32_t maximum_packet_size;

      std::map<string_type, string_type> user_properties;

      bool wildcard_available;

      bool subscription_id_available;

      bool shared_subscription;

      string_type response_info;

      string_type server_reference;

      string_type auth_method;

      byte_string auth_data;

      mqtt_protocol_version protocol_version;

      uint32_t will_delay_interval;

      string_type user_name;

      string_type password;

      mqtt_connection_data(const mqtt_connection_data&) = default;
      mqtt_connection_data(mqtt_connection_data&&) noexcept = default;
      mqtt_connection_data& operator=(const mqtt_connection_data&) = default;
      mqtt_connection_data& operator=(mqtt_connection_data&&) noexcept = default;
      ~mqtt_connection_data() = default;
  protected:

  private:


};






class mqtt_connect_transaction : public mqtt_transaction  
{

  public:
      mqtt_connect_transaction();

      mqtt_connect_transaction (mqtt_connection_data data, const mqtt_publish_data& will_data, const string_type& user = "", const string_type& pass = "", uint32_t timeout = 1000);


      rx_protocol_result_t start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      rx_protocol_result_t process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      bool is_done () const;


      const mqtt_connection_data& get_data () const;

      const mqtt_publish_data& get_will_data () const;


      const uint8_t get_result_code () const;
      void set_result_code (uint8_t value);

      const string_type& get_result_string () const;
      void set_result_string (const string_type& value);


  protected:

  private:


      mqtt_connection_data data_;

      mqtt_publish_data will_data_;


      bool result_received_;

      uint8_t result_code_;

      string_type result_string_;


};






class mqtt_ping_transaction : public mqtt_transaction  
{

  public:
      mqtt_ping_transaction (uint32_t timeout = 1000);


      rx_protocol_result_t start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      rx_protocol_result_t process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      bool is_done () const;


  protected:

  private:


      bool done_;


};






class mqtt_subscribe_data 
{

  public:
      mqtt_subscribe_data();


      string_type topic;

      mqtt_qos_level qos;

      mqtt_retain_handling retain_handling;

      bool no_local;

      bool retain_as_published;

      uint8_t result_code;


  protected:

  private:


};






class mqtt_subscribe_transaction : public mqtt_transaction  
{
    typedef std::vector<mqtt_subscribe_data> topics_type;

  public:
      mqtt_subscribe_transaction();

      mqtt_subscribe_transaction (const string_type& subs_id, topics_type topics, uint16_t id, uint32_t timeout = 1000);

      mqtt_subscribe_transaction (const string_type& subs_id, mqtt_subscribe_data topic, uint16_t id, uint32_t timeout = 1000);


      rx_protocol_result_t start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      rx_protocol_result_t process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      bool is_done () const;


      const string_type& get_result_string () const;
      void set_result_string (const string_type& value);


  protected:

  private:


      topics_type topics_;


      rx_result result_;

      bool done_;

      string_type subscription_id_;

      std::map<string_type, string_type> user_properties_;

      string_type result_string_;


};






class mqtt_packet_decoder 
{
    typedef std::function< rx_protocol_result_t(recv_protocol_packet)> callback_type;

  public:
      mqtt_packet_decoder();


      rx_protocol_result_t received_function (recv_protocol_packet packet);

      void init_decoder (callback_type callback);


  protected:

  private:

      void init_decoder ();



      io::rx_io_buffer receiving_buffer_;

      std::array<uint8_t, 5> header_buffer_;

      size_t expected_;

      size_t collected_;

      size_t collected_header_;

      size_t collected_size_;

      uint8_t* header_;

      size_t header_size_;

      callback_type callback_;

      locks::slim_lock lock_;


};






class mqtt_unsubscribe_transaction : public mqtt_transaction  
{

  public:
      mqtt_unsubscribe_transaction();

      mqtt_unsubscribe_transaction (string_array topics, uint16_t id, uint32_t timeout = 1000);

      mqtt_unsubscribe_transaction (string_type topic, uint16_t id, uint32_t timeout = 1000);


      rx_protocol_result_t start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      rx_protocol_result_t process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version);

      rx_protocol_result_t send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version);

      bool is_done () const;


      const string_type& get_result_string () const;
      void set_result_string (const string_type& value);


  protected:

  private:


      bool done_;

      std::map<string_type, string_type> user_properties_;

      string_type result_string_;

      string_array topics_;


};


} // namespace mqtt_common
} // namespace mqtt
} // namespace protocols

namespace protocols
{
namespace mqtt
{
typedef std::unique_ptr<mqtt_common::mqtt_transaction> mqtt_transaction_ptr;
}
}


#endif
