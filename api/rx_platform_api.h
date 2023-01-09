

/****************************************************************************
*
*  api\rx_platform_api.h
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


#ifndef rx_platform_api_h
#define rx_platform_api_h 1



// rx_job
#include "system/threads/rx_job.h"

#include "rx_library.h"
#include "system/server/rx_log.h"
#include "system/server/rx_ns.h"
#include "system/runtime/rx_objbase.h"
#include "system/meta/rx_obj_types.h"
#include "system/meta/rx_types.h"
#include "platform_api/rx_abi.h"

using rx_platform::meta::meta_data;
using namespace rx_platform::meta;


#ifdef __cplusplus
extern "C" {
#endif

	struct platform_api_t;

#ifdef __cplusplus
}
#endif

namespace rx_platform
{
namespace api
{


void bind_plugins_dynamic_api();
const platform_api_t* get_plugins_dynamic_api();

struct query_result_detail
{
	query_result_detail(const rx_item_type type, const meta_data& data)
	{
		this->type = type;
		this->data = data;
	}
	query_result_detail(const rx_item_type type, meta_data&& data) noexcept
	{
		this->type = type;
		this->data = std::move(data);
	}
	rx_item_type type;
	meta_data data;
};

struct query_result
{
	std::vector<query_result_detail> items;

	bool success = false;
	operator bool() const
	{
		return success;
	}
};



struct rx_context
{
	string_type active_path;
	rx_reference<reference_object> object;
};

}
}


namespace rx_platform {





class extern_timer_job : public jobs::periodic_job  
{

  public:
      extern_timer_job (plugin_job_struct* extern_data);


      void process ();


  protected:

  private:


      rx_reference_ptr anchor_;

      plugin_job_struct* extern_data_;


};






class extern_timers 
{
    typedef std::map<runtime_handle_t, jobs::periodic_job::smart_ptr> timers_type;

  public:

      static extern_timers& instance ();

      runtime_handle_t create_timer (plugin_job_struct* job_impl, uint32_t period, threads::job_thread* pool);

      runtime_handle_t create_calc_timer (plugin_job_struct* job_impl, uint32_t period, threads::job_thread* pool);

      runtime_handle_t create_io_timer (plugin_job_struct* job_impl, uint32_t period);

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);


  protected:

  private:


      timers_type timers_;


      locks::slim_lock lock_;


};






class extern_job : public jobs::job  
{

  public:
      extern_job (plugin_job_struct* extern_data);


      void process ();


  protected:

  private:


      plugin_job_struct* extern_data_;

      rx_reference_ptr anchor_;


};






class extern_period_job : public jobs::post_period_job  
{

  public:
      extern_period_job (plugin_job_struct* extern_data);


      void process ();


  protected:

  private:


      rx_reference_ptr anchor_;

      plugin_job_struct* extern_data_;


};


} // namespace rx_platform



#endif
