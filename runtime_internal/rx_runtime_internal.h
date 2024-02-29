

/****************************************************************************
*
*  runtime_internal\rx_runtime_internal.h
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


#ifndef rx_runtime_internal_h
#define rx_runtime_internal_h 1



// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"

namespace rx_platform {
namespace runtime {
namespace resolvers {
class runtime_subscriber;

} // namespace resolvers
} // namespace runtime
} // namespace rx_platform


#include "rx_runtime_algorithms.h"
#include "sys_internal/rx_inf.h"
#include "api/rx_platform_api.h"
using rx_internal::sys_runtime::runtime_core::runtime_data::application_instance_data;
using rx_internal::sys_runtime::runtime_core::runtime_data::domain_instance_data;
using rx_platform::runtime_data_t;


using namespace rx_platform::runtime;


namespace rx_internal {

namespace sys_runtime {

class runtime_cache;
template<typename typeT>
typename typeT::RTypePtr get_runtime_impl(runtime_cache* whose, const rx_node_id& id);





class runtime_cache 
{

    typedef std::vector< std::pair<runtime::resolvers::runtime_subscriber*, rx_reference_ptr> > subs_list_t;
    struct subscriber_data
    {
        rx_thread_handle_t target;
        string_type name;
        rx_node_id id;
    };
    struct destroyed_data
    {
        runtime::resolvers::runtime_subscriber* whose;
        rx_reference_ptr ref;
        rx_node_id id;
    };
    struct created_data
    {
        runtime::resolvers::runtime_subscriber* whose;
        rx_reference_ptr ref;
        platform_item_ptr item;
    };
    struct runtime_id_data
    {
        platform_item_ptr item;
        std::function<void(const rx_node_id&)> register_f;
        std::function<void(const rx_node_id&)> deleter_f;
    };

    typedef std::map<string_type, std::map<runtime::resolvers::runtime_subscriber*, subscriber_data> > path_subscribers_type;
    typedef std::map<rx_node_id, std::map<runtime::resolvers::runtime_subscriber*, subscriber_data> > id_subscribers_type;
    typedef std::map<string_type, platform_item_ptr> path_cache_type;
    typedef std::map<rx_node_id, rx_object_ptr> objects_cache_type;
    typedef std::map<rx_node_id, rx_port_ptr> ports_cache_type;
    typedef std::map<rx_node_id, rx_domain_ptr> domains_cache_type;
    typedef std::map<rx_node_id, rx_application_ptr> applications_cache_type;
    typedef std::map<rx_node_id, runtime_id_data> id_cahce_type;


    typedef std::map<rx_node_id, rx_relation_ptr> relations_cache_type;
    

    typedef std::map<rx_thread_handle_t, subs_list_t> collected_subscribers_type;

    template<typename typeT>
    friend typename typeT::RTypePtr get_runtime_impl(runtime_cache* whose, const rx_node_id& id);



  public:
      runtime_cache();


      std::vector<platform_item_ptr> get_items (const string_array& paths);

      platform_item_ptr get_item (const rx_node_id& id);

      std::function<void(const rx_node_id&)> remove_from_cache (platform_item_ptr&& item);

      void add_functions (const rx_node_id& id, const std::function<void(const rx_node_id&)>& register_f, const std::function<void(const rx_node_id&)>& deleter_f);

      void remove_functions (const rx_node_id& id);

      void unregister_subscriber (const rx_item_reference& ref, runtime::resolvers::runtime_subscriber* whose);

      void register_subscriber (const rx_item_reference& ref, runtime::resolvers::runtime_subscriber* whose);

      rx_object_ptr get_object (const rx_node_id& id);

      rx_application_ptr get_application (const rx_node_id& id);

      rx_domain_ptr get_domain (const rx_node_id& id);

      rx_port_ptr get_port (const rx_node_id& id);

      void add_to_cache (rx_object_ptr item);

      void add_to_cache (rx_domain_ptr item);

      void add_to_cache (rx_port_ptr item);

      void add_to_cache (rx_application_ptr item);

      rx_result add_target_relation (const rx_node_id& id, relations::relation_data::smart_ptr data);

      rx_result remove_target_relation (const rx_node_id& id, const string_type& name);

      std::vector<platform_item_ptr> get_items (const rx_node_ids& ids);

      void cleanup_cache ();

      rx_relation_ptr get_relation (const rx_node_id& id);

      void add_to_cache (rx_relation_ptr item);

      template<typename typeT>
      typename typeT::RTypePtr get_runtime(const rx_node_id& id)
      {
          return get_runtime_impl<typeT>(this, id);
      }
      runtime_cache(const runtime_cache&) = delete;
      runtime_cache(runtime_cache&&) = delete;
  protected:

  private:

      void add_to_cache (platform_item_ptr&& item, collected_subscribers_type& subscribers);

      void collect_subscribers (const rx_node_id& id, const string_type& name, std::map<rx_thread_handle_t, subs_list_t>& to_send);

      void post_appeared (collected_subscribers_type& subscribers);



      locks::slim_lock lock_;

      path_cache_type path_cache_;

      id_cahce_type id_cache_;

      path_subscribers_type path_subscribers_;

      id_subscribers_type id_subscribers_;

      objects_cache_type objects_cache_;

      applications_cache_type applications_cache_;

      domains_cache_type domains_cache_;

      ports_cache_type ports_cache_;

      relations_cache_type relations_cache_;


};







class platform_runtime_manager 
{
	typedef std::map<rx_node_id, rx_application_ptr> applications_type;
	typedef std::vector<int> coverage_type;
	friend class algorithms::application_algorithms;
    friend class algorithms::shutdown_algorithms;

  public:

      static platform_runtime_manager& instance ();

      rx_thread_handle_t resolve_app_processor (const application_instance_data& data);

      rx_thread_handle_t resolve_domain_processor (const domain_instance_data& data);

      void remove_one (rx_thread_handle_t from_where);

      rx_result initialize (hosting::rx_platform_host* host, runtime_data_t& data);

      void get_applications (api::query_result& result, const string_type& path);

      static runtime_handle_t get_new_handle ();

      static runtime_transaction_id_t get_new_transaction_id ();

      void stop_all ();

      void deinitialize ();


      runtime_cache& get_cache ()
      {
        return cache_;
      }


	  template<class typeT>
	  rx_result init_runtime(typename typeT::RTypePtr what)
	  {
          auto result = algorithms::create_runtime_structure<typeT>(what);
          if (result)
          {
              auto ctx = runtime::algorithms::runtime_holder_algorithms<typeT>::create_init_context(*what);
              result = algorithms::init_runtime<typeT>(what, ctx);
          }
		  return result;
	  }
      template<class typeT>
      rx_result just_init_runtime(typename typeT::RTypePtr what)
      {
          auto result = algorithms::create_runtime_structure<typeT>(what);
          if (result)
          {
              auto ctx = runtime::algorithms::runtime_holder_algorithms<typeT>::create_init_context(*what);
              result = algorithms::just_init_runtime<typeT>(what, ctx);
          }
          return result;
      }

      template<class typeT>
      void just_start_runtime(typename typeT::RTypePtr what, const_callbacks_type callbacks)
      {
          algorithms::just_start_runtime<typeT>(what, std::move(callbacks));
      }
	  template<class typeT>
	  rx_result deinit_runtime(typename typeT::RTypePtr what, rx_result_callback&& callback)
	  {
		  auto result = algorithms::deinit_runtime<typeT>(what, std::move(callback));
		  return result;
	  }
  protected:

  private:
      platform_runtime_manager();


      rx_thread_handle_t resolve_processor_auto ();



      applications_type applications_;

      runtime_cache cache_;


      coverage_type cpu_coverage_;

      rx_thread_handle_t first_cpu_;

      rx_thread_handle_t last_cpu_;


};


} // namespace sys_runtime
} // namespace rx_internal



#endif
