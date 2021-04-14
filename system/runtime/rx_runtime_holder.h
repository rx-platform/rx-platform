

/****************************************************************************
*
*  system\runtime\rx_runtime_holder.h
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


#ifndef rx_runtime_holder_h
#define rx_runtime_holder_h 1


#include "system/meta/rx_meta_support.h"
#include "system/server/rx_platform_item.h"
#include "rx_value_templates.h"

// rx_relations
#include "system/runtime/rx_relations.h"
// rx_operational
#include "system/runtime/rx_operational.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_callback
#include "system/callbacks/rx_callback.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_display_blocks
#include "system/runtime/rx_display_blocks.h"
// rx_runtime_logic
#include "system/runtime/rx_runtime_logic.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_rt_data
#include "lib/rx_rt_data.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_job
#include "lib/rx_job.h"
// rx_tag_blocks
#include "system/runtime/rx_tag_blocks.h"

#include "system/meta/rx_obj_types.h"
namespace rx_internal
{
namespace model
{
template<class typeT>
class types_repository;
namespace algorithms
{
template<class typeT>
class runtime_model_algorithm;
}
} // model
} // rx_internal


namespace rx_platform {

namespace runtime {

namespace algorithms {





template <class typeT>
class process_runtime_job : public rx::jobs::job  
{
    DECLARE_REFERENCE_PTR(process_runtime_job);

  public:
      process_runtime_job (typename typeT::RTypePtr whose);


      void process ();


  protected:

  private:


      typename typeT::RTypePtr whose_;


};







template <class typeT>
class runtime_holder : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(runtime_holder);

    typedef data::runtime_values_data persistent_data_type;

    typedef std::vector<relations::relation_data> relations_type;
    typedef typename typeT::instance_data_t instance_data_t;

    friend class meta::meta_algorithm::object_data_algorithm<typeT>;
    friend class runtime_holder_algorithms<typeT>;
    friend class runtime_scan_algorithms<typeT>;
    friend class rx_internal::model::algorithms::runtime_model_algorithm<typeT>;
    friend class rx_internal::model::types_repository<typeT>;

public:
    typedef typeT DefType;
    typedef typename typeT::RImplPtr ImplPtr;
    typedef typename typeT::RImplType ImplType;

  public:
      runtime_holder (const meta::meta_data& meta, const typename typeT::instance_data_t& instance, typename typeT::runtime_behavior_t&& rt_behavior);

      ~runtime_holder();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      rx_result do_command (rx_object_command_t command_type);

      meta::meta_data& meta_info ();

      platform_item_ptr get_item_ptr () const;

      typename runtime_holder<typeT>::ImplPtr get_implementation ();

      rx_thread_handle_t get_executer () const;

      const typename typeT::runtime_data_t& get_instance_data () const;

      rx_result add_target_relation (relations::relation_data::smart_ptr data);

      rx_result remove_target_relation (const string_type& name);

      typename typeT::instance_data_t get_definition_data ();

      rx_result add_implicit_relation (relations::relation_data::smart_ptr data);


      rx::data::runtime_values_data& get_overrides ()
      {
        return overrides_;
      }


      const ns::rx_directory_resolver& get_directories () const
      {
        return directories_;
      }



      const meta::meta_data& meta_info () const
      {
        return meta_info_;
      }


      typename typeT::runtime_data_t& get_instance_data ()
      {
        return instance_data_;
      }


      constexpr static rx_item_type get_type_id()
      {
          return typeT::RImplType::type_id;
      }
  protected:

  private:


      logic_blocks::logic_holder logic_;

      relations::relations_holder relations_;

      rx::data::runtime_values_data overrides_;

      typename process_runtime_job<typeT>::smart_ptr my_job_ptr_;

      runtime_process_context context_;

      ns::rx_directory_resolver directories_;

      persistent_data_type persistent_;

      display_blocks::displays_holder displays_;

      tag_blocks::tags_holder tags_;


      meta::meta_data meta_info_;

      bool job_pending_;

      locks::slim_lock job_lock_;

      ImplPtr implementation_;

      typename typeT::runtime_data_t instance_data_;

      string_type json_cache_;

      memory::std_buffer binary_cache_;

      owned_value<double, true> last_scan_time_;

      owned_value<double> max_scan_time_;

      owned_value<size_t, true> loop_count_;


};


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform



#endif
