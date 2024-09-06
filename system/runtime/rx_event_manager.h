

/****************************************************************************
*
*  system\runtime\rx_event_manager.h
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


#ifndef rx_event_manager_h
#define rx_event_manager_h 1



// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_values
#include "lib/rx_values.h"
// rx_rt_data
#include "lib/rx_rt_data.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "system/server/rx_platform_item.h"


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


  protected:

  private:


};






class rx_events_callback : public rx::pointers::reference_object  
{

  public:

      virtual void event_fired (const simple_event_item& data) = 0;

      virtual void event_fired (const data_event_item& data) = 0;


  protected:

  private:


};







class runtime_events_manager 
{
    typedef std::map<string_type, fired_event_data> event_cache_type;
    struct subscriber_data_t
    {
        rx_events_callback::smart_ptr callback;
        event_filter filter;
        bool bin_value;
    };
    typedef std::map<runtime_handle_t, subscriber_data_t> subscribers_type;

  public:
      runtime_events_manager();

      ~runtime_events_manager();


      void event_fired (fired_event_data data);


  protected:

  private:


      event_cache_type event_cache_;

      subscribers_type subscribers_;


};


} // namespace events
} // namespace runtime
} // namespace rx_platform



#endif
