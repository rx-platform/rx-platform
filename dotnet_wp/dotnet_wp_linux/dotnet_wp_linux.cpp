

/****************************************************************************
*
*  dotnet_wp\dotnet_wp_linux\dotnet_wp_linux.cpp
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

#include "nethost.h"
#include "hostfxr.h"
#include "coreclr_delegates.h"
#include "system/server/rx_server.h"
#include "dotnet_wp/rx_dotnet_os.h"
#include "system/server/rx_file_helpers.h"



namespace
{

hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
hostfxr_get_dotnet_environment_info_fn get_dotnet_info_fptr = nullptr;
hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
hostfxr_close_fn close_fptr = nullptr;
void* hostfxr_lib = nullptr;

hostfxr_handle host_cxt = nullptr;

get_function_pointer_fn pget_function_pointer = nullptr;
load_assembly_fn pload_assembly = nullptr;

std::vector<string_type> dotnet_runtime_versions;

string_type g_active_version;
string_type g_managed_version;



uint64_t req_version = 0;
uint64_t req_major = 0;


}

namespace rx_internal
{
namespace dotnet {

namespace
{
rx_dotnet_config_data_t _init_config;
}
rx_result load_hostfxr(rx_dotnet_config_data_t& init_config)
{

    // Pre-allocate a large buffer for the path to hostfxr
    char buffer[MAX_PATH];
    size_t buffer_size = sizeof(buffer) / sizeof(char);
    int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
    if (rc != 0)
        return "Unable to locate hostfxr.dll module";

    // Load hostfxr and get desired exports
    hostfxr_lib = dlopen(buffer, RTLD_NOW);
    if (!hostfxr_lib)
        return "Unable to load hostfxr.so library";

    _init_config = init_config;

    if (init_config.requested_version != "latest")
    {
        auto idx1 = init_config.requested_version.find(".");
        if (idx1 != string_type::npos)
        {
            std::from_chars(init_config.requested_version.data(), init_config.requested_version.data() + idx1, req_version);
            std::from_chars(init_config.requested_version.data() + idx1 + 1, init_config.requested_version.data() + init_config.requested_version.size(), req_major);
        }
        else
        {
            std::from_chars(init_config.requested_version.data(), init_config.requested_version.data() + init_config.requested_version.size(), req_version);
        }
        if (req_version == 0)
            return "Invalid .NET version configured";
    }

    init_fptr = (hostfxr_initialize_for_runtime_config_fn)dlsym(hostfxr_lib, "hostfxr_initialize_for_runtime_config");
    get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)dlsym(hostfxr_lib, "hostfxr_get_runtime_delegate");
    get_dotnet_info_fptr = (hostfxr_get_dotnet_environment_info_fn)dlsym(hostfxr_lib, "hostfxr_get_dotnet_environment_info");
    close_fptr = (hostfxr_close_fn)dlsym(hostfxr_lib, "hostfxr_close");

    if (get_dotnet_info_fptr)
    {
        hostfxr_get_dotnet_environment_info_result_fn callback =
            [](const hostfxr_dotnet_environment_info* info, void* context)
            {
                uint64_t ver_max = (req_version << 32) | (req_major << 16);
                if (info && info->framework_count && info->frameworks)
                {
                    dotnet_runtime_versions.clear();
                    for (size_t i = 0; i < info->framework_count; i++)
                    {
                        string_type ver = info->frameworks[i].version;
                        string_type name = info->frameworks[i].name;
                        string_type path = info->frameworks[i].path;

                        if (!name.empty() && string_type::npos != name.find("Microsoft.NETCore.App"))
                        {
                            uint64_t this_version = 0;
                            uint64_t version = 0;
                            uint64_t major = 0;
                            uint64_t minor = 0;
                            auto idx1 = ver.find(".");
                            auto idx2 = ver.find(".", idx1 + 1);
                            if (idx1 != string_type::npos && idx2 != string_type::npos)
                            {
                                std::from_chars(ver.data(), ver.data() + idx1, version);
                                std::from_chars(ver.data() + idx1 + 1, ver.data() + idx2, major);
                                std::from_chars(ver.data() + idx2 + 1, ver.data() + ver.size(), minor);

                                this_version = (version << 32) | (major << 16) | minor;
                            }
                            if (req_version > 0)
                            {
                                if (version == req_version && major == req_major)
                                {
                                    if (this_version >= ver_max)
                                    {
                                        ver_max = this_version;
                                        _init_config.config_tfm = rx_create_string("net", ver.substr(0, idx2));
                                        _init_config.config_framework = name;
                                        _init_config.config_framework_version = ver;
                                        g_active_version = "Microsoft.NETCore.App " + ver;
                                    }
                                }
                            }
                            else
                            {
                                if (this_version >= ver_max)
                                {
                                    ver_max = this_version;
                                    _init_config.config_tfm = rx_create_string("net", ver.substr(0, idx2));
                                    _init_config.config_framework = name;
                                    _init_config.config_framework_version = ver;
                                    g_active_version = "Microsoft.NETCore.App " + ver;
                                }
                            }

                            name = ".NET Core ";
                            dotnet_runtime_versions.emplace_back(name + ver);
                        }
                    }
                }
            };
        rc = get_dotnet_info_fptr(nullptr, nullptr, callback, nullptr);
    }
    if (g_active_version.empty())
        return true;


    init_config = _init_config;

    return true;
}


rx_result init_dotnet_host(string_type runtime_config)
{
    if (init_fptr && get_delegate_fptr && close_fptr)
    {

        string_type config_path = "/tmp/rx-platform.runtimeconfig.json";
        {
            rx_source_file file;
            auto ret = file.open_read_write(config_path.c_str());
            if (!ret)
                return ret;

            ret = file.write_string(runtime_config);
            if (!ret)
            {
                return ret;
            }
        }


        int rc = init_fptr(config_path.c_str(), nullptr, &host_cxt);


        rx_file_delete(config_path.c_str());


        if (rc != 0 || host_cxt == nullptr)
        {
            std::ostringstream ss;
            ss << "Init failed: " << std::hex << std::showbase << rc << std::endl;
            close_fptr(host_cxt);
            host_cxt = nullptr;
            return "Unable to initialize .NET runtime:" + ss.str();
        }



        rc = get_delegate_fptr(
            host_cxt,
            hdt_load_assembly,
            (void**)&pload_assembly);


        if (rc != 0 || pload_assembly == nullptr)
        {
            std::ostringstream ss;
            ss << "Get delegate failed: " << std::hex << std::showbase << rc;
            return ss.str();
        }
        // Get the load assembly function pointer
        rc = get_delegate_fptr(
            host_cxt,
            hdt_get_function_pointer,
            (void**)&pget_function_pointer);


        if (rc != 0 || pget_function_pointer == nullptr)
        {
            std::ostringstream ss;
            ss << "Get delegate failed: " << std::hex << std::showbase << rc;
            return ss.str();
        }
        return true;
    }
    return ".NET runtime not loaded.";
}

string_array get_dotnet_runtimes()
{
    return dotnet_runtime_versions;
}
string_type get_hosted_dotnet_version()
{
    return g_active_version;
}
string_type get_managed_dotnet_version()
{
    return g_managed_version;
}
rx_result load_assembly(const string_type& path)
{
    if (!pload_assembly)
    {
        return "Runtime not initialized.";
    }
    else
    {
        // Convert to UTF-16
        int rc = pload_assembly(
            path.c_str(),
            nullptr,
            nullptr);
        if (rc != 0)
        {
            std::ostringstream ss;
            ss << "Load assembly failed: " << std::hex << std::showbase << rc;
            return ss.str();
        }
        return true;
    }
}
rx_result get_function_pointer(const string_type& type_name, const string_type& method_name, const string_type& delegate_name, void** del)
{
    if (!pget_function_pointer)
    {
        return "Runtime not initialized.";
    }
    else
    {
        const char* str_delegate_name = nullptr;


        if (delegate_name.empty())
        {
            str_delegate_name = UNMANAGEDCALLERSONLY_METHOD;
        }
        else
        {
            str_delegate_name = delegate_name.c_str();
        }

        void* fp = nullptr;
        int rc = pget_function_pointer(
            type_name.c_str(),
            method_name.c_str(),
            str_delegate_name,
            nullptr,
            nullptr,
            &fp);

        if (rc != 0 || fp == nullptr)
        {
            std::ostringstream ss;
            ss << "Load delegate failed: " << std::hex << std::showbase << rc;
            return ss.str();
        }
        *del = fp;
        return true;
    }
}



} // namespace dotnet
} // namespace rx_internal

