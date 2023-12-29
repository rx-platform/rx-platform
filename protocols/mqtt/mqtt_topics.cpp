

/****************************************************************************
*
*  protocols\mqtt\mqtt_topics.cpp
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


#include "pch.h"


// mqtt_subscription
#include "protocols/mqtt/mqtt_subscription.h"
// mqtt_topics
#include "protocols/mqtt/mqtt_topics.h"

using namespace protocols::mqtt::mqtt_common;


namespace protocols {

namespace mqtt {

namespace mqtt_topics {

// Class protocols::mqtt::mqtt_topics::mqtt_topic 

mqtt_topic::mqtt_topic()
      : utf_string(false)
{
}


mqtt_topic::~mqtt_topic()
{
}



void mqtt_topic::publish_received (mqtt_common::mqtt_publish_data data, sessions_cache* sessions)
{
    if (this->data != data.data || this->utf_string != data.utf_string || this->content_type != data.content_type)
    {
        this->data = std::move(data.data);
        this->utf_string = data.utf_string;
        this->content_type = std::move(data.content_type);

        auto new_data = create_publish_data();

        sessions->notify_publish(std::move(new_data));
    }
}

mqtt_common::mqtt_publish_data mqtt_topic::create_publish_data ()
{
    mqtt_common::mqtt_publish_data ret;
    ret.topic = topic_id;
    ret.content_type = content_type;
    ret.utf_string = utf_string;
    ret.data = data;

    return ret;
}


// Class protocols::mqtt::mqtt_topics::topics_cache 

topics_cache::topics_cache()
{
}


topics_cache::~topics_cache()
{
}



void topics_cache::publish_received (mqtt_common::mqtt_publish_data data, sessions_cache* sessions)
{
    auto it = topics_.find(data.topic);
    if (it != topics_.end())
    {
        it->second->publish_received(std::move(data), sessions);
    }
    else
    {
        auto new_topic = std::make_unique<mqtt_topic>();
        new_topic->topic_id = data.topic;
        auto result = topics_.emplace(data.topic, std::move(new_topic));
        result.first->second->publish_received(std::move(data), sessions);
    }
}


} // namespace mqtt_topics
} // namespace mqtt
} // namespace protocols

