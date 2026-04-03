

/****************************************************************************
*
*  dotnet_wp\dotnet_wp_api.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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

#include "dotnet_wp_protocol.h"

// dotnet_wp_api
#include "dotnet_wp/dotnet_wp_api.h"

#include "dotnet_wp/dotnet_wp.h"



#ifdef __cplusplus
extern "C" {
#endif

	void dotnetWriteLog(int type, const char* library, const char* source, uint16_t level, const char* code, const char* message)
	{
		rx_internal::dotnet::dotnet_wp::instance().log_event_fast((rx_internal::dotnet::log_event_type)type, library, source, level, code, message);
	}


	rx_result_struct dotnetBuildType(rx_item_type type, const char* module, const rx_node_id_struct_t* id, const rx_node_id_struct_t* parent
		, const char* name, const char* path, uint32_t version, uint32_t flags, const char* def, int ownType
		, const char* conns, const char* code_info)
	{
		rx_internal::dotnet::build_one_type_message msg;
		msg.type = type;
		msg.library = module;
		msg.id = rx::rx_node_id(id);
		msg.parent = rx::rx_node_id(parent);
		msg.name = name;
		msg.path = path;
		msg.version = version;
		msg.flags = flags;
		msg.def = def;
		msg.ownType = ownType;
		msg.conns = conns;
		msg.code_info = code_info;

		auto trans = std::make_shared < rx_internal::dotnet::build_type_transaction>(std::move(msg), 1000);

		rx_internal::dotnet::dotnet_wp::instance().append_trans(trans);

		auto result = trans->wait_result();
		return result.move();
	}
	rx_result_struct dotnetDeleteType(rx_item_type type, const char* module, const rx_node_id_struct_t* id)
	{
		rx_internal::dotnet::delete_one_type_message msg;
		msg.type = type;
		msg.library = module;
		msg.id = rx::rx_node_id(id);

		auto trans = std::make_shared < rx_internal::dotnet::delete_wp_type_transaction>(std::move(msg), 1000);

		rx_internal::dotnet::dotnet_wp::instance().append_trans(trans);

		auto result = trans->wait_result();
		return result.move();
	}

	void dotnetBuildRuntime(uint64_t transId, rx_item_type type, const char* module, const rx_node_id_struct_t* id, const rx_node_id_struct_t* parent
		, const char* name, const char* path, uint32_t version, uint32_t flags, const char* def, void* instance, dotnetRuntimeCallback_t callback)
	{
		if (module == nullptr)
		{
			rx_result result = rx_result("Invalid module data!").move();
			callback(transId, result.move());
			return;
		}
		if (def == nullptr)
		{
			rx_result result = rx_result("Invalid definition data!").move();
			callback(transId, result.move());
			return;
		}
		if (name == nullptr || path == nullptr)
		{
			rx_result result = rx_result("Invalid name or path!").move();
			callback(transId, result.move());
			return;
		}

		rx_internal::dotnet::build_runtime_message msg;
		msg.type = type;
		msg.library = module;
		msg.id = rx::rx_node_id(id);
		msg.parent = rx::rx_node_id(parent);
		msg.name = name;
		msg.path = path;
		msg.version = version;
		msg.flags = flags;
		msg.def = def;
		msg.instance = instance;

		auto trans = std::make_shared < rx_internal::dotnet::build_runtime_transaction>(transId, std::move(msg), callback, 1000);

		rx_internal::dotnet::dotnet_wp::instance().append_trans(trans);

	}

	void dotnetDeleteRuntime(uint64_t transId, rx_item_type type, const char* module, const rx_node_id_struct_t* id, dotnetRuntimeCallback_t callback)
	{
		if (module == nullptr)
		{
			rx_result result = rx_result("Invalid module data!").move();
			callback(transId, result.move());
			return;
		}


		rx_internal::dotnet::delete_one_runtime_message msg;
		msg.type = type;
		msg.library = module;
		msg.id = rx::rx_node_id(id);

		auto trans = std::make_shared < rx_internal::dotnet::delete_runtime_transaction>(transId, std::move(msg), callback, 1000);

		rx_internal::dotnet::dotnet_wp::instance().append_trans(trans);

	}


	void dotnetResultCallback(uint32_t transId, rx_result_struct result)
	{
		rx_internal::dotnet::dotnet_wp::instance().send_result_response(transId, rx_result(std::move(result)));
	}
	void dotnetVoidCallback(uint32_t transId)
	{
		//rx_internal::dotnet::dotnet_hosting::instance().received_void_callback(transId);
	}
	void dotnetWriteValue(
		rx_item_type itemType
		, uint32_t transId
		, void* instance
		, size_t idx
		, typed_value_type value
		, dotnetRuntimeCallback_t callback/*dotnetRuntimeResultDelegate*/)
	{
		rx_internal::dotnet::wp_write_value_message msg;
		msg.type = itemType;
		msg.inst = (intptr_t)instance;
		msg.idx = idx;
		msg.val = rx_simple_value(std::move(value));

		auto trans = std::make_shared<rx_internal::dotnet::wp_write_transaction>(transId, std::move(msg), callback, 1000);

		rx_internal::dotnet::dotnet_wp::instance().append_trans(trans);
	}


	void dotnetExecuteDone(
		uint32_t transId
		, void* instance
		, const char* value,
		rx_result_struct result)
	{

		rx_internal::dotnet::dotnet_execute_method_response msg;

		if(value)
			msg.result_value = value;
		msg.action_result = rx_result(result);

		rx_internal::dotnet::dotnet_wp::instance().send_message(std::move(msg), transId);

	}

	void dotnetSourceWriteDone(
		uint32_t transId
		, void* instance
		, rx_result_struct result)
	{
		rx_result temp_value(std::move(result));
	//	rx_internal::dotnet::dotnet_hosting::instance().received_source_write_done(transId, instance, std::move(temp_value));


	}


	void dotnetSourceChange(
		void* instance
		, full_value_type value)
	{
		rx_value temp_value(std::move(value));
	//	rx_internal::dotnet::dotnet_hosting::instance().received_source_change(instance, std::move(temp_value));
	}


	void dotnetFireEvent(
		void* instance, const char* value, const char* queue, int state, int remove)
	{
		string_type temp_value(value);
		string_type temp_queue(queue);


		rx_internal::dotnet::dotnet_wp::instance().send_fire_event(instance, std::move(temp_value), std::move(temp_queue), state != 0, remove != 0);
	}




#ifdef __cplusplus
}
#endif




