

/****************************************************************************
*
*  system\runtime\rx_runtime_logic.h
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


#ifndef rx_runtime_logic_h
#define rx_runtime_logic_h 1


#include "system/server/rx_platform_item.h"
#include "rx_runtime_helpers.h"
#include "rx_rt_struct.h"

// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_logic
#include "system/logic/rx_logic.h"



namespace rx_platform {

namespace runtime {

namespace logic_blocks {





class program_data 
{
public:
    program_data() = default;
    ~program_data() = default;
    program_data(const program_data&) = delete;
    program_data(program_data&&) noexcept = default;
    program_data& operator=(const program_data&) = delete;
    program_data& operator=(program_data&&) noexcept = default;
    operator bool() const
    {
        return program_ptr;
    }

  public:
      program_data (runtime_item_ptr&& rt, program_runtime_ptr&& var, const program_data& prototype);


      void fill_data (const data::runtime_values_data& data);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx);

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result get_local_value (const string_type& path, rx_simple_value& val) const;

      void process_program (runtime::runtime_process_context& ctx);


      program_runtime_ptr program_ptr;

      std::unique_ptr<logic::program_context> context;


      runtime_item_ptr item;

      string_type name;

      string_type full_path;


  protected:

  private:


};






class method_data 
{

    typedef std::unique_ptr<std::map<runtime_transaction_id_t, std::pair<bool, std::unique_ptr<structure::execute_task> > > > pending_tasks_type;
public:
    method_data() = default;
    ~method_data() = default;
    method_data(const method_data&) = delete;
    method_data(method_data&&) noexcept = default;
    method_data& operator=(const method_data&) = delete;
    method_data& operator=(method_data&&) noexcept = default;
    operator bool() const
    {
        return method_ptr;
    }

  public:
      method_data (structure::runtime_item::smart_ptr&& rt, method_runtime_ptr&& var, method_data&& prototype);


      void fill_data (const data::runtime_values_data& data);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx);

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result get_local_value (const string_type& path, rx_simple_value& val) const;

      rx_value get_value (runtime_process_context* ctx) const;

      rx_result execute (context_execute_data&& data, std::unique_ptr<structure::execute_task> task, runtime_process_context* ctx);

      void execution_complete (runtime_transaction_id_t id, rx_result&& result, values::rx_simple_value&& data);

      void process_execute_result (runtime_transaction_id_t id, rx_result&& result, values::rx_simple_value&& data);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      data::runtime_data_model get_method_inputs ();

      data::runtime_data_model get_method_outputs ();


      method_runtime_ptr method_ptr;

      structure::block_data inputs;

      structure::block_data outputs;

      structure::value_data value;


      structure::runtime_item::smart_ptr item;

      string_type name;

      string_type full_path;


  protected:

  private:


      pending_tasks_type pending_tasks_;


      runtime_process_context* context_;


};






class logic_holder 
{
    typedef const_size_vector<program_data> runtime_programs_type;
    typedef const_size_vector<method_data> runtime_methods_type;

    template<class typeT>
    friend class meta::meta_algorithm::object_types_algorithm;
    template<class typeT>
    friend class meta::meta_algorithm::object_data_algorithm;

  public:

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      virtual rx_result initialize_logic (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_logic (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_logic (runtime::runtime_start_context& ctx);

      virtual rx_result stop_logic (runtime::runtime_stop_context& ctx);

      void fill_data (const data::runtime_values_data& data, runtime_process_context* ctx);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx);

      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);

      bool is_this_yours (string_view_type path) const;

      void process_programs (runtime_process_context& ctx);

      rx_result get_value_ref (string_view_type path, rt_value_ref& ref);

      rx_result get_struct_value (string_view_type item, string_view_type path, data::runtime_values_data& data, runtime_value_type type, runtime_process_context* ctx) const;

      void set_methods (std::vector<method_data> data);

      void set_programs (std::vector<program_data> data);


  protected:

  private:


      runtime_programs_type runtime_programs_;

      runtime_methods_type runtime_methods_;


};


} // namespace logic_blocks
} // namespace runtime
} // namespace rx_platform



#endif
