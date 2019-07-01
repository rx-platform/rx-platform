

/****************************************************************************
*
*  system\server\rx_server.h
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


#ifndef rx_server_h
#define rx_server_h 1




#include "version/rx_version.h"

#include "os_itf/rx_ositf.h"
#include "system/rx_platform_version.h"
#include "lib/rx_lib.h"
#include "rx_configuration.h"
#include "system/libraries/rx_plugin.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_mngt
#include "system/server/rx_mngt.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_inf
#include "system/server/rx_inf.h"

namespace interfaces {
namespace io_endpoints {
class rx_io_manager;

} // namespace io_endpoints
} // namespace interfaces



#include "lib/rx_log.h"
using namespace rx;


namespace rx_platform {


struct io_manager_data_t
{
};

struct general_data_t
{
	bool test_log = false;
	string_type startup_script;
};

struct configuration_data_t
{
	infrastructure::runtime_data_t runtime_data;
	mngt::managment_data_t managment_data;
	ns::namespace_data_t namespace_data;
	meta::meta_configuration_data_t meta_configuration_data;
	io_manager_data_t io_manager_data;
	general_data_t general;
};

enum rx_platform_status
{
	rx_platform_initializing,
	rx_platform_starting,
	rx_platform_running,
	rx_platform_stopping,
	rx_platform_deinitializing
};






class rx_gate
{
	typedef std::map<string_type,prog::server_script_host*> scripts_type;

  public:

      static rx_gate& instance ();

      void cleanup ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const configuration_data_t& data);

      rx_result stop ();

      rx_directory_ptr get_root_directory ();

      bool shutdown (const string_type& msg);

      bool read_log (const log::log_query_type& query, log::log_events_type& result);

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);


      infrastructure::server_rt& get_infrastructure ()
      {
        return infrastructure_;
      }


      mngt::server_manager& get_manager ()
      {
        return manager_;
      }


      hosting::rx_platform_host * get_host ()
      {
        return host_;
      }



      rx_time get_started () const
      {
        return started_;
      }


      const string_type& get_os_info () const
      {
        return os_info_;
      }


      const string_type& get_rx_version () const
      {
        return rx_version_;
      }


      const string_type& get_lib_version () const
      {
        return lib_version_;
      }


      const string_type& get_rx_name () const
      {
        return rx_name_;
      }


      const string_type& get_comp_version () const
      {
        return comp_version_;
      }


      const string_type& get_hal_version () const
      {
        return hal_version_;
      }


      rx_pid_t get_pid () const
      {
        return pid_;
      }


      const bool is_shutting_down () const
      {
        return shutting_down_;
      }


      rx_platform_status get_platform_status () const
      {
        return platform_status_;
      }



  protected:

      void interface_bind ();

      void interface_release ();


  private:
      rx_gate();

      ~rx_gate();



      infrastructure::server_rt infrastructure_;

      rx_directory_ptr root_;

      mngt::server_manager manager_;

      hosting::rx_platform_host *host_;

      scripts_type scripts_;

      std::unique_ptr<interfaces::io_endpoints::rx_io_manager> io_manager_;


      static rx_gate* g_instance;

      rx_time started_;

      string_type os_info_;

      string_type rx_version_;

      string_type lib_version_;

      string_type rx_name_;

      string_type comp_version_;

      string_type hal_version_;

      rx_pid_t pid_;

      security::security_guard_ptr security_guard_;

      bool shutting_down_;

      rx_platform_status platform_status_;


};

template<typename argT>
void rx_post_function(std::function<void(argT)> f, argT arg, rx_thread_handle_t whome)
{
    typedef jobs::lambda_job<argT, argT> lambda_t;
	rx_gate::instance().get_infrastructure().get_executer(whome)->append(typename lambda_t::smart_ptr(f,arg));
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
					tracnsaction_recursive_function(state, ref, args...);
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
	using state_type = rx_transaction_state<resultT, Args...>;
	if (items.empty())
		return;// nothing to do

	state->items = std::move(items);
	state->current = 0;
	state->count = state->items.size();
	state->ret_thread = rx_thread_context();
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
rx_domain_ptr rx_system_domain();
rx_application_ptr rx_system_application();

rx_domain_ptr rx_unassigned_domain();
rx_application_ptr rx_unassigned_application();

} // namespace rx_platform



#endif
