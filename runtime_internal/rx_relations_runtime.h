

/****************************************************************************
*
*  runtime_internal\rx_relations_runtime.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_relations_runtime_h
#define rx_relations_runtime_h 1



// rx_relations
#include "system/runtime/rx_relations.h"

namespace rx_internal {
namespace sys_runtime {
namespace subscriptions {
class rx_subscription;

} // namespace subscriptions
} // namespace sys_runtime
} // namespace rx_internal


using namespace rx_platform::runtime;


namespace rx_internal {

namespace sys_runtime {

namespace relations_runtime {





class local_relation_connector : public rx_platform::runtime::relations::relation_connector  
{

  public:
      local_relation_connector (platform_item_ptr&& item);


      rx_result read_tag (runtime_handle_t item, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result_with<runtime_handle_t> connect_tag (const string_type& path, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor);

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);


  protected:

  private:


      platform_item_ptr item_ptr_;


};






class remote_relation_connector : public rx_platform::runtime::relations::relation_connector  
{
    typedef std::map<runtime_handle_t, data_source::value_point> values_type;

  public:

      rx_result read_tag (runtime_handle_t item, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result_with<runtime_handle_t> connect_tag (const string_type& path, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor);

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);


  protected:

  private:


      rx_reference<subscriptions::rx_subscription> my_subscription_;


};


} // namespace relations_runtime
} // namespace sys_runtime
} // namespace rx_internal



#endif
