

/****************************************************************************
*
*  lib\rx_thread.h
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


#ifndef rx_thread_h
#define rx_thread_h 1


#include "lib/rx_lock.h"
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
      thread (const string_type& name, rx_thread_handle_t rx_thread_id);

      ~thread();


      void start (int priority = RX_PRIORITY_NORMAL);


  protected:

      virtual uint32_t handler () = 0;


  private:


      static void _inner_handler (void* arg);


      string_type name_;

      uint32_t thread_id_;

      rx_thread_handle_t rx_thread_id_;


};

typedef pointers::reference<thread> thread_ptr;






class job_thread 
{
	job_thread& operator=(const job_thread&) = delete;
	job_thread& operator=(job_thread&&) = delete;
	job_thread(const job_thread&) = delete;
	job_thread(job_thread&&) = delete;

  public:
      job_thread();

      ~job_thread();


      virtual void append (job_ptr pjob) = 0;


  protected:

  private:


};






class dispatcher_thread : public thread  
{

  public:
      dispatcher_thread (const string_type& name, rx_thread_handle_t rx_thread_id, rx_kernel_dispather_t dispatcher);

      ~dispatcher_thread();


  protected:

      uint32_t handler ();


  private:


      rx_kernel_dispather_t dispatcher_;


};







class dispatcher_pool : public job_thread  
{
	typedef std::vector<std::unique_ptr<dispatcher_thread> > threads_type;

  public:
      dispatcher_pool (int count, const string_type& name, rx_thread_handle_t rx_thread_id);

      ~dispatcher_pool();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);


      rx_kernel_dispather_t dispatcher_;


  protected:

  private:


      threads_type threads_;


      string_type name_;


};







class timer : public thread  
{
	typedef std::set<jobs::timer_job_ptr> jobs_type;

  public:
      timer (const string_type& name, rx_thread_handle_t rx_thread_id);

      ~timer();


      void stop ();

      void append_job (timer_job_ptr job, job_thread* executer, uint32_t period, bool now = false);


  protected:

      uint32_t handler ();


  private:

      void wake_up ();



      jobs_type jobs_;


      locks::event wake_up_;

      bool should_exit_;

      locks::lockable lock_;


};






class physical_job_thread : public thread, 
                            	public job_thread  
{
	typedef std::queue<job_ptr> queue_type;

  public:
      physical_job_thread (const string_type& name, rx_thread_handle_t rx_thread_id);

      ~physical_job_thread();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);


  protected:

      uint32_t handler ();

      bool wait (std::vector<job_ptr>& queued, uint32_t timeout = RX_INFINITE);

      void stop (uint32_t timeout = RX_INFINITE);


  private:


      queue_type queue_;

      rx_reference<jobs::job> current_;


      locks::event has_job_;

      locks::lockable lock_;


};


} // namespace threads
} // namespace rx



#endif
