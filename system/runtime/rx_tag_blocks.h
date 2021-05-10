

/****************************************************************************
*
*  system\runtime\rx_tag_blocks.h
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


#ifndef rx_tag_blocks_h
#define rx_tag_blocks_h 1


#include "rx_relations.h"

// rx_operational
#include "system/runtime/rx_operational.h"
// rx_value_point
#include "runtime_internal/rx_value_point.h"

namespace rx_platform {
namespace runtime {
namespace structure {
class runtime_item;
} // namespace structure

namespace relations {
class relation_data;

} // namespace relations
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {

namespace tag_blocks {





class common_runtime_tags 
{
    template<typename typeT>
    friend class algorithms::runtime_holder_algorithms;
    template<typename typeT>
    friend class algorithms::runtime_scan_algorithms;

  public:

      rx_result initialize_runtime (runtime_init_context& ctx);


  protected:

  private:


      owned_value<double, true> last_scan_time_;

      owned_value<double> max_scan_time_;

      owned_value<size_t, true> loop_count_;

      local_value<bool> on_;

      local_value<bool> test_;

      local_value<bool> blocked_;

      local_value<bool> simulate_;


};







class tags_holder 
{

    typedef std::unique_ptr<std::vector<rx_internal::sys_runtime::data_source::value_point> > points_type;
    template <class typeT>
    friend class algorithms::runtime_scan_algorithms;
    template <class typeT>
    friend class algorithms::runtime_holder_algorithms;

  public:

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      void fill_data (const data::runtime_values_data& data, runtime_process_context* ctx);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx);

      rx_result initialize_runtime (runtime_init_context& ctx, relations::relations_holder* relations);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      void set_runtime_data (structure::runtime_item::smart_ptr&& prototype);

      bool is_this_yours (const string_type& path) const;

      void target_relation_removed (relations::relation_data::smart_ptr what);

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);

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
      void set_binded_as(runtime_handle_t handle, valT&& value, runtime_process_context* ctx)
      {
          values::rx_simple_value temp_val;
          temp_val.assign_static<valT>(std::forward<valT>(value));
          auto result = binded_tags_.set_value(handle, std::move(temp_val), connected_tags_, ctx);
      }
      template<typename valT>
      valT get_local_as(const string_type& path, const valT& default_value)
      {
          return item_->get_local_as<valT>(path, default_value);
      }
  protected:

  private:


      connected_tags connected_tags_;

      std::unique_ptr<structure::runtime_item> item_;

      points_type points_;

      binded_tags binded_tags_;

      common_runtime_tags common_tags_;


};


} // namespace tag_blocks
} // namespace runtime
} // namespace rx_platform



#endif
