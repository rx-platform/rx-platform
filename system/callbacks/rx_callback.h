

/****************************************************************************
*
*  system\callbacks\rx_callback.h
*
*  Copyright (c) 2018 Dusan Ciric
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


#ifndef rx_callback_h
#define rx_callback_h 1




using namespace std::placeholders;


namespace rx_platform {

namespace callback {
typedef uint32_t callback_handle_t;
typedef uint32_t callback_state_t;





template <typename lockT, typename argT>
class callback_functor_container 
{
public:
	typedef std::function<void(const argT&, callback_state_t)> callback_function_t;
private:
	typedef std::map<callback_handle_t, callback_function_t > callbacks_type;


  public:
      callback_functor_container();

      ~callback_functor_container();


      uint32_t unregister_callback (callback_handle_t handle);

      void operator () (const argT& argument, callback_state_t state);

	  callback_functor_container(const callback_functor_container&) = delete;
	  callback_functor_container(callback_functor_container&&) = delete;
	  bool operator=(const callback_functor_container&) = delete;
	  bool operator=(callback_functor_container&&) = delete;

	  callback_handle_t register_callback(callback_function_t func);
	  template<typename callbackT>
	  callback::callback_handle_t register_callback(void* p, void(callbackT::*func)(const argT&, callback::callback_state_t));
	  callback::callback_handle_t register_callback(void(func)(const argT&, callback::callback_state_t));
  protected:

  private:


      static std::atomic<callback_handle_t> g_new_handle;

      lockT my_lock_;

      callbacks_type callbacks_;


};


// Parameterized Class rx_platform::callback::callback_functor_container 

template <typename lockT, typename argT>
std::atomic<callback_handle_t> callback_functor_container<lockT,argT>::g_new_handle;

template <typename lockT, typename argT>
callback_functor_container<lockT,argT>::callback_functor_container()
{
}


template <typename lockT, typename argT>
callback_functor_container<lockT,argT>::~callback_functor_container()
{
}



template <typename lockT, typename argT>
uint32_t callback_functor_container<lockT,argT>::unregister_callback (callback_handle_t handle)
{
	uint32_t ret = RX_ERROR;
	this->my_lock_.lock();
	auto it = callbacks_.find(handle);
	if (it != callbacks_.end())
	{
		callbacks_.erase(it);
		ret = RX_OK;
	}
	this->my_lock_.unlock();

	return ret;
}

template <typename lockT, typename argT>
void callback_functor_container<lockT,argT>::operator () (const argT& argument, callback_state_t state)
{
	this->my_lock_.lock();
	if (!callbacks_.empty())
	{
		for (auto it : callbacks_)
		{
			it.second(argument, state);
		}
	}
	this->my_lock_.unlock();
}

template <typename lockT, typename argT>
callback_handle_t callback_functor_container<lockT, argT>::register_callback(callback_function_t func)
{
	callback_handle_t ret = g_new_handle.fetch_add(1,std::memory_order_release);

	this->my_lock_.lock();
	this->callbacks_.emplace(ret, func);
	this->my_lock_.unlock();
	return ret;
}

template <typename lockT, typename argT>
template<typename callbackT>
callback::callback_handle_t callback_functor_container<lockT, argT>::register_callback(void* p, void(callbackT::*func)(const argT&, callback::callback_state_t))
{
	return register_callback(std::bind(func, (callbackT*)p, _1, _2));
}

template <typename lockT, typename argT>
callback::callback_handle_t callback_functor_container<lockT, argT>::register_callback(void(func)(const argT&, callback::callback_state_t))
{
	return register_callback(callback_function_t(func));
}
} // namespace callback
} // namespace rx_platform



#endif
