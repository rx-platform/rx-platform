

/****************************************************************************
*
*  system\runtime\rx_event_blocks.h
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


#ifndef rx_event_blocks_h
#define rx_event_blocks_h 1



// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_values
#include "lib/rx_values.h"
// rx_rt_data
#include "lib/rx_rt_data.h"
// rx_ptr
#include "lib/rx_ptr.h"



namespace rx_platform {

namespace runtime {

namespace event_blocks {





class fired_event_data 
{

  public:

      rx::values::rx_simple_value simple_value;

      rx::data::runtime_values_data struct_value;


      string_type event_id;

      rx_time time;


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






class runtime_events 
{
    typedef std::map<structure::event_data*, fired_event_data> events_cache_type;
    typedef std::map<string_type, structure::event_data*> event_ids_type;

    typedef std::vector<int> subscribers_type;

  public:
      runtime_events();

      ~runtime_events();


      void register_event (structure::event_data* who, const string_type& id);

      void event_fired (const structure::event_data* whose, rx_simple_value value, data::runtime_values_data data);


  protected:

  private:


      events_cache_type events_cache_;

      event_ids_type event_ids_;

      subscribers_type subscribers_;


};


} // namespace event_blocks
} // namespace runtime
} // namespace rx_platform



#endif
