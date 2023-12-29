

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_storage_api.cpp
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


// rx_storage_api
#include "rx_storage_api.h"



#include "platform_api/rx_abi.h"
#include "lib/rx_values.h"
#include "lib/rx_ptr.h"


typedef rx_platform_api::rx_storage_base _rx_storage_holder_stub;

rxRegisterStorageType_t api_reg_storage_func;


extern "C"
{
    void c_get_code_info(void* whose, const char* name, string_value_struct* info);


    rx_result_struct c_init_storage_stub(void* self, const char* reference);
    rx_result_struct c_deinit_storage_stub(void* self);


    plugin_storage_def_struct _g_storage_def_
    {
        c_get_code_info
        ,c_init_storage_stub
        ,c_deinit_storage_stub
    };


    rx_result_struct c_init_storage(rx_platform_api::rx_storage_base* self, const char* reference, const char* name)
    {
        self->bind_runtime(self->impl_.host);
        return self->initialize_storage(reference, name).move();
    }
    rx_result_struct c_deinit_storage(rx_platform_api::rx_storage_base* self)
    {
        return self->deinitialize_storage().move();
    }

}


namespace rx_platform_api {
rx_result register_storage_type(const string_type& prefix, rx_storage_constructor_t construct_func)
{
    if (api_reg_storage_func)
    {
        auto ret = api_reg_storage_func(get_rx_plugin(), prefix.c_str(), construct_func);
        return ret;
    }
    else
    {
        return RX_NOT_SUPPORTED;
    }
}

// Class rx_platform_api::rx_storage_base 

rx_storage_base::rx_storage_base()
      : target_(nullptr)
{
    impl_.def = &_g_storage_def_;
    bind_as_shared(&impl_.anchor);
}


rx_storage_base::~rx_storage_base()
{
}



rx_result rx_storage_base::initialize_storage (const string_type& reference, const string_type& name)
{
    return true;
}

rx_result rx_storage_base::deinitialize_storage ()
{
    return true;
}

void rx_storage_base::list_storage ()
{
}

void rx_storage_base::bind_runtime (void* target)
{
    target_ = target;
}


} // namespace rx_platform_api

