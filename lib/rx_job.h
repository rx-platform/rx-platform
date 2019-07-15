

/****************************************************************************
*
*  lib\rx_job.h
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


#ifndef rx_job_h
#define rx_job_h 1



// rx_ptr
#include "lib/rx_ptr.h"
// rx_thread
#include "lib/rx_thread.h"



namespace rx {
namespace threads
{

void execute_job(void* arg);

}

namespace jobs {
typedef int rx_complexity_t;
const rx_complexity_t rx_default_complextiy = 0x10000;

typedef int job_value_factor_t;
const job_value_factor_t rx_default_value_factor = 0x10000;

//	basic job class



class job : private pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(job);

  public:
      job();


      void cancel ();

      bool is_canceled () const;

      virtual void process () = 0;


      const rx_security_handle_t get_security_context () const
      {
        return security_context_;
      }


      const rx_criticalness get_criticalness () const
      {
        return criticalness_;
      }


      const job_value_factor_t get_value_factor () const
      {
        return value_factor_;
      }


      const rx_complexity_t get_complexity () const
      {
        return complexity_;
      }


      const rx_access get_access_type () const
      {
        return access_type_;
      }



  protected:

      void un_cancel ();


  private:

      job* get_unsafe_ptr ();

      void release_unsafe_ptr ();



      bool canceled_;

      rx_security_handle_t security_context_;

      rx_criticalness criticalness_;

      job_value_factor_t value_factor_;

      rx_complexity_t complexity_;

      rx_access access_type_;

	  friend void threads::execute_job(void*);
    friend class threads::dispatcher_pool;
};






class timer_job : public job  
{
	DECLARE_REFERENCE_PTR(timer_job);

  public:
      timer_job();


      virtual rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, bool& remove) = 0;

      void set_executer (threads::job_thread* executer);

      void lock ();

      void unlock ();


  protected:

      threads::job_thread *executer_;


      rx_timer_ticks_t next_;

      rx_timer_ticks_t period_;


  private:


      threads::timer *my_timer_;


      locks::lockable lock_;


    friend class threads::timer;
};






class post_period_job : public timer_job  
{
	DECLARE_REFERENCE_PTR(post_period_job);

  public:
      post_period_job();


      rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, bool& remove);


  protected:

  private:


};






template <typename argT, typename refT = argT>
class lambda_job : public job  
{
	DECLARE_REFERENCE_PTR(lambda_job);

  public:
      lambda_job (std::function<void(argT)> f, argT&& arg, refT ref)
            : f_(f)
		  , arg_(std::forward<argT>(arg))
		  , ref_(ref)
      {
      }


      void process ()
      {
		  (f_)(std::forward<argT>(arg_));
      }


  protected:

  private:


      std::function<void(argT)> f_;

      argT arg_;

      refT ref_;


};


template <typename argT>
class lambda_job<argT, argT> : public job
{
	DECLARE_REFERENCE_PTR(lambda_job);

public:
	lambda_job(std::function<void(argT)> f, argT arg)
		: f_(f),
		arg_(arg)
	{
	}
	void process()
	{
		(f_)(arg_);
	}

private: 
	std::function<void(argT)> f_;

	argT arg_;

};





class periodic_job : public timer_job  
{
	DECLARE_REFERENCE_PTR(periodic_job);

  public:
      periodic_job();


      rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, bool& remove);


  protected:

  private:


};






template <typename argT, typename refT = argT>
class lambda_period_job : public post_period_job  
{
	DECLARE_REFERENCE_PTR(lambda_period_job);

  public:
      lambda_period_job (std::function<void(argT)> f, argT&& arg, refT ref)
            : f_(f)
		  , arg_(std::forward<argT>(arg))
		  , ref_(ref)
      {
      }


      void process ()
      {
		  cancel();
		  (f_)(std::forward<argT>(arg_));
      }


  protected:

  private:


      std::function<void(argT)> f_;

      argT arg_;

      refT ref_;


};


template <typename argT>
class lambda_period_job<argT, argT> : public post_period_job
{
	DECLARE_REFERENCE_PTR(lambda_period_job);

public:
	lambda_period_job(std::function<void(argT)> f, argT arg)
		: f_(f),
		arg_(arg)
	{
	}
	void process()
	{
		cancel();
		(f_)(arg_);
	}

private:
	std::function<void(argT)> f_;

	argT arg_;

};





template <typename argT, typename refT = argT>
class lambda_timer_job : public periodic_job  
{
	DECLARE_REFERENCE_PTR(lambda_timer_job);

  public:
      lambda_timer_job (std::function<bool(argT)> f, argT&& arg, refT ref)
            : f_(f)
		  , arg_(std::forward<argT>(arg))
		  , ref_(ref)
      {
      }


      void process ()
      {
		  if(!(f_)(std::forward<argT>(arg_)))
			  cancel();
      }


  protected:

  private:


      std::function<bool(argT)> f_;

      argT arg_;

      refT ref_;


};


template <typename argT>
class lambda_timer_job<argT, argT> : public periodic_job
{
	DECLARE_REFERENCE_PTR(lambda_timer_job);

public:
	lambda_timer_job(std::function<bool(argT)> f, argT arg)
		: f_(f),
		arg_(arg)
	{
	}
	void process()
	{
		if (!(f_)(arg_))
			cancel();
	}

private:
	std::function<bool(argT)> f_;

	argT arg_;

};





template <typename argT, typename refT>
class result_lambda_job : public job  
{
	DECLARE_REFERENCE_PTR(result_lambda_job);

  public:
      result_lambda_job (std::function<void(rx_result_with<argT>&&)> f, rx_result_with<argT>&& arg, refT ref)
            : f_(f)
		  , argument_(std::forward<argT>(arg))
		  , ref_(ref)
      {
      }


      void process ()
      {
		  f_(std::move(argument_));
      }


  protected:

  private:


      std::function<void(argT)> f_;

      rx_result_with<argT> argument_;

      refT ref_;


};


// Parameterized Class rx::jobs::lambda_job 


// Parameterized Class rx::jobs::lambda_period_job 


// Parameterized Class rx::jobs::lambda_timer_job 


// Parameterized Class rx::jobs::result_lambda_job 


} // namespace jobs
} // namespace rx



#endif
