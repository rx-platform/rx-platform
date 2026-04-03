

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


#include "system/callbacks/rx_callback.h"
using rx_platform::rx_result_callback;




/////////////////////////////////////////////////////////////
// logging macros for http library
#define DOTNET_LOG_INFO(src,lvl,msg) RX_LOG_INFO("dotnet",src,lvl,msg)
#define DOTNET_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("dotnet",src,lvl,msg)
#define DOTNET_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("dotnet",src,lvl,msg)
#define DOTNET_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("dotnet",src,lvl,msg)
#define DOTNET_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("dotnet",src,lvl,msg)
#define DOTNET_LOG_TRACE(src,lvl,msg) RX_TRACE("dotnet",src,lvl,msg)

#include "dotnet_wp/dotnet_wp_protocol.h"

#include "platform_api/rx_abi.h"


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

namespace api
{
rx_result dotnetBuildType(rx_item_type type, const string_type& module, const  rx_node_id& id, const  rx_node_id& parent
	, const string_type& name, const string_type& path, uint32_t version, uint32_t flags, const string_type& def, int ownType
	, const string_type& conns, const string_type& code_info);

void dotnetBuildRuntime(rx_item_type type, const string_type& module, const  rx_node_id& id, const  rx_node_id& parent
	, const string_type& name, const string_type& path, uint32_t version, uint32_t flags, const string_type& def, std::function<void(rx_result&&)> callback);
rx_result_struct dotnetDeleteType(rx_item_type type, const char* module, const rx_node_id_struct_t* id);

void dotnetDeleteRuntime(rx_item_type type, const char* module, const rx_node_id_struct_t* id, std::function<void(rx_result&&)> callback);

void dotnetWriteValue(
	rx_item_type itemType
	, uint32_t transId
	, void* instance
	, size_t idx
	, typed_value_type value
	, std::function<void(rx_result&&)> callback/*dotnetRuntimeResultDelegate*/);

}



} // namespace dotnet
} // namespace rx_internal


#endif
