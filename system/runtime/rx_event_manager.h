

/****************************************************************************
*
*  system\runtime\rx_event_manager.h
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


#ifndef rx_event_manager_h
#define rx_event_manager_h 1



// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_thread
#include "system/threads/rx_thread.h"
// rx_values
#include "lib/rx_values.h"
// rx_rt_data
#include "lib/rx_rt_data.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "system/server/rx_platform_item.h"
#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"


namespace rx_platform {

namespace runtime {

namespace events {





class fired_event_data 
{

  public:

      rx::values::rx_simple_value simple_value;

      rx::data::runtime_values_data struct_value;


      string_type queue;

      string_type path;

      rx_time time;

      bool test;

      types_cache types;

      bool remove_queue;

      bool state_event;


  protected:

  private:


};






class rx_events_callback : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_events_callback);

  public:

      virtual void event_fired (runtime_handle_t handle, const simple_event_item& data) = 0;

      virtual void event_fired (runtime_handle_t handle, const data_event_item& data) = 0;


  protected:

  private:


};







class runtime_events_manager : public threads::physical_job_thread  
{
    typedef std::map<string_type, std::unique_ptr<fired_event_data> > event_cache_type;
	typedef std::map<rx_node_id, std::set<fired_event_data*>> types_cache_type;
    struct subscriber_data_t
    {
        rx_events_callback::smart_ptr callback;
        event_filter filter;
        bool bin_value;
    };
    typedef std::map<runtime_handle_t, subscriber_data_t> subscribers_list_type;
	typedef std::map<rx_node_id, subscribers_list_type> subscribers_type;

  public:
      runtime_events_manager();

      ~runtime_events_manager();


      void event_fired (fired_event_data data);

      static runtime_events_manager& instance ();

      rx_result_with<runtime_handle_t> register_subscriber (rx_events_callback::smart_ptr who, rx_node_id type, event_filter filter, bool bin);

      rx_result unregister_subscriber (rx_node_id type, runtime_handle_t handle);

      rx_result start (hosting::rx_platform_host* host, const runtime_data_t& data, const io_manager_data_t& io_data);

      void stop ();


  protected:

  private:

      void remove_types (const types_cache& types, fired_event_data* whose);

      void add_types (const types_cache& types, fired_event_data* whose);



      event_cache_type event_cache_;

      subscribers_type subscribers_;


      types_cache_type types_cache_;


};


} // namespace events
} // namespace runtime
} // namespace rx_platform



#endif
