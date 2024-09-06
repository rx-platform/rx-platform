

/****************************************************************************
*
*  runtime_internal\rx_variables.h
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


#ifndef rx_variables_h
#define rx_variables_h 1



// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_blocks
#include "system/runtime/rx_blocks.h"



namespace rx_internal {

namespace sys_runtime {

namespace variables {
rx_result register_variables_constructors();

enum class input_selection_type
{
    first_good = 0,
    first_non_zero = 1,
    max_value = 2,
    max_abs_value = 3,
    min_value = 4,
    min_abs_value = 5
};





template <bool is_obsolite = false>
class simple_variable : public rx_platform::runtime::blocks::variable_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Simple Variable selects first with good quality inputs, writes to all outputs.");

    DECLARE_REFERENCE_PTR(simple_variable);
    struct write_task_data
    {
        int ref_count;
        std::unique_ptr<structure::write_task> task;
    };
    typedef std::unique_ptr<std::map<runtime_transaction_id_t, write_task_data> > pending_tasks_type;

  public:
      simple_variable();

      ~simple_variable();


      rx_result initialize_variable (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_value get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources);

      rx_result variable_write (write_data&& data, std::unique_ptr<structure::write_task> task, runtime_process_context* ctx, runtime_sources_type& sources);

      void process_result (runtime_transaction_id_t id, rx_result&& result);



      std::map<runtime_transaction_id_t, typename simple_variable<is_obsolite>::write_task_data>& get_tasks ();


      pending_tasks_type pending_tasks_;


};






typedef simple_variable< false  > complex_inputs_base;






class complex_inputs_variable : public complex_inputs_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Complex Inputs Variable, writes to all outputs.");

    DECLARE_REFERENCE_PTR(complex_inputs_variable);


  public:
      complex_inputs_variable();

      ~complex_inputs_variable();


      virtual rx_result initialize_variable (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_value get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources);



      input_selection_type inputs_selection_;


};






class complex_outputs_variable : public rx_platform::runtime::blocks::variable_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Complex Outputs Variable selects first good input.");

    DECLARE_REFERENCE_PTR(complex_outputs_variable);
protected:
    struct write_task_data
    {
        int ref_count;
        std::unique_ptr<structure::write_task> task;
    };
    typedef std::unique_ptr<std::map<runtime_transaction_id_t, write_task_data> > pending_tasks_type;

  public:
      complex_outputs_variable();

      ~complex_outputs_variable();


      virtual rx_result initialize_variable (runtime::runtime_init_context& ctx);


  protected:

      std::map<runtime_transaction_id_t, typename complex_outputs_variable::write_task_data>& get_tasks ();


  private:

      rx_value get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources);

      rx_result variable_write (write_data&& data, std::unique_ptr<structure::write_task> task, runtime_process_context* ctx, runtime_sources_type& sources);

      void process_result (runtime_transaction_id_t id, rx_result&& result);



      pending_tasks_type pending_tasks_;


};






class complex_io_variable : public complex_outputs_variable  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Complex IO Variable inputs and outputs are handled.");

    DECLARE_REFERENCE_PTR(complex_io_variable);


  public:
      complex_io_variable();

      ~complex_io_variable();


      virtual rx_result initialize_variable (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_value get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources);



      input_selection_type inputs_selection_;


};






class bridge_variable : public complex_io_variable  
{

  public:
      bridge_variable();

      ~bridge_variable();


      virtual rx_result initialize_variable (runtime::runtime_init_context& ctx);


  protected:

  private:


};






class multiplexer_variable : public rx_platform::runtime::blocks::variable_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Multiplexer Variable selects input and output by selector.");

    DECLARE_REFERENCE_PTR(multiplexer_variable);
    typedef std::unique_ptr<std::map<runtime_transaction_id_t, std::unique_ptr<structure::write_task> > > pending_tasks_type;

  public:
      multiplexer_variable();

      ~multiplexer_variable();


      virtual rx_result initialize_variable (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_value get_variable_input (runtime_process_context* ctx, std::vector<rx_value> sources);

      rx_result variable_write (write_data&& data, std::unique_ptr<structure::write_task> task, runtime_process_context* ctx, runtime_sources_type& sources);

      void process_result (runtime_transaction_id_t id, rx_result&& result);



      std::map<runtime_transaction_id_t, std::unique_ptr<structure::write_task> >& get_tasks ();


      pending_tasks_type pending_tasks_;


      runtime::local_value<int8_t> selector_;


};


} // namespace variables
} // namespace sys_runtime
} // namespace rx_internal



#endif
