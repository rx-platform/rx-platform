

/****************************************************************************
*
*  system\runtime\rx_holder_algorithms.h
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


#ifndef rx_holder_algorithms_h
#define rx_holder_algorithms_h 1



// rx_write_transaction
#include "system/runtime/rx_write_transaction.h"
// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"



namespace rx_platform {

namespace runtime {

namespace algorithms {






template <class typeT>
class runtime_holder_algorithms 
{

  public:

      static std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose);

      static rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose);

      static void fire_job (typename typeT::RType& whose);

      static rx_result write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose);

      static std::vector<rx_result> disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose);

      static void save_runtime (typename typeT::RType& whose);

      static runtime_process_context create_context (typename typeT::RType& whose);

      static runtime_init_context create_init_context (typename typeT::RType& whose);

      static runtime_start_context create_start_context (typename typeT::RType& whose);

      static rx_result get_value_ref (const string_type& path, rt_value_ref& ref, typename typeT::RType& whose);

      static void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback, typename typeT::RType& whose);

      static void fill_data (const data::runtime_values_data& data, typename typeT::RType& whose);

      static void collect_data (data::runtime_values_data& data, runtime_value_type type, const typename typeT::RType& whose);

      static void read_value (const string_type& path, read_result_callback_t callback, const typename typeT::RType& whose);

      static void write_value (const string_type& path, rx_simple_value&& val, write_result_callback_t callback, typename typeT::RType& whose);

      static void read_struct (string_view_type path, read_struct_data data, const typename typeT::RType& whose);

      static void write_struct (string_view_type path, write_struct_data data, typename typeT::RType& whose);

      static rx_result serialize_runtime_value (base_meta_writer& stream, runtime_value_type type, const typename typeT::RType& whose);

      static void execute_method (const string_type& path, data::runtime_values_data data, execute_method_callback_t callback, typename typeT::RType& whose);

      static rx_result execute_item (runtime_transaction_id_t transaction_id, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose);


  protected:

  private:


};






class runtime_relation_algorithms 
{

  public:

      static void notify_relation_connected (const string_type& name, const platform_item_ptr& item, runtime_process_context* ctx);

      static void notify_relation_disconnected (const string_type& name, runtime_process_context* ctx);

      static void relation_value_change (relations::relation_value_data* whose, const rx_value& val, runtime_process_context* ctx);

      static void read_value (const string_type& path, read_result_callback_t callback, const relations::relation_data& whose);

      static void write_value (const string_type& path, rx_simple_value&& val, write_result_callback_t callback, relations::relation_data& whose);

      static void read_struct (string_view_type path, read_struct_data data, const relations::relation_data& whose);

      static void write_struct (string_view_type path, write_struct_data data, relations::relation_data& whose);


  protected:

  private:


};


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform



#endif
