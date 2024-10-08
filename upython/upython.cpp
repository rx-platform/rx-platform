

/****************************************************************************
*
*  upython\upython.cpp
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

#ifdef UPYTHON_SUPPORT
#include "system/hosting/rx_host.h"

// upy_internal
#include "upython/upy_internal.h"
// upython
#include "upython/upython.h"

#include "model/rx_meta_internals.h"
#include "upy_method.h"
#include "system/server/rx_file_helpers.h"


namespace rx_platform {

namespace python {

// Class rx_platform::python::upython 

upython::upython()
{
}


upython::~upython()
{
}



rx_result upython::register_logic_handlers ()
{
    auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<method_type>().register_constructor(
        RX_UPYTHON_SCRIPT_METHOD_TYPE_ID, [] {
            return rx_create_reference<upy_script_method>();
        });
    if (!result)
        return result;

    result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<method_type>().register_constructor(
        RX_UPYTHON_MODULE_METHOD_TYPE_ID, [] {
            return rx_create_reference<upy_module_method>();
        });
    if (!result)
        return result;

    return result;
}

rx_result upython::start_script (hosting::rx_platform_host* host, const configuration_data_t& data)
{
    UPYTHON_LOG_INFO("upython", 900, "Starting micropython module.");
    string_type master_path = data.other.upy_path;
    if (!master_path.empty())
    {
        string_array files;
        string_array dirs;
        auto result = rx_list_files(master_path, "*.py", files, dirs);
        if (!result)
        {
            result.register_error("Error listing python files.");
            return result;
        }
        std::vector<std::pair<string_type, string_type> > modules;
        for (const auto& one : files)
        {
            string_type name = one.substr(0, one.size() - 3);// exclude extension from module name
            rx_source_file file;
            string_type path = rx_combine_paths(master_path, one);
            result = file.open(path.c_str());
            if (!result)
            {
                result.register_error("Error opening python file:"s + path);
                return result;
            }
            string_type code;
            result = file.read_string(code);
            if (result)
            {
                UPYTHON_LOG_TRACE("upython", 900, rx_create_string("micropython loaded module ", name, " from file ", path).c_str());
                modules.emplace_back(name, code);
            }
            else
            {
                result.register_error("Error reading python file:"s + path);
                return result;
            }
        }
        master_path = data.other.upy_user;
        if (!master_path.empty())
        {
            files.clear();
            dirs.clear();
            auto result = rx_list_files(master_path, "*.py", files, dirs);
            if (!result)
            {
                result.register_error("Error listing user python files.");
                return result;
            }
            for (const auto& one : files)
            {
                string_type name = one.substr(0, one.size() - 3);// exclude extension from module name
                rx_source_file file;
                string_type path = rx_combine_paths(master_path, one);
                result = file.open(path.c_str());
                if (!result)
                {
                    result.register_error("Error opening user python file:"s + path);
                    return result;
                }
                string_type code;
                result = file.read_string(code);
                if (result)
                {
                    UPYTHON_LOG_TRACE("upython", 900, rx_create_string("micropython loaded module ", name, " from file ", path).c_str());
                    modules.emplace_back(name, code);
                }
                else
                {
                    result.register_error("Error reading user python file:"s + path);
                    return result;
                }
            }
        }
        auto ret = upy_thread::instance().start_script(modules);
        return ret;
    }
    else
    {
        return "No valid python library path.";
    }
}

void upython::stop_script ()
{
    UPYTHON_LOG_INFO("upython", 900, "Stopping micropython module.");
    upy_thread::instance().stop_script();
}


} // namespace python
} // namespace rx_platform

#endif
