

/****************************************************************************
*
*  protocols\mqtt\mqtt_subscription.h
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


#ifndef mqtt_subscription_h
#define mqtt_subscription_h 1



// mqtt_base
#include "protocols/mqtt/mqtt_base.h"
// rx_ptr
#include "lib/rx_ptr.h"



namespace protocols {

namespace mqtt {

namespace mqtt_topics {





class mqtt_subscription 
{

  public:
      mqtt_subscription();

      ~mqtt_subscription();


      void notify_publish (const mqtt_common::mqtt_publish_data& data);


  protected:

  private:


      mqtt_common::mqtt_subscribe_data filters_;


      string_type subscription_id_;

      std::map<string_type, string_type> user_properties_;


};






class mqtt_session : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(mqtt_session);
    typedef std::map<string_type, std::unique_ptr<mqtt_subscription> > subscriptions_type;

  public:
      mqtt_session();

      ~mqtt_session();


      void notify_publish (const mqtt_common::mqtt_publish_data& data);


      string_type session_id;

      uint32_t session_expiry;


  protected:

  private:


      subscriptions_type subscriptions_;


};






class session_request_result 
{

  public:

      bool succeeded () const;


      string_type result_reason;

      uint8_t result_code;

      mqtt_session::smart_ptr session_ptr;


  protected:

  private:


};







class sessions_cache 
{
    typedef std::map<string_type, mqtt_session::smart_ptr> sessions_type;

  public:
      sessions_cache();

      ~sessions_cache();


      void notify_publish (const mqtt_common::mqtt_publish_data& data);

      session_request_result session_request (mqtt_common::mqtt_connection_data& data);


      uint16_t max_session_expiry;

      uint16_t min_keep_alive;


  protected:

  private:


      sessions_type sessions_;


};


} // namespace mqtt_topics
} // namespace mqtt
} // namespace protocols

namespace protocols
{
namespace mqtt
{
typedef rx_reference<mqtt_topics::mqtt_session> mqtt_session_ptr;
}
}


#endif
