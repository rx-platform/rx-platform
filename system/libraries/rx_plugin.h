

/****************************************************************************
*
*  system\libraries\rx_plugin.h
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


#ifndef rx_plugin_h
#define rx_plugin_h 1





/////////////////////////////////////////////////////////////
// logging macros for console library
#define PLUGIN_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Plugins",src,lvl,msg)
#define PLUGIN_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Plugins",src,lvl,msg)
#define PLUGIN_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Plugins",src,lvl,msg)
#define PLUGIN_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Plugins",src,lvl,msg)
#define PLUGIN_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Plugins",src,lvl,msg)
#define PLUGIN_LOG_TRACE(src,lvl,msg) RX_TRACE("Plugins",src,lvl,msg)

#include "platform_api/rx_abi.h"



namespace rx_platform {

namespace library {

struct rx_plugin_info
{
    string_type plugin_version;
    string_type comp_version;
    string_type lib_version;
    string_type platform_version;

};





class plugin_builder 
{

  public:

      string_type plugin_root;


  protected:

  private:


};







class rx_plugin_base 
{

  public:
      rx_plugin_base();

      virtual ~rx_plugin_base();


      virtual rx_result bind_plugin () = 0;

      virtual rx_plugin_info get_plugin_info () const = 0;

      virtual rx_result init_plugin () = 0;

      virtual rx_result deinit_plugin () = 0;

      virtual rx_result build_plugin (plugin_builder& builder) = 0;

      virtual string_type get_plugin_name () const = 0;


  protected:

  private:


};


class rx_dynamic_plugin;
typedef std::map<uintptr_t, rx_dynamic_plugin*> registered_plugins_type;




class rx_dynamic_plugin : public rx_plugin_base  
{

  public:
      rx_dynamic_plugin (const string_type& lib_path);

      ~rx_dynamic_plugin();


      rx_result bind_plugin ();

      rx_result load_plugin ();

      rx_plugin_info get_plugin_info () const;

      rx_result init_plugin ();

      rx_result deinit_plugin ();

      rx_result build_plugin (plugin_builder& builder);

      string_type get_plugin_name () const;

      static rx_dynamic_plugin* get_registered_plugin (uintptr_t id);

      uint32_t get_stream_version () const;

      static uint32_t get_stream_version (uintptr_t id);


  protected:

  private:


      rx_module_handle_t module_;

      rxBindPlugin_t prxBindPlugin_;

      rxBindPlugin2_t prxBindPlugin2_;

      rxGetPluginInfo_t prxGetPluginInfo_;

      rxGetPluginName_t prxGetPluginName_;

      rxInitPlugin_t prxInitPlugin_;

      rxDeinitPlugin_t prxDeinitPlugin_;

      rxBuildPlugin_t prxBuildPlugin_;

      string_type lib_path_;

      static registered_plugins_type registered_plugins_;

      uint32_t stream_version_;


};


} // namespace library
} // namespace rx_platform

namespace rx_platform
{
typedef library::rx_plugin_base* rx_plugin_ptr;
}


#endif
