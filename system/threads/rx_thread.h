

/****************************************************************************
*
*  system\threads\rx_thread.h
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


#ifndef rx_thread_h
#define rx_thread_h 1


#include "lib/rx_lock.h"
#include "lib/rx_ptr.h"




#ifdef RX_MIN_MEMORY
#define RX_OFFSET_TIMES_SIZE 0x20
#else
#define RX_OFFSET_TIMES_SIZE 0x400
#endif

bool rx_push_thread_context(rx_thread_handle_t obj);

#define RX_THREAD_NULL 0ull

namespace rx_platform
{



namespace jobs
{
class job;
class timer_job;
}
using namespace jobs;
typedef pointers::reference<job> job_ptr;
typedef pointers::reference<timer_job> timer_job_ptr;
}



namespace rx_platform {
namespace jobs {
class timer_job;
class job;

} // namespace jobs
} // namespace rx_platform




namespace rx_platform {

namespace threads {





class thread : public rx::locks::waitable  
{

  public:
      thread (const string_type& name, rx_thread_handle_t rx_thread_id);

      ~thread();


      void start (int priority = RX_PRIORITY_NORMAL);

      static void deinitialize ();


      uint32_t get_thread_id () const
      {
        return thread_id_;
      }



  protected:

      virtual uint32_t handler () = 0;


  private:


      static void _inner_handler (void* arg);


      string_type name_;

      uint32_t thread_id_;

      rx_thread_handle_t rx_thread_id_;

      rx_security_handle_t security_context_;


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

      virtual ~job_thread();


      virtual void append (job_ptr pjob) = 0;


  protected:

  private:


};






class physical_job_thread : public thread, 
                            	public job_thread  
{
	typedef std::queue<job_ptr> queue_type;

  public:
      physical_job_thread (const string_type& name, rx_thread_handle_t rx_thread_id, uint64_t cpu_mask = 0);

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

      uint64_t cpu_mask_;


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
      dispatcher_pool (int count, const string_type& name, rx_thread_handle_t rx_thread_id, uint64_t cpu_mask = 0);

      ~dispatcher_pool();


      void run (int priority = RX_PRIORITY_NORMAL);

      void end (uint32_t timeout = RX_INFINITE);

      void append (job_ptr pjob);

      int get_CPU (rx_thread_handle_t domain) const;


      rx_kernel_dispather_t dispatcher_;


  protected:

  private:


      threads_type threads_;


      string_type name_;

      uint64_t cpu_mask_;


};







class timer : public thread  
{
	typedef std::set<timer_job_ptr> jobs_type;

  public:
      timer (const string_type& name, rx_thread_handle_t rx_thread_id);

      ~timer();


      void stop ();

      void append_job (timer_job_ptr job, job_thread* executer);


  protected:

      uint32_t handler ();


  private:

      void wake_up ();

      void init_random_sequences ();

      rx_timer_ticks_t get_random_time_offset (job& whose);



      jobs_type jobs_;


      locks::event wake_up_;

      bool should_exit_;

      locks::lockable lock_;

      rx_timer_ticks_t medium_randoms_[RX_OFFSET_TIMES_SIZE];

      rx_timer_ticks_t soft_randoms_[RX_OFFSET_TIMES_SIZE];

      int soft_random_index_;

      int medium_random_index_;

      bool real_time_;


    friend class jobs::timer_job;
};


} // namespace threads
} // namespace rx_platform



#endif
