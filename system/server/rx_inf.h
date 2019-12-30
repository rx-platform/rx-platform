

/****************************************************************************
*
*  system\server\rx_inf.h
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


#ifndef rx_inf_h
#define rx_inf_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_job
#include "lib/rx_job.h"
// rx_thread
#include "lib/rx_thread.h"

namespace sys_runtime {
namespace data_source {
class data_controler;

} // namespace data_source
} // namespace sys_runtime


#include "system/hosting/rx_host.h"
using rx_platform::namespace_item_attributes;

#define RX_DOMAIN_EXTERN 0xffb
#define RX_DOMAIN_META 0xfffc
#define RX_DOMAIN_SLOW 0xfffd
#define RX_DOMAIN_IO 0xfffe
#define RX_DOMAIN_GENERAL 0xffff



namespace rx_platform {
struct io_manager_data_t;

namespace infrastructure {





class server_dispatcher_object : public runtime::objects::object_runtime  
{
	DECLARE_REFERENCE_PTR(server_dispatcher_object);
	
	DECLARE_CODE_INFO("rx",0,8,0, "\
class managing io operation resources\r\n\
and general usage thread pool resources\r\n\
");

  public:
      server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id, const rx_node_id& id);

      ~server_dispatcher_object();


      rx::threads::dispatcher_pool& get_pool ()
      {
        return pool_;
      }



  protected:

  private:


      rx::threads::dispatcher_pool pool_;


      int threads_count_;


};






class dispatcher_subscribers_job : public rx::jobs::periodic_job  
{
	DECLARE_REFERENCE_PTR(dispatcher_subscribers_job);

  public:
      dispatcher_subscribers_job();

      ~dispatcher_subscribers_job();


      void process ();


  protected:

  private:


};






class domains_pool : public rx::threads::job_thread, 
                     	public runtime::objects::object_runtime  
{
	DECLARE_REFERENCE_PTR(domains_pool);

	DECLARE_CODE_INFO("rx", 0,5,0, "\
class managing execution domains \r\n\
thread pool resources\r\n\
");

	typedef std::vector<threads::physical_job_thread*> workers_type;
	typedef std::vector<sys_runtime::data_source::data_controler*> data_controlers_type;

  public:
      domains_pool (uint32_t pool_size);

      ~domains_pool();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);

      void reserve ();

      void clear ();

      void append (rx::jobs::timer_job_ptr job, uint32_t domain);

      rx::threads::job_thread* get_executer (rx_thread_handle_t domain);

      sys_runtime::data_source::data_controler* get_data_controler (rx_thread_handle_t domain);


  protected:

  private:


      workers_type workers_;

      data_controlers_type data_controlers_;


      uint32_t pool_size_;


};


struct runtime_data_t
{
	bool real_time = false;
	int io_pool_size = -1;
	int genereal_pool_size = -1;
	int workers_pool_size = -1;
	int slow_pool_size = -1;
	bool has_calculation_timer = false;
	threads::job_thread* extern_executer = nullptr;
};





class server_rt : public runtime::objects::object_runtime  
{
	DECLARE_CODE_INFO("rx",1,0,0, "\
class managing runtime resources:\r\n\
i/o pool, general pool,\
and thread pool for applications.\r\n\
also contains 2 timers:\r\n\
general ( high priority )\r\n\
callculation ( normal priority)");

	DECLARE_REFERENCE_PTR(server_rt);
	typedef std::vector<std::unique_ptr<rx::threads::physical_job_thread> > workers_type;

	friend void rx_post_function(std::function<void(void)> f, rx_thread_handle_t whome);

  public:
      server_rt();

      ~server_rt();


      rx_result initialize (hosting::rx_platform_host* host, runtime_data_t& data, const io_manager_data_t& io_data);

      void deinitialize ();

      void append_timer_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now = false);

      rx_result start (hosting::rx_platform_host* host, const runtime_data_t& data, const io_manager_data_t& io_data);

      void stop ();

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      void get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const;

      namespace_item_attributes get_attributes () const;

      void append_job (rx::jobs::job_ptr job);

      rx::threads::job_thread* get_executer (rx_thread_handle_t domain);

      void append_calculation_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now = false);

      void append_io_job (rx::jobs::job_ptr job);

      void append_general_job (rx::jobs::job_ptr job);

      void append_slow_job (rx::jobs::job_ptr job);

      void append_timer_io_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now = false);

      rx_time get_created_time (values::rx_value& val) const;

      sys_runtime::data_source::data_controler* get_data_controler (rx_thread_handle_t domain);


      rx_reference<server_dispatcher_object> get_io_pool ()
      {
        return io_pool_;
      }


      rx_reference<server_dispatcher_object> get_general_pool ()
      {
        return general_pool_;
      }


      rx_reference<domains_pool> get_workers () const
      {
        return workers_;
      }



  protected:

  private:


      std::unique_ptr<rx::threads::timer> general_timer_;

      std::unique_ptr<rx::threads::timer> calculation_timer_;

      rx_reference<server_dispatcher_object> io_pool_;

      rx_reference<server_dispatcher_object> general_pool_;

      rx_reference<dispatcher_subscribers_job> dispatcher_timer_;

      rx_reference<domains_pool> workers_;


      threads::job_thread* extern_executer_;


};


} // namespace infrastructure
} // namespace rx_platform



#endif
