

/****************************************************************************
*
*  sys_internal\rx_async_functions.h
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


#ifndef rx_async_functions_h
#define rx_async_functions_h 1


#include "system/server/rx_server.h"
#include "sys_internal/rx_inf.h"
#include "system/storage_base/rx_storage.h"
#include "lib/rx_func_to_go.h"

#include <utility>
using namespace rx_platform;
using namespace rx;


namespace rx_platform
{

template <typename T>
struct func_traits : public func_traits<decltype(&T::operator())> {};

template <typename C, typename Ret, typename... Args>
struct func_traits<Ret(C::*)(Args...) const> {
	using result_type = Ret;
	using function_t = typename std::function<result_type(Args...)>;
	template <std::size_t i>
	struct arg {
		using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
	};
};

template <typename T>
void option(T&& t) {
	using traits = func_traits<typename std::decay<T>::type>;

	using return_t = typename traits::result_type;         // Return type.
	using arg0_t = typename traits::template arg<0>::type; // First arg type.

	// Output types.
	std::cout << "Return type: " << typeid(return_t).name() << std::endl;
	std::cout << "Argument type: " << typeid(arg0_t).name() << std::endl;
}

template<typename callableT>
auto do_bind(callableT&& what)
{
    if constexpr (!std::is_object<callableT>::value)
    {
        return what;
    }
	else
	{
        using traits = func_traits<typename std::decay<callableT>::type>;
        using funct_t =typename traits::function_t;
        funct_t ret = what;
        return ret;
	}
}

template<class refT, typename... Args>
void rx_post_packed_to(rx_thread_handle_t target, function_to_go<refT, Args...>&& f)
{
	auto et = rx_internal::infrastructure::server_runtime::instance().get_executer(target);
	auto job = rx_create_reference<function_job<refT, Args...> >(std::move(f));
	//auto job = creator<Args...>(std::forward<funcT>(f));
	if (job)
		et->append(job);
	else
		RX_ASSERT(false);
}

template<class funcT, class... Args>
void rx_post_function_to(rx_thread_handle_t target, rx_reference_ptr ref, funcT what, Args&&... args)
{
	auto et = rx_internal::infrastructure::server_runtime::instance().get_executer(target);
	auto job = rx_create_job<funcT, Args...>()(ref, std::move(what), std::forward<Args>(args)...);
	if (job)
		et->append(job);
	else
		RX_ASSERT(false);
}

template<class funcT, class... Args>
void rx_post_function(rx_reference_ptr ref, funcT what, Args&&... args)
{
	rx_post_function_to(rx_thread_context(), ref, std::move(what), std::forward<Args>(args)...);
}
template<class funcT, class... Args>
post_period_job::smart_ptr  rx_post_delayed_function_to(rx_thread_handle_t target,rx_reference_ptr ref, uint32_t period, funcT what, Args... args)
{
	auto job = rx_create_period_job<funcT, Args...>()(ref, std::move(what), std::forward<Args>(args)...);
	if (job)
	{
		rx_internal::infrastructure::server_runtime::instance().append_timer_job(job);
		job->start(period);
	}
	else
		RX_ASSERT(false);
	return job;
}
template<class funcT, class... Args>
post_period_job::smart_ptr  rx_post_delayed_function(uint32_t period, rx_reference_ptr ref, funcT what, Args... args)
{
    return rx_post_delayed_function_to(rx_thread_context(), ref, period, std::move(what), std::forward<Args>(args)...);
}

template<class funcT, class... Args>
periodic_job::smart_ptr rx_create_periodic_function(uint32_t period, rx_reference_ptr ref, funcT what, Args... args)
{
	auto et = rx_internal::infrastructure::server_runtime::instance().get_executer(rx_thread_context());
	auto job = rx_create_timer_job<funcT, Args...>()(ref, std::move(what), std::forward<Args>(args)...);
	if (job)
		rx_internal::infrastructure::server_runtime::instance().append_timer_job(job, period);
	else
		RX_ASSERT(false);
	return job;
}

template<class funcT>
periodic_job::smart_ptr rx_create_periodic_function(uint32_t period, rx_reference_ptr ref, funcT what)
{
	auto job = rx_create_timer_job<funcT>()(ref, std::move(what));
	if (job)
	{
		rx_internal::infrastructure::server_runtime::instance().append_timer_job(job);
		job->start(period);
	}
	else
		RX_ASSERT(false);
	return job;
}

template<typename funcT>
struct rx_transaction_slot
{
	funcT what;
	rx_thread_handle_t whose;
};

//template<typename funcT, typename callbackT>
//struct rx_transaction_state
//{
//	std::vector<rx_transaction_slot<funcT> > items;
//	size_t current;
//	size_t count;
//	callbackT callback;
//	rx_thread_handle_t ret_thread;
//};
//
//
//template<class resultT, class refT, typename funcT, typename callbackT>
//void tracnsaction_recursive_function(rx_transaction_state<funcT, callbackT>* state, refT ref, Args... args)
//{
//	using state_type = rx_transaction_state<funcT, callbackT>;
//
//	auto my_thread = state->items[state->current].whose;
//	while (my_thread == state->items[state->current].whose && state->current < state->count)
//	{
//		state->result = state->items[state->current].what(args...);
//		if (!state->result)
//		{
//			// we had error report it
//			if (state->ret_thread == my_thread)
//			{
//				state->callback(std::move(state->result));
//			}
//			else
//			{
//				auto jt = rx_internal::infrastructure::server_runtime::instance().get_executer(state->ret_thread);
//				jt->append(
//					rx_create_reference<jobs::function_job<resultT, refT> >(
//						[=](resultT&& ret_val) mutable
//						{
//							state->callback(std::move(ret_val));
//						},
//						std::move(state->result), ref));
//			}
//			// just return we sent callback
//			return;
//		}
//		state->current++;
//	}
//	if (state->current < state->count)
//	{// we're not done here
//		auto next_thread = state->items[state->current].whose;
//		auto et = rx_internal::infrastructure::server_runtime::instance().get_executer(next_thread);
//		et->append(
//			rx_create_reference<jobs::function_job<state_type*, refT> >(
//				[=](state_type* state) mutable
//				{
//					tracnsaction_recursive_function(state, ref, std::move(args...));
//				}, state, ref));
//	}
//	else
//	{// we're done here, send callback
//		resultT result;
//		if (state->ret_thread == my_thread)
//		{
//			state->callback(std::move(state->result));
//		}
//		else
//		{
//			auto jt = rx_internal::infrastructure::server_runtime::instance().get_executer(state->ret_thread);
//			jt->append(
//				rx_create_reference<jobs::function_job<resultT, refT> >(
//					[=](resultT&& ret_val) mutable
//					{
//						state->callback(std::move(ret_val));
//					},
//					std::move(state->result), ref));
//		}
//	}
//}
//
//template<class resultT, class refT, class... Args>
//void rx_do_transaction_with_callback(std::vector<rx_transaction_slot<resultT, Args...> >&& items, std::function<void(resultT)> callback, refT ref, rx_transaction_state<resultT, Args...>* state, Args... args)
//{
//	auto ret_target = rx_thread_context();
//	rx_do_transaction_with_callback(std::move(items), callback, ref, state, ret_target, args...);
//}
//template<class resultT, class refT, class... Args>
//void rx_do_transaction_with_callback(std::vector<rx_transaction_slot<resultT, Args...> >&& items, std::function<void(resultT)> callback, refT ref, rx_transaction_state<resultT, Args...>* state, rx_thread_handle_t ret_thread, Args... args)
//{
//	using state_type = rx_transaction_state<resultT, Args...>;
//	if (items.empty())
//		return;// nothing to do
//
//	state->items = std::move(items);
//	state->current = 0;
//	state->count = state->items.size();
//	state->ret_thread = ret_thread;
//	state->callback = callback;
//
//	auto next_thread = state->items[state->current].whose;
//	auto et = rx_internal::infrastructure::server_runtime::instance().get_executer(next_thread);
//	et->append(
//		rx_create_reference<jobs::function_job<state_type*, refT> >(
//			[=](state_type* state) mutable
//			{
//				tracnsaction_recursive_function(state, ref, args...);
//			}, state, ref));
//}


template<class funcT, class callbackT, class... Args>
void rx_do_with_callback(rx_thread_handle_t where, funcT what, callbackT&& callback, Args&&... args)
{
	using result_type = typename std::invoke_result<funcT, Args...>::type;
	rx_reference_ptr anchor = callback.get_anchor();


	callback::rx_remote_function<result_type, Args...> rmt(anchor, where, what, std::move(callback));

	rmt(std::forward<Args>(args)...);
}
template<class funcT, class callbackT>
void rx_do_with_callback(rx_thread_handle_t where, funcT&& what, callbackT callback)
{
	using result_type = decltype(what());
	auto anchor = callback.get_anchor();

	rx_post_function_to(where, anchor, [](funcT&& what, callbackT&& callback)
		{
			result_type ret(what());
			callback(std::move(ret));

		}, std::forward< funcT>(what), std::move(callback));
}

}// rx_platform




#endif
