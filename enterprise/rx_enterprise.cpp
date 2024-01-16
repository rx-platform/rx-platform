

/****************************************************************************
*
*  enterprise\rx_enterprise.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_enterprise
#include "enterprise/rx_enterprise.h"

#include "rx_json_ent.h"


namespace rx_internal {

namespace enterprise {

// Class rx_internal::enterprise::enterprise_interface 

enterprise_interface::~enterprise_interface()
{
}



// Class rx_internal::enterprise::enterprise_callback 

enterprise_callback::~enterprise_callback()
{
}



// Class rx_internal::enterprise::enterprise_manager 

enterprise_manager* enterprise_manager::g_object_ = nullptr;

enterprise_manager::enterprise_manager()
{
}


enterprise_manager::~enterprise_manager()
{
}



enterprise_manager& enterprise_manager::instance ()
{
	if (!g_object_)
	{
		g_object_ = new enterprise_manager();
	}
	return *g_object_;
}

rx_result enterprise_manager::init_interfaces (const configuration_data_t& config)
{
	RX_ASSERT(g_object_);

	ENT_LOG_INFO("enterprise_manager", 900, "Initializing Enterprise Interfaces");

	enterprise_interface* itf = new json_enterprise_interface;
	interfaces_.emplace(itf->get_name(), itf);
	ENT_LOG_INFO("enterprise_manager", 500, "Added JSON enterprise interface.");


	ENT_LOG_INFO("enterprise_manager", 900, "Enterprise Interfaces initialized.");
	return true;
}

rx_result enterprise_manager::deinit_interfaces ()
{
	RX_ASSERT(g_object_);
	if (g_object_)
	{
		delete g_object_;
		g_object_ = nullptr;
	}
	return true;
}

enterprise_interface* enterprise_manager::get_interface (const string_type& name)
{
	RX_ASSERT(g_object_);
	auto it = interfaces_.find(name);
	if (it != interfaces_.end())
		return it->second;
	else
		return nullptr;
}

string_array enterprise_manager::list_interfaces ()
{
	string_array ret;
	if (!interfaces_.empty())
	{
		ret.reserve(interfaces_.size());
		for (const auto& one : interfaces_)
			ret.push_back(one.first);
	}
	return ret;
}


} // namespace enterprise
} // namespace rx_internal

