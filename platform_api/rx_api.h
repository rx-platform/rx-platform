

/****************************************************************************
*
*  platform_api\rx_api.h
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


#ifndef rx_api_h
#define rx_api_h 1



// rx_ptr
#include "lib/rx_ptr.h"


#include "version/rx_version.h"

/////////////////////////////////////////////////////////////
// logging macros for plugins
#define RX_PLUGIN_LOG_INFO(src,lvl,msg) RX_LOG_INFO(RX_PLUGIN_ID,src,lvl,msg)
#define RX_PLUGIN_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING(RX_PLUGIN_ID,src,lvl,msg)
#define RX_PLUGIN_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR(RX_PLUGIN_ID,src,lvl,msg)
#define RX_PLUGIN_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL(RX_PLUGIN_ID,src,lvl,msg)
#define RX_PLUGIN_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG(RX_PLUGIN_ID,src,lvl,msg)
#define RX_PLUGIN_LOG_TRACE(src,lvl,msg) RX_TRACE(RX_PLUGIN_ID,src,lvl,msg)

#include "rx_abi.h"


extern uintptr_t g_plugin;

////////////////////////////////////////////////////////////////////////
// this macro bellow if ugly but hides "C" ABI details from implementer
// I think that is more important!!!
////////////////////////////////////////////////////////////////////////
#define RX_DECLARE_PLUGIN(class_name) \
std::unique_ptr<class_name> _g_plugin_obj_;\
extern "C" {\
RX_PLUGIN_API rx_result_struct rxBindPlugin(const platform_api* api, uint32_t host_stream_version, uint32_t* plugin_stream_version, uintptr_t* plugin)\
{\
	_g_plugin_obj_ = std::make_unique<class_name>();\
    g_plugin = (uintptr_t)_g_plugin_obj_.get();\
    *plugin = g_plugin;\
	return rx_platform_api::rx_bind_plugin(api, host_stream_version, plugin_stream_version);\
}\
RX_PLUGIN_API void rxGetPluginInfo(string_value_struct* plugin_ver, string_value_struct* lib_ver, string_value_struct* sys_ver, string_value_struct* comp_ver)\
{\
	rx_platform_api::rx_get_plugin_info(RX_PLUGIN_NAME, RX_PLUGIN_MAJOR_VERSION, RX_PLUGIN_MINOR_VERSION, RX_PLUGIN_BUILD_NUMBER, plugin_ver, lib_ver, sys_ver, comp_ver);\
}\
RX_PLUGIN_API void rxGetPluginName(string_value_struct* name)\
{\
	rx_init_string_value_struct(name, RX_PLUGIN_ID, -1);\
}\
RX_PLUGIN_API rx_result_struct rxInitPlugin()\
{\
	return rx_platform_api::rx_init_plugin(_g_plugin_obj_.get());\
}\
RX_PLUGIN_API rx_result_struct rxDeinitPlugin()\
{\
	return rx_platform_api::rx_deinit_plugin(_g_plugin_obj_.get());\
}\
RX_PLUGIN_API rx_result_struct rxBuildPlugin(const char* root)\
{\
	return rx_platform_api::rx_build_plugin(_g_plugin_obj_.get(), root);\
}\
}\


#define DECLARE_PLUGIN_CODE_INFO(v, m, b, d) DECLARE_CODE_INFO(RX_PLUGIN_ID, v, m, b, d)
////////////////////////////////////////////////////////////////////////

using namespace rx;


namespace rx_platform_api {
enum class rx_item_type : uint8_t
{
    rx_directory = 0,
    rx_application = 1,
    rx_application_type = 2,
    rx_domain = 3,
    rx_domain_type = 4,
    rx_object = 5,
    rx_object_type = 6,
    rx_port = 7,
    rx_port_type = 8,
    rx_struct_type = 9,
    rx_variable_type = 10,
    rx_source_type = 11,
    rx_filter_type = 12,
    rx_event_type = 13,
    rx_mapper_type = 14,
    rx_relation_type = 15,
    rx_program_type = 16,
    rx_method_type = 17,
    rx_data_type = 18,
    rx_display_type = 19,

    rx_first_invalid = 20,

    rx_test_case_type = 0xfe,
    rx_invalid_type = 0xff
};





class rx_platform_plugin 
{

  public:

      virtual rx_result init_plugin () = 0;

      virtual rx_result deinit_plugin () = 0;

      virtual rx_result build_plugin () = 0;

      rx_platform_plugin() = default;
      rx_platform_plugin(const rx_platform_plugin&) = delete;
      rx_platform_plugin(rx_platform_plugin&&) = delete;
      rx_platform_plugin& operator=(const rx_platform_plugin&) = delete;
      rx_platform_plugin& operator=(rx_platform_plugin&&) = delete;
      virtual ~rx_platform_plugin() = default;
  protected:

  private:


};

rx_result_struct rx_bind_plugin(const platform_api* api, uint32_t host_stream_version, uint32_t* plugin_stream_version);
void rx_get_plugin_info(const char* name, int major, int minor, int build, string_value_struct* plugin_ver, string_value_struct* lib_ver, string_value_struct* sys_ver, string_value_struct* comp_ver);
rx_result_struct rx_init_plugin(rx_platform_plugin* plugin);
rx_result_struct rx_deinit_plugin(rx_platform_plugin* plugin);
rx_result_struct rx_build_plugin(rx_platform_plugin* plugin, const char* root);





class rx_shared_reference : public rx::pointers::reference_object  
{

  public:

  protected:

  private:


};


} // namespace rx_platform_api



#endif
