

/****************************************************************************
*
*  sys_internal\rx_inf.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_inf_h
#define rx_inf_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_job
#include "system/threads/rx_job.h"
// rx_thread
#include "system/threads/rx_thread.h"

namespace rx_internal {
namespace sys_runtime {
namespace data_source {
class data_controler;

} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal


#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"

using rx_platform::namespace_item_attributes;
using namespace rx_platform;

#define RX_PRIORITY_FROM_DOMAIN(d) ((uint8_t)(((d)>>16)&0xff))
#define RX_ADD_PRIORITY_TO_DOMAIN(p) ((rx_thread_handle_t)(p<<16))
#define RX_DOMAIN_TYPE_MASK 0xffff
#define RX_DOMAIN_UPPER_LIMIT 0xff00




namespace rx_internal {

namespace infrastructure {





class server_dispatcher_object : public rx_platform::runtime::items::object_runtime  
{
	DECLARE_REFERENCE_PTR(server_dispatcher_object);
	
	DECLARE_CODE_INFO("rx",0,9,0, "\
class managing kernel based pool \r\n\
used for I/O pool and general pool\r\n\
");

  public:
      server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id, uint64_t cpu_mask = 0);

      ~server_dispatcher_object();


      int get_CPU (rx_thread_handle_t domain) const;

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      uint16_t get_pool_size () const;


      rx_platform::threads::dispatcher_pool& get_pool ()
      {
        return pool_;
      }



  protected:

  private:


      rx_platform::threads::dispatcher_pool pool_;


      int threads_count_;


};






class dispatcher_subscribers_job : public rx_platform::jobs::periodic_job  
{
	DECLARE_REFERENCE_PTR(dispatcher_subscribers_job);

  public:
      dispatcher_subscribers_job();

      ~dispatcher_subscribers_job();


      void process ();


  protected:

  private:


};






class domains_pool : public rx_platform::threads::job_thread, 
                     	public rx_platform::runtime::items::object_runtime  
{
	DECLARE_REFERENCE_PTR(domains_pool);

	DECLARE_CODE_INFO("rx", 0,5,1, "\
class managing execution domains \r\n\
thread pool resources\r\n\
");

	typedef std::vector<threads::physical_job_thread*> workers_type;
	typedef std::vector<rx_internal::sys_runtime::data_source::data_controler*> data_controlers_type;

  public:
      domains_pool (uint32_t pool_size, uint32_t start_cpu, uint32_t end_cpu);

      ~domains_pool();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);

      void reserve ();

      void clear ();

      void append (timer_job_ptr job, uint32_t domain);

      threads::job_thread* get_executer (rx_thread_handle_t domain);

      rx_internal::sys_runtime::data_source::data_controler* get_data_controler (rx_thread_handle_t domain);

      int get_CPU (rx_thread_handle_t domain) const;

      uint16_t get_pool_size () const;


  protected:

  private:


      workers_type workers_;

      data_controlers_type data_controlers_;


      uint32_t pool_size_;

      uint32_t start_cpu_;

      uint32_t end_cpu_;


};






class physical_thread_object : public rx_platform::runtime::items::object_runtime  
{
    DECLARE_REFERENCE_PTR(physical_thread_object);

    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
class managing physical thread as pool \r\n\
used for special executer types\r\n\
");

  public:
      physical_thread_object (const string_type& name, rx_thread_handle_t rx_thread_id);

      ~physical_thread_object();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);


      rx_platform::threads::physical_job_thread& get_pool ()
      {
        return pool_;
      }


      sys_runtime::data_source::data_controler * get_data_controler ()
      {
        return data_controler_;
      }



  protected:

  private:


      rx_platform::threads::physical_job_thread pool_;

      sys_runtime::data_source::data_controler *data_controler_;


};







class server_runtime : public rx_platform::runtime::items::object_runtime  
{
	DECLARE_CODE_INFO("rx",2,0,0, "\
class managing runtime resources:\r\n\
i/o pool, general pool,\
and thread pool for applications.\r\n\
also contains 2 timers:\r\n\
general ( high priority )\r\n\
calculation ( normal priority)");

	DECLARE_REFERENCE_PTR(server_runtime);
	typedef std::array<rx_reference<domains_pool>, (size_t)rx_domain_priority::priority_count> workers_type;

	friend void rx_post_function(std::function<void(void)> f, rx_thread_handle_t whome);

  public:
      ~server_runtime();


      rx_result initialize (hosting::rx_platform_host* host, runtime_data_t& data, const io_manager_data_t& io_data);

      void deinitialize ();

      void append_timer_job (timer_job_ptr job, threads::job_thread* whose = nullptr);

      rx_result start (hosting::rx_platform_host* host, const runtime_data_t& data, const io_manager_data_t& io_data);

      void stop ();

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      void get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const;

      namespace_item_attributes get_attributes () const;

      void append_job (job_ptr job);

      threads::job_thread* get_executer (rx_thread_handle_t domain);

      void append_calculation_job (timer_job_ptr job, threads::job_thread* whose = nullptr);

      void append_io_job (job_ptr job);

      void append_slow_job (job_ptr job);

      void append_timer_io_job (timer_job_ptr job);

      rx_time get_created_time (values::rx_value& val) const;

      rx_internal::sys_runtime::data_source::data_controler* get_data_controler (rx_thread_handle_t domain);

      int get_CPU (rx_thread_handle_t domain) const;

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      static server_runtime& instance ();

      runtime_data_t get_cpu_data ();


      rx_reference<server_dispatcher_object> get_io_pool ()
      {
        return io_pool_;
      }


      rx_reference<physical_thread_object>& get_unassigned_pool ()
      {
        return unassigned_pool_;
      }



  protected:

  private:
      server_runtime();



      std::unique_ptr<rx_platform::threads::timer> general_timer_;

      std::unique_ptr<rx_platform::threads::timer> calculation_timer_;

      rx_reference<server_dispatcher_object> io_pool_;

      rx_reference<physical_thread_object> unassigned_pool_;

      rx_reference<dispatcher_subscribers_job> dispatcher_timer_;

      workers_type workers_;

      rx_reference<physical_thread_object> meta_pool_;

      rx_reference<server_dispatcher_object> slow_pool_;


      threads::job_thread* extern_executer_;


};


} // namespace infrastructure
} // namespace rx_internal



#endif
