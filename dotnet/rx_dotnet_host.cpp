

/****************************************************************************
*
*  dotnet\rx_dotnet_host.cpp
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


#include "pch.h"

#include "rx_dotnet_os.h"

// rx_dotnet_host
#include "dotnet/rx_dotnet_host.h"



namespace rx_internal {

namespace dotnet {
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
	dotnetSourceChange

};
}

// Class rx_internal::dotnet::dotnet_configuration 

dotnet_configuration::dotnet_configuration (const configuration_data_t& data, const rx_dotnet_config_data_t& init_config)
      : config_(data.dotnet),
        runtime_config_(init_config)
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
		<< "\"System.GC.Server\": " << (config_.server_gc ? "true" : "false") << ","
		<< "\"System.GC.Concurrent\": " << (config_.concurrent_gc ? "true" : "false");
	if (config_.max_threads > 0)
		config
			<< ", \"System.Threading.ThreadPool.MaxThreads\": " << config_.max_threads;
    if (config_.min_threads > 0)
        config
            << ", \"System.Threading.ThreadPool.MinThreads\": " << config_.min_threads;
	config
        << "}"
		<< "}"
        << "}";
	return config.str();
}


// Class rx_internal::dotnet::dotnet_host 

dotnet_host::dotnet_host (const configuration_data_t& data, const rx_dotnet_config_data_t& init_config)
      : configuration_(data, init_config),
        init_fn_(nullptr),
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
{
}


dotnet_host::~dotnet_host()
{
}



rx_result dotnet_host::initialize (hosting::rx_platform_host* host, const string_array& host_files, const string_array& app_files, const string_array& lib_files, string_type& version)
{

	auto result = init_dotnet_host(configuration_.get_runtime_config());

    if(app_files.size() == 0 || !result)
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

	int skip_index = -1;
	for (int i = 0; i < (int)host_files.size(); ++i)
    {
        if (host_files[i].find("rx-platform-dotnet-host.dll") != string_type::npos)
        {
            host_path = host_files[i];
			skip_index = i;
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
            DOTNET_LOG_INFO("dotnet_host::initialize", 500, ".NET host loaded from " + host_path);
            break;
        }
	}
    if (skip_index < 0)
    {
        DOTNET_LOG_ERROR("dotnet_host::initialize", 500, "Unable to find hosting library." + host_path);
        return RX_INVALID_ARGUMENT;
    }
    for (int i = 0; i < (int)host_files.size(); ++i)
    {
        if (i == skip_index)
            continue;
        DOTNET_LOG_INFO("dotnet_host::initialize", 500, "Loading .NET assembly from " + host_files[i]);
        auto load_result = load_assembly(host_files[i]);
        if (!load_result)
        {
            DOTNET_LOG_ERROR("dotnet_host::initialize", 500, "Unable to load assembly:" + load_result.errors_line());
            return load_result;
        }
    }
    for (int i = 0; i < (int)lib_files.size(); ++i)
    {
        if (i == skip_index)
            continue;
        DOTNET_LOG_INFO("dotnet_host::initialize", 500, "Loading .NET assembly from " + lib_files[i]);
        auto load_result = load_assembly(lib_files[i]);
        if (!load_result)
        {
            DOTNET_LOG_ERROR("dotnet_host::initialize", 500, "Unable to load assembly:" + load_result.errors_line());
            return load_result;
        }
    }

    std::vector<const char*> plugin_paths;
    for (size_t i = 0; i < app_files.size(); ++i)
    {
        plugin_paths.push_back(app_files[i].c_str());
    }
    uint32_t dotnet_stream_ver = 0;
	string_value_struct version_info = { 0, nullptr };


    rx_result ret = init_fn_(&g_loading_api, RX_CURRENT_SERIALIZE_VERSION, &dotnet_stream_ver, &plugin_paths[0], (uint32_t)plugin_paths.size(), &version_info);
    if (!ret)
    {
		return ret;
	}
	version = rx_c_str(&version_info);
	DOTNET_LOG_INFO("dotnet_host::initialize", 0, "Native host initialized, version: "s + rx_c_str(&version_info));
    return true;
}

rx_result dotnet_host::start ()
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

void dotnet_host::deinitialize (rx_void_callback callback)
{
    if (deinit_fn_)
    {
        auto trans = rx_get_new_transaction_id();
        {
            std::scoped_lock lock(callbacks_lock_);
            void_callbacks_.emplace(trans, std::move(callback));
        }
        deinit_fn_(trans);
    }
    else
    {
        callback();
    }
}

rx_result dotnet_host::do_build (configuration_data_t& config, startup_create_data_type& data)
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

rx_result dotnet_host::do_reload (const string_type& lib)
{
    rx_result result = build_types_fn_(lib.c_str());
    return result;
}

void dotnet_host::do_unload (const string_type& lib, rx_result_callback callback)
{
    DOTNET_LOG_INFO("dotnet_host::do_reload", 0, "Re-loading .NET library " + lib + " items");
    if (!deinit_library_fn_)
    {
        callback("Re-initialize function is not bound in .NET host.");
    }
    else
    {
        auto trans = rx_get_new_transaction_id();
        {
            std::scoped_lock lock(callbacks_lock_);
            result_callbacks_.emplace(trans, std::move(callback));
        }
        deinit_library_fn_(lib.c_str(), trans);
    }
}

rx_result dotnet_host::bind_functions ()
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
    unbind_object_fn_ = (bind_object_entry_point_fn)delegate;

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

void dotnet_host::received_result_callback (uint32_t transId, rx_result result)
{
    std::scoped_lock lock(callbacks_lock_);
    auto it = result_callbacks_.find(transId);
    if (it != result_callbacks_.end())
    {
        auto cb = std::move(it->second);
        result_callbacks_.erase(it);
        cb(std::move(result));
    }
}

void dotnet_host::received_void_callback (uint32_t transId)
{
    std::scoped_lock lock(callbacks_lock_);
    auto it = void_callbacks_.find(transId);
    if (it != void_callbacks_.end())
    {
        auto cb = std::move(it->second);
        void_callbacks_.erase(it);
        cb();
    }
}

void dotnet_host::received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, rx_result_callback callback)
{
    switch (type)
    {
        case rx_item_type::rx_object:
            {
                auto it = runtime_objects_.find(inst);
                if (it != runtime_objects_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
        case rx_item_type::rx_struct_type:
            {
                auto it = runtime_structs_.find(inst);
                if (it != runtime_structs_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
		case rx_item_type::rx_source_type:
            {
                auto it = runtime_sources_.find(inst);
                if (it != runtime_sources_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
			break;
        case rx_item_type::rx_mapper_type:
            {
                auto it = runtime_mappers_.find(inst);
                if (it != runtime_mappers_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
		case rx_item_type::rx_event_type:
            {
                auto it = runtime_events_.find(inst);
                if (it != runtime_events_.end())
                {
                    it->second->received_write_value(transId, inst, idx, std::move(val), std::move(callback));
                }
            }
            break;
        default:
            callback(rx_result("Type invalid to support write!").move());
			break;
    }
}

void dotnet_host::bind_runtime (const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_object_runtime> ref)
{
    runtime_objects_.emplace(ptr, std::move(ref));
	bind_object_fn_(rx_item_type::rx_object, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
}

void dotnet_host::unbind_runtime (const rx_node_id& id, const rx_node_id& type_id, void* ptr)
{
    unbind_object_fn_(rx_item_type::rx_object, id.c_ptr(), "", type_id.c_ptr(), ptr);
	runtime_objects_.erase(ptr);
}

void dotnet_host::runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose)
{
    if(values_callback_)
    {
        values_callback_(idx, val.move(), type, whose);
	}
}

void dotnet_host::send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose)
{
    if(initial_values_)
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

rx_result dotnet_host::execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value)
{
    if(!execute_method_)
    {
        return "Execute method function is not bound in .NET host.";
	}
	return execute_method_(trans_id, whose, method.c_str(), value.c_str());
}

void dotnet_host::received_execute_done (uint32_t transId, void* inst, const string_type& val, rx_result result)
{
    auto it = runtime_objects_.find(inst);
    if (it != runtime_objects_.end())
    {
        it->second->received_execute_done(transId, val, std::move(result));
    }
}

void dotnet_host::bind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_struct> ref)
{
    runtime_structs_.emplace(ptr, std::move(ref));
    bind_object_fn_(rx_item_type::rx_struct_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
}

void dotnet_host::unbind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    unbind_object_fn_(rx_item_type::rx_struct_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
    runtime_structs_.erase(ptr);
}

void dotnet_host::bind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_source> ref)
{
    runtime_sources_.emplace(ptr, std::move(ref));
    bind_object_fn_(rx_item_type::rx_source_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
}

void dotnet_host::unbind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    unbind_object_fn_(rx_item_type::rx_source_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
    runtime_sources_.erase(ptr);
}

void dotnet_host::bind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_mapper> ref)
{
    runtime_mappers_.emplace(ptr, std::move(ref));
    bind_object_fn_(rx_item_type::rx_mapper_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
}

void dotnet_host::unbind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    unbind_object_fn_(rx_item_type::rx_mapper_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
    runtime_mappers_.erase(ptr);
}

void dotnet_host::bind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_event> ref)
{
    runtime_events_.emplace(ptr, std::move(ref));
    bind_object_fn_(rx_item_type::rx_event_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
}

void dotnet_host::unbind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr)
{
    runtime_events_.erase(ptr);
    unbind_object_fn_(rx_item_type::rx_event_type, id.c_ptr(), path.c_str(), type_id.c_ptr(), ptr);
}

rx_result dotnet_host::source_write (uint32_t trans_id, void* whose, typed_value_type val)
{
	source_write_(trans_id, std::move(val), whose);
	return true;
}

void dotnet_host::received_source_change (void* inst, rx_value val)
{
    auto it = runtime_sources_.find(inst);
    if (it != runtime_sources_.end())
    {
        it->second->source_changed(std::move(val));
    }
}

void dotnet_host::received_source_write_done (uint32_t trans_id, void* inst, rx_result result)
{
    auto it = runtime_sources_.find(inst);
    if (it != runtime_sources_.end())
    {
        it->second->source_result(std::move(result), trans_id);
    }
}


const deinit_hosting_entry_point_fn& dotnet_host::get_deinit_fn () const
{
  return deinit_fn_;
}


} // namespace dotnet
} // namespace rx_internal

