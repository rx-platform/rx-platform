

/****************************************************************************
*
*  system\libraries\rx_plugin.cpp
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


// rx_plugin
#include "system/libraries/rx_plugin.h"

#include "model/rx_meta_internals.h"
#include "api/rx_platform_api.h"


namespace rx_platform {

namespace library {

// Class rx_platform::library::rx_plugin_base 

rx_plugin_base::rx_plugin_base()
{
}


rx_plugin_base::~rx_plugin_base()
{
}



// Class rx_platform::library::plugin_builder 


// Class rx_platform::library::rx_dynamic_plugin 

registered_plugins_type rx_dynamic_plugin::registered_plugins_;

rx_dynamic_plugin::rx_dynamic_plugin (const string_type& lib_path)
      : module_(nullptr),
        prxBindPlugin_(nullptr),
        prxBindPlugin2_(nullptr),
        prxBindPlugin3_(nullptr),
        prxBindPlugin4_(nullptr),
        prxBindPlugin5_(nullptr),
        prxBindPlugin6_(nullptr),
        prxGetPluginInfo_(nullptr),
        prxGetPluginInfo2_(nullptr),
        prxGetPluginName_(nullptr),
        prxInitPlugin_(nullptr),
        prxDeinitPlugin_(nullptr),
        prxBuildPlugin_(nullptr),
        lib_path_(lib_path),
        stream_version_(RX_CURRENT_SERIALIZE_VERSION)
{
}


rx_dynamic_plugin::~rx_dynamic_plugin()
{
    /*if (module_)
        rx_unload_library(module_);*/
}



rx_result rx_dynamic_plugin::bind_plugin ()
{
    if (prxBindPlugin6_ || prxBindPlugin5_ || prxBindPlugin4_ || prxBindPlugin3_ || prxBindPlugin2_ || prxBindPlugin_)
    {
        uintptr_t plugin_id;
        uint32_t plugin_version;
        uint32_t bind_version = 0;
        const char** dependencies = NULL;
        rx_result ret;
        if (prxBindPlugin6_ != nullptr)
        {
            bind_version = 6;
            ret = prxBindPlugin6_(rx_platform::api::get_plugins_dynamic_api6(), RX_CURRENT_SERIALIZE_VERSION, &plugin_version, &plugin_id, &dependencies);
        }
        else if (prxBindPlugin5_ != nullptr)
        {
            bind_version = 5;
            ret = prxBindPlugin5_(rx_platform::api::get_plugins_dynamic_api5(), RX_CURRENT_SERIALIZE_VERSION, &plugin_version, &plugin_id, &dependencies);
        }
        else if (prxBindPlugin4_ != nullptr)
        {
            bind_version = 4;
            ret = prxBindPlugin4_(rx_platform::api::get_plugins_dynamic_api4(), RX_CURRENT_SERIALIZE_VERSION, &plugin_version, &plugin_id, &dependencies);
        }
        else if (prxBindPlugin3_ != nullptr)
        {
            bind_version = 3;
            ret = prxBindPlugin3_(rx_platform::api::get_plugins_dynamic_api3(), RX_CURRENT_SERIALIZE_VERSION, &plugin_version, &plugin_id);
        }
        else if (prxBindPlugin2_ != nullptr)
        {
            bind_version = 2;
            ret = prxBindPlugin2_(rx_platform::api::get_plugins_dynamic_api2(), RX_CURRENT_SERIALIZE_VERSION, &plugin_version, &plugin_id);
        }
        else
        {
            bind_version = 1;
            ret = prxBindPlugin_(rx_platform::api::get_plugins_dynamic_api(), RX_CURRENT_SERIALIZE_VERSION, &plugin_version, &plugin_id);
        }
        if (ret)
        {
            if (dependencies != NULL)
            {
                int idx = 0;
                while (dependencies[idx] != nullptr)
                {
                    dependencies_.push_back(dependencies[idx]);
                    idx++;
                }
            }
            registered_plugins_.emplace(plugin_id, this);
            stream_version_ = plugin_version;
            prxGetPluginInfo_ = (rxGetPluginInfo_t)rx_get_func_address(module_, "rxGetPluginInfo");
            prxGetPluginInfo2_ = (rxGetPluginInfo2_t)rx_get_func_address(module_, "rxGetPluginInfo2");
            prxGetPluginName_ = (rxGetPluginName_t)rx_get_func_address(module_, "rxGetPluginName");
            prxInitPlugin_ = (rxInitPlugin_t)rx_get_func_address(module_, "rxInitPlugin");
            prxDeinitPlugin_ = (rxDeinitPlugin_t)rx_get_func_address(module_, "rxDeinitPlugin");
            prxBuildPlugin_ = (rxBuildPlugin_t)rx_get_func_address(module_, "rxBuildPlugin");

            if (prxGetPluginName_)
            {
                rx::rx_string_wrapper wrap;
                prxGetPluginName_(&wrap);
                std::ostringstream ss;
                ss << "Plugin "
                    << wrap.c_str()
                    << " binded with version "
                    << bind_version
                    << ".";
                PLUGIN_LOG_INFO("rx_dynamic_plugin", 900, ss.str());
            }
        }

        return ret;
    }
    else
    {
        RX_ASSERT(false);
        return RX_INTERNAL_ERROR;
    }
}

rx_result rx_dynamic_plugin::load_plugin ()
{
    module_ = rx_load_library(lib_path_.c_str());
    if (module_)
    {
        prxBindPlugin6_ = (rxBindPlugin6_t)rx_get_func_address(module_, "rxBindPlugin6");
        if (prxBindPlugin6_)
        {
            return true;
        }
        prxBindPlugin5_ = (rxBindPlugin5_t)rx_get_func_address(module_, "rxBindPlugin5");
        if (prxBindPlugin5_)
        {
            return true;
        }
        prxBindPlugin4_ = (rxBindPlugin4_t)rx_get_func_address(module_, "rxBindPlugin4");
        if (prxBindPlugin4_)
        {
            return true;
        }
        prxBindPlugin3_ = (rxBindPlugin3_t)rx_get_func_address(module_, "rxBindPlugin3");
        if (prxBindPlugin3_)
        {
            return true;
        }
        prxBindPlugin2_ = (rxBindPlugin2_t)rx_get_func_address(module_, "rxBindPlugin2");
        if (prxBindPlugin2_)
        {
            return true;
        }
        prxBindPlugin_ = (rxBindPlugin_t)rx_get_func_address(module_, "rxBindPlugin");
        if (prxBindPlugin_)
        {
            return true;
        }
        else
        {
            return "Unable to bind library "s + lib_path_;
        }
    }
    else
    {
        return "Unable to load library "s + lib_path_;
    }
}

rx_plugin_info rx_dynamic_plugin::get_plugin_info () const
{
    rx_plugin_info ret;
    if (prxGetPluginInfo2_)
    {
        string_value_struct plugin_ver, lib_ver, sys_ver, comp_ver, abi_ver, common_ver;
        (prxGetPluginInfo2_)(&plugin_ver, &lib_ver, &sys_ver, &comp_ver, &abi_ver, &common_ver);
        ret.plugin_version = rx_c_str(&plugin_ver);
        rx_destory_string_value_struct(&plugin_ver);
        ret.lib_version = rx_c_str(&lib_ver);
        rx_destory_string_value_struct(&lib_ver);
        ret.platform_version = rx_c_str(&sys_ver);
        rx_destory_string_value_struct(&sys_ver);
        ret.comp_version = rx_c_str(&comp_ver);
        rx_destory_string_value_struct(&comp_ver);
        ret.abi_version = rx_c_str(&abi_ver);
        rx_destory_string_value_struct(&abi_ver);
        ret.common_version = rx_c_str(&common_ver);
        rx_destory_string_value_struct(&common_ver);
    }
    if (prxGetPluginInfo_)
    {
        string_value_struct plugin_ver, lib_ver, sys_ver, comp_ver;
        (prxGetPluginInfo_)(&plugin_ver, &lib_ver, &sys_ver, &comp_ver);
        ret.plugin_version = rx_c_str(&plugin_ver);
        rx_destory_string_value_struct(&plugin_ver);
        ret.lib_version = rx_c_str(&lib_ver);
        rx_destory_string_value_struct(&lib_ver);
        ret.platform_version = rx_c_str(&sys_ver);
        rx_destory_string_value_struct(&sys_ver);
        ret.comp_version = rx_c_str(&comp_ver);
        rx_destory_string_value_struct(&comp_ver);
    }
    return ret;
}

rx_result rx_dynamic_plugin::init_plugin ()
{
    if (prxBindPlugin_ || prxBindPlugin2_ || prxBindPlugin3_ || prxBindPlugin4_ || prxBindPlugin5_ || prxBindPlugin6_)
    {
        if (prxInitPlugin_)
        {
            return prxInitPlugin_();
        }
        else
        {
            return "Jebi gaaaa!!!";
        }
    }
    else
    {
        return "Plugin not binded";
    }
}

rx_result rx_dynamic_plugin::deinit_plugin ()
{
    if (prxDeinitPlugin_)
    {
        return prxDeinitPlugin_();
    }
    else
    {
        return "Jebi gaaaa!!!";
    }
}

rx_result rx_dynamic_plugin::build_plugin (plugin_builder& builder)
{
    if (prxBuildPlugin_)
    {
        return prxBuildPlugin_(builder.plugin_root.c_str());
    }
    else
    {
        return "Jebi gaaaa!!!";
    }
}

string_type rx_dynamic_plugin::get_plugin_name () const
{
    string_type ret;
    if (prxGetPluginInfo_)
    {
        string_value_struct name;
        (prxGetPluginName_)(&name);
        ret = rx_c_str(&name);
        rx_destory_string_value_struct(&name);
    }
    return ret;
}

rx_dynamic_plugin* rx_dynamic_plugin::get_registered_plugin (uintptr_t id)
{
    auto it = registered_plugins_.find(id);
    if (it != registered_plugins_.end())
        return it->second;
    else
        return nullptr;
}

uint32_t rx_dynamic_plugin::get_stream_version () const
{
    return stream_version_;
}

uint32_t rx_dynamic_plugin::get_stream_version (uintptr_t id)
{
    auto it = registered_plugins_.find(id);
    if (it != registered_plugins_.end())
        return it->second->stream_version_;
    else
        return RX_CURRENT_SERIALIZE_VERSION;
}

string_array rx_dynamic_plugin::get_dependencies () const
{
    return dependencies_;
}


} // namespace library
} // namespace rx_platform

