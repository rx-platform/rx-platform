

/****************************************************************************
*
*  runtime_internal\rx_subscription.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_subscription_h
#define rx_subscription_h 1



// rx_operational
#include "system/runtime/rx_operational.h"
// rx_job
#include "lib/rx_job.h"

#include "system/runtime/rx_runtime_helpers.h"
#include "system/runtime/rx_operational.h"
#include "lib/rx_values.h"
#include "system/storage_base/rx_storage.h"

using namespace rx_platform;
using namespace rx::values;
using namespace rx_platform::runtime::tag_blocks;


namespace rx_internal {

namespace sys_runtime {

namespace subscriptions {
class rx_subscription;
typedef rx_reference<rx_subscription> rx_subscription_ptr;





class rx_subscription_tag 
{

  public:

      uint32_t sampling_rate;

      string_type path;

      int ref_count;

      runtime_handle_t target_handle;

      size_t connection_idx;

      runtime_handle_t mine_handle;


  protected:

  private:


};






class rx_subscription_callback 
{

  public:
      virtual ~rx_subscription_callback();


      virtual void items_changed (const std::vector<update_item>& items) = 0;

      virtual void transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items) = 0;

      virtual void write_completed (runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_result> > results) = 0;


  protected:

  private:


};






class runtime_connection_data 
{
    typedef std::vector<rx_subscription_tag> tags_type;

  public:

      rx_subscription_tag* get_tag (runtime_handle_t handle);

      bool remove_tag (runtime_handle_t handle);

      runtime_handle_t add_tag (rx_subscription_tag&& tag, runtime_handle_t connection_handle);

      bool process_connection (const rx_time& ts, rx_subscription_ptr whose);

      bool connection_dead ();


      rx_time last_checked;

      string_type path;

      platform_item_ptr item;

      bool connecting;

      bool connected;


  protected:

  private:


      tags_type tags_;


      std::vector<size_t> empty_slots_;

      string_array to_connect;

      std::vector<size_t> connect_indexes;


};






class subscription_write_transaction 
{
    struct single_write_data
    {
        runtime_handle_t handle;
        size_t transaction_index;
        rx_result result;
    };
    typedef std::map<runtime_transaction_id_t, single_write_data> writes_type;
  public:
      typedef std::map<runtime_transaction_id_t, runtime_transaction_id_t> transactions_map_type;

  public:

      runtime_transaction_id_t add_write (runtime_transaction_id_t master_id, transactions_map_type& map, runtime_handle_t handle);

      bool write_done (runtime_transaction_id_t trans_id, rx_result&& result, transactions_map_type& map);

      bool is_done () const;

      std::vector<std::pair<runtime_handle_t, rx_result> > get_results ();

      runtime_transaction_id_t client_handle;
  protected:

  private:


      writes_type writes_;

      int pending_count_;


};






class rx_subscription : public rx_platform::runtime::tag_blocks::rx_tags_callback  
{
	DECLARE_REFERENCE_PTR(rx_subscription);

	typedef std::map<runtime_handle_t, std::vector<runtime_handle_t> > handles_type;// target_handle -> mine handles
	typedef std::map<string_type, runtime_handle_t> tags_type;// path -> mine handle
    typedef std::map<runtime_handle_t, string_type> inverse_tags_type;// mine handle -> path

    typedef std::map<runtime_handle_t, rx_value> values_cache_type;// values cache

    typedef std::vector<runtime_connection_data> connections_type;
	typedef std::map<string_type, size_t> connection_paths_type;
	typedef std::map<rx_thread_handle_t, std::vector<size_t> > connection_attempts_type;
	static constexpr uint32_t timer_period_ = 1000;

    struct pending_write_data
    {
        runtime_transaction_id_t trans_id;
        runtime_handle_t handle;
        rx_simple_value value;
    };
    typedef std::map<rx_thread_handle_t, std::vector<pending_write_data> > pending_writes_type;
    typedef std::vector<update_item> pending_updates_type;
    typedef std::vector<std::pair<runtime_transaction_id_t, std::vector<std::pair<runtime_handle_t, rx_result> > > > pending_write_results_type;

    typedef std::map<runtime_transaction_id_t, subscription_write_transaction> write_transactions_type;

    friend class runtime_connection_data;

  public:
      rx_subscription (rx_subscription_callback* callback);

      ~rx_subscription();


      rx_result connect_items (const string_array& paths, std::vector<rx_result_with<runtime_handle_t> >& result);

      rx_result disconnect_items (const std::vector<runtime_handle_t>& items, std::vector<rx_result>& results);

      void activate ();

      void deactivate ();

      void items_changed (const std::vector<update_item>& items);

      void transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items);

      rx_thread_handle_t get_target ();

      rx_result write_items (runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_simple_value> >&& values, std::vector<rx_result>& result);

      void write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result);


  protected:

  private:

      void process_subscription (bool posted = false);

      runtime_connection_data* get_connection (runtime_handle_t handle);

      rx_subscription_tag* get_tag (runtime_handle_t handle);

      void process_writes ();

      void process_results ();



      rx_subscription_callback *callback_;

      connections_type connections_;

      rx_reference<rx::jobs::periodic_job> timer_;

      write_transactions_type write_transactions_;


      handles_type handles_;

      inverse_tags_type inverse_tags_;

      tags_type tag_paths_;

      locks::slim_lock items_lock_;

      connection_attempts_type attempts_;

      std::atomic<bool> active_;

      rx_thread_handle_t target_;

      connection_paths_type connection_paths_;

      pending_writes_type pending_writes_;

      pending_updates_type pending_updates_;

      std::unordered_set<size_t> to_retrieve_;

      std::unordered_set<size_t> to_process_;

      subscription_write_transaction::transactions_map_type transactions_map_;

      pending_write_results_type pending_write_results_;

      values_cache_type values_cache_;


};


} // namespace subscriptions
} // namespace sys_runtime
} // namespace rx_internal



#endif
