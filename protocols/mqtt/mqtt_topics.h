

/****************************************************************************
*
*  protocols\mqtt\mqtt_topics.h
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


#ifndef mqtt_topics_h
#define mqtt_topics_h 1



// mqtt_base
#include "protocols/mqtt/mqtt_base.h"

namespace protocols {
namespace mqtt {
namespace mqtt_topics {
class sessions_cache;

} // namespace mqtt_topics
} // namespace mqtt
} // namespace protocols




namespace protocols {

namespace mqtt {

namespace mqtt_topics {






class mqtt_topic 
{

  public:
      mqtt_topic();

      ~mqtt_topic();


      void publish_received (mqtt_common::mqtt_publish_data data, sessions_cache* sessions);


      string_type topic_id;

      byte_string data;

      bool utf_string;

      string_type content_type;


  protected:

  private:

      mqtt_common::mqtt_publish_data create_publish_data ();



};







class topics_cache 
{
    typedef std::map<string_type, std::unique_ptr<mqtt_topic> > topics_type;

  public:
      topics_cache();

      ~topics_cache();


      void publish_received (mqtt_common::mqtt_publish_data data, sessions_cache* sessions);


  protected:

  private:


      topics_type topics_;


};


} // namespace mqtt_topics
} // namespace mqtt
} // namespace protocols



#endif
