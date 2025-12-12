

/****************************************************************************
*
*  dotnet\dotnet_api.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef dotnet_api_h
#define dotnet_api_h 1


#ifdef _WIN32
#include "dotnet_win/coreclr_delegates.h"
#else
#include "dotnet_linux/coreclr_delegates.h"
#endif



#include "platform_api/rx_abi.h"



#ifdef __cplusplus
extern "C" {
#endif
	typedef void(*dotnetWriteLogDelegate_t)(
		int type
		, const char* library
		, const char* source
		, uint16_t level
		, const char* code
		, const char* message);


	typedef rx_result_struct(*dotnetBuildType_t)(rx_item_type type, const char* module, const rx_node_id_struct_t* id, const rx_node_id_struct_t* parent
		, const char* name, const char* path, uint32_t version, uint32_t flags, const char* def, int ownType
		, const char* conns, const char* code_info);
	
	typedef rx_result_struct(*dotnetDeleteType_t)(rx_item_type type, const char* module, const rx_node_id_struct_t* id);


	typedef void(*dotnetRuntimeCallback_t)(uint64_t transId, rx_result_struct result);

	typedef void(*dotnetResultCallback_t)(uint32_t transId, rx_result_struct result);
	typedef void(*dotnetVoidCallback_t)(uint32_t transId);

	typedef void(*dotnetBuildRuntime_t)(uint64_t transId, rx_item_type type, const char* module, const rx_node_id_struct_t* id, const rx_node_id_struct_t* parent
		, const char* name, const char* path, uint32_t version, uint32_t flags, const char* def, void* instance, dotnetRuntimeCallback_t callback);


	typedef void(*dotnetWriteValue_t)(
		rx_item_type itemType
		, uint32_t transId
		, void* instance
		, size_t idx
		, typed_value_type value
		, dotnetRuntimeCallback_t callback /*dotnetRuntimeResultDelegate*/);


	typedef void(*dotnetExecuteDone_t)(
		uint32_t transId
		, void* instance
		, const char* value
		, rx_result_struct result);


	typedef void(*dotnetSourceWriteDone_t)(
		uint32_t transId
		, void* instance
		, rx_result_struct result);


	typedef void(*dotnetSourceChange_t)(
		void* instance
		, full_value_type value);


	typedef void(*dotnetDeleteRuntime_t)(uint64_t transId, rx_item_type type, const char* module, const rx_node_id_struct_t* id, dotnetRuntimeCallback_t callback);

	void dotnetWriteLog(int type, const char* library, const char* source, uint16_t level, const char* code, const char* message);

	rx_result_struct dotnetBuildType(rx_item_type type, const char* module, const rx_node_id_struct_t* id, const rx_node_id_struct_t* parent
		, const char* name, const char* path, uint32_t version, uint32_t flags, const char* def, int ownType
		, const char* conns, const char* code_info);

	rx_result_struct dotnetDeleteType(rx_item_type type, const char* module, const rx_node_id_struct_t* id);

	void dotnetBuildRuntime(uint64_t transId, rx_item_type type, const char* module, const rx_node_id_struct_t* id, const rx_node_id_struct_t* parent
		, const char* name, const char* path, uint32_t version, uint32_t flags, const char* def, void* instance, dotnetRuntimeCallback_t callback);


	void dotnetDeleteRuntime(uint64_t transId, rx_item_type type, const char* module, const rx_node_id_struct_t* id, dotnetRuntimeCallback_t callback);

	void dotnetResultCallback(uint32_t transId, rx_result_struct result);
	void dotnetVoidCallback(uint32_t transId);



	void dotnetWriteValue(
		rx_item_type itemType
		, uint32_t transId
		, void* instance
		, size_t idx
		, typed_value_type value
		, dotnetRuntimeCallback_t callback/*dotnetRuntimeResultDelegate*/);


	void dotnetExecuteDone(
		uint32_t transId
		, void* instance
		, const char* value,
		rx_result_struct result);

	void dotnetSourceWriteDone(
		uint32_t transId
		, void* instance
		, rx_result_struct result);


	void dotnetSourceChange(
		void* instance
		, full_value_type value);


	typedef struct dotnet_loading_api_t
	{
		dotnetWriteLogDelegate_t write_log;

		dotnetBuildType_t build_type;
		dotnetDeleteType_t delete_type;

		dotnetBuildRuntime_t build_runtime;
		dotnetDeleteRuntime_t delete_runtime;

		dotnetResultCallback_t result_callback;
		dotnetVoidCallback_t void_callback;

		dotnetWriteValue_t write_value;

		dotnetExecuteDone_t execute_done;
		dotnetSourceWriteDone_t source_write_done;
		dotnetSourceChange_t source_change;

	} dotnet_loading_api;


	typedef rx_result_struct(CORECLR_DELEGATE_CALLTYPE* init_hosting_entry_point_fn)(dotnet_loading_api_t* apiPtr, uint32_t host_stream_version, uint32_t* dotnet_stream_version, const char** assemblies, uint32_t count, string_value_struct* version);
	typedef rx_result_struct(CORECLR_DELEGATE_CALLTYPE* start_hosting_entry_point_fn)();
	typedef void(CORECLR_DELEGATE_CALLTYPE* bind_object_entry_point_fn)(rx_item_type type, const rx_node_id_struct* node_id, const char* path, const rx_node_id_struct* parent_id, void* runtime);
	typedef rx_result_struct(CORECLR_DELEGATE_CALLTYPE* build_library_hosting_entry_point_fn)(const char* lib);
	typedef void (CORECLR_DELEGATE_CALLTYPE* deinit_library_hosting_entry_point_fn)(const char* lib, uint32_t transId);
	typedef void (CORECLR_DELEGATE_CALLTYPE* deinit_hosting_entry_point_fn)(uint32_t transId);

	typedef void (CORECLR_DELEGATE_CALLTYPE* value_changed_entry_point_fn)(size_t idx, typed_value_type value, rx_item_type type, void* whose);
	typedef void (CORECLR_DELEGATE_CALLTYPE* initial_values_entry_point_fn)(size_t count, char** names, typed_value_type* values, rx_item_type type, void* whose);

	typedef rx_result_struct(CORECLR_DELEGATE_CALLTYPE* execute_method_entry_point_fn)(uint32_t transId, void* whose, const char* method, const char* value);

	typedef void (CORECLR_DELEGATE_CALLTYPE* source_write_entry_point_fn)(uint32_t transId, typed_value_type value, void* whose);
	typedef void (CORECLR_DELEGATE_CALLTYPE* mapped_changed_entry_point_fn)(full_value_type value, void* whose);



#ifdef __cplusplus
}
#endif

struct dotnet_code_info
{
	dotnet_code_info(const string_type& data)
	{
		size_t pos1 = data.find('|');
		if (pos1 != string_type::npos)
		{
			assembly_name = data.substr(0, pos1);
			size_t pos2 = data.find('|', pos1 + 1);
			if (pos2 != string_type::npos)
			{
				class_name = data.substr(pos1 + 1, pos2 - pos1 - 1);
				file_name = data.substr(pos2 + 1);
			}
			else
			{
				class_name = data.substr(pos1 + 1);
			}
		}
		else
		{
			assembly_name = data;
		}
	}
	string_type assembly_name;
	string_type class_name;
	string_type file_name;
};

namespace rx_internal
{
namespace dotnet
{


std::vector<std::pair<string_type, string_type> > get_loaded_dotnet_libraries();



} // namespace dotnet
} // namespace rx_internal


#endif
