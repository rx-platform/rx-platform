

/****************************************************************************
*
*  lib\rx_thread.h
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


#ifndef rx_thread_h
#define rx_thread_h 1


#include "lib/rx_lock.h"

// rx_ptr
#include "lib/rx_ptr.h"

namespace rx {
namespace jobs {
class timer_job;
class job;

} // namespace jobs
} // namespace rx




namespace rx {
namespace jobs
{
typedef pointers::reference<job> job_ptr;
typedef pointers::reference<timer_job> timer_job_ptr;
}
using namespace jobs;

namespace threads {





class thread : public locks::waitable  
{

  public:
      thread();

      thread (const string_type& name);

      virtual ~thread();


      void start (int priority = RX_PRIORITY_NORMAL);


  protected:

      virtual uint32_t handler () = 0;


  private:


      static void _inner_handler (void* arg);


      string_type _name;

      uint32_t _thread_id;


};

typedef pointers::reference<thread> thread_ptr;





class job_aware_thread : public pointers::interface_object  
{
	DECLARE_INTERFACE_PTR(job_aware_thread);

  public:
      job_aware_thread();

      virtual ~job_aware_thread();


      virtual void run (int priority = RX_PRIORITY_NORMAL) = 0;

      virtual void end (uint32_t timeout = RX_INFINITE) = 0;


  protected:

  private:


};







class job_thread : public job_aware_thread  
{
	DECLARE_INTERFACE_PTR(job_thread);

  public:
      job_thread();

      virtual ~job_thread();


      virtual void append (job_ptr pjob) = 0;


  protected:

  private:


};






class physical_job_thread : public thread, 
                   
                        
                            	public
{
	DECLARE_REFERENCE_PTR(physical_job_thread);

	DECLARE_DERIVED_FROM_INTERFACE;
	typedef std::queue<job_ptr> queue_type;

  public:
      physical_job_thread();

      physical_job_thread (const string_type& name);

      virtual ~physical_job_thread();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);


  protected:

      uint32_t handler ();

      bool wait (std::vector<job_ptr>& queued, uint32_t timeout = RX_INFINITE);

      void stop (uint32_t timeout = RX_INFINITE);

      void virtual_bind ();

      void virtual_release ();


  private:


      queue_type _queue;

      jobs::job_ptr _current;


      locks::event _has_job;


};






class dispatcher_thread : public thread  
{

  public:
      dispatcher_thread (const string_type& name, rx_kernel_dispather_t dispatcher);

      virtual ~dispatcher_thread();


  protected:

      uint32_t handler ();


  private:


      rx_kernel_dispather_t _dispatcher;


};







class dispatcher_pool : public job_thread, 
                        	public poi
{
	DECLARE_REFERENCE_PTR(dispatcher_pool);
	DECLARE_DERIVED_FROM_INTERFACE;
	typedef std::vector<dispatcher_thread*> threads_type;

  public:
      dispatcher_pool (int count, const string_type& name);

      virtual ~dispatcher_pool();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);


      rx_kernel_dispather_t _dispatcher;


  protected:

      void virtual_bind ();

      void virtual_release ();


  private:


      threads_type _threads;


      string_type _name;


};







class timer : public thread, 
              	public lo
              	public pointers::ref
{
	DECLARE_REFERENCE_PTR(timer);
	typedef std::set<jobs::timer_job_ptr> jobs_type;

  public:
      timer();

      virtual ~timer();


      void stop ();

      void append_job (timer_job_ptr job, job_thread* executer, uint32_t period, bool now = false);


  protected:

      uint32_t handler ();


  private:

      void wake_up ();



      jobs_type _jobs;


      locks::event _wake_up;

      bool _should_exit;


};


} // namespace threads
} // namespace rx



#endif
