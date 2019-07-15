

/****************************************************************************
*
*  system\server\rx_async_functions.h
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


#ifndef rx_async_functions_h
#define rx_async_functions_h 1


#include "rx_server.h"


namespace rx_platform
{

template<class refT, class Arg>
void rx_post_result_to(rx_thread_handle_t target, std::function<void(rx_result_with<Arg>&&)> what, refT ref, rx_result_with<Arg>&& arg)
{
	auto et = rx_gate::instance().get_infrastructure().get_executer(target);

	et->append(rx_create_reference<jobs::result_lambda_job<Arg, refT> >(what, std::move(arg), ref));
}

template<class refT, class... Args>
void rx_post_function_to(rx_thread_handle_t target, std::function<void(Args...)> what, refT ref, Args... args)
{
	auto et = rx_gate::instance().get_infrastructure().get_executer(target);

	et->append(rx_create_reference<jobs::lambda_job<refT> >(
		[=](refT&&) mutable
		{
			what(std::forward<Args>(args)...);
		}, ref));
}
template<class refT, class... Args>
void rx_post_function(std::function<void(Args...)> what, refT ref, Args... args)
{
	auto et = rx_gate::instance().get_infrastructure().get_executer(rx_thread_context());

	et->append(rx_create_reference<jobs::lambda_job<refT> >(
		[=](refT) mutable
		{
			what(args...);
		}, ref));
}
template<class refT, class... Args>
void rx_post_delayed_function(uint32_t period, std::function<void(Args...)> what, refT ref, Args... args)
{
	rx_gate::instance().get_infrastructure().append_timer_job(rx_create_reference<jobs::lambda_period_job<refT> >(
		[=](refT) mutable
		{
			what(args...);
		}, ref), period);
}

template<class refT, class... Args>
void rx_create_periodic_function(uint32_t period, std::function<bool(Args...)> what, refT ref, Args... args)
{
	rx_gate::instance().get_infrastructure().append_timer_job(rx_create_reference<jobs::lambda_timer_job<refT> >(
		[=](refT) mutable
		{
			return what(args...);
		}, ref), period);
}
template<typename argT>
void rx_post_function(std::function<void(argT)> f, argT arg, rx_thread_handle_t whome)
{
	typedef jobs::lambda_job<argT, argT> lambda_t;
	rx_gate::instance().get_infrastructure().get_executer(whome)->append(typename lambda_t::smart_ptr(f, arg));
}
template<typename argT>
void rx_post_delayed_function(std::function<void(argT)> f, uint32_t interval, argT arg, rx_thread_handle_t whome)
{
	typedef jobs::lambda_period_job<argT> lambda_t;
	rx_gate::instance().get_infrastructure().append_timer_job(typename lambda_t::smart_ptr(f, arg), interval);
}
template<class resultT, class... Args>
struct rx_transaction_slot
{
	std::function<resultT(Args...)> what;
	rx_thread_handle_t whose;
};

template<class resultT, class... Args>
struct rx_transaction_state
{
	std::vector<rx_transaction_slot<resultT, Args...> > items;
	size_t current;
	size_t count;
	std::function<void(resultT)> callback;
	rx_thread_handle_t ret_thread;
	resultT result;
};


template<class resultT, class refT, class... Args>
void tracnsaction_recursive_function(rx_transaction_state<resultT, Args...>* state, refT ref, Args... args)
{
	using state_type = rx_transaction_state<resultT, Args...>;

	auto my_thread = state->items[state->current].whose;
	while (my_thread == state->items[state->current].whose && state->current < state->count)
	{
		state->result = state->items[state->current].what(args...);
		if (!state->result)
		{
			// we had error report it
			if (state->ret_thread == my_thread)
			{
				state->callback(std::move(state->result));
			}
			else
			{
				auto jt = rx_gate::instance().get_infrastructure().get_executer(state->ret_thread);
				jt->append(
					rx_create_reference<jobs::lambda_job<resultT, refT> >(
						[=](resultT&& ret_val) mutable
						{
							state->callback(std::move(ret_val));
						},
						std::move(state->result), ref));
			}
			// just return we sent callback
			return;
		}
		state->current++;
	}
	if (state->current < state->count)
	{// we're not done here
		auto next_thread = state->items[state->current].whose;
		auto et = rx_gate::instance().get_infrastructure().get_executer(next_thread);
		et->append(
			rx_create_reference<jobs::lambda_job<state_type*, refT> >(
				[=](state_type* state) mutable
				{
					tracnsaction_recursive_function(state, ref, std::move(args...));
				}, state, ref));
	}
	else
	{// we're done here, send callback
		resultT result;
		if (state->ret_thread == my_thread)
		{
			state->callback(std::move(state->result));
		}
		else
		{
			auto jt = rx_gate::instance().get_infrastructure().get_executer(state->ret_thread);
			jt->append(
				rx_create_reference<jobs::lambda_job<resultT, refT> >(
					[=](resultT&& ret_val) mutable
					{
						state->callback(std::move(ret_val));
					},
					std::move(state->result), ref));
		}
	}
}

template<class resultT, class refT, class... Args>
void rx_do_transaction_with_callback(std::vector<rx_transaction_slot<resultT, Args...> >&& items, std::function<void(resultT)> callback, refT ref, rx_transaction_state<resultT, Args...>* state, Args... args)
{
	auto ret_target = rx_thread_context();
	rx_do_transaction_with_callback(std::move(items), callback, ref, state, ret_target, args...);
}
template<class resultT, class refT, class... Args>
void rx_do_transaction_with_callback(std::vector<rx_transaction_slot<resultT, Args...> >&& items, std::function<void(resultT)> callback, refT ref, rx_transaction_state<resultT, Args...>* state, rx_thread_handle_t ret_thread, Args... args)
{
	using state_type = rx_transaction_state<resultT, Args...>;
	if (items.empty())
		return;// nothing to do

	state->items = std::move(items);
	state->current = 0;
	state->count = state->items.size();
	state->ret_thread = ret_thread;
	state->callback = callback;

	auto next_thread = state->items[state->current].whose;
	auto et = rx_gate::instance().get_infrastructure().get_executer(next_thread);
	et->append(
		rx_create_reference<jobs::lambda_job<state_type*, refT> >(
			[=](state_type* state) mutable
			{
				tracnsaction_recursive_function(state, ref, args...);
			}, state, ref));
}

template<class resultT, class refT, class... Args>
void rx_do_with_callback(std::function<resultT(Args...)> what, rx_thread_handle_t where, std::function<void(resultT)> callback, refT ref, Args... args)
{
	auto et = rx_gate::instance().get_infrastructure().get_executer(where);
	auto ret_thread = rx_thread_context();
	if (where == ret_thread)
	{
		et->append(
			rx_create_reference<jobs::lambda_job<refT> >(
				[=](refT) mutable
				{
					resultT ret = what(args...);
					callback(std::move(ret));
				}, ref));
	}
	else
	{
		et->append(
			rx_create_reference<jobs::lambda_job<decltype(ret_thread), refT> >(
				[=](decltype(ret_thread) ret_thread) mutable
				{
					resultT ret = what(args...);
					auto jt = rx_gate::instance().get_infrastructure().get_executer(ret_thread);
					jt->append(
						rx_create_reference<jobs::lambda_job<resultT, refT> >(
							[=](resultT&& ret_val) mutable
							{
								callback(std::move(ret_val));
							},
							std::move(ret), ref));
				}, ret_thread, ref));
	}
}

}// rx_platform




#endif
