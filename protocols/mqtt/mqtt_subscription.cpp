

/****************************************************************************
*
*  protocols\mqtt\mqtt_subscription.cpp
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


#include "pch.h"


// mqtt_subscription
#include "protocols/mqtt/mqtt_subscription.h"



namespace protocols {

namespace mqtt {

namespace mqtt_topics {

// Class protocols::mqtt::mqtt_topics::mqtt_subscription 

mqtt_subscription::mqtt_subscription()
{
}


mqtt_subscription::~mqtt_subscription()
{
}



void mqtt_subscription::notify_publish (const mqtt_common::mqtt_publish_data& data)
{
}


// Class protocols::mqtt::mqtt_topics::sessions_cache 

sessions_cache::sessions_cache()
      : max_session_expiry(0),
        min_keep_alive(0)
{
}


sessions_cache::~sessions_cache()
{
}



void sessions_cache::notify_publish (const mqtt_common::mqtt_publish_data& data)
{
    for (auto& one : sessions_)
    {
        one.second->notify_publish(data);
    }
}

protocols::mqtt::mqtt_topics::session_request_result sessions_cache::session_request (mqtt_common::mqtt_connection_data& data)
{
    string_type session_id = data.client_identifier;
    if (session_id.empty())
    {
        if (!data.new_session)
        {
            session_request_result result;
            result.result_code = 0x85;
            result.result_reason = RX_NOT_IMPLEMENTED;
            return result;
        }
        rx_uuid uuid = rx_uuid::create_new();
        string_type rnd;
        uuid.to_string(rnd);
        session_id = "session" + rnd;
        
    }

    session_request_result result;
    result.result_code = 0x00;

    return result;
}


// Class protocols::mqtt::mqtt_topics::mqtt_session 

mqtt_session::mqtt_session()
      : session_expiry(0)
{
}


mqtt_session::~mqtt_session()
{
}



void mqtt_session::notify_publish (const mqtt_common::mqtt_publish_data& data)
{
}


// Class protocols::mqtt::mqtt_topics::session_request_result 


bool session_request_result::succeeded () const
{
    return session_ptr;
}


} // namespace mqtt_topics
} // namespace mqtt
} // namespace protocols

