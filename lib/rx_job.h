

/****************************************************************************
*
*  lib\rx_job.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_job_h
#define rx_job_h 1



// rx_func_to_go
#include "lib/rx_func_to_go.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_thread
#include "lib/rx_thread.h"



#include "lib/rx_templates.h"


namespace rx {
namespace threads
{

void execute_job(void* arg);

}

namespace jobs {

template<class tupleTarget, class tupleSrc, size_t... Is>
constexpr tupleTarget make_arguments_tuple_impl(tupleSrc& t, std::index_sequence<Is...>)
{
    return std::forward_as_tuple(std::get<Is>(std::forward<tupleSrc>(t))...);
}
template<class tupleTarget, class tupleSrc>
constexpr tupleTarget make_arguments_tuple(tupleSrc& t)
{
    return make_arguments_tuple_impl<tupleTarget>(t, std::make_index_sequence<std::tuple_size<tupleSrc>::value >{});
}


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

      virtual ~job();


      void cancel ();

      bool is_canceled () const;

      virtual void process () = 0;

      void process_wrapper ();


      const rx_criticalness get_criticalness () const
      {
        return criticalness_;
      }

      void set_criticalness (rx_criticalness value)
      {
        criticalness_ = value;
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



      std::atomic<bool> canceled_;

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

      ~timer_job();


      virtual rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, rx_timer_ticks_t random_offset, bool& remove) = 0;

      void set_executer (threads::job_thread* executer);

      void lock ();

      void unlock ();

      static constexpr uint64_t max_sleep_period = 200;
  protected:

      rx_timer_ticks_t get_random_time_offset ();

      void wake_timer ();


      threads::job_thread *executer_;


      rx_timer_ticks_t next_;

      rx_timer_ticks_t period_;

      std::atomic<bool> suspended_;

      rx_timer_ticks_t period_error_;


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


      rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, rx_timer_ticks_t random_offset, bool& remove);

      void start (uint32_t period);


  protected:

  private:


};






class periodic_job : public timer_job  
{
	DECLARE_REFERENCE_PTR(periodic_job);

  public:
      periodic_job();


      rx_timer_ticks_t tick (rx_timer_ticks_t current_tick, rx_timer_ticks_t random_offset, bool& remove);

      void start (uint32_t period, bool now = false);

      void suspend ();


  protected:

  private:


};


template<typename refT, typename funcT, typename... Args>
struct rx_create_period_job;




template <typename refT, typename... Args>
class function_period_job : public post_period_job  
{
	DECLARE_REFERENCE_PTR(function_period_job);

    template <typename otherRefT, typename funcT, typename... OtherArgs>
    friend struct rx_create_period_job;
public:
    template <typename... FwdArgs>
    function_period_job(FwdArgs&&... args)
        : function_data_(std::forward<FwdArgs>(args)...)
    {
    }

  public:

      void process ()
      {
		  cancel();
          function_data_.call();
      }


  protected:

  private:


      function_to_go<refT,Args...> function_data_;


};

template<typename refT, typename funcT, typename... Args>
struct rx_create_period_job
{
    post_period_job::smart_ptr operator()(refT&& ref, funcT&& f, Args&&... args)
    {
        using wrapped_type = function_to_go<refT, Args...>;
        auto wrapped = wrapped_type();
        wrapped.set_arguments(std::forward<Args>(args)...);
        wrapped.function = std::forward<funcT>(f);
        auto job = rx_create_reference<function_period_job<refT, Args...> >(std::forward<wrapped_type>(wrapped));
        return job;
    }
};

template<typename refT, typename funcT, typename... Args>
struct rx_create_timer_job;




template <typename refT, typename... Args>
class function_timer_job : public periodic_job  
{
	DECLARE_REFERENCE_PTR(function_timer_job);

    template <typename otherRefT, typename funcT, typename... OtherArgs>
    friend struct rx_create_timer_job;
public:
    template <typename... FwdArgs>
    function_timer_job(FwdArgs&&... args)
        : function_data_(std::forward<FwdArgs>(args)...)
    {
    }

  public:

      void process ()
      {
          function_data_.call();
      }


  protected:

  private:


      function_to_go<refT,Args...> function_data_;


};

template<typename refT, typename funcT, typename... Args>
struct rx_create_timer_job
{
    periodic_job::smart_ptr operator()(refT&& ref, funcT&& f, Args&&... args)
    {
        using wrapped_type = function_to_go<refT, Args...>;
        auto wrapped = wrapped_type();
        wrapped.function = std::forward<funcT>(f);
        wrapped.set_arguments(std::forward<Args>(args)...);
        auto job = rx_create_reference<function_timer_job<refT, Args...> >(std::move(wrapped));
        return job;
    }
};
template<typename refT, typename funcT>
struct rx_create_timer_job<refT, funcT>
{
    periodic_job::smart_ptr operator()(refT&& ref, funcT&& f)
    {
        using wrapped_type = function_to_go<refT>;
        auto wrapped = wrapped_type();
        wrapped.function = std::forward<funcT>(f);
        auto job = rx_create_reference<function_timer_job<refT> >(std::move(wrapped));
        return job;
    }
};

template<typename refT, typename funcT, typename... Args>
struct rx_create_job;




template <typename refT, typename... Args>
class function_job : public job  
{
    DECLARE_REFERENCE_PTR(function_job);

    template <typename otherRefT, typename funcT, typename... OtherArgs>
    friend struct rx_create_job;
  public:
      template <typename... FwdArgs>
      function_job(FwdArgs&&... args)
          : function_data_(std::forward<FwdArgs>(args)...)
      {
      }

  public:

      void process ()
      {
          function_data_.call();
      }


  protected:

  private:


      function_to_go<refT,Args...> function_data_;


};


template<typename refT, typename funcT, typename... Args>
job_ptr rx_create_func_job(refT&& ref, funcT&& f, Args&&... args)
{
    return rx_create_job< refT, funcT, Args...>()(std::move(ref), std::move(f), std::forward<Args>(args)...);
}
template<typename refT, typename funcT, typename... Args>
struct rx_create_job
{
    job_ptr operator()(refT&& ref, funcT&& f, Args&&... args)
    {
        using wrapped_type = function_to_go<refT, Args...>;
        auto wrapped = wrapped_type();
        wrapped.function = std::forward<funcT>(f);
        wrapped.set_arguments(std::forward<Args>(args)...);
        auto job = rx_create_reference<function_job<refT, Args...> >(std::move(wrapped));
        return job;
    }
};

template<typename refT, typename funcT, typename... Args>
struct rx_create_empty_job
{
    job_ptr operator()(refT&& ref, funcT&& f)
    {
        using wrapped_type = function_to_go<refT, Args...>;
        auto wrapped = wrapped_type();
        wrapped.function = std::forward<funcT>(f);
        auto job = rx_create_reference<function_job<refT, Args...> >(std::move(wrapped));
        return job;
    }
};



template<class T, typename ...Args>
class member_functor
{
    typedef typename T::smart_ptr ptrT;
    typedef std::function<void(Args...)> func_t;
    typedef void(T::* mem_fn_t)(Args...);

    ptrT t_;
    func_t func_;
public:
    member_functor(ptrT t, mem_fn_t fn)
        : t_(t)
        , func_(std::bind(fn, t.unsafe_ptr(), std::placeholders::_1))
    {
    }
    void operator()(Args... params) const
    {
        using job_type = jobs::function_job<ptrT, Args...>;
        auto job = rx_create_reference<job_type>(t_, func_, std::forward<Args...>(params...));
        auto queue = t_->get_jobs_queue();
        if (queue)
            queue->append(job);
    }
};


// Parameterized Class rx::jobs::function_period_job 


// Parameterized Class rx::jobs::function_timer_job 


// Parameterized Class rx::jobs::function_job 


} // namespace jobs
} // namespace rx

namespace rx
{
typedef jobs::periodic_job::smart_ptr rx_timer_ptr;
typedef jobs::post_period_job::smart_ptr rx_monostabile_ptr;
}


#endif
