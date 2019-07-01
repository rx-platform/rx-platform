

/****************************************************************************
*
*  runtime_internal\rx_subscription.h
*
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_subscription_h
#define rx_subscription_h 1



// rx_ptr
#include "lib/rx_ptr.h"

#include "system/runtime/rx_runtime_helpers.h"
#include "lib/rx_values.h"

using namespace rx_platform;
using namespace rx::values;


namespace sys_runtime {

namespace subscriptions {





class rx_subscription_tag 
{

  public:

      uint32_t sampling_rate;

      string_type path;

      int ref_count;

      runtime_handle_t target_handle;


  protected:

  private:


};


struct update_item
{
	runtime_handle_t handle;
	rx_value value;
};

struct write_result_item
{
	runtime_handle_t handle;
	rx_result result;
};




class rx_subscription_callback 
{

  public:

      virtual void items_changed (const std::vector<update_item>& items) = 0;

      virtual void write_complete (runtime_transaction_id_t transaction_id, rx_result result, const std::vector<update_item>& items) = 0;


  protected:

  private:


};






class rx_subscription : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_subscription);

	typedef std::map<runtime_handle_t, rx_subscription_tag> tags_type; // mine handle -> item
	typedef std::map<runtime_handle_t, runtime_handle_t> handles_type;// target_handle -> mine handle
	typedef std::map<string_type, runtime_handle_t> inverse_tags_type;// path -> mine handle
  public:

  public:
      rx_subscription (rx_subscription_callback* callback);


      rx_result connect_items (const std::vector<std::pair<string_type, runtime_handle_t> >& paths, std::vector<rx_result_with<runtime_handle_t> >& results);

      rx_result disconnect_items (const std::vector<runtime_handle_t>& items, std::vector<rx_result>& results);


  protected:

  private:


      tags_type tags_;

      rx_subscription_callback *callback_;


      handles_type handles_;

      inverse_tags_type inverse_tags_;

      locks::slim_lock items_lock_;


};


} // namespace subscriptions
} // namespace sys_runtime



#endif
