

/****************************************************************************
*
*  system\runtime\rx_relations.h
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


#ifndef rx_relations_h
#define rx_relations_h 1


#include "rx_runtime_helpers.h"
#include "rx_objbase.h"
#include "rx_rt_struct.h"
#include "rx_operational.h"

// rx_ptr
#include "lib/rx_ptr.h"
// rx_relation_impl
#include "system/runtime/rx_relation_impl.h"
// rx_resolvers
#include "system/runtime/rx_resolvers.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_values
#include "lib/rx_values.h"

namespace rx_platform {
namespace runtime {
namespace relations {
class relation_connections;
class relation_connector;
} // namespace relations

namespace algorithms {
template <class typeT> class runtime_holder_algorithms;

} // namespace algorithms
} // namespace runtime
} // namespace rx_platform


namespace rx_internal
{
namespace model
{
class relations_type_repository;
}
}


namespace rx_platform {
namespace meta
{
namespace meta_algorithm
{
template<class typeT>
class object_types_algorithm;
}
}

namespace runtime {

namespace relations {





class relation_connector 
{

  public:
      virtual ~relation_connector();


      virtual std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths) = 0;

      virtual rx_result disconnect_items (const std::vector<runtime_handle_t>& items) = 0;

      virtual rx_result write_tag (runtime_transaction_id_t trans, bool test, runtime_handle_t item, rx_simple_value&& value) = 0;

      virtual rx_result write_tag (runtime_transaction_id_t trans, bool test, runtime_handle_t item, data::runtime_values_data&& value) = 0;

      virtual rx_result execute_tag (runtime_transaction_id_t trans, bool test, runtime_handle_t item, values::rx_simple_value&& value) = 0;

      virtual rx_result execute_tag (runtime_transaction_id_t trans, bool test, runtime_handle_t item, data::runtime_values_data value) = 0;

      virtual void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback) = 0;

      virtual void read_value (const string_type& path, read_result_callback_t callback) const = 0;

      virtual void read_struct (string_view_type path, read_struct_data data) const = 0;


  protected:

  private:


};






class relation_value_data 
{

  public:

      rx_value get_value (runtime_process_context* ctx) const;

      rx_result write_value (context_write_data&& data, std::unique_ptr<structure::write_task> task, runtime_process_context* ctx);

      rx_result execute (context_execute_data&& data, std::unique_ptr<structure::execute_task> task, runtime_process_context* ctx);


      rx::values::rx_value value;

      relation_connections *parent;


      runtime_handle_t handle;


  protected:

  private:


};






class relation_connections 
{
    typedef std::map<runtime_transaction_id_t, std::unique_ptr<structure::write_task> > pending_tasks_type;
    typedef std::map<runtime_transaction_id_t, std::unique_ptr<structure::execute_task> > pending_execute_tasks_type;
    // connection handles types
    typedef std::vector<std::unique_ptr<relation_value_data> > values_cache_type;
    typedef std::map<runtime_handle_t, relation_value_data*> handles_type;// target_handle -> value index
    typedef std::map<string_type, relation_value_data*> tags_type;// path -> value index
    typedef std::map<runtime_handle_t, string_type> inverse_tags_type;// mine handle -> path

  public:

      rx_result_with<relations::relation_value_data*> connect_tag (const string_type& path, runtime_handle_t handle);

      rx_result write_tag (runtime_handle_t item, context_write_data&& data, std::unique_ptr<structure::write_task> task, runtime_process_context* ctx);

      rx_result execute_tag (runtime_handle_t item, context_execute_data&& data, std::unique_ptr<structure::execute_task> task, runtime_process_context* ctx);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      void read_value (const string_type& path, read_result_callback_t callback, runtime_process_context* ctx) const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void local_relation_connected (platform_item_ptr item);

      void remote_relation_connected (platform_item_ptr item);

      void relation_disconnected ();

      void items_changed (const std::vector<update_item>& items);

      void execute_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, values::rx_simple_value data);

      void execute_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, data::runtime_values_data data);

      void write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result);


      runtime_process_context* context;


  protected:

  private:


      std::unique_ptr<relation_connector> connector_;

      values_cache_type values_cache_;

      pending_tasks_type pending_tasks_;

      pending_execute_tasks_type pending_execute_tasks_;


      tags_type tag_paths_;

      handles_type handles_map_;


};







class relation_data : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(relation_data);
    friend class meta::meta_algorithm::relation_blocks_algorithm;
    friend class rx_internal::model::relations_type_repository;
    template <class typeT>
    friend class meta::meta_algorithm::object_types_algorithm;
    template <class typeT>
    friend class meta::meta_algorithm::object_data_algorithm;
    enum class relation_state
    {
        idle = 0,
        querying = 1,
        same_domain = 2,
        local_domain = 3,
        remote = 4,
        stopping = 5,
    };
    relation_state my_state_ = relation_state::idle;

    class relation_resolver_user : public resolvers::item_resolver_user
    {
    public:
        relation_data::smart_ptr my_relation;
        bool runtime_connected(platform_item_ptr&& item)
        {
            return my_relation->runtime_connected(std::move(item));
        }
        void runtime_disconnected()
        {
            my_relation->runtime_disconnected();
        }
        rx_reference_ptr get_reference()
        {
            return my_relation;
        }
    };
    relation_resolver_user resolver_user_;
    friend class relation_data::relation_resolver_user;


  public:
      relation_data();


      virtual rx_result initialize_relation (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_relation (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_relation (runtime::runtime_start_context& ctx);

      virtual rx_result stop_relation (runtime::runtime_stop_context& ctx);

      void fill_data (const data::runtime_values_data& data, runtime_process_context* ctx);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      relation_data::smart_ptr make_target_relation (const string_type& path);

      virtual rx_result start_target_relation (runtime::runtime_start_context& ctx);

      virtual rx_result stop_target_relation (runtime::runtime_stop_context& ctx);

      rx_result write_value (write_data&& data, runtime_process_context* ctx, std::unique_ptr<structure::write_task> task);

      platform_item_ptr get_item_ptr () const;

      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      const string_type& get_name () const;

      security::security_guard_ptr get_security_guard ();


      const rx_reference<relation_runtime> get_implementation () const
      {
        return implementation_;
      }



      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }


      const meta_data& meta_info () const
      {
        return meta_info_;
      }



      relation_connections connections;


      string_type name;

      rx_node_id target_base_id;

      structure::value_data value;

      string_type object_directory;

      string_type target_path;

      rx_node_id target_id;

      string_type target_relation_name;

      runtime_handle_t runtime_handle;

      string_type parent_path;

      std::bitset<32> value_opt;

      relation_data(const relation_data&) = delete;
      relation_data& operator=(const relation_data&) = delete;
      relation_data(relation_data&&) noexcept = default;
      relation_data& operator=(relation_data&&) noexcept = default;
  protected:

  private:

      void try_resolve ();

      bool runtime_connected (platform_item_ptr&& item);

      void runtime_disconnected ();

      rx_result resolve_inverse_name ();



      rx_reference<relation_runtime> implementation_;

      resolvers::runtime_item_resolver resolver_;


      rx_thread_handle_t executer_;

      runtime_process_context* context_;

      bool is_target_;

      meta_data meta_info_;


};






class relations_holder 
{
    typedef std::vector<relation_data::smart_ptr> source_relations_type;
    typedef std::vector<relation_data::smart_ptr> target_relations_type;
    typedef std::vector<relation_data::smart_ptr> implicit_relations_type;

    typedef std::map<string_type, std::vector<relation_subscriber*> > relation_subscribers_type;

    template<class typeT>
    friend class meta::meta_algorithm::object_types_algorithm;
    template<class typeT>
    friend class meta::meta_algorithm::object_data_algorithm;
    friend class algorithms::runtime_relation_algorithms;

    class extern_relation_subscriber : public runtime::relation_subscriber
    {
        relation_subscriber_data* data_;
    public:
        extern_relation_subscriber(relation_subscriber_data* data)
            : data_(data)
        {
        }
        extern_relation_subscriber(const extern_relation_subscriber&) = delete;
        extern_relation_subscriber(extern_relation_subscriber&&) noexcept = delete;

        void relation_connected(const string_type& name, const platform_item_ptr& item)
        {
            data_->connected_callback(data_->target, name.c_str(), item->meta_info().id.c_ptr());
        }

        virtual void relation_disconnected(const string_type& name)
        {
            data_->disconnected_callback(data_->target, name.c_str());
        }
    };

    typedef std::vector<std::unique_ptr<extern_relation_subscriber> > extern_subscribers_type;

  public:

      virtual rx_result initialize_relations (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_relations (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_relations (runtime::runtime_start_context& ctx);

      virtual rx_result stop_relations (runtime::runtime_stop_context& ctx);

      void fill_data (const data::runtime_values_data& data, runtime_process_context* ctx);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse (const string_type& prefix, const string_type& filter, std::vector<runtime_item_attribute>& items);

      void browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback);

      relation_data::smart_ptr get_relation (const string_type& name);

      rx_result add_target_relation (relations::relation_data::smart_ptr data, runtime::runtime_start_context& ctx);

      rx_result_with<relation_data::smart_ptr> remove_target_relation (const string_type& name, runtime::runtime_stop_context& ctx);

      rx_result add_implicit_relation (relations::relation_data::smart_ptr data);

      bool is_this_yours (string_view_type path) const;

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);

      rx_result register_relation_subscriber (const string_type& name, relation_subscriber* who);

      rx_result register_extern_relation_subscriber (const string_type& name, relation_subscriber_data* who);

      void read_value (const string_type& path, read_result_callback_t callback, runtime_process_context* ctx) const;

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      void read_struct (string_view_type path, read_struct_data data) const;

      void write_struct (string_view_type path, write_struct_data data);


  protected:

  private:


      source_relations_type source_relations_;

      target_relations_type target_relations_;

      implicit_relations_type implicit_relations_;

      extern_subscribers_type extern_subscribers_;


      relation_subscribers_type relation_subscribers_;


};


} // namespace relations
} // namespace runtime
} // namespace rx_platform



#endif
