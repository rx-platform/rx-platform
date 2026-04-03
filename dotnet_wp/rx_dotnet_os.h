

/****************************************************************************
*
*  dotnet\rx_dotnet_os.h
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


#ifndef rx_dotnet_os_h
#define rx_dotnet_os_h 1


#include "dotnet_wp/dotnet_wp_protocol.h"
#include "dotnet_wp_log_macros.h"



/////////////////////////////////////////////////////////////
// logging macros for http library
#define DOTNET_LOG_INFO(src,lvl,msg) RX_LOG_INFO("dotnet",src,lvl,msg)
#define DOTNET_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("dotnet",src,lvl,msg)
#define DOTNET_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("dotnet",src,lvl,msg)
#define DOTNET_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("dotnet",src,lvl,msg)
#define DOTNET_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("dotnet",src,lvl,msg)
#define DOTNET_LOG_TRACE(src,lvl,msg) RX_TRACE("dotnet",src,lvl,msg)


namespace rx_internal
{
namespace dotnet{

enum class log_event_type
{
	debug = 0,
	trace = 1,
	info = 2,
	warning = 3,
	error = 4,
	critical = 5
};
    
rx_result load_hostfxr(rx_dotnet_config_data_t& init_config);
rx_result init_dotnet_host(string_type runtime_config);
string_array get_dotnet_runtimes();
string_type get_hosted_dotnet_version();
rx_result load_assembly(const string_type& path);
rx_result get_function_pointer(const string_type& type_name, const string_type& method_name, const string_type& delegate_name, void** del);


} // namespace dotnet
} // namespace rx_internal





#endif
