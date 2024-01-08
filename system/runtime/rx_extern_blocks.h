

/****************************************************************************
*
*  system\runtime\rx_extern_blocks.h
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


#ifndef rx_extern_blocks_h
#define rx_extern_blocks_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_platform_api
#include "api/rx_platform_api.h"



namespace rx_platform {

namespace runtime {

namespace blocks {






template <typename implT>
class extern_source_runtime : public source_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Extern Source. Source implementation for externally defined source.");

    DECLARE_REFERENCE_PTR(extern_source_runtime);

    typedef std::vector<rx_timer_ptr> timers_type;

  public:
      extern_source_runtime (implT* impl);

      ~extern_source_runtime();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      rx_result start_source (runtime::runtime_start_context& ctx);

      rx_result stop_source (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_source (runtime::runtime_deinit_context& ctx);

      rx_result update_source (rx_value&& val);

      void result_update_source (rx_result&& result, runtime_transaction_id_t id);

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);


  protected:

  private:

      bool supports_input () const;

      virtual bool supports_output () const;

      rx_result source_write (write_data&& data, runtime_process_context* ctx);



      implT* impl_;


};






template <typename implT>
class extern_mapper_runtime : public mapper_runtime  
{

  public:
      extern_mapper_runtime (implT* impl);

      ~extern_mapper_runtime();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      rx_result start_mapper (runtime::runtime_start_context& ctx);

      rx_result stop_mapper (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_mapper (runtime::runtime_deinit_context& ctx);

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      rx_result mapper_write (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val);

      rx_result mapper_execute (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val);

      void extern_map_current_value () const;

      byte_string extern_get_method_inputs ();

      byte_string extern_get_method_outputs ();


  protected:

  private:

      bool supports_read () const;

      bool supports_write () const;

      void mapped_value_changed (rx_value&& val, runtime_process_context* ctx);

      void mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime_process_context* ctx);

      void mapper_execute_result_received (rx_result&& result, values::rx_simple_value out_data, runtime_transaction_id_t id, runtime_process_context* ctx);



      implT* impl_;


};






class extern_filter_runtime : public filter_runtime  
{

  public:
      extern_filter_runtime (plugin_filter_runtime_struct* impl);

      ~extern_filter_runtime();


      rx_result initialize_filter (runtime::runtime_init_context& ctx);

      rx_result start_filter (runtime::runtime_start_context& ctx);

      rx_result stop_filter (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_filter (runtime::runtime_deinit_context& ctx);

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      rx_result extern_filter_changed ();


  protected:

  private:

      bool supports_input () const;

      bool supports_output () const;

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);



      plugin_filter_runtime_struct* impl_;


};






class extern_struct_runtime : public struct_runtime  
{

  public:
      extern_struct_runtime (plugin_struct_runtime_struct* impl);

      ~extern_struct_runtime();


      rx_result initialize_struct (runtime::runtime_init_context& ctx);

      rx_result start_struct (runtime::runtime_start_context& ctx);

      rx_result stop_struct (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_struct (runtime::runtime_deinit_context& ctx);

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);


  protected:

  private:


      plugin_struct_runtime_struct* impl_;


};






class extern_event_runtime : public event_runtime  
{

  public:
      extern_event_runtime (plugin_event_runtime_struct* impl);

      ~extern_event_runtime();


      rx_result initialize_event (runtime::runtime_init_context& ctx);

      rx_result start_event (runtime::runtime_start_context& ctx);

      rx_result stop_event (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_event (runtime::runtime_deinit_context& ctx);

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      byte_string extern_get_arguments ();

      void extern_event_fired (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_timed_value data);


  protected:

  private:


      plugin_event_runtime_struct* impl_;


};






class extern_variable_runtime : public variable_runtime  
{

  public:
      extern_variable_runtime (plugin_variable_runtime_struct* impl);

      ~extern_variable_runtime();


      rx_result initialize_variable (runtime::runtime_init_context& ctx);

      rx_result start_variable (runtime::runtime_start_context& ctx);

      rx_result stop_variable (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_variable (runtime::runtime_deinit_context& ctx);

      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);


  protected:

  private:


      plugin_variable_runtime_struct* impl_;


};


} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
