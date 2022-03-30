

/****************************************************************************
*
*  system\runtime\rx_extern_items.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_extern_items_h
#define rx_extern_items_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_platform_api
#include "api/rx_platform_api.h"

#include "platform_api/rx_abi.h"


namespace rx_platform {

namespace runtime {

namespace items {





class extern_object_runtime : public object_runtime  
{

  public:
      extern_object_runtime (plugin_object_runtime_struct* impl);

      ~extern_object_runtime();


      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      void fill_code_info (std::ostream& info, const string_type& name);


  protected:

  private:


      plugin_object_runtime_struct* impl_;


};







class extern_port_runtime : public port_runtime  
{

  public:
      extern_port_runtime (plugin_port_runtime_struct* impl);

      ~extern_port_runtime();


      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      void fill_code_info (std::ostream& info, const string_type& name);

      void stack_assembled ();

      void stack_disassembled ();

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      rx_protocol_stack_endpoint* construct_initiator_endpoint ();

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result listen_extern (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result connect_extern (const protocol_address* local_address, const protocol_address* remote_address);

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);

      void post_own_job (plugin_job_struct* what);


  protected:

  private:


      plugin_port_runtime_struct* impl_;


};






class extern_application_runtime : public application_runtime  
{

  public:
      extern_application_runtime (plugin_application_runtime_struct* impl);

      ~extern_application_runtime();


      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);


  protected:

  private:


      plugin_application_runtime_struct* impl_;


};






class extern_domain_runtime : public domain_runtime  
{

  public:
      extern_domain_runtime (plugin_domain_runtime_struct* impl);

      ~extern_domain_runtime();


      void start_timer (runtime_handle_t handle, uint32_t period);

      void suspend_timer (runtime_handle_t handle);

      void destroy_timer (runtime_handle_t handle);

      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);


  protected:

  private:


      plugin_domain_runtime_struct* impl_;


};


} // namespace items
} // namespace runtime
} // namespace rx_platform



#endif
