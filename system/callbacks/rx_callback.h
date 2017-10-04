

/****************************************************************************
*
*  system\callbacks\rx_callback.h
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


#ifndef rx_callback_h
#define rx_callback_h 1



// rx_ptr
#include "lib/rx_ptr.h"

using namespace std::placeholders;


namespace rx {

namespace callback {
typedef dword callback_handle_t;
typedef dword callback_state_t;





template <typename lockT, typename argT>
class callback_functor_cotainer 
{
	class dummy_class
	{
	public:
		void dummy_method(const argT&, callback_state_t state)
		{
			RX_ASSERT(false);
		}
	};

	void dummy_function(const argT&, callback_state_t state)
	{

	}

	typedef void(dummy_class::*storage_function_type)(const argT&,callback_state_t state);
	typedef void(*globale_function_type)(const argT&, callback_state_t state);

	typedef decltype(std::mem_fn(&dummy_class::dummy_method)) member_function_type;

	union data_union
	{
		dummy_class* object;
		globale_function_type gf;
	};

	struct callback_data_t
	{
		callback_data_t(void* obj, member_function_type mf)
			: f((member_function_type)mf)
		{
			callback_data.object=(dummy_class*)obj;
		}
		callback_data_t(globale_function_type func)
			: f(std::mem_fn(&dummy_class::dummy_method))
		{
			callback_data.gf = func;
		}
		member_function_type f;
		data_union callback_data;
	};

	typedef std::map<callback_handle_t, callback_data_t > callbacks_type;

  public:
      callback_functor_cotainer();

      virtual ~callback_functor_cotainer();


      dword unregister_callback (callback_handle_t handle);

      void operator () (const argT& argument, dword state);

		template<typename callbackT>
		callback_handle_t register_callback(void* p, void(callbackT::*func)(const argT&, callback_state_t))
		{
			callback_handle_t ret = g_new_handle++;
			ret <<= 1;
			callback_data_t one(p, std::mem_fn((storage_function_type)func));
			this->m_my_lock.lock();
			if (this->m_callbacks == nullptr)
				this->m_callbacks = new callbacks_type;
			this->m_callbacks->emplace(ret, one);
			this->m_my_lock.unlock();
			return ret;

		}
		callback_handle_t register_callback(void(func)(const argT&, callback_state_t))
		{
			callback_handle_t ret = g_new_handle++;
			ret <<= 1;
			ret |= 1;
			callback_data_t one(func);
			this->m_my_lock.lock();
			if (this->m_callbacks == nullptr)
				this->m_callbacks = new callbacks_type;
			this->m_callbacks->emplace(ret, one);
			this->m_my_lock.unlock();
			return ret;
		}
		template<typename callbackT>
		callback_handle_t register_callback(void* p, void(callbackT::*func)(const argT&, callback_state_t),const argT& initial)
		{
			callback_handle_t ret = g_new_handle++;
			ret <<= 1;
			callback_data_t one(p, std::mem_fn((storage_function_type)func));
			this->m_my_lock.lock();
			if (this->m_callbacks == nullptr)
				this->m_callbacks = new callbacks_type;
			(func)((callbackT*)p, initial, 0);
			this->m_callbacks->emplace(ret, one);
			this->m_my_lock.unlock();
			return ret;

		}
		callback_handle_t register_callback(globale_function_type func, const argT& initial)
		{
			callback_handle_t ret = g_new_handle++;
			ret <<= 1;
			ret |= 1;
			callback_data_t one(func);
			this->m_my_lock.lock();
			if (this->m_callbacks == nullptr)
				this->m_callbacks = new callbacks_type;
			(func)(initial, 0);
			this->m_callbacks->emplace(ret, one);
			this->m_my_lock.unlock();
			return ret;
		}
  protected:

  private:


      callbacks_type* m_callbacks;


      static std::atomic<callback_handle_t> g_new_handle;

      lockT m_my_lock;


};


// Parameterized Class rx::callback::callback_functor_cotainer 

template <typename lockT, typename argT>
std::atomic<callback_handle_t> callback_functor_cotainer<lockT,argT>::g_new_handle;

template <typename lockT, typename argT>
callback_functor_cotainer<lockT,argT>::callback_functor_cotainer()
      : m_callbacks(nullptr)
{
}


template <typename lockT, typename argT>
callback_functor_cotainer<lockT,argT>::~callback_functor_cotainer()
{
	if (this->m_callbacks != nullptr)
		delete this->m_callbacks;
}



template <typename lockT, typename argT>
dword callback_functor_cotainer<lockT,argT>::unregister_callback (callback_handle_t handle)
{
	dword ret = RX_ERROR;
	this->m_my_lock.lock();
	if (this->m_callbacks != nullptr)
	{
		auto it = m_callbacks->find(handle);
		if (it != m_callbacks->end())
		{
			m_callbacks->erase(it);
			ret = RX_OK;
		}
		this->m_my_lock.unlock();
	}
	return ret;
}

template <typename lockT, typename argT>
void callback_functor_cotainer<lockT,argT>::operator () (const argT& argument, dword state)
{
	this->m_my_lock.lock();
	if (this->m_callbacks != nullptr && !m_callbacks->empty())
	{
		for (auto& it : *m_callbacks)
		{
			if (it.first & 0x1)
			{// global function
				(it.second.callback_data.gf)(argument, state);
			}
			else
			{// member function
				if (it.second.callback_data.object)
					(it.second.f)(it.second.callback_data.object, argument, state);
			}
		}
	}
	this->m_my_lock.unlock();
}


} // namespace callback
} // namespace rx



#endif
