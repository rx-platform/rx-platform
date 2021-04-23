

/****************************************************************************
*
*  system\runtime\rx_relations.h
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


#ifndef rx_relations_h
#define rx_relations_h 1


#include "rx_runtime_helpers.h"
#include "rx_objbase.h"
#include "rx_rt_struct.h"

// rx_relation_impl
#include "system/runtime/rx_relation_impl.h"
// rx_resolvers
#include "system/runtime/rx_resolvers.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace runtime {
namespace algorithms {
template <class typeT> class runtime_holder_algorithms;
} // namespace algorithms

namespace relations {
class relation_connector;

} // namespace relations
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


      virtual rx_result read_tag (runtime_handle_t item, tags_callback_ptr monitor, runtime_process_context* ctx) = 0;

      virtual rx_result write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, runtime_process_context* ctx) = 0;

      virtual rx_result_with<runtime_handle_t> connect_tag (const string_type& path, tags_callback_ptr monitor, runtime_process_context* ctx) = 0;

      virtual rx_result disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor) = 0;

      virtual rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items) = 0;


  protected:

  private:


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

      rx_result read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);

      rx_result read_tag (runtime_handle_t item, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result_with<runtime_handle_t> connect_tag (const string_type& path, tags_callback_ptr monitor, runtime_process_context* ctx);

      rx_result disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor);

      relation_data::smart_ptr make_target_relation (const string_type& path);

      virtual rx_result start_target_relation (runtime::runtime_start_context& ctx);

      virtual rx_result stop_target_relation (runtime::runtime_stop_context& ctx);


      string_type name;

      rx_node_id target_base_id;

      structure::value_data value;

      string_type object_directory;

      string_type target_path;

      rx_node_id target_id;

      string_type target_relation_name;

      runtime_handle_t runtime_handle;

      string_type parent_path;

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

      std::unique_ptr<relation_connector> connector_;

      resolvers::runtime_item_resolver resolver_;


      rx_thread_handle_t executer_;

      runtime_process_context* context_;

      bool is_target_;


};






class relations_holder 
{
    typedef std::vector<relation_data::smart_ptr> source_relations_type;
    typedef std::vector<relation_data::smart_ptr> target_relations_type;
    typedef std::vector<relation_data::smart_ptr> implicit_relations_type;
    template<class typeT>
    friend class meta::meta_algorithm::object_types_algorithm;
    template<class typeT>
    friend class meta::meta_algorithm::object_data_algorithm;

  public:

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      virtual rx_result initialize_relations (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_relations (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_relations (runtime::runtime_start_context& ctx);

      virtual rx_result stop_relations (runtime::runtime_stop_context& ctx);

      void fill_data (const data::runtime_values_data& data, runtime_process_context* ctx);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);

      relation_data::smart_ptr get_relation (const string_type& name);

      rx_result add_target_relation (relations::relation_data::smart_ptr data, runtime::runtime_start_context& ctx);

      rx_result_with<relation_data::smart_ptr> remove_target_relation (const string_type& name, runtime::runtime_stop_context& ctx);

      rx_result add_implicit_relation (relations::relation_data::smart_ptr data);

      bool is_this_yours (const string_type& path) const;

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);


  protected:

  private:


      source_relations_type source_relations_;

      target_relations_type target_relations_;

      implicit_relations_type implicit_relations_;


};


} // namespace relations
} // namespace runtime
} // namespace rx_platform



#endif
