

/****************************************************************************
*
*  runtime_internal\rx_relations_runtime.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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
// rx_operational
#include "system/runtime/rx_operational.h"
// rx_value_point
#include "runtime_internal/rx_value_point.h"

namespace rx_internal {
namespace sys_runtime {
namespace relations_runtime {
class remote_relation_connector;

} // namespace relations_runtime
} // namespace sys_runtime
} // namespace rx_internal


using namespace rx_platform::runtime;


namespace rx_internal {

namespace sys_runtime {

namespace relations_runtime {






class local_relation_connector : public rx_platform::runtime::relations::relation_connector  
{

    class local_relation_callback : public tag_blocks::rx_tags_callback
    {
        local_relation_connector* parent_;
    public:
        local_relation_callback(local_relation_connector* parent)
            : parent_(parent)
        {
        }
        void items_changed(const std::vector<update_item>& items)
        {
            if (parent_)
                parent_->items_changed(items);
        }
        void transaction_complete(runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items)
        {
            if (parent_)
                parent_->transaction_complete(transaction_id, std::move(result), std::move(items));
        }
        void write_complete(runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result)
        {
            if (parent_)
                parent_->write_complete(transaction_id, item, std::move(result));
        }
        void parent_destroyed()
        {
            parent_ = nullptr;
        }
    };


    typedef std::map<runtime_handle_t, runtime_handle_t> handles_type;// target_handle -> mine handles
    typedef std::map<string_type, runtime_handle_t> tags_type;// path -> mine handle
    typedef std::map<runtime_handle_t, string_type> inverse_tags_type;// mine handle -> path

  public:
      local_relation_connector (platform_item_ptr&& item, relations::relation_connections* whose);

      ~local_relation_connector();


      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths);

      rx_result disconnect_items (const std::vector<runtime_handle_t>& items);

      rx_result write_tag (runtime_transaction_id_t trans, runtime_handle_t item, rx_simple_value&& value);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      void read_value (const string_type& path, read_result_callback_t callback) const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void items_changed (const std::vector<update_item>& items);

      void transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items);

      void write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result);


  protected:

  private:


      platform_item_ptr item_ptr_;

      handles_type handles_;

      inverse_tags_type inverse_tags_;

      tags_type tag_paths_;

      rx_reference<local_relation_callback> monitor_;

      relations::relation_connections* parent_;


};






class relation_value_point : public data_source::value_point_impl  
{

  public:
      relation_value_point (remote_relation_connector* connector, runtime_handle_t handle);


  protected:

  private:

      void value_changed (const rx_value& val);

      void result_received (rx_result&& result, runtime_transaction_id_t id);



      remote_relation_connector *connector_;


      runtime_handle_t handle_;


};






class remote_relation_connector : public rx_platform::runtime::relations::relation_connector  
{
    typedef std::map<runtime_handle_t, std::unique_ptr<relation_value_point> > values_type;

  public:
      remote_relation_connector (platform_item_ptr&& item, relations::relation_connections* whose);

      ~remote_relation_connector();


      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths);

      rx_result disconnect_items (const std::vector<runtime_handle_t>& items);

      rx_result write_tag (runtime_transaction_id_t trans, runtime_handle_t item, rx_simple_value&& value);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      void read_value (const string_type& path, read_result_callback_t callback) const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void value_changed (runtime_handle_t handle, const rx_value& val);

      void result_received (runtime_handle_t handle, rx_result&& result, runtime_transaction_id_t id);


  protected:

  private:


      values_type values_;


      string_type item_path_;

      platform_item_ptr item_ptr_;

      relations::relation_connections* parent_;


};


} // namespace relations_runtime
} // namespace sys_runtime
} // namespace rx_internal



#endif
