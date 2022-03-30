

/****************************************************************************
*
*  platform_api\rx_general.cpp
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


#include "pch.h"


// rx_general
#include "platform_api/rx_general.h"

#include "rx_configuration.h"

extern "C"
{
	void c_process_callback(void* whose)
	{
		rx_platform_api::callback_data* self = (rx_platform_api::callback_data*)whose;
		self->process();
	}
}


rxLockRuntimeManager_t api_lock_runtime_manager;
rxUnlockRuntimeManager_t api_unlock_runtime_manager;

rxWriteLog_t api_write_log_func;
rxRegisterItem_t api_reg_item_binary_func;

string_type api_plugin_root;

namespace rx_platform_api
{

void write_log(log_event_type type, const char* library, const char* source, uint16_t level, const char* code, const char* message)
{
	api_write_log_func(g_plugin, (int)type, library, source, level, code, message);
}
rx_result register_item_binary(rx_item_type type, const string_type& name, const string_type& path
	, const rx_node_id& id, const rx_node_id& parent
	, uint32_t version, rx_time modified, const uint8_t* data, size_t count, uint32_t stream_version)
{
	if (api_plugin_root.empty())
		return "Operation not possible at this moment!";

	if (data == nullptr || count < 10)
		return "No data provided";
	string_type real_path = api_plugin_root;
	if(!path.empty())
		real_path += RX_DIR_DELIMETER + path;
	
	rx_result ret = api_reg_item_binary_func(g_plugin, (uint8_t)type, name.c_str(), real_path.c_str(), id.c_ptr(), parent.c_ptr(), version, modified, stream_version, data, count);
	return ret;
}
rx_result register_item_binary(rx_item_type type, const string_type& name, const string_type& path, const rx_node_id& id
	, const rx_node_id& parent, const uint8_t* data, size_t count, uint32_t stream_version)
{
	return register_item_binary(type, name, path, id, parent, 0x10000, rx_time::now(), data, count, stream_version);
}


rx_runtime_manager_lock::rx_runtime_manager_lock()
{
	api_lock_runtime_manager();
}
rx_runtime_manager_lock::~rx_runtime_manager_lock()
{
	api_unlock_runtime_manager();
}

}



namespace rx_platform_api {

// Class rx_platform_api::callback_data 


void callback_data::init_api_data ()
{
	bind_as_shared(&api_data_.anchor);
	api_data_.process = c_process_callback;
}

void callback_data::process ()
{
	callback_();
}

plugin_job_struct_t* callback_data::c_ptr ()
{
	return &api_data_;
}


} // namespace rx_platform_api

