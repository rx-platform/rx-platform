

/****************************************************************************
*
*  runtime_internal\rx_runtime_internal.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef rx_runtime_internal_h
#define rx_runtime_internal_h 1



// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"

#include "rx_runtime_algorithms.h"
#include "sys_internal/rx_inf.h"
#include "api/rx_platform_api.h"
using rx_platform::runtime::items::application_instance_data;
using rx_platform::runtime::items::domain_instance_data;
using rx_platform::runtime_data_t;


namespace rx_internal {

namespace sys_runtime {





class runtime_cache 
{
    typedef std::map<string_type, platform_item_ptr> path_cache_type;

  public:

      void add_to_cache (platform_item_ptr&& item);

      std::vector<platform_item_ptr> get_items (const string_array& paths);

      platform_item_ptr get_item (const string_type& path);

      void remove_from_cache (const string_type& path);


  protected:

  private:


      locks::slim_lock lock_;

      path_cache_type path_cache_;


};







class platform_runtime_manager 
{
	typedef std::map<rx_node_id, rx_application_ptr> applications_type;
	typedef std::vector<int> coverage_type;
	friend class algorithms::application_algorithms;

  public:

      static platform_runtime_manager& instance ();

      rx_thread_handle_t resolve_app_processor (const application_instance_data& data);

      rx_thread_handle_t resolve_domain_processor (const domain_instance_data& data);

      void remove_one (rx_thread_handle_t from_where);

      rx_result initialize (hosting::rx_platform_host* host, runtime_data_t& data);

      void get_applications (api::query_result& result, const string_type& path);

      static runtime_handle_t get_new_handle ();


      runtime_cache& get_cache ()
      {
        return cache_;
      }


	  template<class typeT>
	  rx_result init_runtime(typename typeT::RTypePtr what)
	  {
          auto result = algorithms::create_runtime_structure<typeT>(what);
          auto ctx = what->create_init_context();
		  result = algorithms::init_runtime<typeT>(what, ctx);
		  return result;
	  }
	  template<class typeT>
	  rx_result deinit_runtime(typename typeT::RTypePtr what, std::function<void(rx_result)> callback)
	  {
          auto result = algorithms::delete_runtime_structure<typeT>(what);
          runtime::runtime_deinit_context ctx;
		  result = algorithms::deinit_runtime<typeT>(what, callback, ctx);
		  return result;
	  }
  protected:

  private:

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
