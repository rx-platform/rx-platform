

/****************************************************************************
*
*  system\runtime\rx_relation_impl.h
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


#ifndef rx_relation_impl_h
#define rx_relation_impl_h 1


#include "rx_runtime_helpers.h"
#include "lib/rx_rt_data.h"

// rx_ptr
#include "lib/rx_ptr.h"



namespace rx_platform {

namespace runtime {

namespace relations {





class relation_runtime : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(relation_runtime);

    friend class relation_data;

  public:
      relation_runtime();

      relation_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~relation_runtime();


      virtual rx_result initialize_relation (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_relation (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_relation (runtime::runtime_start_context& ctx, bool is_target);

      virtual rx_result stop_relation (runtime::runtime_stop_context& ctx, bool is_target);

      virtual rx_item_reference get_implicit_reference (const meta_data& info);

      virtual relation_runtime::smart_ptr make_target_relation ();


  protected:

  private:

      virtual rx_result_with<platform_item_ptr> resolve_runtime_sync (const rx_node_id& id);

      virtual void relation_connected ();

      virtual void relation_disconnected ();



};






class extern_relation_runtime : public relation_runtime  
{

  public:
      extern_relation_runtime (plugin_relation_runtime_struct* impl);

      ~extern_relation_runtime();


      rx_result initialize_relation (runtime::runtime_init_context& ctx);

      rx_result deinitialize_relation (runtime::runtime_deinit_context& ctx);

      rx_result start_relation (runtime::runtime_start_context& ctx, bool is_target);

      rx_result stop_relation (runtime::runtime_stop_context& ctx, bool is_target);

      rx_item_reference get_implicit_reference (const meta_data& info);

      relation_runtime::smart_ptr make_target_relation ();

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);


      const rx_node_id& get_from () const;

      const rx_node_id& get_to () const;


  protected:

  private:

      rx_result_with<platform_item_ptr> resolve_runtime_sync (const rx_node_id& id);

      void relation_connected ();

      void relation_disconnected ();



      plugin_relation_runtime_struct* impl_;

      rx_node_id from_;

      rx_node_id to_;


};


} // namespace relations
} // namespace runtime
} // namespace rx_platform



#endif
