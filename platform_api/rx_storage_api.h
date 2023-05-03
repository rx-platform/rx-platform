

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_storage_api.h
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


#ifndef rx_storage_api_h
#define rx_storage_api_h 1



// rx_ptr
#include "lib/rx_ptr.h"


#include "rx_abi.h"
#include "rx_general.h"
#include "lib/rx_values.h"
#include "lib/rx_lock.h"

using namespace rx;
using namespace rx::values;
namespace rx_platform_api
{

class rx_storage_base;

}


extern "C"
{
    // forward declarations for friend stuff
    rx_result_struct c_init_storage(rx_platform_api::rx_storage_base* self, const char* reference, const char* name);
    rx_result_struct c_deinit_storage(rx_platform_api::rx_storage_base* self);

}


namespace rx_platform_api {





class rx_storage_base : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(rx_storage_base);

  public:
      rx_storage_base();

      ~rx_storage_base();


      virtual rx_result initialize_storage (const string_type& reference, const string_type& name);

      virtual rx_result deinitialize_storage ();


  protected:

  private:

      void bind_runtime (void* target);



      plugin_storage_struct impl_;

      void* target_;

      template<class T>
      friend rx_result register_storage_type(const string_type& prefix);
      friend rx_result_struct(::c_init_storage)(rx_platform_api::rx_storage_base* self, const char* reference, const char* name);
      friend rx_result_struct(::c_deinit_storage)(rx_platform_api::rx_storage_base* self);
};

rx_result register_storage_type(const string_type& prefix, rx_storage_constructor_t construct_func);
template<class T>
rx_result register_storage_type(const string_type& prefix)
{
    auto constr_lambda = []() -> plugin_storage_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_storage_type(prefix, constr_lambda);
}

} // namespace rx_platform_api



#endif
