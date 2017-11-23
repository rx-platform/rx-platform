

/****************************************************************************
*
*  system\server\rx_inf.h
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_inf_h
#define rx_inf_h 1



// rx_objbase
#include "system/meta/rx_objbase.h"
// rx_values
#include "lib/rx_values.h"
// rx_job
#include "lib/rx_job.h"
// rx_thread
#include "lib/rx_thread.h"

#include "system/hosting/rx_host.h"
using server::ns::namespace_item_attributes;

#define RX_DOMAIN_SLOW 0xfffd
#define RX_DOMAIN_IO 0xfffe
#define RX_DOMAIN_GENERAL 0xffff



namespace server {

namespace runtime {






class server_dispatcher_object : public objects::server_object  
{
	DECLARE_REFERENCE_PTR(server_dispatcher_object);
	
	DECLARE_CODE_INFO("rx",1,0,0, "\
class managing io operation resources\r\n\
and general usage thread pool resources\r\n\
");

  public:
      server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id, const rx_node_id& id);

      virtual ~server_dispatcher_object();


      rx_reference<rx::threads::dispatcher_pool> get_pool ()
      {
        return _pool;
      }



  protected:

      void get_items (server_items_type& sub_items, const string_type& pattern) const;


  private:


      rx_reference<rx::threads::dispatcher_pool> _pool;


      objects::server_const_value_item<uint32_t> _threads_count;


};






class dispatcher_subscribers_job : public rx::jobs::periodic_job  
{
	DECLARE_REFERENCE_PTR(dispatcher_subscribers_job);

  public:
      dispatcher_subscribers_job();

      virtual ~dispatcher_subscribers_job();


      void process ();


  protected:

  private:


};






class domains_pool : public rx::threads::job_thread, 
                     	public ob
{
	DECLARE_REFERENCE_PTR(domains_pool);
	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;
	DECLARE_DERIVED_FROM_INTERFACE;

	DECLARE_CODE_INFO("rx", 0,5,0, "\
class managing execution domains \r\n\
thread pool resources\r\n\
");

	typedef std::vector<threads::physical_job_thread*> workers_type;

  public:
      domains_pool (size_t pool_size);

      virtual ~domains_pool();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);

      void reserve ();

      void clear ();

      void append (rx::jobs::timer_job_ptr job, uint32_t domain);

      rx::threads::job_thread* get_executer (rx_thread_handle_t domain);


  protected:

  private:


      workers_type _workers;


      objects::server_const_value_item<uint32_t> _pool_size;


};


struct runtime_data_t
{
	runtime_data_t()
	{
		memzero(this, sizeof(runtime_data_t));
		io_pool_size = -1;
		genereal_pool_size = -1;
		workers_pool_size = -1;
		slow_pool_size = -1;
		io_timer_period = 200;
	}
	int io_pool_size;
	int genereal_pool_size;
	int workers_pool_size;
	int slow_pool_size;
	bool has_callculation_timer;
	uint32_t io_timer_period;
};





class server_rt : public objects::server_object  
{
	DECLARE_CODE_INFO("rx",1,0,0, "\
class managing runtime resources:\r\n\
i/o pool, general pool,\
and thread pool for applications.\r\n\
also contains 2 timers:\r\n\
general ( high priority )\r\n\
callculation ( normal priority)");

	DECLARE_REFERENCE_PTR(server_rt);
	typedef std::vector<rx::threads::physical_job_thread::smart_ptr> workers_type;

  public:
      server_rt();

      virtual ~server_rt();


      uint32_t initialize (hosting::rx_server_host* host, runtime_data_t& data);

      uint32_t deinitialize ();

      void append_timer_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now = false);

      uint32_t start (hosting::rx_server_host* host, const runtime_data_t& data);

      uint32_t stop ();

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      void get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const;

      namespace_item_attributes get_attributes () const;

      void append_job (rx::jobs::job_ptr job);

      void append_calculation_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now = false);

      void append_io_job (rx::jobs::job_ptr job);

      void append_general_job (rx::jobs::job_ptr job);

      void append_slow_job (rx::jobs::job_ptr job);

      void append_timer_io_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now = false);


      rx_reference<server_dispatcher_object> get_io_pool ()
      {
        return _io_pool;
      }


      rx_reference<server_dispatcher_object> get_general_pool ()
      {
        return _general_pool;
      }


      const rx_reference<domains_pool> get_workers () const
      {
        return _workers;
      }


	  server_item_ptr get_item_ptr() { return server_item_ptr::create_from_pointer(this); }
  protected:

      void get_items (server_items_type& sub_items, const string_type& pattern) const;

      void virtual_bind ();

      void virtual_release ();

      rx::threads::job_thread* get_executer (rx_thread_handle_t domain);


  private:


      rx_reference<rx::threads::timer> _general_timer;

      rx_reference<rx::threads::timer> _callculation_timer;

      rx_reference<server_dispatcher_object> _io_pool;

      rx_reference<server_dispatcher_object> _general_pool;

      rx_reference<dispatcher_subscribers_job> _dispatcher_timer;

      rx_reference<domains_pool> _workers;


};


} // namespace runtime
} // namespace server



#endif
