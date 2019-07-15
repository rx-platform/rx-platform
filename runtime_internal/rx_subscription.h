

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
#include "system/runtime/rx_operational.h"
#include "lib/rx_values.h"
#include "system/storage_base/rx_storage.h"

using namespace rx_platform;
using namespace rx::values;
using namespace rx_platform::runtime::operational;


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
	struct connect_data
	{
		connect_data(string_type local)
			: connecting(false), local_path(local)
		{
		}
		bool connecting;
		string_type local_path;
		rx_time last_checked;
	};
	struct runtime_data
	{
		platform_item_ptr item = platform_item_ptr::null_ptr;
		bool querying = false;
		std::map<runtime_handle_t, connect_data> items;
	};
	typedef std::map<string_type, runtime_data> to_connect_type;
	typedef std::map<string_type, std::vector<runtime_handle_t> > attempts_type;
	static constexpr uint32_t timer_period_ = 1000;

	string_array query_names_;
	std::vector<runtime_handle_t> item_handles_;


	class tags_callabck : public runtime::operational::rx_tags_callback
	{
	public:
		rx_subscription* whose;
		void items_changed(const std::vector<update_item>& items)
		{

		}
		virtual void write_complete(runtime_transaction_id_t transaction_id, rx_result result)
		{

		}
	};
	tags_callabck tags_callback_;

  public:
      rx_subscription (rx_subscription_callback* callback);


      rx_result connect_items (const std::vector<std::pair<string_type, runtime_handle_t> >& paths, std::vector<rx_result_with<runtime_handle_t> >& result);

      rx_result disconnect_items (const std::vector<runtime_handle_t>& items, std::vector<rx_result>& results);

      void activate ();

      void deactivate ();


  protected:

  private:

      rx_result process_connections ();



      tags_type tags_;

      rx_subscription_callback *callback_;


      handles_type handles_;

      inverse_tags_type inverse_tags_;

      locks::slim_lock items_lock_;

      to_connect_type to_connect_;

      attempts_type attempts_;

      bool active_;


};


} // namespace subscriptions
} // namespace sys_runtime



#endif
