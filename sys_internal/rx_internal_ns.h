

/****************************************************************************
*
*  sys_internal\rx_internal_ns.h
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


#ifndef rx_internal_ns_h
#define rx_internal_ns_h 1



// rx_platform_item
#include "system/server/rx_platform_item.h"

#include "terminal/rx_terminal_style.h"
#include "api/rx_platform_api.h"
#include "system/server/rx_server.h"
#include "lib/rx_serialization_defs.h"
#include "system/meta/rx_types.h"
#include "system/storage_base/rx_storage.h"
#include "system/runtime/rx_runtime_holder.h"
using namespace rx_platform::ns;


namespace rx_internal {

namespace internal_ns {
template<class itemT>
rx_result rx_save_platform_item(itemT& item)
{
    //rx_internal::model::algorithms::do_with_item_storage()
    const auto& meta = item.meta_info();
    auto storage_result = resolve_storage(meta);
    if (storage_result)
    {
        auto item_result = storage_result.value()->get_item_storage(meta, item.get_type_id());
        if (!item_result)
        {
            item_result.register_error("Error saving item "s + meta.path);
            return item_result.errors();
        }
        auto result = item_result.value()->open_for_write();
        if (result)
        {
            result = item.serialize(item_result.value()->write_stream());
            if(result)
                result = item_result.value()->commit_write();
        }
        return result;
    }
    else // !storage_result
    {
        rx_result result(storage_result.errors());
        storage_result.register_error("Error saving item "s + meta.path);
        return result;
    }
}






template <class TImpl>
class rx_item_implementation : public rx_platform::ns::rx_platform_item  
{
    template<class itemT>
    friend rx_result rx_save_platform_item(itemT& item);
    template<class itemT>
    friend itemT rx_clone_platform_item(itemT& item);

  public:
      rx_item_implementation (TImpl impl);

      ~rx_item_implementation();


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      const meta_data& meta_info () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      void read_value (const string_type& path, read_result_callback_t callback) const;

      void write_value (const string_type& path, bool test, rx_simple_value&& val, write_result_callback_t callback);

      rx_result do_command (rx_object_command_t command_type);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor);

      std::vector<rx_result> disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result write_items (runtime_transaction_id_t transaction_id, bool test, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result serialize_value (base_meta_writer& stream, runtime_value_type type) const;

      rx_result deserialize_value (base_meta_reader& stream, runtime_value_type type);

      rx_result save () const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void write_struct (string_view_type path, bool test, write_struct_data data);

      void execute_method (const string_type& path, bool test, data::runtime_values_data data, named_execute_method_callback_t callback);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, values::rx_simple_value& data, runtime::tag_blocks::tags_callback_ptr monitor);

      byte_string get_definition_as_bytes () const;

      security::security_guard_ptr get_security_guard ();


  protected:

  private:


      TImpl impl_;


};






template <class TImpl>
class rx_meta_item_implementation : public rx_platform::ns::rx_platform_item  
{
    template<class itemT>
    friend rx_result rx_save_platform_item(itemT& item);
    template<class itemT>
    friend itemT rx_clone_platform_item(itemT& item);

  public:
      rx_meta_item_implementation (TImpl impl);

      ~rx_meta_item_implementation();


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      const meta_data& meta_info () const;

      void read_value (const string_type& path, read_result_callback_t callback) const;

      void write_value (const string_type& path, bool test, rx_simple_value&& val, write_result_callback_t callback);

      rx_result do_command (rx_object_command_t command_type);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor);

      std::vector<rx_result> disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result write_items (runtime_transaction_id_t transaction_id, bool test, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result serialize_value (base_meta_writer& stream, runtime_value_type type) const;

      rx_result deserialize_value (base_meta_reader& stream, runtime_value_type type);

      rx_result save () const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void write_struct (string_view_type path, bool test, write_struct_data data);

      void execute_method (const string_type& path, bool test, data::runtime_values_data data, named_execute_method_callback_t callback);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, values::rx_simple_value& data, runtime::tag_blocks::tags_callback_ptr monitor);

      byte_string get_definition_as_bytes () const;

      security::security_guard_ptr get_security_guard ();


  protected:

  private:


      TImpl impl_;


};






template <class TImpl>
class rx_other_implementation : public rx_platform::ns::rx_platform_item  
{
    template<class itemT>
    friend rx_result rx_save_platform_item(itemT& item);
    template<class itemT>
    friend itemT rx_clone_platform_item(itemT& item);

  public:
      rx_other_implementation (TImpl impl);

      ~rx_other_implementation();


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      const meta_data& meta_info () const;

      void read_value (const string_type& path, read_result_callback_t callback) const;

      void write_value (const string_type& path, bool test, rx_simple_value&& val, write_result_callback_t callback);

      rx_result do_command (rx_object_command_t command_type);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor);

      std::vector<rx_result> disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result write_items (runtime_transaction_id_t transaction_id, bool test, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result serialize_value (base_meta_writer& stream, runtime_value_type type) const;

      rx_result deserialize_value (base_meta_reader& stream, runtime_value_type type);

      rx_result save () const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void write_struct (string_view_type path, bool test, write_struct_data data);

      void execute_method (const string_type& path, bool test, data::runtime_values_data data, named_execute_method_callback_t callback);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, values::rx_simple_value& data, runtime::tag_blocks::tags_callback_ptr monitor);

      byte_string get_definition_as_bytes () const;

      security::security_guard_ptr get_security_guard ();


  protected:

  private:


      TImpl impl_;


};






template <class TImpl>
class rx_proxy_item_implementation : public rx_platform::ns::rx_platform_item  
{

  public:
      rx_proxy_item_implementation (TImpl impl);

      ~rx_proxy_item_implementation();


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      const meta_data& meta_info () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      void read_value (const string_type& path, read_result_callback_t callback) const;

      void write_value (const string_type& path, bool test, rx_simple_value&& val, write_result_callback_t callback);

      rx_result do_command (rx_object_command_t command_type);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor);

      std::vector<rx_result> disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result write_items (runtime_transaction_id_t transaction_id, bool test, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result serialize_value (base_meta_writer& stream, runtime_value_type type) const;

      rx_result deserialize_value (base_meta_reader& stream, runtime_value_type type);

      rx_result save () const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void write_struct (string_view_type path, bool test, write_struct_data data);

      void execute_method (const string_type& path, bool test, data::runtime_values_data data, named_execute_method_callback_t callback);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, values::rx_simple_value& data, runtime::tag_blocks::tags_callback_ptr monitor);

      byte_string get_definition_as_bytes () const;

      security::security_guard_ptr get_security_guard ();


  protected:

  private:


      TImpl impl_;


};






template <class TImpl>
class rx_relation_item_implementation : public rx_platform::ns::rx_platform_item  
{

  public:
      rx_relation_item_implementation (TImpl impl);

      ~rx_relation_item_implementation();


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      const meta_data& meta_info () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      void read_value (const string_type& path, read_result_callback_t callback) const;

      void write_value (const string_type& path, bool test, rx_simple_value&& val, write_result_callback_t callback);

      rx_result do_command (rx_object_command_t command_type);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor);

      std::vector<rx_result> disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result write_items (runtime_transaction_id_t transaction_id, bool test, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result serialize_value (base_meta_writer& stream, runtime_value_type type) const;

      rx_result deserialize_value (base_meta_reader& stream, runtime_value_type type);

      rx_result save () const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void write_struct (string_view_type path, bool test, write_struct_data data);

      void execute_method (const string_type& path, bool test, data::runtime_values_data data, named_execute_method_callback_t callback);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, data::runtime_values_data& data, runtime::tag_blocks::tags_callback_ptr monitor);

      rx_result execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, values::rx_simple_value& data, runtime::tag_blocks::tags_callback_ptr monitor);

      byte_string get_definition_as_bytes () const;

      security::security_guard_ptr get_security_guard ();


  protected:

  private:


      TImpl impl_;


};


} // namespace internal_ns
} // namespace rx_internal



#endif
