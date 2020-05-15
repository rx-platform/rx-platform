

/****************************************************************************
*
*  system\runtime\rx_process_context.h
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


#ifndef rx_process_context_h
#define rx_process_context_h 1


#include "rx_runtime_helpers.h"


namespace rx_platform {
namespace runtime {
namespace operational {
class binded_tags;
class connected_tags;

} // namespace operational
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {

namespace algorithms {

enum class runtime_process_step : uint_fast8_t
{
    idle = 0,
    status_change = 1,
    source_inputs = 2,
    mapper_inputs = 3,
    tag_inputs = 4,
    variables = 5,
    programs = 6,
    events = 7,
    filters = 8,
    structs = 9,
    tag_outputs = 10,
    mapper_outputs = 11,
    source_outputs = 12,
    beyond_last = 13
};
template<class T>
struct write_data_struct
{
    T* whose;
    values::rx_simple_value value;
    runtime_transaction_id_t transaction_id;
};
template<class T>
struct update_data_struct
{
    T* whose;
    values::rx_value value;
};

template<class T>
class double_collection
{
    bool first_ = true;
    T first_collection_;
    T second_collection_;
public:
    void emplace_back(typename T::value_type&& elem)
    {
        if (first_)
            first_collection_.emplace_back(std::move(elem));
        else
            second_collection_.emplace_back(std::move(elem));
    }
    T& get_and_swap()
    {
        if (first_)
        {
            first_ = false;
            second_collection_.clear();
            return first_collection_;
        }
        else
        {
            first_ = true;
            first_collection_.clear();
            return second_collection_;
        }
    }
};
typedef std::vector<update_data_struct<structure::mapper_data> > mapper_updates_type;
typedef std::vector<update_data_struct<structure::source_data> > source_updates_type;
typedef std::vector<write_data_struct<structure::mapper_data> > mapper_writes_type;
typedef std::vector<write_data_struct<structure::source_data> > source_writes_type;

typedef std::vector<structure::variable_data*> variables_type;
typedef std::vector<structure::filter_data*> filters_type;
typedef std::vector<structure::event_data*> events_type;
typedef std::vector<structure::struct_data*> structs_type;
typedef std::vector<program_runtime_ptr> programs_type;




class runtime_process_context 
{
    typedef std::function<void()> fire_callback_func_t;

  public:
      runtime_process_context (operational::binded_tags& binded, operational::connected_tags& tags);


      bool should_repeat ();

      void tag_updates_pending ();

      rx_result init_context ();

      void tag_writes_pending ();

      bool should_process_tag_updates ();

      bool should_process_tag_writes ();

      rx_result get_value (runtime_handle_t handle, values::rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, values::rx_simple_value&& val);

      rx_result set_item (const string_type& path, values::rx_simple_value&& what, runtime_init_context& ctx);

      void init_state (structure::hosting_object_data* state, fire_callback_func_t fire_callback);

      void mapper_write_pending (write_data_struct<structure::mapper_data> data);

      mapper_writes_type& get_mapper_writes ();

      structure::hosting_object_data* get_object_state () const;

      void status_change_pending ();

      void mapper_update_pending (update_data_struct<structure::mapper_data> data);

      mapper_updates_type& get_mapper_updates ();

      void source_write_pending (write_data_struct<structure::source_data> data);

      source_writes_type& get_source_writes ();

      void source_update_pending (update_data_struct<structure::source_data> data);

      source_updates_type& get_source_updates ();

      void variable_pending (structure::variable_data* whose);

      variables_type& get_variables_for_process ();

      bool should_process_status_change ();

      void program_pending (program_runtime_ptr whose);

      programs_type& get_programs_for_process ();

      void filter_pending (structure::filter_data* whose);

      filters_type& get_filters_for_process ();

      void variable_value_changed (structure::variable_data* whose, const values::rx_value& val);

      void event_pending (structure::event_data* whose);

      events_type& get_events_for_process ();

      void struct_pending (structure::event_data* whose);

      structs_type& get_structs_for_process ();


      rx_time now;

      template<typename T>
      rx_result set_item_static(const string_type& path, T&& value, runtime_init_context& ctx)
      {
          values::rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = this->set_item(path, std::move(temp), ctx);

          return result;
      }
      template<typename valT>
      valT get_binded_as(runtime_handle_t handle, const valT& default_value)
      {
          values::rx_simple_value temp_val;
          auto result = this->get_value(handle, temp_val);
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
          auto result = this->set_value(handle, std::move(temp_val));
      }
  protected:

  private:


      operational::connected_tags& tags_;

      operational::binded_tags& binded_;


      runtime_process_step current_step_;

      structure::hosting_object_data* state_;

      fire_callback_func_t fire_callback_;

      double_collection<mapper_writes_type> mapper_inputs_;

      double_collection<mapper_updates_type> mapper_outputs_;

      std::bitset<0x10> pending_steps_;

      double_collection<source_updates_type> source_inputs_;

      double_collection<source_writes_type> source_outputs_;

      double_collection<variables_type> variables_;

      double_collection<filters_type> filters_;

      double_collection<programs_type> programs_;

      double_collection<events_type> events_;

      double_collection<structs_type> structs_;

      template<runtime_process_step step>
      void turn_on_pending();
      template<runtime_process_step step>
      bool should_do_step();
};


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform



#endif
