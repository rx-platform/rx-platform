

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_api.cpp
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


// rx_api
#include "rx_api.h"

#include "version/rx_version.h"
#include "rx_platform_version.h"
#include "lib/rx_ser_lib.h"
#include "lib/rx_io_addr.h"


extern rxLockRuntimeManager_t api_lock_runtime_manager;
extern rxUnlockRuntimeManager_t api_unlock_runtime_manager;

extern rxWriteLog_t api_write_log_func;
extern rxRegisterItem_t api_reg_item_binary_func;
extern rxRegisterRuntimeItem_t api_reg_runtime_binary_func;
extern string_type api_plugin_root;

extern rxRegisterSourceRuntime_t api_reg_source_func;
extern rxRegisterMapperRuntime_t api_reg_mapper_func;
extern rxRegisterMapperRuntime3_t api_reg_mapper3_func;
extern rxRegisterFilterRuntime_t api_reg_filter_func;

extern rxRegisterStructRuntime_t api_reg_struct_func;
extern rxRegisterVariableRuntime_t api_reg_variable_func;

extern rxRegisterMethodRuntime_t api_reg_method_func;
extern rxRegisterProgramRuntime_t api_reg_program_func;
extern rxRegisterDisplayRuntime_t api_reg_display_func;
extern rxRegisterEventRuntime_t api_reg_event_func;

extern rxRegisterApplicationRuntime_t api_reg_application_func;
extern rxRegisterDomainRuntime_t api_reg_domain_func;
extern rxRegisterObjectRuntime_t api_reg_object_func;

extern rxRegisterPortRuntime_t api_reg_port_func;

extern rxRegisterRelationRuntime_t api_reg_relation_func;
extern rxRegisterDataTypeRuntime_t api_reg_data_type_func;

extern rxInitCtxBindItem_t api_bind_item_func;
extern rxInitCtxGetCurrentPath_t api_init_get_current_path;
extern rxInitCtxGetLocalValue_t api_init_get_local_value;
extern rxInitCtxSetLocalValue_t api_init_set_local_value;
extern rxInitCtxGetMappingValues_t api_get_mapping_values;
extern rxInitCtxGetSourceValues_t api_get_source_values;
extern rxInitCtxGetItemMeta_t api_item_meta;
extern rxInitCtxConnectItem_t api_connect_item_func;
extern rxInitCtxGetDataType_t api_get_data_type_func;


extern rxStartCtxGetCurrentPath_t api_start_get_current_path;
extern rxStartCtxCreateTimer_t api_start_ctx_create_timer;
extern rxStartCtxGetLocalValue_t api_get_local_value;
extern rxStartCtxSetLocalValue_t api_set_local_value;
extern rxStartCtxSubscribeRelation_t api_subscribe_relation;

extern rxCtxGetValue_t api_get_value_func;
extern rxCtxSetValue_t api_set_value_func;
extern rxCtxSetAsyncPending_t api_set_async_pending_func;

extern rxCtxWriteConnected_t api_write_connected_func;
extern rxCtxExecuteConnected_t api_execute_connected_func;
extern rxCtxWriteBinded_t api_write_binded_func;

extern rxGetNewUniqueId_t api_get_new_unique_id_func;


extern rxRegisterStorageType_t api_reg_storage_func;


const platform_api6* g_api;




namespace rx_platform_api {

namespace
{

static uintptr_t g_plugin = 0;
}

intptr_t get_rx_plugin()
{
	return g_plugin;
}


intptr_t set_rx_plugin(intptr_t what)
{
	uintptr_t prev = g_plugin;
	g_plugin = what;
	RX_ASSERT(prev == 0);
	return prev;
}
void copy_rt_to_rt6(const platform_runtime_api* src, platform_runtime_api6* dest)
{
	memzero(dest, sizeof(platform_runtime_api5));

	dest->prxRegisterSourceRuntime = src->prxRegisterSourceRuntime;
	dest->prxRegisterMapperRuntime = src->prxRegisterMapperRuntime;
	dest->prxRegisterFilterRuntime = src->prxRegisterFilterRuntime;
	dest->prxRegisterStructRuntime = src->prxRegisterStructRuntime;
	dest->prxRegisterVariableRuntime = src->prxRegisterVariableRuntime;
	dest->prxRegisterEventRuntime = src->prxRegisterEventRuntime;

	dest->prxRegisterMethodRuntime = src->prxRegisterMethodRuntime;
	dest->prxRegisterProgramRuntime = src->prxRegisterProgramRuntime;
	dest->prxRegisterDisplayRuntime = src->prxRegisterDisplayRuntime;

	dest->prxRegisterObjectRuntime = src->prxRegisterObjectRuntime;
	dest->prxRegisterApplicationRuntime = src->prxRegisterApplicationRuntime;
	dest->prxRegisterDomainRuntime = src->prxRegisterDomainRuntime;
	dest->prxRegisterPortRuntime = src->prxRegisterPortRuntime;

	dest->prxRegisterRelationRuntime = src->prxRegisterRelationRuntime;

	dest->prxInitCtxBindItem = src->prxInitCtxBindItem;
	dest->prxInitCtxGetCurrentPath = src->prxInitCtxGetCurrentPath;
	dest->prxInitCtxGetLocalValue = src->prxInitCtxGetLocalValue;
	dest->prxInitCtxSetLocalValue = src->prxInitCtxSetLocalValue;
	dest->prxInitCtxGetMappingValues = src->prxInitCtxGetMappingValues;
	dest->prxInitCtxGetSourceValues = src->prxInitCtxGetSourceValues;
	dest->prxInitCtxGetItemMeta = src->prxInitCtxGetItemMeta;

	dest->prxStartCtxGetCurrentPath = src->prxStartCtxGetCurrentPath;
	dest->prxStartCtxCreateTimer = src->prxStartCtxCreateTimer;
	dest->prxStartCtxGetLocalValue = src->prxStartCtxGetLocalValue;
	dest->prxStartCtxSetLocalValue = src->prxStartCtxSetLocalValue;
	dest->prxStartCtxSubscribeRelation = src->prxStartCtxSubscribeRelation;

	dest->prxCtxGetValue = src->prxCtxGetValue;
	dest->prxCtxSetValue = src->prxCtxSetValue;
	dest->prxCtxSetAsyncPending = src->prxCtxSetRemotePending;
}


void copy_rt3_to_rt6(const platform_runtime_api3* src, platform_runtime_api6* dest)
{
	memzero(dest, sizeof(platform_runtime_api6));
	memcpy(dest, src, sizeof(platform_runtime_api3));//!!!!
}


void copy_rt4_to_rt6(const platform_runtime_api4* src, platform_runtime_api6* dest)
{
	memzero(dest, sizeof(platform_runtime_api6));
	memcpy(dest, src, sizeof(platform_runtime_api4));//!!!!
}

void copy_rt5_to_rt6(const platform_runtime_api5* src, platform_runtime_api6* dest)
{
	memzero(dest, sizeof(platform_runtime_api6));
	memcpy(dest, src, sizeof(platform_runtime_api5));//!!!!
}

namespace
{
string_type rx_version_;
string_type lib_version_;
string_type comp_version_;
string_type abi_version_;
string_type common_version_;
uint32_t _g_stream_version_ = RX_CURRENT_SERIALIZE_VERSION;
}

int get_binded_stream_version()
{
	return _g_stream_version_;
}

rx_result_struct rx_bind_plugin(const platform_api6* api, uint32_t host_stream_version, uint32_t* plugin_stream_version)
{
	_g_stream_version_ = std::min(host_stream_version, _g_stream_version_);
	*plugin_stream_version = _g_stream_version_;

	char buff[0x100];
	g_api = api;

	buff[0] = '\0';
	ASSIGN_MODULE_VERSION(buff, RX_SERVER_NAME, RX_SERVER_MAJOR_VERSION, RX_SERVER_MINOR_VERSION, RX_SERVER_BUILD_NUMBER);
	rx_version_ = buff;


	lib_version_ = g_lib_version;
	abi_version_ = g_abi_version;
	common_version_ = g_common_version;

	sprintf(buff, "%s %d.%d.%d",
		RX_COMPILER_NAME,
		RX_COMPILER_VERSION,
		RX_COMPILER_MINOR,
		RX_COMPILER_BUILD);
#ifdef _DEBUG
	strcat(buff, " [DEBUG]");
#endif
	comp_version_ = buff;


	api_write_log_func = api->general.pWriteLog;
	api_reg_item_binary_func = api->general.pRegisterItem;
	api_reg_runtime_binary_func = api->general.prxRegisterRuntimeItem;

	api_lock_runtime_manager = api->general.prxLockRuntimeManager;
	api_unlock_runtime_manager = api->general.prxUnlockRuntimeManager;

	api_reg_source_func = api->runtime.prxRegisterSourceRuntime;
	api_reg_mapper_func = api->runtime.prxRegisterMapperRuntime;
	api_reg_mapper3_func = api->runtime.prxRegisterMapperRuntime3;
	api_reg_filter_func = api->runtime.prxRegisterFilterRuntime;

	api_reg_struct_func = api->runtime.prxRegisterStructRuntime;
	api_reg_variable_func = api->runtime.prxRegisterVariableRuntime;

	api_reg_method_func = api->runtime.prxRegisterMethodRuntime;
	api_reg_event_func = api->runtime.prxRegisterEventRuntime;
	api_reg_program_func = api->runtime.prxRegisterProgramRuntime;
	api_reg_display_func = api->runtime.prxRegisterDisplayRuntime;

	api_reg_port_func = api->runtime.prxRegisterPortRuntime;

	api_reg_relation_func=api->runtime.prxRegisterRelationRuntime;
	api_reg_data_type_func = api->runtime.prxRegisterDataTypeRuntime;

	api_reg_object_func = api->runtime.prxRegisterObjectRuntime;
	api_reg_domain_func = api->runtime.prxRegisterDomainRuntime;
	api_reg_application_func = api->runtime.prxRegisterApplicationRuntime;

	api_bind_item_func = api->runtime.prxInitCtxBindItem;
	api_init_get_current_path = api->runtime.prxInitCtxGetCurrentPath;
	api_init_get_local_value = api->runtime.prxInitCtxGetLocalValue;
	api_init_set_local_value = api->runtime.prxInitCtxSetLocalValue;
	api_get_mapping_values = api->runtime.prxInitCtxGetMappingValues;
	api_get_source_values = api->runtime.prxInitCtxGetSourceValues;
	api_item_meta = api->runtime.prxInitCtxGetItemMeta;
	api_get_data_type_func = api->runtime.prxInitCtxGetDataType;
	api_connect_item_func = api->runtime.prxInitCtxConnectItem;

	api_start_get_current_path = api->runtime.prxStartCtxGetCurrentPath;
	api_start_ctx_create_timer = api->runtime.prxStartCtxCreateTimer;
	api_get_local_value = api->runtime.prxStartCtxGetLocalValue;
	api_set_local_value = api->runtime.prxStartCtxSetLocalValue;
	api_subscribe_relation = api->runtime.prxStartCtxSubscribeRelation;


	api_get_value_func = api->runtime.prxCtxGetValue;
	api_set_value_func = api->runtime.prxCtxSetValue;
	api_write_connected_func = api->runtime.prxCtxWriteConnected;
	api_execute_connected_func = api->runtime.prxCtxExecuteConnected;
	api_set_async_pending_func = api->runtime.prxCtxSetAsyncPending;

	api_reg_storage_func = api->storage.prxRegisterStorageType;
	api_write_binded_func = api->runtime.prxCtxWriteBinded;

	api_get_new_unique_id_func = api->runtime.rxGetNewUniqueId;


	return rx_result(true).move();
}

void rx_get_plugin_info(const char* info
	, string_value_struct* plugin_ver, string_value_struct* lib_ver, string_value_struct* sys_ver, string_value_struct* comp_ver)
{
	rx_init_string_value_struct(plugin_ver, info, -1);
	rx_init_string_value_struct(lib_ver, lib_version_.c_str(), -1);
	rx_init_string_value_struct(sys_ver, rx_version_.c_str(), -1);
	rx_init_string_value_struct(comp_ver, comp_version_.c_str(), -1);
}


void rx_get_plugin_info2(const char* info
	, string_value_struct* plugin_ver
	, string_value_struct* lib_ver
	, string_value_struct* sys_ver
	, string_value_struct* comp_ver
	, string_value_struct* abi_ver
	, string_value_struct* common_ver)
{
	rx_init_string_value_struct(plugin_ver, info, -1);
	rx_init_string_value_struct(lib_ver, lib_version_.c_str(), -1);
	rx_init_string_value_struct(sys_ver, rx_version_.c_str(), -1);
	rx_init_string_value_struct(comp_ver, comp_version_.c_str(), -1);
	rx_init_string_value_struct(abi_ver, abi_version_.c_str(), -1);
	rx_init_string_value_struct(common_ver, common_version_.c_str(), -1);
}


rx_result_struct rx_init_plugin(rx_platform_plugin* plugin)
{
	return plugin->init_plugin().move();
}
rx_result_struct rx_deinit_plugin(rx_platform_plugin* plugin)
{
	return plugin->deinit_plugin().move();
}

rx_result_struct rx_build_plugin(rx_platform_plugin* plugin, const char* root)
{
	api_plugin_root = root;
	rx_result_struct ret = plugin->build_plugin().move();
	api_plugin_root.clear();
	return ret;
}
extern "C" {
	extern uint32_t g_bind_version;
}
uint32_t rx_get_plugin_bind_version()
{
	return g_bind_version;
}

// Class rx_platform_api::rx_platform_plugin 


// Class rx_platform_api::rx_shared_reference 


} // namespace rx_platform_api

