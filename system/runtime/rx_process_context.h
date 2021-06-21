

/****************************************************************************
*
*  system\runtime\rx_process_context.h
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


#ifndef rx_process_context_h
#define rx_process_context_h 1


#include "rx_runtime_helpers.h"
#include "system/meta/rx_meta_data.h"


namespace rx_platform {
namespace runtime {
namespace tag_blocks {
class connected_tags;
class binded_tags;
} // namespace tag_blocks
} // namespace runtime
} // namespace rx_platform

namespace rx_internal {
namespace sys_runtime {
namespace data_source {
class value_point;

} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal




namespace rx_platform {

namespace runtime {





class context_job 
{

  public:

      virtual void process () = 0;


  protected:

  private:


};






template <typename funcT, typename... Args>
class process_context_job : public context_job  
{

  public:
      process_context_job (funcT&& f, Args&&... args)
          : f_(decay_copy(std::forward<funcT>(f))),
          data_(decay_copy(std::forward<Args>(handle_copy(args)))...)
      {
      }


      void process ()
      {
          std::apply(f_, data_);
      }


  protected:

  private:


      std::decay_t<funcT> f_;

      std::tuple<std::decay_t<Args>...> data_;


};






struct write_data 
{


      bool internal;

      runtime_transaction_id_t transaction_id;

      rx_simple_value value;

      bool test;

      rx_security_handle_t identity;

  public:

  protected:

  private:


};

template<class T>
struct write_data_struct
{
    T* whose;
    write_data data;
};

template<class T>
struct write_result_struct
{
    T* whose;
    rx_result result;
    runtime_transaction_id_t transaction_id;
    write_result_struct() = default;
    write_result_struct(write_result_struct&& right) noexcept
    {
        whose = right.whose;
        result = std::move(right.result);
        transaction_id = right.transaction_id;
    }
    write_result_struct& operator=(write_result_struct&& right) noexcept
    {
        whose = right.whose;
        result = std::move(right.result);
        transaction_id = right.transaction_id;
        return *this;
    }
};





class relation_subscriber 
{

  public:

      virtual void relation_connected (const string_type& name, const platform_item_ptr& item) = 0;

      virtual void relation_disconnected (const string_type& name) = 0;

      virtual ~relation_subscriber() = default;
  protected:

  private:


};


enum class runtime_process_step : uint_fast8_t
{
    idle = 0,
    remote_updates = 1,
    status_change = 2,
    source_inputs = 3,
    mapper_inputs = 4,
    tag_inputs = 5,
    variables = 6,
    programs = 7,
    events = 8,
    filters = 9,
    structs = 10,
    own = 11,
    tag_outputs = 12,
    mapper_outputs = 13,
    source_outputs = 14,
    beyond_last = 15
};

template<class T>
struct update_data_struct
{
    T* whose;
    values::rx_value value;
};

struct remotes_data
{
    runtime_handle_t handle;
    values::rx_simple_value value;
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
    size_t full_size()
    {
        return first_collection_.size() + second_collection_.size();
    }
};

typedef std::vector<jobs::job_ptr> owner_jobs_type;

typedef std::vector<update_data_struct<structure::mapper_data> > mapper_updates_type;
typedef std::vector<update_data_struct<structure::source_data> > source_updates_type;
typedef std::vector<write_data_struct<structure::mapper_data> > mapper_writes_type;
typedef std::vector<write_data_struct<structure::source_data> > source_writes_type;
typedef std::vector<write_result_struct<structure::source_data> > source_results_type;
typedef std::vector<remotes_data> remotes_data_type;

typedef std::vector<structure::variable_data*> variables_type;
typedef std::vector<structure::filter_data*> filters_type;
typedef std::vector<structure::event_data*> events_type;
typedef std::vector<structure::struct_data*> structs_type;
typedef std::vector<program_runtime_ptr> programs_type;
typedef std::vector<method_runtime_ptr> methods_type;





class runtime_process_context 
{
    template<typename typeT>
    friend class algorithms::runtime_holder_algorithms;
    template<typename typeT>
    friend class algorithms::runtime_scan_algorithms;
    friend class algorithms::runtime_relation_algorithms;

    typedef std::function<void()> fire_callback_func_t;
    typedef std::vector<rx_internal::sys_runtime::data_source::value_point>* points_type;
    points_type points_;

  public:
      runtime_process_context (tag_blocks::binded_tags& binded, tag_blocks::connected_tags& tags, const meta::meta_data& info, ns::rx_directory_resolver* dirs, points_type points);


      bool should_repeat ();

      void tag_updates_pending ();

      rx_result init_context ();

      void tag_writes_pending ();

      bool should_process_tag_updates ();

      bool should_process_tag_writes ();

      rx_result get_value (runtime_handle_t handle, values::rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, values::rx_simple_value&& val);

      rx_result set_item (const string_type& path, values::rx_simple_value&& what, runtime_init_context& ctx);

      void init_state (fire_callback_func_t fire_callback);

      void mapper_write_pending (write_data_struct<structure::mapper_data> data);

      mapper_writes_type& get_mapper_writes ();

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

      rx_value adapt_value (const rx_value& from) const;

      rx_value adapt_value (const rx_timed_value& from) const;

      rx_value adapt_value (const rx_simple_value& from) const;

      rx_result do_command (rx_object_command_t command_type);

      void own_pending (jobs::job_ptr what);

      owner_jobs_type& get_for_own_process ();

      runtime_handle_t connect (const string_type& path, uint32_t rate, std::function<void(const rx_value&)> callback, runtime_start_context& ctx);

      void source_result_pending (write_result_struct<structure::source_data> data);

      source_results_type& get_source_results ();

      void runtime_dirty ();

      bool should_save ();

      void from_remote_pending (remotes_data data);

      remotes_data_type& get_from_remote ();


      const rx_mode_type get_mode () const
      {
        return mode_;
      }


      rx_time get_mode_time () const
      {
        return mode_time_;
      }


      ns::rx_directory_resolver* get_directory_resolver () const
      {
        return directory_resolver_;
      }



      rx_time now;

      const meta::meta_data& meta_info;

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
      template<typename valT>
      void set_remote_binded_as(runtime_handle_t handle, valT&& value)
      {
          remotes_data data;
          data.handle = handle;
          data.value.assign_static<valT>(std::forward<valT>(value));
          this->from_remote_pending(std::move(data));
      }
      template<typename funcT, typename... Args>
      void send_own(funcT&& func, Args... args)
      {
          auto job = rx_create_reference<jobs::function_job>(std::forward<funcT>(func), std::forward<Args>(args)...);
          own_pending(job);
      }
  protected:

  private:


      tag_blocks::connected_tags& tags_;

      tag_blocks::binded_tags& binded_;


      runtime_process_step current_step_;

      fire_callback_func_t fire_callback_;

      double_collection<mapper_writes_type> mapper_inputs_;

      double_collection<mapper_updates_type> mapper_outputs_;

      std::bitset<0x10> pending_steps_;

      double_collection<source_updates_type> source_inputs_;

      double_collection<source_writes_type> source_outputs_;

      double_collection<source_results_type> source_results_;

      double_collection<variables_type> variables_;

      double_collection<filters_type> filters_;

      double_collection<programs_type> programs_;

      double_collection<events_type> events_;

      double_collection<structs_type> structs_;

      rx_mode_type mode_;

      rx_time mode_time_;

      double_collection<owner_jobs_type> owns_;

      ns::rx_directory_resolver* directory_resolver_;

      locks::slim_lock context_lock_;

      std::atomic<bool> serialize_value_;

      double_collection<remotes_data_type> from_remote_;

      template<runtime_process_step step>
      void turn_on_pending();
      template<runtime_process_step step>
      bool should_do_step();
};


} // namespace runtime
} // namespace rx_platform



#endif
