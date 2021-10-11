

/****************************************************************************
*
*  system\callbacks\rx_callback.cpp
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


#include "pch.h"


// rx_callback
#include "system/callbacks/rx_callback.h"

#include "sys_internal/rx_inf.h"


namespace rx_platform {

namespace callback {

// Parameterized Class rx_platform::callback::rx_any_callback 

void send_callback_back(rx_thread_handle_t target, jobs::job_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().get_executer(target)->append(job);
}
// Parameterized Class rx_platform::callback::rx_remote_function 


} // namespace callback
} // namespace rx_platform

using namespace rx_platform::callback;

template<class callbackT, class funcT>
auto do_other(funcT&& what, callbackT&& callback)
{
	rx_reference_ptr ref;

	auto ret = rx_remote_function<int, bool, string_type>(ref, RX_DOMAIN_META
		, std::forward<funcT>(what), std::forward<callbackT>(callback));

	return ret;
}

void tesing()
{

	std::unique_ptr<rx_any_callback<int, string_type> > p1;

	rx_reference_ptr ref;

	auto rmt = do_other([](bool, string_type)
	{
		return 55;
	}, [](int a)
	{

	});

	rmt(true, "perica");

	p1 = std::make_unique<rx_any_callback<int, string_type> >(ref, [](int a, string_type b)
		{
		});

	(*p1)(6, "zika");

}


// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	target_ = target;

#endif
