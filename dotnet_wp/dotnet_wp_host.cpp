

/****************************************************************************
*
*  dotnet_wp\dotnet_wp_host.cpp
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


// dotnet_wp_host
#include "dotnet_wp/dotnet_wp_host.h"

#include "dotnet_wp/dotnet_wp.h"


namespace rx_internal {

namespace dotnet {

namespace {

init_hosting_entry_point_fn init_hosting_fp = nullptr;
deinit_hosting_entry_point_fn deinit_hosting_fp = nullptr;

}


namespace {

struct dotnet_loading_api_t g_loading_api =
{
    dotnetWriteLog,

    dotnetBuildType,
    dotnetDeleteType,

    dotnetBuildRuntime,
    dotnetDeleteRuntime,

    dotnetResultCallback,
    dotnetVoidCallback,

    dotnetWriteValue,

    dotnetExecuteDone,
    dotnetSourceWriteDone,
    dotnetSourceChange,

	dotnetFireEvent,

    dotnetExecuteValue

};
}

// Class rx_internal::dotnet::dotnet_wp_host 

dotnet_wp_host::dotnet_wp_host (const rx_dotnet_config_data_t& init_config)
      : init_fn_(nullptr),
        start_fn_(nullptr),
        deinit_fn_(nullptr),
        build_types_fn_(nullptr),
        bind_object_fn_(nullptr),
        unbind_object_fn_(nullptr),
        deinit_library_fn_(nullptr),
        values_callback_(nullptr),
        initial_values_(nullptr),
        execute_method_(nullptr),
        source_write_(nullptr),
        mapped_changed_(nullptr)
	, configuration_(init_config)
{
}


dotnet_wp_host::~dotnet_wp_host()
{
}



rx_result dotnet_wp_host::initialize (const string_type& app_dir, const string_array& host_files, const string_array& app_files, const string_array& lib_files, string_type& version)
{
    auto result = init_dotnet_host(configuration_.get_runtime_config(), get_full_path_from_relative(configuration_.get_app_path()));
	string_type app_file = rx_combine_paths(app_dir, "rx-platform.app.json");

    if (rx_file_exsist(app_file.c_str()) == 0 || app_files.size() == 0 || !result)
    {
        string_view_type msg = "No application files specified for.NET hosting parameters.";
        if (result)
        {
            return msg;
        }
        else
        {
            result.register_error(msg);
            return result;
        }
    }
    void* delegate = nullptr;

    string_type host_path;

	std::vector<bool> to_skip(host_files.size(), false);
	bool host_found = false;
    for (int i = 0; i < (int)host_files.size(); ++i)
    {
        if (host_files[i].find("rx-platform-dotnet-host.dll") != string_type::npos)
        {
            host_path = host_files[i];
            to_skip[i] = true;
            DOTNET_LOG_INFO("dotnet_host::initialize", 500, "Loading .NET host assembly from " + host_path);
            auto load_result = load_assembly(host_path);
            if (!load_result)
            {
                return load_result;
            }
            result = bind_functions();
            if (!result)
            {
                return result;
            }
            else
            {
                host_found = true;
            }
            DOTNET_LOG_INFO("dotnet_host::initialize", 500, ".NET host loaded from " + host_path);
            break;
        }
        else if (host_files[i].find("ENSACO.RxPlatform") != string_type::npos)
        {
            to_skip[i] = true;
        }
    }
    if (!host_found)
    {
        DOTNET_LOG_ERROR("dotnet_host::initialize", 500, "Unable to find hosting library." + host_path);
        return RX_INVALID_ARGUMENT;
    }
    for (int i = 0; i < (int)host_files.size(); ++i)
    {
        if (to_skip[i])
            continue;
        DOTNET_LOG_INFO("dotnet_host::initialize", 500, "Loading .NET assembly from " + host_files[i]);
        auto load_result = load_assembly(host_files[i]);
        if (!load_result)
        {
            DOTNET_LOG_WARNING("dotnet_host::initialize", 500, "Unable to load assembly:" + load_result.errors_line());
        }
    }
    std::vector<const char*> lib_paths;
    for (size_t i = 0; i < lib_files.size(); ++i)
    {
        lib_paths.push_back(lib_files[i].c_str());
    }
    std::vector<const char*> plugin_paths;
    for (size_t i = 0; i < app_files.size(); ++i)
    {
        plugin_paths.push_back(app_files[i].c_str());
    }
    uint32_t dotnet_stream_ver = 0;
    string_value_struct version_info = { 0, nullptr };

    int options = 0;
    if (configuration_.should_break())
    {
        options |= 0x1;
    }

	const char** libs = nullptr;
    if(lib_paths.size() > 0)
    {
        libs = &lib_paths[0];
	}

    rx_result ret = init_fn_(&g_loading_api
        , RX_CURRENT_SERIALIZE_VERSION
        , &dotnet_stream_ver
		, app_file.c_str()
		, libs, (uint32_t)lib_paths.size()
        , &plugin_paths[0], (uint32_t)plugin_paths.size()
        , &version_info
        , options);

    if (!ret)
    {
        return ret;
    }
    version = rx_c_str(&version_info);
    return true;
}

rx_result dotnet_wp_host::start ()
{
    if (start_fn_)
    {
        return start_fn_();
    }
    else
    {
        return "Start function is not bound in Native host.";
    }
}

void dotnet_wp_host::deinitialize (uint32_t trans_id, rxwp_void_callback callback)
{
    if (deinit_fn_)
    {
        {
            std::scoped_lock lock(callbacks_lock_);
            void_callbacks_.emplace(trans_id, std::move(callback));
        }
        deinit_fn_(trans_id);
    }
    else
    {
        callback();
    }
}

rx_result dotnet_wp_host::do_build ()
{
    DOTNET_LOG_INFO("dotnet_host::do_build", 0, "Building .NET library items");
    if (!build_types_fn_)
    {
        return "Build function is not bound in .NET host.";
    }
    rx_result result = build_types_fn_("");
    if (!result)
    {
        DOTNET_LOG_ERROR("dotnet_host::do_build", 0, ".NET library items build failed: " + result.errors_line());
    }
    else
    {
        DOTNET_LOG_INFO("dotnet_host::do_build", 0, ".NET library items built");
    }
    return result;
}

rx_result dotnet_wp_host::do_reload (uint32_t trans_id, const string_type& lib)
{
    rx_result result = build_types_fn_(lib.c_str());
    return result;
}

void dotnet_wp_host::do_unload (uint32_t trans_id, const string_type& lib, rxwp_result_callback callback)
{
    DOTNET_LOG_INFO("dotnet_host::do_reload", 0, "Re-loading .NET library " + lib + " items");
    if (!deinit_library_fn_)
    {
        callback("Re-initialize function is not bound in .NET host.");
    }
    else
    {
        {
            std::scoped_lock lock(callbacks_lock_);
            result_callbacks_.emplace(trans_id, std::move(callback));
        }
        deinit_library_fn_(lib.c_str(), trans_id);
    }
}

rx_result dotnet_wp_host::bind_functions ()
{
    void* delegate = nullptr;
    rx_result del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "InitPlatformHosting"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method InitPlatformHosting.");
        return del_result.errors();
    }
    init_fn_ = (init_hosting_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "StartPlatformHosting"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method StartPlatformHosting.");
        return del_result.errors();
    }
    start_fn_ = (start_hosting_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "BuildHostingTypes"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method BuildHostingTypes.");
        return del_result.errors();
    }
    build_types_fn_ = (build_library_hosting_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "BindObject"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method BindObject.");
        return del_result.errors();
    }
    bind_object_fn_ = (bind_object_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "UnbindObject"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method UnbindObject.");
        return del_result.errors();
    }
    unbind_object_fn_ = (unbind_object_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "DeinitLibrary"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method ReinitLibrary.");
        return del_result.errors();
    }
    deinit_library_fn_ = (deinit_library_hosting_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "DeinitPlatformHosting"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method DeinitPlatformHosting.");
        return del_result.errors();
    }
    deinit_fn_ = (deinit_hosting_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "RuntimeValueChanged"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method RuntimeValueChanged.");
        return del_result.errors();
    }
    values_callback_ = (value_changed_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "InitialRuntimeValues"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method InitialRuntimeValues.");
        return del_result.errors();
    }
    initial_values_ = (initial_values_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "ExecuteMethod"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method ExecuteMethod.");
        return del_result.errors();
    }
    execute_method_ = (execute_method_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "SourceWrite"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method SourceWrite.");
        return del_result.errors();
    }
    source_write_ = (source_write_entry_point_fn)delegate;

    del_result = get_function_pointer(
        "ENSACO.RxPlatform.Hosting.PlatformHostMain, rx-platform-dotnet-host"
        , "MappedChanged"
        , "", &delegate);
    if (!del_result)
    {
        del_result.register_error("Unable to get function pointer for method MappedChanged.");
        return del_result.errors();
    }
    mapped_changed_ = (mapped_changed_entry_point_fn)delegate;

    return true;
}

void dotnet_wp_host::received_result_callback (uint32_t transId, rx_result result)
{
}

void dotnet_wp_host::received_void_callback (uint32_t transId)
{
}

void dotnet_wp_host::received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, rxwp_result_callback callback)
{
}

void dotnet_wp_host::bind_runtime (rx_item_type type, const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr, const string_type& initial)
{
    bind_object_fn_(type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr, initial.c_str());
}

void dotnet_wp_host::unbind_runtime (rx_item_type type, const rx_node_id& id, const rx_node_id& type_id, void* ptr)
{
    unbind_object_fn_(type, id.c_ptr(), "", type_id.c_ptr(), ptr);
}

void dotnet_wp_host::runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose)
{
    if (values_callback_)
    {
        values_callback_(idx, val.move(), type, whose);
    }
}

void dotnet_wp_host::send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose)
{
    if (initial_values_)
    {
        std::vector<char*> name_ptrs;
        for (auto& n : names)
        {
            name_ptrs.push_back(const_cast<char*>(n.c_str()));
        }
        std::vector<typed_value_type> values;
        for (auto& v : val)
        {
            values.push_back(v.move());
        }
        initial_values_(values.size(), name_ptrs.data(), values.data(), type, whose);
    }
}

rx_result dotnet_wp_host::execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value)
{
    if (!execute_method_)
    {
        return "Execute method function is not bound in .NET host.";
    }
    return execute_method_(trans_id, whose, method.c_str(), value.c_str());
}

rx_result dotnet_wp_host::source_write (uint32_t trans_id, void* whose, typed_value_type val)
{
	return RX_NOT_IMPLEMENTED;
}

void dotnet_wp_host::received_source_change (void* inst, rx_value val)
{
}

void dotnet_wp_host::received_source_write_done (uint32_t trans_id, void* inst, rx_result result)
{
}

void dotnet_wp_host::received_fire_event (void* inst, string_type val, string_type queue, bool state, bool remove)
{
}


const deinit_hosting_entry_point_fn& dotnet_wp_host::get_deinit_fn () const
{
  return deinit_fn_;
}


// Class rx_internal::dotnet::dotnet_configuration 

dotnet_configuration::dotnet_configuration (const rx_dotnet_config_data_t& init_config)
      : runtime_config_(init_config)
{
}


dotnet_configuration::~dotnet_configuration()
{
}



string_type dotnet_configuration::get_runtime_config () const
{
    RX_ASSERT(!runtime_config_.config_tfm.empty() && !runtime_config_.config_framework.empty() && !runtime_config_.config_framework_version.empty());
    if (runtime_config_.config_tfm.empty() || runtime_config_.config_framework.empty() || runtime_config_.config_framework_version.empty())
        return "";
    std::ostringstream config;
    config
        << "{"
        << "\"runtimeOptions\": {"
        << "\"tfm\": \"" << runtime_config_.config_tfm << "\","
        << "\"framework\": {"
        << "\"name\": \"" << runtime_config_.config_framework << "\","
        << "\"version\": \"" << runtime_config_.config_framework_version << "\""
        << "},"
        << "\"configProperties\": {"
        << "\"System.GC.Server\": " << (runtime_config_.server_gc ? "true" : "false") << ","
        << "\"System.GC.Concurrent\": " << (runtime_config_.concurrent_gc ? "true" : "false");
    if (runtime_config_.max_threads > 0)
        config
        << ", \"System.Threading.ThreadPool.MaxThreads\": " << runtime_config_.max_threads;
    if (runtime_config_.min_threads > 0)
        config
        << ", \"System.Threading.ThreadPool.MinThreads\": " << runtime_config_.min_threads;
    config
        << "}"
        << "}"
        << "}";
    return config.str();
}

bool dotnet_configuration::should_break () const
{
    return runtime_config_.launch_debugger;
}

string_type dotnet_configuration::get_app_path ()
{
	return runtime_config_.lib_path;
}


} // namespace dotnet
} // namespace rx_internal

