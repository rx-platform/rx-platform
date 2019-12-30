

/****************************************************************************
*
*  system\runtime\rx_relations.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_relations_h
#define rx_relations_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace runtime {
namespace operational {
class connected_tags;

} // namespace operational
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {

namespace relations {





class relation_instance_data 
{

  public:

  protected:

  private:


};







class relation_runtime : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(relation_runtime);

	enum relation_state
	{
		relation_state_idle = 0,
		relation_state_querying = 1,
		relation_state_same_domain = 2,
		relation_state_local_domain = 3,
		relation_state_remote = 4,
		relation_state_stopping = 5,
	};
	relation_state my_state_ = relation_state::relation_state_idle;

  public:
      relation_runtime();

      relation_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      ~relation_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void fill_data (const data::runtime_values_data& data);

      void collect_data (data::runtime_values_data& data) const;

      rx_result read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      rx_result connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx);

      meta::meta_data& meta_info ();

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);


      const relation_instance_data& get_instance_data () const
      {
        return instance_data_;
      }



      const meta::meta_data& meta_info () const
      {
        return meta_info_;
      }


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      string_type name;

      static string_type type_name;

      rx_node_id target_base_id;

      rx_node_id target_relation_type;

      string_type target;

      rx_node_id target_id;

      static rx_item_type type_id;

      string_type object_directory;


  protected:

  private:

      void try_resolve ();



      relation_instance_data instance_data_;

      //	This here is unique_ptr just for the memory sake. here
      //	we preserve memory with nullptr values.
      //	I think that about 90% of these will be nullptr and in
      //	the same domain
      std::unique_ptr<operational::connected_tags> connected_tags_;


      meta::meta_data meta_info_;

      platform_item_ptr item_ptr_;

      rx_thread_handle_t executer_;


};


} // namespace relations
} // namespace runtime
} // namespace rx_platform



#endif
