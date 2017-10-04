

/****************************************************************************
*
*  lib\rx_job.h
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


#ifndef rx_job_h
#define rx_job_h 1



// rx_lock
#include "lib/rx_lock.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_thread
#include "lib/rx_thread.h"



namespace rx {
namespace threads
{
class smart_thread_pool;

void execute_job(void* arg);

}

namespace jobs {


//	basic job class



class job : private pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(job);

  public:
      job();

      virtual ~job();


      void cancel ();

      bool is_canceled () const;

      virtual void process () = 0;


      const rx_security_handle_t get_security_context () const
      {
        return m_security_context;
      }


      const rx_thread_handle_t get_destination () const
      {
        return m_destination;
      }



  protected:

      void un_cancel ();


  private:

      job* get_unsafe_ptr ();

      void release_unsafe_ptr ();



      bool m_canceled;

      rx_security_handle_t m_security_context;

      rx_thread_handle_t m_destination;

	  friend void threads::execute_job(void*);
    friend class threads::dispatcher_pool;
};






class timer_job : public job, 
                  	publi
{
	DECLARE_REFERENCE_PTR(timer_job);

  public:
      timer_job();

      virtual ~timer_job();


      virtual dword tick (dword current_tick, bool& remove) = 0;

      void set_executer (threads::job_thread* executer);


  protected:

      threads::job_thread *m_executer;


      dword m_next;

      dword m_period;


  private:


      threads::timer *m_my_timer;


    friend class threads::timer;
};






class post_period_job : public timer_job  
{
	DECLARE_REFERENCE_PTR(post_period_job);

  public:
      post_period_job();

      virtual ~post_period_job();


      dword tick (dword current_tick, bool& remove);


  protected:

  private:


};






template <typename argT>
class labmda_job : public job  
{
	DECLARE_REFERENCE_PTR(labmda_job);

  public:
      labmda_job (std::function<void(argT)> f, argT args)
            : m_f(f),
              m_args(args)
      {
      }

      virtual ~labmda_job()
      {
      }


      void process ()
      {
		  (m_f)(m_args);
      }


  protected:

  private:


      std::function<void(argT)> m_f;

      argT m_args;


};






class periodic_job : public timer_job  
{
	DECLARE_REFERENCE_PTR(periodic_job);

  public:
      periodic_job();

      virtual ~periodic_job();


      dword tick (dword current_tick, bool& remove);


  protected:

  private:


};


// Parameterized Class rx::jobs::labmda_job 


} // namespace jobs
} // namespace rx



#endif
