

/****************************************************************************
*
*  system\runtime\rx_runtime_holder.h
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


#ifndef rx_runtime_holder_h
#define rx_runtime_holder_h 1


#include "system/meta/rx_meta_support.h"

// rx_operational
#include "system/runtime/rx_operational.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_logic
#include "system/logic/rx_logic.h"
// rx_callback
#include "system/callbacks/rx_callback.h"
// rx_rt_data
#include "lib/rx_rt_data.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_job
#include "lib/rx_job.h"

namespace rx_platform {
namespace runtime {
namespace relations {
class relation_runtime;

} // namespace relations
} // namespace runtime
} // namespace rx_platform


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
class object_runtime_algorithms 
{

  public:

      static std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose);

      static void process_runtime (typename typeT::RType& whose);

      static rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose);

      static void fire_job (typename typeT::RType& whose);

      static rx_result write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose);

      rx_result disconnect_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, std::vector<rx_result>& results, bool& has_errors, typename typeT::RType& whose);


  protected:

  private:


};






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

    typedef std::vector<program_runtime_ptr> programs_type;
    typedef std::vector<relation_runtime_ptr> relations_type;
    typedef typename typeT::instance_data_t instance_data_t;

    friend class object_runtime_algorithms<typeT>;
    friend class meta::meta_algorithm::object_types_algorithm<typeT>;
    friend class rx_internal::model::algorithms::runtime_model_algorithm<typeT>;
    friend class rx_internal::model::types_repository<typeT>;
public:
    typedef typeT DefType;
    typedef typename typeT::RImplPtr ImplPtr;
    typedef typename typeT::RImplType ImplType;

  public:
      runtime_holder();

      runtime_holder (const meta::meta_data& meta, const typename typeT::instance_data_t& instance);


      rx_result read_value (const string_type& path, rx_value& value) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx, rx_thread_handle_t whose);

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      rx_result do_command (rx_object_command_t command_type);

      void set_runtime_data (meta::runtime_data_prototype& prototype);

      void fill_data (const data::runtime_values_data& data);

      void collect_data (data::runtime_values_data& data) const;

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);

      bool process_runtime (runtime_process_context& ctx);

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, std::vector<rx_result>& results);

      rx_result write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor, std::vector<rx_result>& results);

      meta::meta_data& meta_info ();

      platform_item_ptr get_item_ptr () const;

      runtime_init_context create_init_context ();

      runtime_start_context create_start_context ();

      typename runtime_holder<typeT>::ImplPtr get_implementation ();

      rx_thread_handle_t get_executer () const;


      rx::data::runtime_values_data& get_overrides ()
      {
        return overrides_;
      }



      const meta::meta_data& meta_info () const
      {
        return meta_info_;
      }


      typename typeT::instance_data_t& get_instance_data ()
      {
        return instance_data_;
      }


      template<typename valT>
      valT get_binded_as(runtime_handle_t handle, const valT& default_value)
      {
          values::rx_simple_value temp_val;
          auto result = binded_tags_.get_value(handle, temp_val);
          if (result)
          {
              return values::extract_value<valT>(temp_val.get_storage(), default_value);
          }
          return default_value;
      }
      template<typename valT>
      void set_binded_as(runtime_handle_t handle, valT&& value)
      {
          values::rx_simple_value temp_val;
          temp_val.assign_static<valT>(std::forward<valT>(value));
          auto result = binded_tags_.set_value(handle, std::move(temp_val), connected_tags_, state_);
      }
      template<typename valT>
      valT get_local_as(const string_type& path, const valT& default_value)
      {
          return item_->get_local_as<valT>(path, default_value);
      }
      constexpr static rx_item_type get_type_id()
      {
          return typeT::RImplType::type_id;
      }
  protected:

  private:


      operational::connected_tags connected_tags_;

      operational::binded_tags binded_tags_;

      programs_type programs_;

      structure::runtime_item::smart_ptr item_;

      relations_type relations_;

      rx::data::runtime_values_data overrides_;

      typename process_runtime_job<typeT>::smart_ptr my_job_ptr_;

      runtime_process_context context_;


      runtime_handle_t scan_time_item_;

      double last_scan_time_;

      double max_scan_time_;

      runtime_handle_t max_scan_time_item_;

      meta::meta_data meta_info_;

      bool job_pending_;

      locks::slim_lock job_lock_;

      ImplPtr implementation_;

      typename typeT::instance_data_t instance_data_;

      structure::hosting_object_data state_;


};


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform



#endif
