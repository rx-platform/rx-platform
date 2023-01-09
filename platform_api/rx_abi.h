

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_abi.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_abi_h
#define rx_abi_h 1



#include "protocols/ansi_c/common_c/rx_packet_buffer.h"
#include "protocols/ansi_c/common_c/rx_protocol_address.h"
#define RX_IO_DATA_INPUT_MASK 0x01
#define RX_IO_DATA_OUTPUT_MASK 0x02



#ifdef __cplusplus
extern "C" {
#endif
	RX_PLATFORM_API void rxWriteLog(uintptr_t plugin, int type, const char* library, const char* source, uint16_t level, const char* code, const char* message);
	RX_PLATFORM_API rx_result_struct rxRegisterItem(uintptr_t plugin, uint8_t item_type, const char* name, const char* path
		, const rx_node_id_struct* id, const rx_node_id_struct* parent
		, uint32_t version, rx_time_struct modified, uint32_t stream_version, const uint8_t* data, size_t count);
	RX_PLATFORM_API rx_result_struct rxRegisterRuntimeItem(uintptr_t plugin, uint8_t item_type, const char* name, const char* path
		, const rx_node_id_struct* id, const rx_node_id_struct* parent
		, uint32_t version, rx_time_struct modified, uint32_t stream_version, const uint8_t* data, size_t count);
	RX_PLATFORM_API void rxLockRuntimeManager();
	RX_PLATFORM_API void rxUnlockRuntimeManager();

	typedef void(*rxWriteLog_t)(uintptr_t plugin, int type, const char* library, const char* source, uint16_t level, const char* code, const char* message);
	typedef rx_result_struct(*rxRegisterItem_t)(uintptr_t plugin, uint8_t item_type, const char* name, const char* path
		, const rx_node_id_struct* id, const rx_node_id_struct* parent
		, uint32_t version, rx_time_struct modified, uint32_t stream_version, const uint8_t* data, size_t count);
	typedef rx_result_struct(*rxRegisterRuntimeItem_t)(uintptr_t plugin, uint8_t item_type, const char* name, const char* path
		, const rx_node_id_struct* id, const rx_node_id_struct* parent
		, uint32_t version, rx_time_struct modified, uint32_t stream_version, const uint8_t* data, size_t count);
	typedef void(*rxLockRuntimeManager_t)();
	typedef void(*rxUnlockRuntimeManager_t)();



	typedef struct platform_general_api_t
	{
		rxWriteLog_t pWriteLog;
		rxRegisterItem_t pRegisterItem;
		rxRegisterRuntimeItem_t prxRegisterRuntimeItem;

		rxLockRuntimeManager_t prxLockRuntimeManager;
		rxUnlockRuntimeManager_t prxUnlockRuntimeManager;

	} platform_general_api;



	typedef void* runtime_ctx_ptr;

	typedef void(*process_func_t)(void* whose);

	typedef struct plugin_job_struct_t
	{
		lock_reference_struct anchor;
		process_func_t process;

	} plugin_job_struct;



	typedef void (*rx_runtime_register_func_t)(const rx_node_id_struct* id, lock_reference_struct* what);
	typedef void (*rx_runtime_unregister_func_t)(const rx_node_id_struct* id);

	struct plugin_source_runtime_struct_t;
	typedef struct plugin_source_runtime_struct_t* (*rx_source_constructor_t)();
	typedef struct plugin_source_register_data_t
	{
		rx_source_constructor_t constructor;
		rx_runtime_register_func_t register_func;
		rx_runtime_unregister_func_t unregister_func;

	} plugin_source_register_data;
	RX_PLATFORM_API rx_result_struct rxRegisterSourceRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_source_register_data construct_data);
	typedef rx_result_struct(*rxRegisterSourceRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, plugin_source_register_data construct_data);

	struct plugin_mapper_runtime_struct_t;
	typedef struct plugin_mapper_runtime_struct_t* (*rx_mapper_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterMapperRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_mapper_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterMapperRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_mapper_constructor_t construct_func);

	struct plugin_filter_runtime_struct_t;
	typedef struct plugin_filter_runtime_struct_t* (*rx_filter_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterFilterRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_filter_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterFilterRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_filter_constructor_t construct_func);

	struct plugin_struct_runtime_struct_t;
	typedef struct plugin_struct_runtime_struct_t* (*rx_struct_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterStructRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_struct_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterStructRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_struct_constructor_t construct_func);

	struct plugin_variable_runtime_struct_t;
	typedef struct plugin_variable_runtime_struct_t* (*rx_variable_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterVariableRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_variable_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterVariableRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_variable_constructor_t construct_func);

	struct plugin_event_runtime_struct_t;
	typedef struct plugin_event_runtime_struct_t* (*rx_event_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterEventRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_event_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterEventRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_event_constructor_t construct_func);


	struct plugin_object_runtime_struct_t;
	typedef struct plugin_object_runtime_struct_t* (*rx_object_constructor_t)();
	typedef struct plugin_object_register_data_t
	{
		rx_object_constructor_t constructor;
		rx_runtime_register_func_t register_func;
		rx_runtime_unregister_func_t unregister_func;

	} plugin_object_register_data;
	RX_PLATFORM_API rx_result_struct rxRegisterObjectRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_object_register_data construct_data);
	typedef rx_result_struct(*rxRegisterObjectRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, plugin_object_register_data construct_data);

	struct plugin_application_runtime_struct_t;
	typedef struct plugin_application_runtime_struct_t* (*rx_application_constructor_t)();
	typedef struct plugin_application_register_data_t
	{
		rx_application_constructor_t constructor;
		rx_runtime_register_func_t register_func;
		rx_runtime_unregister_func_t unregister_func;

	} plugin_application_register_data;
	RX_PLATFORM_API rx_result_struct rxRegisterApplicationRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_application_register_data construct_data);
	typedef rx_result_struct(*rxRegisterApplicationRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, plugin_application_register_data construct_data);

	struct plugin_domain_runtime_struct_t;
	typedef struct plugin_domain_runtime_struct_t* (*rx_domain_constructor_t)();
	typedef struct plugin_domain_register_data_t
	{
		rx_domain_constructor_t constructor;
		rx_runtime_register_func_t register_func;
		rx_runtime_unregister_func_t unregister_func;

	} plugin_domain_register_data;
	RX_PLATFORM_API rx_result_struct rxRegisterDomainRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_domain_register_data construct_data);
	typedef rx_result_struct(*rxRegisterDomainRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, plugin_domain_register_data construct_data);

	struct plugin_port_runtime_struct_t;
	typedef struct plugin_port_runtime_struct_t* (*rx_port_constructor_t)();
	typedef struct plugin_port_register_data_t
	{
		rx_port_constructor_t constructor;
		rx_runtime_register_func_t register_func;
		rx_runtime_unregister_func_t unregister_func;

	} plugin_port_register_data;
	RX_PLATFORM_API rx_result_struct rxRegisterPortRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_port_register_data construct_data);
	typedef rx_result_struct(*rxRegisterPortRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, plugin_port_register_data construct_data);

	struct plugin_relation_runtime_struct_t;
	typedef struct plugin_relation_runtime_struct_t* (*rx_relation_constructor_t)();
	typedef struct plugin_relation_register_data_t
	{
		rx_relation_constructor_t constructor;
		rx_runtime_register_func_t register_func;
		rx_runtime_unregister_func_t unregister_func;

	} plugin_relation_register_data;
	RX_PLATFORM_API rx_result_struct rxRegisterRelationRuntime(uintptr_t plugin, const rx_node_id_struct* id, plugin_relation_register_data construct_data);
	typedef rx_result_struct(*rxRegisterRelationRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, plugin_relation_register_data construct_data);



	struct plugin_method_runtime_struct_t;
	typedef struct plugin_method_runtime_struct_t* (*rx_method_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterMethodRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_method_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterMethodRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_method_constructor_t construct_func);

	struct plugin_program_runtime_struct_t;
	typedef struct plugin_program_runtime_struct_t* (*rx_program_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterProgramRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_program_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterProgramRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_program_constructor_t construct_func);

	struct plugin_display_runtime_struct_t;
	typedef struct plugin_display_runtime_struct_t* (*rx_display_constructor_t)();
	RX_PLATFORM_API rx_result_struct rxRegisterDisplayRuntime(uintptr_t plugin, const rx_node_id_struct* id, rx_display_constructor_t construct_func);
	typedef rx_result_struct(*rxRegisterDisplayRuntime_t)(uintptr_t plugin, const rx_node_id_struct* id, rx_display_constructor_t construct_func);


	typedef void* init_ctx_ptr;
	typedef void(*bind_callback_t)(void* target, const struct full_value_type* val);
	typedef struct bind_callback_data_t
	{
		void* target;
		bind_callback_t callback;

	} bind_callback_data;
	RX_PLATFORM_API rx_result_struct rxInitCtxBindItem(init_ctx_ptr ctx, const char* path, runtime_handle_t* handle, runtime_ctx_ptr* rt_ctx, bind_callback_data* callback);
	typedef rx_result_struct(*rxInitCtxBindItem_t)(init_ctx_ptr ctx, const char* path, runtime_handle_t* handle, runtime_ctx_ptr* rt_ctx, bind_callback_data* callback);

	RX_PLATFORM_API const char* rxInitCtxGetCurrentPath(init_ctx_ptr ctx);
	typedef const char* (*rxInitCtxGetCurrentPath_t)(init_ctx_ptr ctx);

	RX_PLATFORM_API rx_result_struct rxInitCtxGetLocalValue(init_ctx_ptr ctx, const char* path, struct typed_value_type* val);
	typedef rx_result_struct(*rxInitCtxGetLocalValue_t)(init_ctx_ptr ctx, const char* path, struct typed_value_type* val);

	RX_PLATFORM_API rx_result_struct rxInitCtxSetLocalValue(init_ctx_ptr ctx, const char* path, struct typed_value_type val);
	typedef rx_result_struct(*rxInitCtxSetLocalValue_t)(init_ctx_ptr ctx, const char* path, struct typed_value_type val);


	RX_PLATFORM_API rx_result_struct rxInitCtxGetMappingValues(init_ctx_ptr ctx, const rx_node_id_struct* id, const char* path, values_array_struct* vals);
	typedef rx_result_struct(*rxInitCtxGetMappingValues_t)(init_ctx_ptr ctx, const rx_node_id_struct* id, const char* path, values_array_struct* vals);

	RX_PLATFORM_API rx_result_struct rxInitCtxGetSourceValues(init_ctx_ptr ctx, const rx_node_id_struct* id, const char* path, values_array_struct* vals);
	typedef rx_result_struct(*rxInitCtxGetSourceValues_t)(init_ctx_ptr ctx, const rx_node_id_struct* id, const char* path, values_array_struct* vals);

	RX_PLATFORM_API void rxInitCtxGetItemMeta(init_ctx_ptr ctx, const rx_node_id_struct** id, const char** path, const char** name);
	typedef void(*rxInitCtxGetItemMeta_t)(init_ctx_ptr ctx, const rx_node_id_struct** id, const char** path, const char** name);


	typedef void* start_ctx_ptr;

#define RX_TIMER_REGULAR 0
#define RX_TIMER_CALC 1
#define RX_TIMER_IO 2

#define RX_JOB_REGULAR 0
#define RX_JOB_IO 1
#define RX_JOB_SLOW 2

	RX_PLATFORM_API const char* rxStartCtxGetCurrentPath(start_ctx_ptr ctx);
	typedef const char* (*rxStartCtxGetCurrentPath_t)(start_ctx_ptr ctx);

	RX_PLATFORM_API runtime_handle_t rxStartCtxCreateTimer(start_ctx_ptr ctx, int type, plugin_job_struct* job, uint32_t period);
	typedef runtime_handle_t(*rxStartCtxCreateTimer_t)(start_ctx_ptr ctx, int type, plugin_job_struct* job, uint32_t period);

	RX_PLATFORM_API rx_result_struct rxStartCtxGetLocalValue(start_ctx_ptr ctx, const char* path, struct typed_value_type* val);
	typedef rx_result_struct(*rxStartCtxGetLocalValue_t)(start_ctx_ptr ctx, const char* path, struct typed_value_type* val);

	RX_PLATFORM_API rx_result_struct rxStartCtxSetLocalValue(start_ctx_ptr ctx, const char* path, struct typed_value_type val);
	typedef rx_result_struct(*rxStartCtxSetLocalValue_t)(start_ctx_ptr ctx, const char* path, struct typed_value_type val);

	RX_PLATFORM_API rx_result_struct rxCtxGetValue(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type* val);
	typedef rx_result_struct(*rxCtxGetValue_t)(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type* val);

	RX_PLATFORM_API rx_result_struct rxCtxSetValue(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type val);
	typedef rx_result_struct(*rxCtxSetValue_t)(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type val);

	RX_PLATFORM_API void rxCtxSetRemotePending(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type val);
	typedef void(*rxCtxSetRemotePending_t)(runtime_ctx_ptr ctx, runtime_handle_t handle, struct typed_value_type val);


	typedef void(*relation_subscriber_connected_callback_t)(void* target, const char* name, const rx_node_id_struct* id);
	typedef void(*relation_subscriber_disconnected_callback_t)(void* target, const char* name);
	typedef struct relation_subscriber_data_t
	{
		void* target;
		relation_subscriber_connected_callback_t connected_callback;
		relation_subscriber_disconnected_callback_t disconnected_callback;

	} relation_subscriber_data;

	RX_PLATFORM_API rx_result_struct rxStartCtxSubscribeRelation(start_ctx_ptr ctx, const char* name, relation_subscriber_data* callback);
	typedef rx_result_struct(*rxStartCtxSubscribeRelation_t)(start_ctx_ptr ctx, const char* name, relation_subscriber_data* callback);


	typedef struct platform_runtime_api_t
	{

		rxRegisterSourceRuntime_t prxRegisterSourceRuntime;
		rxRegisterMapperRuntime_t prxRegisterMapperRuntime;
		rxRegisterFilterRuntime_t prxRegisterFilterRuntime;
		rxRegisterStructRuntime_t prxRegisterStructRuntime;
		rxRegisterVariableRuntime_t prxRegisterVariableRuntime;
		rxRegisterEventRuntime_t prxRegisterEventRuntime;

		rxRegisterMethodRuntime_t prxRegisterMethodRuntime;
		rxRegisterProgramRuntime_t prxRegisterProgramRuntime;
		rxRegisterDisplayRuntime_t prxRegisterDisplayRuntime;

		rxRegisterObjectRuntime_t prxRegisterObjectRuntime;
		rxRegisterApplicationRuntime_t prxRegisterApplicationRuntime;
		rxRegisterDomainRuntime_t prxRegisterDomainRuntime;
		rxRegisterPortRuntime_t prxRegisterPortRuntime;

		rxRegisterRelationRuntime_t prxRegisterRelationRuntime;

		rxInitCtxBindItem_t prxInitCtxBindItem;
		rxInitCtxGetCurrentPath_t prxInitCtxGetCurrentPath;
		rxInitCtxGetLocalValue_t prxInitCtxGetLocalValue;
		rxInitCtxSetLocalValue_t prxInitCtxSetLocalValue;
		rxInitCtxGetMappingValues_t prxInitCtxGetMappingValues;
		rxInitCtxGetSourceValues_t prxInitCtxGetSourceValues;
		rxInitCtxGetItemMeta_t prxInitCtxGetItemMeta;

		rxStartCtxGetCurrentPath_t prxStartCtxGetCurrentPath;
		rxStartCtxCreateTimer_t prxStartCtxCreateTimer;
		rxStartCtxGetLocalValue_t prxStartCtxGetLocalValue;
		rxStartCtxSetLocalValue_t prxStartCtxSetLocalValue;
		rxStartCtxSubscribeRelation_t prxStartCtxSubscribeRelation;

		rxCtxGetValue_t prxCtxGetValue;
		rxCtxSetValue_t prxCtxSetValue;
		rxCtxSetRemotePending_t prxCtxSetRemotePending;

	} platform_runtime_api;


	typedef struct platform_api_t
	{
		platform_general_api general;
		platform_runtime_api runtime;

	} platform_api;


	// common host stuff, timers...
	typedef rx_result_struct(*rx_post_job_t)(void* whose, int type, plugin_job_struct* job, uint32_t period);
	typedef runtime_handle_t(*rx_create_timer_t)(void* whose, int type, plugin_job_struct* job, uint32_t period);
	typedef void(*rx_start_timer_t)(void* whose, runtime_handle_t timer, uint32_t period);
	typedef void(*rx_suspend_timer_t)(void* whose, runtime_handle_t timer);
	typedef void(*rx_destory_timer_t)(void* whose, runtime_handle_t timer);

	typedef struct host_runtime_def_struct_t
	{
		rx_post_job_t post_job;
		rx_create_timer_t create_timer;
		rx_start_timer_t start_timer;
		rx_suspend_timer_t suspend_timer;
		rx_destory_timer_t destroy_timer;

	} host_runtime_def_struct;

	//  common functions for all plugin stuff
	typedef void(*rx_get_code_info_t)(void* whose, const char* name, string_value_struct* info);

	RX_PLUGIN_API rx_result_struct rxBindPlugin(const platform_api* api, uint32_t host_stream_version, uint32_t* plugin_stream_version, uintptr_t* plugin);

	RX_PLUGIN_API void rxGetPluginInfo(string_value_struct* plugin_ver, string_value_struct* lib_ver, string_value_struct* sys_ver, string_value_struct* comp_ver);
	RX_PLUGIN_API void rxGetPluginName(string_value_struct* name);

	RX_PLUGIN_API rx_result_struct rxInitPlugin();
	RX_PLUGIN_API rx_result_struct rxDeinitPlugin();

	RX_PLUGIN_API rx_result_struct rxBuildPlugin(const char* root);

	typedef rx_result_struct(*rxBindPlugin_t)(const struct platform_api_t* api, uint32_t host_stream_version, uint32_t* plugin_stream_version, uintptr_t* plugin);
	typedef void(*rxGetPluginInfo_t)(string_value_struct* plugin_ver, string_value_struct* lib_ver, string_value_struct* sys_ver, string_value_struct* comp_ver);
	typedef void(*rxGetPluginName_t)(string_value_struct* name);
	typedef rx_result_struct(*rxInitPlugin_t)();
	typedef rx_result_struct(*rxDeinitPlugin_t)();
	typedef rx_result_struct(*rxBuildPlugin_t)(const char* root);

	// Source ABI interface
	//!!! IMPORTANT rx_value_t is fast_uint8, so we have to convert
	typedef rx_result_struct(*rx_init_source_t)(void* whose, init_ctx_ptr ctx, uint8_t value_type);
	typedef rx_result_struct(*rx_start_source_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_source_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_source_t)(void* whose);

	typedef rx_result_struct(*rx_write_source_t)(void* whose
		, runtime_transaction_id_t id, int test, rx_security_handle_t identity
		, struct typed_value_type val, runtime_ctx_ptr ctx);

	typedef struct plugin_source_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_source_t init_source;
		rx_start_source_t start_source;
		rx_stop_source_t stop_source;
		rx_deinit_source_t deinit_source;

		rx_write_source_t write_source;

	} plugin_source_def_struct;


	typedef rx_result_struct(*rx_update_source_t)(void* whose, struct full_value_type val);
	typedef void(*rx_result_update_source_t)(void* whose, rx_result_struct result, runtime_transaction_id_t id);


	typedef struct host_source_def_struct_t
	{
		host_runtime_def_struct runtime;

		rx_update_source_t update_source;
		rx_result_update_source_t result_update_source;
		

	} host_source_def_struct;

	typedef struct plugin_source_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_source_def_struct* def;
		host_source_def_struct* host_def;
		uint32_t io_data;

	} plugin_source_runtime_struct;

	// Mapper ABI interface
	//!!! IMPORTANT rx_value_t is fast_uint8, so we have to convert
	typedef rx_result_struct(*rx_init_mapper_t)(void* whose, init_ctx_ptr ctx, uint8_t value_type);
	typedef rx_result_struct(*rx_start_mapper_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_mapper_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_mapper_t)(void* whose);

	typedef void(*rx_mapped_value_changed_t)(void* whose, struct full_value_type val, runtime_ctx_ptr ctx);
	typedef void(*rx_mapper_result_received_t)(void* whose, rx_result_struct result, runtime_transaction_id_t id, runtime_ctx_ptr ctx);

	typedef struct plugin_mapper_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_mapper_t init_mapper;
		rx_start_mapper_t start_mapper;
		rx_stop_mapper_t stop_mapper;
		rx_deinit_mapper_t deinit_mapper;

		rx_mapped_value_changed_t mapped_value_changed;
		rx_mapper_result_received_t mapper_result_received;

	} plugin_mapper_def_struct;


	typedef rx_result_struct(*rx_mapper_write_pending_t)(void* whose
		, runtime_transaction_id_t id, int test, rx_security_handle_t identity, struct typed_value_type val);
	typedef void(*rx_mapper_map_current_t)(void* whose);

	typedef struct host_mapper_def_struct_t
	{
		host_runtime_def_struct runtime;

		rx_mapper_write_pending_t mapper_write_pending;
		rx_mapper_map_current_t map_current_value;


	} host_mapper_def_struct;

	typedef struct plugin_mapper_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_mapper_def_struct* def;
		host_mapper_def_struct* host_def;
		uint32_t io_data;

	} plugin_mapper_runtime_struct;

	// Filter ABI interface
	typedef rx_result_struct(*rx_init_filter_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_filter_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_filter_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_filter_t)(void* whose);

	typedef rx_result_struct(*rx_filter_input_t)(void* whose, struct full_value_type* val);
	typedef rx_result_struct(*rx_filter_output_t)(void* whose, struct typed_value_type* val);

	typedef struct plugin_filter_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_filter_t init_filter;
		rx_start_filter_t start_filter;
		rx_stop_filter_t stop_filter;
		rx_deinit_filter_t deinit_filter;

		rx_filter_input_t filter_input;
		rx_filter_output_t filter_output;

	} plugin_filter_def_struct;


	typedef rx_result_struct(*rx_filter_changed_t)(void* whose);

	typedef struct host_filter_def_struct_t
	{
		host_runtime_def_struct runtime;

		rx_filter_changed_t filter_changed;

	} host_filter_def_struct;

	typedef struct plugin_filter_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_filter_def_struct* def;
		host_filter_def_struct* host_def;
		uint32_t io_data;

	} plugin_filter_runtime_struct;

	// Struct ABI interface
	typedef rx_result_struct(*rx_init_struct_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_struct_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_struct_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_struct_t)(void* whose);


	typedef struct plugin_struct_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_struct_t init_struct;
		rx_start_struct_t start_struct;
		rx_stop_struct_t stop_struct;
		rx_deinit_struct_t deinit_struct;


	} plugin_struct_def_struct;


	typedef struct host_struct_def_struct_t
	{
		host_runtime_def_struct runtime;

	} host_struct_def_struct;

	typedef struct plugin_struct_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_struct_def_struct* def;
		host_struct_def_struct* host_def;
		uint32_t io_data;

	} plugin_struct_runtime_struct;

	// Variable ABI interface
	typedef rx_result_struct(*rx_init_variable_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_variable_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_variable_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_variable_t)(void* whose);


	typedef struct plugin_variable_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_variable_t init_variable;
		rx_start_variable_t start_variable;
		rx_stop_variable_t stop_variable;
		rx_deinit_variable_t deinit_variable;

	} plugin_variable_def_struct;



	typedef struct host_variable_def_struct_t
	{
		host_runtime_def_struct runtime;

	} host_variable_def_struct;

	typedef struct plugin_variable_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_variable_def_struct* def;
		host_variable_def_struct* host_def;
		uint32_t io_data;

	} plugin_variable_runtime_struct;

	// Event ABI interface
	typedef rx_result_struct(*rx_init_event_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_event_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_event_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_event_t)(void* whose);


	typedef struct plugin_event_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_event_t init_event;
		rx_start_event_t start_event;
		rx_stop_event_t stop_event;
		rx_deinit_event_t deinit_event;


	} plugin_event_def_struct;


	//typedef rx_result_struct(*rx_event_changed_t)(void* whose);

	typedef struct host_event_def_struct_t
	{
		host_runtime_def_struct runtime;

	//	rx_event_changed_t event_changed;

	} host_event_def_struct;

	typedef struct plugin_event_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_event_def_struct* def;
		host_event_def_struct* host_def;
		uint32_t io_data;

	} plugin_event_runtime_struct;


	// Method ABI interface
	typedef rx_result_struct(*rx_init_method_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_method_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_method_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_method_t)(void* whose);


	typedef struct plugin_method_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_method_t init_method;
		rx_start_method_t start_method;
		rx_stop_method_t stop_method;
		rx_deinit_method_t deinit_method;


	} plugin_method_def_struct;


	//typedef rx_result_struct(*rx_method_changed_t)(void* whose);

	typedef struct host_method_def_struct_t
	{
		host_runtime_def_struct runtime;

		//	rx_method_changed_t method_changed;

	} host_method_def_struct;

	typedef struct plugin_method_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_method_def_struct* def;
		host_method_def_struct* host_def;
		uint32_t io_data;

	} plugin_method_runtime_struct;


	// Program ABI interface
	typedef rx_result_struct(*rx_init_program_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_program_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_program_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_program_t)(void* whose);


	typedef struct plugin_program_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_program_t init_program;
		rx_start_program_t start_program;
		rx_stop_program_t stop_program;
		rx_deinit_program_t deinit_program;


	} plugin_program_def_struct;


	//typedef rx_result_struct(*rx_program_changed_t)(void* whose);

	typedef struct host_program_def_struct_t
	{
		host_runtime_def_struct runtime;

		//	rx_program_changed_t program_changed;

	} host_program_def_struct;

	typedef struct plugin_program_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_program_def_struct* def;
		host_program_def_struct* host_def;
		uint32_t io_data;

	} plugin_program_runtime_struct;


	// Program ABI interface
	typedef rx_result_struct(*rx_init_display_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_display_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_display_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_display_t)(void* whose);


	typedef struct plugin_display_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_display_t init_display;
		rx_start_display_t start_display;
		rx_stop_display_t stop_display;
		rx_deinit_display_t deinit_display;


	} plugin_display_def_struct;


	//typedef rx_result_struct(*rx_display_changed_t)(void* whose);

	typedef struct host_display_def_struct_t
	{
		host_runtime_def_struct runtime;

		//	rx_display_changed_t display_changed;

	} host_display_def_struct;

	typedef struct plugin_display_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_display_def_struct* def;
		host_display_def_struct* host_def;
		uint32_t io_data;

	} plugin_display_runtime_struct;

	// Object ABI interface
	typedef rx_result_struct(*rx_init_object_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_object_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_object_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_object_t)(void* whose);


	typedef struct plugin_object_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_object_t init_object;
		rx_start_object_t start_object;
		rx_stop_object_t stop_object;
		rx_deinit_object_t deinit_object;

	} plugin_object_def_struct;

	typedef struct host_object_def_struct_t
	{

		host_runtime_def_struct runtime;

	} host_object_def_struct;

	typedef struct plugin_object_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_object_def_struct* def;
		host_object_def_struct* host_def;
		uint32_t io_data;

	} plugin_object_runtime_struct;

	// Domain ABI interface
	typedef rx_result_struct(*rx_init_domain_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_domain_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_domain_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_domain_t)(void* whose);


	typedef struct plugin_domain_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_domain_t init_domain;
		rx_start_domain_t start_domain;
		rx_stop_domain_t stop_domain;
		rx_deinit_domain_t deinit_domain;

	} plugin_domain_def_struct;

	typedef struct host_domain_def_struct_t
	{

		host_runtime_def_struct runtime;

	} host_domain_def_struct;

	typedef struct plugin_domain_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_domain_def_struct* def;
		host_domain_def_struct* host_def;
		uint32_t io_data;

	} plugin_domain_runtime_struct;

	// Application ABI interface
	typedef rx_result_struct(*rx_init_application_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_application_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_application_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_application_t)(void* whose);


	typedef struct plugin_application_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_application_t init_application;
		rx_start_application_t start_application;
		rx_stop_application_t stop_application;
		rx_deinit_application_t deinit_application;

	} plugin_application_def_struct;

	typedef struct host_application_def_struct_t
	{

		host_runtime_def_struct runtime;

	} host_application_def_struct;

	typedef struct plugin_application_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_application_def_struct* def;
		host_application_def_struct* host_def;
		uint32_t io_data;

	} plugin_application_runtime_struct;

	// Port ABI interface
	typedef rx_result_struct(*rx_init_port_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_port_t)(void* whose, start_ctx_ptr ctx);
	typedef rx_result_struct(*rx_stop_port_t)(void* whose);
	typedef rx_result_struct(*rx_deinit_port_t)(void* whose);

	typedef void(*rx_stack_assembled_t)(void* whose);
	typedef void(*rx_stack_disassembled_t)(void* whose);

	typedef void (*rx_extract_bind_address_t)(void* whose, const uint8_t* binder_data, size_t binder_data_size, protocol_address* local_addr, protocol_address* remote_addr);

	typedef void (*rx_destroy_endpoint_t)(void* whose, struct rx_protocol_stack_endpoint* endpoint);

	typedef struct rx_protocol_stack_endpoint* (*rx_construct_listener_endpoint_t)(void* whose, const struct protocol_address_def* local_address, const struct protocol_address_def* remote_address);
	typedef struct rx_protocol_stack_endpoint* (*rx_construct_initiator_endpoint_t)(void* whose);

	typedef struct plugin_port_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_port_t init_port;
		rx_start_port_t start_port;
		rx_stop_port_t stop_port;
		rx_deinit_port_t deinit_port;

		rx_stack_assembled_t stack_assembled;
		rx_stack_assembled_t stack_disassembled;
		
		rx_extract_bind_address_t extract_bind_address;

		rx_destroy_endpoint_t destroy_endpoint;
		
		rx_construct_listener_endpoint_t construct_listener_endpoint;
		rx_construct_initiator_endpoint_t construct_initiator_endpoint;

	} plugin_port_def_struct;


	typedef rx_result_struct(*rx_alloc_io_buffer_t)(void* whose, rx_packet_buffer* buffer);
	typedef void (*rx_release_io_buffer_t)(void* whose, rx_packet_buffer buffer);

	typedef rx_result_struct(*rx_listen_t)(void* whose, const struct protocol_address_def* local_address, const struct protocol_address_def* remote_address);
	typedef rx_result_struct(*rx_connect_t)(void* whose, const struct protocol_address_def* local_address, const struct protocol_address_def* remote_address);

	typedef rx_result_struct(*rx_unbind_stack_endpoint_t)(void* whose, struct rx_protocol_stack_endpoint* what);
	typedef rx_result_struct(*rx_disconnect_stack_endpoint_t)(void* whose, struct rx_protocol_stack_endpoint* what);

	typedef rx_result_struct(*rx_bind_listener_endpoint_t)(void* whose, struct rx_protocol_stack_endpoint* what, const struct protocol_address_def* local_address, const struct protocol_address_def* remote_address);

	typedef struct host_port_def_struct_t
	{
		host_runtime_def_struct runtime;

		rx_alloc_io_buffer_t alloc_buffer;
		rx_release_io_buffer_t release_buffer;

		rx_listen_t listen;
		rx_connect_t connect;

		rx_unbind_stack_endpoint_t unbind_stack_endpoint;
		rx_disconnect_stack_endpoint_t disconnect_stack_endpoint;
		rx_bind_listener_endpoint_t bind_listener_endpoint;


	} host_port_def_struct;

	typedef struct plugin_port_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_port_def_struct* def;
		host_port_def_struct* host_def;
		uint32_t io_data;

	} plugin_port_runtime_struct;

	// Relation ABI interface
	typedef rx_result_struct(*rx_init_relation_t)(void* whose, init_ctx_ptr ctx);
	typedef rx_result_struct(*rx_start_relation_t)(void* whose, start_ctx_ptr ctx, int target);
	typedef rx_result_struct(*rx_stop_relation_t)(void* whose, int target);
	typedef rx_result_struct(*rx_deinit_relation_t)(void* whose);

	struct plugin_relation_runtime_struct_t;
	typedef rx_result_struct(*rx_make_target_relation_t)(void* whose, struct plugin_relation_runtime_struct_t** target);
	typedef rx_result_struct(*rx_relation_connected_t)(void* whose, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);
	typedef rx_result_struct(*rx_relation_disconnected_t)(void* whose, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);

	typedef struct plugin_relation_def_struct_t
	{
		rx_get_code_info_t code_info;

		rx_init_relation_t init_relation;
		rx_start_relation_t start_relation;
		rx_stop_relation_t stop_relation;
		rx_deinit_relation_t deinit_relation;

		rx_make_target_relation_t make_target_relation;
		rx_relation_connected_t relation_connected;
		rx_relation_disconnected_t relation_disconnected;

	} plugin_relation_def_struct;


	typedef rx_result_struct(*rx_relation_changed_t)(void* whose);

	typedef struct host_relation_def_struct_t
	{
		host_runtime_def_struct runtime;

	//	rx_relation_changed_t relation_changed;

	} host_relation_def_struct;

	typedef struct plugin_relation_runtime_struct_t
	{
		lock_reference_struct anchor;
		void* host;
		plugin_relation_def_struct* def;
		host_relation_def_struct* host_def;
		uint32_t io_data;

	} plugin_relation_runtime_struct;


#ifdef __cplusplus
}
#endif




#endif
