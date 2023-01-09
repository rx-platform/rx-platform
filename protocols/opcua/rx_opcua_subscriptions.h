

/****************************************************************************
*
*  protocols\opcua\rx_opcua_subscriptions.h
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


#ifndef rx_opcua_subscriptions_h
#define rx_opcua_subscriptions_h 1



// rx_opcua_subs_set
#include "protocols/opcua/rx_opcua_subs_set.h"
// rx_opcua_addr_space
#include "protocols/opcua/rx_opcua_addr_space.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace protocols {
namespace opcua {
namespace opcua_subscriptions {
class opcua_subscriptions_collection;

} // namespace opcua_subscriptions
} // namespace opcua
} // namespace protocols


#include "system/threads/rx_job.h"
using rx_platform::rx_timer_ptr;
#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;
#include "rx_opcua_params.h"
using namespace protocols::opcua::common;


namespace protocols {

namespace opcua {

namespace opcua_subscriptions {
typedef typename std::unique_ptr<typename requests::opcua_subscription::opcua_publish_request> publish_request_ptr;
typedef typename std::unique_ptr<typename requests::opcua_subscription::opcua_publish_response> publish_response_ptr;
typedef typename std::unique_ptr<typename requests::opcua_subscription::opcua_republish_request> republish_request_ptr;
typedef typename std::unique_ptr<typename requests::opcua_subscription::opcua_republish_response> republish_response_ptr;

using requests::opcua_response_ptr;





class opcua_monitored_item 
{

  public:
      opcua_monitored_item (uint32_t handle, timestamps_return_type timestamps, const create_monitored_item_data& data);


      const rx_node_id& get_node_id () const;

      virtual void timer_tick (std::vector<requests::opcua_subscription::monitored_item_notification>& notifications) = 0;

      static std::unique_ptr<opcua_monitored_item> create_monitoring_item (uint32_t server_handle, timestamps_return_type timestamps, const create_monitored_item_data& data, create_monitored_item_result& out);

      virtual opcua_result_t register_monitor (opcua_addr_space::opcua_address_space_base* addr_space) = 0;

      virtual opcua_result_t unregister_monitor (opcua_addr_space::opcua_address_space_base* addr_space) = 0;

      opcua_monitored_item() = delete;
      opcua_monitored_item(const opcua_monitored_item&) = delete;
      opcua_monitored_item(opcua_monitored_item&&) = delete;
  protected:

      uint32_t server_handle;

      uint32_t client_handle;

      monitoring_mode_t mode;

      uint32_t interval;

      uint32_t queue_size;

      bool discard_oldest;

      monitoring_filter_ptr filter_ptr;


  private:


      rx_node_id node_id_;


};


struct opcua_subscription_data
{
    uint32_t interval;
    uint32_t lifetime_count;
    uint32_t max_keep_alive;
    uint32_t max_notifications;
    uint8_t priority;
};





class opcua_subscription : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(opcua_subscription);
    enum subscription_state
    {
        closed,
        creating,
        normal,
        late,
        keep_alive
    };
    typedef std::map<uint32_t, std::unique_ptr<opcua_monitored_item> > items_type;

  public:
      opcua_subscription (uint32_t id, opcua_subscription_data& data, opcua_subscriptions_collection* container);


      rx_result start ();

      rx_result stop ();

      void set_enabled ();

      void set_disabled ();

      rx_result create_monitored_item (uint32_t server_handle, timestamps_return_type timestamps, const create_monitored_item_data& data, create_monitored_item_result& out, opcua_addr_space::opcua_address_space_base* addr_space);

      opcua_result_t delete_monitored_item (uint32_t server_handle, opcua_addr_space::opcua_address_space_base* addr_space);

      void clear_all_items (opcua_addr_space::opcua_address_space_base* addr_space);


  protected:

  private:

      void timer_tick ();



      items_type items_;

      opcua_subscriptions_collection *container_;


      rx_timer_ptr timer_;

      uint32_t subscription_id_;

      uint32_t interval_;

      uint32_t lifetime_count_;

      uint32_t max_keep_alive_;

      bool enabled_;

      uint32_t max_notifications_;

      uint8_t priority_;

      subscription_state state_;

      locks::slim_lock lock_;

      std::atomic<uint32_t> next_seq_number_;

      uint32_t current_keep_alive_;


};






class opcua_subscriptions_collection 
{
    typedef std::map<uint32_t, opcua_subscription::smart_ptr> subscriptions_type;
    typedef std::queue<publish_request_ptr> publish_requests_type;

  public:
      opcua_subscriptions_collection (opcua_addr_space::opcua_address_space_base* as);


      uint32_t create_subscription (opcua_subscription_data& data, bool active);

      opcua_result_t delete_subscription (uint32_t id);

      create_monitored_item_result create_monitored_item (uint32_t id, timestamps_return_type timestamps, create_monitored_item_data& data);

      opcua_result_t queue_publish_request (publish_request_ptr req);

      publish_request_ptr dequeue_publish_request ();

      opcua_response_ptr republish_request (republish_request_ptr req);

      opcua_result_t delete_monitored_item (uint32_t subscription_id, uint32_t server_handle);


  protected:

  private:


      subscriptions_type subscriptions_;

      opcua_addr_space::opcua_address_space_base *address_space_;

      publish_requests_type publish_requests_;


      locks::slim_lock lock_;

      static uint32_t g_next_id_;


};






class opcua_monitored_value : public opcua_monitored_item  
{
    typedef std::vector<data_value> queue_type;

  public:
      opcua_monitored_value (uint32_t handle, timestamps_return_type timestamps, const create_monitored_item_data& data);


      void timer_tick (std::vector<requests::opcua_subscription::monitored_item_notification>& notifications);

      opcua_result_t register_monitor (opcua_addr_space::opcua_address_space_base* addr_space);

      opcua_result_t unregister_monitor (opcua_addr_space::opcua_address_space_base* addr_space);

      void monitored_value_changed (data_value val);

      opcua_monitored_value() = delete;
      opcua_monitored_value(const opcua_monitored_value&) = delete;
      opcua_monitored_value(opcua_monitored_value&&) = delete;
  protected:

  private:


      queue_type queue_;

      data_value last_value_;


};


} // namespace opcua_subscriptions
} // namespace opcua
} // namespace protocols



#endif
