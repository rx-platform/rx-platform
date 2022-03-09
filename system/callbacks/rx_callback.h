

/****************************************************************************
*
*  system\callbacks\rx_callback.h
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


#ifndef rx_callback_h
#define rx_callback_h 1



// rx_job
#include "system/threads/rx_job.h"

//#include "system/threads/rx_thread.h"
using namespace std::placeholders;

rx_thread_handle_t rx_thread_context();


namespace rx_platform {

namespace callback {
typedef uint32_t callback_handle_t;
typedef uint32_t callback_state_t;

void send_callback_back(rx_thread_handle_t target, job_ptr job);




template <typename... Args>
class rx_any_callback 
{
    using base_job_ptr_t = rx_reference<jobs::args_job<Args...> >;

  public:

      rx_reference_ptr get_anchor ()
      {
          return job_ptr_->get_anchor();
      }

      template<typename funcT>
      rx_any_callback(rx_reference_ptr anchor, funcT func, rx_thread_handle_t target = 0)
      {
          target_ = target != 0 ? target : rx_thread_context();
          this->job_ptr_ =  rx_create_reference<function_job<funcT, Args...> >(anchor, std::move(func));
      }
      template<typename... FwdArgs>
      void operator () (FwdArgs&&... args)
      {
          this->job_ptr_->set_arguments(std::forward<FwdArgs>(args)...);
          send_callback_back(target_, this->job_ptr_);
      }
      rx_any_callback() = default;
      ~rx_any_callback() = default;
      rx_any_callback(rx_any_callback&&) noexcept = default;
      rx_any_callback& operator=(rx_any_callback&&) noexcept = default;
      rx_any_callback(const rx_any_callback&) = delete;
      rx_any_callback& operator=(const rx_any_callback&) = delete;
  protected:

  private:


      base_job_ptr_t job_ptr_;


      rx_thread_handle_t target_;


};






template <typename resultT, typename... Args>
class rx_remote_function 
{
    using base_job_ptr_t = rx_reference<jobs::remote_args_job<resultT, Args...> >;

  public:

      rx_reference_ptr get_anchor ()
      {
          return job_ptr_->get_anchor();
      }

      void set_target (rx_thread_handle_t target)
      {
          target_ = target;
      }

      void send_result (resultT&& result)
      {
          job_ptr_->send_result_callback(std::forward<resultT>(result));
      }

      template<typename funcT, typename resultFuncT>
      rx_remote_function(rx_reference_ptr anchor, rx_thread_handle_t target, funcT func, resultFuncT result)
      {
          target_ = target;
          rx_any_callback<resultT> callback(anchor, std::forward<resultFuncT>(result));
          this->job_ptr_ = rx_create_reference<remote_function_job<funcT, rx_any_callback<resultT>, resultT,  Args...> >(
              std::move(callback), anchor, std::move(func));
      }
      template<typename funcT>
      rx_remote_function(rx_thread_handle_t target, funcT func, rx_any_callback<resultT> callback)
      {
          target_ = target;
          rx_reference_ptr anchor = callback.get_anchor();
          this->job_ptr_ = rx_create_reference<remote_function_job<funcT, rx_any_callback<resultT>, resultT, Args...> >(
              std::move(callback), anchor, std::move(func));
      }
      template<typename... FwdArgs>
      void operator () (FwdArgs&&... args)
      {
          this->job_ptr_->set_arguments(std::forward<FwdArgs>(args)...);
          send_callback_back(target_, this->job_ptr_);
      }
      rx_remote_function() = default;
      ~rx_remote_function() = default;
      rx_remote_function(rx_remote_function&&) noexcept = default;
      rx_remote_function& operator=(rx_remote_function&&) noexcept = default;
      rx_remote_function(const rx_remote_function&) = delete;
      rx_remote_function& operator=(const rx_remote_function&) = delete;
  protected:

  private:


      base_job_ptr_t job_ptr_;


      rx_thread_handle_t target_;

      rx_thread_handle_t result_target_;


};


} // namespace callback
} // namespace rx_platform

namespace rx_platform
{
typedef rx_platform::callback::rx_any_callback<rx_result&&> rx_result_callback;

template <typename resultT>
using rx_result_with_callback = rx_platform::callback::rx_any_callback<rx_result_with<resultT>&&>;
}


#endif
